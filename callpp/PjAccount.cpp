//
//  PjAccount.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "PjAccount.hpp"

#include "PjCall.hpp"
#include "RegisteringEvent.hpp"

namespace com::csguth::callpp {

void PjAccount::onIncomingCall(pj::OnIncomingCallParam &prm)
{
    if (eventIncomingCall) eventIncomingCall(PjCall::make(*this, prm.callId));
}

void PjAccount::onRegStarted(::pj::OnRegStartedParam &prm)
{
    if (eventRegistering) eventRegistering(RegisteringEvent::Registering);
}

void PjAccount::onRegState(::pj::OnRegStateParam &prm)
{
    if (eventRegistering) eventRegistering(prm.code == 200 && this->getInfo().regStatus ? RegisteringEvent::Registered : RegisteringEvent::Unregistered);
}

PjAccount::~PjAccount()
{
    shutdown();
}

}
