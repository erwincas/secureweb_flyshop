#include "../include/database.h"
/**
 * File for all database functions within the project
 */

//Variable declaration
char *zErrMsg = 0;

/*
 * Create a connection to the SQLITE3 database.
 */

char db_connect(void) {
    rc = sqlite3_open(DATABASE_NAME, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    }
    return 1;
}

/*
 * Close a connection to the SQLITE3 database
 */

void db_close(void) {
    sqlite3_close(db);
}

/*
 * Create the initial tables needed to make the application work.
 * Connect to the database, send a few queries and close the connection again.
 */

void db_create_test_env(void) {
    db_connect();
    char sql[500];
    zErrMsg = 0;

    sprintf(sql, "CREATE TABLE users(id integer primary key, username text not null unique, password text not null, admin int default 0, inhollandMiles integer not null ); INSERT INTO users VALUES (1, 'admin', '%s', 1, 500);", argon2("admin"));
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    sprintf(sql, "CREATE TABLE flights(id integer primary key, flight_name text not null,`from` text not null,`to` text not null,price integer not null,departing_date datetime not null); INSERT INTO flights VALUES(1, 'Transavia-744', 'AMS', 'ATH', 25, '2018-07-10 10:00:00');INSERT INTO flights VALUES(2, 'KLM-011', 'AMS', 'ZMY', 49, '2018-07-01 14:30:00');");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    sprintf(sql, "PRAGMA foreign_keys = ON; CREATE TABLE flight_orders(id integer primary key, flight_id integer, user_id integer, status text, date datetime default current_timestamp, FOREIGN KEY(flight_id) REFERENCES flights(id), FOREIGN KEY(user_id) REFERENCES users(id));");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    fprintf(stdout, "Tables created successfully\n");
    db_close();
}

/*
 * Check if database tables already exist, if not recreate them.
 */

int database(void) {
    if (access(DATABASE_NAME, F_OK) == -1) {
        // file doesn't exist, also create test tables
        db_create_test_env();
    }
    return 0;
}

/*
 * Check the user login.
 * First initialise a connection to the database.
 * Hash the user password, retrieve the stored one.
 * Create a prepared statement, this is against SQL INJECTION, retrieve the result and check if a row is given back.
 * If so check again if the row matches the given data.
 */

char checkLogin(char *user, char *password) {
    db_connect();
    const char * hashed_password;
    //Get the password from the database
    const char *query = "SELECT password FROM users WHERE username=?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_text(statement, 1, user, (int)strlen(user), SQLITE_STATIC);
    if (sqlite3_step( statement ) == SQLITE_ROW) {
        hashed_password = (const char *)sqlite3_column_text(statement, 0);
    }
    sqlite3_finalize( statement );
    db_close();
    if(crypto_pwhash_str_verify(hashed_password, password, strlen(password)) == 0){
        return 1;
    }
    return 0;
}

/*
 * Retrieve the airmiles the current user has.
 * Create a connection to the database, send a prepared statement and retrieve results.
 * Close the database connection and return the miles a user has.
 */

int getMiles(char *user){
    db_connect();
    int miles = 0;
    const char *query = "SELECT inhollandMiles FROM users WHERE username=?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_text(statement, 1, user, (int)strlen(user), SQLITE_STATIC);
    if(sqlite3_step(statement) == SQLITE_ROW)  {
        miles = sqlite3_column_int(statement, 0);
    }
    sqlite3_finalize( statement );
    db_close();
    return miles;
}

char changePassword(char *user, char *password){
    db_connect();
    char success = 0;
    char *hashedPass = argon2(password);
    const char *query = "UPDATE users SET password=? WHERE username=?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_text(statement, 1, hashedPass, (int)strlen(hashedPass), SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, user, (int)strlen(user), SQLITE_STATIC);
    if (sqlite3_step( statement ) == SQLITE_DONE){
        success = 1;
    }
    sqlite3_finalize(statement);
    db_close();
    return success;
}

