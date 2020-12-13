//
//  Uid.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <string>

namespace com::csguth::callpp {

struct Uid
{
    Uid() = default;
    Uid(std::string value) noexcept;
    Uid(Uid&&);
    Uid(const Uid&);
    Uid& operator=(Uid);
    friend void swap(Uid& lhs, Uid& rhs) noexcept;
    
    static Uid create();
    
    friend std::ostream& operator<<(std::ostream& out, const Uid& uid);
    bool operator<(const Uid& other) const;
    
private:
    static int counter;
    std::string value;

};

}
