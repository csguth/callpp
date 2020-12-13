#pragma once

#include <functional>
#include <memory>

#include "Uid.hpp"

namespace com::csguth::callpp {

enum class SignalingEvent;
class PjCall;

class Call final
{
public:
    Call() = default;
    explicit Call(std::unique_ptr<PjCall> pjCall) noexcept;
    Call(Call&& other) noexcept;
    Call(const Call&) = delete;
    Call& operator=(Call other);
    friend void swap(Call& lhs, Call& rhs) noexcept;
    ~Call();
    
    bool operator<(const Call& other) const;
    
    std::function<void(SignalingEvent)> eventState;
    Uid uid;

    void accept();
    
private:
    std::unique_ptr<PjCall> pjCall;
    
    void setupEvents();

};

}
