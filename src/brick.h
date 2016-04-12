#ifndef SRC_BRICK_H_
#define SRC_BRICK_H_

#include <memory>
#include "background.h"

class Brick {

    const Background& background;

    public:
        Brick (const Background &_background) : background(_background) {};

};

#endif
