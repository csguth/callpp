//
//  Hook.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <functional>
#include <memory>

namespace com::csguth::callpp {

class Call;
enum class SignalingEvent;
enum class RegisteringEvent;

class Hook
{
public:
    Hook();
    ~Hook();
    
    Hook& on(std::function<void(RegisteringEvent)> cb);
    Hook& on(std::function<void(Call&, SignalingEvent)> cb);
    
    int run();

private:
    class Private;
    std::unique_ptr<Private> self;

};


}

