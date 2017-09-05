/****************************************************************************
*
* Name: interp.h
*
* Synopsis:
*
*   Interpolates a real or complex signal.  For more information, about
*   interpolation, see dspGuru's Multirate FAQ at:
*
*       http://www.dspguru.com/info/faqs/mrfaq.htm
*
* Description: See function descriptons below.
*
* by Grant R. Griffin
* Provided by Iowegian's "dspGuru" service (http://www.dspguru.com).
* Copyright 2001, Iowegian International Corporation (http://www.iowegian.com)
*
*                          The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies. 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/

/*****************************************************************************
Description:

    interp - "interpolates" a signal by increasing its sampling rate by an
             integral factor via FIR filtering.

Inputs:

    factor_L:
        the interpolation factor (must be >= 1)

    num_taps_per_phase:
        the number of taps per polyphase filter, which is the number of taps
        in the filter divided by factor_L.

    p_H:
        pointer to the array of coefficients for the resampling filter.  (The
        number of total taps in p_H is interp * num_taps_per_phase, so be sure
        to design your filter using a number of taps which is divisible by
        factor_L.)

    num_inp:
        the number of input samples

    p_inp:
        pointer to the input samples

Input/Outputs:

    p_Z:
        pointer to the delay line array (which must have num_taps_per_phase
        elements)    

Outputs:

    p_out:
        pointer to the output sample array

    p_num_out:
        pointer to the number of output samples

*****************************************************************************/
static inline
void interp(int factor_L, int num_taps_per_phase, const double *const p_H,
            double *const p_Z, int num_inp, const double *p_inp,
            double *p_out, int *p_num_out);

/****************************************************************************/
static inline
void interp(int factor_L, int num_taps_per_phase, const double *const p_H,
            double *const p_Z, int num_inp, const double *p_inp,
            double *p_out, int *p_num_out)
{
    int tap, num_out, phase_num;
    const double *p_coeff;
    double sum;

    num_out = 0;
    while (--num_inp > 0) {
        /* shift Z delay line up to make room for next sample */
        for (tap = num_taps_per_phase - 1; tap > 0; tap--) {
            p_Z[tap] = p_Z[tap - 1];
        }

        /* copy next sample from input buffer to bottom of Z delay line */
        p_Z[0] = *p_inp++;

        /* calculate outputs */
        for (phase_num = 0; phase_num < factor_L; phase_num++) {
            /* point to the current polyphase filter */
            p_coeff = p_H + phase_num;

            /* calculate FIR sum */
            sum = 0.0;
            for (tap = 0; tap < num_taps_per_phase; tap++) {
                sum += *p_coeff * p_Z[tap];
                p_coeff += factor_L;          /* point to next coefficient */
            }
            *p_out++ = sum;     /* store sum and point to next output */
            num_out++;
        }
    }

    *p_num_out = num_out;   /* pass number of outputs back to caller */
}
