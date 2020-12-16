//
//  Hook.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "Hook.hpp"

#include "Account.hpp"
#include "PjAccount.hpp"
#include "Endpoint.hpp"
#include "SignalingEvent.hpp"

#include <deque>
#include <set>

namespace {

    auto makeCfg()
    {
        auto out = pj::EpConfig{};
        out.uaConfig.userAgent = "pjsua++-test";
        out.uaConfig.threadCnt = 0;
        out.uaConfig.mainThreadOnly = true;
        out.logConfig.level = 0;
        out.uaConfig.stunServer.emplace_back("stun.l.google.com:19302");
        return out;
    }

    auto makeAccount(com::csguth::callpp::Credentials cred)
    {
        if (cred.user.empty()) cred.user = "csguth";
        auto out = std::make_unique<com::csguth::callpp::PjAccount>();
        
        auto cfg = pj::AccountConfig{};
        cfg.idUri = "sips:" + cred.user + "@csguth.com:5061;transport=TLS";
        cfg.regConfig.registrarUri = "sips:csguth.com:5061;transport=TLS";
        cfg.sipConfig.authCreds.push_back( pj::AuthCredInfo("digest", "*", "test1", 0, "secret1") );
        cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_MANDATORY;
        cfg.mediaConfig.srtpSecureSignaling = 2;

        out->create(cfg);
        
        return out;
    }

}

namespace com::csguth::callpp {

class Hook::Private {
public:
    std::function<void(Call&, SignalingEvent)> cb;
    std::function<void(Account&, RegisteringEvent)> registeringEvent;
    std::function<void(Credentials&)> credentials;
    
    Private(Hook& parent)
    : parent(parent)
    {}
    
    int run()
    {
        Endpoint ep{makeCfg()};

        auto cred = Credentials{};
        if (credentials) credentials(cred);
       
        Account account{makeAccount(std::move(cred))};
        
        account.eventIncomingCall = [this](Call& call)
        {
            if (!cb) return;
            call.eventState = [this, &call](SignalingEvent event)
            {
                if (!cb) return;
                cb(call, event);
            };
            
            cb(call, SignalingEvent::Incoming);
        };
        
        account.eventOutgoingCall = [this](Call& call)
        {
            if (!cb) return;
            call.eventState = [this, &call](SignalingEvent event)
            {
                if (!cb) return;
                cb(call, event);
            };
            
            cb(call, SignalingEvent::Outgoing);
        };

        account.eventRegistering = [this, &account](RegisteringEvent event)
        {
            if (registeringEvent) registeringEvent(account, event);
        };
        
        ep.start();
        while (ep.libHandleEvents(1000) >= 0);
        return 0;
    }

private:
    Hook& parent;
    
    
};

Hook::Hook()
: self{std::make_unique<Private>(*this)}
{}

Hook::~Hook() = default;

Hook& Hook::on(std::function<void(Account&, RegisteringEvent)> cb)
{
    self->registeringEvent = std::move(cb);
    return *this;
}
Hook& Hook::on(std::function<void(Call&, SignalingEvent)> cb)
{
    self->cb = std::move(cb);
    return *this;
}

Hook& Hook::on(std::function<void(Credentials&)> cb)
{
    self->credentials = std::move(cb);
    return *this;
}

int Hook::run()
{
    return self->run();
}

}
