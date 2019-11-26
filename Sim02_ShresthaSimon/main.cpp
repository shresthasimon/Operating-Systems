/**
 * @author Simon Shrestha
 *
 * @file main.cpp
 *
 * @brief Driver program
 *
 * @details reads in metadata file and configuration file, formates, calculates, and outputs
 *
 * @note requires Config.h and MetaData.h
 */
//
// Header Files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <pthread.h>
#include <sstream>
#include <iomanip>

#include "Config.h"
#include "MetaData.h"
using namespace std;
using namespace std::chrono;
//
// Function Definitions//////////////////////////////////////////////////////////////
bool returnInstructions(string line, vector<string>& mdVector);
bool parseInstruction(string instr, string& left, string& middle, string& right);
void readMetaFile(string filePath, vector<MetaData>& mdVector);
void timeCalculation(Config conf, MetaData& metaData, int& sysStatus, int& appStatus);
void outputMessage(Config conf, vector<MetaData> mdVector, ostream& fout, int logOption);
void outputOptions(Config conf, vector<MetaData> mdVector);
void lTrim(string& str);
void rTrim(string& str);
void Trim(string& str);
string allocateMemory(int);
void* timer(void *);
double threadProcessing(int);
void processRunner(int, MetaData&);
//
// Process Control Block Struct
struct PCB{
    int processState;
};
//
// Different states for a process
//
const int START = 0;
const int READY = 1;
const int RUNNING = 2;
const int WAIT = 3;
const int EXIT = 4;
//
// PCB object
PCB ps;
// Start time const variable
const auto START_TIME = chrono::system_clock::now();
//static int TIMER = 0;
//
// Vector to store all output values
static vector<string> logFile;
//
//
// Main Function Implementation /////////////////////////////////////////////////////////////
//
int main(int argc, char* argv[]) {

    ps.processState = EXIT;
    // reads config file as first argc argument
    string configFile = argv[1];
    Config* conf = new Config();
    vector<MetaData> mdVector;
    int sysStatus = 0; //check if S{begin} and S{finish} has been read
    int appStatus = 0; //check if A{begin} and A{finish} has been read

    // outputs error if ./Sim01 is run with no other argument
    if(argc != 2){
        cerr << "Error! You must use a .conf file as an input for this application." << endl;
        return 0;
    }

    // Only accepts .conf files
    if(configFile.substr(configFile.find_first_of(".") + 1) != "conf") {
        cerr << "Error! Only a .conf file is applicable as the argument." << endl;
        return 0;
    }

    ifstream fin;
    ofstream fout;

    // read in config file propagate correctly to variables
    conf->readConfigFile(configFile);

    // checks whether config file is empty
    fin.clear();
    fin.open(argv[1]);
    if(fin.peek() == ifstream::traits_type::eof()){
        cerr << "Error! Empty Configuration File." << endl;
        return 0;
    }
    fin.close();

    //checks whether meta data file is empty or if a .mdf file is not entered in config file
    fin.clear();
    fin.open(conf->getFilePath());
    if(fin.peek() == ifstream::traits_type::eof()){
        cerr << "Error! Empty Meta Data File or .mdf file not entered in config file" << endl;
        exit(0);
    }

    fin.close();

    // reads in metadata file into metadata vector
    readMetaFile(conf->getFilePath(), mdVector);

    // goes through the vector and calculates the process times
    for(auto& pnt : mdVector){
        timeCalculation(*conf, pnt, sysStatus, appStatus);
    }

    // outputs results depending on choice in config file
    outputOptions(*conf, mdVector);

    return 0;
}
/**
 * @name timer
 *
 * @details timer thread for each process. Calculates duration for each process.
 *
 * @param process
 *        process to calculate time for
 *
 */
