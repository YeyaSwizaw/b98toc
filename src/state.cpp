#include "inc/state.hpp"

B98_NS_BEGIN

State::State()
    : actions(), bypassTo(-1) {};

bool State::bypassed() {
    return bypassTo >= 0;
}

bool State::canBeBypassed() {
    if(actions.size() > 1) {
        return false;
    }

    return actions[0]->getLinkedStates().size() == 1;
}

void State::setBypass(int state) {
    bypassTo = state;
}

int State::getBypass() {
    return bypassTo;
}

void State::push_back(Action* action) {
    actions.push_back(action);
}

auto State::end() -> decltype(this->actions.end()) {
    return actions.end();
}

auto State::begin() -> decltype(this->actions.begin()) {
    return actions.begin();
}

auto State::operator[](int i) -> decltype(this->actions[i]) {
    return actions[i];
}

B98_NS_END
