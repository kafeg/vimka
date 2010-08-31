#!/bin/bash

rm -rf ./bin/data
rm -rf ./debian
cp -R ../trunk/resources/data ./bin/data
cp -R ../trunk/resources/debian ./debian
cp ../trunk/resources/convert.sh ./convert.sh
./convert.sh
rm ./convert.sh