void* timer(void *process){
    // creating start and end points
    chrono::system_clock::time_point start;
    chrono::system_clock::time_point end;
    // Setting a time limit
    double timeLimit = 0.1;
    // time spent variable
    chrono::duration<double> timeSpent;
    //starting the timer
    start = chrono::system_clock::now();
    // while the time limit is not reached, calculate time
    while(timeSpent.count() < timeLimit){
        end = chrono::system_clock::now();
        timeSpent = end - start;
    }
    // delete thread
    pthread_exit(NULL);
    return NULL;
}
/**
 * @name threadProcessing
 *
 * @description creates threads for timer
 *
 * @param timeLimit
 *        The max length a process is allowed to take
 *
 * @return
 */
double threadProcessing(int timeLimit){
//    int time = timeLimit;
    // timerThread variable to track time
    pthread_t timerThread;

    //the time limit that a process can reach
    auto timeEnd = chrono::system_clock::now() + chrono::milliseconds(timeLimit);

    //until the max thread is reached, create a new timer thread and add it to the main
    while(chrono::system_clock::now() < timeEnd){
        pthread_create(&timerThread, NULL, timer, NULL);
        pthread_join(timerThread, NULL);
    }
    // current time of the thread
    auto currentTime = chrono::system_clock::now();
    // calculating the amount of time a process has taken
    return chrono::duration<double>(currentTime - START_TIME).count();
}
/**
 * @name processRunner
 *
 * @description runs each individual I/O process
 *
 * @param millisec
 *        max time limit for an i/o process
 *
 * @param metaData
 *        meta data object to access time variables in the class
 */
void processRunner(int millisec , MetaData& metaData){

    //start with the current time of the entire program
    auto currentTime = chrono::system_clock::now();
    //set the starting timee of process
    metaData.setStartTime(chrono::duration<double>(currentTime - START_TIME).count());
    //setting endtime
    auto endTime = threadProcessing(millisec);
    metaData.setTime(endTime);
}
/**
 * @name allocate memory
 *
 * @description set memory to random memory value in hex
 *
 * @param totalMemory
 *        amount of bytes that is available
 * @return
 */
string allocateMemory(int totalMemory){
    int address;
    //set address to random number within the total mem size
    srand(time(NULL));
    if( totalMemory > 0 )
    {
        address = rand() % totalMemory;
    }
    stringstream ss;
    //convert o hex and save
    ss << hex << address;
    string addressString = ss.str();
//    cout << "0x" << addressString << endl;
    return addressString;
}
/**
 * @name returnInstrutions
 *
 * @details reads lines from meta data file and parses them into words to intput into vector
 *
 * @param line
 *        lines gotten from reading the meta data file
 *
 * @param mdVector
 *        vector used to store the words created from parsing each line
 */
bool returnInstructions(string line, vector<string>& mdVector){
    string temp;
    for(unsigned int i = 0; i < line.size(); i++){ //iterate through entire line
        if(line[i] == ';'){ //when a semicolon is found
            mdVector.push_back(temp); // push everything in temp so far into vector
            line.erase(0, i); //erase everything read so far in the line
            i = 0; //restart iteration
            temp.erase(); //erase temp to get new word
            continue;
        }else if(line[i] == '.'){ //same thing as semicolon but for a period as the last word ends with period
            mdVector.push_back(temp);
            line.erase(0, i);
            i = 0;
            return true; //return true say that we have reached the period
        }else{
            temp.append(string(1,line[i])); //add every iteration to temp
        }

    }
    return false; // return false if period was not found

}
/**
 * @name parseInstructions
 *
 * @details parses through each word and outputs left, middle, and right
 *          left being the code
 *          middle being descriptor
 *          right being cycles
 *          this is all relative to the curly braces
 *
 * @param instr
 *        word to be separated into different parts
 *
 * @param left
 *        left side of the curly braces = code
 *
 * @param middle
 *        middle of the two curly braces = descriptor
 *
 * @right right
 *        right of the curly braces = cycles
 */
