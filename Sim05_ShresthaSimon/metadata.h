/**
 * @file MetaData.h
 *
 * @brief Header file for MetaData Class
 *
 * @details used as a datatype for vector in main function to format input of MetaData file
 *
 *
 */

#ifndef METADATA_H
#define METADATA_H

#include "config.h"
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
#include <deque>

using namespace std;

class MetaData
{
public:
    MetaData();
    //Deconstructor
    virtual ~MetaData();
    //Parameterized Constructor
    MetaData(char code, string descriptor, int cycles);

    //functions to parse through metadata file
    string loadMetadataFile(string metadataFile);
    void readMetadataFile(string metadataFile);
    void getMetadataComponents(vector<string> vector);
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

    void setmetaVector(vector<string> mVect);

    vector<string> getmetaVector() const;
private:
    char code;
    string descriptor;
    int cycles;
    double time;
    double startTime;
	vector<string> metaVector;

};

#endif
