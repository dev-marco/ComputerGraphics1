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

    static std::unordered_set<const Object *> invalid;

    bool display, markedToDestroy = false;
    Mesh *mesh, *collider = NULL;
    Background *background;
    std::unordered_set<Object *> children;
    Object *parent = NULL;
    std::valarray<double> position, speed, acceleration;

    void delayedDestroy () {

        if (this->markedToDestroy) {

            this->markedToDestroy = false;

            if (Object::isValid(this)) {

                auto children(this->children);

                this->display = false;

                this->beforeDestroy();

                this->removeParent();

                for (const auto &child : children) {
                    child->destroy(true);
                }
                this->children.clear();

                delete this->background;
                delete this->mesh;

                Object::invalid.insert(this);

                this->afterDestroy();

                delete this;
            }
        }
    }

public:

    inline static bool isValid (const Object *obj) {
        return Object::invalid.find(obj) == Object::invalid.end();
    }

    inline Object (
        const std::array<double, 3> &_position = {0.0, 0.0, 0.0},
        bool _display = true,
        Mesh *_mesh = new Mesh(),
        Background *_background = new Background(),
        const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
        const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0}
    ) : display(_display), mesh(_mesh), background(_background), position(_position.data(), 3), speed(_speed.data(), 3), acceleration(_acceleration.data(), 3) {
        Object::invalid.erase(this);
    };

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

    virtual ~Object () {}

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

    void update (double now, unsigned tick) {
        if (Object::isValid(this)) {

            auto children(this->children);

            this->beforeUpdate();

            this->position += this->speed;
            this->speed += this->acceleration;

            for (const auto &child : children) {
                child->update(now, tick);
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
        return Object::isValid(this);
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
