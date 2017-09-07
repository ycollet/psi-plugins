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

#ifndef OSC_COMMON_H
#define OSC_COMMON_H

#define OSC_TABLE_LEN 2049
#define BL_TABLE_LEN 2048
#define OSC_TABLES 8
// tablen is sample rate * O_SMPL
#define O_SMPL 2

enum waveshape { // enumerators that represent a possible wave shape
  Sine = 0, Triangle, Saw, ReverseSaw, Square, StairStep, Pulse, PinkNs 
};

enum osc_type {
  Default = 0, LFO, BLIT
};

#endif
