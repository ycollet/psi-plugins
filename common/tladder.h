//// LICENSE TERMS: Copyright 2012 Teemu Voipio
// 
// You can use this however you like for pretty much any purpose,
// as long as you don't claim you wrote it. There is no warranty.
//
// Distribution of substantial portions of this code in source form
// must include this copyright notice and list of conditions.
//

// input delay and state for member variables

#include <math.h>

typedef struct {
    sample_t zi;
    sample_t f;
    sample_t s[4];
    sample_t resonance;
    sample_t cutoff;
    
} TLadder;

// tanh(x)/x approximation, flatline at very high inputs
// so might not be safe for very large feedback gains
// [limit is 1/15 so very large means ~15 or +23dB]
static inline
sample_t tanhXdX(sample_t x);

static inline
sample_t tanhXdX(sample_t x)
{
    sample_t a = x*x;
    // IIRC I got this as Pade-approx for tanh(sqrt(x))/sqrt(x) 
    return ((a + 105)*a + 945) / ((15*a + 420)*a + 945);
}

static inline
sample_t tladder_run(TLadder* t, sample_t in, int filter_type);

// cutoff as normalized frequency (eg 0.5 = Nyquist)
// resonance from 0 to 1, self-oscillates at settings over 0.9
static inline
sample_t tladder_run(TLadder* t, sample_t in, int filter_type)
{
    // tuning and feedback
    sample_t r    = (40.0/9.0) * t->resonance;
    sample_t* s   = t->s;
    sample_t out = 0.0;

    sample_t ih = 0.5 * (in + t->zi); t->zi = in;

    // evaluate the non-linear gains
    sample_t t0 = tanhXdX(ih - r * s[3]);
    sample_t t1 = tanhXdX(s[0]);
    sample_t t2 = tanhXdX(s[1]);
    sample_t t3 = tanhXdX(s[2]);
    sample_t t4 = tanhXdX(s[3]);

    // g# the denominators for solutions of individual stages
    sample_t g0 = 1 / (1 + t->f*t1), g1 = 1 / (1 + t->f*t2);
    sample_t g2 = 1 / (1 + t->f*t3), g3 = 1 / (1 + t->f*t4);
    
    // f# are just factored out of the feedback solution
    sample_t f3 = t->f*t3*g3,    f2 = t->f*t2*g2*f3;
    sample_t f1 = t->f*t1*g1*f2, f0 = t->f*t0*g0*f1;

    // solve feedback 
    sample_t y3 = (g3*s[3] + f3*g2*s[2] + f2*g1*s[1] + f1*g0*s[0] + f0*in) / (1 + r*f0);

    // then solve the remaining outputs (with the non-linear gains here)
    sample_t xx = t0*(in - r*y3);
    sample_t y0 = t1*g0*(s[0] + t->f*xx);
    sample_t y1 = t2*g1*(s[1] + t->f*y0);
    sample_t y2 = t3*g2*(s[2] + t->f*y1);

    // update state
    s[0] += 2*t->f * (xx - y0);
    s[1] += 2*t->f * (y0 - y1);
    s[2] += 2*t->f * (y1 - y2);
    s[3] += 2*t->f * (y2 - t4*y3);
    
    switch (filter_type)
    {
        case 1:             //FILTER_LP24:
            out = y3 ;
            break;
        case 4:             //FILTER_HP6:
        case 5:             //FILTER_BP6:
        default:
            out = in;
            break;
    }
    return out;
}

/*
// cutoff as normalized frequency (eg 0.5 = Nyquist)
// resonance from 0 to 1, self-oscillates at settings over 0.9
void transistorLadder(
    sample_t cutoff, sample_t resonance,
    sample_t * in, sample_t * out, unsigned nsamples)
{
    // tuning and feedback
    sample_t f = tan(M_PI * cutoff);
    sample_t r = (40.0/9.0) * resonance;

    for(unsigned n = 0; n < nsamples; ++n)
    {
        // input with half delay, for non-linearities
        sample_t ih = 0.5 * (in[n] + zi); zi = in[n];

        // evaluate the non-linear gains
        sample_t t0 = tanhXdX(ih - r * s[3]);
        sample_t t1 = tanhXdX(s[0]);
        sample_t t2 = tanhXdX(s[1]);
        sample_t t3 = tanhXdX(s[2]);
        sample_t t4 = tanhXdX(s[3]);

        // g# the denominators for solutions of individual stages
        sample_t g0 = 1 / (1 + f*t1), g1 = 1 / (1 + f*t2);
        sample_t g2 = 1 / (1 + f*t3), g3 = 1 / (1 + f*t4);
        
        // f# are just factored out of the feedback solution
        sample_t f3 = f*t3*g3, f2 = f*t2*g2*f3, f1 = f*t1*g1*f2, f0 = f*t0*g0*f1;

        // solve feedback 
        sample_t y3 = (g3*s[3] + f3*g2*s[2] + f2*g1*s[1] + f1*g0*s[0] + f0*in[n]) / (1 + r*f0);

        // then solve the remaining outputs (with the non-linear gains here)
        sample_t xx = t0*(in[n] - r*y3);
        sample_t y0 = t1*g0*(s[0] + f*xx);
        sample_t y1 = t2*g1*(s[1] + f*y0);
        sample_t y2 = t3*g2*(s[2] + f*y1);

        // update state
        s[0] += 2*f * (xx - y0);
        s[1] += 2*f * (y0 - y1);
        s[2] += 2*f * (y1 - y2);
        s[3] += 2*f * (y2 - t4*y3);

        out[n] = y3;
    }

}    */