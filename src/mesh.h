#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include <valarray>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>
#include <cmath>
#include "background.h"

class Mesh {

    std::valarray<double> position;
    std::vector<std::shared_ptr<Mesh>> children;
    bool collides = true;

public:
    static constexpr double PI = 3.14159265359;

    Mesh (const std::array<double, 3> &_position = { 0.0, 0.0, 0.0 }, bool _collides = true) :
        position(_position.data(), 3), collides(_collides) {};

    void draw (const std::valarray<double> &position, const Background *background, double ratio = 1.0) const {

        this->makeDraw(position + this->position, background, ratio);

        for (const auto &mesh : this->children) {
            mesh->draw(position + this->position, background, ratio);
        }
    }

    void addChild (std::shared_ptr<Mesh> child) {
        this->children.push_back(child);
    }

    const std::valarray<double> &getPosition () const {
        return this->position;
    }

    virtual void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {};

    static const Mesh NONE;

};

class Polygon2D : public Mesh {

    double radius, angle;
    int sides;

public:

    Polygon2D (const std::array<double, 3> &_position, double _radius, int _sides, double _angle = 0.0, bool _collides = true) :
        Mesh(_position, _collides), radius(_radius), angle(_angle), sides(_sides) {};

    void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {

        double step = (PI * 2.0) / (double) this->sides;
        glBegin(GL_TRIANGLE_FAN);

        background->apply();

        for (int i = 0; i < this->sides; i++) {

            double ang = i * step + this->angle;
            glVertex3d(position[0] + this->radius * cos(ang), position[1] + this->radius * sin(ang) * ratio, position[2]);

        }

        glEnd();

    }

};

class Sphere2D : public Polygon2D {
public:
    Sphere2D (const std::array<double, 3> &_position, double _radius, bool _collides = true) :
        Polygon2D (_position, _radius, 360, 0.0, _collides) {}
};

class Rectangle2D : virtual public Mesh {

    double width, height;

public:

    Rectangle2D (const std::array<double, 3> &_position, double _width, double _height, bool _collides = true) :
        Mesh(_position, _collides), width(_width), height(_height) {};

    void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {

        glBegin(GL_TRIANGLE_FAN);

        background->apply();

        glVertex3d(position[0], position[1] + height * ratio, position[2]);
        glVertex3d(position[0], position[1], position[2]);
        glVertex3d(position[0] + width, position[1], position[2]);
        glVertex3d(position[0] + width, position[1] + height * ratio, position[2]);

        glEnd();
    }

};

#endif
