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

#ifndef SUPER_WELLE_H
#define SUPER_WELLE_H

#include <math.h>

#undef Bool
#define SW_URI "http://bjones.it/psi-plugins/super_welle"
#define SW_UI_URI "http://bjones.it/psi-plugins/super_welle/ui"

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/log/log.h"
#include "lv2/lv2plug.in/ns/ext/log/logger.h"
#include "lv2/lv2plug.in/ns/ext/time/time.h"

#include "types.h"
#include "synth.h"
#include "filter.h"

#define ENV_TR 0.0001f
#define MAX_DLY 48000 
#define PEAK_FRAMES 20

#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define HOLD    4
#define CLOSING 5

typedef struct {
  LV2_URID atom_Blank;
  LV2_URID atom_Object;
  LV2_URID atom_Vector;
  LV2_URID atom_Float;
  LV2_URID atom_Int;
  LV2_URID atom_eventTransfer;
  LV2_URID atom_Sequence;
  LV2_URID time_Position;
  LV2_URID time_barBeat;
  LV2_URID time_beatsPerMinute;
  LV2_URID time_speed;
  LV2_URID time_frame;
  LV2_URID time_fps;
        
  LV2_URID UIData;
  LV2_URID midi_MidiEvent;
  LV2_URID ui_portNumber;
  LV2_URID ui_portValue;
} SW_LV2_URIS;
    
typedef struct _SuperWelle {
  // lv2 parameters
  float    *left_in;
  float    *right_in;
  float    *left_out;
  float    *right_out;
  const LV2_Atom_Sequence* midi_in;
  LV2_Atom_Sequence*       notify;
  const LV2_Atom_Sequence* control;
    
    
  SWSynth* synth;
  float* params[V_PARAMS];    
  sample_t fs;
  unsigned n_active_notes;
  Delay*   delay;
    
  SW_LV2_URIS uris;
  LV2_URID_Map* map;
  LV2_Atom_Forge forge;
  LV2_Atom_Forge_Frame frame;
    
  LV2_Log_Log*   log;
  LV2_Log_Logger logger;    
    
} SuperWelle;

static inline void map_uris(LV2_URID_Map* map, SW_LV2_URIS* uris) {
  uris->atom_Blank          = map->map(map->handle, LV2_ATOM__Blank);
  uris->atom_Object         = map->map(map->handle, LV2_ATOM__Object);
  uris->atom_Float          = map->map(map->handle, LV2_ATOM__Float);
  uris->atom_Int            = map->map(map->handle, LV2_ATOM__Int);
  uris->atom_eventTransfer  = map->map(map->handle, LV2_ATOM__eventTransfer);
  uris->atom_Sequence       = map->map(map->handle, LV2_ATOM__Sequence);
  uris->time_Position       = map->map(map->handle, LV2_TIME__Position);
  uris->time_barBeat        = map->map(map->handle, LV2_TIME__barBeat);
  uris->time_beatsPerMinute = map->map(map->handle, LV2_TIME__beatsPerMinute);
  uris->time_speed          = map->map(map->handle, LV2_TIME__speed);
  uris->time_frame          = map->map(map->handle, LV2_TIME__frame);
  uris->time_fps            = map->map(map->handle, LV2_TIME__framesPerSecond);
    
  uris->UIData              = map->map(map->handle, SW_URI "#UIData");
  uris->midi_MidiEvent      = map->map(map->handle, LV2_MIDI__MidiEvent);
  uris->ui_portNumber       = map->map(map->handle, SW_URI "#ui_portNumber");
  uris->ui_portValue        = map->map(map->handle, SW_URI "#ui_portValue");
}
#endif  /* SUPERWELLE_H */
