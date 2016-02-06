#pragma once

#include <string>

namespace LED {
using std::string;

enum class State_t {
    ON,
    OFF
};

enum class Color_t {
    RED,
    GREEN,
    BLUE
};

typedef unsigned Rate_t;

int convert_str_to_state(const string& str, State_t *state);
string convert_state_to_str(State_t state);

int convert_str_to_color(const string& str, Color_t *color);
string convert_color_to_str(Color_t color);

int convert_str_to_rate(const string& str, Rate_t *rate);
string convert_rate_to_str(Rate_t rate);

class Led {
private:
    State_t _state;
    Color_t _color;
    Rate_t _rate;

public:
    Led(State_t state, Color_t color, Rate_t rate);

    State_t get_state(void) const;
    string get_state_str(void) const;
    int set_state(State_t state);
    int set_state(const string& str);

    Color_t get_color(void) const;
    string get_color_str(void) const;
    int set_color(Color_t color);
    int set_color(const string& str);

    Rate_t get_rate(void) const;
    string get_rate_str(void) const;
    int set_rate(Rate_t rate);
    int set_rate(const string& str);

    static const Rate_t MIN_RATE = 0;
    static const Rate_t MAX_RATE = 5;
};
} /* namespace LED */
