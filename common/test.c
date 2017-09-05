//#include "roland.h"
#include "adsr.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pink_noise.h"
#ifndef SAMPLE_T
#define SAMPLE_T double
typedef double sample_t;
#endif

inline sample_t midi2freq(sample_t note)
{
    return 440.0 * pow( 2.0, (note - 69.0) / 12.0 );
}


inline int freq2midi(sample_t f)
{
    return round(12*log2(f/440.0f))+57;
}
void main()
{
    int i;
    int m;
    float j,min,max,v,u;
    const sample_t detune[32] = { 0.0f,-0.01952356f,+0.01952356f,-0.06288439f,+0.06288439f,-0.11002313f,+0.11002313f
                    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 
                        };
    int note= 69;
    int end = 72;
    
    sample_t glide = 1.0f * 48000.0f;
    sample_t freq = 0.0f;
    sample_t factor = 0.0f;
    sample_t ratio = 0.0f;
    sample_t start_freq = midi2freq((sample_t)note);
    sample_t target_freq = midi2freq((sample_t)end);
    
    //a.output = start_freq;
    freq = 69.0f;
    ratio = (72.0f / 69.0f);
    factor = pow( ratio, 1.0f / (sample_t)( 100 ));
    min = 0.0f;max=10.0f;
    
    PinkNoise* pink = malloc(sizeof(PinkNoise));
    
    InitializePinkNoise(pink,12);
    for (j=1;j<40;j++) {
        
        printf("%f\n",GeneratePinkNoise(pink));   
    }
    
    /*
    for (j=20;j<40;j+=0.1) {
        u = (j-20.0f)/(max-min);
        v = powf(u,4.0f);
        printf("%f\n",v);   
        
        //if (i=400) adsr_gate(&a,0);
    }*/
    /*
    for (j=0;j<10;j+=0.1) {
        
        u = (j-10.0f)/(max-min);
        u = powf(u,0.25f);
        v = (max-min)*(u)+20.0f;
        printf("%f\n",v);   
    }
    */
    /*
    for (m =0; m < 128; m+=1)
    {
        j = m / 127.0f;
        printf("\n");
        for (i =0; i < 7; ++i)
        {
            printf("%f\n",midi2freq(72.0f)+ midi2freq(72.0f)*(rolandDetune(j))*(detune[i]));
        }
Let:
ratio = new_frequency / previous_frequency

We know that we want:
factor^samples = ratio

thus, factor = ratio^(1/samples)

In C, that's:
factor = pow( ratio, 1.0/samples )
You can spot-check the logic, say prev_freq = 100, new_freq = 200,
so ratio = 2 and let samples = 3:

    }*/
    //note = freq2midi((sample_t)freq);
    /*
    glide = 100;
    start_freq = 100.0f, target_freq = 2000.0f;
    ratio = (target_freq / start_freq);
    for (i=glide; i >=0 ; i-=1)
    {
         //ratio = (target_freq / start_freq);
         factor = pow( ratio, 1.0f / (sample_t)( glide ));
         start_freq = start_freq * factor;
         
         
         //start_freq = freq;
         //printf("%f %f\n",factor,start_freq-.025726225);
    }
    for (i=0; i<16; i++) {
         printf("%f\n",(double)0.045249785f*i - (double)0.025726225f);
    }
    */
}
