/**
 * @author Simon Shrestha
 *
 * @file log.h
 *
 * @brief output class header
 *
 * @details outputs to file, or monitor, or both
 *
 */

#ifndef CS446PROJ4_LOG_H
#define CS446PROJ4_LOG_H

#include <iostream>
#include <string>
#include <fstream>
#include "Config.h"
using namespace std;
class Log {
    private:
        //function to check if already logged
        static bool didFirstLog;
        static bool checkFirstLog();

    public:
        static void outputToStream(ostream&, string);
        static void output(Config, string);

};


#endif //CS446PROJ4_LOG_H
