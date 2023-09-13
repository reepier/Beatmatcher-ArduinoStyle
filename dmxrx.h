#ifndef DMX_H
#define DMX_H

// DMX Settings & variables ----------------------------------------
#define NCh 10  // number of DMX channel allocated to the device
#define Ch0 1   // First channel allocated to the device
// channel definition
#define Mch 0   // master Dimmer
#define Rch 1   // coarse red
#define Gch 2   // coarse green
#define Bch 3   // coarse blue
#define Rchf 4   // coarse red
#define Gchf 5   // coarse green
#define Bchf 6   // coarse blue

// DMX Settings & variables
extern uint8_t dmx_data[NCh];
extern bool DMX_MODE;

bool init_dmx();
void read_dmx();
void display_dmx();
bool check_dmx_cnx();
bool read_dmx_mode();

#endif // !DMX_H