#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>  
#include <sys/types.h> 
#include "serverUtils/loginFunctS.h"
#include "serverUtils/managerFunctS.h"
#include "serverUtils/adminFunctS.h"
#include "serverUtils/employeeFunctS.h"
#include "serverUtils/customerFunctS.h"

#define PORT 8032
#define MAX_CLIENTS 10

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    int role;
    char user_id[50];
    char password[50];
    int flag = 0;

    printf("A new client connected.\n");
    while (1) {
        int bytes_received = recv(client_socket, &role, sizeof(role), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        recv(client_socket, user_id, sizeof(user_id), 0);
        recv(client_socket, password, sizeof(password), 0);

         if (authenticate_user(user_id, password, role)) {
            flag = 1;
            printf("%s %s logged in successfully.\n", (role == 1) ? "Customer" :
                     (role == 2) ? "Bank Employee" :
                     (role == 3) ? "Manager" :
                     "Administrator", user_id);
        } else {
            printf("%s %s failed to log in.\n",(role == 1) ? "Customer" :
                     (role == 2) ? "Bank Employee" :
                     (role == 3) ? "Manager" :
                     "Administrator", user_id);
        }

        send(client_socket, &flag, sizeof(flag), 0);
        if(flag == 1){
            switch(role){
                case 1:
                    customer(client_socket, user_id, role);
                    break;
                case 2:
                    employee(client_socket, user_id, role);
                    break;
                case 3:
                    manager(client_socket, user_id, role);
                    break;
                case 4:
                    admin();
                    break;
            }
        }
        else{
            continue;
        }
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
