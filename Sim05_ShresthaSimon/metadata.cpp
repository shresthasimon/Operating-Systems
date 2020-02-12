/**
 * @file MetaData.cpp
 *
 * @brief Implementation file for MetaData Class
 *
 * @details Implements functions from MetaData.h
 *
 * @note Requires MetaData.h
 */

#include "metadata.h"
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
double MetaData::getTime() const {
    return time;
}
//
void MetaData::setTime(double time) {
    MetaData::time = time;
}
//
double MetaData::getStartTime() const{
    return startTime;
}
//
void MetaData::setStartTime(double startTime){
    MetaData::startTime = startTime;
}
//
void MetaData::setmetaVector(vector<string> vector)
{
    metaVector = vector;
}
//
vector<string> MetaData::getmetaVector() const {
    return metaVector;
}
/**
 * @name lTrim
 *
 * @brief trims whitespaces left of string
 *
 * @param str
 *        input string
 */
void MetaData::lTrim(string& str){
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
void MetaData::rTrim(string& str){
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
void MetaData::Trim(string& str){
    MetaData::rTrim(str); //trim right side
    MetaData::lTrim(str); //trim left side
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
bool MetaData::returnInstructions(string line, deque<string>& mdVector){
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
bool MetaData::parseInstruction(string instr, string& left, string& middle, string& right){
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
void MetaData::readMetaFile(string filePath, deque<MetaData>& mdVector){
    ifstream metaFile;
    // tokenizer to parse through entire metadata file
    deque<string> parser;
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
        seenDotFlag = MetaData::returnInstructions(pos, parser);
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
        if(!MetaData::parseInstruction(pos, code, descriptor, cycles)){
            cerr << "Meta Data file is formatted incorrectly" << endl;
            exit(0);
        }

        MetaData::Trim(code); //trim any whitespaces that may appear
        MetaData::Trim(descriptor);
        MetaData::Trim(cycles);


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
 * @name loadMetadataFile
 *
 * @details: loads in metadata file for reading
 *
 * @param metadataFile
 *      metadata class object
 * @return
 */
string MetaData::loadMetadataFile(string metadataFile)
{
	fstream inputFile;
	string tempMetadataData;
    //open file
	inputFile.open(metadataFile, ios::in | ios::binary);
    // read line by line until completed
	if (inputFile)
	{
		while (inputFile.good())
		{
			tempMetadataData += inputFile.get();
		}
	}
	else
	{
	    cerr << "Error, metafile file not read correctly" << endl;
	    exit(0);
	}

    //close file
	inputFile.close();
	return tempMetadataData;
}
/**
 * @name readMetaDataFile
 *
 * @details parses through data read from metadata file
 *
 * @param metadataData
 *      Metadata class object
 */
void MetaData::readMetadataFile(string metadataData)
{
	vector<string> tempVector;
	string tempString;
	int lineBegin = 0, lineEnd = 0;
	//erase the last line
	metadataData.erase(lineBegin, metadataData.find('\n') + 1);
	lineBegin = metadataData.find("End Program Meta-Data Code");
	lineEnd = metadataData.find('\n');
	metadataData.erase(lineBegin, lineEnd);
    //separate the lines into groups and push into vector
	for (unsigned int i = 0; i < metadataData.size(); i++)
	{

		lineBegin = metadataData.find("{") - 1;
		lineEnd = metadataData.find(";");

		tempString = metadataData.substr(lineBegin, lineEnd - lineBegin);

		tempVector.push_back(tempString);

		metadataData.erase(lineBegin, (lineEnd - lineBegin) + 2);
	}
    //gets the final group that ends in a period
	lineBegin = metadataData.find("{") - 1;
	lineEnd = metadataData.find(".");
	tempString = metadataData.substr(lineBegin, lineEnd - lineBegin);
	tempVector.push_back(tempString);
    //store into a vector
	setmetaVector(tempVector);
}
/**
 * @name getMetaDataComponents
 *
 * @details breaks the groups into individual components of code, descriptor, and cycle
 *
 * @param vector
 *      pass in vector to parse through
 */
void MetaData::getMetadataComponents(vector<string> vector)
{
	string tempString;
	int lineBegin = 0, lineEnd = 1;
	int numElementsOld = vector.size();
    // read through entire vector
	for (unsigned int i = 0; i < metaVector.size(); i++)
	{
	    //get the code
		tempString = vector[i];
		lineBegin = tempString.find("{") - 1;
		lineEnd = tempString.find("{");
		tempString = (tempString.substr(lineBegin, lineEnd - lineBegin));
		vector.push_back(tempString);
        //get the descriptor
		tempString = vector[i];
		lineBegin = tempString.find("{") + 1;
		lineEnd = tempString.find("}");
		tempString = (tempString.substr(lineBegin, lineEnd - lineBegin));
		vector.push_back(tempString);
        //get the cycle
		tempString = vector[i];
		lineBegin = tempString.find("}") + 1;
		lineEnd = tempString.find('\n');
		tempString = (tempString.substr(lineBegin, lineEnd - lineBegin));
        // if empty or negative cycle number
		if (tempString.empty())
		{
		    cerr << "Error, cycle number not found" << endl;
		    exit(0);
		}
		if (stoi(tempString) < 0)
		{
		    cerr << "Error, incorrect cycle number" << endl;
		    exit(0);
		}
		vector.push_back(tempString);
	}
	// remove the groups that are still in the vector
	reverse(vector.begin(), vector.end());

	for (int i = 0; i < numElementsOld; i++)
	{
		vector.pop_back();
	}
	// place back into the correct order
	reverse(vector.begin(), vector.end());
    // set the vector
	setmetaVector(vector);
}


