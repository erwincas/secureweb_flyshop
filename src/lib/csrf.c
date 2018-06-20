#include <string.h>
#include "../include/csrf.h"

void generate_csrf_token(onion_request *req, onion_dict *dict)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char * csrf = malloc(sizeof(char) * CSRF_LEN);
    srand(time(NULL));
    for (size_t i = 0; i < CSRF_LEN; i++) {
        int key = rand() % (int) (sizeof charset - 1);
        csrf[i] = charset[key];
    }
    csrf[CSRF_LEN] = '\0';

    onion_dict *session = onion_request_get_session_dict(req);
    onion_dict_add(session, "csrf", csrf, OD_STRING|OD_DUP_ALL|OD_REPLACE);
    onion_dict_add(dict, "csrf", csrf, OD_STRING|OD_DUP_ALL|OD_REPLACE);
}


char validate_csrf_token(onion_request *req, char * csrf){
    const char * csrf_session = getFromSession(req, "csrf");
    if (csrf_session != NULL && strcmp(csrf_session, csrf) == 0) {
        return 1;
    }
    return 0;
}