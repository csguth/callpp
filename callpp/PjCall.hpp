//
//  PjCall.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <deque>

#include <pjsua2/siptypes.hpp>
#include <pjsua2.hpp>

enum pjsip_inv_state;

namespace com::csguth::callpp {

class PjCall: public ::pj::Call {
public:
    static std::unique_ptr<PjCall> make(::pj::Account& acc, int call_id = PJSUA_INVALID_ID);
    
    std::function<void(pjsip_inv_state)> eventState;
    
private:
    PjCall() = delete;
    using ::pj::Call::Call;
    void onCallState(pj::OnCallStateParam &prm) override;
    void onCallMediaState(pj::OnCallMediaStateParam &prm) override;

};

}
