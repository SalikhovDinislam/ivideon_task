#include <exception>
#include <utility>
#include "dispatcher.hpp"

namespace Dispatcher {
using namespace LED;

using std::make_pair;
using std::pair;
using std::string;

enum class CallbackType_t {
    SETTER,
    GETTER,
};
typedef string (Led::*getter_t)(void) const;
typedef int (Led::*setter_t)(const string&);
typedef struct {
    union {
        getter_t getter;
        setter_t setter;
    };
    CallbackType_t type;
} generic_callback_t;
/* TODO: use sorted array instead? */
static map<string, generic_callback_t> callbacks;

LedDispatcher::LedDispatcher(Led *led)
: _led_ptr(led)
{
    init_callbacks();
}

LedDispatcher::LedDispatcher(std::shared_ptr<Led> &led)
: _led_ptr(led)
{
    init_callbacks();
}

void LedDispatcher::init_callbacks(void)
{
    static bool already_inited = false;
    static std::mutex lock;
    std::lock_guard<mutex> guard_lock(lock);
    generic_callback_t callback;

    if (already_inited) {
        return;
    }

    /* Add setters */
    callback.type = CallbackType_t::SETTER;
    callback.setter = &Led::set_state;
    callbacks["set-led-state"] = callback;
    callback.setter = &Led::set_color;
    callbacks["set-led-color"] = callback;
    callback.setter = &Led::set_rate;
    callbacks["set-led-rate"] = callback;

    /* Add getters */
    callback.type = CallbackType_t::GETTER;
    callback.getter = &Led::get_state_str;
    callbacks["get-led-state"] = callback;
    callback.getter = &Led::get_color_str;
    callbacks["get-led-color"] = callback;
    callback.getter = &Led::get_rate_str;
    callbacks["get-led-rate"] = callback;

    already_inited = true;
}

static pair<string, string> split_command(const string &command)
{
    const char delimeter = ' ';

    /* skip leading spaces */
    size_t cmd_name_start = command.find_first_not_of(delimeter);
    if (cmd_name_start == string::npos) {
        /* empty command */
        return make_pair("", "");
    }

    size_t cmd_name_end = command.find_first_of(delimeter, cmd_name_start);
    if (cmd_name_end == string::npos) {
        /* command without arguments */
        return make_pair(command.substr(cmd_name_start), "");
    }

    /* skip trailing spaces */
    size_t cmd_name_len = cmd_name_end - cmd_name_start;
    size_t cmd_args_start = command.find_first_not_of(delimeter, cmd_name_end);
    if (cmd_args_start == string::npos) {
        /* command without arguments */
        return make_pair(command.substr(cmd_name_start, cmd_name_len), "");
    }

    /* command with arguments */
    size_t cmd_args_end = command.find_last_not_of(delimeter) + 1;
    size_t cmd_args_len = cmd_args_end - cmd_args_start;
    return make_pair(command.substr(cmd_name_start, cmd_name_len),
                     command.substr(cmd_args_start, cmd_args_len));
}

int LedDispatcher::handle_command(const string& command, string *led_feedback) const
{
    pair<string, string> cmd_name_and_args = split_command(command);
    const string &command_name = cmd_name_and_args.first;
    const string &command_args = cmd_name_and_args.second;

    auto it = callbacks.find(command_name);
    if (it == callbacks.end()) {
        return -ENOENT;
    }

    auto &callback = it->second;
    std::lock_guard<mutex> guard_lock(_lock);

    switch (callback.type) {
    case CallbackType_t::SETTER:
        return (_led_ptr.get()->*(callback.setter))(command_args);
    case CallbackType_t::GETTER:
        if (!command_args.empty()) {
            return -E2BIG;
        }
        *led_feedback = (_led_ptr.get()->*(callback.getter))();
        return 0;
    }

    /* to suppress compiler's warning */
    /* TODO: use __builtin_unreachable()? */
    return -EPERM;
}
} /* namespace Dispatcher */
