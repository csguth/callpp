//
//  Uri.cpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#include "Uri.hpp"

#include <ostream>

namespace com::csguth::callpp {

bool Uri::operator==(const Uri& other) const
{
    return uri == other.uri;
}

bool Uri::operator!=(const Uri& other) const
{
    return !operator==(other);
}

std::ostream& operator<<(std::ostream& out, const Uri& uri)
{
    return out << "URI " + uri.uri;
}

}
