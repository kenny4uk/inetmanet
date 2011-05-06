/*
 * Copyright (c) 2008,2009 IITP RAS
 * Copyright (c) 2011 Universidad de Málaga
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Kirill Andreev <andreev@iitp.ru>
 * Author: Alfonso Ariza <aarizaq@uma.es>
 */

#include "hwmp.h"
#include "Ieee80211MgmtFrames_m.h"
#include "Ieee802Ctrl_m.h"
#define delay uniform(0.0,0.01)


void ProactivePreqTimer::expire()
{
   HwmpProtocol*  hwmpProtocol = dynamic_cast<HwmpProtocol*> (agent_);
   if (hwmpProtocol==NULL)
      opp_error("agent not valid");

   hwmpProtocol->SendProactivePreq();
};

void PreqTimeout::expire()
{
   HwmpProtocol*  hwmpProtocol = dynamic_cast<HwmpProtocol*> (agent_);
   if (hwmpProtocol==NULL)
      opp_error("agent not valid");
   hwmpProtocol->RetryPathDiscovery(dest);
};


PreqTimeout::PreqTimeout(MACAddress d, ManetRoutingBase* agent) : ManetTimer(agent)
{
    dest = d;
}

PreqTimeout::PreqTimeout(MACAddress d) : ManetTimer()
{
    dest = d;
}



void PreqTimer::expire()
{
   HwmpProtocol*  hwmpProtocol = dynamic_cast<HwmpProtocol*> (agent_);
   if (hwmpProtocol==NULL)
      opp_error("agent not valid");
   hwmpProtocol->SendMyPreq();
};



void PerrTimer::expire()
{
   HwmpProtocol*  hwmpProtocol = dynamic_cast<HwmpProtocol*> (agent_);
   if (hwmpProtocol==NULL)
      opp_error("agent not valid");
   hwmpProtocol->SendMyPerr();
};



HwmpProtocol::HwmpProtocol ()
{
    m_proactivePreqTimer = new ProactivePreqTimer(this);
    m_perrTimer = new PerrTimer(this);
}

HwmpProtocol::~HwmpProtocol ()
{
    delete m_proactivePreqTimer;
    delete m_perrTimer;
}


void
HwmpProtocol::initialize(int stage)
{
    if (stage==4)
    {
        createTimerQueue();
        registerRoutingModule();
        m_maxQueueSize=par("maxQueueSize");
        m_dot11MeshHWMPmaxPREQretries=par("dot11MeshHWMPmaxPREQretries");
        m_dot11MeshHWMPnetDiameterTraversalTime=par("dot11MeshHWMPnetDiameterTraversalTime");
        m_dot11MeshHWMPpreqMinInterval=par("dot11MeshHWMPpreqMinInterval");
        m_dot11MeshHWMPperrMinInterval=par("dot11MeshHWMPperrMinInterval");
        m_dot11MeshHWMPactiveRootTimeout=par("dot11MeshHWMPactiveRootTimeout");
        m_dot11MeshHWMPactivePathTimeout=par("dot11MeshHWMPactivePathTimeout");
        m_dot11MeshHWMPpathToRootInterval=par("dot11MeshHWMPpathToRootInterval");
        m_dot11MeshHWMPrannInterval=par("dot11MeshHWMPrannInterval");
        m_isRoot=par("isRoot");
        m_maxTtl=par("maxTtl");
        m_unicastPerrThreshold=par("unicastPerrThreshold");
        m_unicastPreqThreshold=par("unicastPreqThreshold");
        m_unicastDataThreshold=par("unicastDataThreshold");
        m_ToFlag=par("ToFlag");
        m_concurrentReactive = par("concurrentReactive");
        m_preqTimer = new PreqTimer(this);
        m_perrTimer = new PerrTimer(this);
        m_proactivePreqTimer = new ProactivePreqTimer(this);
        if (isRoot())
           setRoot();
        scheduleEvent();
    }
}


void HwmpProtocol::handelMessage(cMessage *msg)
{
   if (!checkTimer(msg))
       proccesData (msg);
   scheduleEvent();
}

void HwmpProtocol::proccesData (cMessage *msg)
{
  Ieee80211ActionHWMPFrame * pkt = dynamic_cast<Ieee80211ActionHWMPFrame*>(msg);
  if (pkt==NULL)
  {
      Ieee80211MeshFrame * pkt = dynamic_cast<Ieee80211MeshFrame*>(msg);
      if (pkt==NULL)
      {
         delete msg;
         return;
      }
      // enqueue and request route
      QueuedPacket qpkt;
      qpkt.pkt= pkt;
      qpkt.dst=pkt->getAddress4();
      qpkt.src=pkt->getAddress3();
      if (pkt->getControlInfo())
      {
          Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(pkt->removeControlInfo());
          qpkt.inInterface = ctrl->getInputPort();
          delete ctrl;
      }
      HwmpRtable::LookupResult result = m_rtable->LookupReactive (qpkt.dst);
      HwmpRtable::LookupResult resultProact = m_rtable->LookupProactive ();
      if (result.retransmitter == MACAddress::BROADCAST_ADDRESS &&  resultProact.retransmitter==MACAddress::BROADCAST_ADDRESS)
      {
          if (ShouldSendPreq (qpkt.dst))
          {
              GetNextHwmpSeqno ();
              uint32_t dst_seqno = 0;
              m_stats.initiatedPreq ++;
              requestDestination (qpkt.dst, dst_seqno);
          }
      }
  }

  if (pkt->getBody().getTTL()==0)
  {
     delete msg;
     return;
  }

  pkt->getBody().setTTL(pkt->getBody().getTTL()-1);

  switch (pkt->getBody().getId())
  {
  case IE11S_RANN:
      proccessRann(msg);
      break;
  case IE11S_PREQ:
      proccessPreq(msg);
      break;
  case IE11S_PREP:
      proccessPrep(msg);
      break;
  case IE11S_PERR:
      proccessPerr(msg);
      break;
  default:
      opp_error("");
  }

}


void
HwmpProtocol::sendPreq (PREQElem preq,bool isProactive)
{
  std::vector<PREQElem> preq_vector;
  preq_vector.push_back (preq);
  sendPreq (preq_vector,isProactive);
}


