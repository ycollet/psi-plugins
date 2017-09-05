/**
 * @file
 * @author  Brendan Jones <brendan.jones.it@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 * 
 * Copyright 2012 Nigel Redmon
 * Copyright 2014, Brendan Jones
 * 
 * This source code is provided as is, without warranty.
 * You may copy and distribute verbatim copies of b document.
 * You may modify and use b source code to create binary code for your own purposes, free or commercial.
 * 
 * @section DESCRIPTION
 * Based on the C++ class created by Nigel Redmon on 5/15/12
 * EarLevel Engineering: earlevel.com
 * 
 *
 * For a complete explanation of the wavetable oscillator and code,
 * read the series of articles by the author, starting here:
 * www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
 *
 */


#ifndef BL_OSC_H
#define BL_OSC_H

#include "types.h"
#include "osc_common.h"

#define doLinearInterp 1

#define numWaveTableSlots 32

// one band
typedef struct {
    sample_t topFreq;
    int waveTableLen;
    float *waveTable;
} waveTable;

// one for each band
typedef struct {
    int shape;
    int numWaveTables;
    waveTable waveTables[numWaveTableSlots];
} waveTableShape;

// the oscillator instance
typedef struct {
    sample_t  phasor;      // phase accumulator
    sample_t  freq;        // phase increment / freq
    sample_t  phaseOfs;    // phase offset for PWM
    int       shape;
} BL_Osc;

inline
void BL_Osc_init(BL_Osc* b);

inline 
void bl_freq(BL_Osc* b, sample_t inc);

inline 
void bl_phase_offset(BL_Osc* b, sample_t offset);

inline 
void bl_update_phase(BL_Osc* b);

float bl_output(BL_Osc* b);

float bl_output_minus_offset(BL_Osc* b);

int bl_add_wave_table(int shape, int len, float *waveTableIn, sample_t topFreq);

void bl_fill_tables(int shape, sample_t *freqWaveRe, sample_t *freqWaveIm, int numSamples);

void bl_fft(int N, sample_t *ar, sample_t *ai);

float bl_make_WaveTable(int shp, int len, sample_t *ar, sample_t *ai, sample_t scale, sample_t topFreq);

void bl_saw_osc(long tableLen);

void bl_wave_osc(int shape, float *waveSamples, int tableLen);

inline
void bl_freq(BL_Osc* b, sample_t inc) {
    b->freq = inc;
}

inline
void bl_phase_offset(BL_Osc* b, sample_t offset) {
    b->phaseOfs = offset;
}

inline
void bl_update_phase(BL_Osc* b) {
    b->phasor += b->freq;
    
    if (b->phasor >= 1.0)
        b->phasor -= 1.0;
}

inline
void BL_Osc_init(BL_Osc* b) {
    b->phasor = 0.0;
    b->freq = 0.0;
    b->phaseOfs = 0.5;
}

#endif
