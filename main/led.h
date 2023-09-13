

extern uint16_t color1[3];
extern uint16_t color2[6];
extern uint16_t color3[3];

void pwm_init();
void set_color(uint16_t rgb[], uint16_t c0=0, uint16_t c1=0, uint16_t c2=0, uint16_t c3=0, uint16_t c4=0, uint16_t c5=0);
void rgb_color(uint16_t, uint16_t, uint16_t);

void flash_master(bool, unsigned long, bool, uint8_t fade_rate = 80);
void flash_master_BS(uint16_t);

void reset_period();