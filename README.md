# CEpollion

🚀 **CEpollion** is a lightweight, high-performance TCP server built using the **epoll** event loop mechanism in C. It efficiently handles thousands of concurrent client connections and demonstrates how event-driven architectures work under the hood.

This project serves as a learning tool to understand the **epoll** event loop mechanism, **non-blocking I/O**, and **connection pooling**. While it is functional, it is a **raw and naive implementation** and is **not optimized** for serious **production-grade** load handling.

## Features

- **Single-threaded event loop** using epoll
- **Non-blocking I/O** for handling multiple clients efficiently
- **Basic command processing** (SET, GET, DEL, GETALL)
- **Connection pooling in the client** for efficient communication
- **Logging support** with timestamps and execution time measurement

## Architecture Overview

- **Server Implementation (C with epoll)**:

  - Uses **epoll** for efficient event-driven networking.
  - Manages multiple client connections in a scalable manner.
  - Handles basic command parsing and execution.

- **Client Implementation (Go)**:
  - Implements a **connection pool** for efficient resource utilization.
  - Sends commands to the server and measures execution time.
  - Supports parallel request execution using goroutines.

## Getting Started

### Prerequisites

Ensure you have the following installed:

- **GCC** (for compiling the C server)
- **Go 1.20+** (for running the Go client)
- **Linux/macOS** (for epoll support, Windows not supported)

### Building and Running the Server

```sh
# Clone the repository
git clone https://github.com/akgarg0472/CEpollion
cd CEpollion

# Compile the server
make

# Run the server
./out/cepollion
```

### Running the Go Client

```sh
# Navigate to the client directory
cd client

# Initialize Go module (only if not already done)
go mod init client

# Download dependencies
go mod tidy

# Run the Go client
go run client.go --host=127.0.0.1 --port=2318 --poolSize=4 --numRequests=100000
```

## Usage Example

The Go client interacts with the server using basic commands:

```sh
SET key1 value1

GET key1

DEL key1

GETALL
```

## Performance Testing

The client pool can be adjusted using CLI flags to test performance with concurrent requests:

```sh
# Example usage with custom values
go run client.go --host=192.168.1.100 --port=4000 --poolSize=10 --numRequests=50000
```

### Available CLI Flags:

- `--host` : Server IP address (default: `127.0.0.1`)
- `--port` : Server port (default: `2318`)
- `--poolSize` : Number of TCP connections in the pool (default: `4`)
- `--numRequests` : Number of requests to send (default: `100000`)

The system logs operation time and tracks overall server statistics.

## Contributions & Improvements

This project is a **work in progress**, and contributions are welcome! Future improvements could include:

- **Optimized memory management**
- **Better error handling**
- **More efficient threading model**
- **Advanced command support**

Feel free to submit issues or pull requests. 🚀
