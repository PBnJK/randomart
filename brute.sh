#!/bin/bash

# Simple brute-force script for testing the program for crashes.
# Careful not to get stuck watching the mesmerizing images!

ninja -C build

while [[ $? -eq 0 ]]; do
	clear
	./build/randomart
done
