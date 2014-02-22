#include "inc/parser.hpp"

B98_NS_BEGIN

void Parser::generateOutput() {
	generateHeader();
	generateSource();

} // void Parser::generateOutput();

void Parser::generateHeader() {
	std::stringstream headerContent;

	headerContent << B98_OUTPUT_HEADER_TOP 
		<< "\n" << B98_OUTPUT_HEADER_INCLUDES
		<< "\n" << B98_OUTPUT_HEADER_STACK_DECL
		<< "\n";

	for(auto &info : stateStartPoints) {
		headerContent << "int state" << info.state << "(struct Stack *stackPtr);\n";

	} // for(auto &info : stateStartPoints);

	headerContent << "\n" << B98_OUTPUT_HEADER_BOTTOM;

	std::ofstream headerFile("output.h");
	headerFile << headerContent.str();

	headerFile.close();

} // void Parser::generateHeader();

void Parser::generateSource() {
	std::stringstream sourceContent;

	sourceContent << B98_OUTPUT_SRC_TOP
		<< "\n" << B98_OUTPUT_SRC_STACK_IMP
		<< "\n";

	int xMax = codegrid[0].size();
	int yMax = codegrid.size();

	for(auto &info : stateStartPoints) {
		xPos = info.xPos;
		yPos = info.yPos;
		xd = info.xd;
		yd = info.yd;
		currState = info.state;

		sourceContent << "int state" << currState << "(struct Stack *stackPtr) {\n";

		char c;
		bool intcreated = false;
		bool stringmode = false;
		while(true) {
			if(anyStartButCurrent()) {
				sourceContent << "	return state" << startHere() << "(stackPtr);\n";
				break;

			} // if(anyStartButCurrent());

			c = codegrid[yPos][xPos];

			if(stringmode) {
				if(c == '"') {
					stringmode = false;

				} // if(c == '"');
				else {
					sourceContent << "	stackPush(stackPtr, '" << c << "');\n";

				} // else;

			} // if(stringmode);

			else if(c != ' ') {
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

				else if(c == '"') {
					stringmode = true;

				} // else if(c == '"');

				else if(c >= '0' && c <= '9') {
					sourceContent << "	stackPush(stackPtr, " << c << ");\n";

				} // else if(c >= '0' && c <= '9');

				else if(c == '+' || c == '*') {
					sourceContent << "	";

					if(!intcreated) {
						sourceContent << "int ";
						intcreated = true;

					} // if(!intcreated);

					sourceContent << "val = stackPop(stackPtr);\n"
						<< "	stackPtr->top->data " << c << "= val;\n";

				} // else if(c == '*');

				else if(c == '-' || c == '/' || c == '%') {
					sourceContent << "	";

					if(!intcreated) {
						sourceContent << "int ";
						intcreated = true;

					} // if(!intcreated);

					sourceContent << "val = stackPop(stackPtr);\n"
						<< "	stackPtr->top->data = val " << c << "= stackPtr->top->data;\n";

				} // else if(c == '-' || c == '/');

				else if(c == ':') {
					sourceContent << "	";

					if(!intcreated) {
						sourceContent << "int ";
						intcreated = true;

					} // if(!intcreated);

					sourceContent << "val = stackPop(stackPtr);\n"
						<< "	stackPush(stackPtr, val);\n"
						<< "	stackPush(stackPtr, val);\n";

				} // else if(c == ':');

				else if(c == '~') {
					sourceContent << "stackPush(stackPtr, getchar());\n";

				} // else if(c == '~');

				else if(c == ',') {
					sourceContent << "	if(putchar(stackPop(stackPtr)) == '\\n') {;\n"
						<< "		fflush(stdout);\n"
						<< "	}\n";

				} // else if(c == ',');

				else if(c == '_' || c == '|') {
					sourceContent << "	if(stackPop(stackPtr) == 0) {\n"
						<< "		return state" << (c == '_' ? startAtDelta(1, 0) : startAtDelta(0, 1)) << "(stackPtr);\n"
						<< "	} else {\n"
						<< "		return state" << (c == '_' ? startAtDelta(-1, 0) : startAtDelta(0, -1)) << "(stackPtr);\n"
						<< "	}\n";

					break;

				} // else if(c == '_');

				else if(c == 'w') {
					sourceContent << "	";

					if(!intcreated) {
						sourceContent << "int ";
						intcreated = true;

					} // if(!intcreated);

					sourceContent << "val = stackPop(stackPtr);\n"
						<< "	if(val == stackPtr->top->data) {\n"
						<< "		stackPop();\n"
						<< "		return state" << startAtDelta(xd, yd) << "(stackPtr);\n"
						<< "	} else if(val < stackPtr->top->data) {\n"
						<< "		stackPop();\n"
						<< "		return state";

					turnRight();
					sourceContent << startAtDelta(xd, yd) << "(stackPtr);\n"
						<< "	} else {\n"
						<< "		stackPop();\n"
						<< "		return state";

					turnLeft();
					turnLeft();
					sourceContent << startAtDelta(xd, yd) << "(stackPtr);\n"
						<< "	}\n";

					break;

				} // else if(c == 'w');

				else if(c == '@') {
					sourceContent << "	return 0;\n";
					break;

				} // else if(c == '@');

			} // else if(c != ' ');
 
			xPos = safeAddDelta(xPos, xd, xMax);
			yPos = safeAddDelta(yPos, yd, yMax);

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

		} // while(parsing);

		sourceContent << "\n}\n\n";

	} // for(auto &info : stateStartPoints);

	sourceContent << B98_OUTPUT_SRC_MAIN;

	std::ofstream sourceFile("output.c");
	sourceFile << sourceContent.str();

	sourceFile.close();

} // void Parser::generateSource();

B98_NS_END
