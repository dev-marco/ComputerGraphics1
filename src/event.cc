#include "event.h"

double Event::MouseMove::posx = 0.0;
double Event::MouseMove::posy = 0.0;

std::unordered_map<GLFWwindow *, std::list<std::tuple<std::function<void(GLFWwindow *, double, double, double, double)>, std::string, unsigned>>> Event::MouseMove::trigger_list;
