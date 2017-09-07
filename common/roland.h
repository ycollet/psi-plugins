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
 * Roland JP8000 SuperSaw emulation. See:
 * http://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf
 * 
 */

#ifndef ROLAND_H
#define ROLAND_H

#include "math.h"
#include "types.h"

typedef struct _roland {
  sample_t rol_detune[128];
  sample_t rol_cosc[128];
  sample_t rol_sosc[128];
} Roland;


static const sample_t roland_distance[32] = {
  0.0f,-0.01952356f, 0.01952356f, -0.06288439f, 0.06288439f, -0.11002313f
  ,  0.11002313f
  , -0.155273f, 0.155273f, -0.200523f, 0.200523f, -0.245772f, 0.245772f
  , -0.291022f, 0.291022f, -0.336272f, 0.336272f
  , -0.381522f, 0.381522f, -0.426772f, 0.426772f, -0.472021f, 0.472021f
  , -0.517271f, 0.517271f, -0.562521f, 0.562521f
  , -0.607771f, 0.607771f, -0.653021f ,0.653021f
  };

static inline sample_t roland_detune(sample_t x);

static inline void roland_init(Roland* r);

static inline sample_t roland_unison_shift(Roland* r, uint8_t detune, uint8_t oscillator);

static inline sample_t roland_amplitude(Roland* r, uint8_t mix, uint8_t oscillator);

static inline sample_t roland_detune(sample_t x) {
  return 
    (10028.7312891634   * pow(x,11.0))- (50818.8652045924  * pow(x,10.0))
    +(111363.4808729368 * pow(x,9.0)) - (138150.6761080548 * pow(x,8.0))
    +(106649.6679158292 * pow(x,7.0)) - (53046.9642751875  * pow(x,6.0))
    +(17019.9518580080  * pow(x,5.0)) - (3425.0836591318   * pow(x,4.0))
    +(404.2703938388    * pow(x,3.0)) - (24.1878824391     * pow(x,2.0))
    +(0.6717417634*x)   + 0.0030115596 ;
}

static inline sample_t roland_mix(sample_t x, uint8_t side) {
  if (side) {
    // detuned osciallator
    return -0.73764 * pow(x,2.0) + 1.2841*x + 0.044372;
  }
  else {
    // center
    return -0.55366 * x + 0.99785;
  }
}

/* fill detune table, centre osc amp and side osc amp tables */
static inline void roland_init(Roland* r) {
  uint8_t i =0;
    
  for (i = 0; i < 128; ++i) {
    r->rol_detune[i] = roland_detune((sample_t) i/127.0);
  }
  for (i = 0; i < 128; ++i) {
    r->rol_cosc[i]   = roland_mix((sample_t) i/127.0, 0);
  }
  for (i = 0; i < 128; ++i) {
    r->rol_sosc[i]   = roland_mix((sample_t) i/127.0, 1);
  }
} 

/* returns the unison shift 
   detune =   [ 0 ..127 ]
   oscillator [ 0 .. 31 ]
*/
static inline sample_t roland_unison_shift(Roland* r, uint8_t detune, uint8_t oscillator) {
  return r->rol_detune[detune] * roland_distance[oscillator];
}

static inline sample_t roland_amplitude(Roland* r, uint8_t mix, uint8_t oscillator) {
  return ( oscillator == 0 ? r->rol_cosc[mix] : r->rol_sosc[mix] );
}

#endif
