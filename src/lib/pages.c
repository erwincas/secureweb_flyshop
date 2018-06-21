#include "../include/pages.h"
#include "../include/csrf.h"

/*
 * Redirect to the login page after a user is logged in or the session is still set.
 */
onion_connection_status loginRedirect(void *p, onion_request *req, onion_response *res) {
    return onion_shortcut_redirect("index", req, res);
}

/*
 * Redirect to the login page if there is no session available and the user isn't logged in.
 */
onion_connection_status guestRedirect(void *p, onion_request *req, onion_response *res) {
    return onion_shortcut_redirect("login", req, res);
}

/*
 * Functionality for the login page.
 * Gets called when the user lands on the login page or uses the login function.
 *
 * Create a dict, this is used to send data to the templating engine and render it on a page.
 * Check if the user was already logged in, if so redirect to the homepage.
 * If a login request to the server is captured check if the csrf token is valid.
 * If so, retrieve the username and password, check if they are filled.
 * Call the function to retrieve user data and check if the current input matches a row.
 * If a row is found, create a session and log a user in.
 *
 * A CSRF token is generated if the user lands on the login page.
 */

void login_page(void *p, onion_request *req, onion_response *res) {
    onion_dict *dict = onion_dict_new();
    if (getUser(req)) loginRedirect(p, req, res);
    if (onion_request_get_flags(req) & OR_POST) {
        char *csrf = get_post_data(p, req, res, "_token");
        if (validate_csrf_token(req, csrf)) {
            char *uname = get_post_data(p, req, res, "uname");
            char *pass = get_post_data(p, req, res, "pwd");
            if (uname && pass) {
                if (checkLogin(uname, pass)) {
                    createSession(req, uname);
                    loginRedirect(p, req, res);
                } else {
                    onion_dict_add(dict, "message", "Gebruikersnaam of wachtwoord is incorrect!", 0);
                }
            }
        } else {
            onion_dict_add(dict, "message", "Geen valide CSRF meegegeven", 0);
        }
    }
    generate_csrf_token(req, dict);
    return login_html(dict, res);
}

/*
 * Functionality of the home page.
 *
 * Each time a request is captured retrieve the current user from the session.
 * If no user is found, redirect to the login page.
 *
 * Create a new dictionary to render data on the web page.
 * Check if the CSRF token is valid.
 * If a user tries to buy a ticket, check if it has enough resources and if the flight still exists.
 *
 * If a buy request is not caught, render the webpage, show all the possible flights.
 */

void index_page(void *p, onion_request *req, onion_response *res) {
    onion_dict *user = getUser(req);
    if (user) {
        onion_dict *dict = onion_dict_new();
        if (onion_request_get_flags(req) & OR_POST) {
            char *csrf = get_post_data(p, req, res, "_token");
            if (validate_csrf_token(req, csrf)) {
                int buy = atoi(get_post_data(p, req, res, "buy"));
                if (buy) {
                    int code = buyFlight(user, buy);
                    switch (code) {
                        case 1:
                            onion_dict_add(dict, "message", "Vlucht succesvol gekocht!", 0);
                            break;
                        case 2:
                            onion_dict_add(dict, "message", "Geen vlucht met dit id gevonden", 0);
                            break;
                        case 3:
                            onion_dict_add(dict, "message", "Niet genoeg miles voor dit product", 0);
                            break;
                        default:
                            onion_dict_add(dict, "message", "Database of onbekende error", 0);
                            break;
                    }
                }
            } else {
                onion_dict_add(dict, "message", "Geen valide CSRF meegegeven!", 0);
            }
        }
        int user_id = atoi(onion_dict_get(getUser(req), "id"));
        onion_dict *flights = getFlights(user_id);
        onion_dict_add(dict, "flights", flights, OD_DICT | OD_FREE_ALL);
        onion_dict_add(dict, "user", getUser(req), OD_DICT | OD_FREE_ALL);
        generate_csrf_token(req, dict);
        return index_html(dict, res);
    }
    guestRedirect(p, req, res);
}

/*
 * Personal orders page
 *
 * Check if a user is logged in, if not redirect to the login page.
 * Retrieve the orders based on the user id.
 * Verify the CSRF token
 *
 *
 */
