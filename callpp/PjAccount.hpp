//
//  PjAccount.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <functional>

#include <pjsua2/account.hpp>

namespace com::csguth::callpp {

class PjCall;
enum class RegisteringEvent;

class PjAccount final: public ::pj::Account
{
public:
    std::function<void(std::unique_ptr<PjCall>)> eventIncomingCall;
    std::function<void(RegisteringEvent)> eventRegistering;
    
    ~PjAccount();

private:
    void onIncomingCall(pj::OnIncomingCallParam &prm) override;
    void onRegStarted(::pj::OnRegStartedParam &prm) override;
    void onRegState(::pj::OnRegStateParam &prm) override;

};

}
