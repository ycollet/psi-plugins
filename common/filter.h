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

#ifndef FILTER_H

#define FILTER_H
#include <stdlib.h>
#include "types.h"
#include "interp.h"
#include "decim.h"
#include "tladder.h"
#include "biquad.h"
#include "ls_filter.h"
#include "adsr.h"

#define PEAK_BW          0.3f /* Peak EQ bandwidth (octaves) */
#define SHELF_SLOPE 1.5f      /* Shelf EQ slope*/

#ifndef FILTER_4POLE
# define FILTER_4POLE 2
#endif

/*
##define    FILTER_INTERP       2
##define    FILTER_INTERPF      2.0
##define    FILTER_INTERP_TAPS  6
*/
#define    FILTER_INTERP       1
#define    FILTER_INTERPF      1.0
#define    FILTER_INTERP_TAPS  6

#define saturate(x) tanh(x)

#define CLAMP(x) x > FILTER_INTERPF ? FILTER_INTERPF : (x < -1.0* FILTER_INTERPF ? - 1.0 * FILTER_INTERPF : x)

#define    FILTER_NONE  0
#define    FILTER_LP12  1
#define    FILTER_HP12  2
#define    FILTER_BP12  3 
#define    FILTER_LP24  4     /* Moog filter */
#define    FILTER_HP24  5
#define    FILTER_BP24  6
#define    FILTER_BP6   7
#define    FILTER_HP6   8
#define    FILTER_EQ12  9
#define    FILTER_LS    10
#define    FILTER_HS    11
#define    FILTER_PINK_NS 12


const static sample_t fir_coeff[FILTER_INTERP_TAPS * FILTER_INTERP] =
{           0.036681502163648017
            ,0.2746317593794541
            ,0.56109896978791948
            ,0.769741833862266
            ,0.8922608180038789
            ,0.962094548378084
};

// musicdsp moog filter
typedef struct {
    sample_t    sample_rate;
    sample_t    cutoff;
    sample_t    resonance;
    sample_t    buf[5];
    sample_t    t1, t2;                      // temporary buffers
    sample_t    f, q;                        // frequency , Q 
} Moog1;

// aciddose moog filter (kvr)
typedef struct {
    sample_t    sample_rate;
    sample_t    cutoff;
    sample_t    resonance;
    sample_t    buf[5];
    sample_t    oversample[5];
    sample_t    fb, c, c2, g, cgfbr, cg;
} Moog2;

typedef struct _Filter { 
    // RBJ biquad filters
    biquad      bq;
    biquad      bq2;
    // Linux sampler resonant biquad filters
    ls_filt     ls_bq;
    ls_filt     ls_bq2;
    // Moog filter implementation
    Moog1       m1;
    Moog2       m2;
    TLadder     tl1;
    
    sample_t    sample_rate;
    sample_t    cutoff;
    sample_t    resonance;           
    sample_t    amp;
    sample_t    bandwidth;
    
    // state varaible parameters
    sample_t    low;
    sample_t    band;
    // upsampling
    sample_t    interp_delay[FILTER_INTERP_TAPS];
    
    sample_t    decim_delay[FILTER_INTERP_TAPS];
    
    
    uint8_t     filter_type;
    uint8_t     retrigger;
    uint8_t     mod1; 
    uint8_t     mod2;
    uint8_t     mod3;
    
} Filter;

static void 
filter_alloc(Filter* f, sample_t srate);

static void 
filter_free(Filter* f);

static void 
filter_init(Filter* f, int ftype);

static void 
filter_reset(Filter* f, sample_t cutoff, sample_t resonance, sample_t amp
           , sample_t bandwith);

static sample_t 
filter_run(Filter* f, sample_t in);

static sample_t 
filter_run_moog1(Moog1* m, sample_t in, uint8_t filter_type);

static sample_t 
filter_run_moog2(Moog2* m, sample_t in, uint8_t filter_type);

static void 
filter_alloc(Filter* f, sample_t srate)
{
    f->sample_rate    = srate;
    biquad_init(&f->bq);
    biquad_init(&f->bq2);
    ls_filt_init(&f->ls_bq);

    ls_filt_init(&f->ls_bq2);
}

static void
filter_init(Filter* f, int ftype)
{
    f->filter_type   = ftype;
    
    biquad_init(&f->bq);
    biquad_init(&f->bq2);
    ls_filt_init(&f->ls_bq);
    ls_filt_init(&f->ls_bq2);
    
    f->m1.buf[0] = f->m1.buf[1] = f->m1.buf[2] = f->m1.buf[3] = 0.0;
    f->m1.buf[4] = f->m1.t1     = f->m1.t2     = 0.0;
    f->m1.f = 0.0;
    f->m1.sample_rate = f->m2.sample_rate = f->sample_rate;
    
    f->m2.buf[0] = f->m2.buf[1] = f->m2.buf[2] = f->m2.buf[3] = 0.0;

    f->tl1.s[0] = f->tl1.s[1] = f->tl1.s[2] = f->tl1.s[3] = 0.0;
    f->tl1.zi = 0.0;
            
    
}

