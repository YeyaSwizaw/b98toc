#include "inc/parser.hpp"

B98_NS_BEGIN

int Parser::generateOutput() {
    if(generateHeader() < 0 || generateSource() < 0) {
        return -1;
    }

    return 0;
}

int Parser::generateHeader() {
    std::ostringstream content;

    /* Header guard and includes */
    content << "#ifndef BEFUNGE_OUTPUT_H" << std::endl
            << "#define BEFUNGE_OUTPUT_H" << std::endl << std::endl
            << "#include <stdlib.h>" << std::endl
            << "#include <stdio.h>" << std::endl << std::endl
            << "#include \"stack.h\"" << std::endl;

    if(variables) {
        content << "#include \"vartable.h\"" << std::endl;
    }

    content << std::endl;

    /* State definitions */
    for(decltype(parsedStates)::size_type state = 0; state < parsedStates.size(); ++state) {
        if(!parsedStates[state].bypassed()) {
            content << "int state" << state << 
                (variables ? "(Stack* stack, Table* table);" : "(Stack* stack);") << std::endl;
        }
    }

    content << std::endl << "#endif" << std::endl;

    std::ofstream output(outHeader);

    if(!output) {
        std::cerr << "Error opening '" << outHeader << "' for writing" << std::endl;
        return -1;
    }

    output << content.str();
    output.close();

    return 0;
}

int Parser::generateSource() {
    std::ostringstream content;

    content << "#include \"" << outHeader << "\"" << std::endl << std::endl;

    for(decltype(parsedStates)::size_type state = 0; state < parsedStates.size(); ++state) {
        if(!parsedStates[state].bypassed()) {
            content << "int state" << state << 
                (variables ? "(Stack* stack, Table* table) {" : "(Stack* stack) {") << std::endl;

            bool int1 = false;
            bool int2 = false;
            for(Action* act : parsedStates[state]) {
                act->output(content, variables, int1, int2);
            }

            content << "}" << std::endl << std::endl;
        }
    }

    content << "int main(int argc, char* argv[]) {" << std::endl
            << "    Stack* stack = stackNew();" << std::endl;

    if(variables) {
        content << "    Table* table = tableNew();" << std::endl
                << "    int ret = state0(stack, table);" << std::endl
                << "    tableFree(table);" << std::endl;
    } else {
        content << "    int ret = state0(stack);" << std::endl;
    }
    
    content << "    stackFree(stack);" << std::endl
            << "    return ret;" << std::endl
            << "}" << std::endl;

    std::ofstream output(outSource);

    if(!output) {
        std::cerr << "Error opening '" << outSource << "' for writing" << std::endl;
        return -1;
    }

    output << content.str();
    output.close();

    return 0;
}

B98_NS_END
