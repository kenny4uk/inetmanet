#ifndef __WIFI_MODULATION_TYPE_H__
#define __WIFI_MODULATION_TYPE_H__
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
#include <ModulationType.h>

class WifyModulationType
{
public:
  ModulationType GetDsssRate1Mbps ();
  ModulationType GetDsssRate2Mbps ();
  ModulationType GetDsssRate5_5Mbps ();
  ModulationType GetDsssRate11Mbps ();
  ModulationType GetErpOfdmRate6Mbps ();
  ModulationType GetErpOfdmRate9Mbps ();
  ModulationType GetErpOfdmRate12Mbps ();
  ModulationType GetErpOfdmRate18Mbps ();
  ModulationType GetErpOfdmRate24Mbps ();
  ModulationType GetErpOfdmRate36Mbps ();
  ModulationType GetErpOfdmRate48Mbps ();
  ModulationType GetErpOfdmRate54Mbps ();
  ModulationType GetOfdmRate6Mbps ();
  ModulationType GetOfdmRate9Mbps ();
  ModulationType GetOfdmRate12Mbps ();
  ModulationType GetOfdmRate18Mbps ();
  ModulationType GetOfdmRate24Mbps ();
  ModulationType GetOfdmRate36Mbps ();
  ModulationType GetOfdmRate48Mbps ();
  ModulationType GetOfdmRate54Mbps ();
  ModulationType GetOfdmRate3MbpsBW10MHz ();
  ModulationType GetOfdmRate4_5MbpsBW10MHz ();
  ModulationType GetOfdmRate6MbpsBW10MHz ();
  ModulationType GetOfdmRate9MbpsBW10MHz ();
  ModulationType GetOfdmRate12MbpsBW10MHz ();
  ModulationType GetOfdmRate18MbpsBW10MHz ();
  ModulationType GetOfdmRate24MbpsBW10MHz ();
  ModulationType GetOfdmRate27MbpsBW10MHz ();
  ModulationType GetOfdmRate1_5MbpsBW5MHz ();
  ModulationType GetOfdmRate2_25MbpsBW5MHz ();
  ModulationType GetOfdmRate3MbpsBW5MHz ();
  ModulationType GetOfdmRate4_5MbpsBW5MHz ();
  ModulationType GetOfdmRate6MbpsBW5MHz ();
  ModulationType GetOfdmRate9MbpsBW5MHz ();
  ModulationType GetOfdmRate12MbpsBW5MHz ();
  ModulationType GetOfdmRate13_5MbpsBW5MHz ();

  ModulationType getMode80211a(uint32_t bitrate);
  ModulationType getMode80211b(uint32_t bitrate);
  ModulationType getMode80211g(uint32_t bitrate);
  ModulationType getMode80211p(uint32_t bitrate);
};
#endif

