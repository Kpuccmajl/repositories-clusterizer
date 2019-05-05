#/bin/bash

cd $1
git log --pretty=%ct -1
