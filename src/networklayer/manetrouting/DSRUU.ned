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

simple DSRUU
{
    parameters:
        bool PrintDebug= default(false);
        bool FlushLinkCache= default(true);
        bool PromiscOperation= default(false);
        bool UseNetworkLayerAck= default(true);
        int BroadCastJitter= default(20); // msecond
        int RouteCacheTimeout= default(300); // second units
        int SendBufferTimeout= default(30); // seconds
        int SendBufferSize= default(-1); // Max internal size
        int RequestTableSize= default(-1);  // Max internal size
        int RequestTableIds= default(-1);  // Max internal size
        int MaxRequestRexmt= default(16);
        int MaxRequestPeriod= default(10); // seconds
        int RequestPeriod= default(50); // mseconds
        int NonpropRequestTimeout= default(30); // mseconds
        int RexmtBufferSize= default(-1);   // Max internal size
        int MaintHoldoffTime= default(250); // mseconds
        int MaxMaintRexmt= default(2);
        bool TryPassiveAcks= default(true);
        int PassiveAckTimeout= default(100);// mseconds
        int GratReplyHoldOff= default(1); // seconds
        int MAX_SALVAGE_COUNT= default(15);
        int LifoSize= default(20);
        bool PathCache= default(true);
	bool RetryPacket = default(false);
        int RREQMaxVisit= default(1);   // The node can be visit multiples times in the RREQ phase Gerla article
        bool RREPDestinationOnly= default(false); // only the destination send the RREP
        bool ETX_Active= default(false);
        double ETXHelloInterval= default(1);
        double ETXWindowNumHello= default(10);
        int ETXRetryBeforeFail= default(-1); // if !=-1 use the etx how hello to detect break links, 
					     // if don't receive consecutive ETXRetryBeforeFail number detect link break   
	   volatile double broadCastDelay @unit("s") = default (uniform(0s,0.005s));
	   volatile double uniCastDelay @unit("s") = default (0s);

    gates:
        input fromIp;
        output toIp;
}

