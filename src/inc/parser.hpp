#ifndef B98COMPILER_PARSER_HPP
#define B98COMPILER_PARSER_HPP

#include "defines.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

B98_NS_BEGIN

struct StateInfo {
public:
	StateInfo(int state, int xd, int yd)
		: state(state), xd(xd), yd(yd) { }
	
	int state, xd, yd;

}; // struct StateInfo;

struct StateInfo2 : public StateInfo {
public:
	StateInfo2(int state, int xd, int yd, int xPos, int yPos)
		: StateInfo(state, xd, yd), xPos(xPos), yPos(yPos) { }

	int xPos, yPos;

}; // struct StateInfo2 : public StateInfo;

class Parser {
public:
	Parser();

	int loadFile(std::string filename);

	void parse();

private:
	int xPos, yPos, xd, yd;

	int currState, nextState;
	bool bulldozing;

	std::vector<std::vector<char>> codegrid;
	std::vector<std::vector<std::vector<StateInfo>>> stateinfogrid;

	std::vector<StateInfo2> stateQueue;

	std::vector<StateInfo2> stateStartPoints;

	bool parseStep(char c);
	bool setNextState();

	void turnLeft();
	void turnRight();

	int bulldozeIsValid();

	bool stateHasNoStart(int state);
	bool startPointHere(int state);

	bool anyStartHere();
	bool anyStartButCurrent();

	int startAtDelta(int dx, int dy);
	int startHere();

	int safeAddDelta(int pos, int d, int max);

	void generateOutput();

	void generateHeader();
	void generateSource();

}; // class Parser;

B98_NS_END

#endif // B98COMPILER_PARSER_HPP
