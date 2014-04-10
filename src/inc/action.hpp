#ifndef B98TOC_ACTION_HPP
#define B98TOC_ACTION_HPP

#include "defines.hpp"

#include <vector>
#include <iostream>

B98_NS_BEGIN

class Action {
public:
    Action(bool number = false, bool sum = false);

    virtual ~Action() = default;

    virtual bool isNumber();
    virtual bool isArith();

    virtual int getNumber();
    virtual char getArith();

    virtual std::vector<int>& getLinkedStates();
    virtual void setLinkedState(int index, int state);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2) = 0;

protected:
    std::vector<int> linkedStates;
    bool number, arith;
};

class PushCharAction : public Action {
public:
    PushCharAction(char c);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);

private:
    char c;
};

class PushIntAction : public Action {
public:
    PushIntAction(int c);

    virtual int getNumber();

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);

private:
    int c;
};

class PushHexAction : public Action {
public:
    PushHexAction(char c);

    virtual int getNumber();

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);

private:
    char c;
};

class ArithmeticAction : public Action {
public:
    ArithmeticAction(char c);

    virtual char getArith();

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);

private:
    char c;
};

class SwapAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class PopAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class DuplicateAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class OutputCharAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class OutputIntAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class InputCharAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class ZeroCheckAction : public Action {
public:
    ZeroCheckAction(int ifstate, int elsestate);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class CompareAction : public Action {
public:
    CompareAction(int fd, int lt, int rt);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class NextStateAction : public Action {
public:
    NextStateAction(int state);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class ReturnAction : public Action {
public:
    ReturnAction(int ret);

    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);

private:
    int ret;
};

class TablePutAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

class TableGetAction : public Action {
public:
    virtual void output(std::ostream& out, bool vars, bool& int1, bool& int2);
};

B98_NS_END

#endif
