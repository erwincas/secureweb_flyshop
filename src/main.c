#include "include/database.h"
#include "include/webserver.h"

/*
 * Initialisation of the database and webserver.
 *
 * Creates a new database and the appropriate tables if they don't exist.
 * Start the webserver thread.
 * Webserver handles all requests and responses.
 */
int main(int argc, char **argv){

    database();
    webserver();
    return 0;
}