void HwmpProtocol::sendPreq (std::vector<PREQElem> preq,bool isProactive)
{
    Ieee802Ctrl *ctrl = new Ieee802Ctrl();
    ctrl->setInputPort(interface80211ptr->getInterfaceId());
    std::vector<MACAddress> receivers = getPreqReceivers (interface80211ptr->getInterfaceId());
    if (receivers.size()==1 && receivers[0]==MACAddress::BROADCAST_ADDRESS)
    {
        Ieee80211ActionPREQFrame* msg = createPReq (preq,false,MACAddress::BROADCAST_ADDRESS,isProactive);
        for (int i = 0; i<getNumWlanInterfaces()-1; i++)
        {
 // It's necessary to duplicate the the control info message and include the information relative to the interface
           Ieee802Ctrl *ctrlAux = ctrl->dup();
           Ieee80211ActionPREQFrame *msgAux = msg->dup();
 // Set the control info to the duplicate packet
           ctrlAux->setInputPort(getWlanInterfaceEntry(i)->getInterfaceId());
           msgAux->setControlInfo(ctrlAux);
           if (msgAux->getBody().getTTL()==0)
               delete msgAux;
           else
              sendDelayed(msgAux,delay,"to_ip");
        }
        if (msg->getBody().getTTL()==0)
              delete msg;
        else
            sendDelayed(msg,delay,"to_ip");
    }
    else
    {
        for (unsigned int i=0;i<receivers.size();i++)
        {
            Ieee80211ActionPREQFrame* msg = createPReq (preq,true,receivers[i],isProactive);
            // It's necessary to duplicate the the control info message and include the information relative to the interface
            Ieee802Ctrl *ctrlAux = ctrl->dup();
            ctrlAux->setDest(receivers[i]);
            int index  = getInterfaceReceiver(receivers[i]);
            InterfaceEntry *ie = NULL;
            if (index>=0)
                ie=getInterfaceEntryById (index);
                // Set the control info to the duplicate packet
            if (ie)
                 ctrlAux->setInputPort(ie->getInterfaceId());
             msg->setControlInfo(ctrlAux);
             if (msg->getBody().getTTL()==0)
                   delete msg;
             else
                sendDelayed(msg,delay,"to_ip");
        }
        delete ctrl;
    }
}

void HwmpProtocol::sendPrep (
    MACAddress src,
    MACAddress dst,
    MACAddress retransmitter,
    uint32_t initMetric,
    uint32_t originatorDsn,
    uint32_t destinationSN,
    uint32_t lifetime,
    uint32_t interface)
{
    Ieee80211ActionPREPFrame* ieee80211ActionPrepFrame = new Ieee80211ActionPREPFrame ();
    ieee80211ActionPrepFrame->getBody().setHopsCount(0);
    ieee80211ActionPrepFrame->getBody().setTTL(GetMaxTtl());

    ieee80211ActionPrepFrame->getBody().setOriginator(src);
    ieee80211ActionPrepFrame->getBody().setOriginatorSeqNumber(originatorDsn); // must be actualized previously
    ieee80211ActionPrepFrame->getBody().setLifeTime(GetActivePathLifetime());
    ieee80211ActionPrepFrame->getBody().setMetric(0);
    ieee80211ActionPrepFrame->getBody().setTarget(dst);
    ieee80211ActionPrepFrame->getBody().setTargetSeqNumber(destinationSN); // must be actualized previously

    ieee80211ActionPrepFrame->setReceiverAddress(retransmitter);
    ieee80211ActionPrepFrame->setTransmitterAddress(GetAddress());
    ieee80211ActionPrepFrame->setAddress3(ieee80211ActionPrepFrame->getTransmitterAddress());

    //Send Management frame
    m_stats.txPrep++;
    m_stats.txMgt++;
    m_stats.txBytes += ieee80211ActionPrepFrame->getByteLength ();
    // It's necessary to duplicate the the control info message and include the information relative to the interface
    Ieee802Ctrl *ctrl = new Ieee802Ctrl;
    ctrl->setDest(retransmitter);

    int index = getInterfaceReceiver(retransmitter);
    // Set the control info to the duplicate packet
    InterfaceEntry *ie = NULL;
    if (index>=0)
        ie=getInterfaceEntryById (index);
    if (ie)
        ctrl->setInputPort(ie->getInterfaceId());
    ieee80211ActionPrepFrame->setControlInfo(ctrl);
    if (ieee80211ActionPrepFrame->getBody().getTTL()==0)
          delete ieee80211ActionPrepFrame;
    else
        sendDelayed(ieee80211ActionPrepFrame,delay,"to_ip");
    m_stats.initiatedPrep ++;
}


void
HwmpProtocol::sendPreqProactive ()
{
  if (!isRoot())
     return;
  PREQElem preq;
  preq.TO = true;
  preq.targetAddress= MACAddress::BROADCAST_ADDRESS;
  sendPreq(preq,true);
}

Ieee80211ActionPREQFrame*
HwmpProtocol::createPReq (PREQElem preq,bool individual,MACAddress addr,bool isProactive)
{
    std::vector<PREQElem> preqVec;
    preqVec.push_back(preq);
    return createPReq (preqVec,individual,addr,isProactive);
}

Ieee80211ActionPREQFrame*
HwmpProtocol::createPReq (std::vector<PREQElem> preq,bool individual,MACAddress addr,bool isProactive)
{
  Ieee80211ActionPREQFrame* ieee80211ActionPreqFrame = new Ieee80211ActionPREQFrame ();
  if (isRoot())
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()|0x80);
  else
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()& 0x7F);
  
  if (individual)
  {
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()|0x40);
     ieee80211ActionPreqFrame->setReceiverAddress(addr);
  }
  else
  {
     ieee80211ActionPreqFrame->setReceiverAddress(MACAddress::BROADCAST_ADDRESS);
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()& 0xBF);
  }

  if (isProactive)
  {
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()|0x20);
  }
  else
  {
     ieee80211ActionPreqFrame->getBody().setFlags(ieee80211ActionPreqFrame->getBody().getFlags()& 0xDF);
  }

  ieee80211ActionPreqFrame->getBody().setHopsCount(0);
  ieee80211ActionPreqFrame->getBody().setTTL(GetMaxTtl());

  ieee80211ActionPreqFrame->getBody().setPreqElemArraySize(preq.size());
  ieee80211ActionPreqFrame->getBody().setPathDiscoveryId(GetNextPreqId());
  ieee80211ActionPreqFrame->getBody().setOriginator(GetAddress());
  ieee80211ActionPreqFrame->getBody().setOriginatorSeqNumber(m_hwmpSeqno); // must be actualized previously
  if (!isProactive)
     ieee80211ActionPreqFrame->getBody().setLifeTime(GetActivePathLifetime());
  else
     ieee80211ActionPreqFrame->getBody().setLifeTime(GetRootPathLifetime());
  ieee80211ActionPreqFrame->getBody().setMetric(0);

  for (unsigned int i=0;i<preq.size();i++)
  {
      ieee80211ActionPreqFrame->getBody().setPreqElem(i,preq[i]);
  }
  ieee80211ActionPreqFrame->getBody().setTargetCount(ieee80211ActionPreqFrame->getBody().getPreqElemArraySize());

  ieee80211ActionPreqFrame->getBody().setBodyLength(ieee80211ActionPreqFrame->getBody().getBodyLength()+(preq.size()*PREQElemLen));
  ieee80211ActionPreqFrame->setByteLength(ieee80211ActionPreqFrame->getByteLength()+(preq.size()*PREQElemLen));

  ieee80211ActionPreqFrame->setTransmitterAddress(GetAddress());
  ieee80211ActionPreqFrame->setAddress3(ieee80211ActionPreqFrame->getTransmitterAddress());
  return ieee80211ActionPreqFrame;
}


