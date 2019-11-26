/**
 * @author Simon Shrestha
 *
 * @file log.cpp
 *
 * @brief implementation of log class
 *
 * @details outputs to file, terminal or both
 *
 */
#include <iostream>
#include <string>
#include <fstream>

#include "Log.h"
#include "Config.h"
using namespace std;

//flag to check if the file is already logged
bool Log::didFirstLog = false;

//output function that knows where to output
void Log::output(Config conf, string logOutput) {
    ofstream log;

    bool moniterF = false; //flag to check whether to output to moniter
    bool fileF = false;    //flag to check whether to ouput to file

    if(conf.getLogOption() == "Log to Both"){
        moniterF = true;
        fileF = true;
    }else if( conf.getLogOption() == "Log to Monitor"){
        moniterF = true;
    }else if(conf.getLogOption() == "Log to File"){
        fileF = true;
    }else{
        cerr << "Error! Log to option is not one of the options avalaible." << endl;
        exit(0);
    }
    //ouput to monitor
    if(moniterF){
        Log::outputToStream(cout, logOutput);
    }
    //output to file
    if(fileF){
        log.open(conf.getLogFilePath(), (checkFirstLog()) ? ofstream::app : ofstream::trunc);

        Log::outputToStream(log, logOutput);
        log.close();
    }
}

// formatting of output
void Log::outputToStream(ostream& out, string logOutput) {
    out << logOutput << endl;
}

//function to check of the file is already logged
bool Log::checkFirstLog() {
    if(didFirstLog == false){
        didFirstLog = true;
        return false;
    }else{
        return didFirstLog;
    }
}