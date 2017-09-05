/**
 * @file
 * @author  Brendan Jones <brendan.jones.it@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 * 
 * Copyright 2014, Brendan Jones
 *
 * This program is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.   See the GNU General Public License (version 2 or
 * later) for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * @section DESCRIPTION
 */

#include "types.h"
#ifndef OSCILLATOR_H
#define OSCILLATOR_H
#include <math.h>
#include <stdlib.h>
#include "ladspa-util.h"
#include "osc_common.h"
#include "bl_osc.h"
#define TWO_PI  (2 * 3.14159265)

typedef struct _oscwave {
    sample_t        srate;
    sample_t        phase;
    sample_t        amp; 
    sample_t        freq; 
    sample_t        offset; 
    sample_t        out;
    BL_Osc*         bosc;
    long            tablen;
    long            idx;
    enum waveshape  shape;
    enum osc_type   type;
    int             sync;
    const float** table;
} OscWave;

static inline void osc_init(OscWave* const osc,
    enum osc_type _type,
    enum waveshape _shape,
    sample_t _srate,
    long _tablen,
    sample_t _amp,
    sample_t _freq,
    sample_t _offset,
    sample_t _initialPhase,
    const float** const table); 

static inline void 
osc_phase(OscWave* const osc, sample_t _phase);

static inline void osc_update(OscWave* const osc,
    sample_t _amp,
    sample_t _freq
    );

// load the wave tables

static inline void osc_filltables(float** const table, const sample_t srate
                                , const sample_t len);

static inline void blosc_make_tables(const sample_t s_rate, const sample_t len);

static inline sample_t osc_tic_freq(OscWave* const osc, sample_t amp
                                    , sample_t freq); 

static inline sample_t osc_tic(OscWave* const osc); 

static inline void copy1stSmp(float* table, long tablen) // copies the first sample to the length+1th...
{
    table[tablen-1] = table[0];
}


static inline void 
osc_init(OscWave* const osc,
    enum osc_type _type,
    enum waveshape _shape,
    sample_t _srate,
    long _tablen,
    sample_t _amp,
    sample_t _freq,
    sample_t _offset,
    sample_t _initialPhase,
    const float** const table) 
{
    osc->shape  = _shape;
    osc->type   = _type;
    osc->srate  = _srate;
    osc->tablen = _tablen;
    osc->amp    = _amp;
    osc->freq   = _freq;
    osc->offset   = _offset;    // phase offset 
    osc->phase  = _initialPhase;
    osc->sync   = 0;
    osc->table  = table;
    osc->idx= 0; 
    
    if (_type == BLIT) {
        osc->bosc           = (BL_Osc*) calloc(1, sizeof(BL_Osc));
        BL_Osc_init(osc->bosc);
        osc->bosc->phaseOfs = _offset;
        osc->bosc->phasor   = _initialPhase; 
        osc->bosc->shape    = _shape;
    } else {
        osc->bosc = NULL;
    }
}   

static inline void 
osc_phase(OscWave* const osc, sample_t _offset)
{
    if (_offset != osc->offset) 
    {
            osc->bosc->phaseOfs = _offset * 0.95;
    }
    osc->offset = _offset;
}

static inline void 
osc_update(OscWave* const osc
                    , sample_t _amp
                    , sample_t _freq
                    )
{
    osc->amp = _amp;
    osc->freq = _freq;
    
    if (osc->bosc) {
        osc->bosc->freq = _freq;
        // set if phase has been changed
    }

}

static inline sample_t
func_stair(int sample_num, sample_t tablen)
{
    if (sample_num < (tablen/4.0)) {
        return 0.0;
    }
    sample_num -= (tablen/4.0);
    if (sample_num < (tablen/4.0)) {
        return 1.0;
    }
    sample_num -= (tablen/4.0);
    if (sample_num < (tablen/4.0)) {
        return 0.0;
    }
    return -1.0;
}

