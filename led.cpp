#include <PCA9685.h>
#include "constant.h"

PCA9685 PWMModule;

uint16_t color1[3];
uint16_t color2[6];
uint16_t color3[3];
uint16_t back_color[3];
float coef = 0;

unsigned long period[6];
#define MAX_T 30
#define MIN_T 2

void set_color(uint16_t rgb[], uint16_t c0=0, uint16_t c1=0, uint16_t c2=0, uint16_t c3=0, uint16_t c4=0, uint16_t c5=0){
  rgb[0]=c0;
  rgb[1]=c1;
  rgb[2]=c2;
  rgb[3]=c3;
  rgb[4]=c4;
  rgb[5]=c5;
}

void rgb_color(uint16_t r, uint16_t g, uint16_t b){
    // create a library for color display, with a wrapper (similar to this function)
    // that hides all the I2C stuff...
    /*
    analogWrite(redpin, r);
    analogWrite(greenpin, g);
    analogWrite(bluepin, b);
    */
    /*
    Serial.print(r);
    Serial.print(" ");
    Serial.print(g);
    Serial.print(" ");
    Serial.println(b);
    */

    PWMModule.setChannelPWM(ROUT, (int)(MASTERDIMMER*r));
    PWMModule.setChannelPWM(GOUT, (int)(MASTERDIMMER*g));
    PWMModule.setChannelPWM(BOUT, (int)(MASTERDIMMER*b));
}

void flash_master(bool beat, unsigned long beat_t, bool flash, uint8_t fade_rate = 80){
  unsigned long t = millis();

  // color2[] stores [redmin, redmax, greenmin, green max...]
  back_color[0] = (color2[0]+color2[1])/2 
                  + (color2[1]-color2[0])/4
                          *(sin(2*3.14*t/period[0])+sin(2*3.14*t/period[1]));
  back_color[1] = (color2[2]+color2[3])/2 
                  + (color2[3]-color2[2])/4
                          *(sin(2*3.14*t/period[2])-sin(2*3.14*t/period[3]));
  back_color[2] = (color2[4]+color2[5])/2 
                  + (color2[5]-color2[4])/4
                          *(sin(2*3.14*t/period[4])-sin(2*3.14*t/period[5]));
                               
  if (beat && flash){
    rgb_color(color1[1], color1[2], color1[2]);
  }
  else{
    if (flash){
      coef = exp(-(float)(t-beat_t)/fade_rate);
    }
    else{
      coef = 0;  
    }
    rgb_color(back_color[0]+coef*(color1[0]-back_color[0]), back_color[1]+coef*(color1[1]-back_color[1]), back_color[2]+coef*(color1[2]-back_color[2]));
  }
}

void flash_master_BS(uint16_t vol){
  // secondary flashing mode
  // the LED display the color contained in color1 array but with an intensity following the volume
  float intensity = sq((float)map(vol, 0, 180, 5, 255)/255);
  
  rgb_color(intensity*(float)color1[0], intensity*(float)color1[1], intensity*(float)color1[2]);
}

void reset_period(){
  randomSeed(analogRead(musicpin));

  period[0] = 1000*random(MIN_T, MAX_T);
  period[1] = 1000*random(MIN_T, MAX_T);
  period[2] = 1000*random(MIN_T, MAX_T);
  period[3] = 1000*random(MIN_T, MAX_T);
  period[4] = 1000*random(MIN_T, MAX_T);
  period[5] = 1000*random(MIN_T, MAX_T); 
}

void animator(uint8_t i){
        switch (i){
          case 0:
            // Warm Flashes
            set_color(color1, 255<<4, 150<<4, 80<<4);   // flash RGB color
            set_color(color2, 30<<4,100<<4, 1<<4,7<<4, 0<<4,0<<4);  // back color wave settings (Rmax, Rmin, Gmax,...)
          break;
          case 1:
            // Sodium Flashes
            set_color(color1, 255<<4, 30<<4, 0<<4);
            set_color(color2, 15<<4,50<<4, 0<<4,5<<4, 0<<4,0<<4);
          break;
          case 2:
            // Sodium Flashes
            set_color(color1, 255<<4, 50<<4, 10<<4);
            set_color(color2, 15<<4,50<<4, 0<<4,5<<4, 0<<4,0<<4);      
          break;
          case 3:
          // Very Warm
            set_color(color1, 255<<4, 150<<4, 80<<4);
            set_color(color2, 30<<4,100<<4, 1<<4,7<<4, 0<<4,0<<4);      
          break;
          case 4:
            // Slide to pink palette introducing blue
            set_color(color1, 255<<4, 150<<4, 80<<4);
            set_color(color2, 30<<4,100<<4, 1<<4,7<<4, 0<<4,10<<4);   
          break;
          case 5:
            // reduce red, increase blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,80<<4, 0<<4,0<<4, 0<<4,20<<4);
          break;
          case 6:
            // reduce red, increase blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,50<<4, 0<<4,0<<4, 0<<4,80<<4);     
          break;
          case 7:
            // Mostly blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,20<<4, 0<<4,5<<4, 0<<4,100<<4);      
          break;
          case 8:
            // Introduce green to slide towards cyan - yellow palette
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,10<<4, 0<<4,10<<4, 0<<4,100<<4);    
          break;
          case 9:
            //BLACK BACKGROUND with WHITE FLASHES
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);     
          break;
          case 10:
            // pure pink flashes
            set_color(color1, 255<<4, 0<<4, 25<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);
          break;
          case 11:
            // pure blue flashes
            set_color(color1, 0<<4, 0<<4, 255<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);
          break;
          case 12:
            // pure red flashes
            set_color(color1, 255<<4, 0<<4, 0<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);     

          break;
        }
}

void pwm_init(){
    Wire.begin();
    PWMModule.resetDevices();
    PWMModule.init();
    PWMModule.setPWMFrequency(1526);
    reset_period();
}