//
// Created by erwin on 15-5-18.
//

#include "../include/encryption.h"

/*
 * Function used to hash passwords
 * The function is based on the argon2 encryption.
 * Currently the most secure encryption method used in web development.
 *
 * Create a char array with enough size to encapsulate the encrypted password.
 * Call the hash function, send the char array address and the password to hash.
 * Return the hashed password.
 */

char * argon2(char string[]){
    char *hashed_password;
    hashed_password = malloc(sizeof(char) * crypto_pwhash_argon2id_STRBYTES);
    if(crypto_pwhash_str(hashed_password, string, strlen(string), crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0){
        fprintf(stderr, "Crypto password hash out of memory!");
    };
    return hashed_password;
}