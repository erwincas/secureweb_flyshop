#include "../include/session.h"

/*
 * Function to easily add variables to the session scope.
 *
 * The session dictionary is an array filled with keys and values.
 */

void addToSession(onion_request *req, char * key, char * value){
    onion_dict *session = onion_request_get_session_dict(req);
    onion_dict_add(session, key, value, OD_DUP_ALL|OD_REPLACE|OD_FREE_ALL);
}

/*
 * Create a new session if a user is successfully logged in.
 * Add the current username to the session dictionary.
 */

void createSession(onion_request *req, char *username) {
    addToSession(req, "username", username);
}

/*
 * Retrieve data from the session dictionary based on the given key.
 */

const char * getFromSession(onion_request *req, char * key){
    return onion_request_get_session(req, key);
}

/*
 * Delete the current session when a user logs out.
 * This also happens automatically when the browser is closed.
 */

void deleteSession(onion_request *req){
    onion_request_session_free(req);
}

/*
 * Get the current user from the session dictionary.
 * If none is found, return NULL.
 */

onion_dict *getUser(onion_request *req) {
    char * username = (char *)getFromSession(req, "username");
    if (username) {
        return getUserInfo(username);
    }
    return NULL;
}