char changeUser(char *user, char *password, char *miles, char *admin){
    db_connect();
    char success = 0;
    int adminValue = (admin == NULL) ? 0 : 1;
    fprintf(stderr, "ADMIN %i", adminValue);
    if(strcmp(password, "") != 0){
        char *hashedPass = argon2(password);
        const char *query = "UPDATE users SET password=?, inhollandMiles=?, admin=? WHERE username=?;";
        sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
        sqlite3_bind_text(statement, 1, hashedPass, (int)strlen(hashedPass), SQLITE_STATIC);
        sqlite3_bind_text(statement, 2, miles, (int)strlen(miles), SQLITE_STATIC);
        sqlite3_bind_int(statement, 3, adminValue);
        sqlite3_bind_text(statement, 4, user, (int)strlen(user), SQLITE_STATIC);
    } else{
        const char *query = "UPDATE users SET inhollandMiles=?, admin=? WHERE username=?;";
        sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
        sqlite3_bind_text(statement, 1, miles, (int)strlen(miles), SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, adminValue);
        sqlite3_bind_text(statement, 3, user, (int)strlen(user), SQLITE_STATIC);
    }
    if (sqlite3_step( statement ) == SQLITE_DONE){
        success = 1;
    }
    sqlite3_finalize(statement);
    db_close();
    return success;
}

/*
 * Function used to easily cast an integer value to a string.
 *
 * Allocate enough data to encapsulate the integer value.
 * Print to test if the cast went succesfully.
 * Return the string.
 */
char * intToString(int number){
    char *string = malloc(sizeof(int) * 2);
    sprintf(string, "%d", number);
    return string;
}

int countTableRows(char *tablename){
    db_connect();
    char query[60];
    int count = 0;
    sprintf(query, "SELECT COUNT(*) FROM %s;", tablename);
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    if (sqlite3_step( statement ) == SQLITE_ROW){
        count = sqlite3_column_int(statement, 0);
    }
    sqlite3_finalize(statement);
    db_close();
    return count;
}

char registerUser(char *user, char *password){
    char success = 0;
    char *hashedPass = argon2(password);
    const char *query = "INSERT INTO users (username, password, inhollandMiles) VALUES (?,?,500);";
    db_connect();
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_text(statement, 1, user, (int)strlen(user), SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, hashedPass, (int)strlen(hashedPass), SQLITE_STATIC);
    if (sqlite3_step( statement ) == SQLITE_DONE){
        success = 1;
    }
    sqlite3_finalize(statement);
    db_close();
    return success;
}

onion_dict * getFlights(int user_id){
    db_connect();
    onion_dict *flights = onion_dict_new();
    const char *query = "SELECT * from flights WHERE flights.id NOT IN (SELECT flight_id FROM flight_orders WHERE user_id == ?);";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_int(statement, 1, user_id);
    int i = 0;
    while (sqlite3_step( statement ) == SQLITE_ROW){
        onion_dict *flight = onion_dict_new();
        onion_dict_add(flight, "id",sqlite3_column_text(statement, 0),OD_DUP_VALUE);
        onion_dict_add(flight, "flight_name",sqlite3_column_text(statement, 1),OD_DUP_VALUE);
        onion_dict_add(flight, "from",sqlite3_column_text(statement, 2),OD_DUP_VALUE);
        onion_dict_add(flight, "to",sqlite3_column_text(statement, 3),OD_DUP_VALUE);
        onion_dict_add(flight, "price",sqlite3_column_text(statement, 4),OD_DUP_VALUE);
        onion_dict_add(flight, "date",sqlite3_column_text(statement, 5),OD_DUP_VALUE);
        onion_dict_add(flights, intToString(i), flight, OD_DUP_ALL|OD_DICT|OD_FREE_VALUE);
        i++;
    }
    sqlite3_finalize(statement);
    db_close();
    return flights;
}

onion_dict * getUserInfo(char * username){
    db_connect();
    onion_dict *user = onion_dict_new();
    int success = 0;
    const char *query = "SELECT id, username, inhollandMiles, admin FROM users WHERE username=?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_text(statement, 1, username, (int)strlen(username), SQLITE_STATIC);
    if (sqlite3_step( statement ) == SQLITE_ROW){
        onion_dict_add(user, "id",sqlite3_column_text(statement, 0),OD_DUP_VALUE);
        onion_dict_add(user, "username",sqlite3_column_text(statement, 1),OD_DUP_VALUE);
        onion_dict_add(user, "miles",sqlite3_column_text(statement, 2),OD_DUP_VALUE);
        onion_dict_add(user, "admin",sqlite3_column_text(statement, 3),OD_DUP_VALUE);
        success = 1;
    }
    sqlite3_finalize(statement);
    db_close();
    if(success){
        return user;
    }
    return NULL;
}

