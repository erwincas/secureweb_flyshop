#include "../include/session.h"

void addToSession(onion_request *req, char * key, char * value){
    onion_dict *session = onion_request_get_session_dict(req);
    onion_dict_add(session, key, value, OD_DUP_ALL|OD_REPLACE|OD_FREE_ALL);
}

//creates new session with username set.
void createSession(onion_request *req, char *username) {
    addToSession(req, "username", username);
}

const char * getFromSession(onion_request *req, char * key){
    return onion_request_get_session(req, key);
}

void deleteSession(onion_request *req){
    onion_request_session_free(req);
}

//get user (string) of current session, returns NULL on no user.
onion_dict *getUser(onion_request *req) {
    char * username = (char *)getFromSession(req, "username");
    if (username) {
        return getUserInfo(username);
    }
    return NULL;
}

