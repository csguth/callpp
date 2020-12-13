//
//  PjCall.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "PjCall.hpp"

namespace com::csguth::callpp {

std::unique_ptr<PjCall> PjCall::make(::pj::Account& acc, int call_id)
{
    return std::unique_ptr<PjCall>{new PjCall{acc, call_id}};
}

void PjCall::onCallState(pj::OnCallStateParam &prm)
{
    if (!eventState) return;
    
    auto state = getInfo().state;
    eventState(state);
}

void PjCall::onCallMediaState(pj::OnCallMediaStateParam &prm)
{
    try {
        auto media = getAudioMedia(-1);
        
        media.startTransmit(pj::Endpoint::instance().audDevManager().getPlaybackDevMedia());
        pj::Endpoint::instance().audDevManager().getCaptureDevMedia().startTransmit(media);
        
    } catch(...) {
        return;
    }

}

}
