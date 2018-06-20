# Base image
FROM ubuntu:18.04
MAINTAINER Erwin Castricum

# Open ports
EXPOSE 8080

ADD . /root/secureweb
RUN apt-get update && apt-get install -yqq build-essential unzip wget cmake libsqlite3-dev libgnutls28-dev libgcrypt20-dev libpthread-stubs0-dev libsodium-dev && \
    cd /root && wget "https://github.com/davidmoreno/onion/archive/master.zip" && unzip master.zip && cd onion-master && mkdir build && cd build && cmake .. && make && make install && ldconfig && cd /root && \
    rm -R onion-master && rm master.zip && cd secureweb && sh buildtools/generate_cert.sh && mkdir build && cd build && cmake .. && make
WORKDIR /root/secureweb/build
CMD ["./Secure_Programming"]





