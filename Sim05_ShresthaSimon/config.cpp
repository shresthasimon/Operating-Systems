/**
 * @file Config.cpp
 *
 * @brief Implementation file for Config Class
 *
 * @details Implements functions from Config.h
 *
 * @note Requires Config.h
 */
#include "config.h"
using namespace std;

//setter functions
void Config::setPrinterResources(string printer)
{
	printerResources = stoi(printer);
}

void Config::setHarddriveResources(string harddrive)
{
	harddriveResources = stoi(harddrive);
}

void Config::setKeyboardResources(string keyboard)
{
	keyboardResources = stoi(keyboard);
}

void Config::setMonitorResources(string monitor)
{
	monitorResources = stoi(monitor);
}

void Config::setMouseResources(string mouse)
{
	mouseResources = stoi(mouse);
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
                version = rightSide; //convert to float abd assign

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
                    metadataFilePath = rightSide; //if the ending is correct then assign
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
                monitorDisplayTime = stoi(rightSide);
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
                logSetting = rightSide;
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
        }else if(leftSide == "System memory {kbytes}" || leftSide == "System memory {Mbytes}" || leftSide == "System memory {Gbytes}" ) {
            try{
                systemMemoryType = leftSide;
                if(systemMemoryType.find("kbytes") != string::npos){
                    systemMemorySize = stoi(rightSide);
                }else if(systemMemoryType.find("Mbytes") != string::npos){
                    systemMemorySize = stoi(rightSide) * 1000;
                }else if(systemMemoryType.find("Gbytes") != string::npos){
                    systemMemorySize = stoi(rightSide) * 1000000;
                }
            }
            catch (int e) {
                cerr << "Error! Incorrect System Memory Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Memory block size {kbytes}" || leftSide == "Memory block size {Mbytes}" || leftSide == "Memory block size {Gbytes}"){
            try{
                memoryBlockType = leftSide;
                if(memoryBlockType.find("kbytes") != string::npos){
                    memoryBlockSize = stoi(rightSide);
                }else if(memoryBlockType.find("Mbytes") != string::npos){
                    memoryBlockSize = stoi(rightSide) * 1000;
                }else if(memoryBlockType.find("Gbytes") != string::npos){
                    memoryBlockSize = stoi(rightSide) * 1000000;
                }
            }
            catch (int e) {
                cerr << "Error! Incorrect Memory block size Format." << endl;
                exit(0);
            }
        }else if(leftSide == "Printer quantity"){
            try{
                printerResources = stoi(rightSide);
            }
            catch(int e){
                cerr << "Error! Incorrect printer resource format." << endl;
                exit(0);
            }
        }else if(leftSide == "Hard drive quantity"){
            try{
                harddriveResources = stoi(rightSide);
            }
            catch(int e){
                cerr << "Error! Incorrect hard drive resource format." << endl;
                exit(0);
            }
        }else if(leftSide == "Keyboard quantity") {
            try {
                keyboardResources = stoi(rightSide);
            }
            catch (int e) {
                cerr << "Error! Incorrect keyboard resource format." << endl;
                exit(0);
            }
        }else if(leftSide == "Monitor quantity") {
            try {
                monitorResources = stoi(rightSide);
            }
            catch (int e) {
                cerr << "Error! Incorrect monitor resource format." << endl;
                exit(0);
            }
        }else if(leftSide == "Mouse quantity") {
            try {
                mouseResources = stoi(rightSide);
            }
            catch (int e) {
                cerr << "Error! Incorrect mouse resource format." << endl;
                exit(0);
            }
        }else if(leftSide == "CPU Scheduling Code"){
            try{
                schedulingCode = rightSide;
            }
            catch (int e){
                cerr << "Error! Incorrect Scheduling Algo format." << endl;
                exit(0);
            }
        }else if(leftSide == "Processor Quantum Number {msec}"){
            try{
                quantumNumber = stoi(rightSide);
            }
            catch (int e){
                cerr << "Error! Incorrect quantum num format" << endl;
                exit(0);
            }
        }else{
            cerr << "Error! Config file not formatted correctly." << endl;
            exit(0);
        }
    }

    configFile.close();
    //chchking for if output file is in config file
    if((logSetting == "Log to Both" || logSetting == "Log to File") && !(seenLogFile)){
        cerr << "Error! Missing input for log option" << endl;
        exit(0);
    }
}
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
//trim function
void Config::trim(string& str){
    str.erase(0, str.find_first_not_of(" ")); // erase empty space
}

//getter functions
int Config::getPrinterTime() const {
    return printerCycleTime;
}

int Config::getProcessorTime() const {
    return processorCycleTime;
}

int Config::getKeyboardTime() const {
    return keyboardCycleTime;
}

int Config::getMonitorTime() const {
    return monitorDisplayTime;
}

int Config::getMouseTime() const {
    return mouseCycleTime;
}

int Config::getHarddriveTime() const {
    return harddriveCycleTime;
}

int Config::getMemoryTime() const {
    return memoryCycleTime;
}

int Config::getSystemMemorySize() const {
    return systemMemorySize;
}

int Config::getPrinterResources() const {
    return printerResources;
}

int Config::getHarddriveResources() const {
    return harddriveResources;
}

int Config::getKeyboardResources() const {
    return keyboardResources;
}

int Config::getMonitorResources() const {
    return monitorResources;
}

int Config::getMouseResources() const {
    return mouseResources;
}

int Config::getBlockSize() const {
    return memoryBlockSize;
}

int Config::getCPUQuantumNumber() const {
    return quantumNumber;
}

string Config::getVersion() const {
    return version;
}

string Config::getMetadataFile() const {
    return metadataFilePath;
}

string Config::getLog() const {
    return logSetting;
}

string Config::getLogFile() const {
    return logFilePath;
}

string Config::getConfigFile() const {
    return configFilePath;
}

string Config::getSystemMemoryType() const {
    return systemMemoryType;
}

string Config::getBlockType() const {
    return memoryBlockType;
}

string Config::getCPUSchedulingAlgorithm() const {
    return schedulingCode;
}
