#include <stdint.h> //developer.mbed.org/handbook/C-Data-Types
#include "globals.h"
#include "calculations.h"

int itbLoadTpsSwitchPoint = 0;
int itbLoadAtTpsSwitchPoint = 0;

int16_t getLoad(int algorithm) 
{
    if (algorithm == LOAD_SOURCE_MAP) //Check which fueling algorithm is being used
  {
    //Speed Density
    return currentStatus.MAP;
  }
  else if (algorithm == LOAD_SOURCE_TPS)
  {
    //Alpha-N
    return currentStatus.TPS;
  }
  else if (algorithm == LOAD_SOURCE_IMAPEMAP)
  {
    //IMAP / EMAP
    return (currentStatus.MAP * 100) / currentStatus.EMAP;
  }
  else if (algorithm == LOAD_SOURCE_ITBLOAD) {   

    itbLoadTpsSwitchPoint = table2D_getValue(&itbTpsTable, currentStatus.RPM); //TPS value at MAP switchpoint for AN mode
    itbLoadAtTpsSwitchPoint = table2D_getValue(&itbLoadTable, currentStatus.RPM); //devision line for the ve map defining SD and AN areas
    uint16_t load = 0;
    
    //AN mode. Uses the area of the map above the itbLoadAtTpsSwitchpoint line
    if (currentStatus.TPS >= configPage15.itbIdleTpsThreshold && currentStatus.MAP > configPage15.itbBaroSwitchPoint) {                
        //rescale to fully use available tps range (above itbLoadTpsSwitchPoint) and all load bins assigned to AN operation
        load = (currentStatus.TPS - itbLoadTpsSwitchPoint) * (100 - itbLoadAtTpsSwitchPoint);
        load = load / (100 - itbLoadTpsSwitchPoint);
        return load + itbLoadAtTpsSwitchPoint;
    } 
    //SD mode. Uses the area of the map below the itbLoadAtTpsSwitchpoint line
    else {
        //rescale to fully use available MAP range and all load bins assigned to SD operation
        load = currentStatus.MAP * itbLoadAtTpsSwitchPoint;
        return load / configPage15.itbBaroSwitchPoint;
    }
    //also add crc routines for new page with maps
    return currentStatus.MAP; // placeholder
  }
  else {return currentStatus.MAP; } //Fallback to Speed Density
}