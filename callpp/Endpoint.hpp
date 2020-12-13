
#pragma once

#include <memory>

#include <pjsua2.hpp>
#include <pjsua2/endpoint.hpp>

namespace com::csguth::callpp {

class Endpoint final {
public:
    Endpoint(pj::EpConfig cfg);
    ~Endpoint();
    void start();
    int libHandleEvents(unsigned msec_timeout);
    
private:
    class Private;
    std::unique_ptr<Private> self;
    
};

}
