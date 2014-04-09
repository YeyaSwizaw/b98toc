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
            << "#include \"stack.h\"" << std::endl << std::endl;

    /* State definitions */
    for(decltype(stateStarts)::size_type state = 0; state < stateStarts.size(); ++state) {
        content << "int state" << state << "(Stack* stack);" << std::endl;
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

    for(decltype(stateStarts)::size_type state = 0; state < stateStarts.size(); ++state) {
        content << "int state" << state << "(Stack* stack) {" << std::endl;
        if(generateState(content, state) < 0) {
            return -1;
        }
        content << "}" << std::endl << std::endl;
    }

    content << "int main(int argc, char* argv[]) {" << std::endl
            << "    Stack* stack = stackNew();" << std::endl
            << "    int ret = state0(stack);" << std::endl
            << "    stackFree(stack);" << std::endl
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

int Parser::generateState(std::ostringstream& content, int state) {
    auto& start = stateStarts[state];
    Dir* dir = start.dir;
    int x = start.x + dir->dx;
    int y = start.y + dir->dy;

    int maxX = codegrid[0].size();
    int maxY = codegrid.size();

    bool intcreated = false;
    bool stringmode = false;

    while(true) {
        char& c = codegrid[y][x];

        if(c == '"') {
            stringmode = !stringmode;
        }

        if(stringmode && c != '"') {
            state = stategrid[y][x][dir->index];
            content << "    stackPush(stack, '" << c << "');" << std::endl;
        }

        else {
            if(c == '>') {
                dir = &Dir::RIGHT;
            } else if(c == '<') {
                dir = &Dir::LEFT;
            } else if(c == '^') {
                dir = &Dir::UP;
            } else if(c == 'v') {
                dir = &Dir::DOWN;
            } else if(c == '[') {
                dir = dir->left;
            } else if(c == ']') {
                dir = dir->right;
            }

            if(stategrid[y][x][dir->index] != state) {
                content << "    return state" << stategrid[y][x][dir->index] << "(stack);" << std::endl;
                return 0;
            }

            if(c >= '0' && c <= '9') {
                content << "    stackPush(stack, " << c << ");" << std::endl;
            }

            else if(c == '+' || c == '*') {
                content << "    stackPush(stack, stackPop(stack) " << c << " stackPop(stack));" << std::endl;
            }

            else if(c == ':') {
                content << "    ";

                if(!intcreated) {
                    content << "int ";
                    intcreated = true;
                }

                content << "val = stackPop(stack);" << std::endl
                        << "    stackPush(stack, val);" << std::endl
                        << "    stackPush(stack, val);" << std::endl;
            }

            else if(c == ',') {
                content << "    if(putchar(stackPop(stack)) == '\\n') {" << std::endl
                        << "        fflush(stdout);" << std::endl
                        << "    }" << std::endl;
            }

            else if(c == '_' || c == '|') {
                decltype(stateStarts)::size_type ifstate = 0;
                for(; ifstate < stateStarts.size(); ++ifstate) {
                    if(x == stateStarts[ifstate].x 
                        && y == stateStarts[ifstate].y 
                        && stateStarts[ifstate].dir->index == (c == '_' ? Dir::RIGHT : Dir::DOWN).index) {
                        
                        break;
                    }
                }

                decltype(stateStarts)::size_type elsestate = 0;
                for(; elsestate < stateStarts.size(); ++elsestate) {
                    if(x == stateStarts[elsestate].x 
                        && y == stateStarts[elsestate].y 
                        && stateStarts[elsestate].dir->index == (c == '_' ? Dir::LEFT : Dir::UP).index) {
                        
                        break;
                    }
                }

                content << "    if(stackPop(stack) == 0) {" << std::endl
                        << "        return state" << ifstate << "(stack);" << std::endl
                        << "    } else {" << std::endl
                        << "        return state" << elsestate << "(stack);" << std::endl
                        << "    }" << std::endl;

                return 0;
            }

            else if(c == '@') {
                content << "    return 0;" << std::endl;
                return 0;
            }
        }

        x += (!stringmode && c == '#' ? 2 : 1) * dir->dx;
        y += (!stringmode && c == '#' ? 2 : 1) * dir->dy;

        if(x < 0) {
            x += maxX;
        } else if(x >= maxX) {
            x -= maxX;
        }
      
        if(y < 0) {
            y += maxY;
        } else if(y >= maxY) {
            y -= maxY;
        }
    }

    return 0;
}

B98_NS_END
