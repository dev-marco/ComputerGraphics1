#ifndef SRC_BACKGROUND_H_
#define SRC_BACKGROUND_H_

#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>
#include "color.h"

class Background {

    public:
        Background () {};
        virtual void apply () const {};
        static const Background NONE;
};

class BackgroundColor : virtual public Background {

    const Color color;

public:
    BackgroundColor (const Color &_color) : color(_color) {};

    void apply () const {
        this->color.apply();
    }
};

#endif
