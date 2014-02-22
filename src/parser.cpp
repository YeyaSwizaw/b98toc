#include "inc/parser.hpp"

B98_NS_BEGIN

Parser::Parser()
	: xPos(0), yPos(0), xd(1), yd(0),
	  currState(0), nextState(1),
	  bulldozing(false) {

} // Parser::Parser();

int Parser::loadFile(std::string filename) {
	std::ifstream srcfile(filename);
	if(!srcfile) {
		std::cerr << B98_NOFILE_MSG(filename);
		return -1;

	} // if(!srcfile);

	/* Load code into vectors */
	char ch;
	std::vector<char>::size_type maxsize = 0;
	codegrid.push_back(std::vector<char>());
	while(srcfile >> std::noskipws >> ch) {
		if(ch == '\n') {
			maxsize = std::max(maxsize, codegrid.back().size());
			codegrid.push_back(std::vector<char>());

		} // if(ch == '\n');
		else {
			codegrid.back().push_back(ch);

		} // else;

	} // while(srcfile >> std::noskipws >> ch);

	/* Remove extra newline */
	if(codegrid.back().empty()) {
		codegrid.pop_back();

	} // if(codegrid.back().empty());

	/* Pad vectors to same length; initialise info vector */
	for(auto& v : codegrid) {
		if(v.size() < maxsize) {
			v.resize(maxsize, ' ');

		} // if(v.size() < maxsize);

		stateinfogrid.push_back(std::vector<std::vector<StateInfo>>());
		stateinfogrid.back().resize(maxsize);

	} // for(auto& v : codegrid);

	srcfile.close();
	return 0;

} // int Parser::loadFile(std::string filename);

void Parser::parse() {
	int xMax = codegrid[0].size();
	int yMax = codegrid.size();

	// std::cerr << xMax << " x " << yMax << "\n";

	// std::cerr << xPos << ", " << yPos << ": ";
	
	while(parseStep(codegrid[yPos][xPos])) {
		xPos += xd;
		yPos += yd;

		if(xPos < 0) {
			xPos = xMax + xPos;

		} // if(xPos < 0);
		else if(xPos >= xMax) {
			xPos = xPos - xMax;

		} // else if(xPos >= xMax);

		if(yPos < 0) {
			yPos = yMax + yPos;

		} // if(yPos < 0);
		else if(yPos >= yMax) {
			yPos = yPos - yMax;

		} // else if(yPos >= yMax);

		if(xd > 0) {
			xd = 1;

		} // if(xd > 0);
		else if(xd < 0) {
			xd = -1;

		} // else if(xd < 0);

		if(yd > 0) {
			yd = 1;

		} // if(yd > 0);
		else if(yd < 0) {
			yd = -1;

		} // else if(yd < 0);

		// std::cerr << xPos << ", " << yPos << ": ";

	} // while(parseStep(codegrid[xPos][yPos]));

	std::sort(stateStartPoints.begin(), stateStartPoints.end(), [](StateInfo2 a, StateInfo2 b){ return (a.state < b.state); });

	std::cerr << "\n";

	for(auto& info : stateStartPoints) {
		std::cerr << "State " << info.state << " (" << info.xPos << ", " << info.yPos << ") -> (" << info.xd << ", " << info.yd << ")\n";

	} // for(auto& info : stateStartPoints);

	std::cerr << "\n";

	for(auto& v : stateinfogrid) {
		for(auto& v2 : v) {
			if(v2.empty()) {
				std::cerr << ' ';

			} // if(v2.empty());
			else {
				std::cerr << v2.back().state;

			} // else;

		} // for(auto& v2 : v);

		std::cerr << '\n';

	} // for(auto& info : stateinfogrid);

	generateOutput();

} // void Parser::parse();

