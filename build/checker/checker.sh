#!/bin/bash

cd "$(dirname "$0")" || exit 1
cd ..

if [[ "$1" == "--with-clean" ]]; then
    make clean
    if [ $? != 0 ]; then
        echo "Make clean failed, bailing out..." >&2
        exit 1
    fi
fi

make
if [ $? != 0 ]; then
    echo "Make failed, bailing out..." >&2
    exit 1
fi

sudo fuser -k 6653/tcp
sudo python3 checker/topo.py tests
