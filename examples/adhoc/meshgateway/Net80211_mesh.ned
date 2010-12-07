//
// Copyright (C) 2010 Alfonso Ariza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package inet.examples.adhoc.meshgateway;


import inet.networklayer.autorouting.HostAutoConfigurator;
import inet.nodes.adhoc.MobileHost;
import inet.experimental.nodes.adhoc.MobileManetRoutingMesh;
import inet.experimental.nodes.adhoc.BasicMobileManetMesh;
import inet.experimental.nodes.adhoc.gateWayMesh;
import inet.nodes.ethernet.EtherSwitch;

import inet.world.ChannelControl;


network Net80211_mesh
{
    parameters:
        int numHosts;
        int numFixHosts;
        int numBasic;
        double playgroundSizeX;
        double playgroundSizeY;
    submodules:
        fixhost: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=987,908;is=s");
        }
        gateWay: gateWayMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=520,901");
        }
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=46,33;i=misc/sun");
        }
        gateWay1: gateWayMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=46,502");
        }
        fixhost1: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=138,20");
        }
        SW: EtherSwitch {
            @display("p=351,344");

        }
        fixhost2: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=753,908;is=s");
        }
        fixhost3: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=73,232");
        }
        gateWay2: gateWayMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=606,291");
        }
        fixhost4: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=824,232");
        }
        fixhost5: MobileManetRoutingMesh {
            parameters:
                @display("i=device/pocketpc_s;r=,,#707070;p=936,29");
        }
    connections allowunconnected:

        gateWay.ethg++ <--> SW.ethg++;
        gateWay1.ethg++ <--> SW.ethg++;
        gateWay2.ethg++ <--> SW.ethg++;
}


