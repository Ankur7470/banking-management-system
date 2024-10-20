#ifndef ADMIN
#define ADMIN

#include <stdio.h>

void admin_menu(){
    printf("\nAdministrator Menu:\n");
    printf("1. Add New Bank Employee\n");
    printf("2. Modify Customer/Employee Details\n");
    printf("3. Manage User Roles\n");
    printf("4. Change Password\n");
    printf("5. Log Out\n");
    printf("6. Exit\n");
    printf("Enter Option: ");
}

void add_new_bank_emp(){

}

void modify_cust_emp_details(){

}

void manage_user_roles(){

}

void change_password_admin(){

}

void log_out_admin(){

}

void admin(){
    int opt;
    while(1){
        admin_menu();
        scanf("%d",&opt);

        if(opt == 6){
            printf("Disconnecting...\n");
            break; 
        }

        if (opt < 1 || opt > 10) {
            printf("Invalid choice. Please try again.\n");
            continue; 
        }

        switch(opt){
            case 1:
                add_new_bank_emp();
                break;
            case 2:
                modify_cust_emp_details();
                break;
            case 3:
                manage_user_roles();
                break;
            case 4:
                change_password_admin();
                break;
            case 5:
                log_out_admin();
                break;
        }
    }
}

#endif