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

#include "voice_params.h"
#include "voice.h"
#include "roland.h"

#include <stdio.h>

#define LSF_BW 0.9
#define LSF_FB 0.9f

void voice_init(Voice* v, const sample_t s_rate, Roland* r) {
  v->srate = s_rate;

  adsr_init(&v->env[0],0.2,0.0,1.0,0.1, s_rate);
  adsr_init(&v->env[1],0.2,0.0,1.0,0.1, s_rate);
  adsr_init(&v->env[2],0.0,0.0,1.0,0.0, s_rate);
  adsr_init(&v->env[3],0.0,0.0,1.0,0.0, s_rate);
    
  v->lfo1         = calloc( 1, sizeof(OscWave));
  v->lfo2         = calloc( 1, sizeof(OscWave));
  v->lfo3         = calloc( 1, sizeof(OscWave));
  v->noise        = calloc( 1, sizeof(OscWave));
  v->sub_osc      = calloc( 1, sizeof(OscWave));
  v->filter1      = calloc( 1, sizeof(Filter));
  v->filter2      = calloc( 1, sizeof(Filter));
  v->r            = r;
  v->debug        = 0;
  v->gain         = 1.0;
  v->srate        = s_rate;
  v->table_len    = s_rate * O_SMPL + 1;
    
  // not in use
  v->osc_parms[OSC1].hp_filter = v->osc_parms[OSC1].lp_filter = 0;
  v->osc_parms[OSC2].hp_filter = v->osc_parms[OSC2].lp_filter = 0;
    
  filter_alloc(v->filter1, s_rate);
  filter_alloc(v->filter2, s_rate);
}

/*
 * Initialize an array of voices - done outside the RT thread
 */

void voice_init_all(Voice** v, sample_t s_rate, Roland* r) {
  for (unsigned int i = 0; i < NUM_VOICES;i++) {    
      if ((v[i] = calloc( 1, sizeof(Voice)))) {
          voice_init(v[i], s_rate, r);
          adsr_reset(&v[i]->env[0]);
          adsr_reset(&v[i]->env[1]);
          adsr_reset(&v[i]->env[2]);
          adsr_reset(&v[i]->env[3]);
        }
    }
}

int count = 0;

void voice_lfo(Voice* const v, const float* const params[], const sample_t tempo) {
  const sample_t lfo1_freq    = *(params[V_LF1_FRQ]);
  const sample_t lfo2_freq    = *(params[V_LF2_FRQ]);
  const sample_t lfo3_freq    = *(params[V_LF3_FRQ]);
  const sample_t lfo1_amp     = *(params[V_LF1_AMP]);
  const sample_t lfo2_amp     = *(params[V_LF2_AMP]);
  const sample_t lfo3_amp     = *(params[V_LF3_AMP]);    
  const int lfo1_sync         = f_round(*(params[V_LF1_SYN]));
  const int lfo2_sync         = f_round(*(params[V_LF2_SYN]));
  const int lfo3_sync         = f_round(*(params[V_LF3_SYN]));    
    
  v->lfo1->amp                = lfo1_amp;
  v->lfo2->amp                = lfo2_amp;
  v->lfo3->amp                = lfo3_amp;
    
  v->lfo1->sync               = lfo1_sync;
  v->lfo2->sync               = lfo2_sync;
  v->lfo3->sync               = lfo3_sync;
    
  if (lfo1_sync == 1) {
    const sample_t in_freq = lfo1_freq==0 ? 0.000001 : lfo1_freq;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF1_FRQ].max - parameters[V_LF1_FRQ].min));
    v->lfo1->freq = tempo / tempo_values[idx].factor;
  } else {
    v->lfo1->freq = lfo1_freq;
  }
    
  if (lfo2_sync == 1) {
    const sample_t in_freq = lfo2_freq==0 ? 0.000001 : lfo2_freq;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF2_FRQ].max - parameters[V_LF2_FRQ].min));
    v->lfo2->freq = tempo / tempo_values[idx].factor;
  } else {
    v->lfo2->freq = lfo2_freq;
  }
    
  if (lfo3_sync == 1) {
    const sample_t in_freq = lfo3_freq==0 ? 0.000001 : lfo3_freq;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF3_FRQ].max - parameters[V_LF3_FRQ].min));
    v->lfo3->freq = tempo / tempo_values[idx].factor;
  } else {
    v->lfo3->freq = lfo3_freq;
  }
}

/* 
 * Assign an incoming note to a voice
 */