bool parseInstruction(string instr, string& left, string& middle, string& right){
    int state = 0; // counter used to append to different sides and to check curly braces
    bool seenLeft = false; // flag for left curly brace
    bool seenRight = false; // flag for right curly brace
    for(unsigned int i = 0; i < instr.size(); i++){
        if(instr[i] == '{'){
            if(!seenLeft){ //if the left curly brace has not been seen then add one to state and mark true
                state += 1;
                seenLeft = true;
            }else{
                return false;
            }
        }else if(instr[i] == '}'){
            if(!seenRight){ //if the right has not been seen then add one to state and mark true
                state += 1;
                seenRight = true;
            }else{
                return false;
            }
        }else{
            if(state == 0){
                left.append(string(1,instr[i])); //append to left until left curly brace this includes white spaces
            }else if(state == 1){
                middle.append(string(1,instr[i])); //append until right curly brace this includes white spaces
            }else if(state == 2){
                right.append(string(1,instr[i])); // append everything after curly brace this includes white spaces
            }else{
                return false;
            }
        }
    }
    if(state != 2){
        return false; //if duplicate curly braces are found
    }
    return true;
}
/**
 * @name readMetaFile
 *
 * @brief reads metadata file
 *
 * @details parses through meta data file and formates it into vector to be read easily by timeCalculation function
 *
 * @param filePath
 *        .mdf file mentioned in config file
 *
 * @param mdVector
 *        vector used to store values properly using MetaData class
 */
void readMetaFile(string filePath, vector<MetaData>& mdVector){
    ifstream metaFile;
    // tokenizer to parse through entire metadata file
    vector<string> parser;
    string pos; //reads in code, descriptor, and cycle as one string
    string temp;
    string code;
    string descriptor;
    string cycles;
    string emptyCycle = ""; //used to check for missing cycle number
    bool seenDotFlag = false; //flag used to indicate end of the file


    // open file and get first line and check if it is correct
    metaFile.open(filePath);
    getline(metaFile, pos);
    if(pos != "Start Program Meta-Data Code:"){
        cerr << "Error! Meta Data File is not formatted correctly" << endl;
        exit(0);
    }

    //until the line with a dot is seen, get line and parse it into different words
    while(!(seenDotFlag) && getline(metaFile, pos)){
        seenDotFlag = returnInstructions(pos, parser);
    }

    // get the last line of check if it is the correct line
    try{
        getline(metaFile, pos);
        if(pos != "End Program Meta-Data Code."){
            cerr << "Error! Meta Data File is not formatted correctly" << endl;
            exit(0);
        }
    }
    catch (int e){
        cerr << "Error! No closing tag" << endl;
        exit(0);
    }

    metaFile.close();

    // Parser for entire vector that contains all words
    for(auto pnt = parser.begin(); pnt != parser.end(); pnt++){

        pos = *pnt; //begin assignment of variables
        code.erase(); //reset code to assign code of next word
        descriptor.erase(); //same thing as code
        cycles.erase(); //same thing as code

        //parse all words and assign, otherwise output error
        if(!parseInstruction(pos, code, descriptor, cycles)){
            cerr << "Meta Data file is formatted incorrectly" << endl;
            exit(0);
        }

        Trim(code); //trim any whitespaces that may appear
        Trim(descriptor);
        Trim(cycles);


        // if any of the variables are empty then output error
        if(code == "" || descriptor == "" || cycles == ""){
            cerr << "Missing code, descriptor, or cycles!" << endl;
            exit(0);
        }

        //if cycles is negative
        if(stoi(cycles) < 0){
            cerr << "Cycle number is negative." << endl;
            exit(0);
        }
        //convert the data read into vector that uses metadata datatype
        MetaData input(code[0], descriptor, stoi(cycles));
        mdVector.push_back(input);

    }

}
/**
 * @name timeCalculation
 *
 * @brief calculates process times
 *
 * @details reads data from config class and metadata vector to calculate correct porcess times
 *
 * @param conf
 *        config class that had how long a cycle is
 *
 * @param metaData
 *        metaData vector that has code,descriptor, and cycle times
 *
 * @param sysStatus
 *        used to check if S process is running or ended
 * @param appStatus
 *        used to check if A process is running or ended
 */
