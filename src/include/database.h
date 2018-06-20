//
// Created by erwin on 15-5-18.
//

#ifndef SECURE_PROGRAMMING_DATABASE_H
#define SECURE_PROGRAMMING_DATABASE_H
#define DATABASE_NAME "database.db"

#include <sqlite3.h>
#include <onion/dict.h>
#include <stdio.h>
#include <unistd.h>
#include "encryption.h"

//Variable declaration
sqlite3 *db;
int rc;
sqlite3_stmt *statement;

int database();
char checkLogin(char *user, char *password);
char changePassword(char *user, char *password);
char registerUser(char *user, char *password);
char changeUser(char *user, char *password, char *miles, char *admin);
int getMiles(char *user);
char * intToString(int number);
char buyFlight(onion_dict *user, int flight_id);
char deleteOrder(int user_id, int flight_id);
onion_dict * getFlights(int user_id);
onion_dict * getUsers();
onion_dict * getUserInfo(char * username);
onion_dict * getOrders(int user_id);


#endif //SECURE_PROGRAMMING_DATABASE_H
