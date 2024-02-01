#include "servers/display_server.h" 

#include <vpad/input.h>


//Wii U has various TV resolutions ranging from 480i to 1080p. 
class WiiUTVDisplayServer: public DisplayServer {
    has_feature(Feature feature);
}

//DRC is 480p, maybe I could support 720p cause I heard 720p gamepad streams
//are a thing, and this class will also handle button inputs from the DRC because 
//of how the VPAD library works
class WiiUDRCDisplayServer: public DisplayServer {

private:

    VPADStatus drc_state;
    VPADChan drc_chan = VPAD_CHAN_0;
    VPADReadError drc_err;

public:

    bool has_feature(Feature feature);

    void update_drc_inputs(); //call this in main loop to update VPADStatus

    //Nintendoland uses it vertically, so...
    void screen_set_orientation(ScreenOrientation p_orientation, int p_screen);
}