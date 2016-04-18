#ifndef SRC_BRICK_H_
#define SRC_BRICK_H_

#include <memory>
#include <array>
#include <valarray>
#include "object.h"

class Brick : public Object {
public:
    Brick (
        const std::array<double, 3> &_position,
        double _width,
        double _height,
        Background *_background,
        const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
        const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0}
    ) : Object(_position, true, new Rectangle2D({0.0, 0.0, 0.0}, _width, _height), _background, _speed, _acceleration) {}

    std::string getType () const { return "brick"; }

};

#endif