static void
filter_free(Filter* f)
{
}

static void 
filter_reset(Filter *f, sample_t cutoff, sample_t resonance
                , sample_t amp
                , sample_t bandwith)
{
    switch (f->filter_type)
    {
        case FILTER_EQ12:
             eq_set_params(&f->bq, f->cutoff, f->amp
                  , f->bandwidth, f->sample_rate);
        case FILTER_LS:
            ls_set_params(&f->bq, f->cutoff, f->amp
                , 1.5, f->sample_rate);
            break;
        case FILTER_HS:
            hs_set_params(&f->bq, f->cutoff, f->amp
                , 1.5, f->sample_rate);
            break;
            
        case FILTER_LP12:
        case FILTER_BP12:
        case FILTER_HP12:
            f->cutoff = cutoff;
            f->resonance = resonance;
            break;    
            
        case FILTER_LP24:

            if ( FILTER_4POLE == 1 ) 
            {
                f->m1.cutoff        = cutoff;
                f->m1.resonance     = resonance;
                f->m1.f             = tan(f->cutoff * M_PI / f->sample_rate);
            }
            
            if ( FILTER_4POLE == 2 )
            {
                f->m2.cutoff        = cutoff;
                f->m2.resonance     = resonance;
                f->m2.fb            = 4.0 * resonance;
                f->m2.c             = tan(cutoff * M_PI / f->m2.sample_rate);
                f->m2.g             = 1.0      / (1.0 + f->m2.c);
                f->m2.cg            = f->m2.c  / (1.0 + f->m2.c);
                f->m2.c2            = f->m2.c * 2.0;
                f->m2.cgfbr         = 1.0 / 
                                    (1.0 + f->m2.fb * f->m2.cg * f->m2.cg
                                    * f->m2.cg * f->m2.cg);
            }
            
            if ( FILTER_4POLE ==  3 )
            {
                f->tl1.cutoff        = cutoff;
                f->tl1.resonance     = resonance;
                f->tl1.f             = tan(cutoff * M_PI / f->sample_rate);
            }

            break;
            
        case FILTER_BP24:
            ls_filt_setup(&f->ls_bq, LS_F_TYPE_BP, cutoff, resonance
                ,f->sample_rate);
            ls_filt_setup(&f->ls_bq2, LS_F_TYPE_BP, cutoff, resonance
                ,f->sample_rate);
            break;
            
        case FILTER_HP24:
            ls_filt_setup(&f->ls_bq, LS_F_TYPE_HP, cutoff, resonance
                ,f->sample_rate);
            ls_filt_setup(&f->ls_bq2, LS_F_TYPE_HP, cutoff, resonance
                ,f->sample_rate);
            break;
            
        case FILTER_BP6:
            break;
            
        case FILTER_HP6:
            break;
            
        default:    
            break;
    }
}

static sample_t 
filter_run_moog1(Moog1* m, sample_t in, uint8_t filter_type)
{
    sample_t out = 0.0;
    sample_t* buf = m->buf;
    m->f = m->cutoff * (1.0/(m->sample_rate/2.0));    
    if (m->f > 0.999999) m->f = 0.999999;
    
    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    // Set coefficients given f & resonance [0.0...1.0]
    sample_t q  = 1.0 - (m->f);
    sample_t p  = m->f + 0.8 * (m->f) * q;
    sample_t _f = p + p - 1.0;
    q = m->resonance * (1.0f + 0.5f * q * (1.0 - q + 5.6 * q * q));

    // Filter (in [-1.0...+1.0])
    in -= q * buf[4];                          //feedback
    
    m->t1 = buf[1];  buf[1] = (in + buf[0]) * p - buf[1] * _f;
    m->t2 = buf[2];  buf[2] = (buf[1] + m->t1) * p - buf[2] * _f;
    m->t1 = buf[3];  buf[3] = (buf[2] + m->t2) * p - buf[3] * _f;
    buf[4] = (buf[3] + m->t1) * p - buf[4] * _f;
    buf[4] = buf[4] - buf[4] * buf[4] * buf[4] * 0.166667;    //clipping
    buf[0] = in;
    
    switch (filter_type)
    {
        case FILTER_LP24:
            out = buf[4] ;
            break;
        case FILTER_HP6:
            out = in - buf[4];
            break;
        case FILTER_BP6:
            out =  3.0 * (buf[3] - buf[4]);
            break;
        default:
            out = in;
            break;
    }
    return out;   
}

