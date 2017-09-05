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
 */

#ifndef SYNTH_H
#define SYNTH_H

#include "common/types.h"
#include "common/voice.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "common/adsr.h"
#include "common/oscillator.h"
#include "common/ls_filter.h"
#include "common/roland.h"
#include "common/filter.h"
#include "common/delay.h"
#include <stdlib.h>

/* 
 * the number of concurrent voices for the same midi note
 */
#define NUM_NOTES 16
#define NUM_LS 3
#define NUM_ES 4

#define EQ_IN(x) exp(2.99573227355399 + 6.90775527898214 * x)

typedef struct _note_history {
    Voice*      voices[NUM_VOICES];
    uint8_t     next;
} NoteHistory;

enum modulatable {
    MB_FL1
  , MB_FL2
  , MB_FAMP1
  , MB_FAMP2
  , MB_OS1_PIT
  , MB_OS2_PIT
  , MB_PAN 
  
};
    
typedef struct _SWSynth {
    
    float*          params[V_PARAMS];
    Roland*         roland;
    Voice*          history[128];
    NoteHistory     nhistory[128];
    uint8_t         midi_cc[18][128];
    float           prev[V_PARAMS];
        
    enum filter_input filter_input[4];
    OscWave         lfo[3];
    Adsr            env[4];
    Voice*          voices[NUM_VOICES];
    Voice*          voice_last;
    Filter          filter[4];
    biquad          eq_filters[3];
    Delay           delay_l;
    Delay           delay_r;
    
    //InterpParm      pan;
    InterpParm      mix;
    InterpParm      filter_mix;
    InterpParm      filter3_co;
    InterpParm      filter4_co;
    InterpParm      filter_res[4];
    
    GlobalParms     gbl;           /// global parameters from MIDI or otherwise
    ModSource       mod_cutoff1;
    ModSource       mod_cutoff2;
    ModSource       mod_cutoff3;
    ModSource       mod_cutoff4;
    ModSource       mod_panning;
    ModSource       mod_osc1_pitch;
    ModSource       mod_osc2_pitch;
    
    // oscillator wave tables
    long            table_len;
    sample_t        fs;
    sample_t        peak;
    sample_t        panning;
    sample_t        midi_brightness;
    sample_t        midi_timbre;
    sample_t        fpb;
    
    uint32_t        peak_frames;
    uint32_t        n_active_notes;
    uint32_t        program_prev;
    uint32_t        frames;
    uint8_t         delay_split;
    uint8_t         filter_prev;
    uint8_t         osc1_wave_prev;
    uint8_t         osc2_wave_prev;
    uint8_t         lfo1_wave_prev;
    uint8_t         lfo2_wave_prev;
    sample_t        last_note[NUM_OSCS];    
    uint8_t         clock;
    uint8_t         polyphony;
    uint8_t         key_split;
    uint8_t         midi_channel;
    uint8_t         last_port;
    uint8_t         midi_learn_on;
    uint8_t         voice_idx;
    float*          table[OSC_TABLES];
    
    
    
        
} SWSynth;

void
synth_init(SWSynth* const s, const float* const params[], const sample_t s_rate);

void synth_frame(SWSynth* const s, uint32_t len);

sample_t 
synth_process(SWSynth* const s, const uint32_t pos, sample_t* const left
            , sample_t* const right);

/* 
 * Assign an incoming note to a voice
 */
Voice* 
synth_note_on(SWSynth* const s, const uint8_t note, const uint8_t vel);

void 
synth_all_notes_off(SWSynth* const s);

void 
synth_note_off(SWSynth* const s, const uint8_t note, const uint8_t vel);

void 
synth_midi(SWSynth* const s,const uint8_t* const msg);

void
synth_free(SWSynth* s);

static inline sample_t mod_value(SWSynth* const s, ModSource* m);
static inline sample_t mod_value(SWSynth* const s, ModSource* m) {
    sample_t val = 0;
    
    if (m->mod1 + m->mod2 + m->mod3 == 0) return 0;
    if (m->mod1) 
        switch(m->mod1) 
        {
            case M_L1:
                val += (s->lfo[0].out*m->mod1_amt);
                break;
            case M_L2:
                val += (s->lfo[1].out*m->mod1_amt);
                break;
            case M_L3:
                val += (s->lfo[2].out*m->mod1_amt);
                break;
            case M_E1:
                val += (s->env[0].output);
                break;
            case M_E2:
                val += (s->env[1].output);
                break;
            case M_E3:
                val += (s->env[2].output);
                break;
            case M_E4:
                val += (s->env[3].output);
                break;    
            default:
                break;            
        }
        
    if (m->mod2) 
        switch(m->mod2) 
        {
            case M_L1:
                val += (s->lfo[0].out);
                break;
            case M_L2:
                val += (s->lfo[1].out);
                break;
            case M_L3:
                val += (s->lfo[2].out);
                break;
            case M_E1:
                val += (s->env[0].output);
                break;
            case M_E2:
                val += (s->env[1].output);
                break;
            case M_E3:
                val += (s->env[2].output);
                break;
            case M_E4:
                val += (s->env[3].output);
            default:
                break;            
        }
    if (m->mod3) 
        switch(m->mod3) 
        {
            case M_L1:
                val += (s->lfo[0].out);
                break;
            case M_L2:
                val += (s->lfo[1].out);
                break;
            case M_L3:
                val += (s->lfo[2].out);
                break;
            case M_E1:
                val += (s->env[0].output);
                break;
            case M_E2:
                val += (s->env[1].output);
                break;
            case M_E3:
                val += (s->env[2].output);
                break;
            case M_E4:
                val += (s->env[3].output);
            default:
                break;            
        }
    return val;
}


