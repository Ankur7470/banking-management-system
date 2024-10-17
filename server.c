#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8010
#define MAX_CLIENTS 10

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    int client_id = client_socket;  // Use the socket descriptor as a unique ID for simplicity
    client_id -= 3;
    int choice;
    char buffer[1024];

    printf("New client with ID %d connected.\n", client_id);

    while (1) {
        memset(&choice, 0, sizeof(choice));
        int bytes_received = recv(client_socket, &choice, sizeof(choice), 0);
        
        if (bytes_received <= 0) {
            printf("Client with ID %d disconnected.\n", client_id);
            break; 
        }

        printf("Client with ID %d selected choice %d.\n", client_id, choice);

        snprintf(buffer, sizeof(buffer), "Choice %d received.", choice);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket); 
    return NULL; 
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation error");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue; 
        }

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)&client_socket) != 0) {
            perror("Thread creation failed");
            close(client_socket);
        }

        pthread_detach(client_thread); 
    }

    close(server_socket); 
    return 0;
}
