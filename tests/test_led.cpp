#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "led.hpp"

using namespace LED;

using std::string;
using std::stringstream;

int main()
{
    Rate_t min_rate = Led::MIN_RATE;
    Rate_t max_rate = Led::MAX_RATE;
    assert(min_rate <= max_rate);

    bool failed = true;
    try {
        delete new Led(State_t::ON, Color_t::BLUE, max_rate + 1);
    } catch (...) {
        failed = false;
    }
    assert(!failed);

    failed = true;
    try {
        delete new Led(State_t::ON, Color_t::BLUE, min_rate - 1);
    } catch (...) {
        failed = false;
    }
    assert(!failed);

    Rate_t rate = min_rate + (max_rate - min_rate) / 2;
    Led l(State_t::ON, Color_t::BLUE, rate);

    assert(l.get_state() == State_t::ON);
    assert(l.set_state(State_t::OFF) == 0);
    assert(l.set_state(State_t::OFF) == 0);
    assert(l.get_state_str() == "off");
    assert(l.set_state("on") == 0);
    assert(l.get_state_str() == "on");
    assert(l.get_state() == State_t::ON);

    assert(l.set_state("") != 0);
    assert(l.set_state("42") != 0);
    assert(l.set_state("off off") != 0);
    assert(l.get_state_str() == "on");

    assert(l.get_color() == Color_t::BLUE);
    assert(l.set_color(Color_t::RED) == 0);
    assert(l.set_color(Color_t::GREEN) == 0);
    assert(l.get_color_str() == "green");
    assert(l.set_color("red") == 0);
    assert(l.get_color_str() == "red");
    assert(l.get_color() == Color_t::RED);

    assert(l.set_color("") != 0);
    assert(l.set_color("blu") != 0);
    assert(l.set_color("red red") != 0);
    assert(l.get_color_str() == "red");

    stringstream ss;
    assert(l.get_rate() == rate);
    assert(l.set_rate(max_rate) == 0);
    assert(l.set_rate(min_rate) == 0);
    ss << min_rate;
    assert(l.get_rate_str() == ss.str());
    if (min_rate < max_rate) {
        rate = min_rate + 1;
        ss.str(string());
        ss << rate;
        assert(l.set_rate(ss.str()) == 0);
        assert(l.get_rate_str() == ss.str());
        assert(l.get_rate() == rate);
    } else {
        rate = min_rate;
    }

    assert(l.set_rate("") != 0);
    assert(l.set_rate("-1") != 0);
    assert(l.set_rate("s2s") != 0);
    assert(l.set_rate("2z") != 0);
    assert(l.set_rate("42") != 0);
    ss.str("");
    ss << min_rate << " ";
    assert(l.set_rate(ss.str()) != 0);
    assert(l.set_rate(max_rate + 1) != 0);
    assert(l.set_rate(min_rate - 1) != 0);
    assert(l.get_rate() == rate);

    return 0;
}
