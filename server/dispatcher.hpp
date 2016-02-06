#pragma once

#include <map>
#include <memory>
#include <mutex>
#include "led.hpp"

namespace Dispatcher {
using std::map;
using std::mutex;
using std::shared_ptr;
using std::string;
using LED::Led;

/* TODO: make it a template */
class LedDispatcher {
private:
    shared_ptr<Led> _led_ptr;
    mutable mutex _lock;

public:
    static void init_callbacks(void);

    LedDispatcher(Led *led);
    LedDispatcher(shared_ptr<Led> &led);

    int handle_command(const string& command, string *led_feedback) const;
};

} /* namespace Dispatcher */
