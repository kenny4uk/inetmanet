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

simple OLSR_ETX extends BaseRouting
{
    parameters:
        @class(OLSR_ETX);
        int Willingness = default(3); //ms
        int Hello_ival = default(2); // sec
        int Tc_ival = default(5); // sec
        int Mid_ival = default(5); // sec
        int use_mac = default(0);
        int Mpr_algorithm = default(1);
        int routing_algorithm = default(1);
        int Link_quality = default(2);
        bool Fish_eye = default(false);
        int Tc_redundancy @enum(0,1,2,3)= default(3);
        // Reported by Mohamed Belhassen
        // 0 OLSR_ETX_TC_MPR_SEL_SET_REDUNDANCY
        // 1 OLSR_ETX_TC_MPR_PLUS_MPR_SEL_SET_REDUNDANCY
        // 2 OLSR_ETX_TC_ALL_NEIGHBOR_SET_REDUNDANCY
        // 3 disable
        bool Link_delay = default(true);
        double C_alpha = default(0.2);
        bool UseIndex = default(false); // use the interface index instead the ip to identify the interface EXPERIMENTAL
        bool reduceFuncionality = default (false);
    gates:
        input from_ip;
        output to_ip;
}

