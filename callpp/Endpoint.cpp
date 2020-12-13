
#include "Endpoint.hpp"

#include <pjsua2/endpoint.hpp>

namespace com::csguth::callpp {

class Endpoint::Private: public pj::Endpoint
{
public:
    Private(pj::EpConfig cfg)
    {
        libCreate();
        
        auto tcfg = pj::TransportConfig();
        // Optional, set CA/certificate/private key files.
        // tcfg.tlsConfig.certFile = "cert.crt";
        // tcfg.tlsConfig.privKeyFile = "priv.key";
        // Optional, set ciphers. You can select a certain cipher/rearrange the order of ciphers here.
        // tcfg.ciphers = ep->utilSslGetAvailableCiphers();
        tcfg.tlsConfig.method = PJSIP_TLSV1_3_METHOD;
        tcfg.tlsConfig.verifyServer = true;
        pj::TransportId tid = transportCreate(PJSIP_TRANSPORT_TLS, tcfg);
        
        
        libInit(cfg);
    }
    
    void start()
    {
        libStart();
    }
    
    void onTransportState(const pj::OnTransportStateParam &prm) override
    {
        
    }
    
    ~Private()
    {
        libDestroy();
    }
};

Endpoint::Endpoint(pj::EpConfig cfg)
    : self{std::make_unique<Private>(std::move(cfg))}
{}
Endpoint::~Endpoint() = default;
void Endpoint::start()
{
    self->start();
}
int Endpoint::libHandleEvents(unsigned msec_timeout)
{
    return self->libHandleEvents(msec_timeout);
}

}
