/**
 * @file resourceManager.h
 *
 * @brief header file for Resource class
 *
 * @details handles the resources for the different IO devices
 */
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "config.h"
#include <deque>

using namespace std;

class Resource
{
public:
    deque<int> harddrive;
    deque<int> keyboard;
    deque<int> monitor;
    deque<int> printer;
    deque<int> mouse;
    void resourceStart(Config &config);
    void resourceManagement(string descriptor);
private:

};

#endif // !RESOURCEMANAGER_H

