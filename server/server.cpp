#include <cerrno>
#include <csignal>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <sys/stat.h>
#include "dispatcher.hpp"
#include "server.hpp"

namespace Server {
using std::ifstream;
using std::ofstream;
using std::string;
using std::stringstream;

const string Server::main_channel = "led_server";
const string Server::led_input_channel_prefix = "to_led_";
const string Server::led_output_channel_prefix = "from_led_";

struct thread_state {
    int client_id;
    bool *state;
    pthread_rwlock_t *lock;
    shared_ptr<LedDispatcher> *dispatcher;
};

static bool get_state_value(thread_state *state)
{
    bool result;

    pthread_rwlock_rdlock(state->lock);
    result = state->state;
    pthread_rwlock_unlock(state->lock);

    return result;
}

static __thread bool epipe_handled = false;

static void handle_epipe(int signum)
{
    (void)signum;
    epipe_handled = true;
}

static int set_signal_handlers(void)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_epipe;

    return sigaction(SIGPIPE, &act, NULL);
}

static void *client_handler(void *arg)
{
    thread_state *state = (thread_state *)arg;

    if (set_signal_handlers()) {
        return arg;
    }

    try {
        stringstream ss;
        ss << Server::led_input_channel_prefix << state->client_id;
        ifstream input(ss.str());
        
        ss.str(string());
        ss << Server::led_output_channel_prefix << state->client_id;
        ofstream output(ss.str());
        
        auto dispatcher = state->dispatcher;
        while (!epipe_handled && get_state_value(state)) {
            string client_command;
            string led_feedback;

            /* TODO: eliminate busy-loop */
            getline(input, client_command);
            if (!input.fail()) {
                if (dispatcher->get()->handle_command(client_command, &led_feedback)) {
                    output << "FAILED" << std::endl;
                } else {
                    output << "OK" << (led_feedback.empty() ? "" : " ")
                           << led_feedback << std::endl;
                }
            } else {
                input.clear();
            }
        }
    } catch (...) {
        /* TODO: What to do? Print something? */
    }

    return arg;
}

void Server::reap_finished_threads(void)
{
    for (pthread_t& thread: _threads) {
        if (!pthread_equal(self_thread_id, thread)
         && !pthread_kill(thread, 0)) {
            void *state;
            pthread_join(thread, &state);
            free(state);
            thread = self_thread_id;
        }
    }
}

int Server::add_new_client_handler(int client_id)
{
    _threads.reserve(_threads.size() + 1);
    reap_finished_threads();

    thread_state *state = (thread_state *)malloc(sizeof(*state));
    if (!state) {
        return -errno;
    }
    state->client_id = client_id;
    state->state = &_is_running;
    state->lock = &_state_lock;
    state->dispatcher = &_dispatcher;

    pthread_t thread;
    int ret = pthread_create(&thread, NULL, client_handler, (void *)state);
    if (ret) {
        return -ret;
    }

    for (size_t i = 0; i < _threads.size(); ++i) {
        if (pthread_equal(_threads[i], self_thread_id)) {
            _threads[i] = thread;
            break;
        }
    }

    return 0;
}

int Server::Server::handle_requests(void)
{
    const char *input_channel = main_channel.c_str();
    if (mkfifo(input_channel, 0666) && errno != EEXIST) {
        return -errno;
    }
    
    ifstream input(input_channel);

    _is_running = true;
    /* TODO: eliminate busy-loop */
    while (true) {
        int client_id = 0;
        input >> client_id;
        if (!input.fail()) {
            if (add_new_client_handler(client_id)) {
                break;
            }
        } else {
            input.clear();
        }
    }

    return 0;
}

Server::Server(LedDispatcher *dispatcher)
: _dispatcher(dispatcher),
  _is_running(false)
{
    int error_value = pthread_rwlock_init(&_state_lock, NULL);
    if (error_value != 0) {
        std::error_code error_code(error_value, std::generic_category());
        throw std::system_error(error_code, "pthread_rwlock_init() failed");
    }
    self_thread_id = pthread_self();
}

Server::Server(shared_ptr<LedDispatcher> &dispatcher)
: _dispatcher(dispatcher),
  _is_running(false)
{
    int error_value = pthread_rwlock_init(&_state_lock, NULL);
    if (error_value != 0) {
        std::error_code error_code(error_value, std::generic_category());
        throw std::system_error(error_code, "pthread_rwlock_init() failed");
    }
    self_thread_id = pthread_self();
}

Server::~Server()
{
    pthread_rwlock_wrlock(&_state_lock);
    _is_running = false;
    pthread_rwlock_unlock(&_state_lock);

    reap_finished_threads();

    pthread_rwlock_destroy(&_state_lock);
}
} /* namespace Server */
