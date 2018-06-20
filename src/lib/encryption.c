//
// Created by erwin on 15-5-18.
//

#include "../include/encryption.h"

char * argon2(char string[]){
    char *hashed_password;
    hashed_password = malloc(sizeof(char) * crypto_pwhash_argon2id_STRBYTES);
    if(crypto_pwhash_str(hashed_password, string, strlen(string), crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0){
        fprintf(stderr, "Crypto password hash out of memory!");
    };
    return hashed_password;
}