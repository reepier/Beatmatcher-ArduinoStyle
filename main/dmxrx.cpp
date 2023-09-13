#include "debug.h"
#ifndef PRINTDEBUG  // in debug mode, this file is useless (therefore empty)

  #include <Arduino.h>
  #include <DMXSerial.h>
  #include "constant.h"
  #include "dmxrx.h"
  #include "led.h"

  // DMX Settings & variables
  uint8_t dmx_data[NCh];
  uint8_t dmx_last_data[NCh];
  bool DMX_MODE;
  bool DMX_new_data = false;



  bool init_dmx(){
    DMXSerial.init(DMXReceiver);
  }

  void read_dmx(){
    bool new_data = false;
    for (int i=0; i<NCh; i++)
    {
      uint8_t temp = DMXSerial.read(Ch0+i); 
      if (temp != dmx_data[i]){
          new_data = true;
      }
      else{
        new_data = (false || new_data);
      }
      dmx_data[i] = temp;
    }

    // if data is new !!!!
    if (new_data){
      digitalWrite(led_beat, HIGH);
    }
    else{
      digitalWrite(led_beat, LOW);
    }
  }

  void display_dmx(){
    
    /*
    analogWrite(redpin, dmx_data[Rch]);
    analogWrite(greenpin, dmx_data[Gch]);
    analogWrite(bluepin, dmx_data[Bch]);
    */
  //
    float M = dmx_data[Mch]/255.0;
    uint16_t red    = M * ((dmx_data[Rch]<<4) + (dmx_data[Rchf]>>4));
    uint16_t green  = M * ((dmx_data[Gch]<<4) + (dmx_data[Gchf]>>4));
    uint16_t blue   = M * ((dmx_data[Bch]<<4) + (dmx_data[Bchf]>>4));

    rgb_color(red, green, blue);
    /*
    PWMmodule.setChannelPWM(ROUT, red);
    PWMmodule.setChannelPWM(GOUT, green);
    PWMmodule.setChannelPWM(BOUT, blue);
    */
  }

  bool check_dmx_cnx(){
      DMX_MODE = (DMXSerial.noDataSince() > 1000) ? false:true;    
      return DMX_MODE;
  }

  bool read_dmx_mode(){
      return DMX_MODE;
  }

  void wait_for_new_frame(){
    while(!DMXSerial.dataUpdated()){
      delayMicroseconds(100);
    }
    DMXSerial.resetUpdated();
  }

#endif // !PRINTDEBUG