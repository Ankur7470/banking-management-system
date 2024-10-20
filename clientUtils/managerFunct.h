#ifndef MANAGER
#define MANAGER

#include <stdio.h>

void manager_menu(){
    printf("\nManager Menu:\n");
    printf("1. Activate/Deactivate Customer Accounts\n");
    printf("2. Assign Loan Application Processes to Employees\n");
    printf("3. Review Customer Feedback\n");
    printf("4. Change Password\n");
    printf("5. Log Out\n");
    printf("6. Exit\n");
    printf("Enter Option: ");
}

void act_deact_cust_acc(int client_socket){

}

void assign_loan_application(){

}

void review_cust_feedback(){

}

void change_password_manager(int client_socket){
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

void log_out_manager(int client_socket){
    int res;
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        return;
    }
}

void manager(int client_socket){
    int opt;
    while(1){
        manager_menu();
        scanf("%d",&opt);

        send(client_socket, &opt, sizeof(opt), 0);

        if(opt == 6){
            printf("Disconnecting...\n");
            break; 
        }

        if (opt < 1 || opt > 6) {
            printf("Invalid choice. Please try again.\n");
            continue; 
        }

        switch(opt){
            case 1:
                act_deact_cust_acc(client_socket);
                break;
            case 2:
                assign_loan_application();
                break;
            case 3:
                review_cust_feedback();
                break;
            case 4:
                change_password_manager(client_socket);
                break;
            case 5:
                log_out_manager(client_socket);
                break;
            default:
                printf("Invalid choice: %d\n", opt);
                break;
        }
    }
}

#endif