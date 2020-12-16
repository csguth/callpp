//
//  Hook.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <functional>
#include <memory>
#include <string>

namespace com::csguth::callpp {

class Account;
class Call;
enum class SignalingEvent;
enum class RegisteringEvent;


struct Credentials
{
    std::string user;

};

class Hook
{
public:
    Hook();
    ~Hook();
    
    Hook& on(std::function<void(Account&, RegisteringEvent)> cb);
    Hook& on(std::function<void(Call&, SignalingEvent)> cb);
    Hook& on(std::function<void(Credentials&)> cb);

    int run();

private:
    class Private;
    std::unique_ptr<Private> self;

};


}

