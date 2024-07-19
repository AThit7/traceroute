# Traceroute

A lightweight tool for mapping the route data packets take to reach a server, highlighting each network hop and its latency. This project was completed as a part of a Computer Networks course.

## Usage

To use the traceroute tool, run the following command with the desired destination IP address:

```sh
$ ./traceroute <ipv4 address>
```

### Example

```sh
$ ./traceroute 8.8.8.8
1. 192.168.1.1 10ms
2. 10.0.0.1 16ms
3. 10.0.0.2 40ms
4. 10.0.0.3 56ms
5. 10.0.0.4 24ms
6. 10.0.0.5 45ms
7. 10.0.0.6 26ms
8. 10.0.0.7 39ms
9. 8.8.8.8 23ms
```

## Installation

Clone the repository and compile the source code:

```sh
$ git clone https://github.com/AThit7/traceroute.git
$ cd traceroute
$ make
```
