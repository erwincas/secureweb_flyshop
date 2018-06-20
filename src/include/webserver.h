//
// Created by erwin on 15-5-18.
//

#ifndef SECURE_PROGRAMMING_ONIONCUSTOM_H
#define SECURE_PROGRAMMING_ONIONCUSTOM_H

#include <onion/onion.h>
#include <onion/sessions_sqlite3.h>
#include <signal.h>
#include <unistd.h>

int webserver();
char *get_post_data(void *_, onion_request *req, onion_response *res, char *postName);

#endif //SECURE_PROGRAMMING_ONIONCUSTOM_H