void HwmpProtocol::requestDestination (MACAddress dst, uint32_t dst_seqno)
{  
  PREQElem preq;
  preq.targetAddress = dst;
  preq.targetSeqNumber = dst_seqno;
  preq.TO = GetToFlag();
  myPendingReq.push_back(preq);
  SendMyPreq ();
}

void HwmpProtocol::SendMyPreq ()
{
  if (m_preqTimer->isScheduled ())
      return;

  if (myPendingReq.empty())
      return;

  //reschedule sending PREQ
  m_preqTimer->resched((double) m_dot11MeshHWMPpreqMinInterval);
  while (!myPendingReq.empty())
  {
      if (myPendingReq.size()>20) // The maximum value of N is 20. 80211s
      {
          std::vector<PREQElem> vectorAux;
          vectorAux.assign (myPendingReq.begin(),myPendingReq.begin()+20);
          sendPreq (vectorAux);
          myPendingReq.erase(myPendingReq.begin(),myPendingReq.begin()+20);
      }
      else
      {
          sendPreq (myPendingReq);
          myPendingReq.clear();
      }
  }
}


bool HwmpProtocol::ShouldSendPreq (MACAddress dst)
{
  std::map<MACAddress, PreqEvent>::const_iterator i = m_preqTimeouts.find (dst);
  if (i == m_preqTimeouts.end ())
    {
      m_preqTimeouts[dst].preqTimeout = new PreqTimeout(dst,this);
      m_preqTimeouts[dst].preqTimeout->resched(m_dot11MeshHWMPnetDiameterTraversalTime * 2.0);
      m_preqTimeouts[dst].whenScheduled = simTime();
      m_preqTimeouts[dst].numOfRetry = 1;
      return true;
    }
  return false;
}

void HwmpProtocol::RetryPathDiscovery (MACAddress dst)
{
  HwmpRtable::LookupResult result = m_rtable->LookupReactive (dst);
  HwmpRtable::LookupResult resultProact = m_rtable->LookupProactive();

  std::map<MACAddress, PreqEvent>::iterator i  = m_preqTimeouts.find (dst);
  if (result.retransmitter != MACAddress::BROADCAST_ADDRESS) // address valid, don't retransmit
  {
      ASSERT (i != m_preqTimeouts.end ());
      i->second.preqTimeout->removeTimer();
      delete i->second.preqTimeout;
      m_preqTimeouts.erase (i);
      return;
  }
  if (resultProact.retransmitter != MACAddress::BROADCAST_ADDRESS && !m_concurrentReactive) // address valid, don't retransmit
  {
      ASSERT (i != m_preqTimeouts.end ());
      i->second.preqTimeout->removeTimer();
      delete i->second.preqTimeout;
      m_preqTimeouts.erase (i);
      return;
  }
  if (i->second.numOfRetry > m_dot11MeshHWMPmaxPREQretries)
  {
      ASSERT (i != m_preqTimeouts.end ());
      QueuedPacket packet = DequeueFirstPacketByDst (dst);
      //purge queue and delete entry from retryDatabase
      while (packet.pkt != 0)
      {
          m_stats.totalDropped ++;
           // what to do? packet.reply (false, packet.pkt, packet.src, packet.dst, packet.protocol, HwmpRtable::MAX_METRIC);
          delete packet.pkt;
          packet = DequeueFirstPacketByDst (dst);
      }
      m_preqTimeouts.erase (i);
      i->second.preqTimeout->removeTimer();
      delete i->second.preqTimeout;
      m_preqTimeouts.erase (i);
      return;
  }
  if (i == m_preqTimeouts.end ())
  {
     m_preqTimeouts[dst].preqTimeout = new PreqTimeout(dst,this);
     m_preqTimeouts[dst].preqTimeout->resched(m_dot11MeshHWMPnetDiameterTraversalTime * 2.0);
     m_preqTimeouts[dst].whenScheduled = simTime();
     m_preqTimeouts[dst].numOfRetry = 0;
  }
  i  = m_preqTimeouts.find (dst);
  i->second.numOfRetry++; 
  GetNextHwmpSeqno (); // actualize the sequence number
  uint32_t dst_seqno = m_rtable->LookupReactiveExpired (dst).seqnum;
  requestDestination (dst,dst_seqno);
  i->second.preqTimeout->resched((2 * (i->second.numOfRetry + 1)) *  m_dot11MeshHWMPnetDiameterTraversalTime);
  
}

