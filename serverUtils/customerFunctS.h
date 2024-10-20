#ifndef CUST_SERVER
#define CUST_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include "common_struct.h"

#define CUST_DATA_FILE "customers.txt"
#define MAX_CUSTOMERS 100
#define RECORD_LENGTH 128

void load_customers(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open customer data file");
        return;
    }

    char buffer[8192];  
    int bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading file");
        close(file_desc);
        return;
    }
    buffer[bytes_read] = '\0';  

    cust_count = 0;  

    char *line = strtok(buffer, "\n");
    while (line != NULL && cust_count < MAX_CUSTOMERS) {
        sscanf(line, "%s %s %lf", 
               customers[cust_count].customer_id, 
               customers[cust_count].password, 
               &customers[cust_count].balance);
        cust_count++;
        line = strtok(NULL, "\n");
    }

    close(file_desc);
}


void save_customers(const char *filename) {
    int file_desc = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (file_desc < 0) {
        perror("Could not open customer data file for writing");
        return;
    }

    char buffer[8192];  
    int total_bytes = 0;

    for (int i = 0; i < cust_count; i++) {
        int bytes_written = snprintf(buffer + total_bytes, sizeof(buffer) - total_bytes,
                                          "%s %s %.2lf\n",
                                          customers[i].customer_id, 
                                          customers[i].password, 
                                          customers[i].balance);
        total_bytes += bytes_written;  
    }

    if (write(file_desc, buffer, total_bytes) < 0) {
        perror("Error writing updated content");
    }

    close(file_desc);
}

void view_acc_details(int client_socket, const char *customer_id) {
    printf("Customer %s is viewing account details\n", customer_id);
   
    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, customer_id) == 0) {
            send(client_socket, customers[i].customer_id, sizeof(customers[i].customer_id), 0);
            send(client_socket, &customers[i].balance, sizeof(customers[i].balance), 0);
            break;
        }
    }
}

void update_balance(int client_socket, const char *customer_id, int is_deposit) {
    if(is_deposit){
        printf("Customer %s is depositing money\n", customer_id);
    }
    else{
        printf("Customer %s is withdrawing money\n", customer_id);
    }
    double amount = 0.0;
    int flag = 0;

    int bytes_received = recv(client_socket, &amount, sizeof(amount), 0);
    if (bytes_received < 0) {
        perror("Failed to receive amount");
        return;
    }

    int file_desc = open(CUST_DATA_FILE, O_RDWR);
    if (file_desc < 0) {
        perror("Could not open customer data file");
        return;
    }

    struct flock lock;
    int record_position = -1;

    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, customer_id) == 0) {
            record_position = i;

            lock.l_type = F_WRLCK; 
            lock.l_whence = SEEK_SET;
            lock.l_start = record_position * RECORD_LENGTH;
            lock.l_len = RECORD_LENGTH; 
   
            if (fcntl(file_desc, F_SETLKW, &lock) < 0) {
                perror("Failed to lock the record");
                close(file_desc);
                return;
            }

            if (is_deposit) {
                customers[i].balance += amount;
            } else {
                if (customers[i].balance >= amount) {
                    customers[i].balance -= amount;
                } else {
                    printf("Insufficient balance for withdrawal.\n");
                    flag = 0;
                }
            }
            flag = 1; 
            break;
        }
    }

    if (record_position != -1) {
        lock.l_type = F_UNLCK; 
        fcntl(file_desc, F_SETLK, &lock);
    }

    if (flag == 1) {
        save_customers(CUST_DATA_FILE);
    }

    send(client_socket, &flag, sizeof(flag), 0);
    close(file_desc);
}

int find_record_position(const char *customer_id) {
    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, customer_id) == 0) {
            return i; 
        }
    }
    return -1; 
}

