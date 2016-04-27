#ifndef SRC_BREAKOUT_BRICK_H_
#define SRC_BREAKOUT_BRICK_H_

#include <memory>
#include <array>
#include <valarray>
#include "mesh.h"
#include "object.h"

namespace Breakout {

    class Brick : public Object {

        double width, height;

    public:
        Brick (
            const std::array<double, 3> &_position,
            double _width,
            double _height,
            Background *_background,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0}
        ) : Object(_position, true, new Rectangle2D({0.0, 0.0, 0.0}, _width, _height), _background, _speed, _acceleration), width(_width), height(_height) {}

        std::string getType () const { return "brick"; }

        inline double getWidth () const { return this->width; }
        inline double getheight () const { return this->height; }

    };

}

#endif
