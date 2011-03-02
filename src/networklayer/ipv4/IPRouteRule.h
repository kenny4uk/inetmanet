//
// Copyright (C) 2008 Andras Varga
// Copyright (C) 2011 Alfonso Ariza
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

#ifndef __INET_IPROUTEROULE_H
#define __INET_IPROUTEROULE_H

#include <vector>
#include <map>
#include <omnetpp.h>
#include "INETDefs.h"
#include "IPProtocolId_m.h"
#include "IPAddress.h"

class InterfaceEntry;

/**
 * IPv4 route in IRoutingTable.
 *
 * @see IRoutingTable, IRoutingTable
 */
class NatElement : public cPolymorphic
{
public:
    IPAddress addr;
    int port;
private:
  // copying not supported: following are private and also left undefined
    NatElement(const NatElement& obj);
    NatElement& operator=(const NatElement& obj);
};

class INET_API IPRouteRule : public cPolymorphic
{
  public:
    /** Specifies where the route comes from */
    enum Rule
    {
        DROP,
        ACCEPT,
        NAT,
        NONE
    };
  protected:
    class Nat
    {
    private:
        std::map<int,NatElement*> natAddress;
    public:
        void addNatAddres(){}
        void delNatAddress(){}
        const NatElement* getNat() const;
        ~Nat() {
             while (!natAddress.empty())
             {
                delete natAddress.begin()->second;
                natAddress.erase(natAddress.begin());
             }
        };
    };
    IPAddress address;     ///< Destination
    IPAddress netmask;  ///< Route mask
    int port;  ///
    IPProtocolId protocol;
    Rule     rule;
    InterfaceEntry *interfacePtr; ///< interface
    std::map<int, Nat> natRule;

  private:
    // copying not supported: following are private and also left undefined
    IPRouteRule(const IPRouteRule& obj);
    IPRouteRule& operator=(const IPRouteRule& obj);

  public:
    IPRouteRule();
    ~IPRouteRule();
    virtual std::string info() const;
    virtual std::string detailedInfo() const;

    void setAddress(IPAddress host)  {this->address = host;}
    void setNetmask(IPAddress netmask)  {this->netmask = netmask;}
    void setPort(int port)  {this->port = port;}
    void setInterface(InterfaceEntry *interfacePtr)  {this->interfacePtr = interfacePtr;}
    void setRoule(Rule rule);
    void setProtocol(IPProtocolId protocol){this->protocol = protocol;}

    /** Ip addres prefix for set the rule  */
    IPAddress getAddress() const {return address;}

    /** Represents length of prefix to match */
    IPAddress getNetmask() const {return netmask;}

    /** Get port */
    const int getPort() const {return port;}

    const IPProtocolId getProtocol() const {return protocol;}

    /** Next hop interface */
    InterfaceEntry *getInterface() const {return interfacePtr;}

    /** Convenience method */
    const char *getInterfaceName() const;

    /** Route type: Direct or Remote */
    Rule getRule() const {return rule;}

};

#endif

