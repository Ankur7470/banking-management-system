#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>  
#include <sys/types.h> 

#define PORT 8001
#define MAX_CLIENTS 10
#define MAX_USERS 100
#define CREDENTIALS_FILE "credentials.txt"

typedef struct {
    char user_id[50];
    char password[50];
    int role; // 1: Customer, 2: Bank Employee, 3: Manager, 4: Administrator
} User;

// Global array to hold users
User users[MAX_USERS];
int user_count = 0;

// Function to load user credentials from a file using system calls
void load_user_credentials(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open credentials file");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    ssize_t bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading credentials file");
        close(file_desc);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0'; // Null-terminate the buffer

    // Parse the buffer for user credentials
    char *line = strtok(buffer, "\n");
    while (line != NULL && user_count < MAX_USERS) {
        sscanf(line, "%49[^,],%49[^,],%d", users[user_count].user_id,
               users[user_count].password, &users[user_count].role);
        user_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}

int authenticate_user(const char *user_id, const char *password, int *role) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].user_id, user_id) == 0 && strcmp(users[i].password, password) == 0) {
            *role = users[i].role;
            return 1; // Successful authentication
        }
    }
    return 0; // Authentication failed
}

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    int role;
    char user_id[50];
    char password[50];
    char buffer[100];

    printf("A new client connected.\n");
    while (1) {
        int bytes_received = recv(client_socket, &role, sizeof(role), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        recv(client_socket, user_id, sizeof(user_id), 0);
        recv(client_socket, password, sizeof(password), 0);

         if (authenticate_user(user_id, password, &role)) {
            snprintf(buffer, sizeof(buffer), "Hello %s! You have logged in successfully.", user_id);
            printf("%s %s logged in successfully.\n", (role == 1) ? "Customer" :
                     (role == 2) ? "Bank Employee" :
                     (role == 3) ? "Manager" :
                     "Administrator", user_id);
        } else {
            snprintf(buffer, sizeof(buffer), "Login failed! Invalid User ID or Password.");
            printf("%s %s failed to log in.\n",(role == 1) ? "Customer" :
                     (role == 2) ? "Bank Employee" :
                     (role == 3) ? "Manager" :
                     "Administrator", user_id);
        }

        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket); 
    return NULL; 
}

int main() {
    load_user_credentials(CREDENTIALS_FILE); 

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
