//
//  Account.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include "Call.hpp"
#include "Uid.hpp"

#include <map>

namespace com::csguth::callpp {

enum class RegisteringEvent;

class PjAccount;
class Account final
{
public:
    Account() = default;
    Account(Account&&);
    Account& operator=(Account&&);
    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;
    friend void swap(Account& lhs, Account& rhs) noexcept;
    explicit Account(std::unique_ptr<PjAccount> self);
    ~Account();
    
    std::function<void(Call&)> eventIncomingCall;
    std::function<void(Call&)> eventOutgoingCall;
    std::function<void(RegisteringEvent)> eventRegistering;
    
    void sendRegister();
    void makeCall(std::string to);
    
private:
    std::unique_ptr<PjAccount> self;
    std::map<Uid, Call> calls;
    
};

}
