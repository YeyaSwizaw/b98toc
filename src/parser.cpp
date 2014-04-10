#include "inc/parser.hpp"

B98_NS_BEGIN

Parser::Parser() {};

int Parser::run(int argc, char* argv[]) {
    if(parseArgs(argc, argv) < 0) {
        return -1;
    }

    if(readFile() < 0) {
        return -1;
    }

    if(verbose) {
        std::cout << "Input File:   " << inFile << std::endl
                  << "Output Files: " << outSource << ", " << outHeader << std::endl << std::endl;

        std::cout << "File Contents:" << std::endl;
        for(decltype(codegrid)::size_type y = 0; y < codegrid.size(); ++y) {
            for(decltype(codegrid)::size_type x = 0; x < codegrid[y].size(); ++x) {
                std::cout << codegrid[y][x];
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    if(createStates() < 0) {
        return -1;
    }

    if(verbose) {
        std::cout << std::endl << "State Starts:" << std::endl;
        for(decltype(stateStarts)::size_type i = 0; i < stateStarts.size(); ++i) {
            std::cout << i << ": (" << stateStarts[i].x << ", " << stateStarts[i].y << ") delta(" << stateStarts[i].dir->dx << ", " << stateStarts[i].dir->dy << ")" << std::endl;
        }
        std::cout << std::endl;
    }

    if(parse() < 0) {
        return -1;
    }

    if(bypass || eval) {
        if(optimize() < 0) {
            return -1;
        }
    }

    if(generateOutput() < 0) {
        return -1;
    }

    return 0;
}

int Parser::parseArgs(int argc, char* argv[]) {
    try {
        TCLAP::CmdLine cmd("Befunge98 to C parser");

        TCLAP::SwitchArg verboseArg(
            "v", "verbose",
            "Produce console output while parsing");

        TCLAP::SwitchArg bypassArg(
            "", "no-bypasss",
            "Don't optimize by bypassing empty states",
            true);

        TCLAP::SwitchArg evalArg(
            "", "no-eval",
            "Don't optimize by evaluating constants",
            true);

        TCLAP::ValueArg<std::string> outFileArg(
            "o", "output", 
            "The name for the produced .c and .h files", 
            false, "output", 
            "filename without extension");
    
        TCLAP::UnlabeledValueArg<std::string> inFileArg(
            "input",
            "The filename to read input from",
            true, "",
            "filename");

        cmd.add(verboseArg);
        cmd.add(bypassArg);
        cmd.add(evalArg);
        cmd.add(outFileArg);
        cmd.add(inFileArg);
        cmd.parse(argc, argv);

        verbose = verboseArg.getValue();
        bypass = bypassArg.getValue();
        eval = evalArg.getValue();
        inFile = inFileArg.getValue();
        outSource = outFileArg.getValue() + ".c";
        outHeader = outFileArg.getValue() + ".h";
    }
    catch(TCLAP::ArgException& e) {
        std::cerr << "Argument Error (Arg " << e.argId() << ")" << std::endl << e.error() << std::endl;
        return -1;
    }

    return 0;
}

int Parser::readFile() {
    std::ifstream srcfile(inFile);
    if(!srcfile) {
        std::cerr << "Unable to read file: " << inFile << std::endl;
        return -1;
    }

    decltype(codegrid)::size_type maxsize = 0;
    codegrid.push_back(std::vector<char>());

    char ch;
    while(srcfile >> std::noskipws >> ch) {
        if(ch == '\n') {
            maxsize = std::max(maxsize, codegrid.back().size());
            codegrid.push_back(std::vector<char>());
        } else {
            codegrid.back().push_back(ch);
        }
    }

    srcfile.close();

    if(codegrid.back().empty()) {
        codegrid.pop_back();
    }

    for(auto& v : codegrid) {
        v.resize(maxsize, ' ');

        stategrid.push_back(std::vector<std::vector<int>>());
        stategrid.back().resize(maxsize, { -1, -1, -1, -1 });
    }

    return 0;
}

int Parser::createStates() {
    Dir* dir = &Dir::RIGHT;
    int x = 0;
    int y = 0;

    int maxX = codegrid[0].size();
    int maxY = codegrid.size();

    int state = 0;
    stateStarts.push_back({-1, 0, &Dir::RIGHT});

    std::queue<std::pair<Location, bool>> stateQueue;

    bool bulldozing = false;
    bool stringmode = false;
    bool getnextchar = false;
    bool nextstate = false;

    while(true) {
        char c = codegrid[y][x];

        if(getnextchar) {
            getnextchar = false;
        }

        if(!stringmode) {
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
        }

        if(c == '\'') {
            getnextchar = true;
        }

        else if(c == '"') {
            stringmode = !stringmode;
        }

        else if(!stringmode) {
            if(c == '@') {
                nextstate = true;
            } 

            else if(c == '_' || c == '|') {
                bool can = true;
                for(auto& loc : stateStarts) {
                    if(x == loc.x && y == loc.y) {
                        can = false;
                        break;
                    }
                }

                if(can) {
                    stateQueue.push({{x, y, c == '_' ? &Dir::LEFT : &Dir::UP}, false});
                    stateQueue.push({{x, y, c == '_' ? &Dir::RIGHT : &Dir::DOWN}, false});

                    if(verbose) {
                        std::cout << "Added " << c << " at (" << x << ", " << y << ") to state queue" << std::endl;
                    }
                }

                nextstate = true;
            }

            else if(c == 'w') {
                bool can = true;
                for(auto& loc : stateStarts) {
                    if(x == loc.x && y == loc.y) {
                        can = false;
                        break;
                    }
                }

                if(can) {
                    stateQueue.push({{x, y, dir}, false});
                    stateQueue.push({{x, y, dir->left}, false});
                    stateQueue.push({{x, y, dir->right}, false});

                    if(verbose) {
                        std::cout << "Added w at (" << x << ", " << y << ") to state queue" << std::endl;
                    }
                }

                nextstate = true;
            }
        }
        
        if(stategrid[y][x][dir->index] >= 0) {
            bool starthere = false;

            auto& here = stateStarts[stategrid[y][x][dir->index]];

            if(x - here.dir->dx == here.x
                && y - here.dir->dy == here.y
                && dir->index == here.dir->index) {

                starthere = true;

                if(verbose) {
                    std::cout << "Met start of state " << stategrid[y][x][dir->index] << " at (" << x << ", " << y << ")" << std::endl;
                }
            }

            if(starthere) {
                nextstate = true;
            } else {
                if(!bulldozing) {
                    if(verbose) {
                        std::cout << "Added bulldozer at (" << x << ", " << y << ") to state queue" << std::endl;

                    }
                    stateQueue.push({{x - dir->dx, y - dir->dy, dir}, true});
                    nextstate = true;
                } else {
                    if(verbose) {
                        std::cout << c << " at (" << x << ", " << y << ") delta(" << dir->dx << ", " << dir->dy << ") -> " << state << std::endl;
                    }
                    stategrid[y][x][dir->index] = state;
                }
            }
        } else {
            if(verbose) {
                std::cout << c << " at (" << x << ", " << y << ") delta(" << dir->dx << ", " << dir->dy << ") -> " << state << std::endl;
            }
            stategrid[y][x][dir->index] = state;
        }

        if(nextstate) {
            if(stateQueue.empty()) {
                break;
            }

            auto& next = stateQueue.front();
            x = next.first.x + next.first.dir->dx;
            y = next.first.y + next.first.dir->dy;
            dir = next.first.dir;
            bulldozing = next.second;

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
    
            stateStarts.push_back(std::move(next.first));
            stateQueue.pop();
            state++;

            nextstate = false;
        } else {
            x += (getnextchar || (!stringmode && c == '#') ? 2 : 1) * dir->dx;
            y += (getnextchar || (!stringmode && c == '#') ? 2 : 1) * dir->dy;

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
    }

    return 0;
}

int Parser::parse() {
    for(decltype(stateStarts)::size_type state = 0; state < stateStarts.size(); ++state) {
        parsedStates.push_back(State());
        if(parseState(state) < 0) {
            return -1;
        }
    }

    return 0;
}

int Parser::parseState(int state) {
    auto& start = stateStarts[state];
    Dir* dir = start.dir;

    int maxX = codegrid[0].size();
    int maxY = codegrid.size();

    auto addSafe = [](int val, int d, int max) { 
        val += d;
        if(val < 0) {
            val += max;
        } else if(val >= max) {
            val -= max;
        }
        return val;
    };

    int x = addSafe(start.x, dir->dx, maxX);
    int y = addSafe(start.y, dir->dy, maxY);

    auto stateWithStart = [&x, &y, this](Dir* dir) {
        decltype(stateStarts)::size_type res = 0;
        for(; res < stateStarts.size(); ++res) {
            if(x == stateStarts[res].x 
                && y == stateStarts[res].y 
                && stateStarts[res].dir->index == dir->index) {

                break;
            }
        }
        return res;
    };

    bool stringmode = false;
    bool getnextchar = false;

    while(true) {
        char& c = codegrid[y][x];

        if(c == '"') {
            stringmode = !stringmode;
        }

        if(stringmode && c != '"') {
            state = stategrid[y][x][dir->index];
            parsedStates.back().push_back(new PushCharAction(c));
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
                parsedStates.back().push_back(new NextStateAction(stategrid[y][x][dir->index]));
                return 0;
            }

            if(c == '\'') {
                getnextchar = true;
            }

            else if(c >= '0' && c <= '9') {
                parsedStates.back().push_back(new PushIntAction(c - '0'));
            }

            else if(c >= 'a' && c <= 'f') {
                parsedStates.back().push_back(new PushHexAction(c));
            }

            else if(c == '-' || c == '/' || c == '%' || c == '+' || c == '*' ) {
                parsedStates.back().push_back(new ArithmeticAction(c));
            }

            else if(c == '\\') {
                parsedStates.back().push_back(new SwapAction());
            }

            else if(c == '$') {
                parsedStates.back().push_back(new PopAction());
            }

            else if(c == ':') {
                parsedStates.back().push_back(new DuplicateAction());
            }

            else if(c == ',') {
                parsedStates.back().push_back(new OutputCharAction());
            }

            else if(c == '.') {
                parsedStates.back().push_back(new OutputIntAction());
            }

            else if(c == '~') {
                parsedStates.back().push_back(new InputCharAction());
            }

            else if(c == '_' || c == '|') {
                parsedStates.back().push_back(new ZeroCheckAction(
                    stateWithStart(c == '_' ? &Dir::RIGHT : &Dir::DOWN),
                    stateWithStart(c == '_' ? &Dir::LEFT : &Dir::UP)));
                return 0;
            }

            else if(c == 'w') {
                parsedStates.back().push_back(new CompareAction(
                    stateWithStart(dir),
                    stateWithStart(dir->left),
                    stateWithStart(dir->right)));
                return 0;
            }

            else if(c == '@') {
                parsedStates.back().push_back(new ReturnAction(0));
                return 0;
            }
        }

        if(getnextchar) {
            x = addSafe(x, dir->dx, maxX);
            y = addSafe(y, dir->dy, maxY);

            parsedStates.back().push_back(new PushCharAction(codegrid[y][x]));

            x = addSafe(x, dir->dx, maxX);
            y = addSafe(y, dir->dy, maxY);
            getnextchar = false;
        } else {
            x = addSafe(x, (!stringmode && c == '#' ? 2 : 1) * dir->dx, maxX);
            y = addSafe(y, (!stringmode && c == '#' ? 2 : 1) * dir->dy, maxY);
        }
    }

    return 0;
}

int Parser::optimize() {
    if(verbose) {
        std::cout << "Running Optimization Step:" << std::endl;

    }

    auto attemptEval = [this](State& state, int i) {
        std::deque<int> stack;
        stack.push_front(state[i]->getNumber());

        bool done = false;

        int i2 = i + 1;
        while(i2 < state.size() - 1) {
            if(state[i2]->isNumber()) {
                stack.push_front(state[i2]->getNumber());
            } else {
                if(state[i2]->isArith()) {
                    if(stack.size() >= 2) {
                        char c = state[i2]->getArith();
                        if(c == '*') {
                            int v1 = stack.front();
                            stack.pop_front();

                            if(verbose) {
                                std::cout << "Evaluating " << stack.front() << " * " << v1 << " = " << (stack.front() * v1) << std::endl;
                            }

                            stack.front() *= v1;
                            done = true;
                        }  
                        
                        else if(c == '+') {
                            int v1 = stack.front();
                            stack.pop_front();

                            if(verbose) {
                                std::cout << "Evaluating " << stack.front() << " + " << v1 << " = " << (stack.front() + v1) << std::endl;
                            }

                            stack.front() += v1;
                            done = true;

                        } 
                        
                        else if(c == '/') {
                            int v1 = stack.front();
                            stack.pop_front();

                            if(verbose) {
                                std::cout << "Evaluating " << stack.front() << " / " << v1 << " = " << (stack.front() / v1) << std::endl;
                            }

                            stack.front() /= v1;
                            done = true;

                        }

                        else if(c == '-') {
                            int v1 = stack.front();
                            stack.pop_front();

                            if(verbose) {
                                std::cout << "Evaluating " << stack.front() << " - " << v1 << " = " << (stack.front() - v1) << std::endl;
                            }

                            stack.front() -= v1;
                            done = true;
                        }

                        else if(c == '%') {
                            int v1 = stack.front();
                            stack.pop_front();

                            if(verbose) {
                                std::cout << "Evaluating " << stack.front() << " % " << v1 << " = " << (stack.front() % v1) << std::endl;
                            }

                            stack.front() %= v1;
                            done = true;
                        }
                        
                        else {
                            // NYI
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }

            i2++;
        }

        if(done) {
            for(int a = i; a < i2; ++a) {
                delete state[a];
                if(a - i <= stack.size()) {
                    state[a] = new PushIntAction(stack.back());
                    stack.pop_back();
                } else {
                    state.getVect().erase(state.begin() + a);
                    a--;
                    i2--;
                }
            }
        }
    };

    if(bypass) {
        int i = 0;
        for(auto& state : parsedStates) {
            if(state.canBeBypassed()) {
                int s = i;
                do {
                    s = parsedStates[s][0]->getLinkedStates()[0];
                } while(parsedStates[s].canBeBypassed());
                state.setBypass(s);

                if(verbose) {
                    std::cout << "Adding state bypass: " << i << " -> " << s << std::endl;
                }
            }

            ++i;
        }
    }

    for(auto& state : parsedStates) {
        if(!state.bypassed()) {
            for(int i = 0; i < state.size(); ++i) {
                if(eval) {
                    if(state[i]->isNumber()) {
                        attemptEval(state, i);
                    }
                }

                if(bypass) {
                    int link = 0;
                    for(int s : state[i]->getLinkedStates()) {
                        if(parsedStates[s].bypassed()) {
                            state[i]->setLinkedState(link, parsedStates[s].getBypass());
                        }
                        ++link;
                    }
                }
            }
        }
    }

    return 0;
}

Parser::~Parser() {
    for(auto& state : parsedStates) {
        for(auto action : state) {
            delete action;
        }
    }
}

B98_NS_END
