#ifndef AUDIO_H 
#define AUDIO_H 

void config_timer(void);
void __attribute__((__interrupt__)) _T1Interrupt(void);
void __attribute__((__interrupt__)) _T2Interrupt(void);

#endif