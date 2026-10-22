#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void send_file(SOCKET client_fd, const char *filepath, const char *content_type) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        char *err = "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
        send(client_fd, err, strlen(err), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    char header[256];
    sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", content_type, size);
    send(client_fd, header, strlen(header), 0);
    send(client_fd, data, size, 0);

    free(data);
}
int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("startup failed\n");
        return 1;
    }
    printf("step 1: winsock startup\n");

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET){
        printf("socket creation failed\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
        printf("step 2: socket bound to port 8000\n");

    if (listen(server_fd, 5) == SOCKET_ERROR) {
        printf("listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("step 3: server is run on http://localhost:8000\n");
while (1) {
        SOCKET client_fd = accept(server_fd, NULL, NULL);

        char buffer[4096] = {0};
        recv(client_fd, buffer, 4095, 0);

        char method[16] = {0};
        char path[256] = {0};
        sscanf(buffer, "%s %s", method, path);
        printf("Method: %s | Path: %s\n", method, path);

        
        if (strcmp(method, "GET") == 0 && (strcmp(path, "/") == 0 || strcmp(path, "/register") == 0)) {
            send_file(client_fd, "public/index.html", "text/html");
        }
        
        else if (strcmp(method, "GET") == 0 && strcmp(path, "/index.css") == 0) {
            send_file(client_fd, "public/index.css", "text/css");
        }
       
        else if (strcmp(method, "GET") == 0 && strcmp(path, "/index.js") == 0) {
            send_file(client_fd, "public/index.js", "application/javascript");
        }
      
        else if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body != NULL && strstr(body, "user") != NULL && strstr(body, "pass") != NULL) {
                char *ok = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nSet-Cookie: session_user=admin; Path=/; HttpOnly\r\n\r\n200 OK - Login Success!";
                send(client_fd, ok, strlen(ok), 0);
            } else {
                char *bad = "HTTP/1.1 400 Bad Request\r\n\r\nMissing user or pass";
                send(client_fd, bad, strlen(bad), 0);
            }
        }

        closesocket(client_fd);
    }
    closesocket(server_fd);
    WSACleanup();
    return 0;
}