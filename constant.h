#ifndef CONSTANT_H
#define CONSTANT_H
// HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-

// Arduino pins (Hardware dependant, do not change) ----------------
// system state led output
#define led_BT 3   // BT = Beat_tracking
#define led_BK 5   // BK = Break
#define led_BL 6   // BL = Beat Lost
#define led_clip 4 // Led Clip (bleue)
#define led_beat 7 // Led Beat detection (white)
// ledstrip output
    //#define redpin 11       // LED output RED     REPLACED WITH I2C PWM 12bits Module
    //#define greenpin 10     // LED output GREEN
    //#define bluepin 9       // LED output BLUE
#define ROUT 4
#define GOUT 5
#define BOUT 6
// music input
#define musicpin A0


// -- CONFIG ------------------------------------------------------------
#define     MAX_CONT_FLASH      100000          // 120000
#define     TEMPO_ANIM_CHANGE   240000          // 240000
#define     MASTERDIMMER        1.0             //fraction --> must be decimal (literal float) !!!


// HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-HEADER-

#endif // !CONSTANT_H