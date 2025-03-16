package main

import (
	"bufio"
	"errors"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"net"
	"strings"
	"sync"
	"time"

	"github.com/schollz/progressbar/v3"
)

type Connection struct {
	conn   net.Conn
	reader *bufio.Reader
	mu     sync.Mutex
}

type TCPClientPool struct {
	host         string
	port         int
	poolSize     int
	connections  []*Connection
	poolLock     sync.Mutex
	currentIndex int
}

func NewTCPClientPool(host string, port, poolSize int) (*TCPClientPool, error) {
	pool := &TCPClientPool{
		host:     host,
		port:     port,
		poolSize: poolSize,
	}

	for i := 0; i < poolSize; i++ {
		conn, err := pool.createConnection()
		if err != nil {
			return nil, fmt.Errorf("failed to create initial connection: %v", err)
		}
		pool.connections = append(pool.connections, conn)
	}

	return pool, nil
}

func (pool *TCPClientPool) createConnection() (*Connection, error) {
	address := net.JoinHostPort(pool.host, fmt.Sprintf("%d", pool.port))
	conn, err := net.Dial("tcp", address)
	if err != nil {
		return nil, fmt.Errorf("failed to connect: %v", err)
	}
	return &Connection{
		conn:   conn,
		reader: bufio.NewReader(conn),
	}, nil
}

func (pool *TCPClientPool) getConnection() *Connection {
	pool.poolLock.Lock()
	conn := pool.connections[pool.currentIndex]
	pool.currentIndex = (pool.currentIndex + 1) % pool.poolSize
	pool.poolLock.Unlock()
	return conn
}

func (pool *TCPClientPool) sendCommand(command string, stats *OperationStats) (string, error) {
	conn := pool.getConnection()
	conn.mu.Lock()
	defer conn.mu.Unlock()

	start := time.Now()
	_, err := conn.conn.Write([]byte(command + "\n"))
	if err != nil {
		stats.addFailure()
		return "", fmt.Errorf("failed to send command: %v", err)
	}
	writeTime := time.Since(start)

	start = time.Now()
	response, err := conn.reader.ReadString('\n')
	if err != nil {
		stats.addFailure()
		return "", fmt.Errorf("failed to read response: %v", err)
	}
	readTime := time.Since(start)

	stats.addSuccess(writeTime + readTime)
	return strings.TrimSpace(response), nil
}

func (pool *TCPClientPool) Set(key, value string, stats *OperationStats) (string, error) {
	if key == "" || value == "" {
		stats.addFailure()
		return "", errors.New("key and value must not be empty")
	}
	return pool.sendCommand(fmt.Sprintf("SET %s %s", key, value), stats)
}

func (pool *TCPClientPool) Get(key string, stats *OperationStats) (string, error) {
	if key == "" {
		stats.addFailure()
		return "", errors.New("key must not be empty")
	}
	return pool.sendCommand(fmt.Sprintf("GET %s", key), stats)
}

func (pool *TCPClientPool) Delete(key string, stats *OperationStats) (string, error) {
	if key == "" {
		stats.addFailure()
		return "", errors.New("key must not be empty")
	}
	return pool.sendCommand(fmt.Sprintf("DEL %s", key), stats)
}

func (pool *TCPClientPool) GetAll(stats *OperationStats) (string, error) {
	return pool.sendCommand("GETALL", stats)
}

func (pool *TCPClientPool) Close() {
	for _, conn := range pool.connections {
		conn.conn.Close()
	}
}

type OperationStats struct {
	mu       sync.Mutex
	times    []time.Duration
	failures int
	totalOps int
}

func (s *OperationStats) addSuccess(duration time.Duration) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.times = append(s.times, duration)
	s.totalOps++
}

func (s *OperationStats) addFailure() {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.failures++
	s.totalOps++
}

func (s *OperationStats) minMaxAvg() (min, max, avg time.Duration) {
	if len(s.times) == 0 {
		return 0, 0, 0
	}
	min, max, sum := s.times[0], s.times[0], time.Duration(0)
	for _, t := range s.times {
		if t < min {
			min = t
		}
		if t > max {
			max = t
		}
		sum += t
	}
	avg = sum / time.Duration(len(s.times))
	return min, max, avg
}

func main() {
	ip := flag.String("host", "127.0.0.1", "Server IP address")
	port := flag.Int("port", 2318, "Server port")
	poolSize := flag.Int("poolSize", 4, "Number of TCP connections in the pool")
	numRequests := flag.Int("numRequests", 100000, "Number of requests to send")

	flag.Parse()

	fmt.Println("\n🚀 Starting Load Test...")
	fmt.Printf("📌 Target Server: %s:%d\n", *ip, *port)
	fmt.Printf("🛠 Pool Size: %d | 🔄 Total Requests: %d\n", *poolSize, *numRequests)
	fmt.Println("------------------------------------------------")

	clientPool, err := NewTCPClientPool(*ip, *port, *poolSize)
	if err != nil {
		log.Fatalf("Error creating connection pool: %v", err)
	}
	defer clientPool.Close()

	var wg sync.WaitGroup
	startTime := time.Now()

	setStats, getStats, delStats, getAllStats := &OperationStats{}, &OperationStats{}, &OperationStats{}, &OperationStats{}

	bar := progressbar.Default(int64(*numRequests))

	for i := range *numRequests {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			_, _ = clientPool.Set(fmt.Sprintf("key%d", i), fmt.Sprintf("value%d", i), setStats)
			_, _ = clientPool.Get(fmt.Sprintf("key%d", i), getStats)
			if rand.Intn(2) == 1 {
				_, _ = clientPool.Delete(fmt.Sprintf("key%d", i), delStats)
			}
			bar.Add(1)
		}(i)
	}

	wg.Wait()

	_, _ = clientPool.GetAll(getAllStats)

	elapsedTime := time.Since(startTime)

	fmt.Println("\n----- Benchmark Results -----")
	fmt.Printf("Total TCP Connections: %d\n", *poolSize)
	fmt.Printf("Total Requests Made: %d\n", *numRequests)
	fmt.Printf("Total Duration: %v\n", elapsedTime)

	fmt.Println()

	printStats("SET", setStats)
	printStats("GET", getStats)
	printStats("DEL", delStats)
	printStats("GETALL", getAllStats)

	totalQueries := setStats.totalOps + getStats.totalOps + delStats.totalOps + getAllStats.totalOps
	totalFailures := setStats.failures + getStats.failures + delStats.failures + getAllStats.failures
	overallQPS := float64(totalQueries) / elapsedTime.Seconds()
	successRate := float64(totalQueries-totalFailures) / float64(totalQueries) * 100

	fmt.Println("\n----- Cumulative Metrics -----")
	fmt.Printf("Total Queries Processed: %d\n", totalQueries)
	fmt.Printf("Total Failures: %d\n", totalFailures)
	fmt.Printf("Overall QPS: %.2f\n", overallQPS)
	fmt.Printf("Success Rate: %.2f%%\n", successRate)
	fmt.Println("-----------------------------")
}

func printStats(operation string, stats *OperationStats) {
	min, max, avg := stats.minMaxAvg()
	fmt.Printf("%s -> Min: %v | Max: %v | Avg: %v | Failures: %d\n",
		operation, min, max, avg, stats.failures)
}
