/**
 * @file Config.h
 *
 * @brief Header file for Config Class
 *
 * @details used to store information read from configuration file
 *
 *
 */
// Precompiler Directives
//
#ifndef CS446PROJ1_CONFIG_H
#define CS446PROJ1_CONFIG_H
//
// Header Files
//
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
//
// Class Definition
//
class Config {
public:
    // Constructor
    Config();
    //Destructor
    virtual ~Config();
    //Getter methods private variables
    float getVersionNumber() const;
    int getProjectorCycleTime() const;
    int getProcessorCycleTime() const;
    int getKeyboardCycleTime() const;
    int getMoniterDisplayTime() const;
    int getScannerCycleTime() const;
    int getHarddriveCycleTime() const;
    int getPrinterCycleTime() const;
    int getMemoryCycleTime() const;
    int getMouseCycleTime() const;
    int getMemory() const;
    int getPrinterResources() const;
    int getHarddriveResources() const;
    int getMemBlockSize() const;
    const string &getFilePath() const;
    const string &getLogOption() const;
    const string &getLogFilePath() const;
    // Function to Parse through configuration file
    void readConfigFile(string configFilePath);
    // Print Configuration file
    void printConfigFile();
    // split function
    void splitString(string, char, string&, string&);
    //trim function
    void trim(string&);



private:
    float versionNumber;
    int projectorCycleTime;
    int processorCycleTime;
    int keyboardCycleTime;
    int moniterDisplayTime;
    int scannerCycleTime;
    int harddriveCycleTime;
    int printerCycleTime;
    int memoryCycleTime;
    int mouseCycleTime;
    int memory;

    int printerResources;
    int harddriveResources;
    int memBlockSize;
    string filePath;
    string logOption;
    string logFilePath;
    string memoryType;


};
//
// Terminating Precompiler Directive
#endif
//
