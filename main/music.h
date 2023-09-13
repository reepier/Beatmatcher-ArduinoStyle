#include <KickSort.h>
#include <arduinoFFT.h>

#define SAMP_N 32      // record length (number of individual samples)
#define SAMP_F 2000     // sampling frequency
#define FREQ_BAND 2     // frequency band used to measure volume

#define PHRASE_LEN 128   // number of volume samples in 1 phrase

#define ADC_MAX 1023
#define ADC_MIN 0

// thresholds
#define THD_toBK 45
#define THD_BTtoBS 3
#define THD_BStoBT 5

class SoundAnalyzer{
            
    // record storage
    private :   double record_real[SAMP_N];
    private :   double record_imag[SAMP_N];
    public :    bool clip; // true if record() detects ADC saturation
    public :    int volume;
    // FFT library
    private :   arduinoFFT FFT = arduinoFFT();
    // phrase storage
    private :   int v_memory[PHRASE_LEN];
    private :   int v_memory_sorted[PHRASE_LEN];
    // phrase analysis variables
    private :   uint8_t i_mem = 0;
    private :   bool tab_is_full;
    public :    bool enable_analysis;
    public :    float v_max, v_mean, v_quarter[3], v_95, v_XX;
    public :    float ratio_95_q1, ratio_95_q2, ratio_95_q3;

    // Beat tracking variables
    public :    float beat_thresh = 60;
    public :    bool beat=false, new_beat=false, filtered_beat=false;
    public :    unsigned long last_beat = 0;         // Stores timestamp of the last beat=true event
    public :    unsigned long last_new_beat = 0;
    public :    unsigned long beat_tracking_start = 0;

    // state machine variables
    private :   uint8_t BS_cpt = 0, BT_cpt = 0;
    public :    uint8_t system_state = 1, old_sys_state = 1;
    public :    bool state_change = false;    

    // Main functions
    public :    void record();          
    public :    void process_record();
    // hidden functions
    private :   void copy_memory();
    private :   void compute_stats();
    private :   void update_beat_threshold();
    private :   void update_state();

    // fake analysis (for animation developpement purpose)
    public : void fake_analysis(); 
};

extern SoundAnalyzer beat_analyzer;