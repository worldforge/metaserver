#!/bin/sh
echo "Launching 10 test servers..."

./testserver 127.0.0.1 &
./testserver 127.0.1.1 &
./testserver 127.0.2.1 &
./testserver 127.0.3.1 &
./testserver 127.0.4.1 &
./testserver 127.0.5.1 &
./testserver 127.0.6.1 &
./testserver 127.0.7.1 &
./testserver 127.0.8.1 &
./testserver 127.0.9.1 &
