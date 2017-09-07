/**
 * @file
 * @author  Brendan Jones <brendan.jones.it@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 * 
 * Copyright 2012 Nigel Redmon
 * Copyright 2014, Brendan Jones
 * 
 * This source code is provided as is, without warranty.
 * You may copy and distribute verbatim copies of b document.
 * You may modify and use b source code to create binary code for your own purposes, free or commercial.
 * 
 * @section DESCRIPTION
 * 
 * Based on the C++ class created by Nigel Redmon on 5/15/12
 * EarLevel Engineering: earlevel.com
 * 
 *
 * For a complete explanation of the wavetable oscillator and code,
 * read the series of articles by the author, starting here:
 * www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
 *
 */

#include "bl_osc.h"
#include "bl_osc_g.h"

extern waveTableShape waveTableShapes[OSC_TABLES];

//
// bl_output
//
// returns the current oscillator output
//
inline float bl_output(BL_Osc* b) {
  // grab the appropriate wavetable
  int waveTableIdx = 0;
  while ((b->freq >= waveTableShapes[b->shape].waveTables[waveTableIdx].topFreq) && (waveTableIdx < (waveTableShapes[b->shape].numWaveTables - 1))) {
    ++waveTableIdx;
  }
  waveTable *waveTable = &waveTableShapes[b->shape].waveTables[waveTableIdx];
    
#if !doLinearInterp
  // truncate
  return waveTable->waveTable[(int)(b->phasor * waveTable->waveTableLen)];
#else
  // linear interpolation
  sample_t temp = b->phasor * waveTable->waveTableLen;
  int intPart = temp;
  sample_t fracPart = temp - intPart;
  float samp0 = waveTable->waveTable[intPart];
  if (++intPart >= waveTable->waveTableLen)
    intPart = 0;
  float samp1 = waveTable->waveTable[intPart];
  return samp0 + (samp1 - samp0) * fracPart;
#endif
}


//
// bl_output_minus_offset
//
// for variable pulse width: initialize to sawtooth,
// set phaseOfs to duty cycle, use b for osc output
//
// returns the current oscillator output
//
inline float bl_output_minus_offset(BL_Osc* b) {
  // grab the appropriate wavetable
  int waveTableIdx = 0;
  long numWaveTables = waveTableShapes[b->shape].numWaveTables;
  while ( (b->freq >= waveTableShapes[b->shape].waveTables[waveTableIdx].topFreq) 
          && (waveTableIdx < (numWaveTables - 1)) ) {
    ++waveTableIdx;
  }
  waveTable *waveTable = &waveTableShapes[b->shape].waveTables[waveTableIdx];
    
#if !doLinearInterp
  // truncate
  sample_t offsetPhasor = b->phasor + b->phaseOfs;
  if (offsetPhasor >= 1.0)
    offsetPhasor -= 1.0;
  return waveTable->waveTable[(int)(b->phasor * waveTable->waveTableLen)] 
    - waveTable->waveTable[(int)(offsetPhasor * waveTable->waveTableLen)];
#else
  // linear
  sample_t temp = b->phasor * waveTable->waveTableLen;
  int intPart = (int) temp;
  sample_t fracPart = temp - intPart;
  float samp0 = waveTable->waveTable[intPart];
  if (++intPart >= waveTable->waveTableLen)
    intPart = 0;
  float samp1 = waveTable->waveTable[intPart];
  float samp = samp0 + (samp1 - samp0) * fracPart;
    
  // and linear again for the offset part
  sample_t offsetPhasor = b->phasor + b->phaseOfs;
  if (offsetPhasor > 1.0)
    offsetPhasor -= 1.0;
  temp = offsetPhasor * waveTable->waveTableLen;
  intPart = (int) temp;
  fracPart = temp - (sample_t) intPart;
  samp0 = waveTable->waveTable[intPart];
  if (++intPart >= waveTable->waveTableLen)
    intPart = 0;
  samp1 = waveTable->waveTable[intPart];
  return samp - (samp0 + (samp1 - samp0) * fracPart);
#endif
}

