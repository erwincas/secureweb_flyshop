#!/bin/bash

#Cd to script directory a.k.a source directory
cd "$(dirname "$0")"

templates_folder=../templates
c_code_folder=../src/templates
asset_file=../src/include/assets.h

#First refresh the template folder by deleting the c_code folder and the assets file
rm -Rf "$c_code_folder"
rm -f "$asset_file"
mkdir "$c_code_folder"

#Loop over all template files and make the c file
for f in $templates_folder/*;
do
    if [ ! -d "$f" ]; then #If not a folder
        filename=$(basename -- "${f%.*}")
        otemplate $f $c_code_folder/$filename"_html.c" -a $asset_file
    fi
done

#Cd back to previous folder
cd ..