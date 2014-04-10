#include "inc/action.hpp"

B98_NS_BEGIN

std::vector<int>& Action::getLinkedStates() {
    return linkedStates;
}

void Action::setLinkedState(int index, int state) {
    linkedStates[index] = state;
}

PushCharAction::PushCharAction(char c)
    : c(c) {}

void PushCharAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    stackPush(stack, '";
    if(c == '\'' || c == '\\') {
        out << "\\";
    }
    out << c << "');" << std::endl;
}

PushIntAction::PushIntAction(char c)
    : c(c) {}

void PushIntAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    stackPush(stack, " << c << ");" << std::endl;
}

PushHexAction::PushHexAction(char c)
    : c(c) {}

void PushHexAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    stackPush(stack, 0x" << c << ");" << std::endl;
}

ArithmeticAction::ArithmeticAction(char c)
    : c(c) {}

void ArithmeticAction::output(std::ostream& out, bool& int1, bool& int2) {
    if(c == '+' || c == '*') {
        out << "    stackPush(stack, stackPop(stack) " << c << " stackPop(stack));" << std::endl;
    } else {
        out << "    ";
        if(!int1) {
            out << "int ";
            int1 = true;
        }
        out << "val = stackPop(stack);" << std::endl
            << "    stackPush(stack, stackPop(stack) " << c << " val);" << std::endl;
    }
}

void SwapAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    ";
    if(!int1) {
        out << "int ";
        int1 = true;
    }
    out << "val = stackPop(stack);" << std::endl << "    ";
    if(!int2) {
        out << "int ";
        int2 = true;
    }
    out << "val2 = stackPop(stack);" << std::endl
        << "    stackPush(stack, val);" << std::endl
        << "    stackPush(stack, val2);" << std::endl;
}

void PopAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    stackPop(stack);" << std::endl;
}

void DuplicateAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    ";
    if(!int1) {
        out << "int ";
        int1 = true;
    }
    out << "val = stackPop(stack);" << std::endl
        << "    stackPush(stack, val);" << std::endl
        << "    stackPush(stack, val);" << std::endl;
}

void OutputCharAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    putchar(stackPop(stack));" << std::endl;
}

void OutputIntAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    printf(\"%d\", stackPop(stack));" << std::endl;
}

void InputCharAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    stackPush(stack, getchar());" << std::endl;
}

ZeroCheckAction::ZeroCheckAction(int ifstate, int elsestate) {
    linkedStates.push_back(ifstate);
    linkedStates.push_back(elsestate);
}

void ZeroCheckAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    if(stackPop(stack) == 0) {" << std::endl
        << "        return state" << linkedStates[0] << "(stack);" << std::endl
        << "    } else {" << std::endl
        << "        return state" << linkedStates[1] << "(stack);" << std::endl
        << "    }" << std::endl;
}

CompareAction::CompareAction(int fd, int lt, int rt) {
    linkedStates.push_back(fd);
    linkedStates.push_back(rt);
    linkedStates.push_back(lt);
}

void CompareAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    ";
    if(!int1) {
        out << "int ";
        int1 = true;
    }
    out << "val = stackPop(stack);" << std::endl << "    ";
    if(!int2) {
        out << "int ";
        int2 = true;
    }
    out << "val2 = stackPop(stack);" << std::endl
        << "    if(val == val2) {" << std::endl
        << "        return state" << linkedStates[0] << "(stack);" << std::endl
        << "    } else if(val < val2) {" << std::endl
        << "        return state" << linkedStates[1] << "(stack);" << std::endl
        << "    } else {" << std::endl
        << "        return state" << linkedStates[2] << "(stack);" << std::endl
        << "    }" << std::endl;
}

NextStateAction::NextStateAction(int state)
    : state(state) {};

void NextStateAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    return state" << state << "(stack);" << std::endl;
}

ReturnAction::ReturnAction(int ret)
    : ret(ret) {};

void ReturnAction::output(std::ostream& out, bool& int1, bool& int2) {
    out << "    return " << ret << ";" << std::endl;
}

B98_NS_END