//
// bl_add_WaveTable
//
// add wavetables in order of lowest frequency to highest
// topFreq is the highest frequency supported by a wavetable
// wavetables within an oscillator can be different lengths
//
// returns 0 upon success, or the number of wavetables if no more room is available
//
int bl_add_WaveTable(int shape, int len, float *waveTableIn, sample_t topFreq) {
    
  if (waveTableShapes[shape].numWaveTables < numWaveTableSlots) {
    int numTables = waveTableShapes[shape].numWaveTables;
    waveTableShapes[shape].waveTables[numTables].waveTable = (float*) malloc(sizeof(float[len]));
    float *waveTable = waveTableShapes[shape].waveTables[numTables].waveTable;
    waveTableShapes[shape].waveTables[numTables].waveTableLen = len;
    waveTableShapes[shape].waveTables[numTables].topFreq = topFreq;
       
    ++waveTableShapes[shape].numWaveTables;
        
    // fill in wave
    for (long idx = 0; idx < len; idx++)
      waveTable[idx] = waveTableIn[idx];
        
    return 0;
  }
  return waveTableShapes[shape].numWaveTables;
}

//
// fillBLTables:
//
// The main function of interest here; call this with a pointer to an new, empty oscillator,
// and the real and imaginary arrays and their length. The function fills the oscillator with
// all wavetables necessary for full-bandwidth operation.
//
void bl_fill_tables(int shape, sample_t *freqWaveRe, sample_t *freqWaveIm, int numSamples) {
  int idx;
    
  // zero DC offset and Nyquist
  freqWaveRe[0] = freqWaveIm[0] = 0.0;
  freqWaveRe[numSamples >> 1] = freqWaveIm[numSamples >> 1] = 0.0;
    
  // determine maxHarmonic, the highest non-zero harmonic in the wave
  int maxHarmonic = numSamples >> 1;
  const sample_t minVal = 0.000001; // -120 dB
  while ((fabs(freqWaveRe[maxHarmonic]) + fabs(freqWaveIm[maxHarmonic]) > minVal) && maxHarmonic) --maxHarmonic;

  // calculate topFreq for the initial wavetable
  // maximum non-aliasing playback rate is 1 / (2 * maxHarmonic), but we allow aliasing up to the
  // point where the aliased harmonic would meet the next octave table, which is an additional 1/3
  sample_t topFreq = 2.0 / 3.0 / maxHarmonic;
    
  // for subsquent tables, sample_t topFreq and remove upper half of harmonics
  sample_t ar[numSamples];
  sample_t ai[numSamples];
  sample_t scale = 0.0;
  while (maxHarmonic) {
    // fill the table in with the needed harmonics
    for (idx = 0; idx < numSamples; idx++)
      ar[idx] = ai[idx] = 0.0;
    for (idx = 1; idx <= maxHarmonic; idx++) {
      ar[idx] = freqWaveRe[idx];
      ai[idx] = freqWaveIm[idx];
      ar[numSamples - idx] = freqWaveRe[numSamples - idx];
      ai[numSamples - idx] = freqWaveIm[numSamples - idx];
    }
        
    // make the wavetable
    scale = bl_make_WaveTable(shape, numSamples, ar, ai, scale, topFreq);

    // prepare for next table
    topFreq *= 2;
    maxHarmonic >>= 1;
  }
}

