/****************************************************************************
*
* Name: decim.h
*
* Synopsis:
*
*   Decimates a real or complex signal.  For more information about
*   decimation, see dspGuru's Multirate FAQ at:
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

    decim - "decimates" a signal by changing reducing its sampling rate by an
             integral factor via FIR filtering.

Inputs:

    factor_M:
        the decimation factor (must be >= 1)

    H_size:
        the number of taps in the filter

    p_H:
        pointer to the array of coefficients for the resampling filter.

    num_inp:
        the number of input samples

    p_inp:
        pointer to the input samples

Input/Outputs:

    p_Z:
        pointer to the delay line array (which must have  H_size elements)

Outputs:

    p_out:
        pointer to the output sample array.

    p_num_out:
        pointer to the number of output samples

*****************************************************************************/
static inline
void decim(int factor_M, int H_size, const double *const p_H,
           double *const p_Z, int num_inp, const double *p_inp,
           double *p_out, int *p_num_out);

/****************************************************************************/
static inline
void decim(int factor_M, int H_size, const double *const p_H,
           double *const p_Z, int num_inp, const double *p_inp, double *p_out,
           int *p_num_out)
{
    int tap, num_out;
    double sum;

    /* this implementation assuems num_inp is a multiple of factor_M */
    //assert(num_inp % factor_M == 0);

    num_out = 0;
    while (num_inp >= factor_M) {
        /* shift Z delay line up to make room for next samples */
        for (tap = H_size - 1; tap >= factor_M; tap--) {
            p_Z[tap] = p_Z[tap - factor_M];
        }

        /* copy next samples from input buffer to bottom of Z delay line */
        for (tap = factor_M - 1; tap >= 0; tap--) {
            p_Z[tap] = *p_inp++;
        }
        num_inp -= factor_M;

        /* calculate FIR sum */
        sum = 0.0;
        for (tap = 0; tap < H_size; tap++) {
            sum += p_H[tap] * p_Z[tap];
        }
        *p_out++ = sum;     /* store sum and point to next output */
        num_out++;
    }

    *p_num_out = num_out;   /* pass number of outputs back to caller */
}
