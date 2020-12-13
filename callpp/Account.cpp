//
//  Account.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "Account.hpp"
#include "Call.hpp"
#include "PjAccount.hpp"
#include "PjCall.hpp"

namespace com::csguth::callpp {

Account::Account(Account&& other)
    : Account()
{
    swap(*this, other);
}
Account& Account::operator=(Account&& other)
{
    swap(*this, other);
    return *this;
}

void swap(Account& lhs, Account& rhs) noexcept
{
    using std::swap;
    swap(lhs.eventIncomingCall, rhs.eventIncomingCall);
    swap(lhs.eventRegistering, rhs.eventRegistering);
    swap(lhs.self, rhs.self);
    swap(lhs.calls, rhs.calls);
}

Account::Account(std::unique_ptr<PjAccount> self)
    : self{std::move(self)}
{
    this->self->eventIncomingCall = [this](std::unique_ptr<PjCall> pjCall)
    {
        if (!eventIncomingCall) return;
        
        auto call = Call{std::move(pjCall)};
        auto result = calls.insert(std::make_pair(call.uid, std::move(call)));
        eventIncomingCall(result.first->second);
    };
    
    this->self->eventRegistering = [this](RegisteringEvent event)
    {
        if (!eventRegistering) return;
        
        eventRegistering(event);
    };
    
}
Account::~Account() = default;

void Account::sendRegister()
{
    if (!self) return;
    self->setRegistration(true);
}

}
