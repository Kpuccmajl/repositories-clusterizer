#!/bin/bash

sloccount $1 &> tmp
cat tmp | grep ".*:.*%)$"
rm tmp
