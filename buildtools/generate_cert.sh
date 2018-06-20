#!/bin/bash

#Cd to script directory a.k.a source directory
cd "$(dirname "$0")"

rm -R ../cert
mkdir ../cert

openssl req -x509 -out ../cert/localhost.crt -keyout ../cert/localhost.key -newkey rsa:2048 -nodes -days 365 -sha256 -subj '/CN=localhost/O=AirInholland LTD./C=NL'