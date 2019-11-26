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
#include <semaphore.h>

#include "Config.h"
#include "MetaData.h"
using namespace std;
using namespace std::chrono;
//
// Function Definitions//////////////////////////////////////////////////////////////
bool returnInstructions(string line, vector<string>& mdVector);
bool parseInstruction(string instr, string& left, string& middle, string& right);
void readMetaFile(string filePath, vector<MetaData>& mdVector);
void timeCalculation(Config* conf, MetaData& metaData, int& sysStatus, int& appStatus);
void lTrim(string& str);
void rTrim(string& str);
void Trim(string& str);
string allocateMemory(int);
void* timer(void *);
double threadProcessing(int);
void processRunner(int, MetaData&, sem_t&, pthread_mutex_t&);
void initSem(Config);
void directOutput(Config, string);
void outputToStream(ostream&, string);
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
//  Semaphore objects
//
sem_t printerSemaphore;
sem_t harddriveSemaphore;
sem_t mouseSemaphore;
sem_t monitorSemaphore;
sem_t keyboardSempahore;
//
//  Mutex Objects
//
pthread_mutex_t printerLock;
pthread_mutex_t harddriveLock;
pthread_mutex_t mouseLock;
pthread_mutex_t monitorLock;
pthread_mutex_t keyboardLock;
//
//  Counting objects for hard drive, printer, and memory block resources
//
static int harddriveCount = 0;
static int printerCount = 0;
static int memoryBlocks = 0;
//  flag to check if everything has been logged to the file
static bool alreadyLogged = false;
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
    Config conf;
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
    conf.readConfigFile(configFile);

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
    fin.open(conf.getFilePath());
    if(fin.peek() == ifstream::traits_type::eof()){
        cerr << "Error! Empty Meta Data File or .mdf file not entered in config file" << endl;
        exit(0);
    }

    fin.close();

    // reads in metadata file into metadata vector
    readMetaFile(conf.getFilePath(), mdVector);

    initSem(conf);
    // goes through the vector and calculates the process times
    for(auto& pnt : mdVector){
        timeCalculation(&conf, pnt, sysStatus, appStatus);
    }
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
void processRunner(int timeLimit, MetaData& metaData, sem_t &semaphore, pthread_mutex_t &mutex){

    sem_wait(&semaphore);
    pthread_mutex_lock(&mutex);
    auto endTime = threadProcessing(timeLimit);
    pthread_mutex_unlock(&mutex);
    metaData.setTime(endTime);
    sem_post(&semaphore);
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
    string input;
    stringstream ss;
    ss << hex << setw(8) << setfill('0')  << totalMemory;
    return ss.str();
}

/**
 * @name initSem
 *
 * @description initializes all mutex and semaphore objects
 *
 * @param conf
 *        config class
 */
