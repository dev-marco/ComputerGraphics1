#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include <valarray>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include "background.h"

class Mesh {

    std::valarray<double> position;
    std::vector<std::shared_ptr<Mesh>> children;

public:

    static constexpr long double PI = 3.141592653589793238462643383279502884L;

    static std::valarray<double> closestPoint2D (
        std::valarray<double> ray_start,
        std::valarray<double> ray_end,
        std::valarray<double> point
    ) {

        double A1 = ray_end[1] - ray_start[1], B1 = ray_start[0] - ray_end[0];

        if (A1 && B1) {

            double det_inv = 1.0 / (A1 * A1 + B1 * B1),
                   C1 = (ray_end[1] - ray_start[1]) * ray_start[0] + (ray_start[0] - ray_end[0]) * ray_start[1],
                   C2 = A1 * point[1] - B1 * point[0];

            return {
                (A1 * C1 - B1 * C2) * det_inv,
                (A1 * C2 + B1 * C1) * det_inv
            };
        }
        return point;
    }

    static double distance2D (std::valarray<double> point_1, std::valarray<double> point_2) {
        std::valarray<double> diff = point_1 - point_2;
        diff *= diff;
        return sqrt(diff[0] + diff[1]);
    }

    static bool collisionCircles2D (
        std::valarray<double> position_1,
        double radius_1,
        std::valarray<double> position_2,
        double radius_2
    ) {
        return Mesh::distance2D(position_1, position_2) <= (radius_1 + radius_2);
    }

    static bool collisionRectangles2D (
        std::valarray<double> position_1,
        double width_1,
        double height_1,
        std::valarray<double> position_2,
        double width_2,
        double height_2
    ) {
        return position_1[0] < (position_2[0] + width_2 ) && (position_1[0] + width_1 ) > position_2[0] &&
               position_1[1] < (position_2[1] + height_2) && (position_1[1] + height_1) > position_2[1];
    }

    static bool collisionRayCircle2D (
        std::valarray<double> ray_start,
        std::valarray<double> ray_end,
        std::valarray<double> circle_center,
        double circle_radius
    ) {
        return Mesh::distance2D(Mesh::closestPoint2D(ray_start, ray_end, circle_center), circle_center) <= circle_radius;
    }

    static bool collisionRectangleCircle2D (
        std::valarray<double> rect_top_left,
        double rect_width,
        double rect_height,
        std::valarray<double> circle_center,
        double circle_radius
    ) {
        std::valarray<double> rect_top_right = { rect_top_left[0] + rect_width, rect_top_left[1] },
                              rect_bottom_right = { rect_top_left[0] + rect_width, rect_top_left[1] + rect_height },
                              rect_bottom_left = { rect_top_left[0], rect_top_left[1] + rect_height };

        return (
            rect_top_left[0] <= circle_center[0] && circle_center[0] >= rect_bottom_right[0] &&
            rect_top_left[1] <= circle_center[1] && circle_center[1] >= rect_bottom_right[1]
        ) ||
        Mesh::collisionRayCircle2D(rect_top_left, rect_top_right, circle_center, circle_radius) ||
        Mesh::collisionRayCircle2D(rect_top_right, rect_bottom_right, circle_center, circle_radius) ||
        Mesh::collisionRayCircle2D(rect_bottom_right, rect_bottom_left, circle_center, circle_radius) ||
        Mesh::collisionRayCircle2D(rect_bottom_left, rect_top_left, circle_center, circle_radius);
    }

    Mesh (const std::array<double, 3> &_position = { 0.0, 0.0, 0.0 }) :
        position(_position.data(), 3) {};

    virtual ~Mesh () {}

    void draw (const std::valarray<double> &position, const Background *background, double ratio = 1.0) const {

        std::valarray<double> offset = position + this->position;

        this->makeDraw(offset, background, ratio);

        for (const auto &mesh : this->children) {
            mesh->draw(offset, background, ratio);
        }
    }

    inline void addChild (std::shared_ptr<Mesh> child) { this->children.push_back(child); }

    inline const std::valarray<double> &getPosition () const { return this->position; }

    inline void setPosition (const std::array<double, 3> _position) { this->position = std::valarray<double>(_position.data(), 3); }

    inline virtual void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {}

    inline virtual std::string getType () const { return "mesh"; }

};

class Polygon2D : public Mesh {

    double radius, angle;
    int sides;

public:

    inline Polygon2D (const std::array<double, 3> &_position, double _radius, int _sides, double _angle = 0.0) :
        Mesh(_position), radius(_radius), angle(_angle), sides(_sides) {};

    void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {

        double step = (Polygon2D::PI * 2.0) / static_cast<double>(this->sides);
        glBegin(GL_TRIANGLE_FAN);

        background->apply();

        for (int i = 0; i < this->sides; i++) {

            double ang = i * step + this->angle;
            glVertex3d(position[0] + this->radius * cos(ang), position[1] + this->radius * sin(ang) * ratio, position[2]);

        }

        glEnd();

    }

    inline double getRadius () const { return this->radius; }

    inline void setRadius (double _radius) { this->radius = _radius; }

    inline virtual std::string getType () const { return "polygon2d"; }

};

class Sphere2D : public Polygon2D {
public:
    Sphere2D (const std::array<double, 3> &_position, double _radius) :
        Polygon2D (_position, _radius, 360, 0.0) {}

    inline virtual std::string getType () const { return "sphere2d"; }
};

class Rectangle2D : virtual public Mesh {

    double width, height;

public:

    Rectangle2D (const std::array<double, 3> &_position, double _width, double _height) :
        Mesh(_position), width(_width), height(_height) {};

    inline void makeDraw (const std::valarray<double> &position, const Background *background, double ratio) const {

        glBegin(GL_TRIANGLE_FAN);

        background->apply();

        glVertex3d(position[0], position[1] + height * ratio, position[2]);
        glVertex3d(position[0], position[1], position[2]);
        glVertex3d(position[0] + width, position[1], position[2]);
        glVertex3d(position[0] + width, position[1] + height * ratio, position[2]);

        glEnd();
    }

    inline double getWidth () const { return this->width; }
    inline double getHeight () const { return this->height; }

    inline void setWidth (double _width) { this->width = _width; }
    inline void setHeight (double _height) { this->height = _height; }

    inline virtual std::string getType () const { return "rectangle2d"; }

};

#endif
