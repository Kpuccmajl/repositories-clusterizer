#!/bin/bash

sloccount $1 &> tmp
cat tmp | grep "Total Physical Source Lines"
rm tmp
