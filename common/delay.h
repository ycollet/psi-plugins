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

#ifndef DELAY_H
#define DELAY_H
#include "types.h"

#ifndef CF_WIN
# define CF_WIN 0.010
#endif

// we will only start to cross fade when the delay has changed by this much
#ifndef CF_THRESHOLD
# define CF_THRESHOLD 0.010
#endif

typedef struct _DelayLine {
  sample_t* buffer;
  sample_t  feedback;
  sample_t  max_delay;
  sample_t  delay;
  int       counter;
} DelayLine;

typedef struct _Delay {
  DelayLine*  delay1;     // delay lines
  DelayLine*  delay2;
  DelayLine*  curr;       // current delay line pointer
  DelayLine*  next;       // cross fade to delay line pointer
  sample_t*   buffer;
  sample_t    feedback;
  sample_t    delay;
  sample_t    max_delay;
  sample_t    fs;
  sample_t    inc;        // cross fade increment
  int         window;
  int         cf_cnt;     // cross fade counter
  int         counter;    // dry history
} Delay;


__attribute__((unused)) static void delayline_alloc(DelayLine *d, sample_t max_delay) {
  d->buffer       = (sample_t*) calloc(max_delay, sizeof(sample_t));
  d->delay        = 0.0;
  d->feedback     =  0.0;
  d->max_delay    = max_delay;
  d->counter      = 0;
  for(int s=0; s < d->max_delay;++s)
    d->buffer[s] = 0;
}

__attribute__((unused)) static void delayline_init(DelayLine *d, sample_t feedback, sample_t delay) {
  d->counter=0;
  d->delay = delay;
  d->feedback = feedback;
    
  for(int s=0;s< (int) d->max_delay; s++)
    d->buffer[s]=0;
    
}

__attribute__((unused)) static void delayline_free(DelayLine *d) {
  free(d->buffer);
  d->buffer = 0;
}

__attribute__((unused)) static inline sample_t delayline_feed(DelayLine* d, sample_t in) {        
  // calculate delay offset
  double back=(double)(d->counter) - (d->delay * d->max_delay);
    
  // clip lookback buffer-bound
  if(back<0.0)
    back=d->max_delay+back;
    
  // compute interpolation left-floor
  int const index0=(int)floor(back);
    
  // compute interpolation right-floor
  int index_1 =   index0-1;
  int index1  =   index0+1;
  int index2  =   index0+2;
    
  // clip interp. buffer-bound
  if (index_1<0)   index_1=d->max_delay-1;
  if (index1>=d->max_delay)    index1=0;
  if (index2>=d->max_delay)    index2=0;
    
  // get neighbourgh samples
  sample_t const y_1= d->buffer [index_1];
  sample_t const y0 = d->buffer [index0];
  sample_t const y1 = d->buffer [index1];
  sample_t const y2 = d->buffer [index2];
    
  // compute interpolation x
  sample_t const x=(sample_t)back-(sample_t)index0;
    
  // calculate
  sample_t const c0 = y0;
  sample_t const c1 = 0.5*(y1-y_1);
  sample_t const c2 = y_1 - 2.5*y0 + 2.0*y1 - 0.5*y2;
  sample_t const c3 = 0.5*(y2-y_1) + 1.5*(y0-y1);
    
  sample_t const output=((c3*x+c2)*x+c1)*x+c0;
    
  // add to delay buffer
    
  d->buffer[d->counter] = in + output * d->feedback;
    
  // increment delay counter
  d->counter++;
    
  // clip delay counter
  if(d->counter>=d->max_delay)
    d->counter=0;
    
  // return output
  return output * d->feedback;
}    

__attribute__((unused)) static void delay_free(Delay *d) {
  delayline_free(d->delay1);
  delayline_free(d->delay2);
}

__attribute__((unused)) static void delay_init(Delay *d, sample_t feedback, sample_t delay) {
  d->counter=0;
  d->delay = delay;
  d->feedback = feedback;
}

__attribute__((unused)) static void delay_alloc(Delay *d, sample_t max_delay, sample_t fs) {
  d->delay1       = (DelayLine*) calloc(1, sizeof(DelayLine));
  d->delay2       = (DelayLine*) calloc(1, sizeof(DelayLine));   
  d->buffer       = (sample_t*) calloc(max_delay, sizeof(sample_t));
  delayline_alloc(d->delay1, max_delay);
  delayline_alloc(d->delay2, max_delay);
  d->curr         = d->delay1;
  d->next         = d->delay2;
  d->delay        = 0.0;
  d->feedback     = 0.0;
  d->max_delay    = max_delay;
  d->counter      = 0;
  d->cf_cnt       = 0;
  d->inc          = 0;
  d->fs           = fs;
  d->window       = round(fs * CF_WIN);
}

__attribute__((unused)) static void delay_update(Delay* d, sample_t feedback, sample_t delay, uint32_t len) {
  d->feedback = feedback;
  d->curr->feedback = feedback;
  d->next->feedback = feedback;
  if ( d->delay != delay) 
    {
      // cross fade required
      d->delay          = delay;
      d->cf_cnt         = d->window;
      d->inc            = 1.0 / ((sample_t) d->window);
      for(int s=(int) d->max_delay; --s;)
        d->next->buffer[s] =  d->buffer[s];

      d->next->delay    = delay;
      d->next->counter  = d->counter;
      d->next->feedback = feedback;
    }
}

__attribute__((unused)) static inline sample_t delay_feed(Delay* d, sample_t in) {   
  sample_t out;
  if (d->cf_cnt) 
    {
      DelayLine* c = d->curr;
    
      const sample_t o2 = delayline_feed(d->next, in);
      const sample_t o1 = delayline_feed(d->curr, in);
        
      const sample_t fd = M_PI * (d->inc * d->cf_cnt)/2;
        
      out = o1 * SIN(fd)
        + o2 * COS(fd);
        
      if ( 0 == --d->cf_cnt) {
        d->curr = d->next;
        d->next = c;
      }
        
    } else {
    out = delayline_feed(d->curr, in);
  }
  d->buffer[d->counter] = in;
  if (++d->counter==d->max_delay)
    d->counter = 0;
  return out;
}

#endif    
