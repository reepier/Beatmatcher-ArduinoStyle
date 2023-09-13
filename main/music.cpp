#include "debug.h"
#include "music.h"
#include "constant.h"

SoundAnalyzer beat_analyzer;
extern boolean beat;

void SoundAnalyzer::record(){
    unsigned long next = micros();
      clip = false;
      for (int i=0; i<SAMP_N; i++){
        next = next + 1000000.0/SAMP_F;
        record_real[i] = analogRead(musicpin);
        record_imag[i] = 0;
        if (record_real[i] > (ADC_MAX-10) || record_real[i] < (ADC_MIN+10)){
          clip = true;
        }
        while(micros() < next){}    // wait until the end of the samplign period
      }
}

void SoundAnalyzer::process_record(){
    FFT.DCRemoval(record_real, SAMP_N);
    //FFT.Windowing(value_r, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(record_real, record_imag, SAMP_N, FFT_FORWARD);
    FFT.ComplexToMagnitude(record_real, record_imag, SAMP_N);
      
    // extract current volume (measure in FFT frequency band "FREQ_BAND")
    volume = record_real[FREQ_BAND]*2/SAMP_N;
    
    // save the current volume sample in moving memory
    // when the memory is full, overwrite the first records.
    v_memory[i_mem] = volume;

    i_mem++;
    if (i_mem == PHRASE_LEN){   // wrap i_mem within [0;128[
        i_mem = 0;
        // toggle tab_is_full when memory is full (this will only happen once, at loop's 128th run)
        if (!tab_is_full){     
            tab_is_full = true;
        }
    }

    // check for beat 
    // Compare current level to threshold and control LED 
      if (volume >= beat_thresh){
        if (!beat){
          new_beat = true;
          last_new_beat = millis();
        }
        else{
          new_beat = false;
        }
        
        beat = true;
        last_beat = millis();
      }
      else{
        beat = false;
        filtered_beat = false;
        new_beat = false;
      }

    // activate phrase analyis enable flag every 32 samples (PHRASE_LEN/4) 
    if (tab_is_full && (i_mem%(PHRASE_LEN/4) == 0))
        enable_analysis = true;
    else
        enable_analysis = false;

    // if phrase analysis enable flag is TRUE, analyze & compute statistics
    if (enable_analysis){
        copy_memory();      // copy v_memory
        KickSort<int>::quickSort(v_memory_sorted, PHRASE_LEN);  // sort volume record by value (ascending) for statistical analysis
        compute_stats();    // compute the statistics
      }

    // in any case, update system state
    update_state();
    update_beat_threshold();
}

void SoundAnalyzer::update_state(){
    // update system state 
    switch (system_state){     
        // If Beat Tracking
        case 1:
            if (enable_analysis){
                // If volume drops below threshold
                if (v_95 < THD_toBK){
                system_state = 2; // go to BREAK
                }
                else if (ratio_95_q1 < THD_BTtoBS){
                BS_cpt += 1;
                if (BS_cpt == 4){
                    system_state = 3;
                    BS_cpt = 0;
                }
                }
                else{BS_cpt=0;}
            }
            break;

            // If Break
            case 2:
                if(beat){
                    system_state = 1;
                }
            break;

            // If Beat Lost
            case 3:
                if (enable_analysis){
                    if (v_95 < THD_toBK){
                        system_state = 2;
                    }
                }
        default:
            break;
        }

        // update state change flag
        if (system_state != old_sys_state){
            state_change = true;
            old_sys_state = system_state;
        }
        else{
            state_change = false;
        }
}

void SoundAnalyzer::update_beat_threshold(){
    if (enable_analysis == 1 && system_state != 2){
          beat_thresh = v_XX*(float)1/10 + beat_thresh*(float)9/10;
        }
}

void SoundAnalyzer::copy_memory(){
    for (int i=0;i<PHRASE_LEN;i++)
        v_memory_sorted[i] = v_memory[i];
}

void SoundAnalyzer::compute_stats(){
  unsigned long sum=0;
  for (int i=0; i<PHRASE_LEN; i++){
    sum = sum + v_memory_sorted[i];
  }
  v_mean = sum/PHRASE_LEN;
  
  v_max = v_memory_sorted[PHRASE_LEN-1];
  v_quarter[0] = v_memory_sorted[PHRASE_LEN/4];
  v_quarter[1] = v_memory_sorted[PHRASE_LEN/2];
  v_quarter[2] = v_memory_sorted[3*PHRASE_LEN/4];
  v_95 = v_memory_sorted[95*PHRASE_LEN/100];
  
  v_XX = v_memory_sorted[90*PHRASE_LEN/100];
  
  if (v_quarter[0]>0){ratio_95_q1 = (float)v_95/v_quarter[0];}
  else {ratio_95_q1 = 1;}
  
  if (v_quarter[1]>0){ratio_95_q2 = (float)v_95/v_quarter[1];}
  else {ratio_95_q2 = 1;}
  
  if (v_quarter[2]>0){ratio_95_q3 = (float)v_95/v_quarter[2];}
  else {ratio_95_q3 = 1;}

}


#ifdef FAKEMUSIC

#define BPM 140          // BPM
#define BREAKDuration 16  // beats
#define DROPDuration  32 // beats

const int beat_T_ms = 60000/BPM;
const int break_T_ms = BREAKDuration * beat_T_ms;
const int drop_T_ms = DROPDuration * beat_T_ms;

unsigned long t = 0;
unsigned long n_break = t + drop_T_ms, n_drop, n_beat = 0;
void SoundAnalyzer::fake_analysis(){
  unsigned long fake_t = millis();
  
  // update fake state
  switch (system_state)
  {
  case 1:
    if (fake_t > n_break){
      state_change = true;
      system_state = 2; 
      n_drop = n_break + break_T_ms;
    }
    else{
      state_change = false;
    }
    break;
  case 2:
    if (fake_t > n_drop){
      state_change = true;
      system_state = 1;
      n_break = n_drop + drop_T_ms;
    }
    else{
      state_change = false;
    }
    break;
  default:
    break;
  }

  // generate fake beat
    if (fake_t>n_beat){
      beat = true;
      new_beat = true;
      last_beat = fake_t;
      last_new_beat = fake_t;

      n_beat += beat_T_ms;
    }
    else{
      beat = false;
      new_beat = false;
    }

  delay(30);

}

#endif // FAKEMUSIC