void voice_update_params(Voice* const v, const float* const p[]) {
  // TODO reorder
  v->filter_mix                   = *(p[V_F_MIX]);
  v->mix                          = *(p[V_OSC_MIX]);
  v->osc_parms[OSC1].amp          = *(p[V_OS1_VOL]);
  v->osc_parms[OSC1].phs_offset   = *(p[V_OS1_WID]);
  v->osc_parms[OSC1].uni_type     = f_round(*(p[V_UNI_TYP]));
  v->osc_parms[OSC1].uni          = f_round(*(p[V_OS1_UNI]));
  v->osc_parms[OSC1].tune         = *(p[V_OS1_TUNE]);
  v->osc_parms[OSC1].wave         = f_round(*(p[V_OS1_SHP]));
  v->osc_parms[OSC1].uni_det      = f_round(*(p[V_OS1_UNI_DET]));
  v->osc_parms[OSC1].uni_mix      = f_round(*(p[V_OS1_UNI_MIX]));
  v->osc_parms[OSC1].distortion   = *(p[V_OS1_DST]);
  v->osc_parms[OSC1].sub_osc      = *(p[V_OS1_SUB]);
    
  v->osc_parms[OSC2].amp          = *(p[V_OS2_VOL]);
  v->osc_parms[OSC2].phs_offset   = *(p[V_OS2_WID]);
  v->osc_parms[OSC2].uni_type     = f_round(*(p[V_UNI_TYP]));
  v->osc_parms[OSC2].uni          = f_round(*(p[V_OS2_UNI]));
  v->osc_parms[OSC2].tune         = *(p[V_OS2_TUNE]);
  v->osc_parms[OSC2].wave         = f_round(*(p[V_OS2_SHP]));
  v->osc_parms[OSC2].uni_det      = f_round(*(p[V_OS2_UNI_DET]));
  v->osc_parms[OSC2].uni_mix      = f_round(*(p[V_OS2_UNI_MIX]));
  v->osc_parms[OSC2].distortion   = *(p[V_OS2_DST]);
  v->osc_parms[OSC2].noise        = *(p[V_OS2_NSE]);
    
  v->mod_cutoff1.key_track        = *(p[V_FL1_TRK]);
  v->mod_cutoff1.mod1             = f_round(*(p[V_FL1_M1]));
  v->mod_cutoff1.mod2             = f_round(*(p[V_FL1_M2]));
  v->mod_cutoff1.mod3             = f_round(*(p[V_FL1_M3]));
    
  v->mod_cutoff2.key_track        = *(p[V_FL2_TRK]);
  v->mod_cutoff2.mod1             = f_round(*(p[V_FL2_M1]));
  v->mod_cutoff2.mod2             = f_round(*(p[V_FL2_M2]));
  v->mod_cutoff2.mod3             = f_round(*(p[V_FL2_M3]));
    
  v->mod_cutoff1.mod1_amt         = *(p[V_FL1_M1A]);
  v->mod_cutoff1.mod2_amt         = *(p[V_FL1_M2A]);
  v->mod_cutoff1.mod3_amt         = *(p[V_FL1_M3A]);
    
  v->mod_cutoff2.mod1_amt         = *(p[V_FL2_M1A]);
  v->mod_cutoff2.mod2_amt         = *(p[V_FL2_M2A]);
  v->mod_cutoff2.mod3_amt         = *(p[V_FL2_M3A]);

  v->osc_parms[OSC1].pwm_mod.mod1 = f_round(*(p[V_OS1_WID_MOD]));
  v->osc_parms[OSC2].pwm_mod.mod1 = f_round(*(p[V_OS2_WID_MOD]));

  v->filter1->cutoff              = *(p[V_FL1_CUT]);
  v->filter1->resonance           = *(p[V_FL1_RES]);
  v->filter2->cutoff              = *(p[V_FL2_CUT]);
  v->filter2->resonance           = *(p[V_FL2_RES]);
}

int debug_amp=0;

