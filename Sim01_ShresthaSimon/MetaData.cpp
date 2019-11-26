/**
 * @file MetaData.cpp
 *
 * @brief Implementation file for MetaData Class
 *
 * @details Implements functions from MetaData.h
 *
 * @note Requires MetaData.h
 */

// Header Files
//
#include "MetaData.h"
#include <iostream>
#include <string>
using namespace std;
// Constructor Implementation/////////////////////////////////////////////////////////////////
//
MetaData::MetaData() {

}
//
// Destructor Implementation//////////////////////////////////////////////////////////////
//
MetaData::~MetaData() {

}
//
// Parameterized Constructor Implementation///////////////////////////////////////////////
//
MetaData::MetaData(char code, string descriptor, int cycles) {
    this->code = code;
    this->descriptor = descriptor;
    this->cycles = cycles;
}
//
// Printing MetaData info implementation
void MetaData::printMetaData(ostream& out) {
    out << code << "{"<< descriptor << "}" << cycles;
}
//
// Setter and Getter method Implementation///////////////////////////////////////////////
//
void MetaData::setCode(char code) {
    MetaData::code = code;
}
//
char MetaData::getCode() const {
    return code;
}
//
void MetaData::setDescriptor(const string &descriptor) {
    MetaData::descriptor = descriptor;
}
//
const string &MetaData::getDescriptor() const {
    return descriptor;
}
//
void MetaData::setCycles(int cycles) {
    MetaData::cycles = cycles;
}
//
int MetaData::getCycles() const {
    return cycles;
}
//
int MetaData::getTime() const {
    return time;
}
//
void MetaData::setTime(int time) {
    MetaData::time = time;
}
//






