#include <fstream>
#include <iostream>

#include "Config.h"

using namespace std;

//
// Config: public methods
//

Config::Config()
{
}

Config::~Config()
{
}

void Config::parse(const char* filename)
{
    host_.clear();
    media_.clear();
    parameter_.clear();

    string line;

    // open file
    ifstream file(filename);
    if (!file.is_open())
        return;

    // parse file
    while (!file.eof()) {
        getline(file,line);
        parseLine(line);
    }

    // close file
    file.close();
}

std::string get_working_path()
{
    char temp[500];
    return ( getcwd(temp, 500) ? std::string( temp ) : std::string("") );
}

string&
Config::host(string name)
{
    return host_[name];
}

string&
Config::media(string name)
{
    return media_[name];
}

string&
Config::parameter(string name)
{
    return parameter_[name];
}

//
// Config: private methods
//

void Config::parseLine(std::string &line)
{
    vector<string> tokens = tokenizer_.parse(line);
    if (tokens.size() < 3)
        return;
    if (tokens.at(0) == "host"){
        string fullPath = tokens.at(2);
        if(tokens.at(2).compare(0,1,"/") != 0){
            std::string cwd = get_working_path();
            fullPath = cwd + "/" + tokens.at(2);
        }
        host_[tokens.at(1)] = fullPath;
    }
    else if (tokens.at(0) == "media")
        media_[tokens.at(1)] = tokens.at(2);
    else if (tokens.at(0) == "parameter")
        parameter_[tokens.at(1)] = tokens.at(2);
}
