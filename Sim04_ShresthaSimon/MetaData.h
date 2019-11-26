/**
 * @file MetaData.h
 *
 * @brief Header file for MetaData Class
 *
 * @details used as a datatype for vector in main function to format input of MetaData file
 *
 *
 */

// Precompiler Directives
//
#ifndef CS446PROJ1_METADATA_H
#define CS446PROJ1_METADATA_H
//
// Header Files
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
using namespace std;
// Class Definition
//
class MetaData {
public:
    //Constructor
    MetaData();
    //Deconstructor
    virtual ~MetaData();
    //Parameterized Constructor
    MetaData(char code, string descriptor, int cycles);

    //functions to parse through metadata file
    static void readMetaFile(string, deque<MetaData>&);
    static bool returnInstructions(string, deque<string>& );
    static bool parseInstruction(string , string& , string& , string& );
    static void lTrim(string& str);
    static void rTrim(string& str);
    static void Trim(string& str);

    //Getter and Setter methods for private variables
    void setCode(char code);
    char getCode() const;

    void setDescriptor(const string &descriptor);
    const string &getDescriptor() const;

    void setCycles(int cycles);
    int getCycles() const;

    void printMetaData(ostream& out);

    double getTime() const;
    void setTime(double time);

    double getStartTime() const;
    void setStartTime(double startTime);

private:
    char code;
    string descriptor;
    int cycles;
    double time;
    double startTime;

};
//
// Terminating Precompiler Directives
//
#endif
//