static sample_t 
filter_run_moog2(Moog2* m, sample_t in, uint8_t filter_type)
{
        // if you want more harmonic, allow fb past 4.0, 4.5 is a good range.
        // do not do this without oversample unless you want aliasing.
        //const float fb = 4.0f * shape(resonance);

        // if you use saturation functions, recommended you use at least 2x oversample.
        // "works" with linear and no oversample but results are very poor.
        // if you only 2x oversample with linear, even zero-stuffing works better than nothing.    // upsample
    /*
    const sample_t intn_in = 1;
    int   intn_out = 0;        
    const sample_t decn_in = 1;
    int   decn_out = 0;        
    
    interp(FILTER_INTERP, FILTER_INTERP_TAPS, (const double*) fir_coeff,
        (double *const) f->interp_delay, intn_in, &in,
        f->oversample, &intn_out);
    for (int i = 0; i < FILTER_INTERP; i++) 
    {
        const float in1 = oversample[i];
        */
        sample_t* buf = m->buf;
        sample_t out = 0.0;
        const float in1 = in;
        const float prediction =
            buf[3] * m->g +
            m->cg * (buf[2] * m->g +
            m->cg * (buf[1] * m->g +
            m->cg * (buf[0] * m->g +
            m->cg * in1)));
                
        //const float xv = in1 - CLAMP(fb * prediction) * cgfbr;
        const float xv = in1 - m->fb * prediction * m->cgfbr;
        
        const float xw = saturate(m->g * (buf[0] + m->c * xv));
        const float xx = saturate(m->g * (buf[1] + m->c * xw));
        const float xy = saturate(m->g * (buf[2] + m->c * xx));
        const float xz = saturate(m->g * (buf[3] + m->c * xy));

        buf[0] += m->c2 * (xv - xw);
        buf[1] += m->c2 * (xw - xx);
        buf[2] += m->c2 * (xx - xy);
        buf[3] += m->c2 * (xy - xz);
        out = xz;
        
    
    /*
    decim(FILTER_INTERP, FILTER_INTERP_TAPS, (const double*) fir_coeff,
        (double *const) &f->decim_delay, decn_in, &xz,
        &out, &decn_out);
    */    
    return out;
}

static sample_t 
filter_run(Filter* f, sample_t in)
{
    sample_t out = in;
    if (f->filter_type == 0 ) return in;
    filter_reset(f, f->cutoff, f->resonance, f->amp, f->bandwidth);
    OVERFLOW_CHK(in) ;
    in = LIMIT(in,-1.0,1.0);
    
    if ( (f->filter_type == FILTER_LP24 ) 
        || (f->filter_type == FILTER_BP6 )
        || (f->filter_type == FILTER_HP6 ) )
    {
        
        if ( FILTER_4POLE == 1 )
        {
            out = filter_run_moog1(&f->m1,in, f->filter_type);
            return out;
        }
        
        if ( FILTER_4POLE == 2 ) 
        {
            out = filter_run_moog2(&f->m2,in, f->filter_type);
            return out;
        }

        if ( FILTER_4POLE ==  3 )
        {
            out = tladder_run(&f->tl1, in, f->filter_type - 3);
            return out;
        }
    }
    
    // State variable filter
    if (  (f->filter_type == FILTER_LP12 ) 
       || (f->filter_type == FILTER_BP12 )
       || (f->filter_type == FILTER_HP12 )
       )
    {    
        
        sample_t ff     = 2.0 * SIN(M_PI * f->cutoff / f->sample_rate); 
        sample_t q      = f->resonance;
        
        sample_t low    = f->low;
        sample_t band   = f->band;
        sample_t high   = 0;
        //sample_t notch  = 0;
        
        q = 1.0 - atan(sqrt(q * 100)) * 2.0 / M_PI;
        sample_t scale = sqrt(q);
        ff = f->cutoff / f->sample_rate * 4.0;
        if (ff > 0.999999) 
            ff          = 0.999999;
        low             = low + ff * band;
        high            = scale * in - low - q*band;
        band            = ff * high + band;
        //notch           = high + low;
        
        if (f->filter_type == FILTER_LP12 ) out = low;
        if (f->filter_type == FILTER_BP12 ) out = band;
        if (f->filter_type == FILTER_HP12 ) out = high;
        f->low = low;
        f->band = band;
        out = LIMIT(out,-1.0,1.0);
    }
    if ( (f->filter_type == FILTER_BP24 ) || (f->filter_type == FILTER_HP24 ) )
    {
       out = ls_filt_run(&f->ls_bq,in);
       out = ls_filt_run(&f->ls_bq2,out);
    }
    if ( f->filter_type == FILTER_EQ12 ) 
        out = biquad_run(&f->bq2,in);
    return out;
}

#endif