Voice* voice_set(sample_t note, const uint8_t vel, Voice* const v, Filter* const filter1, Filter* const filter2,
                 const float** const tables, const float* const p[], const sample_t last_note) {
  Filter*  f1                     = v->filter1;
  Filter*  f2                     = v->filter2;
  sample_t freq                   = 0.0;
  sample_t low_freq               = 0.0;
    
  debug_amp=0;
    
  v->note                         = (sample_t) note;
  v->key_note                     = (sample_t) note;
  v->vel                          = (sample_t) vel;
  v->tuning                       = 0;
  v->debug                        = 0;

  v->panning                      = *(p[V_PAN]);
  v->amp                          = vel/127.0;
  v->freq                         = midi2freq(v->note);
  v->glide                        = *(p[V_GLIDE]);
  v->glide_step                   = f_round(*(p[V_GLIDE]) *v->srate);
  v->key_split                    = f_round(*(p[V_MID_KBD_SPT]));
  v->channel                      = f_round(*(p[V_MID_CHN]));
    
  v->last_note                    = (sample_t) last_note;
  low_freq                        = v->freq;
  v->osc_parms[OSC1].mod.mod1     =  f_round(*(p[V_OS1_WID_MOD]));
  v->osc_parms[OSC2].mod.mod1     =  f_round(*(p[V_OS2_WID_MOD]));
  v->osc_parms[OSC1].mod.mod1     =  f_round(*(p[V_OS1_PIT_M1]));
  v->osc_parms[OSC1].mod.mod2     =  f_round(*(p[V_OS1_PIT_M2]));
  v->osc_parms[OSC1].mod.mod3     =  f_round(*(p[V_OS1_PIT_M3]));
  v->osc_parms[OSC2].mod.mod1     =  f_round(*(p[V_OS2_PIT_M1]));
  v->osc_parms[OSC2].mod.mod2     =  f_round(*(p[V_OS2_PIT_M2]));
  v->osc_parms[OSC2].mod.mod3     =  f_round(*(p[V_OS2_PIT_M3]));
  v->osc_parms[OSC1].mod.mod1_amt = *(p[V_OS1_PIT_M1A]);
  v->osc_parms[OSC1].mod.mod2_amt = *(p[V_OS1_PIT_M2A]);
  v->osc_parms[OSC1].mod.mod3_amt = *(p[V_OS1_PIT_M3A]);
  v->osc_parms[OSC2].mod.mod1_amt = *(p[V_OS2_PIT_M1A]);
  v->osc_parms[OSC2].mod.mod2_amt = *(p[V_OS2_PIT_M2A]);
  v->osc_parms[OSC2].mod.mod3_amt = *(p[V_OS2_PIT_M3A]);
  v->filter1_input                = f_round(*(p[V_FL1_IN]));
  v->filter2_input                = f_round(*(p[V_FL2_IN]));
    
  osc_init(v->lfo1, LFO, *(p[V_LF1_SHP]), v->srate, 
           v->table_len, *(p[V_LF1_AMP]), *(p[V_LF1_FRQ]), 0.0,
           *(p[V_LF1_PHS]), tables);
    
  osc_init(v->lfo2, LFO, *(p[V_LF2_SHP]), v->srate,
           v->table_len, *(p[V_LF2_AMP]), *(p[V_LF2_FRQ]), 0.0,
           *(p[V_LF2_PHS]), tables);
    
  osc_init(v->lfo3, LFO, *(p[V_LF3_SHP]), v->srate,
           v->table_len, *(p[V_LF3_AMP]), *(p[V_LF3_FRQ]), 0.0,
           *(p[V_LF3_PHS]), tables);
    
  osc_init(v->noise, Default, PinkNs, v->srate, 
           v->table_len, 1.0, v->table_len + 1., 0.0,
           0.0, tables);
    
  osc_init(v->sub_osc, BLIT, Square, v->srate,
           v->table_len, 1.0, 220, 0.0,
           0, tables);
    
  // initialize the filter using parent filter values
  filter_init(f1, filter1->filter_type);
  filter_reset(f1,filter1->cutoff,filter1->resonance,1.0, filter1->bandwidth);
    
  filter_init(f2, filter2->filter_type);
  filter_reset(f2,filter2->cutoff,filter2->resonance,1.0, filter2->bandwidth);    
    
  adsr_init(&v->env[0], *(p[V_EN1_ATT]), *(p[V_EN1_DEC]), *(p[V_EN1_SUS]), *(p[V_EN1_REL]), v->srate);
  v->env[0].loop = *(p[V_EN1_TRG]);
  adsr_init(&v->env[1], *(p[V_EN2_ATT]), *(p[V_EN2_DEC]), *(p[V_EN2_SUS]), *(p[V_EN2_REL]), v->srate);
  v->env[1].loop = *(p[V_EN2_TRG]);
  adsr_init(&v->env[2], *(p[V_EN3_ATT]), *(p[V_EN3_DEC]), *(p[V_EN3_SUS]), *(p[V_EN3_REL]), v->srate);
  v->env[2].loop = *(p[V_EN3_TRG]);
  adsr_init(&v->env[3], *(p[V_EN4_ATT]), *(p[V_EN4_DEC]), *(p[V_EN4_SUS]), *(p[V_EN4_REL]), v->srate);
  v->env[3].loop = *(p[V_EN4_TRG]);
  adsr_gate(&v->env[0],1);
  adsr_gate(&v->env[1],1);
  adsr_gate(&v->env[2],1);
  adsr_gate(&v->env[3],1);

  if (last_note == 0.0) {
    v->glide = 0.0;
    v->last_note = v->note;
  }
  if (v->glide == 0.0) v->glide_step = 0;
    
  if (v->glide_step > 0) {
    v->glide_factor = ((sample_t) note - last_note ) / v->glide_step;
    v->note = last_note;
    note = last_note;
  }
    
  for (unsigned int i = 0; i < NUM_OSCS; ++i ) {
    freq = midi2freq(note + v->osc_parms[i].tune);
    //filter_init( v->osc_parms[i].hp_filter, FILTER_HP12);
    //filter_init( v->osc_parms[i].lp_filter, FILTER_LP12);
    if (v->key_split && i == 0 && v->note>59 && v->key_split) continue;
    if (v->key_split && i == 1 && v->note<60 && v->key_split) continue;
        
    for (unsigned int j = 0; j < 32 ; ++j) {
      const sample_t phs = (sample_t) rand() / (sample_t) RAND_MAX;
      osc_init(&v->osc[i][j]
               , BLIT
               , v->osc_parms[i].wave
               , v->srate
               , v->table_len
               , 1.0
               , freq
               , 0.0
               , phs
               , tables);
            
            
      if (j==0) low_freq = freq;
    }
  }
    
  return v;
}

