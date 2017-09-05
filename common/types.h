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

#ifndef TYPES_H
#define TYPES_H
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <time.h>

#ifndef __cplusplus
#include <stdbool.h>    
#endif
#ifndef SAMPLE_T
#define SAMPLE_T double
typedef SAMPLE_T sample_t;
#endif

#ifndef BIQUAD_TYPE
#define BIQUAD_TYPE double
#define COS cos
#define SIN sin
#define SINH sinh
#define POW pow
#endif


#ifndef LN_2_2
# define LN_2_2 0.34657359f // ln(2)/2
#endif

#ifndef M_LN2
#define M_LN2       0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846  /* pi */
#endif

#ifndef LIMIT
# define LIMIT(v,l,u) (v<l?l:(v>u?u:v))
#endif

#define DEBUG
#ifdef DEBUG
# ifndef OVERFLOW_CHK
#  define OVERFLOW_CHK(x) if (abs(x) > 1.0) printf("Overflow: %f\n",x)
# endif
#else
# define OVERFLOW_CHK(x) 
#endif 

//typedef void (*PluginWriteFuncPtr)(int,double,int);

#endif
