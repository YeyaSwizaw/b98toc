#include "inc/dir.hpp"

B98_NS_BEGIN

Dir Dir::UP(0, 0, -1, &Dir::DOWN, &Dir::LEFT, &Dir::RIGHT);
Dir Dir::DOWN(1, 0, 1, &Dir::UP, &Dir::RIGHT, &Dir::LEFT);
Dir Dir::LEFT(2, -1, 0, &Dir::RIGHT, &Dir::DOWN, &Dir::UP);
Dir Dir::RIGHT(3, 1, 0, &Dir::LEFT, &Dir::UP, &Dir::DOWN);

B98_NS_END
