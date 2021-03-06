#include "../callpp/Call.hpp"
#include "../callpp/Hook.hpp"
#include "../callpp/Uri.hpp"
#include "../callpp/RegisteringEvent.hpp"
#include "../callpp/SignalingEvent.hpp"

#include <iostream>

using namespace com::csguth::callpp;

int main(int argc, char *argv[])
{
    return Hook{}
    .on(
        [](RegisteringEvent event)
        {
            switch (event) {
                case RegisteringEvent::Registering:
                    std::cout << "RegisteringEvent::Registering\n";
                    break;
                case RegisteringEvent::Registered:
                    std::cout << "RegisteringEvent::Registered\n";
                    break;
                case RegisteringEvent::Unregistered:
                    std::cout << "RegisteringEvent::Unregistered\n";
                    break;
            }
        }
    )
    .on(
        [](Call& call, SignalingEvent event)
        {
            switch (event)
            {
                case SignalingEvent::Incoming:
                    std::cout << "Call " << call.uid << " SignalingEvent::Incoming\n";
                    call.accept();
                    break;
                case SignalingEvent::Connecting:
                    std::cout << "Call " << call.uid << " SignalingEvent::Connecting\n";
                    break;
                case SignalingEvent::Connected:
                    std::cout << "Call " << call.uid << " SignalingEvent::Connected\n";
                    break;
                case SignalingEvent::Disconnected:
                    std::cout << "Call " << call.uid << " SignalingEvent::Disconnected\n";
                    break;
                default:
                    break;
            }
        }
    )
    .run();
}
