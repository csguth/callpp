#include "Call.hpp"

#include "PjCall.hpp"
#include "SignalingEvent.hpp"
#include <pjsip.h>

#include <pjsua2.hpp>

#include <deque>

namespace com::csguth::callpp {

    Call::Call(Call&& other) noexcept
        : Call()
    {
        swap(*this, other);
        setupEvents();
    }

    void swap(Call& lhs, Call& rhs) noexcept
    {
        using std::swap;
        swap(lhs.pjCall, rhs.pjCall);
        swap(lhs.uid, rhs.uid);
        swap(lhs.eventState, rhs.eventState);
    }

    Call::Call(std::unique_ptr<PjCall> pjCall) noexcept
        : pjCall{std::move(pjCall)}
        , uid{Uid::create()}
    {
        setupEvents();
    }
    
    Call::~Call()
    {
        if (pjCall) pjCall->eventState = nullptr;
    }
    
    void Call::accept()
    {
        if (!pjCall) return;
        auto prm = pj::CallOpParam{};
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        prm.statusCode = PJSIP_SC_ACCEPTED;
        pjCall->answer(prm);
    }

    bool Call::operator<(const Call& other) const
    {
        return pjCall.get() < other.pjCall.get();
    }

    void Call::setupEvents()
    {
        pjCall->eventState = [this](pjsip_inv_state state)
        {
            if (!eventState) return;
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
                    this->pjCall = nullptr;
                    eventState(SignalingEvent::Disconnected);
                    break;
            }
        };
    }

void Call::make(std::string to)
{
    auto params = pj::CallOpParam{};
    params.opt.audioCount = 1;
    params.opt.videoCount = 0;
    pjCall->makeCall("<sips:" + std::move(to) + "@csguth.com:5061;transport=TLS>", params);
}



}