void HwmpProtocol::sendPerr(std::vector<HwmpFailedDestination> failedDestinations, std::vector<MACAddress> receivers)
{
    Ieee80211ActionPERRFrame * ieee80211ActionPerrFrame  = new Ieee80211ActionPERRFrame ();
    ieee80211ActionPerrFrame->getBody().setPerrElemArraySize(failedDestinations.size());
    ieee80211ActionPerrFrame->getBody().setBodyLength(ieee80211ActionPerrFrame->getBody().getBodyLength()+(failedDestinations.size()*PERRElemLen));
    ieee80211ActionPerrFrame->setByteLength(ieee80211ActionPerrFrame->getByteLength()+(failedDestinations.size()*PERRElemLen));
    ieee80211ActionPerrFrame->getBody().setTTL(GetMaxTtl());

    ieee80211ActionPerrFrame->setTransmitterAddress(GetAddress());
    ieee80211ActionPerrFrame->setAddress3(ieee80211ActionPerrFrame->getTransmitterAddress());

    for (unsigned int i=0;i<failedDestinations.size();i++)
    {
        PERRElem perr;
        perr.destAddress=failedDestinations[i].destination;
        perr.destSeqNumber=failedDestinations[i].seqnum;
        perr.reasonCode= RC_MESH_PATH_ERROR_DESTINATION_UNREACHABLE;
        ieee80211ActionPerrFrame->getBody().setPerrElem(i,perr);
    }

    if (receivers.size () >=  GetUnicastPerrThreshold ())
    {
        receivers.clear ();
        receivers.push_back (MACAddress::BROADCAST_ADDRESS);
    }
    for (unsigned int i=0;i<receivers.size()-1; i++)
    {
        //
        // 64-bit Intel valgrind complains about hdr.SetAddr1 (*i).  It likes this
        // just fine.
        //
        Ieee80211ActionPERRFrame * frameAux = ieee80211ActionPerrFrame->dup();
        frameAux->setReceiverAddress(receivers[i]);

        m_stats.txPerr++;
        m_stats.txMgt++;
        m_stats.txBytes += frameAux->getByteLength ();
        if (frameAux->getBody().getTTL()==0)
              delete frameAux;
        else
            sendDelayed(frameAux,delay,"to_ip");
      }
      ieee80211ActionPerrFrame->setReceiverAddress(receivers.back());
      m_stats.txPerr++;
      m_stats.txMgt++;
      m_stats.txBytes += ieee80211ActionPerrFrame->getByteLength ();
      if (ieee80211ActionPerrFrame->getBody().getTTL()==0)
            delete ieee80211ActionPerrFrame;
      else
            sendDelayed(ieee80211ActionPerrFrame,delay,"to_ip");
}

void HwmpProtocol::forwardPerr (std::vector<HwmpFailedDestination> failedDestinations, std::vector<MACAddress> receivers)
{

    while (!failedDestinations.empty())
    {
        if (failedDestinations.size()>20) // The maximum value of N is 20. 80211s
           {
            std::vector<HwmpFailedDestination> vectorAux;
            vectorAux.assign (failedDestinations.begin(),failedDestinations.begin()+20);
            sendPerr (vectorAux,receivers);
            failedDestinations.erase(failedDestinations.begin(),failedDestinations.begin()+20);
           }
        else
        {
            sendPerr (failedDestinations,receivers);
            failedDestinations.clear();
        }
    }
}

void HwmpProtocol::initiatePerr (std::vector<HwmpFailedDestination> failedDestinations, std::vector<MACAddress> receivers)
{
  //All duplicates in PERR are checked here, and there is no reason to
  //check it at any athoer place
  {
      std::vector<MACAddress>::const_iterator end = receivers.end ();
      for (std::vector<MACAddress>::const_iterator i = receivers.begin (); i != end; i++)
      {
          bool should_add = true;
          for (std::vector<MACAddress>::const_iterator j = m_myPerr.receivers.begin (); j
              != m_myPerr.receivers.end (); j++)
          {
              if ((*i) == (*j))
              {
                  should_add = false;
              }
          }
          if (should_add)
          {
              m_myPerr.receivers.push_back (*i);
          }
      }
  }
  {
      std::vector<HwmpFailedDestination>::const_iterator end = failedDestinations.end ();
      for (std::vector<HwmpFailedDestination>::const_iterator i = failedDestinations.begin (); i != end; i++)
      {
          bool should_add = true;
          for (std::vector<HwmpFailedDestination>::const_iterator j = m_myPerr.destinations.begin (); j
              != m_myPerr.destinations.end (); j++)
          {
              if (((*i).destination == (*j).destination) && ((*j).seqnum > (*i).seqnum))
              {
                  should_add = false;
              }
          }
          if (should_add)
          {
              m_myPerr.destinations.push_back (*i);
          }
      }
  }
  SendMyPerr ();
}

void HwmpProtocol::SendMyPerr ()
{
  if (m_perrTimer->isScheduled ())
  {
      return;
  }
  m_perrTimer->resched(GetPerrMinInterval ());
  forwardPerr (m_myPerr.destinations, m_myPerr.receivers);
  m_myPerr.destinations.clear ();
  m_myPerr.receivers.clear ();
}

uint32_t HwmpProtocol::GetLinkMetric (MACAddress peerAddress) const
{
    // TODO: Replace ETX by Airlink metric
  return interface80211ptr->getEstimateCostProcess(0)->getCost(1,peerAddress);
}

void HwmpProtocol::proccessPreq(cMessage *msg)
{
    Ieee80211ActionPREQFrame *frame = dynamic_cast<Ieee80211ActionPREQFrame*>(msg);
    if (frame==NULL)
    {
        delete msg;
        return;
    }
    MACAddress from = frame->getTransmitterAddress();
    MACAddress fromMp = frame->getAddress3();
    uint32_t metric = frame->getBody().getMetric();
    uint32_t interface;
    if (frame->getControlInfo())
    {
        Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(frame->removeControlInfo());
        interface = ctrl->getInputPort();
        delete ctrl;
    }
    else
        interface = interface80211ptr->getInterfaceId();

    receivePreq (frame, from,interface,fromMp,metric);
}

void HwmpProtocol::proccessPrep(cMessage *msg)
{
    Ieee80211ActionPREPFrame *frame = dynamic_cast<Ieee80211ActionPREPFrame*>(msg);
    if (frame==NULL)
    {
        delete msg;
        return;
    }
    MACAddress from = frame->getTransmitterAddress();
    MACAddress fromMp = frame->getAddress3();
    uint32_t metric = frame->getBody().getMetric();
    uint32_t interface;
    if (frame->getControlInfo())
    {
        Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(frame->removeControlInfo());
        interface = ctrl->getInputPort();
        delete ctrl;
    }
    else
        interface = interface80211ptr->getInterfaceId();

    receivePrep (frame, from,interface,fromMp,metric);
}

void HwmpProtocol::proccessPerr(cMessage *msg)
{
    Ieee80211ActionPERRFrame *frame = dynamic_cast<Ieee80211ActionPERRFrame*>(msg);
    if (frame==NULL)
    {
        delete msg;
        return;
    }
    MACAddress from = frame->getTransmitterAddress();
    MACAddress fromMp = frame->getAddress3();
    uint32_t interface;
    if (frame->getControlInfo())
    {
        Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl*>(frame->removeControlInfo());
        interface = ctrl->getInputPort();
        delete ctrl;
    }
    else
        interface = interface80211ptr->getInterfaceId();

    std::vector<HwmpFailedDestination> destinations;
    for (unsigned int i=0;i<frame->getBody().getPerrElemArraySize();i++)
    {
        PERRElem perr = frame->getBody().getPerrElem(i);
        HwmpFailedDestination fdest;
        fdest.destination = perr.destAddress;
        fdest.seqnum = perr.destSeqNumber;

    }
    delete msg;
    receivePerr (destinations, from, interface,fromMp);
}


