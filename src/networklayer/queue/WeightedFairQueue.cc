//
// Copyright (C) 2001-2006  Sergio Andreozzi
// Copyright (C) 2009 A. Ariza Universidad de Málaga
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

/***************************************************************************/

#include "WeightedFairQueue.h"
Define_Module(WeightedFairQueue);

void WeightedFairQueue::initialize()
{
    PassiveQueueBase::initialize();
    // configuration
    frameCapacity = par("frameCapacity");
    const char *classifierClass = par("classifierClass");
    bandwidth  = par("Bandwidth");

    classifier = check_and_cast<IQoSClassifier*>(createOne(classifierClass));

    const char *vstr = par("queueWeight").stringValue();
    std::vector<double> queueWeight = cStringTokenizer(vstr).asDoubleVector();
    numQueues = classifier->getNumQueues();
    if (numQueues<(int)queueWeight.size())
        numQueues = queueWeight.size();
    for (int i=0; i<numQueues; i++)
    {
        SubQueueData queueData;
        char buf[32];
        sprintf(buf, "WFqueue-%d", i);
        cQueue queue(buf);
        subqueueData.push_back(queueData);
        queueArray.push_back(queue);
        subqueueData[i].queueWeight = 1;
    }

    for (unsigned int i=0; i<queueWeight.size(); i++)
    {
        subqueueData[i].queueWeight = queueWeight[i];
    }
}


bool WeightedFairQueue::enqueue(cMessage *msg)
{
    int queueIndex = classifier->classifyPacket(msg);

    if (frameCapacity && queueArray[queueIndex].length() >= frameCapacity)
    {
        EV << "Queue " << queueIndex << " full, dropping packet.\n";
        delete msg;
        return true;
    }
    else
    {
        queueArray[queueIndex].insert(msg);
        if (GPS_idle)
        {
            last_vt_update=SIMTIME_DBL(simTime());
            virt_time=0;
            GPS_idle=false;
        }
        else
        {
            virt_time=virt_time+(SIMTIME_DBL(simTime())-last_vt_update)/sum;
            last_vt_update=SIMTIME_DBL(simTime());
        }
        double pkleng =(double)PK(msg)->getBitLength();
        subqueueData[queueIndex].finish_t = (subqueueData[queueIndex].finish_t > virt_time ?
                                             subqueueData[queueIndex].finish_t:virt_time)+
                                            pkleng /(subqueueData[queueIndex].queueWeight*bandwidth);
        subqueueData[queueIndex].time.push(subqueueData[queueIndex].finish_t);
        if ((subqueueData[queueIndex].B++)==0)
            sum=sum+(double)subqueueData[queueIndex].queueWeight;
        if ( fabs(sum) < safe_limit )
            sum=0;
        return false;
    }
}

cMessage *WeightedFairQueue::dequeue()
{
    int selectQueue=-1;
    double endTime =1e20;
    for (int i=0; i<numQueues; i++)
    {
        if (!queueArray[i].empty())
        {
            double time = subqueueData[i].time.front();
            if (time<endTime)
            {
                selectQueue=i;
                endTime=time;
            }
        }
    }
    if (selectQueue!=-1)
    {
        subqueueData[selectQueue].time.pop();
        virt_time=virt_time+(SIMTIME_DBL(simTime())-last_vt_update)/sum;
        last_vt_update=SIMTIME_DBL(simTime());
        //update sum
        if ((--subqueueData[selectQueue].B)==0)
            sum=sum-(double)subqueueData[selectQueue].queueWeight;
        if ( fabs(sum) < safe_limit )
            sum=0;
        if (sum==0)
        {
            GPS_idle=true;
            for (int i=0; i <numQueues; i++) subqueueData[i].finish_t=0;
        }
        return (cMessage *)queueArray[selectQueue].pop();
    }
    return NULL;
}

void WeightedFairQueue::sendOut(cMessage *msg)
{
    send(msg, outGate);
}


