#include <pjsua2/account.hpp>
#include <pjsua2/call.hpp>
#include <pjsua2/endpoint.hpp>
#include <array>
#include <deque>
#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <thread>
#include <chrono>

#include "../callpp/Endpoint.hpp"

using namespace com::csguth::callpp;


struct Uri {
    std::string uri;
    bool operator==(const Uri& other) const
    {
        return uri == other.uri;
    }
    friend std::ostream& operator<<(std::ostream& out, const Uri& uri)
    {
        return out << "URI " + uri.uri;
    }
};

struct Uid
{
    Uid()
    : value{std::to_string(counter++)}
    {}
    std::string value;
    
    friend std::ostream& operator<<(std::ostream& out, const Uid& uid)
    {
        return out << "UID " << uid.value;
    }
    
private:
    static int counter;
};


int Uid::counter = 0;

struct PjCall: pj::Call {
    using pj::Call::Call;
    
    std::function<void(pjsip_inv_state)> eventState;
    
    void onCallState(pj::OnCallStateParam &prm) override
    {
        auto state = getInfo().state;
        retained.push_back(state);
        while (eventState && !retained.empty())
        {
            auto front = retained.front();
            retained.pop_front();
            eventState(front);
        }
    }
    
    void onCallMediaState(pj::OnCallMediaStateParam &prm) override
    {
        try {
            auto media = getAudioMedia(-1);
            
            media.startTransmit(pj::Endpoint::instance().audDevManager().getPlaybackDevMedia());
            pj::Endpoint::instance().audDevManager().getCaptureDevMedia().startTransmit(media);
            
        } catch(...) {
            return;
        }


    }
    
private:
    
    std::deque<pjsip_inv_state> retained;
};

struct IncomingCall {
    Uri uri;
    std::unique_ptr<PjCall> pjCall;
};

enum class SignalingEvent
{
    Incoming,
    Connecting,
    Connected,
    Disconnected
};

enum class RegisteringEvent
{
    Registering,
    Registered,
    Unregistered
};

struct Call
{
    Call(
         Uid uid,
         std::unique_ptr<PjCall> pjCall
    )
    : uid{std::move(uid)}
    , pjCall{std::move(pjCall)}
    {
        if (this->pjCall)
        {
            this->pjCall->eventState = [this](pjsip_inv_state state)
            {
                if (eventState)
                {
                    switch (state)
                    {
                            
                        case PJSIP_INV_STATE_NULL:
                            break;
                        case PJSIP_INV_STATE_CALLING:
                            break;
                        case PJSIP_INV_STATE_INCOMING:
                            eventState(SignalingEvent::Incoming);
                            break;
                        case PJSIP_INV_STATE_EARLY:
                            break;
                        case PJSIP_INV_STATE_CONNECTING:
                            eventState(SignalingEvent::Connecting);
                            break;
                        case PJSIP_INV_STATE_CONFIRMED:
                            eventState(SignalingEvent::Connected);
                            break;
                        case PJSIP_INV_STATE_DISCONNECTED:
                            eventState(SignalingEvent::Disconnected);
                            break;
                    }
                    
                };
            };
        }
    }
    
    ~Call()
    {
        if (pjCall) pjCall->eventState = nullptr;
    }
    
    Uid uid;
    
    std::function<void(SignalingEvent)> eventState;

    void ring()
    {
        if (!pjCall) return;
        auto prm = pj::CallOpParam{};
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        prm.statusCode = PJSIP_SC_RINGING;
        pjCall->answer(prm);
    }
    void accept()
    {
        if (!pjCall) return;
        auto prm = pj::CallOpParam{};
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        prm.statusCode = PJSIP_SC_ACCEPTED;
        pjCall->answer(prm);
    }
    
private:
    std::unique_ptr<PjCall> pjCall;
};

struct Account: pj::Account
{
    std::function<void(IncomingCall)> eventIncomingCall;
    std::function<void(RegisteringEvent)> eventRegistering;
    
    void onIncomingCall(pj::OnIncomingCallParam &prm) override
    {
        auto call = IncomingCall{
            Uri{"sip:user@pjsip.com"},
            std::make_unique<PjCall>(*this, prm.callId)
        };
        
        if (eventIncomingCall) eventIncomingCall(std::move(call));
    }
    
    void onRegStarted(pj::OnRegStartedParam &prm) override
    {
        if (eventRegistering) eventRegistering(RegisteringEvent::Registering);
    }
    
    void onRegState(pj::OnRegStateParam &prm) override
    {
        if (eventRegistering) eventRegistering(prm.code == 200 && this->getInfo().regStatus ? RegisteringEvent::Registered : RegisteringEvent::Unregistered);
    }
    
    ~Account()
    {
        shutdown();
    }

};


struct Hook {
  
    Hook()
        : epCfg{[](){
            auto out = pj::EpConfig{};
            out.uaConfig.userAgent = "pjsua++-test";
            out.uaConfig.threadCnt = 0;
            out.uaConfig.mainThreadOnly = true;
            out.logConfig.level = 0;
            out.uaConfig.stunServer.emplace_back("stun.l.google.com:19302");
            return out;
        }()}
        , ep{epCfg}
        , account{}
    {
        account.eventIncomingCall = [this](IncomingCall incoming)
        {
            if (cb)
            {
                calls.emplace_back(std::make_unique<Call>(Uid{}, std::move(incoming.pjCall)));
                
                auto back = calls.back().get();
                
                if (back)
                {
                    back->eventState = [this, back](SignalingEvent event)
                    {
                        if (cb && back) cb(*back, event);
                    };
                }
                
            }
        };
        
        account.eventRegistering = [this](RegisteringEvent event)
        {
            retainedRegisteringEvents.push_back(event);
            propagateRetained();
        };
        
        auto cfg = pj::AccountConfig{};
        cfg.idUri = "sip:csguth@csguth.com:5061;transport=TLS";
        cfg.regConfig.registrarUri = "sip:csguth.com:5061;transport=TLS";
        cfg.sipConfig.authCreds.push_back( pj::AuthCredInfo("digest", "*", "test1", 0, "secret1") );
        cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_DISABLED;
        cfg.mediaConfig.srtpSecureSignaling = false;

        account.create(cfg);
    }
    
    ~Hook()
    {
        account.eventRegistering = nullptr;
        account.eventIncomingCall = nullptr;
    }
    
    Hook& on(std::function<void(RegisteringEvent)> cb)
    {
        this->registeringEvent = std::move(cb);
        propagateRetained();
        return *this;
    }
    
    Hook& on(std::function<void(Call&, SignalingEvent)> cb)
    {
        this->cb = std::move(cb);
        return *this;
    }
    
    int wait()
    {
        ep.start();
        while (ep.libHandleEvents(1000) >= 0);
        return 0;
    }
    
    pj::EpConfig epCfg;
    Endpoint ep;
    Account account;
    
    std::function<void(Call&, SignalingEvent)> cb;
    std::function<void(RegisteringEvent)> registeringEvent;
    std::deque<RegisteringEvent> retainedRegisteringEvents;

private:
    void propagateRetained()
    {
        while (registeringEvent && !retainedRegisteringEvents.empty())
        {
            auto event = retainedRegisteringEvents.front();
            retainedRegisteringEvents.pop_front();
            registeringEvent(event);
        }
    }
    
    std::vector<std::unique_ptr<Call>> calls;
    
};

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
                    call.ring();
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
    .wait();
}
