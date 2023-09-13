#include "debug.h"

#include "constant.h"
#ifndef PRINTDEBUG
  #include "dmxrx.h"
#endif // !PRINTDEBUG
#include "led.h"
#include "music.h"

// Counters
  uint8_t animation_i = 1;  //do not initialize to ZERO !!!!!!
  
// Timer variables
  unsigned long last_change_anim = millis();

// Flags
  boolean flash = true;
  boolean animation_change = true;
 
void setup() {
  pwm_init();

  pinMode(led_BT, OUTPUT);
  digitalWrite(led_BT, LOW);
  pinMode(led_BL, OUTPUT);
  digitalWrite(led_BL, LOW);
  pinMode(led_BK, OUTPUT);
  digitalWrite(led_BK, LOW);
  pinMode(led_clip, OUTPUT);
  digitalWrite(led_clip, LOW);
  pinMode(led_beat, OUTPUT);
  digitalWrite(led_beat, LOW);

  // intialize DMX receiver
  #ifndef PRINTDEBUG
    init_dmx();
  #else
    Serial.begin(2000000);
  #endif
}

bool first_loop = true;
void loop(){
  
  
  // switch mode depending on availability Serial (DMX) connexion state !
  #ifndef PRINTDEBUG
  if (!check_dmx_cnx()){
  #endif
  // Use internal Beat Detector Program (default)
    //----------------------------------------------
    // MUSIC RECORDING
    //Sampling loop --> period : sampling_period
    
    // digitalWrite(led_beat, HIGH); // debug
    #ifndef FAKEMUSIC
      beat_analyzer.record();

    //---------------------------------------------------------------------
    // SAMPLE PROCESSING
      beat_analyzer.process_record();

    #else
      beat_analyzer.fake_analysis();
    #endif // !FAKEMUSIC
    // digitalWrite(led_beat, LOW); // debug


    // --------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------
    // ANIMATOR
    // --------------------------------------------------------------------------------------------------------
    // Animation selector
    /* updates the current animation index (animation_i) whenever <state_change> is TRUE.
     * Animation_i is of type uint8_t, it has 255 possible values (excessive)
     */
      uint8_t n_animation = 13;
      
      if (beat_analyzer.state_change & (millis()-last_change_anim > TEMPO_ANIM_CHANGE)){
        animation_change = true;
        last_change_anim = millis();
        animation_i = (animation_i+1) % n_animation;
        reset_period();
        // call set_colors here
      }
      else{
        animation_change=false;
      }
    
      if (beat_analyzer.state_change){
        if (beat_analyzer.system_state == 1){
          flash = true;
          beat_analyzer.beat_tracking_start = millis();
        }
        else{
          flash = false; //flash only if beat tracking is OK
        }
      }
      
      if (beat_analyzer.system_state == 1 && (millis()-beat_analyzer.beat_tracking_start) > MAX_CONT_FLASH){ // --> do not flash for more than 120s (eye confort)
        flash = false;
      }
      
        // move this block to a "set_colors" function in led.cpp
        // call set_colors() right after reset_period()
        switch (animation_i){
          case 0:
            // Warm Flashes
            set_color(color1, 255<<4, 150<<4, 80<<4);   // flash RGBor
            set_color(color2, 30<<4,100<<4, 1<<4,7<<4, 0<<4,0<<4);  // back color wave settings (Rmax, Rmin, Gmax,...)
          break;
          case 1:
            // Sodium Flashes
            set_color(color1, 255<<4, 50<<4, 10<<4);
            set_color(color2, 15<<4,50<<4, 0<<4,5<<4, 0<<4,0<<4);      
          break;
          case 2:
          // Very Warm
            set_color(color1, 255<<4, 150<<4, 80<<4);
            set_color(color2, 30<<4,100<<4, 1<<4,7<<4, 0<<4,0<<4);      
          break;
          case 3:
            //BLACK BACKGROUND RED flashes
            set_color(color1, 255<<4, 0<<4, 0<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);
          break;
          case 4:
            // Slide to pink palette introducing blue
            set_color(color1, 255<<4, 150<<4, 80<<4);
            set_color(color2, 20<<4,70<<4, 0<<4, 0<<4, 0<<4,5<<4);
          break;
          case 5:
            //BLACK BACKGROUND PINK flashes
            set_color(color1, 0<<4, 0<<4, 255<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4); 
          break;
          case 6:
            // reduce red, increase blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,80<<4, 0<<4,0<<4, 0<<4,20<<4);
          break;
          case 7:
            // reduce red, increase blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,30<<4, 0<<4,0<<4, 0<<4,50<<4);     
          break;
          case 8:
            // Mostly blue
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,20<<4, 0<<4, 0<<4, 0<<4,100<<4);      
          break;
          case 9:
            //BLACK BACKGROUND BLUE flashes
            set_color(color1, 0<<4, 0<<4, 255<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);
          break;
          case 10:
          // Introduce green to slide towards cyan - yellow palette
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,10<<4, 0<<4,10<<4, 0<<4,100<<4);  
          break;
          case 11:
            // Sodium Flashes
            set_color(color1, 255<<4, 30<<4, 0<<4);
            set_color(color2, 5<<4,20<<4, 0<<4,2<<4, 0<<4,0<<4);
          break;
          case 12:
            //BLACK BACKGROUND with WHITE FLASHES
            set_color(color1, 255<<4, 100<<4, 80<<4);
            set_color(color2, 0<<4,0<<4, 0<<4,0<<4, 0<<4,0<<4);
          break;
        }

      if (beat_analyzer.system_state != 3){
        flash_master(beat_analyzer.new_beat, beat_analyzer.last_new_beat, flash, 60);
      }
      else{
        set_color(color1, 255<<4,0<<4,0<<4);
        flash_master_BS(beat_analyzer.volume);
      }
  #ifndef PRINTDEBUG
    }else{   
    // Use DMX Receiver
    read_dmx();
    display_dmx();
    // Digitalwrite something to monitor loop duration !!
    //digitalWrite(led_beat, HIGH);
    delay(1);  // use nodatasince() instead of delay() to wait until a new frame is received
    digitalWrite(led_beat, LOW);
    }
  #endif
  
  state_display();
  #ifdef PRINTDEBUG
    debug();
  #endif
  


  if (first_loop)
    first_loop = false;
}
// ---------------------------------------------------
// END MAIN()
// ---------------------------------------------------