void orders_page(void *p, onion_request *req, onion_response *res) {
    onion_dict *user = getUser(req);
    if (user) {
        onion_dict *dict = onion_dict_new();
        int user_id = atoi(onion_dict_get(getUser(req), "id"));
        if (onion_request_get_flags(req) & OR_POST) {
            char *csrf = get_post_data(p, req, res, "_token");
            if (validate_csrf_token(req, csrf)) {
                deleteOrder(atoi(onion_dict_get(user, "id")), atoi(get_post_data(p, req, res, "flight_id")));
            } else {
                onion_dict_add(dict, "message", "Geen valide CSRF meegegeven!", 0);
            }
        }

        onion_dict_add(dict, "orders", getOrders(user_id), OD_DICT | OD_FREE_ALL);
        generate_csrf_token(req, dict);
        return orders_html(dict, res);
    }
    guestRedirect(p, req, res);
}

void profile_page(void *p, onion_request *req, onion_response *res) {
    onion_dict *user = getUser(req);
    if (user) {
        onion_dict *dict = onion_dict_new();
        if (onion_request_get_flags(req) & OR_POST) {
            char *csrf = get_post_data(p, req, res, "_token");
            if (validate_csrf_token(req, csrf)) {
                char *pass = get_post_data(p, req, res, "pwd");
                if (strcmp(pass, "") != 0) {
                    changePassword((char *) onion_dict_get(user, "username"), pass);
                    onion_dict_add(dict, "message", "Wachtwoord gewijzigd!", 0);
                }
            } else {
                onion_dict_add(dict, "message", "Geen valide CSRF meegegeven!", 0);
            }
        }
        onion_dict_add(dict, "user", user, OD_DICT | OD_FREE_VALUE);
        generate_csrf_token(req, dict);
        return profile_html(dict, res);
    }
    guestRedirect(p, req, res);
}

void register_page(void *p, onion_request *req, onion_response *res) {
    if (getUser(req)) loginRedirect(p, req, res);
    onion_dict *dict = onion_dict_new();
    if (onion_request_get_flags(req) & OR_POST) {
        char *csrf = get_post_data(p, req, res, "_token");
        if (validate_csrf_token(req, csrf)) {
            char *uname = get_post_data(p, req, res, "uname");
            char *pass = get_post_data(p, req, res, "pwd");
            if (uname && pass) {
                if (registerUser(uname, pass)) {
                    createSession(req, uname);
                    loginRedirect(p, req, res);
                } else {
                    onion_dict_add(dict, "message", "Gebruikersnaam bestaat al!", 0);
                }
            }
        } else {
            onion_dict_add(dict, "message", "Geen valide CSRF meegegeven!", 0);
        }
    }
    generate_csrf_token(req, dict);
    return register_html(dict, res);
}

void admin_page(void *p, onion_request *req, onion_response *res) {
    const char *admin = onion_dict_get(getUser(req), "admin");
    if (admin != NULL && strcmp(admin, "1") == 0) {
        onion_dict *dict = onion_dict_new();
        if (onion_request_get_flags(req) & OR_POST) {
            char *csrf = get_post_data(p, req, res, "_token");
            if (validate_csrf_token(req, csrf)) {
                char *username = get_post_data(p, req, res, "username");
                char *pass = get_post_data(p, req, res, "password");
                char *miles = get_post_data(p, req, res, "miles");
                char *adminPost = get_post_data(p, req, res, "admin");
                changeUser(username, pass, miles, adminPost);
                onion_dict_add(dict, "message", "Gebruiker gewijzigd!", 0);
            } else {
                onion_dict_add(dict, "message", "Geen valide CSRF meegegeven!", 0);
            }
        }
        generate_csrf_token(req, dict);
        onion_dict_add(dict, "users", getUsers(), OD_DICT | OD_FREE_ALL);
        return admin_html(dict, res);
    }
    guestRedirect(p, req, res);
}

void logout(void *p, onion_request *req, onion_response *res) {
    if (getUser(req)) deleteSession(req);
    guestRedirect(p, req, res);
}