onion_dict * getUsers(){
    db_connect();
    onion_dict *users = onion_dict_new();
    const char *query = "SELECT id, username, inhollandMiles, admin FROM users;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    int i = 0;
    while (sqlite3_step( statement ) == SQLITE_ROW){
        onion_dict *user = onion_dict_new();
        onion_dict_add(user, "id",sqlite3_column_text(statement, 0),OD_DUP_VALUE);
        onion_dict_add(user, "username",sqlite3_column_text(statement, 1),OD_DUP_VALUE);
        onion_dict_add(user, "miles",sqlite3_column_text(statement, 2),OD_DUP_VALUE);
        onion_dict_add(user, "admin",sqlite3_column_text(statement, 3),OD_DUP_VALUE);
        onion_dict_add(users, intToString(i), user, OD_DUP_KEY|OD_DICT|OD_FREE_ALL);
        i++;
    }
    sqlite3_finalize(statement);
    db_close();
    return users;
}

onion_dict * getOrders(int user_id){
    db_connect();
    onion_dict *orders = onion_dict_new();
    const char *query = "SELECT * from flights WHERE flights.id IN (SELECT flight_id FROM flight_orders WHERE user_id == ?);";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_int(statement, 1, user_id);
    int i = 0;
    while (sqlite3_step( statement ) == SQLITE_ROW){
        onion_dict *flight = onion_dict_new();
        onion_dict_add(flight, "id",sqlite3_column_text(statement, 0),OD_DUP_VALUE);
        onion_dict_add(flight, "flight_name",sqlite3_column_text(statement, 1),OD_DUP_VALUE);
        onion_dict_add(flight, "from",sqlite3_column_text(statement, 2),OD_DUP_VALUE);
        onion_dict_add(flight, "to",sqlite3_column_text(statement, 3),OD_DUP_VALUE);
        onion_dict_add(flight, "price",sqlite3_column_text(statement, 4),OD_DUP_VALUE);
        onion_dict_add(flight, "date",sqlite3_column_text(statement, 5),OD_DUP_VALUE);
        onion_dict_add(orders, intToString(i), flight, OD_DUP_ALL|OD_DICT|OD_FREE_VALUE);
        i++;
    }
    sqlite3_finalize(statement);
    db_close();
    return orders;
}

char buyFlight(onion_dict *user, int flight_id){
    db_connect();
    const char *query;
    char code;
    int user_id = atoi(onion_dict_get(user, "id"));
    int user_miles = atoi(onion_dict_get(user,"miles"));
    // Get flight
    query = "SELECT * FROM flights WHERE id == ?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_int(statement, 1, flight_id);
    if (sqlite3_step( statement ) == SQLITE_ROW){
        int neededMiles = sqlite3_column_int(statement, 4);
        if(user_miles >= neededMiles){
            sqlite3_finalize(statement);
            query = "INSERT INTO flight_orders (flight_id, user_id, status) VALUES(?,?, 'BOOKED');";
            sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
            sqlite3_bind_int(statement, 1, flight_id);
            sqlite3_bind_int(statement, 2, user_id);
            if (sqlite3_step( statement ) == SQLITE_DONE){
                // Write off miles
                sqlite3_finalize(statement);
                query = "UPDATE users SET inhollandMiles = ? WHERE id = ?";
                sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
                sqlite3_bind_int(statement, 1, (user_miles - neededMiles));
                sqlite3_bind_int(statement, 2, user_id);
                if(sqlite3_step(statement) == SQLITE_DONE){
                    //Success code
                    code = 1;
                } else{
                    //Database error
                    code = 4;
                    goto error;
                }
            } else {
                //Database error
                code = 4;
                goto error;
            }

        } else {
            //Not enough money
            code = 3;
            goto error;
        }
    } else{
        //No flight found by this id
        code = 2;
        goto error;
    }
    error:
    sqlite3_finalize(statement);
    db_close();
    return code;
}

char deleteOrder(int user_id, int flight_id){
    db_connect();
    char success = 0;
    char * query = "DELETE FROM flight_orders WHERE flight_id = ? AND user_id = ?;";
    sqlite3_prepare_v2(db, query, (int)strlen(query), &statement, NULL);
    sqlite3_bind_int(statement, 1, flight_id);
    sqlite3_bind_int(statement, 2, user_id);
    if (sqlite3_step( statement ) == SQLITE_DONE){
        success = 1;
    }
    sqlite3_finalize(statement);
    db_close();
    return success;
}