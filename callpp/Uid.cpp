//
//  Uid.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "Uid.hpp"

#include <ostream>

namespace com::csguth::callpp {

int Uid::counter = 0;

Uid::Uid(std::string value) noexcept
: value{std::move(value)}
{}

Uid::Uid(Uid&& other)
: Uid{}
{
    swap(*this, other);
}

Uid::Uid(const Uid& other)
: Uid{other.value}
{}

Uid Uid::create()
{
    return Uid{std::to_string(counter++)};
}

Uid& Uid::operator=(Uid other)
{
    swap(*this, other);
    return *this;
}

void swap(Uid& lhs, Uid& rhs) noexcept
{
    using std::swap;
    swap(lhs.value, rhs.value);
}

bool Uid::operator<(const Uid& other) const
{
    return value < other.value;
}

std::ostream& operator<<(std::ostream& out, const Uid& uid)
{
    return out << "UID " << uid.value;
}

}
