#!/bin/bash
#Cd to script directory a.k.a source directory
cd "$(dirname "$0")"
pwd

BUILD_FOLDER="build"
PACKAGE_LIST="packages.txt"
COLOR_GREEN='\033[0;32m'
COLOR_BLUE='\033[0;34m'
COLOR_RED='\033[0;31m'
COLOR_RESET='\033[0m'

# Make sure only non root can run our script
if [ "$(id -u)" -eq 0 ]; then
        echo "${COLOR_RED}Don't run this script as root. It will ask for permissions when needed ${COLOR_RESET}" >&2
        exit 1
fi

#Update aptitude
echo "${COLOR_BLUE}Updating aptitude packagelist ${COLOR_RESET}"
sudo apt-get update -qq

#Install packages from packages.txt
echo "${COLOR_BLUE}Installing packages from packages.txt ${COLOR_RESET}"
sudo apt-get install -yqq $(grep -v '^#' packages.txt)

#Install onion
if [ -d "onion" ]; then
  "${COLOR_RED}Onion folder already found, removing and rebuilding it ${COLOR_RESET}"
  rm -R onion
fi
echo "${COLOR_BLUE}Cloning libonion github repository ${COLOR_RESET}"
git clone "https://github.com/davidmoreno/onion.git"
cd onion && mkdir build && cd build
echo "${COLOR_BLUE}Making and installing libonion ${COLOR_RESET}"
cmake .. || exit 1 && make || exit 1 && sudo make install || exit 1
echo "${COLOR_BLUE}Updating library cache and cd'ing back to project root ${COLOR_RESET}"
sudo ldconfig && cd ../..

#Cleanup onion
echo "${COLOR_BLUE}Back in project root. Deleting onion folder ${COLOR_RESET}"
sudo rm -R onion

#Generate certificates
echo "${COLOR_BLUE}Generating https certificates ${COLOR_RESET}"
sh buildtools/generate_cert.sh

#Build otemplates
echo "${COLOR_BLUE}Converting otemplate html files into c files ${COLOR_RESET}"
sh buildtools/otemplate.sh

#Install project
if [ -d "build" ]; then
  echo "${COLOR_RED}Project build folder already found, removing and rebuilding it ${COLOR_RESET}"
  rm -R build
fi

mkdir build && cd build
echo "${COLOR_BLUE}Building project ${COLOR_RESET}"
cmake .. -DCMAKE_C_FLAGS=\"-Werror\" -DCMAKE_CXX_FLAGS=\"-Werror\" || exit 1
make || exit 1

echo "${COLOR_GREEN}Succesfully installed the Secureweb application. Start the server using ./build/Secure_Programming${COLOR_RESET}"