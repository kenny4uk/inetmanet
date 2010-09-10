//
//   Copyright (C) 2009 by Alfonso Ariza
//   Malaga University 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.networklayer.manetrouting;

import inet.networklayer.manetrouting.BaseRouting;

simple DYMOUM extends BaseRouting
{
    parameters:
        @class(DYMOUM);
        bool no_path_acc_= default(false);
        bool reissue_rreq_= default(false);
        bool s_bit_= default(false);
        int hello_ival_= default(0); // 0 No hello, link layer feedback
        int MaxPktSec= default(20);
        int NetDiameter= default(10);
        int RouteTimeOut= default(3000); // msecond
        int RouteDeleteTimeOut= default(3000*5); //5*RouteTimeOut
        int RREQWaitTime= default(1000); // msecond
        int RREQTries= default(3); // Rreq reties before fail
// Experimental
		bool promiscuous= default(false); // Experimental
        int noRouteBehaviour= default(1); // experimental
        bool intermediateRREP= default(false); // experimental
        bool RREQattachPacket= default(false); // experimental
        volatile double broadCastDelay @unit("s") = default (uniform(0s,0.005s));
        volatile double uniCastDelay @unit("s") = default (0s);
        bool EqualDelay =default(true);
        bool UseIndex=default(false);// use the interface index instead the ip to identify the interface
    gates:
        input from_ip;
        output to_ip;
}



 