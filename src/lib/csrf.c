#include <string.h>
#include "../include/csrf.h"

/*
 * Function to create a Cross Site Forgery Token.
 *
 * The token is used to validate if the request to the server is from the webapplication.
 * This makes sure that no requests can be send through, for example, email to change the content of the website.
 * The token is bound to the session of the user.
 *
 * Create a char array big enough to encapsulate the token.
 * Fill the array with pseudo random characters to make hijacking harder.
 * End the string by adding a NULL.
 * Render the token on the web page to check each time a form is submitted.
 */

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

/*
 * Validate the generated CSRF token, if the token is invalid the POST request is ended and a message is shown.
 */

char validate_csrf_token(onion_request *req, char * csrf){
    const char * csrf_session = getFromSession(req, "csrf");
    if (csrf_session != NULL && strcmp(csrf_session, csrf) == 0) {
        return 1;
    }
    return 0;
}