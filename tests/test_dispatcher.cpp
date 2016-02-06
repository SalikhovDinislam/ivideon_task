#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "led.hpp"
#include "dispatcher.hpp"

using namespace LED;
using namespace Dispatcher;

using std::string;
using std::stringstream;

int main()
{
using namespace std;
    Rate_t min_rate = Led::MIN_RATE;
    Rate_t max_rate = Led::MAX_RATE;
    Rate_t rate = min_rate + (max_rate - min_rate) / 2;
    LedDispatcher d(new Led(State_t::ON, Color_t::BLUE, rate));

    stringstream ss, ss1;
    string feedback;

    assert(d.handle_command("get-led-state", &feedback) == 0 && feedback == "on");
    assert(d.handle_command("set-led-state off", &feedback) == 0 && feedback == "on");
    assert(d.handle_command("get-led-state", &feedback) == 0 && feedback == "off");
    assert(d.handle_command("  set-led-state  on  ", &feedback) == 0 && feedback == "off");
    assert(d.handle_command("  get-led-state  ", &feedback) == 0 && feedback == "on");

    assert(d.handle_command("get-led-color", &feedback) == 0 && feedback == "blue");
    assert(d.handle_command("set-led-color red", &feedback) == 0 && feedback == "blue");
    assert(d.handle_command("get-led-color", &feedback) == 0 && feedback == "red");
    assert(d.handle_command("  set-led-color  green  ", &feedback) == 0 && feedback == "red");
    assert(d.handle_command("  get-led-color  ", &feedback) == 0 && feedback == "green");

    ss << rate;
    assert(d.handle_command("get-led-rate", &feedback) == 0 && feedback == ss.str());
    ss1 << "set-led-rate " << min_rate;
    assert(d.handle_command(ss1.str(), &feedback) == 0 && feedback == ss.str());
    ss.str(string());
    ss << min_rate;
    assert(d.handle_command("get-led-rate", &feedback) == 0 && feedback == ss.str());
    ss1.str(string());
    ss1 << "   set-led-rate   " << max_rate << "  ";
    assert(d.handle_command(ss1.str(), &feedback) == 0 && feedback == ss.str());
    ss.str(string());
    ss << max_rate;
    assert(d.handle_command("  get-led-rate  ", &feedback) == 0 && feedback == ss.str());

    feedback = "42";
    assert(d.handle_command("get-led-state1", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("set-led-state of", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("get-led-stat", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("set-led-state", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("get-led-state on", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("  get-led-state  ", &feedback) == 0 && feedback == "on");

    feedback = "42";
    assert(d.handle_command("get-led-colour", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("set-led-color black", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("get-led-colo", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("set-led-color", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("get-led-color red", &feedback) != 0 && feedback == "42");
    assert(d.handle_command("  get-led-color  ", &feedback) == 0 && feedback == "green");

    feedback = "42";
    assert(d.handle_command("set-led-rate", &feedback) != 0 && feedback == "42");
    ss.str(string());
    ss << "set-led-rate " << min_rate - 1;
    assert(d.handle_command(ss.str(), &feedback) != 0 && feedback == "42");
    ss.str(string());
    ss << "set-led-rate " << max_rate + 1;
    assert(d.handle_command(ss.str(), &feedback) != 0 && feedback == "42");
    ss << "get-led-rate " << max_rate;
    assert(d.handle_command(ss.str(), &feedback) != 0 && feedback == "42");

    return 0;
}
