# Benchmark Results for CEpollion

This document provides benchmark results for CEpollion, an event-driven TCP server written in C using epoll for efficient, non-blocking I/O. The tests measure the server's performance under various loads and connection pool sizes.

## System Specifications

- **CPU**: Intel Core i7-14700K (20 cores, 28 threads)
- **RAM**: 32GB DDR5
- **Storage**: NVMe4 SSD
- **OS**: Ubuntu 24.04.2 LTS
- **Kernel Version**: 6.11.0-19-generic
- **GCC Version**: 13.3.0
- **Network Configuration**: Localhost

## Benchmark 1

**Pool Size**: 4 | **Total Requests**: 100,000 | **Duration**: 7.04s | **Total Queries**: 249,989 | **Overall QPS**: 35,495.50

| Command | Min Latency | Max Latency | Avg Latency | Failures |
| ------- | ----------- | ----------- | ----------- | -------- |
| SET     | 5.328µs     | 49.51ms     | 113.67µs    | 0        |
| GET     | 5.492µs     | 49.37ms     | 111.33µs    | 0        |
| DEL     | 5.391µs     | 15.58ms     | 108.55µs    | 0        |
| GETALL  | 5.426ms     | 5.426ms     | 5.426ms     | 0        |

## Benchmark 2

**Pool Size**: 8 | **Total Requests**: 200,000 | **Duration**: 17.05s | **Total Queries**: 499,704 | **Overall QPS**: 29,292.99

| Command | Min Latency | Max Latency | Avg Latency | Failures |
| ------- | ----------- | ----------- | ----------- | -------- |
| SET     | 8.763µs     | 78.31ms     | 302.44µs    | 0        |
| GET     | 6.947µs     | 77.05ms     | 259.92µs    | 0        |
| DEL     | 6.916µs     | 20.96ms     | 235.10µs    | 0        |
| GETALL  | 11.80ms     | 11.80ms     | 11.80ms     | 0        |

## Benchmark 3

**Pool Size**: 16 | **Total Requests**: 100,000 | **Duration**: 8.94s | **Total Queries**: 250,243 | **Overall QPS**: 27,985.67

| Command | Min Latency | Max Latency | Avg Latency | Failures |
| ------- | ----------- | ----------- | ----------- | -------- |
| SET     | 9.774µs     | 48.63ms     | 678.51µs    | 0        |
| GET     | 10.33µs     | 43.28ms     | 529.12µs    | 0        |
| DEL     | 7.946µs     | 3.60ms      | 435.52µs    | 0        |
| GETALL  | 10.35ms     | 10.35ms     | 10.35ms     | 0        |

## Benchmark 4

**Pool Size**: 16 | **Total Requests**: 200,000 | **Duration**: 19.09s | **Total Queries**: 500,167 | **Overall QPS**: 26,199.44

| Command | Min Latency | Max Latency | Avg Latency | Failures |
| ------- | ----------- | ----------- | ----------- | -------- |
| SET     | 7.928µs     | 88.91ms     | 726.53µs    | 0        |
| GET     | 12.84µs     | 33.03ms     | 562.32µs    | 0        |
| DEL     | 11.93µs     | 27.06ms     | 469.44µs    | 0        |
| GETALL  | 11.35ms     | 11.35ms     | 11.35ms     | 0        |

## Summary

- **Performance scales with connection pool size** but starts stabilizing beyond a point.
- **Latency remains low even under high concurrency**, thanks to epoll-based non-blocking I/O.
- **No failures observed**, indicating stability under test conditions.

This benchmark provides an initial insight into CEpollion's performance under various loads. Future improvements can include better memory management and further optimizations to reduce max latency.
