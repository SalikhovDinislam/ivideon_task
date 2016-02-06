#pragma once

#include <pthread.h>
#include <string>
#include <vector>
#include "dispatcher.hpp"

namespace Server {
using namespace Dispatcher;

using std::shared_ptr;
using std::string;
using std::vector;

class Server {
private:
    shared_ptr<LedDispatcher> _dispatcher;
    /* TODO: Get rid of explicit pthread usage */
    vector<pthread_t> _threads;
    pthread_rwlock_t _state_lock;
    pthread_t self_thread_id;
    bool _is_running;

    int add_new_client_handler(int client_id);
    void reap_finished_threads(void);
    
public:
    static const string main_channel;
    static const string led_input_channel_prefix;
    static const string led_output_channel_prefix;

    Server(LedDispatcher *dispatcher);
    Server(shared_ptr<LedDispatcher> &dispatcher);
    ~Server();

    int handle_requests(void);
};
} /* namespace Server */
