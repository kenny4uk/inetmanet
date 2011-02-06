/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include <omnetpp.h>
#include "WifiMode.h"
#include "Ieee80211DataRate.h"


ModulationType
WifyModulationType::GetDsssRate1Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_DSSS);
   mode.setBandwidth(22000000);
   mode.setDataRate(1000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetDsssRate2Mbps ()
{
  
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_DSSS);
   mode.setBandwidth(22000000);
   mode.setDataRate(2000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(2);
   return mode;

}

/**
 * Clause 18 rates (HR/DSSS)
 */
ModulationType
WifyModulationType::GetDsssRate5_5Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_DSSS);
   mode.setBandwidth(22000000);
   mode.setDataRate(5500000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetDsssRate11Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_DSSS);
   mode.setBandwidth(22000000);
   mode.setDataRate(11000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(4);
   return mode;
}


/**
 * Clause 19.5 rates (ERP-OFDM)
 */
ModulationType
WifyModulationType::GetErpOfdmRate6Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(6000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate9Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(9000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate12Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(12000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate18Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(18000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate24Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(24000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate36Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(36000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate48Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(48000000);
   mode.setCodeRate(CODE_RATE_2_3);
   mode.setConstellationSize(64);
   return mode;
}

ModulationType
WifyModulationType::GetErpOfdmRate54Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_ERP_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(54000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(64);
   return mode;
}


/**
 * Clause 17 rates (OFDM)
 */
ModulationType
WifyModulationType::GetOfdmRate6Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(6000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate9Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(9000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate12Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(12000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate18Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(18000000);
   mode.setCodeRate(CODE_RATE_UNDEFINED);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate24Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(24000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate36Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(36000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate48Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(48000000);
   mode.setCodeRate(CODE_RATE_2_3);
   mode.setConstellationSize(64);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate54Mbps ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(20000000);
   mode.setDataRate(54000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(64);
   return mode;
}

/* 10 MHz channel rates */
ModulationType
WifyModulationType::GetOfdmRate3MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(3000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate4_5MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(4500000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate6MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(6000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate9MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(9000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate12MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(12000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate18MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(18000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate24MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(24000000);
   mode.setCodeRate(CODE_RATE_2_3);
   mode.setConstellationSize(64);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate27MbpsBW10MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(10000000);
   mode.setDataRate(27000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(64);
   return mode;
}

/* 5 MHz channel rates */
ModulationType
WifyModulationType::GetOfdmRate1_5MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(1500000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate2_25MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(2250000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(2);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate3MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(3000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate4_5MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(4500000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(4);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate6MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(6000000);
   mode.setCodeRate(CODE_RATE_1_2);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate9MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(9000000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(16);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate12MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(12000000);
   mode.setCodeRate(CODE_RATE_2_3);
   mode.setConstellationSize(64);
   return mode;
}

ModulationType
WifyModulationType::GetOfdmRate13_5MbpsBW5MHz ()
{
   ModulationType mode;
   mode.setModulationClass(MOD_CLASS_OFDM);
   mode.setBandwidth(5000000);
   mode.setDataRate(13500000);
   mode.setCodeRate(CODE_RATE_3_4);
   mode.setConstellationSize(64);
   return mode;
}

ModulationType
WifyModulationType:: getMode80211a(double bitrate)
{
   if (bitrate== BITRATES_80211g[7])
	   return GetOfdmRate54Mbps();
   else if (bitrate== BITRATES_80211g[6])
	   return GetOfdmRate48Mbps();
   else if (bitrate== BITRATES_80211g[5])
	   return GetOfdmRate36Mbps();
   else if (bitrate== BITRATES_80211g[4])
	   return GetOfdmRate24Mbps();
   else if (bitrate== BITRATES_80211g[3])
	   return GetOfdmRate18Mbps();
   else if (bitrate== BITRATES_80211g[2])
	   return GetOfdmRate12Mbps();
   else if (bitrate== BITRATES_80211g[1])
	   return GetOfdmRate9Mbps();
   else if (bitrate== BITRATES_80211g[0])
	   return GetOfdmRate6Mbps();
   else
	   opp_error("mode not valid");
   return ModulationType();
}

ModulationType
WifyModulationType:: getMode80211g(double bitrate)
{
   if (bitrate== BITRATES_80211g[11]) 
	   return GetErpOfdmRate54Mbps();
   else if (bitrate== BITRATES_80211g[10])
	   return GetErpOfdmRate48Mbps();
   else if (bitrate== BITRATES_80211g[9]) 
	   return GetErpOfdmRate36Mbps();
   else if (bitrate== BITRATES_80211g[8]) 
	   return GetErpOfdmRate24Mbps();
   else if (bitrate== BITRATES_80211g[7])
	   return GetErpOfdmRate18Mbps();
   else if (bitrate== BITRATES_80211g[6]) 
	   return GetErpOfdmRate12Mbps();
   else if (bitrate== BITRATES_80211g[5]) 
	   return GetDsssRate11Mbps();
   else if (bitrate== BITRATES_80211g[4]) 
	   return GetErpOfdmRate9Mbps();
   else if (bitrate== BITRATES_80211g[3]) 
	   return GetErpOfdmRate6Mbps();
   else if (bitrate== BITRATES_80211g[2]) 
	   return GetDsssRate5_5Mbps();
   else if (bitrate== BITRATES_80211g[1]) 
	   return GetDsssRate2Mbps();
   else if (bitrate== BITRATES_80211g[0]) 
	   return GetDsssRate1Mbps();
   else
	   opp_error("mode not valid");
   return ModulationType();
}


ModulationType
WifyModulationType:: getMode80211b(double bitrate)
{
   if (bitrate== BITRATES_80211b[4])
	   return GetDsssRate11Mbps();
   else if (bitrate== BITRATES_80211b[2]) 
	   return GetDsssRate5_5Mbps();
   else if (bitrate== BITRATES_80211b[1]) 
	   return GetDsssRate2Mbps();
   else if (bitrate== BITRATES_80211b[0]) 
	   return GetDsssRate1Mbps();
   else
	   opp_error("mode not valid");
   return ModulationType();
}


ModulationType
WifyModulationType::getMode80211p (double bitrate)
{
   if (bitrate== BITRATES_80211p[7]) 
       return GetOfdmRate27MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[6]) 
	   return GetOfdmRate24MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[5]) 
	   return GetOfdmRate18MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[4]) 
	   return GetOfdmRate12MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[3]) 
	   return GetOfdmRate9MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[2]) 
	   return GetOfdmRate6MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[1]) 
	   return GetOfdmRate4_5MbpsBW10MHz();
   else if (bitrate== BITRATES_80211p[0]) 
	   return GetOfdmRate3MbpsBW10MHz();
   else
	   opp_error("mode not valid");
   return ModulationType();
}

