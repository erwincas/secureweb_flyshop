#ifndef SECURE_PROGRAMMING_SESSION_H
#define SECURE_PROGRAMMING_SESSION_H

#include <onion/onion.h>
#include <onion/dict.h>
#include <stdio.h>
#include <database.h>

void createSession(onion_request *req, char username []);
onion_dict *getUser(onion_request *req);
void deleteSession(onion_request *req);
const char * getFromSession(onion_request *req, char * key);
void addToSession(onion_request *req, char * key, char * value);
#endif //SECURE_PROGRAMMING_SESSION_H





