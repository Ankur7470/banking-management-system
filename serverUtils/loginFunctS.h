#ifndef LOGIN_SERVER
#define LOGIN_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include "common_struct.h"

#define MAX_USERS 100

void load_cust_credentials(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open credentials file");
        exit(EXIT_FAILURE);
    }

    char buffer[8192];
    int bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading credentials file");
        close(file_desc);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0'; 

    cust_count = 0;

    char *line = strtok(buffer, "\n");
    while (line != NULL && cust_count < MAX_USERS) {
        sscanf(line, "%s %s %lf", customers[cust_count].customer_id,
               customers[cust_count].password, &customers[cust_count].balance);
        cust_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}

void save_cust_credentials(const char *filename) {
    int file_desc = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (file_desc < 0) {
        perror("Could not open customer data file for writing");
        return;
    }

    char buffer[8192];  
    int total_bytes = 0;

    for (int i = 0; i < cust_count; i++) {
        int bytes_written = snprintf(buffer + total_bytes, sizeof(buffer) - total_bytes,
                                          "%s %s %lf\n",
                                          customers[i].customer_id, 
                                          customers[i].password,
                                          customers[i].balance );
        total_bytes += bytes_written;  
    }

    if (write(file_desc, buffer, total_bytes) < 0) {
        perror("Error writing updated content");
    }

    close(file_desc);
}

void load_emp_credentials(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open credentials file");
        exit(EXIT_FAILURE);
    }

    char buffer[8192];
    int bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading credentials file");
        close(file_desc);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';

    emp_count = 0;

    char *line = strtok(buffer, "\n");
    while (line != NULL && emp_count < MAX_USERS) {
        sscanf(line, "%s %s", empl[emp_count].emp_id,
               empl[emp_count].password);
        emp_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}

void load_man_credentials(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open credentials file");
        exit(EXIT_FAILURE);
    }

    char buffer[8192];
    int bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading credentials file");
        close(file_desc);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0'; 

    manager_count = 0;

    char *line = strtok(buffer, "\n");
    while (line != NULL && manager_count < MAX_USERS) {
        sscanf(line, "%s %s", mng[manager_count].man_id,
               mng[manager_count].password);
        manager_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}

void load_admn_credentials(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open credentials file");
        exit(EXIT_FAILURE);
    }

    char buffer[8192];
    int bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading credentials file");
        close(file_desc);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0'; 

    admin_count = 0;

    char *line = strtok(buffer, "\n");
    while (line != NULL && admin_count < MAX_USERS) {
        sscanf(line, "%s %s", admn[admin_count].admn_id,
               admn[admin_count].password);
        admin_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}


int authenticate_user(const char *user_id, const char *password, int role) {
    if(role == 1){
        load_cust_credentials("customers.txt");
        for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, user_id) == 0 && strcmp(customers[i].password, password) == 0) {
            return 1; 
        }
        }
    }
    else if(role == 2){
        load_emp_credentials("employee.txt");
        for (int i = 0; i < emp_count; i++) {
        if (strcmp(empl[i].emp_id, user_id) == 0 && strcmp(empl[i].password, password) == 0) {
            return 1; 
        }
        }
    }
    else if(role == 3){
        load_man_credentials("manager.txt");
        for (int i = 0; i < manager_count; i++) {
        if (strcmp(mng[i].man_id, user_id) == 0 && strcmp(mng[i].password, password) == 0) {
            return 1; 
        }
        }
    }
    else{
    load_admn_credentials("admin.txt");
        for (int i = 0; i < admin_count; i++) {
        if (strcmp(admn[i].admn_id, user_id) == 0 && strcmp(admn[i].password, password) == 0) {
            return 1; 
        }
        }
    }
    
    return 0; 
}


#endif