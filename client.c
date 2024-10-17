#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "loginFunct.h"

#define PORT 8001

void display_menu() {
    printf("\n|| Banking Management System ||\n\n");
    printf("Choose Your Role: \n\n");
    printf("1. Customer\n");
    printf("2. Bank Employee\n");
    printf("3. Manager\n");
    printf("4. Administrator\n");
    printf("5. Exit\n\n");
    printf("Choose your role (1-5): ");
}

// void login(int client_socket, int role) {
//     char user_id[50], password[50];
//     char login_buffer[1024];

//     printf("Enter User ID: ");
//     scanf("%s", user_id);
//     printf("Enter Password: ");
//     scanf("%s", password);

//     send(client_socket, &role, sizeof(role), 0);
//     send(client_socket, user_id, sizeof(user_id), 0);
//     send(client_socket, password, sizeof(password), 0);

//     read(client_socket, login_buffer, sizeof(login_buffer));
//     printf("Server: %s\n", login_buffer);
// }

int main() {
    int choice;
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation error");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    while (1) {
        display_menu();
        scanf("%d", &choice);

        if (choice == 5) {
            printf("Disconnecting...\n");
            break; 
        }

        if (choice < 1 || choice > 5) {
            printf("Invalid choice. Please try again.\n");
            continue; 
        }

        login(client_socket, choice);
    }

    close(client_socket);
    return 0;
}
