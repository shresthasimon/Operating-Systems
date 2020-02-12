/**
 * @file Config.h
 *
 * @brief Header file for Config Class
 *
 * @details used to store information read from configuration file
 *
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <algorithm>

using namespace std;

class Config
{
public:
    //setter functions
    void setPrinterResources(string );
    void setHarddriveResources(string );
    void setKeyboardResources(string );
    void setMonitorResources(string );
    void setMouseResources(string );
    //getter functions
    int getPrinterTime() const;
    int getProcessorTime() const;
    int getKeyboardTime() const;
    int getMonitorTime() const;
    int getMouseTime() const;
    int getHarddriveTime() const;
    int getMemoryTime() const;
    int getSystemMemorySize() const;
    int getPrinterResources() const;
    int getHarddriveResources() const;
    int getKeyboardResources() const;
    int getMonitorResources() const;
    int getMouseResources() const;
    int getBlockSize() const;
    int getCPUQuantumNumber() const;
    string getVersion() const;
    string getMetadataFile() const;
    string getLog() const;
    string getLogFile() const;
    string getConfigFile() const;
    string getSystemMemoryType() const;
    string getBlockType() const;
    string getCPUSchedulingAlgorithm() const;
    //config file reader functions
    void readConfigFile(string configFilePath);
    void splitString(string, char, string&, string&);
    void trim(string&);

private:
    int printerCycleTime;
    int processorCycleTime;
	int keyboardCycleTime;
	int monitorDisplayTime;
	int mouseCycleTime;
	int harddriveCycleTime;
	int memoryCycleTime;
	int printerResources;
	int harddriveResources;
	int keyboardResources;
	int monitorResources;
	int mouseResources;
	int systemMemorySize;
	int memoryBlockSize;
	int quantumNumber;
	string version;
	string metadataFilePath;
	string systemMemoryType;
	string logSetting;
	string logFilePath;
	string configFilePath;
	string memoryBlockType;
	string schedulingCode;
};

#endif
