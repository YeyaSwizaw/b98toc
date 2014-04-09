#ifndef B98TOC_DIR_HPP
#define B98TOC_DIR_HPP

#include "defines.hpp"

B98_NS_BEGIN

struct Dir {
    int index;
    int dx, dy;

    Dir* opposite;
    Dir* left;
    Dir* right;

    static Dir UP;
    static Dir DOWN;
    static Dir LEFT;
    static Dir RIGHT;

private:
    Dir(int index, int dx, int dy, Dir* o, Dir* l, Dir* r)
        : index(index), dx(dx), dy(dy), opposite(o), left(l), right(r) {};
};


B98_NS_END

#endif
