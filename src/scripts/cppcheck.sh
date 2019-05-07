#!/bin/bash

cppcheck --enable=warning --enable=performance -q --force $1 &> tmp
cat tmp
rm tmp
