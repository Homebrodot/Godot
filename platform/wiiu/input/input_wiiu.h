#include "core/input/input.h"
#include <vpad/input.h>
#include <padscore/kpad.h>

//Note: Touch input is *usually* handled by DisplayServer, not the input singleton,
//which might cause issues with multi-DRC and touch inputs, idk.

//This class handles input from both the gamepad and any connected Wiimotes or other controllers.
//However, the backing code 

class InputWiiU {
public:
    InputWiiU();
    ~InputWiiU();

    DRC vpads[1];
    Wiimote kpads[3];

    struct DRC {
        //Pointer to a DisplayServer exclusive to that gamepad,
        //so that multiple unique displays are possible

        VPadStatus drcinputs;
        //Whether the DRC is actually connected/in use
        bool active;
    }

    struct Wiimote {
        KPADStatus basebuttons;
        //Whether the Wiimote is actually active
        bool active;
        
    }

    void process_vpad();
    void process_kpad();

    void process_inputs();
    void process_joypads(){
        process_vpad();
        process_kpad();
    };

    void setup_kpad();
    void setup_vpad();

    Input *input;
};