void initSem(Config conf){
    //initialize all mutex objects
    pthread_mutex_init(&printerLock, NULL);
    pthread_mutex_init(&harddriveLock, NULL);
    pthread_mutex_init(&monitorLock, NULL);
    pthread_mutex_init(&mouseLock, NULL);
    pthread_mutex_init(&keyboardLock, NULL);
    // initialize all semaphore objects
    sem_init(&harddriveSemaphore, 0, conf.getHarddriveResources());
    sem_init(&printerSemaphore, 0, conf.getPrinterResources());
    sem_init(&monitorSemaphore, 0, 1);
    sem_init(&keyboardSempahore, 0, 1);
    sem_init(&monitorSemaphore, 0, 1);
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
void timeCalculation(Config* conf, MetaData& metaData, int& sysStatus, int& appStatus){
    int timeLimit;
    // counter to keep track of current process num
    static int processNum = 1;
    // checks if code is one of the correct options
    if(metaData.getCode() != 'S' && metaData.getCode() != 'A' && metaData.getCode() != 'P' && metaData.getCode() != 'I' && metaData.getCode() != 'O' && metaData.getCode() != 'M'){
        cerr << "Error! Missing Meta Data Code or incorrect format." << endl;
        exit(0);
    }
    // options for if the code is an S
    if(metaData.getCode() == 'S'){
        if(metaData.getDescriptor() == "begin" && ps.processState == EXIT){
            //set process state to start
            ps.processState = START;
            // start timer
            auto currentTime = chrono::system_clock::now();
            // set time
            metaData.setStartTime(chrono::duration<double>(currentTime - START_TIME).count());
            //output
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "Simulator program starting");
            sysStatus = 1;
        }else if(metaData.getDescriptor() == "finish" && ps.processState == START){
            ps.processState = EXIT;
            auto currentTime = chrono::system_clock::now();
            metaData.setTime(chrono::duration<double>(currentTime - START_TIME).count());
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Simulator program ending" );
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
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "OS: Preparing Process " + to_string(processNum));
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "OS: Starting Process " + to_string(processNum));
            appStatus = 1;
        }else if(metaData.getDescriptor() == "finish" && ps.processState == READY){
            ps.processState = START;
            auto currentTime = chrono::system_clock::now();
            metaData.setTime(chrono::duration<double>(currentTime - START_TIME).count());
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "OS: Removing Process " + to_string(processNum));
            processNum++;
            appStatus = 0;
        }else{
            cerr << "Error! Missing begin or finish descriptor for A" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'P'){ //options for if code is P
        if(metaData.getDescriptor() == "run"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() + conf->getProcessorCycleTime();
            //set time
            auto currentTime = chrono::system_clock::now();
            // start starting time
            metaData.setStartTime(chrono::duration<double>(currentTime - START_TIME).count());
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start processing action");
            //get time of when process finished
            auto endTime = threadProcessing(timeLimit);
            //set end time
            metaData.setTime(endTime);
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) +": end processing action");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for P" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'I'){ //options for if code is I
        if(metaData.getDescriptor() == "hard drive"){
            timeLimit = metaData.getCycles() + conf->getHarddriveCycleTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf,to_string(metaData.getStartTime()) + " - " + "Process "+ to_string(processNum) +": start hard drive input on HDD " + to_string(harddriveCount++ % conf->getHarddriveResources()));
            // run thread with mutex and semaphore
            processRunner(timeLimit, metaData, harddriveSemaphore, harddriveLock);
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Process "+ to_string(processNum) +": end hard drive input");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "keyboard"){
            timeLimit = metaData.getCycles() + conf->getKeyboardCycleTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) +": start keyboard input");
            // run thread with mutex and semaphore
            processRunner(timeLimit, metaData, keyboardSempahore, keyboardLock);
            directOutput(*conf,to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end keyboard input");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "mouse"){
            timeLimit = metaData.getCycles() + conf->getMouseCycleTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start mouse input");
            // run thread with mutex and semaphore
            processRunner(timeLimit,metaData,mouseSemaphore,mouseLock);
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end mouse input");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for I" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'O'){ //options for if code is O
        if(metaData.getDescriptor() == "hard drive"){
            timeLimit = metaData.getCycles() + conf->getHarddriveCycleTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf,to_string(metaData.getStartTime()) + " - " + "Process "+ to_string(processNum) +": start hard drive input on HDD " + to_string(harddriveCount++ % conf->getHarddriveResources()));
            // run thread with mutex and semaphore
            processRunner(timeLimit, metaData, harddriveSemaphore, harddriveLock);
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Process "+ to_string(processNum) +": end hard drive input");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "monitor"){
            timeLimit = metaData.getCycles() + conf->getMoniterDisplayTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start monitor output");
            // run thread with mutex and semaphore
            processRunner(timeLimit,metaData,monitorSemaphore,monitorLock);
            directOutput(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end monitor output");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "printer"){
            timeLimit = metaData.getCycles() + conf->getPrinterCycleTime();
            auto currentTime = chrono::system_clock::now();
            ps.processState = RUNNING;
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf,to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start printer output on PRIN " + to_string(printerCount++ % conf->getPrinterResources()));
            // run thread with mutex and semaphore
            processRunner(timeLimit,metaData,printerSemaphore,printerLock);
            directOutput(*conf,to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end printer output");
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for O" << endl;
            exit(0);
        }
    }else if(metaData.getCode() == 'M'){ //options for if code is M
        if(metaData.getDescriptor() == "block"){
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() + conf->getMemoryCycleTime();
            auto currentTime = chrono::system_clock::now();
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf,to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start memory blocking");
            auto endTime = threadProcessing(timeLimit);
            metaData.setTime(endTime);
            directOutput(*conf,to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": end memory blocking");
            ps.processState = READY;
        }else if(metaData.getDescriptor() == "allocate") {
            ps.processState = RUNNING;
            timeLimit = metaData.getCycles() + conf->getMemoryCycleTime();
            auto currentTime = chrono::system_clock::now();
            metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
            directOutput(*conf,to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNum) + ": start memory allocating");
            auto endTime = threadProcessing(timeLimit);
            metaData.setTime(endTime);
            directOutput(*conf,to_string(metaData.getTime()) + " - " + "Process " + to_string(processNum) + ": memory allocated at 0x" + allocateMemory(memoryBlocks++ * conf->getMemBlockSize()));
            ps.processState = READY;
        }else{
            cerr << "Error! Missing or Misspelled descriptor for M" << endl;
            exit(0);
        }
    }
}

/**
 * @name outputToStream
 *
 * @details simple output function that knows where to output
 * @param out
 *        ostream object, can be either cout or the file
 * @param logOutput
 */
void outputToStream(ostream& out, string logOutput){
    out << logOutput << endl;
}

/**
 * @name directOutput
 *
 * @details decides where to output
 * @param conf
 *        config class
 * @param logOutput
 *        what to output
 */
void directOutput(Config conf, string logOutput){
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
        outputToStream(cout, logOutput);
    }
    //output to file
    if(fileF){
        log.open(conf.getLogFilePath(), alreadyLogged ? ofstream::app : ofstream::trunc);
        //check if it was already logged to file
        if(!alreadyLogged){
            alreadyLogged = true;
        }

        outputToStream(log, logOutput);
        log.close();
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