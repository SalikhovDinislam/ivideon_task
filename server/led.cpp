#include <cerrno>
#include <stdexcept>
#include <string>
#include <sstream>
#include "led.hpp"

using namespace LED;

using std::string;
using std::stringstream;

Led::Led(State_t state, Color_t color, Rate_t rate)
: _state(state),
  _color(color)
{
    if (rate < MIN_RATE || rate > MAX_RATE) {
        throw std::range_error("LED rate is incorrect");
    }
    _rate = rate;
}

int LED::convert_str_to_state(const string &str, State_t *state)
{
    if (str == "on") {
        *state = State_t::ON;
    } else if (str == "off") {
        *state = State_t::OFF;
    } else {
        return -EINVAL;
    }

    return 0;
}

string LED::convert_state_to_str(State_t state)
{
    switch (state) {
    case State_t::ON:
        return "on";
    case State_t::OFF:
        return "off";
    }

    /* To suppress compiler's warning */
    return "";
}

State_t Led::get_state(void) const
{
    return _state;
}

string Led::get_state_str(void) const
{
    return convert_state_to_str(get_state());
}

int Led::set_state(State_t state)
{
    _state = state;
    return 0;
}

int Led::set_state(const string &str)
{
    State_t state;
    int result;

    result = convert_str_to_state(str, &state);
    if (result) {
        return result;
    }

    return set_state(state);
}

int LED::convert_str_to_color(const string &str, Color_t *color)
{
    if (str == "red") {
        *color = Color_t::RED;
    } else if (str == "green") {
        *color = Color_t::GREEN;
    } else if (str == "blue") {
        *color = Color_t::BLUE;
    } else {
        return -EINVAL;
    }

    return 0;
}

string LED::convert_color_to_str(Color_t color)
{
    switch (color) {
    case Color_t::RED:
        return "red";
    case Color_t::GREEN:
        return "green";
    case Color_t::BLUE:
        return "blue";
    }

    /* To suppress compiler's warning */
    return "";
}

Color_t Led::get_color(void) const
{
    return _color;
}

string Led::get_color_str(void) const
{
    return convert_color_to_str(get_color());
}

int Led::set_color(Color_t color)
{
    _color = color;
    return 0;
}

int Led::set_color(const string &str)
{
    Color_t color;
    int result;

    result = convert_str_to_color(str, &color);
    if (result) {
        return result;
    }

    return set_color(color);
}

int LED::convert_str_to_rate(const string &str, Rate_t *rate)
{
    stringstream ss(str);
    Rate_t tmp;
    ss >> tmp;
    if (ss.eof() && !ss.fail()) {
        *rate = tmp;
        return 0;
    }

    return -EINVAL;
}

string LED::convert_rate_to_str(Rate_t rate)
{
    stringstream ss;
    ss << rate;
    return ss.str();
}

Rate_t Led::get_rate(void) const
{
    return _rate;
}

string Led::get_rate_str(void) const
{
    return convert_rate_to_str(get_rate());
}

int Led::set_rate(Rate_t rate)
{
    if (rate < MIN_RATE || rate > MAX_RATE) {
        return -ERANGE;
    }
    _rate = rate;
    return 0;
}

int Led::set_rate(const string &str)
{
    Rate_t rate;
    int result;

    result = convert_str_to_rate(str, &rate);
    if (result) {
        return result;
    }

    return set_rate(rate);
}