void timeCalculation(Config conf, MetaData& metaData, int& sysStatus, int& appStatus){
    int timeLimit;
    static int processNum = 1;
    // checks if code is one of the correct options
    if(metaData.getCode() != 'S' && metaData.getCode() != 'A' && metaData.getCode() != 'P' && metaData.getCode() != 'I' && metaData.getCode() != 'O' && metaData.getCode() != 'M'){
        cerr << "Error! Missing Meta Data Code or incorrect format." << endl;
        exit(0);
    }
    // options for if the code is an S
    if(metaData.getCode() == 'S'){
        if(metaData.getDescriptor() == "begin" && ps.processState == EXIT){
            ps.processState = START;
            auto currentTime = chrono::system_clock::now();
            metaData.setStartTime(chrono::duration<double>(currentTime - START_TIME).count());
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Simulator program starting");
            sysStatus = 1;
        }else if(metaData.getDescriptor() == "finish" && ps.processState == START){
            ps.processState = EXIT;
            auto currentTime = chrono::system_clock::now();
            metaData.setTime(chrono::duration<double>(currentTime - START_TIME).count());
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Simulator program ending");
            sysStatus = 0;
        }else{
            cerr << "Error! Missing begin or finish descriptor for S" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'A'){ //options for if code is A
        if(metaData.getDescriptor() == "begin" && ps.processState == START){
            ps.processState = READY;
            auto currentTime = chrono::system_clock::now();
            metaData.setStartTime(chrono::duration<double>(currentTime - START_TIME).count());
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "OS: Preparing Process " + to_string(processNum));
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "OS: Starting Process " + to_string(processNum));
            appStatus = 1;
        }else if(metaData.getDescriptor() == "finish" && ps.processState == READY){
            ps.processState = START;
            auto currentTime = chrono::system_clock::now();
            metaData.setTime(chrono::duration<double>(currentTime - START_TIME).count());
            logFile.push_back(to_string(metaData.getTime()) + " - " + "OS: Removing Process " + to_string(processNum));
            processNum++;
            appStatus = 0;
        }else{
            cerr << "Error! Missing begin or finish descriptor for A" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'P'){ //options for if code is P
        if(metaData.getDescriptor() == "run"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getProcessorCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start processing action");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) +": end processing action");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for P" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'I'){ //options for if code is I
        if(metaData.getDescriptor() == "hard drive"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getHarddriveCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process "+ to_string(processNum) +": start hard drive input");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process "+ to_string(processNum) +": end hard drive input");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "keyboard"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getKeyboardCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) +": start keyboard input");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end keyboard input");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "mouse"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getMouseCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start mouse input");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end mouse input");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for I" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'O'){ //options for if code is O
        if(metaData.getDescriptor() == "hard drive"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getHarddriveCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start hard drive output");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end hard drive output");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "monitor"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getMoniterDisplayTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start monitor output");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end monitor output");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "printer"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getPrinterCycleTime();
            processRunner(timeLimit,metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start printer output");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end printer output");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for O" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'M'){ //options for if code is M
        if(metaData.getDescriptor() == "block"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getMemoryCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start memory blocking");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end memory blocking");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "allocate") {
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() * conf.getMemoryCycleTime();
            processRunner(timeLimit, metaData);
            logFile.push_back(to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start memory allocating");
            logFile.push_back(to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": memory allocated at 0x" + allocateMemory(conf.getMemory()));
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for M" << endl;
            exit(0);
        }
    }
}
/**
 * @name outputMessage
 *
 * @brief formates output text
 *
 * @param conf
 *        Config class to get information about configuration file
 * @param mdVector
 *        metaData vector to read in formated vector with data from metadata file
 * @param fout
 *        option to decide whether to cout(moniter) of fout(file)
 * @param logOption
 *        number that indicates where to output to
 */