static inline void 
osc_filltables(float** const table, const sample_t srate, const sample_t len) {
    sample_t white, b0, b1, b2, b3, b4, b5;
    const sample_t div = ((sample_t)RAND_MAX) * 0.5;
    for (int i = 0; i < OSC_TABLES; i++)
    {
        long j = 0;
        sample_t tablen = len;
        switch (i) 
        {
            case Sine:        
                for (j = 0; j < tablen; j++) {
                    table[i][j] = (sample_t) sin(TWO_PI * j / tablen);
                }
                copy1stSmp(table[i],tablen);
                break;
            case Triangle:
                for( j = 0; j < tablen/2.0; j++)
                    table[i][j] =  2.0 * (sample_t) j/ (sample_t) (tablen/2.0) - 1.0;
                for( j = tablen/2.0; j < tablen; j++)
                    table[i][j] = 1.0 -  (2.0 * (sample_t) (j-tablen/2.0) / (sample_t) (tablen/2.0));
                copy1stSmp(table[i],tablen);
                break;
            case Saw:
            case Pulse:
                for( j = 0; j < tablen; j++)
                    table[i][j] = (2.0 * (sample_t) j / (sample_t) tablen) - 1.0 ;
                copy1stSmp(table[i],tablen);
                break;
            case ReverseSaw:
                for( j = 0; j < tablen; j++)
                    table[i][j] = (2.0 * (tablen - (sample_t) j )/ (sample_t) tablen) - 1.0;
                copy1stSmp(table[i],tablen);
                break;
            case Square:
                for( j = 0; j < tablen/2; j++) table[i][j] = 1;
                for( j = tablen/2; j < tablen; j++) table[i][j] = -1.0;
                copy1stSmp(table[i],tablen);
                break;
            case PinkNs:
                white=b0=b1=b2=b3=b4=b5=0;
                for( j = 0; j < tablen; j++) {
                    white = (rand()/div) - 1.0;
                    b0 = 0.997 * b0 + 0.029591 * white;
                    b1 = 0.985 * b1 + 0.032534 * white;
                    b2 = 0.950 * b2 + 0.048056 * white;
                    b3 = 0.850 * b3 + 0.090579 * white;
                    b4 = 0.620 * b4 + 0.108990 * white;
                    b5 = 0.250 * b5 + 0.255784 * white;
                    table[i][j] = 0.55 * (b0 + b1 + b2 + b3 + b4 + b5);
                }
                copy1stSmp(table[i],tablen);
                break;
            case StairStep:
                for( j = 0; j < tablen; j++) {
                    table[i][j] = func_stair(j,tablen);
                }
                copy1stSmp(table[i],tablen);
                break;
            default:
                break;
        }
        
    }
}

static inline void blosc_make_tables(const sample_t s_rate, const sample_t len)
{
    float* in[OSC_TABLES];
    for (uint16_t i = 0;  i < OSC_TABLES; i++) 
    {
        in[i]     = (float*) malloc((int)len*sizeof(float));
    }
    osc_filltables(in, s_rate, len);
    for (int i = 0; i < OSC_TABLES; i++)
    {
        if ( i != Saw) bl_wave_osc(i, in[i], len);
        else bl_saw_osc(len);
    }
    for (uint16_t i = 0;  i < OSC_TABLES; i++) {
        free(in[i]);
    }
}

static inline sample_t 
osc_tic_freq(OscWave *osc, sample_t amp, sample_t freq)
{
    if (freq==0) return 0;
    sample_t out = 0;
    if ((osc->type == BLIT)) {
        BL_Osc* bosc = osc->bosc;
        bl_freq(bosc,(float) freq / osc->srate);
        out = bl_output_minus_offset(bosc) * amp;
        bl_update_phase(bosc);
        return out * osc->amp;
    }
    sample_t curr_phs = osc->phase * osc->tablen;   // get current phase and scale it
    long index = (long) curr_phs;       // get the integer part only
    sample_t curr_samp = (sample_t) osc->table[osc->shape][index];    // get current sample of the table
    sample_t next_samp = (sample_t) osc->table[osc->shape][index+1];  // get the next sample too
    sample_t fract = curr_phs - index;    // get the fractional part of phase
    // apply the linear interpolation formula to get the output
    out = (curr_samp + (next_samp - curr_samp) * fract) * amp;
    sample_t si = freq / osc->srate;             // calculate the sampling increment
    osc->phase += si;                        // increment phase
    while (osc->phase >=1 ) osc->phase -= 1; // check if the phase is out of range...
    while (osc->phase < 0 ) osc->phase += 1;      // ...and eventually wrap it around
    osc->out = out;
    return out;                         // return current interpolated sample
}

static inline sample_t 
osc_tic(OscWave *osc)
{
    sample_t out;
    sample_t amp = osc->amp;
    sample_t freq = osc->freq;
    
    if ((osc->type == BLIT)) {
        //if (osc->shape == Pulse) {
        if (osc->offset != 0 ) {
            BL_Osc* bosc = osc->bosc;
            bl_freq(bosc,(float) freq / osc->srate);
            out = bl_output_minus_offset(bosc);
            bl_update_phase(bosc);
            return out * osc->amp;
        } else {
            BL_Osc* bosc = osc->bosc;
            bl_freq(bosc,(float) freq / osc->srate);
            out = bl_output(bosc);
            bl_update_phase(bosc);
            return out * osc->amp;
        }
    }
    if (freq==0) return 0;
    if (osc->shape == PinkNs ) {
        // ignore frequncy and phase just output
        out = osc->table[osc->shape][osc->idx++];
        if (osc->idx > osc->tablen) osc->idx = 0;
        return out;
    }
    sample_t curr_phs = osc->phase * osc->tablen;   // get current phase and scale it
    long index = (long) curr_phs;       // get the integer part only
    sample_t curr_samp = (sample_t) osc->table[osc->shape][index];    // get current sample of the table
    sample_t next_samp = (sample_t) osc->table[osc->shape][index+1];  // get the next sample too
    sample_t fract = curr_phs - index;    // get the fractional part of phase
    // apply the linear interpolation formula to get the output
    out = (curr_samp + (next_samp - curr_samp) * fract) * amp;
    sample_t si = freq / osc->srate;             // calculate the sampling increment
    osc->phase += si;                        // increment phase
    while (osc->phase >=1 ) osc->phase -= 1; // check if the phase is out of range...
    while (osc->phase < 0 ) osc->phase += 1;      // ...and eventually wrap it around
    osc->out = out;
    return out;                         // return current interpolated sample
}
#endif
