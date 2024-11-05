#!/bin/bash

ninja -C build

while [[ $? -eq 0 ]]; do
	clear
	./build/randomart
done
