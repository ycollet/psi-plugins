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

#ifndef MIDI_MAP_H
#define MIDI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define MIDI_CHANNEL_OMNI       0
#define MIDI_CHANNEL_OFF        17 // and over

#define MIDI_PITCHBEND_MIN      -8192
#define MIDI_PITCHBEND_MAX      8191

    
  /*typedef uint8_t byte;

    typedef byte StatusByte;
    typedef byte DataByte;
    typedef byte Channel;
    typedef byte FilterMode;
  */

  enum MidiType {
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
  };

  /*! \brief Enumeration of Control Change command numbers.
    See the detailed controllers numbers & description here:
    http://www.somascape.org/midi/tech/spec.html#ctrlnums
  */
  enum MidiControlChangeNumber {
    // High resolution Continuous Controllers MSB (+32 for LSB) ----------------
    BankSelect                  = 0,
    ModulationWheel             = 1,
    BreathController            = 2,
    // CC3 undefined
    FootController              = 4,
    PortamentoTime              = 5,
    DataEntry                   = 6,
    ChannelVolume               = 7,
    Balance                     = 8,
    // CC9 undefined
    Pan                         = 10,
    ExpressionController        = 11,
    EffectControl1              = 12,
    EffectControl2              = 13,
    // CC14 undefined
    // CC15 undefined
    GeneralPurposeController1   = 16,
    GeneralPurposeController2   = 17,
    GeneralPurposeController3   = 18,
    GeneralPurposeController4   = 19,

    // Switches ----------------------------------------------------------------
    Sustain                     = 64,
    Portamento                  = 65,
    Sostenuto                   = 66,
    SoftPedal                   = 67,
    Legato                      = 68,
    Hold                        = 69,

    // Low resolution continuous controllers -----------------------------------
    SoundController1            = 70,   ///< Synth: Sound Variation   FX: Exciter On/Off
    SoundController2            = 71,   ///< Synth: Harmonic Content  FX: Compressor On/Off
    SoundController3            = 72,   ///< Synth: Release Time      FX: Distortion On/Off
    SoundController4            = 73,   ///< Synth: Attack Time       FX: EQ On/Off
    SoundController5            = 74,   ///< Synth: Brightness        FX: Expander On/Off
    SoundController6            = 75,   ///< Synth: Decay Time        FX: Reverb On/Off
    SoundController7            = 76,   ///< Synth: Vibrato Rate      FX: Delay On/Off
    SoundController8            = 77,   ///< Synth: Vibrato Depth     FX: Pitch Transpose On/Off
    SoundController9            = 78,   ///< Synth: Vibrato Delay     FX: Flange/Chorus On/Off
    SoundController10           = 79,   ///< Synth: Undefined         FX: Special Effects On/Off
    GeneralPurposeController5   = 80,
    GeneralPurposeController6   = 81,
    GeneralPurposeController7   = 82,
    GeneralPurposeController8   = 83,
    PortamentoControl           = 84,
    // CC85 to CC90 undefined
    Effects1                    = 91,   ///< Reverb send level
    Effects2                    = 92,   ///< Tremolo depth
    Effects3                    = 93,   ///< Chorus send level
    Effects4                    = 94,   ///< Celeste depth
    Effects5                    = 95,   ///< Phaser depth

    // Channel Mode messages ---------------------------------------------------
    AllSoundOff                 = 120,
    ResetAllControllers         = 121,
    LocalControl                = 122,
    AllNotesOff                 = 123,
    OmniModeOff                 = 124,
    OmniModeOn                  = 125,
    MonoModeOn                  = 126,
    PolyModeOn                  = 127
  };

  static const char *midi_cc_values[128] = {
    "BankSelect"
    , "Modulation Wheel"
    , "Breath Controller"                        // 3
    , ""
    , "Foot Controller"
    , "Portamento Time"
    , "Data Entry"
    , "Channel Volume"
    , "Balance"
    // 9
    , ""
    , "Pan"
    , "Expression Controller"
    , "Effect Control 1"
    , "Effect Control 2"
    , "", "" // 14 - 15
    , "General Controller 1"
    , "General Controller 2"
    , "General Controller 3"
    , "General Controller 4"
    , "" ,"" ,"" ,"", "", "", "", "", "", ""    // 20 - 63 LSB etc
    , "" ,"" ,"" ,"", "", "", "", "", "", ""
    , "" ,"" ,"" ,"", "", "", "", "", "", ""
    , "" ,"" ,"" ,"", "", "", "", "", "", ""
    , "", "", "", "" 
    , "Sustain"
    , "Portamento"
    , "Sostenuto"
    , "Soft Pedal"
    , "Legato"
    , "Hold"
    , "Sound Controller 1"
    , "Sound Controller 2"
    , "Sound Controller 3"
    , "Sound Controller 4"
    , "Sound Controller 5"
    , "Sound Controller 6"
    , "Sound Controller 7"
    , "Sound Controller 8"
    , "Sound Controller 9"
    , "Sound Controller 10"
    , "General Controller 5"
    , "General Controller 6"
    , "General Controller 7"
    , "General Controller 8"
    , "Portamento Control"
    , "" ,"" ,"" ,"", "", ""    // 85 - 90
    , "Effects 1"
    , "Effects 2"
    , "Effects 3"
    , "Effects 4"
    , "Effects 5"
    , "" ,"" ,"" ,"", ""        // 95 - 119
    , "" ,"" ,"" ,"", "", "", "", "", "", ""
    , "" ,"" ,"" ,"", "", "", "", "", ""
    , "All Sound Off"
    , "Reset All Controllers"
    , "Local Control"
    , "All Notes Off"
    , "Omni Mode Off"
    , "Omni Mode On"
    , "Mono Mode On"
    , "Poly Mode On"
  }; 
    
  static const char *midi_cc_display[128] = {
    "0: Bank Select"
    , "  1: Mod Wheel"
    , "  2: Breath"
    // 3
    , "  3: "
    , "  4: Foot Controller"
    , "  5: Portamento Time"
    , "  6: Data Entry"
    , "  7: Channel Volume"
    , "  8: Balance"
    // 9
    , "  9: "
    , " 10: Pan"
    , " 11: Expression"
    , " 12: Effect 1"
    , " 13: Effect 2"
    // 14 - 15
    , " 14: ", " 15: "
    , " 16: General Controller 1"
    , " 17: General Controller 2"
    , " 18: General Controller 3"
    , " 19: General Controller 4"
    // 20 - 64 LSB etc
    , " 20: " , " 21: " , " 22: " , " 23: ", " 24: ", " 25: ", " 26: ", " 27: ", " 28: ", " 29: "
    , " 30: " , " 31: " , " 32: " , " 33: ", " 34: ", " 35: ", " 36: ", " 37: ", " 38: ", " 39: "
    , " 40: " , " 41: " , " 42: " , " 43: ", " 44: ", " 45: ", " 46: ", " 47: ", " 48: ", " 49: "
    , " 50: " , " 51: " , " 52: " , " 53: ", " 54: ", " 55: ", " 56: ", " 57: ", " 58: ", " 59: "
    , " 60: ", " 61: ", " 62: ", " 63: " 
    , " 64: Sustain"
    , " 65: Portamento"
    , " 66: Sostenuto"
    , " 67: Soft Pedal"
    , " 68: Legato"
    , " 69: Hold"
    , " 70: Sound Controller 1"
    , " 71: Sound Controller 2"
    , " 72: Sound Controller 3"
    , " 73: Sound Controller 4"
    , " 74: Sound Controller 5"
    , " 75: Sound Controller 6"
    , " 76: Sound Controller 7"
    , " 77: Sound Controller 8"
    , " 78: Sound Controller 9"
    , " 79: Sound Controller 10"
    , " 80: General Controller 5"
    , " 81: General Controller 6"
    , " 82: General Controller 7"
    , " 83: General Controller 8"
    , " 84: Portamento Control"
    // 85 - 90
    , " 85: " , " 86: " , " 87: " , " 88: ", " 89: ", " 90: "
    , " 91: Effects 1"
    , " 92: Effects 2"
    , " 93: Effects 3"
    , " 94: Effects 4"
    , " 95: Effects 5"
    // 95 - 119
    , " 96: " , " 97: " , " 98: " , " 99: ", "100: "
    , "101: " , "102: " , "103: " , "104: ", "105: ", "106: ", "107: ", "108: ", "109: ", "110: "
    , "111: " , "112: " , "113: " , "114: ", "115: ", "116: ", "117: ", "118: ", "119: "
    , "120: All Sound Off"
    , "121: Reset All Controllers"
    , "122: Local Control"
    , "123: All Notes Off"
    , "124: Omni Mode Off"
    , "125: Omni Mode On"
    , "126: Mono Mode On"
    , "127: Poly Mode On"
  }; 

#ifdef __cplusplus
}
#endif
#endif  // MIDI_MAP_H
