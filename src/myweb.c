#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1000000

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int is_valid_ipv4(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

int is_valid_port(const char *port) {
    if (port[0] == '\0') {
        // Empty port, use default (80)
        return 1;
    }

    int port_number = atoi(port);
    return port_number == 80;
}

void send_request(int sockfd, const char *request) {
    if (send(sockfd, request, strlen(request), 0) < 0) {
        error("Error sending request");
    }
}

void receive_response_dynamic(int sockfd, FILE *output_file) {
    size_t buffer_size = BUFFER_SIZE;
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        error("Error allocating memory for buffer");
    }

    ssize_t bytes_received;
    int body_started = 0;
    size_t total_received = 0;

    while ((bytes_received = recv(sockfd, buffer, buffer_size, 0)) > 0) {
        if (!body_started) {
            char *body_start = strstr(buffer, "\r\n\r\n");
            if (body_start != NULL) {
                fwrite(body_start + 4, 1, bytes_received - (body_start - buffer) - 4, output_file);
                body_started = 1;
                total_received += bytes_received - (body_start - buffer) - 4;
            }
        } else {
            fwrite(buffer, 1, bytes_received, output_file);
            total_received += bytes_received;
        }

        if (bytes_received == buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                error("Error reallocating memory for buffer");
            }
        }
    }

    if (bytes_received < 0) {
        error("Error receiving response");
    } else if (bytes_received == 0) {
        // Connection closed by the server
    }

    free(buffer);
}

void receive_response_header(int sockfd) {
    size_t buffer_size = BUFFER_SIZE;
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        error("Error allocating memory for buffer");
    }

    ssize_t bytes_received;
    size_t total_received = 0;

    while ((bytes_received = recv(sockfd, buffer, buffer_size, 0)) > 0) {
        char *headers_end = strstr(buffer, "\r\n\r\n");
        if (headers_end != NULL) {
            fwrite(buffer, 1, headers_end - buffer + 4, stdout);
            total_received += headers_end - buffer + 4;
            break;
        }

        if (bytes_received == buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                error("Error reallocating memory for buffer");
            }
        }
    }

    if (bytes_received < 0) {
        error("Error receiving response");
    }

    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Please enter the full command in this format: %s <hostname> <ip:port/path> [-h]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    const char *ip_port_path = argv[2];

    char ip[BUFFER_SIZE], port[BUFFER_SIZE], path[BUFFER_SIZE];

    if (sscanf(ip_port_path, "%[^:]:%[^/]/%s", ip, port, path) != 3 &&
        sscanf(ip_port_path, "%[^:]:%[^/]", ip, port) != 2) {
        strcpy(port, "80");
        sscanf(ip_port_path, "%[^/]/%s", ip, path);
    }

    if (strlen(hostname) == 0) {
        fprintf(stderr, "Error: Invalid hostname.\n");
        exit(EXIT_FAILURE);
    }

    if (!is_valid_port(port)) {
        fprintf(stderr, "Error: Invalid port number. Only port 80 is supported in this example.\n");
        exit(EXIT_FAILURE);
    }

    if (!is_valid_ipv4(ip)) {
        fprintf(stderr, "Error: Invalid IPv4 address.\n");
        exit(EXIT_FAILURE);
    }

    char *request;
    size_t request_size = snprintf(NULL, 0, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname) + 1;

    request = (char *)malloc(request_size);
    if (request == NULL) {
        error("Error allocating memory for request");
    }

    snprintf(request, request_size, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        error("Error converting IP address");
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    if (argc == 4 && strcmp(argv[3], "-h") == 0) {
        send_request(sockfd, request);
        receive_response_header(sockfd);
    } else {
        FILE *output_file = fopen("output.dat", "wb");
        if (output_file == NULL) {
            error("Error opening output file");
        }
        send_request(sockfd, request);
        receive_response_dynamic(sockfd, output_file);
        fclose(output_file);
    }

    close(sockfd);
    free(request);

    return 0;
}
