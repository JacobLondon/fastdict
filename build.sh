#!/bin/bash

#python3 def2.py -f data_structures.json -o data_structures
python3 mkdict.py -f base --output data_structures

clang++ -o dictperf -O3 -DNDEBUG dictperf.cpp data_structures.c