void transfer_funds(int client_socket, const char *customer_id) {
    int res = 1; 
    char recv_id[50];
    double amt;

    int bytes_received_id = recv(client_socket, recv_id, sizeof(recv_id), 0);
    if (bytes_received_id < 0) {
        perror("Failed to receive receiver ID");
        res = 0; 
    }

    int bytes_received_amt = recv(client_socket, &amt, sizeof(amt), 0);
    if (bytes_received_amt < 0) {
        perror("Failed to receive amount");
        res = 0; 
    }

    printf("Transferring Funds: %.2lf from %s to %s\n", amt, customer_id, recv_id);

    int sender_index = find_record_position(customer_id);
    int receiver_index = find_record_position(recv_id);

    if (sender_index == -1 || receiver_index == -1) {
        printf("Sender or Receiver ID not found.\n");
        res = 0; 
    } else if (customers[sender_index].balance < amt) {
        printf("Insufficient balance for transfer.\n");
        res = 0; 
    }

    if (res == 1) { 
        int file_desc = open(CUST_DATA_FILE, O_RDWR); 
        if (file_desc < 0) {
            perror("Could not open customer data file");
            res = 0; 
        } else {
            struct flock lock;
            lock.l_type = F_WRLCK; 
            lock.l_whence = SEEK_SET;
            lock.l_start = sender_index * RECORD_LENGTH; 
            lock.l_len = RECORD_LENGTH;
            fcntl(file_desc, F_SETLKW, &lock);

            lock.l_start = receiver_index * RECORD_LENGTH; 
            fcntl(file_desc, F_SETLKW, &lock);

            customers[sender_index].balance -= amt;
            customers[receiver_index].balance += amt;

            char updated_content[8192] = {0};
            for (int i = 0; i < cust_count; i++) {
                snprintf(updated_content + strlen(updated_content), sizeof(updated_content) - strlen(updated_content),
                         "%s %s %.2lf\n", customers[i].customer_id, customers[i].password,
                         customers[i].balance);
            }

            if (ftruncate(file_desc, 0) != 0) {
                perror("Error truncating file");
                close(file_desc);
                return;
            }

            lseek(file_desc, 0, SEEK_SET);
            if (write(file_desc, updated_content, strlen(updated_content)) < 0) {
                perror("Error writing updated content");
                close(file_desc);
                return;
            }

            lock.l_type = F_UNLCK;
            fcntl(file_desc, F_SETLK, &lock);
            close(file_desc);
        }
    }

    int bytes_sent = send(client_socket, &res, sizeof(res), 0);
    if (bytes_sent < 0) {
        perror("Failed to send response to client");
    }
}

void apply_for_loan(){

}

void change_password_cust(int client_socket, const char *customer_id) {
    printf("Customer %s is changing Password\n", customer_id);

    int flag = 0;
    char new_password[50];

    if (recv(client_socket, new_password, sizeof(new_password), 0) <= 0) {
        perror("Failed to receive new password");
        return;
    }

    load_customers(CUST_DATA_FILE);

    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, customer_id) == 0) {
            strcpy(customers[i].password, new_password);
            flag = 1; 
            break;
        }
    }

    if (flag == 1) {
        save_customers(CUST_DATA_FILE);
    }

    send(client_socket, &flag, sizeof(flag), 0);
    return;
}


void adding_feedback(){

}

void view_transaction_hist(){

}

void log_out_cust(int client_socket, char *customer_id){
    int log=1;
    send(client_socket, &log, sizeof(log), 0);
    printf("Customer %s has logged out\n", customer_id);
}


void customer(int client_socket, char *user_id, int role){
    while(1){
        load_customers(CUST_DATA_FILE);
        int choice;
        recv(client_socket, &choice, sizeof(choice), 0);
        printf("Customer %s selected choice %d\n", user_id, choice);
        switch(choice){
            case 1:
                view_acc_details(client_socket, user_id);
                break;
            case 2:
                update_balance(client_socket, user_id, 1);
                break;
            case 3:
                update_balance(client_socket, user_id, 0);
                break;
            case 4:
                transfer_funds(client_socket, user_id);
                break;
            case 5:
                apply_for_loan();
                break;
            case 6:
                change_password_cust(client_socket, user_id);
                break;
            case 7:
                adding_feedback();
                break;
            case 8:
                view_transaction_hist();
                break;
            case 9:
                log_out_cust(client_socket, user_id);
                return;   
            case 10: 
                return;    
        }
    }
}

#endif