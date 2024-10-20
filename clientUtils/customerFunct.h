#ifndef CUST
#define CUST

#include <stdio.h>

void customer_menu(){
    printf("\nCustomer Menu:\n");
    printf("1. View Account Details\n");
    printf("2. Deposit Money\n");
    printf("3. Withdraw Money\n");
    printf("4. Transfer Funds\n");
    printf("5. Apply for a Loan\n");
    printf("6. Change Password\n");
    printf("7. Adding Feedback\n");
    printf("8. View Transaction History\n");
    printf("9. Log Out\n");
    printf("10. Exit\n");
    printf("Enter Option: ");
}

void view_acc_details(int client_socket){
    char name[50], cust_id[50];
    double curr_balance;
    recv(client_socket, cust_id, sizeof(cust_id), 0);
    recv(client_socket, &curr_balance, sizeof(curr_balance), 0);
    printf("\nUser ID: %s\n", cust_id);
    printf("Current Balance: %.2lf\n", curr_balance);
}

void deposit_money(int client_socket){
    double amt;
    int res;
    printf("Enter the Amount You Want to deposit: ");
    scanf("%lf", &amt);
    send(client_socket, &amt, sizeof(amt), 0);
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        printf("Amount Deposited Successfully!!\n");
    }
    else{
        printf("Amount not Deposited!!");
    }
}

void withdraw_money(int client_socket){
    int res;
    double amt;
    printf("Enter the Amount You Want to withdraw: ");
    scanf("%lf", &amt);
    send(client_socket, &amt, sizeof(amt), 0);
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        printf("Amount Withdrawn Successfully!!\n");
    }
    else{
        printf("Failed to Withdraw!!");
    }
}

void transfer_funds(int client_socket) {
    double amt;
    char recv_id[50];

    printf("Enter receiver ID: ");
    scanf("%s", recv_id);
    printf("Enter amount: ");
    scanf("%lf", &amt);

    int bytes_sent_id = send(client_socket, recv_id, sizeof(recv_id), 0);
    if (bytes_sent_id < 0) {
        perror("Failed to send receiver ID");
        return; 
    }

    ssize_t bytes_sent_amt = send(client_socket, &amt, sizeof(amt), 0);
    if (bytes_sent_amt < 0) {
        perror("Failed to send amount");
        return; 
    }

    int res;
    printf("Waiting to receive transfer response from server.\n");

    int bytes_received = recv(client_socket, &res, sizeof(res), 0);
    if (bytes_received < 0) {
        perror("Failed to receive response from server");
        return; 
    }

    if (res == 1) {
        printf("Funds Transferred Successfully!!\n");
    } else {
        printf("Failed to Transfer Funds!!\n");
    }
}



void apply_for_loan(){

}

void change_password_cust(int client_socket){
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

void adding_feedback(){

}

void view_transaction_hist(){

}

void log_out_cust(int client_socket){
    int res;
    recv(client_socket, &res, sizeof(res), 0);
    if(res == 1){
        return;
    }
}

void customer(int client_socket){
    int opt;
    while(1){
        customer_menu();
        scanf("%d",&opt);

        send(client_socket, &opt, sizeof(opt), 0);

        if(opt == 10){
            printf("Disconnecting...\n");
            break; 
        }

        if (opt < 1 || opt > 10) {
            printf("Invalid choice. Please try again.\n");
            continue; 
        }

        switch(opt){
            case 1:
                view_acc_details(client_socket);
                break;
            case 2:
                deposit_money(client_socket);
                break;
            case 3:
                withdraw_money(client_socket);
                break;
            case 4:
                transfer_funds(client_socket);
                break;
            case 5:
                apply_for_loan();
                break;
            case 6:
                change_password_cust(client_socket);
                break;
            case 7:
                adding_feedback();
                break;
            case 8:
                view_transaction_hist();
                break;
            case 9:
                log_out_cust(client_socket);
                return;
        }
    }
}

#endif