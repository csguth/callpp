//
//  Uri.hpp
//  hello
//
//  Created by Chrystian Guth on 13/12/2020.
//

#pragma once

#include <iosfwd>
#include <string>

namespace com::csguth::callpp {

struct Uri
{
    std::string uri;
    bool operator==(const Uri& other) const;
    bool operator!=(const Uri& other) const;
    friend std::ostream& operator<<(std::ostream& out, const Uri& uri);
};


}