/* calculate voice cutoff modulation. LFO sweeps in dB */
static inline sample_t mod_cutoff(SWSynth* const s, ModSource* m
                                  , sample_t val);
static inline sample_t mod_cutoff(SWSynth* const s, ModSource* m
                                  , sample_t val) {
    if (m->mod1_amt+m->mod2_amt+m->mod3_amt == 0) return val;
    if (m->mod1+m->mod2+m->mod3 ==0) return val;
    sample_t amt = 0.0;

    if (m->mod1) 
        amt = 0;
        switch(m->mod1) 
        {
            case M_L1:
                amt = s->lfo[0].out * m->mod1_amt * 64.0;
                break;
            case M_L2:
                amt = s->lfo[1].out * m->mod1_amt * 64.0;
                break;
            case M_L3:
                amt = s->lfo[2].out * m->mod1_amt * 64.0;
                break;
            case M_E1:
                amt = 64.0f * m->mod1_amt * (s->env[0].output);
                break;
            case M_E2:
                amt = 64.0f * m->mod1_amt * (s->env[1].output);
                break;
            case M_E3:
                amt = 64.0f * m->mod1_amt * (s->env[2].output);
                break;
            case M_E4:
                amt = 64.0f * m->mod1_amt * (s->env[3].output);
                break;    
            case M_EN3_INV:
                amt = -64.0f * m->mod1_amt * (s->env[2].output);
                break;
            case M_EN4_INV:
                amt = -64.0f * m->mod1_amt * (s->env[3].output);
                break;  
            case M_L1_X:
                amt *= m->mod1_amt * (s->lfo[0].out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod1_amt * (s->lfo[1].out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod1_amt * (s->lfo[2].out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod1_amt * (s->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod1_amt * (s->env[3].output);
                break;    
            default:
                break;            
        }
    if (m->mod2) 
    
        switch(m->mod2) 
        {
            case M_L1:
                amt += s->lfo[0].out * m->mod2_amt * 64.0;
                break;
            case M_L2:
                amt += s->lfo[1].out * m->mod2_amt * 64.0;
                break;
            case M_L3:
                amt += s->lfo[2].out * m->mod2_amt * 64.0;
                break;
            case M_E1:
                amt += 64.0f * m->mod2_amt * (s->env[0].output);
                break;
            case M_E2:
                amt += 64.0f * m->mod2_amt * (s->env[1].output);
                break;
            case M_E3:
                amt += 64.0f * m->mod2_amt * (s->env[2].output);
                break;
            case M_E4:
                amt += 64.0f * m->mod2_amt * (s->env[3].output);
                break;    
            case M_EN3_INV:
                amt += -64.0f * m->mod2_amt * (s->env[2].output);
                break;
            case M_EN4_INV:
                amt += -64.0f * m->mod2_amt * (s->env[3].output);
                break;    
            case M_L1_X:
                amt *= m->mod2_amt * (s->lfo[0].out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod2_amt * (s->lfo[1].out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod2_amt * (s->lfo[2].out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod2_amt * (s->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod2_amt * (s->env[3].output);
                break;   
            default:
                break;              
        }
    if (m->mod3) 
        switch(m->mod3) 
        {
            case M_L1:
                amt += s->lfo[0].out * m->mod3_amt * 64.0;
                break;
            case M_L2:
                amt += s->lfo[1].out * m->mod3_amt * 64.0;
                break;
            case M_L3:
                amt += s->lfo[2].out * m->mod3_amt * 64.0;
                break;
            case M_E1:
                amt += 64.0f * m->mod3_amt * (s->env[0].output);
                break;
            case M_E2:
                amt += 64.0f * m->mod3_amt * (s->env[1].output);
                break;
            case M_E3:
                amt += 64.0f * m->mod3_amt * (s->env[2].output);
                break;
            case M_E4:
                amt += 64.0f * m->mod3_amt * (s->env[3].output);
                break;    
            case M_EN3_INV:
                amt += -64.0f * m->mod3_amt * (s->env[2].output);
                break;
            case M_EN4_INV:
                amt += -64.0f * m->mod3_amt * (s->env[3].output);
            case M_L1_X:
                amt *= m->mod3_amt * (s->lfo[0].out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod3_amt * (s->lfo[1].out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod3_amt * (s->lfo[2].out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod3_amt * (s->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod3_amt * (s->env[3].output);
                break;   
            default:
                break;       
        }
    if (amt!=0) 
        val = midi2freq(freq2midi(val) + amt);
    if (val>20000.0) val =20000.0;
    
    return val;
}

#endif
