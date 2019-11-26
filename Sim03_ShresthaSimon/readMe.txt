Simon Shrestha CS446 Project 3

Note: The design of this project was discusses with Erik Duong

To run:
-------
1. make sure that all files are in the same folder
2. type make
3. ./Sim03 config_3.conf
4. you can also type
    make clean && make && ./Sim03 config_3.conf

main.cpp
---------------------------------------------
Line 61-65 declaring semaphore objects
Line 69-73 declaring mutex objects
Line 221 semaphore wait is called
Line 222 mutex lock is called
Line 224 mutex unlock is called
Line 226 semaphore post is called
Line 254-258 mutex objects are initialized
Line 260-264 semaphore objects are initialized

