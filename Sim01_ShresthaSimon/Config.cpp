/**
 * @file Config.cpp
 *
 * @brief Implementation file for Config Class
 *
 * @details Implements functions from Config.h
 *
 * @note Requires Config.h
 */
//
// Header Files
#include "Config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
//
// Constructor function
Config::Config() {

}
//
// Destructor Function
Config::~Config() {

}
//
/**
 * @brief Parses through configuration file and assigns values rea to variables
 *
 * @pre string configFilePath must be a .conf file
 *
 * @param configFilePath
 *        the configuration file input
 *
 */
void Config::readConfigFile(string configFilePath) {
    // open input file
    ifstream configFile;
    configFile.open(configFilePath);
    // flag used to check if output file was entered
    bool seenLogFile = false;

    // outputs error if file not found in the same directory
    if(!configFile){
        cerr << "Error! Can't Find File" << endl;
        exit(1);
    }

    // vector hold entire file input and to parse through
    vector<string>iterator;
    //string to hold words being read in
    string mark;

    // inputs everything from file into vector line by line
    while(getline(configFile, mark)){
        iterator.push_back(mark);
    }
    //if there is only the first and last line
    if(iterator.size() < 2){
        cerr << "Error! Missing lines in Configuration File." << endl;
        exit(0);
    }
    // if the starting line does not exist
    if(iterator.front() != "Start Simulator Configuration File"){
        cerr << "Error! Configuration file is formatted incorrectly." << endl;
        exit(0);
    }
    // if ending line does not exist
    if(iterator.back() != "End Simulator Configuration File"){
        cerr << "Error! Configuration file is formatted incorrectly." << endl;
        exit(0);
    }
    // deleting first and last line as they are not needed to reading
    iterator.erase(iterator.begin());
    iterator.erase(iterator.end());
    //character check to destinguish between left and right side
    // left side will be everything before : and right side will be everything after
    char delimiter = ':';


    //iterates through entire vector and assigns values to variables
    for(auto pnt = iterator.begin(); pnt != iterator.end(); pnt++){
        string leftSide;
        string rightSide;
        // splits line into left and right side
        splitString(*pnt, delimiter, leftSide, rightSide);
        // trims right side so that there is no space before it
        trim(rightSide);
        // assignment portion
        if(leftSide == "Version/Phase"){
            try{
                versionNumber = stof(rightSide); //convert to float abd assign
            }
            catch (int e){
                cerr << "Error! Incorrect Version/Phase Format." << endl;
                exit(0);
            }
        }else if(leftSide == "File Path"){
            try{
                string ending = rightSide; //create copy
                ending = ending.erase(0, ending.size()-4); //leave only .mdf portion
                if(ending == ".mdf"){
                    filePath = rightSide; //if the ending is correct then assign
                }else {
                    cerr << "Error! Missing .mdf" << endl;
                    exit(0);
                }
            }
            catch (int e){
                cerr << "Error! Incorrect File Path Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Monitor display time {msec}"){
            try{
                moniterDisplayTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Monitor Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Processor cycle time {msec}"){
            try{
                processorCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Processor Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Mouse cycle time {msec}"){
            try{
                mouseCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Mouse Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Hard drive cycle time {msec}"){
            try{
                harddriveCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect hard drive Format." << endl;
                exit(0);
            }

        }else if(leftSide == "Keyboard cycle time {msec}"){
            try{
                keyboardCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Keyboard Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Memory cycle time {msec}"){
            try{
                memoryCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Memory Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Printer cycle time {msec}"){
            try{
                printerCycleTime = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect Printer Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Log"){
            try{
                logOption = rightSide;
            }
            catch (int e){
                cerr << "Error! Incorrect Log Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Log File Path"){
            try{
                string ending = rightSide;
                ending = ending.erase(0, ending.size()-4);
                if(ending == ".lgf"){
                    logFilePath = rightSide;
                }else {
                    cerr << "Error! Missing .lgf" << endl;
                    exit(0);
                }
                seenLogFile = true;
            }
            catch (int e){
                cerr << "Error! Incorrect Log File Path Format." << endl;
                exit(0);
            }
        }else{
            cerr << "Error! Config file not formatted correctly." << endl;
            exit(0);
        }
    }

    configFile.close();
    //chchking for if output file is in config file
    if((logOption == "Log to Both" || logOption == "Log to File") && !(seenLogFile)){
        cerr << "Error! Missing input for log option" << endl;
        exit(0);
    }
}
//
//splitting function
void Config::splitString(string in, char delimiter, string& left, string& right) {
    unsigned int pos; //position within string
    bool delimFound = false; // flag to see if delimiter has been reached
    for(pos = 0; pos < in.size(); pos++){
        if(in[pos]==delimiter){
            delimFound=true; //when found, set flag and skip the delimiter character
            continue;
        }
        if(!delimFound) {
            left.append(1, in[pos]); //append left while not finding delimiter
        }
        else{
            right.append(1, in[pos]); //append right after delimiter is found
        }

    }
}
//
// Print Config File
void Config::printConfigFile() {
    cout << endl;
    cout << "Configuration File Data" << endl;
    cout << "Version: " << versionNumber << endl;
    cout << "Monitor = " << moniterDisplayTime << endl;
    cout << "Processor = " << processorCycleTime << endl;
    cout << "Mouse = " << mouseCycleTime << endl;
    cout << "Hard Drive = " << harddriveCycleTime << endl;
    cout << "Keyboard = " << keyboardCycleTime << endl;
    cout << "Memory = " << memoryCycleTime << endl;
    cout << "Printer = " << printerCycleTime << endl;
    cout << "Logged to: " << logOption << endl;


}
//
// Get methods for all private variables
float Config::getVersionNumber() const {
    return versionNumber;
}
//
int Config::getProjectorCycleTime() const {
    return projectorCycleTime;
}
//
int Config::getProcessorCycleTime() const {
    return processorCycleTime;
}
//
int Config::getKeyboardCycleTime() const {
    return keyboardCycleTime;
}
//
int Config::getMoniterDisplayTime() const {
    return moniterDisplayTime;
}
//
int Config::getScannerCycleTime() const {
    return scannerCycleTime;
}
//
int Config::getHarddriveCycleTime() const {
    return harddriveCycleTime;
}
//
int Config::getPrinterCycleTime() const {
    return printerCycleTime;
}
//
int Config::getMemoryCycleTime() const {
    return memoryCycleTime;
}
//
int Config::getMouseCycleTime() const {
    return mouseCycleTime;
}
//
const string &Config::getFilePath() const {
    return filePath;
}
//
const string &Config::getLogOption() const {
    return logOption;
}
//
const string &Config::getLogFilePath() const {
    return logFilePath;
}
//
// Trim function
void Config::trim(string& str){
    str.erase(0, str.find_first_not_of(" ")); // erase empty space
}
//



