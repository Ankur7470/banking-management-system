#ifndef EMPLOYEE
#define EMPLOYEE

#include <stdio.h>

void emp_menu(){
    printf("\nBank Employee Menu:\n");
    printf("1. Add New Customer\n");
    printf("2. Modify Customer Details\n");
    printf("3. Process Loan Applications\n");
    printf("4. Approve/Reject Loans\n");
    printf("5. View Assigned Loan Applications\n");
    printf("6. Change Password\n");
    printf("7. Log Out\n");
    printf("8. Exit\n");
    printf("Enter Option: ");
}

void add_new_cust(int client_socket){
    char user_id[50], pass[50];
    double bal= 0.0;
    int res;
    int already;
    printf("Enter the user ID of the Customer: ");
    scanf("%s", user_id);
    send(client_socket, user_id, sizeof(user_id), 0);
    recv(client_socket, &already, sizeof(already), 0);
    if(already){
        printf("Customer with User ID: %s already Exists !!\n", user_id);
        return;
    }
    else{
        printf("Enter a password for Customer: ");
        scanf("%s", pass);
        send(client_socket, pass, sizeof(pass), 0);
        printf("Enter the initial balance of Customer: ");
        scanf("%lf", &bal);
        send(client_socket, &bal, sizeof(bal), 0);

        recv(client_socket, &res, sizeof(res), 0);
        if(res == 1){
            printf("New Customer Added Successfully!!\n");
        }
        else{
            printf("Failed to Add New Customer!!\n");
        }
    }
}


void modify_cust_details(int client_socket){
    char user_id[50];
    int already;
    int res;
    printf("Enter the user ID of the Customer: ");
    scanf("%s", user_id);
    send(client_socket, user_id, sizeof(user_id), 0);
    recv(client_socket, &already, sizeof(already), 0);
    if(!already){
        printf("Customer with User ID: %s does not exists !!\n", user_id);
        return;
    }
    else{
        int ch;
        printf("--Modyifying Details--\n");
        printf("Select a choice: \n");
        printf("1. Modify User ID\n");
        printf("2. Modify Password\n");
        printf("3. Modify Account Balance\n");
        printf("Enter your choice: ");
        scanf("%d", &ch);
        send(client_socket, &ch, sizeof(ch), 0);
        if(ch == 1){
            char new_user[50];
            printf("\nEnter New User ID: ");
            scanf("%s", new_user);
            send(client_socket, new_user, sizeof(new_user), 0);
        }
        else if(ch == 2){
            char new_pass[50];
            printf("\nEnter New Pass: ");
            scanf("%s", new_pass);
            send(client_socket, new_pass, sizeof(new_pass), 0);
        }
        else if(ch == 3){
            double up_bal;
            printf("\nEnter New Balance: ");
            scanf("%lf", &up_bal);
            send(client_socket, &up_bal, sizeof(up_bal), 0);
        }
        else{
            printf("Invalid Choice!!\n");
        }
        recv(client_socket, &res, sizeof(res), 0);
        if(res == 1){
            printf("Customer details modified Successfully!!\n");
        }
        else{
            printf("Failed to modify!!\n");
        }
    }
}

void process_loan_applications(){

}

void approve_reject_loans(){

}

void view_assigned_loan_applications(){

}
void change_pasword_emp(int client_socket){
    int res = 0;
    char new_pass[50];
    printf("Enter New Password: ");
    scanf("%s", new_pass);
    send(client_socket, new_pass, sizeof(new_pass), 0);
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        printf("Password Changed Successfully !!\n");
    }
    else{
        printf("Error while changing the password!!\n");
    }
}

void log_out_emp(int client_socket){
    int res;
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        return;
    }
}

void employee(int client_socket){
    int opt;
    while(1){
        emp_menu();
        scanf("%d",&opt);

        send(client_socket, &opt, sizeof(opt), 0);

        if(opt == 8){
            printf("Disconnecting...\n");
            break; 
        }

        if (opt < 1 || opt > 8) {
            printf("Invalid choice. Please try again.\n");
            continue; 
        }

        switch(opt){
            case 1:
                add_new_cust(client_socket);
                break;
            case 2:
                modify_cust_details(client_socket);
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
                change_pasword_emp(client_socket);
                break;
            case 7:
                log_out_emp(client_socket);
                return;
             default:
                printf("Invalid choice: %d\n", opt);
                break;
        }
    }
}

#endif