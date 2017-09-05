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
 * 
 */

#ifndef VOICE_H
#define VOICE_H

#include "types.h"
#include "voice_params.h"
#include "adsr.h"
#include "oscillator.h"
#include "roland.h"
#include "ls_filter.h"
#include "filter.h"
#include "ladspa-util.h"

#define NUM_VOICES      16
#define NUM_SHPS        32
#define NUM_OSCS         2
#define CENTER_NOTE     48

#define FILT_IN(x) 20.0 * pow(14000 / 20, x)
#define ADSR_IN(x) 0.00001 * pow(10000 / 0.001, x)

#ifdef __cplusplus
extern "C" {
#endif
enum osc_idx{
       OSC1 = 0
       , OSC2
};

enum mod_source {
        M_NONE       = 0
      , M_L1
      , M_L2
      , M_L3
      , M_E1
      , M_E2
      , M_E3
      , M_E4
      , M_EN3_INV
      , M_EN4_INV
      , M_L1_X
      , M_L2_X
      , M_L3_X
      , M_E3_X
      , M_E4_X
      , M_AFTER
      , M_PRESSURE

};

enum filter_input {
        FI_NONE       = 0
      , FI_OSC1OSC2
      , FI_OSC1     
      , FI_OSC2
      , FI_FL1
};

typedef struct {
    sample_t        key_track;
    sample_t        mod1_amt;
    sample_t        mod2_amt;
    sample_t        mod3_amt;
    sample_t        out;
    enum mod_source mod1;
    enum mod_source mod2;
    enum mod_source mod3;
    uint8_t         mod1_retrigger;
    uint8_t         mod2_retrigger;
    uint8_t         mod3_retrigger;    
} ModSource;

typedef struct {
    sample_t        tuning;
    sample_t        bender;         
    sample_t        modwheel;
    sample_t        tempo;
} GlobalParms;
    
typedef struct _osc_parms {
    Filter*         hp_filter;         // frequency tracking HP filter
    Filter*         lp_filter;         // 20000HZ filter
    sample_t        amp;               // total amplitude all oscillators
    sample_t        phs_offset;        // phase offset
    sample_t        tune;              // frequency
    sample_t        distortion;        // waveshaping
    sample_t        noise;             // noise amount
    sample_t        sub_osc;           // sub amount
    sample_t        mod_value; 
    ModSource       mod;
    ModSource       pwm_mod;
    uint8_t         uni;               // number of oscillators in unison
    uint8_t         wave;              // osciallator wave shape
    uint8_t         uni_type;          // detune amount [ 0 .. 127 ]
    uint8_t         uni_det;           // detune amount [ 0 .. 127 ]
    uint8_t         uni_mix;           // mix amount [ 0 .. 127 ]
    
} OscParms;


typedef struct {
    sample_t value;
    sample_t target;
    sample_t inc;
    uint32_t samples;
} InterpParm;

typedef struct _adsr_parms {
    sample_t        attack;
    sample_t        decay;
    sample_t        release;
    sample_t        sustain;
    uint8_t         loop;
} AdsrParms;

typedef struct _voice 
{
    OscWave*    lfo1;
    OscWave*    lfo2;
    OscWave*    lfo3;
    Adsr        env[4];
    Roland*     r; 
    OscWave*    noise;
    OscWave*    sub_osc;
    Filter*     filter1;
    Filter*     filter2;
    InterpParm  filter1_co;
    InterpParm  filter2_co;
    ModSource   mod_cutoff1;
    ModSource   mod_cutoff2;
    OscParms    osc_parms[NUM_OSCS];        // oscillator parameters
    OscWave     osc[NUM_OSCS][NUM_SHPS];    // oscillator wave tables
    
    sample_t    center_note[NUM_OSCS];      // for key tracking
    sample_t    note;                       // current midi note on
    sample_t    key_note;                   // keyed note
    sample_t    srate;                      // sample rate
    sample_t    last_note;                  // previous note (for glide)
    sample_t    freq;                       // modulated frequency
    sample_t    amp;                        // amplitude
    sample_t    gain;                       // master gain
    sample_t    tuning;                     // tuning 
    sample_t    mix;                        // osc1/osc2 mix
    sample_t    table_len;
    sample_t    filter_mix;
    sample_t    panning;                    // panning amount [-1.0 .. +1.0]
    uint32_t    debug;
    
    sample_t    glide;                      // glide amount
    sample_t    glide_factor;               // per sample glide_factor
    sample_t    glide_factor_mult;          // cumulative factor
    
    sample_t    midi_modwheel;              // midi_modwheel
    sample_t    midi_bender;                // bender
    
    uint32_t    glide_step;                 // glide amount
    enum filter_input filter1_input;
    enum filter_input filter2_input;
    
    uint8_t     vel;                        // incoming velocity
    uint8_t     channel;                    // channel
    uint8_t     key_split;                  // key split
    
    
} Voice;

/*
 * Pre-alloc all voices, envelopes and oscillators
 */
void 
voice_init_all(Voice** v, const sample_t s_rate, Roland* r);

uint8_t
voice_find_free(Voice** v);

static inline sample_t midi2freq(sample_t note);
static inline sample_t midi2freq(sample_t note)
{
    return 440.0 * pow( 2.0, (note - 69.0) / 12.0 );
}

static inline sample_t freq2midi(sample_t f);
static inline sample_t freq2midi(sample_t f)
{
    return 12 * log2(f/440.0f) + 69.0;
}

Voice* 
voice_set(sample_t note, const uint8_t vel, Voice* const v
        , Filter* const filter1
        , Filter* const filter2
        , const float** const tables
        , const float* const p[]
        , const sample_t last_note);

void
voice_update(
             Voice**        v
           , GlobalParms*   gbl
           , Filter* const  filter1
           , Filter* const  filter2
           , const float* const params[]
           , const uint32_t len);

/*
 * Simple linear interpolation over _samples
 */
static inline void interpolate(InterpParm *i, sample_t _val, sample_t _newval
                             , uint32_t _samples)
{
    i->samples  = _samples;
    i->target   = _newval;
    i->value    = _val;
    if (_val == _newval) 
    {
        i->samples = 0;
        i->inc  = 0;
    }
    else 
    {
        i->inc  = ( i->target - i->value ) / _samples;
    }
}

/*
 * Simple linear interpolation over _samples
 */
static inline sample_t interp_value(InterpParm *i)
{
    if (i->samples != 0) {
        i->samples--;
        i->value += i->inc;
    } else {
       i->value = i->target;
    }
    
    return i->value;
}
    
static inline sample_t voice_mod_phase( Voice* const v
                                        , sample_t phase
                                        , const ModSource* const m);

static inline sample_t voice_mod_phase( Voice* const v
                                        , sample_t phase
                                        , const ModSource* const m)
{
    if (m->mod1) {
        switch(m->mod1) 
        {
            case M_L1:
                phase = phase + (0.95 * v->lfo1->out + 1.0) * 0.5;
                break;
            case M_L2:
                phase = phase + (0.95 * v->lfo2->out + 1.0) * 0.5;
                break;
            case M_L3:
                phase = phase + (0.95 * v->lfo3->out + 1.0) * 0.5;
                break;
            default:
                break;
        }
    }
    return phase;
}
            
/* calculate modulation */
static inline sample_t voice_mod_cutoff(Voice* const v, const ModSource* const m
                                     , sample_t val, const sample_t note);
static inline sample_t voice_mod_cutoff(Voice* const v, const ModSource* const m
                                     , sample_t val, const sample_t note) {
    if (m->mod1_amt + m->mod2_amt + m->mod3_amt + m->key_track ==0) return val;
    if (m->mod1 + m->mod2 + m->mod3 + m->key_track ==0) return val;
    sample_t amt = 0.0;
    
    if (m->mod1) 
        amt = 0;
        switch(m->mod1) {
            case M_L1:
                amt = v->lfo1->out * m->mod1_amt * 64.0;
                break;
            case M_L2:
                amt = v->lfo2->out * m->mod1_amt * 64.0;
                break;
            case M_L3:
                amt = v->lfo3->out * m->mod1_amt * 64.0;
                break;
            case M_E1:
                amt = 64.0f * m->mod1_amt * (v->env[0].output);
                break;
            case M_E2:
                amt = 64.0f * m->mod1_amt * (v->env[1].output);
                break;
            case M_E3:
                amt = 64.0f * m->mod1_amt * (v->env[2].output);
                break;
            case M_E4:
                amt = 64.0f * m->mod1_amt * (v->env[3].output);
                break;    
            case M_EN3_INV:
                amt = -64.0f * m->mod1_amt * (v->env[2].output);
                break;
            case M_EN4_INV:
                amt = -64.0f * m->mod1_amt * (v->env[3].output);
                break;    
            case M_L1_X:
                amt *= m->mod1_amt * (v->lfo1->out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod1_amt * (v->lfo2->out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod1_amt * (v->lfo3->out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod1_amt * (v->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod1_amt * (v->env[3].output);
                break;    
            default:
                break;            
        }
    if (m->mod2) 
    
        switch(m->mod2) {
            case M_L1:
                amt += v->lfo1->out * m->mod2_amt * 64.0;
                break;
            case M_L2:
                amt += v->lfo2->out * m->mod2_amt * 64.0;
                break;
            case M_L3:
                amt += v->lfo3->out * m->mod2_amt * 64.0;
                break;
            case M_E1:
                amt += 64.0f * m->mod2_amt * (v->env[0].output);
                break;
            case M_E2:
                amt += 64.0f * m->mod2_amt * (v->env[1].output);
                break;
            case M_E3:
                amt += 64.0f * m->mod2_amt * (v->env[2].output);
                break;
            case M_E4:
                amt += 64.0f * m->mod2_amt * (v->env[3].output);
                break;    
            case M_EN3_INV:
                amt += -64.0f * m->mod2_amt * (v->env[2].output);
                break;
            case M_EN4_INV:
                amt += -64.0f * m->mod2_amt * (v->env[3].output);
                break;    
            case M_L1_X:
                amt *= m->mod2_amt * (v->lfo1->out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod2_amt * (v->lfo2->out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod2_amt * (v->lfo3->out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod2_amt * (v->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod2_amt * (v->env[3].output);
                break;    
            default:
                break;              
        }
    if (m->mod3) 
        switch(m->mod3) {
            case M_L1:
                amt += v->lfo1->out * m->mod3_amt * 64.0;
                break;
            case M_L2:
                amt += v->lfo2->out * m->mod3_amt * 64.0;
                break;
            case M_L3:
                amt += v->lfo3->out * m->mod3_amt * 64.0;
                break;
            case M_E1:
                amt += 64.0f * m->mod3_amt * (v->env[0].output);
                break;
            case M_E2:
                amt += 64.0f * m->mod3_amt * (v->env[1].output);
                break;
            case M_E3:
                amt += 64.0f * m->mod3_amt * (v->env[2].output);
                break;
            case M_E4:
                amt += 64.0f * m->mod3_amt * (v->env[3].output);
                break;    
            case M_EN3_INV:
                amt += -64.0f * m->mod3_amt * (v->env[2].output);
                break;
            case M_EN4_INV:
                amt += -64.0f * m->mod3_amt * (v->env[3].output);
                break;    
            case M_L1_X:
                amt *= m->mod3_amt * (v->lfo1->out+1.0)/2;
                break;
            case M_L2_X:
                amt *= m->mod3_amt * (v->lfo2->out+1.0)/2;
                break;
            case M_L3_X:
                amt *= m->mod3_amt * (v->lfo3->out+1.0)/2;
                break;
            case M_E3_X:
                amt *= m->mod3_amt * (v->env[2].output);
                break;
            case M_E4_X:
                amt *= m->mod3_amt * (v->env[3].output);
                break;    
            default:
                break;       
        }
    if ((amt!=0)||(m->key_track!=0)) 
        val = midi2freq(freq2midi(val) + amt 
                     + (note - CENTER_NOTE) * m->key_track);
    if (val>20000.0) val =20000.0;
    return val;
}

static sample_t 
voice_process(Voice* const v, const uint32_t cnt
            , sample_t* const left, sample_t* const right);

static sample_t 
voice_process(Voice* const v, const uint32_t cnt
            , sample_t* const left, sample_t* const right)
{
    const sample_t vel  = v->vel/127.0;
    const sample_t clip = 100.0;
    const sample_t clipi = 1.0 / 100.0;

    const int uni1      = v->osc_parms[OSC1].uni;
    const int uni2      = v->osc_parms[OSC2].uni;
    Filter* const f1 = v->filter1;
    Filter* const f2 = v->filter2;
    sample_t mono_out   = 0.0;
    sample_t adsr1      = 1.0;
    sample_t adsr2      = 1.0;
    sample_t filt1_out  = 0.0;
    sample_t filt2_out  = 0.0;
    sample_t note1      = 0.0;
    sample_t note2      = 0.0;
    sample_t nnoise     = 0.0;
    sample_t noise      = v->osc_parms[OSC2].noise;
    sample_t nsub_osc   = 0.0;
    sample_t sub_osc    = v->osc_parms[OSC1].sub_osc;
    
    sample_t amp1       = v->osc_parms[OSC1].amp;
    sample_t amp2       = v->osc_parms[OSC2].amp;
    sample_t offset1    = v->osc_parms[OSC1].phs_offset;
    sample_t offset2    = v->osc_parms[OSC2].phs_offset;
    
    ModSource offset1_m = v->osc_parms[OSC1].pwm_mod;    
    ModSource offset2_m = v->osc_parms[OSC2].pwm_mod;    
    
    /*sample_t dist1      = v->osc_parms[OSC1].distortion > 0.0 
                          ? (v->osc_parms[OSC1].distortion*2)
                            /(1-v->osc_parms[OSC1].distortion)
                          : 0.0;*/
    sample_t dist1      = v->osc_parms[OSC1].distortion > 0.0 
                          ? pow(10, v->osc_parms[OSC1].distortion
                                * v->osc_parms[OSC1].distortion * 3.0)
                            - 1.0 + 0.0001
                          : 0.0;
    sample_t dist2      = v->osc_parms[OSC2].distortion > 0.0 
                          ? (v->osc_parms[OSC2].distortion*2)
                            /(1-v->osc_parms[OSC2].distortion)
                          : 0.0;
    f1->cutoff    = interp_value(&v->filter1_co);
    f2->cutoff    = interp_value(&v->filter2_co);
    
    f1->cutoff = voice_mod_cutoff(v,&v->mod_cutoff1
                                    , v->filter1->cutoff, v->center_note[0]);
    f2->cutoff = voice_mod_cutoff(v,&v->mod_cutoff2
                                    , v->filter2->cutoff, v->center_note[1]);
    
    
    if ((v->env[0].state == adsr_idle)&&(v->env[1].state == adsr_idle)) return 0;
    
    if (v->amp == 0.0 ) return 0;
 
    osc_tic(v->lfo1);
    osc_tic(v->lfo2);
    osc_tic(v->lfo3);
    
    adsr1 = adsr_process(&v->env[0]);
    adsr2 = adsr_process(&v->env[1]);
    adsr_process(&v->env[2]);
    adsr_process(&v->env[3]);
    
    if ( (v->note<60 && v->key_split) || !v->key_split ) 
    {
        if (v->env[0].state != adsr_idle)
        {
            for (unsigned int j = uni1; j--; ) 
            {
                OscWave* o = &(v->osc[OSC1][j]);
                if (j == 0) osc_phase(o, voice_mod_phase(v, offset1, &offset1_m));
                note1 +=  osc_tic(o);
            }
            
            if (dist1 > 0.0) 
            {
                //if (dist1 >= 1.0) dist1 = 0.9999;
                //note1 = ((1+dist1)*(note1)/(1+dist1*abs(note1)));
                
                note1 = atan(note1 * dist1) / atan(dist1);
                //note1 = (note1 ) / atan(dist1);
            }
            
            if (sub_osc > 0.0) 
            {
                nsub_osc = osc_tic(v->sub_osc);
            }
            if (dist1 > 0.0) 
            {
                nsub_osc = atan(nsub_osc * dist1) / atan(dist1);
            }
        }
    }
    
    note1 = (note1 * adsr1 * vel * amp1 
            + nsub_osc * adsr1 * vel * sub_osc) * v->gain;
    
    if (note1) {
        //note1 = filter_run(v->osc_parms[OSC1].hp_filter,note1);
        //note1 = filter_run(v->osc_parms[OSC1].lp_filter,note1);
        if (abs(note1)>1.0) note1 = clipi * atan( note1 * clip );
        filt1_out += filter_run(f1, note1);
    }
    
    if ( (v->note>59 && v->key_split) || !v->key_split) {
        if (v->env[1].state != adsr_idle) 
        {    
            for (unsigned int j = uni2; j--; ) 
            {
                OscWave* o = &(v->osc[OSC2][j]);
                if (j == 0) osc_phase(o, voice_mod_phase(v, offset2, &offset2_m));
                note2 +=  osc_tic(o);
            }
            if (dist2 > 0.0) 
            {
                if (dist2 >= 1.0) dist2 = 0.9999;
                note2 = (1+dist2)*note2/(1+dist2*abs(note2));
            }
        
            if (noise > 0.0) 
            {
                nnoise = osc_tic(v->noise)*noise;
            }
        }
    }
    note2 = (note2 * adsr2 * vel * amp2
            + nnoise * adsr2 * vel * noise);
            
    if (note2) {
        if (abs(note2)>1.0) note2 = clipi * atan( note2 * clip );
        filt2_out += filter_run(f2, note2);
    }
    
    if (v->glide_step > 0) {
        v->glide_step--;
        v->note += v->glide_factor;
    }
    
    mono_out  = filt2_out + filt1_out;
    
    *left    = filt1_out;
    *right   = filt2_out;
    
    if (abs(mono_out)>1.0) {
        mono_out = clipi * atan( mono_out * clip );
    }
    if (mono_out > 1.0) printf("overflow3: %f\n",mono_out);
    return mono_out;
   
}
#ifdef __cplusplus
}
#endif

#endif
