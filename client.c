#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "clientUtils/loginFunct.h"
#include "clientUtils/customerFunct.h"
#include "clientUtils/employeeFunct.h"
#include "clientUtils/managerFunct.h"
#include "clientUtils/adminFunct.h"

#define PORT 8032

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

        if(login(client_socket, choice)){
            switch(choice){
                case 1:
                    customer(client_socket);
                    break;
                case 2:
                    employee(client_socket);
                    break;
                case 3:
                    manager(client_socket);
                    break;
                case 4:
                    admin();
                    break;
            }
        }
        else{
            printf("Login failed! Invalid User ID or Password.");
        }
    }

    close(client_socket);
    return 0;
}
