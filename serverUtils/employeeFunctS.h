#ifndef EMP_SERVER
#define EMP_SERVER

#include <stdio.h>
#include <stdbool.h>
#include "common_struct.h"
#include "customerFunctS.h"
#define MAX_LINE_LENGTH 256

#define CUST_DATA_FILE "customers.txt"
#define EMP_DATA_FILE "employee.txt"

int cust_already_exists(const char *user_id) {
    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, user_id) == 0) {
            return 1; 
        }
    }
    return 0; 
}

void add_new_cust(int client_socket, char *emp_id) {
    printf("Employee %s is Adding a New Customer\n", emp_id);

    char cust_id[50], pass[50];
    double bal = 0.0;
    int flag = 0;
    int already = 0;

    load_customers(CUST_DATA_FILE);

    if (recv(client_socket, cust_id, sizeof(cust_id), 0) < 0) {
        perror("Error receiving customer ID");
        return;
    }
  
    printf("Received customer ID: %s\n", cust_id); 


    if (cust_already_exists(cust_id)) {
        printf("Customer with User ID %s already exists\n", cust_id);
        already = 1;
    }

    send(client_socket, &already, sizeof(already), 0);

    if (already == 1) {
        return; 
    }

    if (recv(client_socket, pass, sizeof(pass), 0) < 0) {
        perror("Error receiving password");
        return;
    }

    printf("Password received: %s\n", pass); 

    if (recv(client_socket, &bal, sizeof(bal), 0) < 0) {
        perror("Error receiving balance");
        return;
    }
    printf("Balance received: %.2lf\n", bal);

    if (cust_count < MAX_CUSTOMERS) {
        strncpy(customers[cust_count].customer_id, cust_id, sizeof(customers[cust_count].customer_id));
        strncpy(customers[cust_count].password, pass, sizeof(customers[cust_count].password));
        customers[cust_count].balance = bal;

        printf("New customer added successfully: %s\n", cust_id);
        cust_count++; 
        flag = 1; 

        save_customers(CUST_DATA_FILE);
    } else {
        printf("Maximum customer limit reached.\n");
        flag = 0; 
    }

    send(client_socket, &flag, sizeof(flag), 0);
}


void modify_cust_details(int client_socket, char *emp_id) {
    printf("Employee %s is Modifying Customer Details\n", emp_id);

    char cust_id[50], new_cust[50], new_pass[50];
    double new_bal = 0.0;
    int flag = 0, already = 0, ch;

    load_customers(CUST_DATA_FILE);

    if (recv(client_socket, cust_id, sizeof(cust_id), 0) < 0) {
        perror("Error receiving customer ID");
        return;
    }

    printf("Received customer ID: %s\n", cust_id); 

    already = cust_already_exists(cust_id);
    send(client_socket, &already, sizeof(already), 0);

    if (already != 1) {
        printf("Customer with User ID %s does not exist\n", cust_id);
        return; 
    }

    recv(client_socket, &ch, sizeof(ch), 0);

    int cust_index = -1;
    for (int i = 0; i < cust_count; i++) {
        if (strcmp(customers[i].customer_id, cust_id) == 0) {
            cust_index = i; 
            break;
        }
    }

    if (cust_index == -1) {
        printf("Customer not found in the list.\n");
        return; 
    }

    if (ch == 1) {
        recv(client_socket, new_cust, sizeof(new_cust), 0);
        printf("New User ID received: %s\n", new_cust);

        strncpy(customers[cust_index].customer_id, new_cust, sizeof(customers[cust_index].customer_id));
        printf("User ID updated successfully: %s\n", new_cust);
        flag = 1;
    } else if (ch == 2) { 
        recv(client_socket, new_pass, sizeof(new_pass), 0);
        printf("New Password received: %s\n", new_pass);

        strncpy(customers[cust_index].password, new_pass, sizeof(customers[cust_index].password));
        printf("Password updated successfully\n");
        flag = 1; 
    } else if (ch == 3) { 
        recv(client_socket, &new_bal, sizeof(new_bal), 0);
        printf("New Balance: %.2lf\n", new_bal);

        customers[cust_index].balance = new_bal;
        printf("Balance modified successfully\n");
        flag = 1; 
    } else {
        printf("Invalid choice for modification.\n");
        flag = 0; 
    }

    save_customers(CUST_DATA_FILE);

    send(client_socket, &flag, sizeof(flag), 0);
}