void voice_update(Voice** const voices, GlobalParms* gbl, Filter* const filter1, Filter* const filter2,
                  const float* const params[], const uint32_t len) {
  Voice* v;
  Filter* f1;
  Filter* f2;
  OscParms* osc;
  sample_t freq1 = 0.0, tune_new =0.0, amp_adj = 0.0;
  sample_t unison_shift = 0.0;
  sample_t center_note[2];
  sample_t center_amp[2];
       
  for (unsigned int i = NUM_VOICES; i-- ; ) {
    v = voices[i];
    f1 = v->filter1;
    f2 = v->filter2;
    interpolate(&v->filter1_co, v->filter1_co.value, filter1->cutoff, len);
    interpolate(&v->filter2_co, v->filter2_co.value, filter2->cutoff, len);
    voice_update_params(v, params);
    voice_lfo(v, params, gbl->tempo);
    if ((v->env[0].state == adsr_idle)&&(v->env[1].state == adsr_idle))
      continue;
    // pulse / phase width
        
    for (unsigned int idx = NUM_OSCS; idx--; ) {
            
      if ( idx==0 && v->note>59 && v->key_split) continue;
      if ( idx==1 && v->note<60 && v->key_split) continue;
            
      osc = &v->osc_parms[idx];
            
      tune_new = voice_mod_cutoff(v, &osc->mod, 
                                  midi2freq(v->note + osc->tune + gbl->tuning ),
                                  CENTER_NOTE);
      center_note[idx] = v->note + osc->tune + gbl->tuning;
      const unsigned int det = f_round(osc->uni_det);
      const unsigned int mix = f_round(osc->uni_mix);
      sample_t tot_amp = 0;
            
      const uint8_t unison = osc->uni;
      const sample_t unison_sqr = pow(osc->uni, 0.5);
            
      for (unsigned int j = unison; j--; ) {
        if (unison>1) {
          if (osc->uni_type) {
            sample_t det2 = 0.50 * det / 127.0 ;
            freq1 = tune_new;
            if (j!=0) {
              if (j % 2 == 1) 
                unison_shift = - pow(((j-unison/2.0)/unison * det2),2.0);
              else 
                unison_shift = pow((((j-1)-unison/2.0)/unison * det2),2.0);
            }
          } else {
            if (osc->uni_det == 0) unison_shift = 0;
            else unison_shift = roland_unison_shift(v->r, det, j); 
          }
        } else {
          unison_shift = 0;
          amp_adj = (j == 0 ? v->r->rol_cosc[mix] : v->r->rol_sosc[mix]);
        }
                
        amp_adj = (j == 0 ? v->r->rol_cosc[mix] : v->r->rol_sosc[mix]) / unison_sqr ;
        tot_amp += amp_adj;
        freq1 = tune_new + tune_new * unison_shift;
        if (j==0) center_amp[idx] = amp_adj;              
                
        osc_update(&(v->osc[idx][j]), amp_adj, freq1);
      }
    }    
        
    osc_update(v->sub_osc, center_amp[0], midi2freq(center_note[0]-12)); 
    v->center_note[0] = center_note[0];
    v->center_note[1] = center_note[1];
  }
  debug_amp =1;
}
