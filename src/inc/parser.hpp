#ifndef B98TOC_PARSER_HPP
#define B98TOC_PARSER_HPP

#include <tclap/CmdLine.h>

#include "defines.hpp"
#include "action.hpp"
#include "dir.hpp"

#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

B98_NS_BEGIN

class Parser {
public:
    Parser();
    virtual ~Parser();

    int run(int argc, char* argv[]);

private:
    struct Location {
        int x, y;
        Dir* dir;
    };

    bool verbose;

    std::string inFile, outSource, outHeader;

    std::vector<std::vector<char>> codegrid;
    std::vector<std::vector<std::vector<int>>> stategrid;

    std::vector<Location> stateStarts;
    std::vector<std::vector<Action*>> parsedStates;

    int parseArgs(int argc, char* argv[]);
    int readFile();

    int createStates();
    int parse();
    int parseState(int state);

    int generateOutput();
    int generateHeader();
    int generateSource();
};

B98_NS_END

#endif
