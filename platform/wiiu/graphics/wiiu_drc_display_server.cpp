#include "wiiu_display_server.h"
#include <vpad/input.h>

void WiiUDRCDisplayServer::has_feature(Feature feature){
    return false; //Placeholder; it should support touch screen, etc.
}

void WiiUDRCDisplayServer::update_drc_inputs(){
    VPADRead(drc_chan,drc_state, 1, *drc_err);
    
    /*
    Check drc_err for errors



    Pass off controller inputs to Input, idk how yet

    Orphan values to pass to "unrelated" places:
    * battery: ????
    * slideVolume, usingHeadphones: Audio output (coming soon:tm:)
    * micStatus: Audio input (coming soon:tm:)
    
    VPAD has several layers of touch validity checks it provides to get, well, accurate 
    touch results. 

}