void HwmpProtocol::receivePreq (Ieee80211ActionPREQFrame *preqFrame, MACAddress from, uint32_t interface, MACAddress fromMp, uint32_t metric)
{
  preqFrame->getBody().setMetric(preqFrame->getBody().getMetric()+metric);
  uint32_t totalMetric = preqFrame->getBody().getMetric();
  MACAddress originatorAddress = preqFrame->getBody().getOriginator ();
  uint32_t originatorSeqNumber= preqFrame->getBody().getOriginatorSeqNumber ();
  bool  addMode = (preqFrame->getBody().getFlags()|0x20)!=0;

  //acceptance cretirea:
  std::map<MACAddress, std::pair<uint32_t, uint32_t> >::const_iterator i = m_hwmpSeqnoMetricDatabase.find (
      originatorAddress);
  bool freshInfo (true);
  if (i != m_hwmpSeqnoMetricDatabase.end ())
  {
      if ((int32_t)(i->second.first - originatorSeqNumber)  > 0)
      {
          delete preqFrame;
          return;
      }
      if (i->second.first == originatorSeqNumber)
      {
          freshInfo = false;
          if (i->second.second <= totalMetric)
          {
              delete preqFrame;
              return;
          }
      }
  }
  m_hwmpSeqnoMetricDatabase[originatorAddress] = std::make_pair (originatorSeqNumber, totalMetric);
  EV << "I am " << GetAddress () << "Accepted preq from address" << from << ", preq:" << originatorAddress;
  //Add reactive path to originator:
  if ((freshInfo) ||
      ((m_rtable->LookupReactive (originatorAddress).retransmitter == MACAddress::BROADCAST_ADDRESS) ||
        (m_rtable->LookupReactive (originatorAddress).metric > totalMetric))
     )
  {
      m_rtable->AddReactivePath (
          originatorAddress,
          from,
          interface,
          totalMetric,
          ((double)preqFrame->getBody().getLifeTime()*1024.0)/1000000.0,
          originatorSeqNumber
        );
      ReactivePathResolved (originatorAddress);
  }
  if (
      (m_rtable->LookupReactive (fromMp).retransmitter == MACAddress::BROADCAST_ADDRESS) ||
      (m_rtable->LookupReactive (fromMp).metric > metric)
      )
  {
      m_rtable->AddReactivePath (
          fromMp,
          from,
          interface,
          metric,
          ((double)preqFrame->getBody().getLifeTime()*1024.0)/1000000.0,
          originatorSeqNumber
          );
      ReactivePathResolved (fromMp);
  }
  std::vector<MACAddress> delAddress;
  for (unsigned int  preqCount=0;preqCount<preqFrame->getBody().getPreqElemArraySize();preqCount++)
  {
      PREQElem preq =  preqFrame->getBody().getPreqElem(preqCount);
      if (preq.targetAddress == MACAddress::BROADCAST_ADDRESS)
      {
          //only proactive PREQ contains destination
          //address as broadcast! Proactive preq MUST
          //have destination count equal to 1 and
          //per destination flags DO and RF
          ASSERT (preqFrame->getBody().getTargetCount () == 1);
          //Add proactive path only if it is the better then existed
          //before
          if (
              ((m_rtable->LookupProactive ()).retransmitter == MACAddress::BROADCAST_ADDRESS) ||
              ((m_rtable->LookupProactive ()).metric > totalMetric )
            )
          {
              m_rtable->AddProactivePath (
                  totalMetric,
                  originatorAddress,
                  from,
                  interface,
                  ((double)preqFrame->getBody().getLifeTime()*1024.0)/1000000.0,
                  originatorSeqNumber);
              ProactivePathResolved ();
          }
          bool proactivePrep = false;
          if ((preqFrame->getBody().getFlags()&0x20)!=0)
              proactivePrep = true;
          if (proactivePrep)
          {
              sendPrep (
                  GetAddress (),
                  originatorAddress,
                  from,
                  (uint32_t)0,
                  originatorSeqNumber,
                  GetNextHwmpSeqno (),
                  preqFrame->getBody().getLifeTime(),
                  interface
              );
          }
          break;
      }
      if (preq.targetAddress == GetAddress ())
      {
          sendPrep (
              GetAddress (),
              originatorAddress,
              from,
              (uint32_t)0,
              originatorSeqNumber,
              GetNextHwmpSeqno (),
              preqFrame->getBody().getLifeTime(),
              interface
          );
          ASSERT(m_rtable->LookupReactive (originatorAddress).retransmitter != MACAddress::BROADCAST_ADDRESS);
          delAddress.push_back(preq.targetAddress);
          continue;
      }
      //check if can answer:
      HwmpRtable::LookupResult result = m_rtable->LookupReactive (preq.targetAddress);
      if ((! (preq.TO)) && (result.retransmitter != MACAddress::BROADCAST_ADDRESS))
      {
          //have a valid information and can answer
          uint32_t lifetime = (result.lifetime.dbl()*1000000.0 / 1024.0);
          if ((lifetime > 0) && ((int32_t)(result.seqnum - preq.targetSeqNumber ) >= 0))
          {
              sendPrep (
                  preq.targetAddress,
                  originatorAddress,
                  from,
                  result.metric,
                  originatorSeqNumber,
                  result.seqnum,
                  lifetime,
                  interface
                  );
              m_rtable->AddPrecursor (preq.targetAddress, interface, from,(preqFrame->getBody().getLifeTime()*1024)/1000000);
              delAddress.push_back(preq.targetAddress); // not propagate
              continue;
          }
      }
      if (addMode &&  preq.TO && (result.retransmitter == MACAddress::BROADCAST_ADDRESS))
      {
          delAddress.push_back(preq.targetAddress); // not propagate
          continue;
      }
    }

  if (preqFrame->getBody().getPreqElemArraySize () == delAddress.size())
  {
      delete preqFrame;
      return;
  }
  std::vector<PREQElem> preqElements;
  if (!delAddress.empty())
  {
     for (unsigned int preqCount=0;preqCount<preqFrame->getBody().getPreqElemArraySize();preqCount++)
     {
         PREQElem preq =  preqFrame->getBody().getPreqElem(preqCount);
         for (unsigned int i=0;i<delAddress.size();i++)
         {
            if (preq.targetAddress==delAddress[i])
            {
                preqElements.push_back(preq);
                delAddress.erase(delAddress.begin()+i);
                break;
            }
         }
      }
  }
  
  //check if must retransmit:
  if (preqElements.size() == 0)
  {
      delete preqFrame;
      return;
  }
// prepare the frame for retransmission
  int numEleDel = preqFrame->getBody().getPreqElemArraySize()-preqElements.size();
  preqFrame->getBody().setPreqElemArraySize(preqElements.size());
  for (unsigned int  i=0;i<preqFrame->getBody().getPreqElemArraySize();i++)
     preqFrame->getBody().setPreqElem(i,preqElements[i]);

// actualize sizes
  preqFrame->getBody().setBodyLength(preqFrame->getBody().getBodyLength()-(numEleDel*PREQElemLen));
  preqFrame->setByteLength(preqFrame->getByteLength()-(numEleDel*PREQElemLen));

// actualize address
  preqFrame->setTransmitterAddress(GetAddress());
  preqFrame->setAddress3(preqFrame->getTransmitterAddress());

  if (addMode)
  {
    int iId = interface80211ptr->getInterfaceId();
    std::vector<MACAddress> receivers = getPreqReceivers (iId);
    if (receivers.size()==1 && receivers[0]==MACAddress::BROADCAST_ADDRESS)
    {
         preqFrame->getBody().setFlags(preqFrame->getBody().getFlags()&0xDF);
         preqFrame->setReceiverAddress(MACAddress::BROADCAST_ADDRESS);
    }
    else
    {
       for (unsigned int i = 0;i<receivers.size()-1;i++)
       {
           if (from==receivers[i])
               continue;
           preqFrame->setReceiverAddress(receivers[i]);
           if (preqFrame->getBody().getTTL()==0)
           {
                 delete preqFrame;
                 return;
           }
           else
                sendDelayed(preqFrame->dup(),delay,"to_ip");
       }
       if (from != receivers.back())
           preqFrame->setReceiverAddress(receivers.back());
       else
       {
           delete preqFrame;
           return;
       }
    }
  }
  else
      preqFrame->setReceiverAddress(MACAddress::BROADCAST_ADDRESS);
  if (preqFrame->getBody().getTTL()==0)
      delete preqFrame;
  else
      sendDelayed(preqFrame,delay,"to_ip");
}

