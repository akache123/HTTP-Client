**HTTP-Client**

---

### README File Description  

# HTTP Client (C Program)

This is a simple C-based HTTP client that connects to a specified server, sends an HTTP GET request, and receives the response. The program is designed to be compiled and run from the command line.

---

## Build and Run

- To build the client program, run:  
  `make`
- The executable will be placed in the 'bin' directory.

---

## Usage

1. Compile the program using the provided Makefile:  
   `make`
   
2. Execute the compiled program from the 'bin' directory:  
   `./bin/client <hostname> <ip:port/path>`

   - Replace `<hostname>` with the desired hostname.  
   - Replace `<ip:port/path>` with the server’s IP address (IPv4), port, and optional path.

---

## Documentation

### Internal Design

- **Socket Setup**: Opens a socket using `socket()` and sets up the server address structure, then connects to the server using `connect()`.
- **Request Construction**: Parses command-line arguments to extract hostname, IP, port, and path, then constructs an HTTP GET request dynamically.
- **Request Sending**: Sends the constructed request to the server using `send()`.
- **Response Handling**: Dynamically receives the HTTP response from the server and writes the response body to an output file.

---

## Test Cases

- **Valid Request**  
  Command: `./bin/client example.com 192.168.1.1:80/test`  
  Expected: Successfully connects, sends request, and receives response.

- **Invalid IP Address**  
  Command: `./bin/client example.com invalid_ip:80/test`  
  Expected: Error message indicating an invalid IPv4 address.

- **Invalid Port**  
  Command: `./bin/client example.com 192.168.1.1:8080/test`  
  Expected: Error message indicating an invalid port.

- **Empty Path**  
  Command: `./bin/client example.com 192.168.1.1:80`  
  Expected: Successfully connects with an empty path.

- **Invalid Hostname**  
  Command: `./bin/client "" 192.168.1.1:80/test`  
  Expected: Error message indicating an invalid hostname.

---

## Shortcomings

- Only supports port 80 in this example.
- Limited error handling for various scenarios.
- Does not support HTTPS connections.
- For large files, it may take time to write the output to the `outpu.dat` file.

---

## Sources

- HTTP requests: [RFC 2616 Section 5](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html)
- HTTP responses: [RFC 2616 Section 6](https://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html)

---
