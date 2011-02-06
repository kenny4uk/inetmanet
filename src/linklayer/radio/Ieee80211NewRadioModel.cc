//
// Copyright (C) 2006 Andras Varga, Levente Meszaros
// Based on the Mobility Framework's SnrEval by Marc Loebbers
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
// 2010 Alfoso Ariza (universidad de Málaga), new radio model, inspired in the yans and ns3 models

#include "Ieee80211NewRadioModel.h"
#include "Ieee80211Consts.h"
#include "FWMath.h"
#define NS3CALMODE


Register_Class(Ieee80211NewRadioModel);

Ieee80211NewRadioModel::~Ieee80211NewRadioModel()
{
    if (parseTable)
        delete parseTable;
}


void Ieee80211NewRadioModel::initializeFrom(cModule *radioModule)
{
    snirThreshold = dB2fraction(radioModule->par("snirThreshold"));


    if (strcmp("SHORT",radioModule->par("WifiPreambreMode").stringValue())==0)
        wifiPreamble =WIFI_PREAMBLE_SHORT;
    else if (strcmp("LONG",radioModule->par("WifiPreambreMode").stringValue())==0)
        wifiPreamble =WIFI_PREAMBLE_LONG;
    else
        wifiPreamble =WIFI_PREAMBLE_LONG;

    if (strcmp("b",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='b';
    else if (strcmp("g",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='g';
    else
        phyOpMode='g';

    parseTable = NULL;
    PHY_HEADER_LENGTH=26e-6;

    snirVector.setName("snirVector");
    i=0;
    const char *fname = radioModule->par("berTableFile");
    std::string name (fname);
    if (!name.empty())
    {
        parseTable = new BerParseFile(phyOpMode);
        parseTable->parseFile(fname);
        fileBer=true;
    }
    else
        fileBer=false;
}



double Ieee80211NewRadioModel::calculateDuration(AirFrame *airframe)
{
    double duration;
#ifndef NS3CALMODE
    if (phyOpMode=='g')
        duration=4*ceil((16+airframe->getBitLength()+6)/(airframe->getBitrate()/1e6*4))*1e-6 + PHY_HEADER_LENGTH;
    else
        // The physical layer header is sent with 1Mbit/s and the rest with the frame's bitrate
        duration=airframe->getBitLength()/airframe->getBitrate() + 192/BITRATE_HEADER;
#else

    if (phyOpMode=='g')
    {
    	ModulationType modeBody = WifyModulationType::getMode80211g(airframe->getBitrate());
        duration=SIMTIME_DBL(calculateTxDuration (airframe->getBitLength(),modeBody, wifiPreamble));
    }
    else if (phyOpMode=='b')
    {
        // The physical layer header is sent with 1Mbit/s and the rest with the frame's bitrate
    	ModulationType modeBody = WifyModulationType::getMode80211b(airframe->getBitrate());
        duration=SIMTIME_DBL(calculateTxDuration (airframe->getBitLength(),modeBody,wifiPreamble));
    }
    else if (phyOpMode=='a')
    {
        // The physical layer header is sent with 1Mbit/s and the rest with the frame's bitrate
    	ModulationType modeBody = WifyModulationType::getMode80211a(airframe->getBitrate());
        duration=SIMTIME_DBL(calculateTxDuration (airframe->getBitLength(),modeBody,wifiPreamble));
    }
    else
        opp_error("Radio model not supported yet, must be a,b or g");
#endif
    EV<<"Radio:frameDuration="<<duration*1e6<<"us("<<airframe->getBitLength()<<"bits)"<<endl;
    return duration;
}


bool Ieee80211NewRadioModel::isReceivedCorrectly(AirFrame *airframe, const SnrList& receivedList)
{
    // calculate snirMin
    double snirMin = receivedList.begin()->snr;
    for (SnrList::const_iterator iter = receivedList.begin(); iter != receivedList.end(); iter++)
        if (iter->snr < snirMin)
            snirMin = iter->snr;

    cPacket *frame = airframe->getEncapsulatedPacket();
    EV << "packet (" << frame->getClassName() << ")" << frame->getName() << " (" << frame->info() << ") snrMin=" << snirMin << endl;

    if (i%1000==0)
    {
        snirVector.record(10*log10(snirMin));
        i=0;
    }
    i++;

    if (snirMin <= snirThreshold)
    {
        // if snir is too low for the packet to be recognized
        EV << "COLLISION! Packet got lost. Noise only\n";
        return false;
    }
    else if (isPacketOK(snirMin, frame->getBitLength(), airframe->getBitrate()))
    {
        EV << "packet was received correctly, it is now handed to upper layer...\n";
        return true;
    }
    else
    {
        EV << "Packet has BIT ERRORS! It is lost!\n";
        return false;
    }
}


bool Ieee80211NewRadioModel::isPacketOK(double snirMin, int lengthMPDU, double bitrate)
{
    double berHeader, berMPDU;
    ModulationType modeBody;
    ModulationType modeHeader;

    WifiPreamble preambleUsed = wifiPreamble;
    double headerNoError;

    if (phyOpMode=='g')
    {
        modeBody =WifyModulationType::getMode80211g(bitrate);
        modeHeader = getPlcpHeaderMode (modeBody, preambleUsed);
        uint32_t headerSize = ceil(SIMTIME_DBL(getPlcpHeaderDuration (modeBody, preambleUsed))*modeHeader.getDataRate());
        headerNoError = yansModel.GetChunkSuccessRate(modeHeader,snirMin,headerSize);
    }
    else if (phyOpMode=='b')
    {
        modeBody =WifyModulationType::getMode80211g(bitrate);
        modeHeader = getPlcpHeaderMode (modeBody,preambleUsed);
        uint32_t headerSize = ceil(SIMTIME_DBL(getPlcpHeaderDuration (modeBody, preambleUsed))*modeHeader.getDataRate());
        headerNoError = yansModel.GetChunkSuccessRate(modeHeader,snirMin,headerSize);

    }
    else if (phyOpMode=='a')
    {
        modeBody =WifyModulationType::getMode80211a(bitrate);
        modeHeader = getPlcpHeaderMode (modeBody, preambleUsed);
        uint32_t headerSize = ceil(SIMTIME_DBL(getPlcpHeaderDuration (modeBody, preambleUsed))*modeHeader.getDataRate());
        headerNoError = yansModel.GetChunkSuccessRate(modeHeader,snirMin,headerSize);
    }
    else
    {
        opp_error("Radio model not supported yet, must be a,b or g");
    }
    // probability of no bit error in the MPDU
    double MpduNoError;
    if (fileBer)
        MpduNoError=1-parseTable->getPer(bitrate,snirMin,lengthMPDU/8);
    else
        MpduNoError = yansModel.GetChunkSuccessRate(modeHeader,snirMin,lengthMPDU);

    EV << "berHeader: " << berHeader << " berMPDU: " <<berMPDU <<" lengthMPDU: "<<lengthMPDU<<" PER: "<<1-MpduNoError<<endl;
    if (MpduNoError>=1 && headerNoError>=1)
        return true;
    double rand = dblrand();

    if (rand > headerNoError)
        return false; // error in header
    else if (dblrand() > MpduNoError)
        return false;  // error in MPDU
    else
        return true; // no error
}

double Ieee80211NewRadioModel::dB2fraction(double dB)
{
    return pow(10.0, (dB / 10));
}

simtime_t
Ieee80211NewRadioModel::getPlcpHeaderDuration (ModulationType payloadMode, WifiPreamble preamble)
{
  switch (payloadMode.getModulationClass ())
    {
    case MOD_CLASS_OFDM:
      {
        switch (payloadMode.getBandwidth ()) {
        case 20000000:
        default:
          // IEEE Std 802.11-2007, section 17.3.3 and figure 17-4
          // also section 17.3.2.3, table 17-4
          // We return the duration of the SIGNAL field only, since the
          // SERVICE field (which strictly speaking belongs to the PLCP
          // header, see section 17.3.2 and figure 17-1) is sent using the
          // payload mode.
          return 4.0/1000000.0;
        case 10000000:
          // IEEE Std 802.11-2007, section 17.3.2.3, table 17-4
          return 8;
        case 5000000:
          // IEEE Std 802.11-2007, section 17.3.2.3, table 17-4
          return 16.0/1000000.0;
        }
      }

    case MOD_CLASS_ERP_OFDM:
      return 16.0/1000000.0;

    case MOD_CLASS_DSSS:
      if (preamble == WIFI_PREAMBLE_SHORT)
        {
          // IEEE Std 802.11-2007, section 18.2.2.2 and figure 18-2
          return 24.0/1000000.0;
        }
      else // WIFI_PREAMBLE_LONG
        {
          // IEEE Std 802.11-2007, sections 18.2.2.1 and figure 18-1
          return 48.0/1000000.0;
        }

    default:
      opp_error("unsupported modulation class");
      return 0;
    }
}

simtime_t
Ieee80211NewRadioModel::getPlcpPreambleDuration (ModulationType payloadMode, WifiPreamble preamble)
{
  switch (payloadMode.getModulationClass ())
    {
    case MOD_CLASS_OFDM:
      {
        switch (payloadMode.getBandwidth ()) {
        case 20000000:
        default:
          // IEEE Std 802.11-2007, section 17.3.3,  figure 17-4
          // also section 17.3.2.3, table 17-4
          return 16.0/1000000.0;
        case 10000000:
          // IEEE Std 802.11-2007, section 17.3.3, table 17-4
          // also section 17.3.2.3, table 17-4
          return 32.0/1000000.0;
        case 5000000:
          // IEEE Std 802.11-2007, section 17.3.3
          // also section 17.3.2.3, table 17-4
          return 64.0/1000000.0;
        }
      }

    case MOD_CLASS_ERP_OFDM:
      return 4.0/1000000.0;

    case MOD_CLASS_DSSS:
      if (preamble == WIFI_PREAMBLE_SHORT)
        {
          // IEEE Std 802.11-2007, section 18.2.2.2 and figure 18-2
          return 72.0/1000000.0;
        }
      else // WIFI_PREAMBLE_LONG
        {
          // IEEE Std 802.11-2007, sections 18.2.2.1 and figure 18-1
          return 144.0/1000000.0;
        }

    default:
      opp_error("unsupported modulation class");
      return 0;
    }
}

simtime_t
Ieee80211NewRadioModel::getPayloadDuration (uint32_t size, ModulationType payloadMode)
{
  simtime_t val;
  switch (payloadMode.getModulationClass ())
    {
    case MOD_CLASS_OFDM:
    case MOD_CLASS_ERP_OFDM:
      {
        // IEEE Std 802.11-2007, section 17.3.2.3, table 17-4
        // corresponds to T_{SYM} in the table
        uint32_t symbolDurationUs;

        switch (payloadMode.getBandwidth ()) {
        case 20000000:
        default:
          symbolDurationUs = 4;
          break;
        case 10000000:
          symbolDurationUs = 8;
          break;
        case 5000000:
          symbolDurationUs = 16;
          break;
        }

        // IEEE Std 802.11-2007, section 17.3.2.2, table 17-3
        // corresponds to N_{DBPS} in the table
        double numDataBitsPerSymbol = payloadMode.getDataRate ()  * symbolDurationUs / 1e6;

        // IEEE Std 802.11-2007, section 17.3.5.3, equation (17-11)
        uint32_t numSymbols = lrint (ceil ((16 + size * 8.0 + 6.0)/numDataBitsPerSymbol));

        // Add signal extension for ERP PHY
        double aux;
        if (payloadMode.getModulationClass () == MOD_CLASS_ERP_OFDM)
          aux = numSymbols*symbolDurationUs + 6;
        else
          aux = numSymbols*symbolDurationUs;
        val =(aux/1000000);
        return val;
      }
    case MOD_CLASS_DSSS:
      // IEEE Std 802.11-2007, section 18.2.3.5
      double aux;
      aux = lrint(ceil ((size * 8.0) / (payloadMode.getDataRate () / 1.0e6)));
      val =(aux/1000000);
      return val;
      break;
    default:
      opp_error("unsupported modulation class");
      return 0;
    }
}

simtime_t
Ieee80211NewRadioModel::calculateTxDuration (uint32_t size, ModulationType payloadMode, WifiPreamble preamble)
{
  simtime_t duration = getPlcpPreambleDuration (payloadMode, preamble)
                      + getPlcpHeaderDuration (payloadMode, preamble)
                      + getPayloadDuration (size, payloadMode);
  return duration;
}

ModulationType
Ieee80211NewRadioModel::getPlcpHeaderMode (ModulationType payloadMode, WifiPreamble preamble)
{
  switch (payloadMode.getModulationClass ())
     {
     case MOD_CLASS_OFDM:
       {
         switch (payloadMode.getBandwidth ()) {
         case 5000000:
           return WifyModulationType::GetOfdmRate1_5MbpsBW5MHz ();
         case 10000000:
           return WifyModulationType::GetOfdmRate3MbpsBW10MHz ();
         default:
           // IEEE Std 802.11-2007, 17.3.2
           // actually this is only the first part of the PlcpHeader,
           // because the last 16 bits of the PlcpHeader are using the
           // same mode of the payload
           return WifyModulationType::GetOfdmRate6Mbps ();
         }
       }

     case MOD_CLASS_ERP_OFDM:
       return WifyModulationType::GetErpOfdmRate6Mbps ();

     case MOD_CLASS_DSSS:
       if (preamble == WIFI_PREAMBLE_LONG)
         {
           // IEEE Std 802.11-2007, sections 15.2.3 and 18.2.2.1
           return WifyModulationType::GetDsssRate1Mbps ();
         }
       else //  WIFI_PREAMBLE_SHORT
         {
           // IEEE Std 802.11-2007, section 18.2.2.2
           return WifyModulationType::GetDsssRate2Mbps ();
         }

     default:
       opp_error("unsupported modulation class");
       return ModulationType ();
     }
}