void outputMessage(Config conf, vector<MetaData> mdVector, ostream& fout, int logOption){

    for(auto i = logFile.begin(); i != logFile.end(); i++){
        fout << fixed << setprecision(6) << *i << endl;
    }
//    fout << "Configuration File Data" << endl;
//    fout << "Monitor = " << conf.getMoniterDisplayTime() << "ms/cycle" << endl;
//    fout << "Processor = " << conf.getProcessorCycleTime() << "ms/cycle" << endl;
//    fout << "Mouse = " << conf.getMouseCycleTime() << "ms/cycle" << endl;
//    fout << "Hard Drive = " << conf.getHarddriveCycleTime() << "ms/cycle" << endl;
//    fout << "Keyboard = " << conf.getKeyboardCycleTime() << "ms/cycle" << endl;
//    fout << "Memory = " << conf.getMemoryCycleTime() << "ms/cycle" << endl;
//    fout << "Printer = " << conf.getPrinterCycleTime() << "ms/cycle" << endl;
//    fout << "Memory = " << conf.getMemory() << "bytes" << endl;
//
//    if((logOption == 1)){
//        fout << "Logged to: Monitor and " << conf.getLogFilePath() << endl; //output to file and monitor if 1
//    }else if(logOption == 2){
//        fout << "Logged to: " << conf.getLogFilePath() << endl; //output to file if 2
//    }else if(logOption == 3){
//        fout << "Logged to: Monitor" << endl; //output to monitor if 3
//    }
//
//    fout << endl;
//
//    fout << "Meta-Data Metrics" << endl;
//
//    // parses through entire vector
//    for(auto pnt = mdVector.begin(); pnt != mdVector.end(); pnt++){
//        MetaData metaData  = *pnt; // allows to use get methods if made the same datatype
//        if(metaData.getCode() != 'S' && metaData.getCode() != 'A'){ // only print if not S or A
//            metaData.printMetaData(fout);
//            fout << " - " <<  metaData.getTime() << "ms" << endl;
//        }
//    }

}
/**
 * @name outputOptions
 *
 * @brief decides where to output to
 *
 * @param conf
 *        config class to use methods
 * @param mdVector
 *        using vector that has formatted metadata
 */
void outputOptions(Config conf, vector<MetaData> mdVector){
    ofstream fout;
    int logOption = 0;
    bool moniterF = false; //flag to check whether to output to moniter
    bool fileF = false;    //flag to check whether to ouput to file

    if(conf.getLogOption() == "Log to Both"){
        moniterF = true;
        fileF = true; //both flags used to output to both monitor and file
        logOption = 1;
    }else if(conf.getLogOption() == "Log to File"){
        fileF = true;
        logOption = 2;
    }else if(conf.getLogOption() == "Log to Monitor"){
        moniterF = true;
        logOption = 3;
    }else{
        cerr << "Error! Log to option is not one of the options avalaible." << endl;
        exit(0);
    }

    if(moniterF){
        outputMessage(conf, mdVector, cout, logOption); //output to moniter using cout
    }
    if(fileF){
        cout << "Successfully logged output to file" << endl;
        fout.open(conf.getLogFilePath());
        outputMessage(conf, mdVector, fout, logOption); //ouput to file using fout
        fout.close();
    }
}
/**
 * @name lTrim
 *
 * @brief trims whitespaces left of string
 *
 * @param str
 *        input string
 */
void lTrim(string& str){
    str.erase(0, str.find_first_not_of(" ")); // erase empty space
}
/**
 * @name rTrim
 *
 * @brief trims whitespaces right of string
 *
 * @param str
 *        input string
 */
void rTrim(string& str){
    str.erase(str.find_last_not_of(" ") + 1); // erase empty space
}
/**
 * @name Trim
 *
 * @brief trims whitespaces of string
 *
 * @param str
 *        input string
 */
void Trim(string& str){
    rTrim(str); //trim right side
    lTrim(str); //trim left side
}