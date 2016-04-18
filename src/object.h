#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#include <memory>
#include <array>
#include <vector>
#include <iostream>
#include <valarray>
#include "background.h"
#include "mesh.h"

class Object {

    bool display;
    const Mesh *mesh;
    const Background *background;
    std::vector<std::reference_wrapper<Object>> children;
    std::valarray<double> position, speed, acceleration;

public:

    Object (
        const std::array<double, 3> &_position = {0.0, 0.0, 0.0},
        bool _display = true,
        const Mesh *_mesh = &Mesh::NONE,
        const Background *_background = &Background::NONE,
        const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
        const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0}
    ) :
        display(_display), mesh(_mesh), background(_background), position(_position.data(), 3), speed(_speed.data(), 3), acceleration(_acceleration.data(), 3) {};

    void detectCollision (const Object &other) const {
        // this->collider.detectCollision(other.getCollider());
    }

    void addChild (Object &obj) {
        this->children.push_back(obj);
    }

    void update () {
        this->position += this->speed;
        this->speed += this->acceleration;

        for (const auto &child : this->children) {
            child.get().update();
        }
    }

    void draw (double ratio = 1.0) const {
        if (this->display) {
            this->mesh->draw(this->position, this->background, ratio);

            for (const auto &child : this->children) {
                child.get().draw(ratio);
            }
        }
    }

};

#endif
