#include <memory>
#include "dispatcher.hpp"
#include "led.hpp"
#include "server.hpp"

using namespace Dispatcher;
using namespace LED;

int main()
{
    shared_ptr<Led> led(new Led(State_t::OFF, Color_t::BLUE, Led::MIN_RATE));
    shared_ptr<LedDispatcher> dispatcher(new LedDispatcher(led));
    Server::Server server(dispatcher);
    server.handle_requests();

    return 0;
}
