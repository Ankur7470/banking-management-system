#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_CUSTOMERS 100
#define MAX_EMP 20

typedef struct {
    char user_id[50];
    int role; 
} Session;

Session current_session;
bool is_logged_in = false; 

typedef struct {
    char customer_id[50];
    char password[50];
    double balance;
} Customer;

Customer customers[100];
int cust_count = 0;

typedef struct {
    char emp_id[50];
    char password[50];
} Employee;

Employee empl[100];
int emp_count = 0;

typedef struct {
    char man_id[50];
    char password[50];
} Manager;

Manager mng[100];
int manager_count = 0;

typedef struct {
    char admn_id[50];
    char password[50];
} Admin;

Admin admn[100];
int admin_count = 0;

#endif 