void state_display(){
  #ifndef PRINTDEBUG
  if (read_dmx_mode()){
    digitalWrite(led_BT, HIGH);
    digitalWrite(led_BK, LOW);
    digitalWrite(led_BL, HIGH);
    digitalWrite(led_clip, LOW);
    digitalWrite(led_beat, LOW);
  }else{
  #endif 
  
    switch(beat_analyzer.system_state){
      case 1:
        digitalWrite(led_BT, HIGH);
        digitalWrite(led_BK, LOW);
        digitalWrite(led_BL, LOW);
      break;
      case 2:
        digitalWrite(led_BT, LOW);
        digitalWrite(led_BK, HIGH);
        digitalWrite(led_BL, LOW);
      break;
      case 3:
        digitalWrite(led_BT, LOW);
        digitalWrite(led_BK, LOW);
        digitalWrite(led_BL, HIGH);
      break;
    }
  #ifndef PRINTDEBUG
  }
  #endif // !PRINTDEBUG

  // toggle blue led if clipping
      if (beat_analyzer.clip)
        digitalWrite(led_clip, HIGH);
      else
        digitalWrite(led_clip, LOW);

  // toggle white led with the beat
      if (beat_analyzer.new_beat)
        digitalWrite(led_beat, HIGH);
      else
        digitalWrite(led_beat, LOW);
}

#ifdef PRINTDEBUG
  void debug(){
  float mult = 0.1;

//    Serial.print(beat_analyzer.last_new_beat);   
//    Serial.print(" ");
//    Serial.print(m_loop_period);   
//    Serial.print(" ");
//    Serial.print(v_mem_i);   
//    Serial.print(" ");
//    Serial.print(10*beat_analyzer.enable_analysis);   
//    Serial.print(" ");
    // Serial.print(mult*beat_analyzer.beat_thresh);   
    // Serial.print(" ");
//    Serial.print(loop_period);   
//    Serial.print(" ");
//    Serial.print(m_loop_period);   
//    Serial.print(" ");
//    Serial.print(beat_analyzer.state_change);   
//    Serial.print(" ");
    // Serial.print(animation_change);   
    // Serial.print(" ");
    Serial.print(animation_i);
    Serial.print(" ");
  //  Serial.print(beat_analyzer.system_state);   
  //  Serial.print(" ");
//    Serial.print(beat_analyzer.beat);   
//    Serial.print(" ");
//    Serial.print(3*new_beat);   
//    Serial.print(" ");
//    Serial.print(filtered_beat);   
//    Serial.print(" ");
//    Serial.print(3*new_filtered_beat);   
//    Serial.print(" ");
//    Serial.print(3*flash);   
//    Serial.print(" ");
//    Serial.print(mult*v_max);   
//    Serial.print(" ");
//    Serial.print(mult*v_XX);   
//    Serial.print(" ");
//    Serial.print(mult*v_95);   
//    Serial.print(" ");
//    Serial.print(mult*v_quarter[0]);
//    Serial.print(" ");
//    Serial.print(mult*v_quarter[1]);
//    Serial.print(" ");
//    Serial.print(mult*v_quarter[2]);   
//    Serial.print(" ");
//    Serial.print(beat_analyzer.ratio_95_q1);
//    Serial.print(" ");
//    Serial.print(thresh_bs_to_bt);
//    Serial.print(" ");   
//    Serial.print(THD_BTtoBS);
//    Serial.print(" ");  
//    Serial.print(mult*THD_toBK);
//    Serial.print(" ");  
//    Serial.print(ratio_95_2);   
//    Serial.print(" ");
//    Serial.print(ratio_95_3);   
//    Serial.print(" ");
//    Serial.print(millis()-last_change_anim);
//    Serial.print(" ");
//    Serial.print(analogRead(musicpin));
//    Serial.print(" ");
    // Serial.println(mult*beat_analyzer.volume);
   Serial.println(0);
}

#endif // DEBUG
