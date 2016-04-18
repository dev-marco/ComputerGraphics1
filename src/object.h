#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#include <memory>
#include <array>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <valarray>
#include "background.h"
#include "mesh.h"

class Object {

    bool display, exists = true, markedToDestroy = false;
    Mesh *mesh, *collider = NULL;
    Background *background;
    std::unordered_set<Object *> children;
    Object *parent = NULL;
    std::valarray<double> position, speed, acceleration;

    void delayedDestroy () {
        if (this->markedToDestroy) {

            if (Object::isValid(this)) {

                this->beforeDestroy();

                this->removeParent();
                delete this->background;
                delete this->mesh;
                for (const auto &child : this->children) {
                    child->destroy(true);
                }
                this->children.clear();
                this->exists = false;
                this->display = false;

                this->afterDestroy();
            }
        }
    }

public:

    static bool isValid (const Object *obj) {
        return obj && *obj;
    }

    inline Object (
        const std::array<double, 3> &_position = {0.0, 0.0, 0.0},
        bool _display = true,
        Mesh *_mesh = new Mesh(),
        Background *_background = new Background(),
        const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
        const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0}
    ) : display(_display), mesh(_mesh), background(_background), position(_position.data(), 3), speed(_speed.data(), 3), acceleration(_acceleration.data(), 3) {};

    void detectCollisions () {

        auto child = this->children.begin(), child_end = this->children.end();

        while (child != child_end) {
            if (Object::isValid(*child) && (*child)->collides()) {
                for (auto next = std::next(child); next != child_end; next++) {
                    if ((*child)->detectCollision(*next)) {
                        (*child)->onCollision(*next);
                        (*next)->onCollision(*child);
                    }
                }
            }
            child++;
        }
    }

    bool detectCollision (const Object *other) const {
        return false;
    }

    bool collides () {
        return this->collider != NULL;
    }

    inline void addChild (Object *obj) {
        if (Object::isValid(this) && Object::isValid(obj)) {
            obj->parent = this;
            this->children.insert(obj);
        }
    }

    inline void removeChild (Object *obj) {
        if (Object::isValid(this) && Object::isValid(obj)) {
            obj->parent = NULL;
            this->children.erase(obj);
        }
    }

    inline void setParent (Object *obj) {
        if (Object::isValid(this) && Object::isValid(obj)) {
            this->parent->addChild(obj);
        }
    }

    inline void removeParent () {
        if (Object::isValid(this) && Object::isValid(this->parent)) {
            this->parent->removeChild(this);
        }
    }

    inline const std::unordered_set<Object *> &getChildren () const {
        return this->children;
    }

    void update () {
        if (Object::isValid(this)) {

            this->beforeUpdate();

            this->position += this->speed;
            this->speed += this->acceleration;

            for (const auto &child : this->children) {
                child->update();
            }

            this->afterUpdate();

            this->delayedDestroy();
        }
    }

    void draw (double ratio = 1.0) const {
        if (Object::isValid(this)) {
            if (this->display) {

                this->beforeDraw();

                this->mesh->draw(this->position, this->background, ratio);

                for (const auto &child : this->children) {
                    child->draw(ratio);
                }

                this->afterDraw();
            }
        } else {
            std::cout << "drawing error" << std::endl;
        }
    }

    void destroy (bool now = false) {
        this->markedToDestroy = true;
        if (now) {
            this->delayedDestroy();
        }
    }

    inline operator bool () const {
        return this->exists;
    }

    virtual void onCollision (const Object *other) {}
    virtual void beforeDestroy () {}
    virtual void afterDestroy () {}
    virtual void beforeUpdate () {}
    virtual void afterUpdate () {}
    virtual void beforeDraw () const {}
    virtual void afterDraw () const {}

    virtual std::string getType () const { return "object"; }

};

#endif
