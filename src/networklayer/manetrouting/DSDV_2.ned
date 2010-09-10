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

simple DSDV_2
{
    parameters:
      double hellomsgperiod_DSDV @unit("s")=default(5s);
      string netmask = default("255.255.0.0");
      int RNGseed_DSDV = default(1);
      double MaxVariance_DSDV=default(1);
      double timetolive_routing_entry @unit("s")=default(5s);
    gates:
      input ip_toDSDV;
      output DSDV_toip;
}