void
HwmpProtocol::receivePrep (Ieee80211ActionPREPFrame * prepFrame, MACAddress from, uint32_t interface, MACAddress fromMp, uint32_t metric)
{
  uint32_t totalMetric = prepFrame->getBody().getMetric()+metric;
  prepFrame->getBody().setMetric(totalMetric);
  MACAddress originatorAddress = prepFrame->getBody().getOriginator ();
  uint32_t originatorSeqNumber = prepFrame->getBody().getOriginatorSeqNumber ();
  MACAddress destinationAddress = prepFrame->getBody().getTarget ();
  //acceptance cretirea:
  std::map<MACAddress, std::pair<uint32_t, uint32_t> >::const_iterator i = m_hwmpSeqnoMetricDatabase.find (
      originatorAddress);
  bool freshInfo (true);
  if (i != m_hwmpSeqnoMetricDatabase.end ())
    {
      if ((int32_t)(i->second.first - originatorSeqNumber) > 0)
        {
          return;
        }
      if (i->second.first == originatorSeqNumber)
        {
          freshInfo = false;
        }
    }
  m_hwmpSeqnoMetricDatabase[originatorAddress] = std::make_pair (originatorSeqNumber, totalMetric);
  //update routing info
  //Now add a path to destination and add precursor to source
  EV << "received Prep with originator :"<< originatorAddress<< " from " << from << endl;
  HwmpRtable::LookupResult result = m_rtable->LookupReactive (destinationAddress);
  //Add a reactive path only if seqno is fresher or it improves the
  //metric
  if (
      (freshInfo) ||
      (
       ((m_rtable->LookupReactive (originatorAddress)).retransmitter == MACAddress::BROADCAST_ADDRESS) ||
       ((m_rtable->LookupReactive (originatorAddress)).metric > totalMetric)
      )
     )
    {
      m_rtable->AddReactivePath (
          originatorAddress,
          from,
          interface,
          totalMetric,
          ((double)prepFrame->getBody().getLifeTime()*1024.0)/1000000.0,
          originatorSeqNumber);
      m_rtable->AddPrecursor (destinationAddress, interface, from,
          ((double)prepFrame->getBody().getLifeTime()*1024.0)/1000000.0);
      if (result.retransmitter != MACAddress::BROADCAST_ADDRESS)
        {
          m_rtable->AddPrecursor (originatorAddress, interface, result.retransmitter,
              result.lifetime);
        }
      ReactivePathResolved (originatorAddress);
    }
  if (
      ((m_rtable->LookupReactive (fromMp)).retransmitter == MACAddress::BROADCAST_ADDRESS) ||
      ((m_rtable->LookupReactive (fromMp)).metric > metric)
      )
    {
      m_rtable->AddReactivePath (
          fromMp,
          from,
          interface,
          metric,
          ((double)prepFrame->getBody().getLifeTime()*1024.0)/1000000.0,
          originatorSeqNumber);
      ReactivePathResolved (fromMp);
    }
  if (destinationAddress == GetAddress ())
    {
      EV << "I am "<<GetAddress ()<<", resolved "<< originatorAddress << endl;
      delete prepFrame;
      return;
    }
  if (result.retransmitter == MACAddress::BROADCAST_ADDRESS)
    {
      delete prepFrame;
      return;
    }
  //Forward PREP
 // HwmpProtocolMacMap::const_iterator prep_sender = m_interfaces.find (result.ifIndex);
  // ASSERT (prep_sender != m_interfaces.end ());
  prepFrame->setTransmitterAddress(GetAddress());
  prepFrame->setAddress3(prepFrame->getTransmitterAddress());
  prepFrame->setReceiverAddress(result.retransmitter);
  if (prepFrame->getBody().getTTL()==0)
      delete prepFrame;
  else
      sendDelayed(prepFrame,delay,"to_ip");
}


