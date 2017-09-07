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

#ifndef VOICE_PARAMS_H
#define VOICE_PARAMS_H

#include "types.h"
#include "ladspa-util.h"

#define V_PORT_OFFSET    7
#define V_PARAMS       173
#define V_TEMPO         25
#define V_TEMPO_DLY     18

#define V_MIDI_MAPS     16

#ifdef __cplusplus
extern "C" {
#endif

  static inline float voice_param_value(int port, const double value, const int sync);

  static inline float voice_param_display(int port, double value, char* str_val, char* desc, const int sync);

  typedef enum {
    V_MIDI_IN    = 0
    , V_LEFT_IN 
    , V_RIGHT_IN
    , V_LEFT_OUT
    , V_RIGHT_OUT
    , V_NOTIFY
    , V_CONTROL
    , V_VOLUME
    , V_AMT
    , V_F_MIX
    , V_OSC_MIX
    , V_UNI_TYP
    , V_DLY_FBL
    , V_DLY_FBR
    , V_DLY_TML
    , V_DLY_TMR
    , V_DLY_BPM
    , V_DLY_SPT
    , V_GLIDE
    , V_PLY
    , V_PAN_M1
    , V_PAN
    , V_LOW
    , V_MID
    , V_HGH
    , V_TMP
    , V_OS1_SHP
    , V_OS1_WID_MOD
    , V_OS1_WID
    , V_OS1_VOL
    , V_OS1_TUNE
    , V_OS1_SUB
    , V_OS1_UNI
    , V_OS1_UNI_DET
    , V_OS1_UNI_MIX
    , V_OS1_DST 
    , V_OS1_PIT_M1
    , V_OS1_PIT_M2
    , V_OS1_PIT_M3
    , V_OS1_PIT_M1A
    , V_OS1_PIT_M2A
    , V_OS1_PIT_M3A
    , V_OS2_SHP
    , V_OS2_WID_MOD
    , V_OS2_WID
    , V_OS2_VOL
    , V_OS2_TUNE
    , V_OS2_NSE
    , V_OS2_UNI
    , V_OS2_UNI_DET
    , V_OS2_UNI_MIX
    , V_OS2_DST
    , V_OS2_PIT_M1
    , V_OS2_PIT_M2
    , V_OS2_PIT_M3
    , V_OS2_PIT_M1A
    , V_OS2_PIT_M2A
    , V_OS2_PIT_M3A
    , V_FL1_CUT
    , V_FL1_RES
    , V_FL1_WID
    , V_FL1_TYP
    , V_FL1_IN
    , V_FL1_TRG
    , V_FL1_TRK
    , V_FL1_M1
    , V_FL1_M2
    , V_FL1_M3
    , V_FL1_M1A
    , V_FL1_M2A
    , V_FL1_M3A
    , V_FL2_CUT
    , V_FL2_RES
    , V_FL2_WID
    , V_FL2_TYP
    , V_FL2_IN       
    , V_FL2_TRG
    , V_FL2_TRK
    , V_FL2_M1
    , V_FL2_M2
    , V_FL2_M3
    , V_FL2_M1A
    , V_FL2_M2A
    , V_FL2_M3A
    , V_FL3_CUT
    , V_FL3_RES
    , V_FL3_WID
    , V_FL3_TYP
    , V_FL3_IN
    , V_FL3_TRG
    , V_FL3_M1
    , V_FL3_M2
    , V_FL3_M3
    , V_FL3_M1A
    , V_FL3_M2A
    , V_FL3_M3A
    , V_FL4_CUT
    , V_FL4_RES
    , V_FL4_WID
    , V_FL4_TYP
    , V_FL4_IN
    , V_FL4_TRG
    , V_FL4_M1
    , V_FL4_M2
    , V_FL4_M3
    , V_FL4_M1A
    , V_FL4_M2A
    , V_FL4_M3A
    , V_LF1_FRQ
    , V_LF1_AMP
    , V_LF1_SHP
    , V_LF1_PHS
    , V_LF1_SYN
    , V_LF2_FRQ
    , V_LF2_AMP
    , V_LF2_SHP
    , V_LF2_PHS
    , V_LF2_SYN
    , V_LF3_FRQ
    , V_LF3_AMP
    , V_LF3_SHP
    , V_LF3_PHS
    , V_LF3_SYN
    , V_EN1_ATT   
    , V_EN1_DEC
    , V_EN1_SUS
    , V_EN1_REL
    , V_EN1_TRG
    , V_EN2_ATT
    , V_EN2_DEC
    , V_EN2_SUS
    , V_EN2_REL
    , V_EN2_TRG
    , V_EN3_ATT
    , V_EN3_DEC
    , V_EN3_SUS
    , V_EN3_REL
    , V_EN3_TRG
    , V_EN4_ATT
    , V_EN4_DEC
    , V_EN4_SUS
    , V_EN4_REL
    , V_EN4_TRG
    , V_NOT_OFF
    , V_BL1
    , V_BL2
    , V_BL3
    , V_BL4
    , V_BL5
    , V_BL6
    , V_BL7
    , V_BL8
    , V_BL9
    , V_MID_ON
    , V_MID_CHN
    , V_MID_KBD_SPT
    , V_LAST_PORT
    , V_MID_L01
    , V_MID_L02
    , V_MID_L03
    , V_MID_L04
    , V_MID_L05
    , V_MID_L06
    , V_MID_L07
    , V_MID_L08
    , V_MID_L09
    , V_MID_L10
    , V_MID_L11
    , V_MID_L12
    , V_MID_L13
    , V_MID_L14
    , V_MID_L15
    , V_MID_L16
       
  } ParamIndex;


  typedef struct __VoiceParams {
    const int       index;
    const bool      can_map;
    const bool      can_display;
    const sample_t  min;
    const sample_t  max;
    const sample_t  scale;
    const bool      round;
    const bool      exp;
    const char      format[16];
    const char      description[32];
    const char**    display_values;
  } VoiceParams;

  typedef struct __TempoParams {
    const char*     description;
    const sample_t  factor;
  } TempoParams;

  static const char *mod_values[16] = {
    "OFF" ,
    "LFO1",
    "LFO2",
    "LFO3",
    "ENV1",
    "ENV2",
    "ENV3",
    "ENV4",
    "ENV3 INVERT",
    "ENV4 INVERT",
    "LFO1 MULT",
    "LFO2 MULT",
    "LFO3 MULT",
    "ENV3 MULT",
    "ENV4 MULT",
    0 
  }; 
    
  static const char *wave_values[8] = {
    "Sine" ,
    "Tri" ,
    "Saw" ,
    "Rev Saw" ,
    "Sqr" ,
    "Stair" ,
    "Pulse" ,
    0  }; 
       
  static const char *filter_values[10] = {
    "Bypass" ,
    "LP12" ,
    "HP12" ,
    "BP12" ,
    "LP24" ,
    "HP24" ,
    "BP24" ,
    "HP6" ,
    "BP6" ,
    0 }; 
       
  static const char *env_values[3] = {
    "Poly" ,
    "Loop" ,
    0 }; 
      
  static const char *detune_values[3] = {
    "Roland" ,
    "Exponential" ,
    0 }; 
      
  static const char *on_off_values[3] = {
    "Off" ,
    "On" ,
    0 }; 
      
  static const char *finput_values[6] = {
    "Bypass",
    "OSC1+2",
    "OSC1",
    "OSC2",
    "FILT",
    0 };
  static const char *chn_values[18] = {
    "All" ,
    "1" ,
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    0 
  }; 

  /*
    Half note                 = BPM / 120
    Quarter note              = BPM / 60
    Eighth note               = BPM / 30
    Sixteenth note            = BPM / 15
    Dotted-quarter note       = BPM / 90
    Dotted-eighth note        = BPM / 45
    Dotted-sixteenth note     = BPM / 22.5
    Triplet-quarter note      = BPM / 40
    Triplet-eighth note       = BPM / 20
    Triplet-sixteenth note    = BPM / 10
  */
    
  static const TempoParams tempo_values[V_TEMPO] = {
    { "8/1",   1920  },
    { "7/1",   1680  },
    { "6/1",   1440  },
    { "5/1",   1200  },
    { "4/1",   960   },
    { "3/1",   720   },
    { "2/1",   480   },
    { "1/1",   240   },
    { "1/2.",  180   } ,
    { "1/2",   120   } , 
    { "1/4.",  90    } ,
    { "1/2t",  75.0    } ,
    { "1/4",   60    } ,
    { "1/8.",  45.0    } ,
    { "1/4t",  40    } ,
    { "1/8",   30    } ,
    { "1/16.", 22.5    } ,
    { "1/8t",  20    } ,
    { "1/16",  15.0    } ,
    { "1/32.", 11.25   } ,
    { "1/16t", 10    } ,
    { "1/32",  7.5     } ,
    { "1/64.", 5.625   } ,
    { "1/32t", 5.0     } ,
    { "1/64",  3.75    } 
  }; 

  static const TempoParams delay_values[V_TEMPO_DLY] = {
    { "1/64",  3.75    } ,
    { "1/32t", 5.0     } ,
    { "1/64.", 5.625   } ,
    { "1/32",  7.5     } ,
    { "1/16t", 10    } ,
    { "1/32.", 11.25   } ,
    { "1/16",  15.0    } ,
    { "1/8t",  20    } ,
    { "1/16.", 22.5    } ,
    { "1/8",   30    } ,
    { "1/4t",  40    } ,
    { "1/8.",  45.0    } ,
    { "1/4",   60    } ,
    { "1/2t",  75.0    } ,
    { "1/4.",  90    } ,
    { "1/2",   120   } , 
    { "1/2.",  180   } ,
    { "1/1",   240   }
  }; 

  static const VoiceParams parameters[V_PARAMS] = {
    { V_MIDI_IN        , false, false, 0,   1.0, 100, false, false, "", "", 0}    
    , { V_LEFT_IN        , false, false, 0,   1.0, 100, false, false, "", "", 0}
    , { V_RIGHT_IN       , false, false, 0,   1.0, 100, false, false, "", "", 0}
    , { V_LEFT_OUT       , false, false, 0,   1.0, 100, false, false, "", "", 0}
    , { V_RIGHT_OUT      , false, false, 0,   1.0, 100, false, false, "", "", 0}
    , { V_NOTIFY         , false, false, 0,   1.0, 100, false, false, "", "", 0}
    , { V_CONTROL        ,  true,  true, 0,   1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_VOLUME         ,  true,  true, 0,   1.0, 100, false, false, "%.2f%%", "Master Volume", 0}
    , { V_AMT            ,  true,  true, -70, 6.0, 100, false, false, "%.2f%%", "Db Out", 0}
    , { V_F_MIX          ,  true,  true, 0,  1.0, 1.0,      false, false, "%.2f", "Filter mix", 0}
    , { V_OSC_MIX        ,  true,  true, 0,  1.0, 1.0,      false, false, "%.2f", "Oscillator mix", 0}
    , { V_UNI_TYP        ,  true,  true, 0,   1.0, 1.0,  true, false, "%s", "Unison method", detune_values}
    , { V_DLY_FBL        ,  true,  true, 0,   1.0, 100, false, false, "%.2f%%", "Delay feedback left ", 0}
    , { V_DLY_FBR        ,  true,  true, 0,   1.0, 100, false, false, "%.2f%%", "Delay feedback right", 0}
    , { V_DLY_TML        ,  true,  true, 0,   1.0, 1.0, false, false, "%.2fs", "Delay time left", 0}
    , { V_DLY_TMR        ,  true,  true, 0,   1.0, 1.0, false, false, "%.2fs", "Delay time right", 0}
    , { V_DLY_BPM        ,  true,  true, 0,   1.0, 1.0, false, false, "%.0f", "Delay Sync", 0}
    , { V_DLY_SPT        ,  true,  true, 0,   1.0, 1.0, false, false, "%.0f", "Delay Split", 0}
    , { V_GLIDE          ,  true,  true, 0,   1.0, 1.0, false, false, "%.2fs", "Glide", 0}
    , { V_PLY            ,  true,  true, 1.0,   16.0, 1.0,  true, false, "%.0f", "Polyphony", 0}
    , { V_PAN_M1         ,  true,  true, 0,  15.0, 1.0, false, false, "%s", "Panning mod", mod_values}
    , { V_PAN            ,  true,  true, -0.5,   0.5, 100, false, false, "%.2f", "Panning", 0}
    , { V_LOW            ,  true,  true, -70, 6.0, 1.0, false, false, "%.2fdB", "EQ Low", 0}
    , { V_MID            ,  true,  true, -70, 6.0, 1.0, false, false, "%.2fdB", "EQ Mid", 0}
    , { V_HGH            ,  true,  true, -70, 6.0, 1.0, false, false, "%.2fdB", "EQ High", 0}
    , { V_TMP            ,  true,  true, 1.0, 256.0, 1.0, false, false, "%.0f BPM", "Tempo", 0}
    , { V_OS1_SHP        ,  true,  true, 0,   6.0, 1.0,  true, false, "%s", "OSC 1 wave shape", wave_values }
    , { V_OS1_WID_MOD    ,  true,  true, 0, 11.0, 1.0,  true, false, "%s", "OSC 1 pulse width mod", mod_values }
    , { V_OS1_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 1 Pulse width", 0}
    , { V_OS1_VOL        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 1 volume", 0}
    , { V_OS1_TUNE       ,  true,  true, -24.0, 24.0, 1.0, false, false, "%.2fst", "OSC 1 fine-tune", 0}
    , { V_OS1_SUB        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 1 sub volume", 0}
    , { V_OS1_UNI        ,  true,  true, 1.0, 16.0, 1.0,  true, false, "%0.f", "OSC 1 unison", 0}
    , { V_OS1_UNI_DET    ,  true,  true, 0, 127.0, 1.0,  true, false, "%0.f", "OSC 1 detune", 0}
    , { V_OS1_UNI_MIX    ,  true,  true, 0, 127.0, 1.0,  true, false, "%0.f", "OSC 1 unsion mix", 0}
    , { V_OS1_DST        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 1 distortion", 0}
    , { V_OS1_PIT_M1     ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "OSC 1 pitch mod 1", mod_values }
    , { V_OS1_PIT_M2     ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "OSC 1 pitch mod 2", mod_values }
    , { V_OS1_PIT_M3     ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "OSC 1 pitch mod 3", mod_values }
    , { V_OS1_PIT_M1A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 1 pitch mod 1 amt", 0}
    , { V_OS1_PIT_M2A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 1 pitch mod 2 amt", 0}
    , { V_OS1_PIT_M3A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 1 pitch mod 3 amt", 0}
    , { V_OS2_SHP        ,  true,  true, 0, 6.0, 1.0,  true, false, "%s", "OSC 2 wave shape", wave_values }
    , { V_OS2_WID_MOD    ,  true,  true, 0, 11.0, 1.0,  true, false, "%s", "OSC 2 pulse width mod", mod_values }
    , { V_OS2_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 2 Pulse width", 0}
    , { V_OS2_VOL        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 2 volume", 0}
    , { V_OS2_TUNE       ,  true,  true, -24.0, 24.0, 1.0, false, false, "%.2fst", "OSC 2 fine-tune", 0}
    , { V_OS2_NSE        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 2 noise volume", 0}
    , { V_OS2_UNI        ,  true,  true,1.0,  16.0, 1.0, false, false, "%0.f", "OSC 2 unison", 0}
    , { V_OS2_UNI_DET    ,  true,  true, 0, 127.0, 1.0,  true, false, "%.0f", "OSC 2 detune", 0}
    , { V_OS2_UNI_MIX    ,  true,  true, 0, 127.0, 1.0, false, false, "%.0f", "OSC 2 unsion mix", 0}
    , { V_OS2_DST        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "OSC 2 distortion", 0}
    , { V_OS2_PIT_M1     ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "OSC 2 pitch mod 1", mod_values }
    , { V_OS2_PIT_M2     ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "OSC 2 pitch mod 2", mod_values }
    , { V_OS2_PIT_M3     ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "OSC 2 pitch mod 3", mod_values }
    , { V_OS2_PIT_M1A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 2 pitch mod 1 amt", 0}
    , { V_OS2_PIT_M2A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 2 pitch mod 2 amt", 0}
    , { V_OS2_PIT_M3A    ,  true,  true, 0, 64.0, 64.0, false, true, "%.2fst", "OSC 2 pitch mod 3 amt", 0}
    , { V_FL1_CUT        ,  true,  true, 20, 20000, 1.0, false, true, "%.0fHz", "Filter 1 cutoff", 0}
    , { V_FL1_RES        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 resonance", 0}
    , { V_FL1_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 width", 0}
    , { V_FL1_TYP        ,  true,  true, 0, 7.0, 1.0, false, false, "%s", "Filter 1 type", filter_values}
    , { V_FL1_IN         ,  true,  true, 0, 1.0, 1.0, false, false, "%.0f", "Filter 1 input", 0}
    , { V_FL1_TRG        ,  true,  true, 0, 1.0, 1.0, false, false, "%.0f", "Filter 1 trigger", 0}
    , { V_FL1_TRK        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 key tracking", 0}
    , { V_FL1_M1         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 1 freq mod 1", mod_values }
    , { V_FL1_M2         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 1 freq mod 2", mod_values }
    , { V_FL1_M3         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 1 freq mod 3", mod_values }
    , { V_FL1_M1A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 freq mod 1 amt", 0}
    , { V_FL1_M2A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 freq mod 2 amt", 0}
    , { V_FL1_M3A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 1 freq mod 3 amt", 0}
    , { V_FL2_CUT        ,  true,  true, 20, 20000, 1.0, false, true, "%.0fHz", "Filter 2 cutoff", 0}
    , { V_FL2_RES        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 resonance", 0}
    , { V_FL2_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 width", 0}
    , { V_FL2_TYP        ,  true,  true, 0, 7.0, 1.0, false, false, "%s", "Filter 2 type", filter_values}
    , { V_FL2_IN         ,  true,  true, 0, 1.0, 1.0, false, false, "%.0f", "Filter 2 input", 0}
    , { V_FL2_TRG        ,  true,  true, 0, 1.0, 1.0, false, false, "%.0f", "Filter 2 trigger", 0}
    , { V_FL2_TRK        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 key tracking", 0}
    , { V_FL2_M1         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 2 freq mod 1", mod_values }
    , { V_FL2_M2         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 2 freq mod 2", mod_values }
    , { V_FL2_M3         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 2 freq mod 3", mod_values }
    , { V_FL2_M1A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 freq mod 1 amt", 0}
    , { V_FL2_M2A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 freq mod 2 amt", 0}
    , { V_FL2_M3A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 2 freq mod 3 amt", 0}
    , { V_FL3_CUT        ,  true,  true, 20, 20000, 1.0, false, true, "%.0fHz", "Filter 3 cutoff", 0}
    , { V_FL3_RES        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 resonance", 0}
    , { V_FL3_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 width", 0}
    , { V_FL3_TYP        ,  true,  true, 0, 7.0, 1.0, false, false, "%s", "Filter 3 type", filter_values}
    , { V_FL3_IN         ,  true,  true, 0, 3.0, 1.0,  true, false, "%s", "Filter 3 input", finput_values}
    , { V_FL3_TRG        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 trigger", 0}
    , { V_FL3_M1         ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "Filter 3 freq mod 1", mod_values }
    , { V_FL3_M2         ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "Filter 3 freq mod 2", mod_values }
    , { V_FL3_M3         ,  true,  true, 0, 15.0, 1.0,  true, false, "%s", "Filter 3 freq mod 3", mod_values }
    , { V_FL3_M1A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 freq mod 1 amt", 0}
    , { V_FL3_M2A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 freq mod 2 amt", 0}
    , { V_FL3_M3A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 3 freq mod 3 amt", 0}
    , { V_FL4_CUT        ,  true,  true, 20, 20000, 1.0, false, true, "%.0fHz", "Filter 4 cutoff", 0}
    , { V_FL4_RES        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 4 resonance", 0}
    , { V_FL4_WID        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 4 width", 0}
    , { V_FL4_TYP        ,  true,  true, 0, 7.0, 1.0, false, false, "%s", "Filter 4 type", filter_values}
    , { V_FL4_IN         ,  true,  true, 0, 5.0, 1.0,  true, false, "%s", "Filter 4 input", finput_values}
    , { V_FL4_TRG        ,  true,  true, 0, 1.0, 1.0,  true, false, "%.0f", "Filter 4 trigger", 0}
    , { V_FL4_M1         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 4 freq mod 1", mod_values }
    , { V_FL4_M2         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 4 freq mod 2", mod_values }
    , { V_FL4_M3         ,  true,  true, 0, 15.0, 1.0, false, false, "%s", "Filter 4 freq mod 3", mod_values }
    , { V_FL4_M1A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 4 freq mod 1 amt ", 0}
    , { V_FL4_M2A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 4 freq mod 2 amt", 0}
    , { V_FL4_M3A        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "Filter 4 freq mod 3 amt", 0}
    , { V_LF1_FRQ        ,  true,  true, 0, 20, 1.0, false,  true, "%.2fHz", "LFO 1 Rate", 0}
    , { V_LF1_AMP        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "LFO 1 Amplitude", 0}
    , { V_LF1_SHP        ,  true,  true, 0, 6.0, 1.0,  true, false, "%s", "LFO 1 Shape", wave_values }
    , { V_LF1_PHS        ,  true,  true, 0, 1.0, 100, false, false, "%.0f", "LFO 1 Phase", 0}
    , { V_LF1_SYN        ,  true,  true, 0, 1.0, 1.0, false, false, "%s", "LFO 1 Sync", on_off_values}
    , { V_LF2_FRQ        ,  true,  true, 0, 20, 1.0, false,  true, "%.2fHz", "LFO 2 Rate", 0}
    , { V_LF2_AMP        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "LFO 2 Amplitude", 0}
    , { V_LF2_SHP        ,  true,  true, 0, 6.0, 1.0,  true, false, "%s", "LFO 2 Shape", wave_values }
    , { V_LF2_PHS        ,  true,  true, 0, 1.0, 100, false, false, "%.0f", "LFO 2 Phase", 0}
    , { V_LF2_SYN        ,  true,  true, 0, 1.0, 1.0, false, false, "%s", "LFO 2 Sync", on_off_values}
    , { V_LF3_FRQ        ,  true,  true, 0, 20, 1.0, false,  true, "%.2fHz", "LFO 3 Rate", 0}
    , { V_LF3_AMP        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "LFO 3 Amplitude", 0}
    , { V_LF3_SHP        ,  true,  true, 0, 6.0, 1.0,  true, false, "%s", "LFO 3 Shape", wave_values }
    , { V_LF3_PHS        ,  true,  true, 0, 1.0, 100, false, false, "%.0f", "LFO 3 Phase", 0}
    , { V_LF3_SYN        ,  true,  true, 0, 1.0, 1.0, false, false, "%s", "LFO 3 Sync", on_off_values}
    , { V_EN1_ATT        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 1 Attack", 0}
    , { V_EN1_DEC        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 1 Decay", 0}
    , { V_EN1_SUS        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "ENV 1 Sustain", 0}
    , { V_EN1_REL        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 1 Release", 0}
    , { V_EN1_TRG        ,  true,  true, 0, 1.0, 1.0, false, false, "%s", "ENV 1 Trigger", env_values}
    , { V_EN2_ATT        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 2 Attack", 0}
    , { V_EN2_DEC        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 2 Decay", 0}
    , { V_EN2_SUS        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "ENV 2 Sustain", 0}
    , { V_EN2_REL        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 2 Release", 0}
    , { V_EN2_TRG        ,  true,  true, 0, 1.0, 1.0,  true, false, "%s", "ENV 2 Trigger", env_values}
    , { V_EN3_ATT        ,  true,  true, 0, 10, 1.0, false, true, "%.3fs", "ENV 3 Attack", 0}
    , { V_EN3_DEC        ,  true,  true, 0, 10, 1.0, false, true, "%.3fs", "ENV 3 Decay", 0}
    , { V_EN3_SUS        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "ENV 3 Sustain", 0}
    , { V_EN3_REL        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 3 Release", 0}
    , { V_EN3_TRG        ,  true,  true, 0, 1.0, 1.0,  true, false, "%s", "ENV 3 Trigger", env_values}
    , { V_EN4_ATT        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 4 Attack", 0}
    , { V_EN4_DEC        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 4 Decay", 0}
    , { V_EN4_SUS        ,  true,  true, 0, 1.0, 100, false, false, "%.2f%%", "ENV 4 Sustain", 0}
    , { V_EN4_REL        ,  true,  true, 0, 10, 1.0, false,  true, "%.3fs", "ENV 4 Release", 0}
    , { V_EN4_TRG        ,  true,  true, 0, 1.0, 1.0,  true, false, "%s", "ENV 4 Trigger", env_values}
    , { V_NOT_OFF        ,  true, false, 0, 1.0, 1.0, true, false, "%.0f", "All notes off", 0}
    , { V_BL1            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL2            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL3            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL4            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL5            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL6            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL7            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL8            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_BL9            ,  true, false, 0, 1.0, 100, false, false, "%.2f%%", "", 0}
    , { V_MID_ON         ,  true, false, 0, 1.0, 1.0,  true, false, "%s", "MIDI learn", on_off_values}
    , { V_MID_CHN        ,  true,  true, 0, 16.0, 1.0,  true, false, "%s", "MIDI Channel", chn_values}
    , { V_MID_KBD_SPT    ,  true,  true, 0, 1.0,  1.0,  true, false, "%s", "Keyboard split", on_off_values}
    , { V_LAST_PORT      ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L01        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L02        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L03        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L04        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L05        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L06        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L07        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L08        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L09        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L10        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L11        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L12        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L13        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L14        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L15        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
    , { V_MID_L16        ,  true, false, 0, 150, 1.0,  true, false, "%.0f", "", 0}
  };

  static inline float voice_param_value(int port, double value, const int sync) {
    if ((port < 0) || (port > V_PARAMS )) {
      printf("Port error: %d\n", port);
      return 0;
    }
    //float val = value * (float) parameters[port].scale;
    if (parameters[port].round) value = f_round(value);
    
    if ((parameters[port].exp)&&(sync==0)) {
      value = (float) ((parameters[port].min+1) * 
                       pow( (parameters[port].max+1)/(parameters[port].min+1)
                            , (value / parameters[port].max)) - 1); 
    }
    
    return (float) value;
  }

  static inline float voice_param_normalize(int port, double value) {
    if ((port < 0) || (port > V_PARAMS )) {
      printf("Port error: %d\n", port);
      return 0;
    }
    value = value * parameters[port].scale;

    if (parameters[port].round) value = f_round(value);
    if (parameters[port].exp) {
      value = (float)(parameters[port].max 
                      * log(((value + 1) / (parameters[port].min+1))) 
                      / log((parameters[port].max+1)/(parameters[port].min+1))); 
    }
   
    return value;
  }

  static inline float voice_param_display(int port, const double value, char* str_val, char* desc, const int sync) {
    if ((port < 0) || (port > V_PARAMS )) {
      printf("Port error: %d\n", port);
      return 0;
    }
    
    char s[128];
    char t[128];
    // scale for display
    float val = value * parameters[port].scale;
    
    if ((parameters[port].display_values == NULL)&&(sync==0)) {
      snprintf( s, 128, parameters[port].format, val);
    } else {
      if (sync == 0) {
        snprintf( s, 128, parameters[port].format, parameters[port].display_values[f_round(val)]);
      } else {
        // use tempo parameters if parameter is a rate synced to BPM
        int idx = 0;
        if (port == V_DLY_TML || port == V_DLY_TMR) {
          idx = f_round((V_TEMPO_DLY - 1) * value 
                        / (parameters[port].max - parameters[port].min));
          snprintf( s, sizeof(delay_values[idx].description), "%s", delay_values[idx].description);
        } else {
          idx = f_round((V_TEMPO-1) * value 
                        / (parameters[port].max - parameters[port].min));
          snprintf( s, sizeof(tempo_values[idx].description), "%s", tempo_values[idx].description);
        }
            
      }
    }
    
    snprintf( t, 128, "%s ", parameters[port].description);
    desc = (char*) &t;
    str_val = (char*) &s;
    return val;
  }

#ifdef __cplusplus
}
#endif


#endif
