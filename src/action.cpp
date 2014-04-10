#include "inc/action.hpp"

B98_NS_BEGIN

Action::Action(bool number, bool arith) 
    : number(number), arith(arith) {};

bool Action::isNumber() {
    return number;
}

bool Action::isArith() {
    return arith;
}

int Action::getNumber() {
    return 0;
}

char Action::getArith() {
    return ' ';
}

std::vector<int>& Action::getLinkedStates() {
    return linkedStates;
}

void Action::setLinkedState(int index, int state) {
    linkedStates[index] = state;
}

PushCharAction::PushCharAction(char c)
    : Action(), c(c) {}

void PushCharAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    stackPush(stack, '";
    if(c == '\'' || c == '\\') {
        out << "\\";
    }
    out << c << "');" << std::endl;
}

PushIntAction::PushIntAction(int c)
    : Action(true), c(c) {}

int PushIntAction::getNumber() {
    return c;
}

void PushIntAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    stackPush(stack, " << c << ");" << std::endl;
}

PushHexAction::PushHexAction(char c)
    : Action(true), c(c) {}

int PushHexAction::getNumber() {
    return c - 'W';
}

void PushHexAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    stackPush(stack, 0x" << c << ");" << std::endl;
}

ArithmeticAction::ArithmeticAction(char c)
    : Action(false, true), c(c) {}

char ArithmeticAction::getArith() {
    return c;
}

void ArithmeticAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
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

void SwapAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
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

void PopAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    stackPop(stack);" << std::endl;
}

void DuplicateAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    ";
    if(!int1) {
        out << "int ";
        int1 = true;
    }
    out << "val = stackPop(stack);" << std::endl
        << "    stackPush(stack, val);" << std::endl
        << "    stackPush(stack, val);" << std::endl;
}

void OutputCharAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    putchar(stackPop(stack));" << std::endl;
}

void OutputIntAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    printf(\"%d\", stackPop(stack));" << std::endl;
}

void InputCharAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    stackPush(stack, getchar());" << std::endl;
}

ZeroCheckAction::ZeroCheckAction(int ifstate, int elsestate) {
    linkedStates.push_back(ifstate);
    linkedStates.push_back(elsestate);
}

void ZeroCheckAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    if(stackPop(stack) == 0) {" << std::endl
        << "        return state" << linkedStates[0] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl
        << "    } else {" << std::endl
        << "        return state" << linkedStates[1] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl
        << "    }" << std::endl;
}

CompareAction::CompareAction(int fd, int lt, int rt) {
    linkedStates.push_back(fd);
    linkedStates.push_back(rt);
    linkedStates.push_back(lt);
}

void CompareAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
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
        << "        return state" << linkedStates[0] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl
        << "    } else if(val < val2) {" << std::endl
        << "        return state" << linkedStates[1] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl
        << "    } else {" << std::endl
        << "        return state" << linkedStates[2] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl
        << "    }" << std::endl;
}

NextStateAction::NextStateAction(int state) {
    linkedStates.push_back(state);
}

void NextStateAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    return state" << linkedStates[0] << "(stack" << (vars ? ", table" : "" ) << ");" << std::endl;
}

ReturnAction::ReturnAction(int ret)
    : Action(), ret(ret) {};

void ReturnAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    out << "    return " << ret << ";" << std::endl;
}

void TablePutAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
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
        << "    tablePut(table, val2, val, stackPop(stack));" << std::endl;
}

void TableGetAction::output(std::ostream& out, bool vars, bool& int1, bool& int2) {
    if(!int1) {
        out << "int val;" << std::endl;
        int1 = true;
    }
    out << "    ";
    if(!int2) {
        out << "int ";
        int2 = true;
    }
    out << "val2 = stackPop(stack);" << std::endl
        << "    if(tableGet(table, stackPop(stack), val2, &val) == false) {" << std::endl
        << "        val = ' ';" << std::endl
        << "    }" << std::endl
        << "    stackPush(stack, val);" << std::endl;
}

B98_NS_END