void
HwmpProtocol::receivePerr (std::vector<HwmpFailedDestination> destinations, MACAddress from, uint32_t interface, MACAddress fromMp)
{
  //Acceptance cretirea:
  EV  << GetAddress ()<<", received PERR from "<<from << endl;
  std::vector<HwmpFailedDestination> retval;
  HwmpRtable::LookupResult result;
  for (unsigned int i = 0; i < destinations.size (); i ++)
  {
    result = m_rtable->LookupReactiveExpired (destinations[i].destination);
    if (!(
        (result.retransmitter != from) ||
        (result.ifIndex != interface) ||
        ((int32_t)(result.seqnum - destinations[i].seqnum) > 0)
        ))
      {
        retval.push_back (destinations[i]);
      }
  }
  if (retval.size () == 0)
    {
      return;
    }
  forwardPathError (makePathError (retval));
}


void HwmpProtocol::processLinkBreak(const cPolymorphic *details)
{
   if (dynamic_cast<Ieee80211TwoAddressFrame *>(const_cast<cPolymorphic*> (details)))
   {
       Ieee80211TwoAddressFrame *frame = dynamic_cast<Ieee80211TwoAddressFrame *>(const_cast<cPolymorphic*>(details));
       packetFailedMac(frame);
    }
}

void HwmpProtocol::packetFailedMac(Ieee80211TwoAddressFrame *frame)
{
    std::vector<HwmpFailedDestination> destinations = m_rtable->GetUnreachableDestinations (frame->getReceiverAddress());
    initiatePathError (makePathError (destinations));
}


HwmpProtocol::PathError
HwmpProtocol::makePathError (std::vector<HwmpFailedDestination> destinations)
{
  PathError retval;
  //HwmpRtable increments a sequence number as written in 11B.9.7.2
  retval.receivers = getPerrReceivers (destinations);
  if (retval.receivers.size () == 0)
    {
      return retval;
    }
  m_stats.initiatedPerr ++;
  for (unsigned int i = 0; i < destinations.size (); i ++)
  {
    retval.destinations.push_back (destinations[i]);
    m_rtable->DeleteReactivePath (destinations[i].destination);
  }
  return retval;
}

void
HwmpProtocol::initiatePathError(PathError perr)
{
  for (int i=0;i< getNumWlanInterfaces(); i ++)
  {
    std::vector<MACAddress> receivers_for_interface;
    for (unsigned int j = 0; j < perr.receivers.size (); j ++)
    {
        if (getWlanInterfaceEntry(i)->getInterfaceId() == (int)perr.receivers[j].first)
            receivers_for_interface.push_back (perr.receivers[j].second);
    }
    initiatePerr (perr.destinations, receivers_for_interface);
  }
}

void
HwmpProtocol::forwardPathError(PathError perr)
{
  for (int i=0;i< getNumWlanInterfaces(); i ++)
  {
    std::vector<MACAddress> receivers_for_interface;
    for (unsigned int j = 0; j < perr.receivers.size (); j ++)
    {
        if (getWlanInterfaceEntry(i)->getInterfaceId() == (int)perr.receivers[j].first)
            receivers_for_interface.push_back (perr.receivers[j].second);
    }
    forwardPerr (perr.destinations, receivers_for_interface);
  }
}

std::vector<std::pair<uint32_t, MACAddress> >
HwmpProtocol::getPerrReceivers (std::vector<HwmpFailedDestination> failedDest)
{
  HwmpRtable::PrecursorList retval;
  for (unsigned int i = 0; i < failedDest.size (); i ++)
  {
    HwmpRtable::PrecursorList precursors = m_rtable->GetPrecursors (failedDest[i].destination);
    m_rtable->DeleteReactivePath (failedDest[i].destination);
    m_rtable->DeleteProactivePath (failedDest[i].destination);
    for (unsigned int j = 0; j < precursors.size (); j ++)
        retval.push_back (precursors[j]);
  }
  //Check if we have dublicates in retval and precursors:
  for (unsigned int i = 0; i < retval.size (); i ++)
  {
      for (unsigned int j = i+1; j < retval.size (); j ++)
      {
          if (retval[i].second == retval[j].second)
              retval.erase (retval.begin () + j);
      }
  }
  return retval;
}


std::vector<MACAddress>
HwmpProtocol::getPreqReceivers (uint32_t interface)
{
  std::vector<MACAddress> retval;
  int numNeigh = 0;

  InterfaceEntry *ie = getInterfaceEntryById(interface);
  if (ie->getEstimateCostProcess(0))
    numNeigh = ie->getEstimateCostProcess(0)->getNumNeighbors();

  if ((numNeigh >= m_unicastPreqThreshold) || (numNeigh == 0))
  {
      retval.clear ();
      retval.push_back (MACAddress::BROADCAST_ADDRESS);
      return retval;
  }

  MACAddress addr[m_unicastPreqThreshold];
  ie->getEstimateCostProcess(0)->getNeighbors(addr);

  retval.clear ();
  for (int i=0;i<numNeigh;i++)
      retval.push_back (addr[i]);
  return retval;
}

std::vector<MACAddress>
HwmpProtocol::getBroadcastReceivers (uint32_t interface)
{
  std::vector<MACAddress> retval;
  int numNeigh = 0;
  InterfaceEntry *ie = getInterfaceEntryById(interface);

  if (ie->getEstimateCostProcess(0))
    numNeigh = ie->getEstimateCostProcess(0)->getNumNeighbors();

  if ((numNeigh >= m_unicastDataThreshold) || (numNeigh == 0))
  {
      retval.clear ();
      retval.push_back (MACAddress::BROADCAST_ADDRESS);
      return retval;
  }

  MACAddress addr[m_unicastDataThreshold];
  ie->getEstimateCostProcess(0)->getNeighbors(addr);

  retval.clear ();
  for (int i=0;i<numNeigh;i++)
    retval.push_back (addr[i]);
  return retval;
}

bool
HwmpProtocol::QueuePacket (QueuedPacket packet)
{
  if (m_rqueue.size () > m_maxQueueSize)
    {
      return false;
    }
  m_rqueue.push_back (packet);
  return true;
}

HwmpProtocol::QueuedPacket
HwmpProtocol::DequeueFirstPacketByDst (MACAddress dst)
{
  HwmpProtocol::QueuedPacket retval;
  retval.pkt = 0;
  for (std::vector<QueuedPacket>::iterator i = m_rqueue.begin (); i != m_rqueue.end (); i++)
    {
      if ((*i).dst == dst)
        {
          retval = (*i);
          m_rqueue.erase (i);
          break;
        }
    }
  return retval;
}

