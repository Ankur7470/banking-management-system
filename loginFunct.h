#ifndef TEMP_LOGIN
#define TEMP_LOGIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//used by client.c
void login(int client_socket, int role) {
    char user_id[50], password[50];
    char login_buffer[100];

    printf("Enter User ID: ");
    scanf("%s", user_id);
    printf("Enter Password: ");
    scanf("%s", password);

    send(client_socket, &role, sizeof(role), 0);
    send(client_socket, user_id, sizeof(user_id), 0);
    send(client_socket, password, sizeof(password), 0);

    read(client_socket, login_buffer, sizeof(login_buffer));
    printf("Server: %s\n", login_buffer);
}

//used by server.c



#endif