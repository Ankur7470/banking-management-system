#ifndef TEMP_LOGIN_CLIENT
#define TEMP_LOGIN_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>  

int login(int client_socket, int role) {
    char user_id[50], password[50];
    int login_res;

    printf("Enter User ID: ");
    scanf("%s", user_id);
    printf("Enter Password: ");
    scanf("%s", password);

    send(client_socket, &role, sizeof(role), 0);
    send(client_socket, user_id, sizeof(user_id), 0);
    send(client_socket, password, sizeof(password), 0);

    read(client_socket, &login_res, sizeof(login_res));

    return login_res;
}

#endif