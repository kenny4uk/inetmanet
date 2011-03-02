//
// Copyright (C) 2004-2006 Andras Varga
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <sstream>
#include "IPRouteRule.h"
#include "InterfaceEntry.h"

void IPRouteRule::setRoule(Rule rule)
{
    if (DROP!=rule || NONE!=rule)
        opp_error("Rule not supported yet");
    this->rule = rule;
}

IPRouteRule::IPRouteRule()
{
    interfacePtr = NULL;
    rule = NONE;
    port=-1;
    protocol=IP_PROT_NONE;

}

IPRouteRule::~IPRouteRule()
{
}

std::string IPRouteRule::info() const
{
    std::stringstream out;
    out << "addr:"; if (address.isUnspecified()) out << "*  "; else out << address << "  ";
    out << "mask:"; if (netmask.isUnspecified()) out << "*  "; else out << netmask << "  ";
    out << "port:" << port << " ";
    out << "if:"; if (!interfacePtr) out << "*  "; else out << interfacePtr->getName() << "  ";
    switch (rule)
    {
        case DROP:       out << " DROP"; break;
        case ACCEPT: out << " ACCEPT"; break;
        case NAT:          out << " NAT"; break;
        case NONE:         out << " NONE"; break;
        default:           out << " ???"; break;
    }
    return out.str();
}

std::string IPRouteRule::detailedInfo() const
{
    return std::string();
}

const char *IPRouteRule::getInterfaceName() const
{
    return interfacePtr ? interfacePtr->getName() : "";
}

