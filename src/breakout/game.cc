#include "game.h"

namespace Breakout {

    Game::Game (Engine::Window &_window, std::vector<std::string> _stages)
    : window(_window) {
        for (const auto &stage : _stages) {
            this->stages.push(new Stage(this->window, stage));
        }

        this->sound_win.load("audio/effects/youwin.ogg");
        this->sound_lose.load("audio/effects/youlose.ogg");

        this->texture_win = loadPNG("images/youwin.png");
        this->texture_lose = loadPNG("images/youlose.png");
    }

    void Game::clear (void) {
        while (!this->stages.empty()) {
            delete this->stages.front();
            this->stages.pop();
        }
    }
};
