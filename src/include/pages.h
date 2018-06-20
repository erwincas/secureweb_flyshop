//
// Created by erwin on 13-6-18.
//

#ifndef SECURE_PROGRAMMING_PAGES_H
#define SECURE_PROGRAMMING_PAGES_H

#include <onion/types.h>
#include <onion/request.h>
#include <onion/shortcuts.h>
#include "assets.h"
#include "database.h"
#include "webserver.h"
#include "session.h"

void login_page(void *p, onion_request *req, onion_response *res);
void register_page(void *p, onion_request *req, onion_response *res);
void profile_page(void *p, onion_request *req, onion_response *res);
void orders_page(void *p, onion_request *req, onion_response *res);
void index_page(void *p, onion_request *req, onion_response *res);
void admin_page(void *p, onion_request *req, onion_response *res);
void logout(void *p, onion_request *req, onion_response *res);

#endif //SECURE_PROGRAMMING_PAGES_H
