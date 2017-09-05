
#include "noise.h"
#include <math.h>

// make noise - from audacity
bool makeNoise(int noiseType,float *buffer, int len, float fs, float amplitude)
{
   float white = 0;
   int i = 0;
   float div = ((float)RAND_MAX) / 2.0f;
   float x, y, z;
   float buf0,buf1,buf2,buf3,buf4,buf5,buf6;
   buf0 = buf1 = buf2 = buf3 = buf4 = buf5 = buf6 =0;
   x = y = z = 0;
   switch (noiseType) {
   default:
   case 0: // white
       for(i=0; i<len; i++)
          buffer[i] = amplitude * ((rand() / div) - 1.0f);
       break;

   case 1: // pink
      // based on Paul Kellet's "instrumentation grade" algorithm.
      white=0;

      // 0.129f is an experimental normalization factor.
      amplitude *= 0.129f;
      for(i=0; i<len; i++) {
      white=(rand() / div) - 1.0f;
      buf0=0.99886f * buf0 + 0.0555179f * white;
      buf1=0.99332f * buf1 + 0.0750759f * white;
      buf2=0.96900f * buf2 + 0.1538520f * white;
      buf3=0.86650f * buf3 + 0.3104856f * white;
      buf4=0.55000f * buf4 + 0.5329522f * white;
      buf5=-0.7616f * buf5 - 0.0168980f * white;
      buffer[i] = amplitude * 
         (buf0 + buf1 + buf2 + buf3 + buf4 + buf5 + buf6 + white * 0.5362);
      buf6 = white * 0.115926; 
      }
      break;

   case 2: // Brownian
       white=0;
       // min and max protect against instability at extreme sample rates.
       float leakage = ((fs-144.0)/fs < 0.9999)? (fs-144.0)/fs : 0.9999;
       float scaling = (9.0/sqrt(fs) > 0.01)? 9.0/sqrt(fs) : 0.01;

       for(i=0; i<len; i++){
         white=(rand() / div) - 1.0f;
         z = leakage * y + white * scaling;
         y = (fabs(z) > 1.0) ? (leakage * y - white * scaling) : z;
         buffer[i] = amplitude * y;
       }
       break;
   }
   return true;
}
