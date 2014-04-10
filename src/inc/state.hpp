#ifndef B98TOC_STATE_HPP
#define B98TOC_STATE_HPP

#include "defines.hpp"
#include "action.hpp"

B98_NS_BEGIN

class State {
private:
    std::vector<Action*> actions;

    int bypassTo;

public:
    State();

    bool bypassed();
    bool canBeBypassed();
    void setBypass(int state);
    int getBypass();

    void push_back(Action* action);

    auto end() -> decltype(actions.end());
    auto begin() -> decltype(actions.begin());
    auto operator[](int i) -> decltype(actions[i]);
};

B98_NS_END

#endif