HwmpProtocol::QueuedPacket
HwmpProtocol::DequeueFirstPacket ()
{
  HwmpProtocol::QueuedPacket retval;
  retval.pkt = 0;
  if (m_rqueue.size () != 0)
    {
      retval = m_rqueue[0];
      m_rqueue.erase (m_rqueue.begin ());
    }
  return retval;
}

void
HwmpProtocol::ReactivePathResolved (MACAddress dst)
{
  std::map<MACAddress, PreqEvent>::iterator i = m_preqTimeouts.find (dst);

  HwmpRtable::LookupResult result = m_rtable->LookupReactive (dst);
  ASSERT(result.retransmitter != MACAddress::BROADCAST_ADDRESS);
  //Send all packets stored for this destination
  QueuedPacket packet = DequeueFirstPacketByDst (dst);
  while (packet.pkt != 0)
    {
      m_stats.txUnicast ++;
      m_stats.txBytes += packet.pkt->getByteLength ();
      send(packet.pkt,"to_ip");
      packet = DequeueFirstPacketByDst (dst);
    }
}

void
HwmpProtocol::ProactivePathResolved ()
{
  //send all packets to root
  HwmpRtable::LookupResult result = m_rtable->LookupProactive ();
  ASSERT (result.retransmitter != MACAddress::BROADCAST_ADDRESS);
  QueuedPacket packet = DequeueFirstPacket ();
  while (packet.pkt != 0)
    {
      m_stats.txUnicast ++;
      m_stats.txBytes += packet.pkt->getByteLength ();
      send(packet.pkt,"to_ip");
      packet = DequeueFirstPacket ();
    }
}



//Proactive PREQ routines:
void
HwmpProtocol::setRoot ()
{
  double randomStart = uniform(0.0, m_randomStart);
  m_proactivePreqTimer->resched(randomStart);
  EV << "ROOT IS: " << GetAddress ();
  m_isRoot = true;
}

void
HwmpProtocol::UnsetRoot ()
{
  m_proactivePreqTimer->removeTimer();
}

void
HwmpProtocol::SendProactivePreq ()
{
  sendPreqProactive();
}

bool
HwmpProtocol::GetToFlag ()
{
  return m_ToFlag;
}

double
HwmpProtocol::GetPreqMinInterval ()
{
  return m_dot11MeshHWMPpreqMinInterval;
}

double
HwmpProtocol::GetPerrMinInterval ()
{
  return m_dot11MeshHWMPperrMinInterval;
}
uint8_t
HwmpProtocol::GetMaxTtl ()
{
  return m_maxTtl;
}

uint32_t
HwmpProtocol::GetNextPreqId ()
{
  m_preqId ++;
  return m_preqId;
}

uint32_t
HwmpProtocol::GetNextHwmpSeqno ()
{
  m_hwmpSeqno ++;
  return m_hwmpSeqno;
}

uint32_t
HwmpProtocol::GetActivePathLifetime ()
{
  return m_dot11MeshHWMPactivePathTimeout * 1000000 / 1024;
}


uint32_t
HwmpProtocol::GetRootPathLifetime ()
{
  return m_dot11MeshHWMPactiveRootTimeout * 1000000 / 1024;
}


uint8_t
HwmpProtocol::GetUnicastPerrThreshold ()
{
  return m_unicastPerrThreshold;
}
MACAddress
HwmpProtocol::GetAddress ()
{
  return getAddress().getMACAddress();
}

bool HwmpProtocol::isOurType(cPacket *p)
{
   if (dynamic_cast<Ieee80211ActionHWMPFrame*>(p))
      return true;
   return false;
}

bool HwmpProtocol::getDestAddress(cPacket *msg,Uint128 &addr)
{
    Ieee80211ActionPREQFrame *rreq = dynamic_cast <Ieee80211ActionPREQFrame *>(msg);
    if (!rreq)
        return false;
    PREQElem pqeq= rreq->getBody().getPreqElem(0);
    addr = pqeq.targetAddress;
    return true;
}

bool  HwmpProtocol::getNextHop(const Uint128 &dest,Uint128 &add, int &iface,double &cost)
{
   HwmpRtable::LookupResult result = m_rtable->LookupReactive (dest.getMACAddress());
   HwmpRtable::LookupResult resultProact = m_rtable->LookupProactive ();
   if (result.retransmitter == MACAddress::BROADCAST_ADDRESS) // address not valid
   {
      result = m_rtable->LookupProactive ();
      if (m_concurrentReactive && !isRoot())
      {
          if (ShouldSendPreq (dest.getMACAddress()))
          {
              GetNextHwmpSeqno ();
              uint32_t dst_seqno = 0;
                m_stats.initiatedPreq ++;
                requestDestination (dest.getMACAddress(), dst_seqno);
          }
      }
   }
   if(resultProact.retransmitter == MACAddress::BROADCAST_ADDRESS)
          return false; // the Mesh code should send the packet to hwmp
   add=result.retransmitter;
   cost=result.metric;
   iface=result.ifIndex;
   return true;
}

bool  HwmpProtocol::getNextHopReactive(const Uint128 &dest,Uint128 &add, int &iface,double &cost)
{
   HwmpRtable::LookupResult result = m_rtable->LookupReactive (dest.getMACAddress());
   if (result.retransmitter == MACAddress::BROADCAST_ADDRESS) // address not valid
       return false;
   add=result.retransmitter;
   cost=result.metric;
   iface=result.ifIndex;
   return true;
}


bool  HwmpProtocol::getNextHopProactive(const Uint128 &dest,Uint128 &add, int &iface,double &cost)
{
   HwmpRtable::LookupResult result = m_rtable->LookupProactive ();
   if(result.retransmitter == MACAddress::BROADCAST_ADDRESS)
       return false;
   add=result.retransmitter;
   cost=result.metric;
   iface=result.ifIndex;
   return true;
}

int  HwmpProtocol::getInterfaceReceiver(MACAddress add)
{
   HwmpRtable::LookupResult result = m_rtable->LookupReactive (add);
   if (result.retransmitter == MACAddress::BROADCAST_ADDRESS) // address not valid
      result = m_rtable->LookupProactive ();
   if(result.retransmitter == MACAddress::BROADCAST_ADDRESS)
          return -1;
   if (result.retransmitter != add)
       return -1;
   return result.ifIndex;;
}