void process_loan_applications(){

}

void approve_reject_loans(){

}

void view_assigned_loan_applications(){

}

void load_employee(const char *filename) {
    int file_desc = open(filename, O_RDONLY);
    if (file_desc < 0) {
        perror("Could not open employee data file");
        return;
    }

    char buffer[8192];  // Buffer to read data
    ssize_t bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading file");
        close(file_desc);
        return;
    }
    buffer[bytes_read] = '\0';  // Null-terminate the buffer

    emp_count = 0;  // Reset employee count

    // Tokenize the buffer to read each line
    char *line = strtok(buffer, "\n");
    while (line != NULL && emp_count < MAX_EMP) {
        sscanf(line, "%s %s", 
               empl[emp_count].emp_id, 
               empl[emp_count].password);
        printf("Loaded Employee: %s, Password: %s\n", empl[emp_count].emp_id, empl[emp_count].password);  // Debugging print
        emp_count++;
        line = strtok(NULL, "\n");
    }

    printf("Total Employees Loaded: %d\n", emp_count);  // Debugging print
    close(file_desc);
}

void save_employee(const char *filename) {
    // Open file in write mode, truncate to overwrite previous contents
    int file_desc = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (file_desc < 0) {
        perror("Could not open employee data file for writing");
        return;
    }

    char buffer[8192];  // Buffer to hold the data to write
    ssize_t total_bytes = 0;

    // Write all employees to buffer
    for (int i = 0; i < emp_count; i++) {
        ssize_t bytes_written = snprintf(buffer + total_bytes, sizeof(buffer) - total_bytes,
                                         "%s %s\n",
                                         empl[i].emp_id, 
                                         empl[i].password);
        total_bytes += bytes_written;  // Update total bytes written

        // Debugging print for each employee saved
        printf("Saving Employee: %s, Password: %s\n", empl[i].emp_id, empl[i].password);

        if (total_bytes >= sizeof(buffer)) {
            perror("Buffer size exceeded");
            break;
        }
    }

    // Write buffer to file
    if (write(file_desc, buffer, total_bytes) < 0) {
        perror("Error writing updated content");
    } else {
        printf("File save successful, wrote %zd bytes\n", total_bytes);  // Debugging print
    }

    close(file_desc);
}


void change_password_emp(int client_socket, const char *emp_id) {
    printf("Employee %s is changing Password\n", emp_id);

    int flag = 0;
    char new_password[50];

    if (recv(client_socket, new_password, sizeof(new_password), 0) <= 0) {
        perror("Failed to receive new password");
        return;
    }

    load_employee(EMP_DATA_FILE);

    for (int i = 0; i < emp_count; i++) {
        if (strcmp(empl[i].emp_id, emp_id) == 0) {
            strncpy(empl[i].password, new_password, sizeof(empl[i].password) - 1);
            empl[i].password[sizeof(empl[i].password) - 1] = '\0';  
            printf("Updated Password for Employee: %s to %s\n", empl[i].emp_id, empl[i].password); 
            flag = 1; 
            break;
        }
    }

    if (flag == 1) {
        save_employee(EMP_DATA_FILE);
        printf("Password updated successfully for employee: %s\n", emp_id);
    } else {
        printf("Employee ID not found: %s\n", emp_id);
    }

    send(client_socket, &flag, sizeof(flag), 0);
    return;
}


void log_out_emp(int client_socket, char *emp_id){
    int log=1;
    send(client_socket, &log, sizeof(log), 0);
    printf("Employee %s has logged out\n", emp_id);
}

void employee(int client_socket, char *user_id, int role){
    while(1){
        int choice;
        recv(client_socket, &choice, sizeof(choice), 0);
        printf("Employee %s selected choice %d\n", user_id, choice);

        switch(choice){
            case 1:
                add_new_cust(client_socket, user_id);
                break;
            case 2:
                modify_cust_details(client_socket, user_id);
                break;
            case 3:
                process_loan_applications();
                break;
            case 4:
                approve_reject_loans();
                break;
            case 5:
                view_assigned_loan_applications();
                break;
            case 6:
                change_password_emp(client_socket, user_id);
                break;
            case 7:
                log_out_emp(client_socket, user_id);
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

#endif