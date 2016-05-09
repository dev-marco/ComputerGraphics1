#include "game.h"

namespace Breakout {

    Game::Game (Engine::Window &_window, std::vector<std::string> _stages)
    : window(_window) {
        for (const auto &stage : _stages) {
            this->stages.push(new Stage(this->window, stage));
        }
    }

    void Game::clear (void) {
        while (!this->stages.empty()) {
            delete this->stages.front();
            this->stages.pop();
        }
    }
};
