/* MSHV
 *
 * By Hrisimir Hristov - LZ2HV
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef NETWORK_DEF_H
#define NETWORK_DEF_H

#define ICOMLAN_FIRST 15 //IC8TEM Icom LAN (one entry for all Icom radios with LAN)
#define ICOMLAN_COUNT 1
#define IS_ICOMLAN(i) ((i)>=ICOMLAN_FIRST && (i)<ICOMLAN_FIRST+ICOMLAN_COUNT)
#if defined HAVE_HAMLIB
#define NETWORK_COUNT 19 //IC8TEM +1 Icom LAN +3 Icom (Hamlib): IC-7300, IC-7610, IC-9700
#define HAMLIB_FIRST 16  //IC8TEM Hamlib library: Server = serial port, Port = baud rate
#define HAMLIB_COUNT 3
#define HAMLIB_IC7300 16
#else
#define NETWORK_COUNT 16 //IC8TEM +1 Icom LAN
#define HAMLIB_FIRST -100
#define HAMLIB_COUNT 0
#define HAMLIB_IC7300 -100
#endif
#define IS_HAMLIB(i) ((i)>=HAMLIB_FIRST && (i)<HAMLIB_FIRST+HAMLIB_COUNT)
// Hamlib model numbers: RIG_MODEL_IC7300 3073, RIG_MODEL_IC7610 3078, RIG_MODEL_IC9700 3081
#define HAMLIB_HL_MODEL(i) ((i)==HAMLIB_FIRST+1 ? 3078 : ((i)==HAMLIB_FIRST+2 ? 3081 : 3073))
#if defined _NETWORK_RIGS_
#include "../rigdef.h"
#define NETWORK_ID 13
static RigSet rigs_network[NETWORK_COUNT] =
    {
        {"Ham Radio Deluxe",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FLRig",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"DX Lab Suite Commander",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"TCI Client RX1",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},//tci
        {"TCI Client RX2",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},//tci
        {"Hamlib NET rigctl",RIG_PTT_RIG_MICDATA,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},//or Hamlib NET rigctl
        {"FlexRadio 6xxx",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice A TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice B TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice C TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice D TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice E TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice F TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"FlexRadio SmartSDR Slice G TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},   
        {"FlexRadio SmartSDR Slice H TCP",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        //IC8TEM Icom LAN (native RS-BA1 network protocol, CAT + audio)
        {"Icom LAN",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
#if defined HAVE_HAMLIB
        //IC8TEM Icom radios through the Hamlib library (Server = COM port, Port = baud rate)
        {"Icom IC-7300 (Hamlib)",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"Icom IC-7610 (Hamlib)",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
        {"Icom IC-9700 (Hamlib)",RIG_PTT_RIG,RIG_PORT_NETWORK,4800,4800,8,1,PAR_NONE,FLOW_OFF,0,0,200,3},
#endif        	     	        						
    };
#endif
#endif