bool Parser::parseStep(char c) {
	if(c == '>') {
		xd = 1;
		yd = 0;

	} // if(c == '>');
	else if(c == '<') {
		xd = -1;
		yd = 0;

	} // else if(c == '<');
	else if(c == '^') {
		xd = 0;
		yd = -1;

	} // else if(c == '^');
	else if(c == 'v') {
		xd = 0;
		yd = 1;

	} // else if(c == 'v');
	else if(c == '[') {
		turnLeft();

	} // else if(c == '[');
	else if(c == ']') {
		turnRight();

	} // else if(c == ']');
	else if(c == '#') {
		xd *= 2;
		yd *= 2;

	} // else if(c == '#');

	if(!bulldozing) {
		int state = bulldozeIsValid();
		if(state != nextState) {
			for(auto& info : stateinfogrid[yPos][xPos]) {
				if(info.state == state) {
					std::cerr << "Char '" << c << "' at (" << xPos << ", " << yPos << "): Bulldozed from " << info.state << " to " << nextState << "\n";

					currState = nextState;
					nextState++;

					info.state = currState;

					if(stateHasNoStart(currState)) {
						stateStartPoints.push_back(StateInfo2(currState, xd, yd, xPos, yPos));

					} // if(stateHasNoStart(currState));

					bulldozing = true;

				} // if(xd == info.xd && yd == info.yd);

			} // for(auto& info : stateinfogrid[yPos][xPos]);

		} // if(bulldozeIsValid() != nextState);

		if(!bulldozing) {
			if(!anyStartHere()) {
				stateinfogrid[yPos][xPos].push_back(StateInfo(currState, xd, yd));

				std::cerr << "Char '" << c << "' at (" << xPos << ", " << yPos << "): State " << currState << " (" << xd << ", " << yd << ")\n";

				if(stateHasNoStart(currState)) {
					stateStartPoints.push_back(StateInfo2(currState, xd, yd, xPos, yPos));

				} // if(stateHasNoStart(currState));

			} // if(!anyStartHere());

		} // if(!bulldozing);

	} // if(!bulldozing);
	else {
		for(auto& info : stateinfogrid[yPos][xPos]) {
			if(xd == info.xd && yd == info.yd) {
				std::cerr << "Char '" << c << "' at (" << xPos << ", " << yPos << "): Bulldozed from " << info.state << " to " << currState << "\n";

				info.state = currState;

			} // if(xd == info.xd && yd == info.yd);

		} // for(auto& info : stateinfogrid[yPos][xPos]);

	} // else;

	if(c == '@') {
		bulldozing = false;

		if(!setNextState()) {
			return false;

		} // if(!setNextState());

	} // if(c == '@');

	else if(c == '_' || c == '|') {
		if(!bulldozing) {
			if(c == '_') {
				stateQueue.push_back(StateInfo2(nextState, -1, 0, xPos, yPos));
				stateQueue.push_back(StateInfo2(nextState + 1, 1, 0, xPos, yPos));

			} // if(c == '_');
			else {
				stateQueue.push_back(StateInfo2(nextState, 0, -1, xPos, yPos));
				stateQueue.push_back(StateInfo2(nextState + 1, 0, 1, xPos, yPos));

			} // else;

			nextState += 2;

		} // if(!bulldozing);

		bulldozing = false;

		if(!setNextState()) {
			return false;

		} // if(!setNextState());

	} // else if(c == '_');

	else if(c == 'w') {
		if(!bulldozing) {
			stateQueue.push_back(StateInfo2(nextState, xd, yd, xPos, yPos));

			turnLeft();
			stateQueue.push_back(StateInfo2(nextState + 1, xd, yd, xPos, yPos));

			turnRight();
			turnRight();
			stateQueue.push_back(StateInfo2(nextState + 2, xd, yd, xPos, yPos));

			nextState += 3;

		} // if(!bulldozing);

		bulldozing = false;

		if(!setNextState()) {
			return false;

		} // if(!setNextState());

	} // else if(c == 'w');

	// std::cerr << c << "\n";

	return true;

} // bool Parser::parseStep(char c);

B98_NS_END

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << B98_USAGE_MSG;
		return -1;

	} // if(argc < 2);

	b98::Parser p;
	if(p.loadFile(argv[1]) < 0) {
		return -1;

	} // if(p.loadFile(argv[1]) < 0);
	
	p.parse();

	return 0;

} // int main(int argc, char* argv[]);
