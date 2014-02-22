#include "inc/parser.hpp"

B98_NS_BEGIN

void Parser::turnLeft() {
	if(xd == 0) {
		xd = yd;
		yd = 0;

	} // if(xd == 0);
	else {
		yd = -xd;
		xd = 0;

	} // else;

} // void Parser::turnLeft();

void Parser::turnRight() {
	if(xd == 0) {
		xd = -yd;
		yd = 0;

	} // if(xd == 0);
	else {
		yd = xd;
		xd = 0;

	} // else;

} // void Parser::turnRight();

bool Parser::setNextState() {
	while(true) {
		if(stateQueue.empty()) {
			return false;

		} // if(stateQueue.empty());

		StateInfo2& info = stateQueue.back();

		// Check and remove duplicates
		bool found = false;
		for(auto it = stateQueue.begin(); it != stateQueue.end(); ++it) {
			if(it->xPos == info.xPos && it->yPos == info.yPos && it->xd == info.xd && it->yd == info.yd && it->state != info.state) {
				stateQueue.pop_back();

				found = true;
				break;

			} // if(it->xPos == info.xPos && it->yPos == info.yPos && it->xd == info.xd && it->yd == info.yd && it->state != info.state);

		} // for(auto it = stateQueue.begin(); it != stateQueue.end(); ++it);

		if(found) {
			continue;

		} // if(found);

		for(auto it = stateStartPoints.begin(); it != stateStartPoints.end(); ++it) {
			if(it->xPos == safeAddDelta(info.xPos, info.xd, codegrid[0].size()) && it->yPos == safeAddDelta(info.yPos, info.yd, codegrid.size()) && it->xd == info.xd && it->yd == info.yd && it->state != info.state) {
				stateQueue.pop_back();

				found = true;
				break;

			} // if(it->xPos == safeAddDelta(info.xPos, info.xd, codegrid[0].size()) && it->yPos == safeAddDelta(info.yPos, info.yd, codegrid.size()) && it->xd == info.xd && it->yd == info.yd && it->state != info.state);

		} // for(auto it = stateStartPoints.begin(); it != stateStartPoints.end(); ++it);

		xPos = info.xPos;
		yPos = info.yPos;
		xd = info.xd;
		yd = info.yd;
		currState = info.state;

		stateQueue.pop_back();
		return true;

	} // while(true);

} // bool Parser::setNextState();

int Parser::bulldozeIsValid() {
	int state;

	for(auto& info : stateinfogrid[yPos][xPos]) {
		if(xd == info.xd && yd == info.yd) {
			if(startPointHere(info.state) == true) {
				return nextState;

			} // if(startPointHere(info.state) == true);
			else {
				state = info.state;

			} // else;

		} // if(xd == info.xd && yd == info.yd);

	} // for(auto& info : stateinfogrid[yPos][xPos]);

	return state;

} // int Parser::bulldozeIsValid();

bool Parser::startPointHere(int state) {
	for(auto& info : stateStartPoints) {
		if(info.state == state && info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd) {
			return true;

		} // if(info.state == state && info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd);

	} // for(auto& info : stateStartPoints);

	return false;

} // bool Parser::startPointHere(int state);

bool Parser::stateHasNoStart(int state) {
	for(auto& info : stateStartPoints) {
		if(info.state == state) {
			return false;

		} // if(info.state == state);

	} // for(auto& info : stateStartPoints);

	return true;

} // bool Parser::stateHasNoStart(int state);

bool Parser::anyStartHere() {
	for(auto& info : stateStartPoints) {
		if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd) {
			return true;

		} // if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd);

	} // for(auto& info : stateStartPoints);

	return false;

} // bool Parser::anyStartHere();

bool Parser::anyStartButCurrent() {
	for(auto& info : stateStartPoints) {
		if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd && info.state != currState) {
			return true;

		} // if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd);

	} // for(auto& info : stateStartPoints);

	return false;

} // bool Parser::anyStartButCurrent();

int Parser::startAtDelta(int dx, int dy) {
	int x = safeAddDelta(xPos, dx, codegrid[0].size());
	int y = safeAddDelta(yPos, dy, codegrid.size());

	for(auto& info : stateStartPoints) {
		if(info.xPos == x && info.yPos == y && info.xd == dx && info.yd == dy) {
			return info.state;

		} // if(info.xPos == x && info.yPos = y && info.xd == dx && info.yd == dy);

	} // for(auto& info : stateStartPoints);

	return -1;

} // int Parser::startAtDelta(int dx, int dy);

int Parser::startHere() {
	for(auto& info : stateStartPoints) {
		if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd) {
			return info.state;

		} // if(info.xPos == xPos && info.yPos == yPos && info.xd == xd && info.yd == yd);

	} // for(auto& info : stateStartPoints);

	return -1;

} // int Parser::startHere();

int Parser::safeAddDelta(int pos, int d, int max) {
	pos += d;

	if(pos < 0) {
		pos = max + pos;

	} // if(pos < 0);
	else if(pos >= max) {
		pos = pos - max;

	} // else if(xPos >= xMax);

	return pos;

} // int Parser::safeAddDelta(int pos, int d, int max);

B98_NS_END
