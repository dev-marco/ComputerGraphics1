#ifndef SRC_BRICK_H_
#define SRC_BRICK_H_

#include <memory>
#include <array>
#include <valarray>
#include "background.h"

class Brick {

    const Background& background;
    std::valarray<double> position, speed, acceleration;

public:
    Brick (const Background &_background, const std::array<double, 2> &_position, const std::array<double, 2> &_speed, const std::array<double, 2> &_acceleration) :
        background(_background), position(_position.data(), 2), speed(_speed.data(), 2), acceleration(_acceleration.data(), 2) {};
    Brick (const Background &_background, const std::array<double, 2> &_position, const std::array<double, 2> &_speed) : Brick(_background, _position, _speed, { 0.0, 0.0 }) {};
    Brick (const Background &_background, const std::array<double, 2> &_position) : Brick(_background, _position, { 0.0, 0.0 }, { 0.0, 0.0 }) {};

    void onUpdate () {
        speed += acceleration;
        position += speed;
    }

};

#endif