//
// bl_fft
//
// I grabbed (and slightly modified) this Rabiner & Gold translation...
//
// (could modify for real data, could use a template version, blah blah--just keeping it short)
//
void bl_fft(int N, sample_t *ar, sample_t *ai)
/*
  in-place complex bl_fft
 
  After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
 
  program adapted from FORTRAN 
  by K. Steiglitz  (ken@princeton.edu)
  Computer Science Dept. 
  Princeton University 08544          */
{    
  int i, j, k, L;            /* indexes */
  int M, TEMP, LE, LE1, ip;  /* M = log N */
  int NV2, NM1;
  sample_t t;               /* temp */
  sample_t Ur, Ui, Wr, Wi, Tr, Ti;
  sample_t Ur_old;
    
  // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    
  NV2 = N >> 1;
  NM1 = N - 1;
  TEMP = N; /* get M = log N */
  M = 0;
  while (TEMP >>= 1) ++M;
    
  /* shuffle */
  j = 1;
  for (i = 1; i <= NM1; i++) {
    if(i<j) {             /* swap a[i] and a[j] */
      t = ar[j-1];     
      ar[j-1] = ar[i-1];
      ar[i-1] = t;
      t = ai[j-1];
      ai[j-1] = ai[i-1];
      ai[i-1] = t;
    }
        
    k = NV2;             /* bit-reversed counter */
    while(k < j) {
      j -= k;
      k /= 2;
    }
        
    j += k;
  }
    
  LE = 1.;
  for (L = 1; L <= M; L++) {            // stage L
    LE1 = LE;                         // (LE1 = LE/2) 
    LE *= 2;                          // (LE = 2^L)
    Ur = 1.0;
    Ui = 0.; 
    Wr = cos(M_PI/(float)LE1);
    Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
    for (j = 1; j <= LE1; j++) {
      for (i = j; i <= N; i += LE) { // butterfly
        ip = i+LE1;
        Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
        Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
        ar[ip-1] = ar[i-1] - Tr;
        ai[ip-1] = ai[i-1] - Ti;
        ar[i-1]  = ar[i-1] + Tr;
        ai[i-1]  = ai[i-1] + Ti;
      }
      Ur_old = Ur;
      Ur = Ur_old * Wr - Ui * Wi;
      Ui = Ur_old * Wi + Ui * Wr;
    }
  }
}

//
// if scale is 0, auto-scales
// returns scaling factor (0.0 if failure), and wavetable in ai array
//

float bl_make_WaveTable(int widx, int len, sample_t *ar, sample_t *ai, sample_t scale, sample_t topFreq) {
  bl_fft(len, ar, ai);
    
  if (scale == 0.0) {
    // calc normal
    sample_t max = 0;
    for (int idx = 0; idx < len; idx++) {
      sample_t temp = fabs(ai[idx]);
      if (max < temp)
        max = temp;
    }
    scale = 1.0 / max * .999;        
  }
    
  // normalize 
  float wave[len];
  for (int idx = 0; idx < len; idx++)
    wave[idx] = ai[idx] * scale;
        
  if (bl_add_WaveTable(widx, len, wave, topFreq))
    scale = 0.0;
    
  return scale;
}

//
// example that builds a sawtooth oscillator via frequency domain
//
void bl_saw_osc(long tableLen) {
  // int tableLen = 2048;    // to give full bandwidth from 20 Hz
  int idx;
  sample_t freqWaveRe[tableLen];
  sample_t freqWaveIm[tableLen];
    
  // make a sawtooth
  for (idx = 0; idx < tableLen; idx++) {
    freqWaveIm[idx] = 0.0;
  }
  freqWaveRe[0] = freqWaveRe[tableLen >> 1] = 0.0;
  for (idx = 1; idx < (tableLen >> 1); idx++) {
    freqWaveRe[idx] = 1.0 / idx;                    // sawtooth spectrum
    freqWaveRe[tableLen - idx] = -freqWaveRe[idx];  // mirror
  }
    
  bl_fill_tables(Saw, freqWaveRe, freqWaveIm, tableLen);

}

//
// example that creates and oscillator from an arbitrary time domain wave
//
void bl_wave_osc(int shape, float *waveSamples, int tableLen) {
  int idx;
  sample_t freqWaveRe[tableLen];
  sample_t freqWaveIm[tableLen];
    
  // take FFT
  for (idx = 0; idx < tableLen; idx++) {
    freqWaveIm[idx] = waveSamples[idx];
    freqWaveRe[idx] = 0.0;
  }
  bl_fft(tableLen, freqWaveRe, freqWaveIm);
    
  bl_fill_tables(shape, freqWaveRe, freqWaveIm, tableLen);
}
