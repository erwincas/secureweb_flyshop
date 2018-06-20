//
// Created by erwin on 15-6-18.
//

#ifndef SECURE_PROGRAMMING_CSRF_H
#define SECURE_PROGRAMMING_CSRF_H

#define CSRF_LEN 25

#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "../include/session.h"

void generate_csrf_token(onion_request *req, onion_dict *dict);
char validate_csrf_token(onion_request *req, char * csrf);

#endif //SECURE_PROGRAMMING_CSRF_H
