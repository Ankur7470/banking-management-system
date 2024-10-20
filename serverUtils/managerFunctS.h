#ifndef MANAGER_SERVER
#define MANAGER_SERVER

#include <stdio.h>
#include "common_struct.h"

#define MAN_DATA_FILE "manager.txt"

void act_deact_cust_acc(int client_socket, const char *man_id){
    
}

void assign_loan_application(){

}

void review_cust_feedback(){

}

void load_manager(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open manager data file");
        return;
    }

    char buffer[8192];  
    ssize_t bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading file");
        close(file_desc);
        return;
    }
    buffer[bytes_read] = '\0';

    manager_count = 0; 

    char *line = strtok(buffer, "\n");
    while (line != NULL && manager_count < 1) {
        sscanf(line, "%s %s", 
               mng[manager_count].man_id, 
               mng[manager_count].password);
        printf("Loaded Employee: %s, Password: %s\n", mng[manager_count].man_id, mng[manager_count].password);  // Debugging print
        manager_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}

void save_manager(const char *filename) {
    int file_desc = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (file_desc < 0) {
        perror("Could not open Manager data file for writing");
        return;
    }

    char buffer[8192];  
    ssize_t total_bytes = 0;

    for (int i = 0; i < 1; i++) {
        ssize_t bytes_written = snprintf(buffer + total_bytes, sizeof(buffer) - total_bytes,
                                         "%s %s\n",
                                         mng[i].man_id, 
                                         mng[i].password);
        total_bytes += bytes_written;  

        printf("Saving Manager: %s, Password: %s\n", mng[i].man_id, mng[i].password);

        if (total_bytes >= sizeof(buffer)) {
            perror("Buffer size exceeded");
            break;
        }
    }

    if (write(file_desc, buffer, total_bytes) < 0) {
        perror("Error writing updated content");
    } else {
        printf("File save successful, wrote %zd bytes\n", total_bytes);  // Debugging print
    }

    close(file_desc);
}


void change_password_manager(int client_socket, char *man_id){
    printf("Manager %s is changing Password\n", man_id);

    int flag = 0;
    char new_password[50];

    if (recv(client_socket, new_password, sizeof(new_password), 0) <= 0) {
        perror("Failed to receive new password");
        return;
    }

    load_manager(MAN_DATA_FILE);

    for (int i = 0; i < 1; i++) {
        if (strcmp(mng[i].man_id, man_id) == 0) {
            strncpy(mng[i].password, new_password, sizeof(mng[i].password) - 1);
            mng[i].password[sizeof(mng[i].password) - 1] = '\0'; 

            printf("Updated Password for Manager: %s to %s\n", mng[i].man_id, mng[i].password); 
            flag = 1; 
            break;
        }
    }

    if (flag == 1) {
        save_manager(MAN_DATA_FILE);
        printf("Password updated successfully for employee: %s\n", man_id);
    } else {
        printf("Manager not found: %s\n", man_id);
    }

    send(client_socket, &flag, sizeof(flag), 0);
    return;
}

void log_out_manager(int client_socket, char *man_id){
    int log=1;
    send(client_socket, &log, sizeof(log), 0);
    printf("Manager %s has logged out\n", man_id);
}

void manager(int client_socket, char *user_id, int role){
    while(1){
        int choice;
        recv(client_socket, &choice, sizeof(choice), 0);
        printf("Manager %s selected choice %d\n", user_id, choice);

        switch(choice){
            case 1:
                act_deact_cust_acc(client_socket, user_id);
                break;
            case 2:
                assign_loan_application();
                break;
            case 3:
                review_cust_feedback();
                break;
            case 4:
                change_password_manager(client_socket, user_id);
                break;
            case 5:
                log_out_manager(client_socket, user_id);
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

#endif