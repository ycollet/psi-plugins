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
 * You may copy and distribute verbatim copies of this document.
 * You may modify and use this source code to create binary code for your own purposes, free or commercial.
 * 
 * @section DESCRIPTION
 *
 *
 * Based on the C++ class by Nigel Redmon on 12/18/12.
 * http://www.earlevel.com/main/2013/06/01/envelope-generators/
 * EarLevel Engineering: earlevel.com
 * For a complete explanation of the ADSR envelope generator and code,
 * read the series of articles by the author, starting here:
 * http://www.earlevel.com/main/2013/06/01/envelope-generators/
 *
 */

#ifndef ADRS_h
#define ADRS_h
#include <math.h>
#include "types.h"

enum envState {
  adsr_idle = 0,
  adsr_att,
  adsr_dec,
  adsr_sus,
  adsr_rel
};

typedef struct _adsr {
  sample_t srate;
  sample_t output;
  sample_t attackRate;
  sample_t decayRate;
  sample_t releaseRate;
  sample_t attackCoef;
  sample_t decayCoef;
  sample_t releaseCoef;
  sample_t sustainLevel;
  sample_t targetRatioA;
  sample_t targetRatioDR;
  sample_t attackBase;
  sample_t decayBase;
  sample_t releaseBase;
  uint8_t state;
  uint8_t loop;
    
} Adsr;


static inline  void adsr_init(Adsr* self, sample_t attack, sample_t decay, sample_t sustain,
                              sample_t release, sample_t sampleRate);
static inline  sample_t adsr_calcCoef(sample_t rate, sample_t targetRatio);
static inline  sample_t adsr_process(Adsr* self);
static inline  void adsr_gate(Adsr* self,int on);
    
static inline  void adsr_setDecayRate(Adsr* self, sample_t rate);
static inline  void adsr_setAttackRate(Adsr* self, sample_t rate);
static inline  void adsr_setReleaseRate(Adsr* self, sample_t rate);
static inline  void adsr_setSustainLevel(Adsr* self, sample_t level);
static inline  void adsr_setTargetRatioA(Adsr* self, sample_t targetRatio);
static inline  void adsr_setTargetRatioDR(Adsr* self, sample_t targetRatio);
static inline  void adsr_reset(Adsr* self);
static inline  void adsr_loop(Adsr* self, int loop);

static inline void adsr_init(Adsr* self, sample_t attack, sample_t decay, sample_t sustain,
                             sample_t release, sample_t sampleRate) {
  self->srate = sampleRate;
    
  adsr_setTargetRatioA(self,0.3 );
  adsr_setTargetRatioDR(self,0.0001 );
    
  adsr_setAttackRate(self,attack * (sample_t)sampleRate);
  adsr_setDecayRate(self,decay * (sample_t)sampleRate);
  adsr_setReleaseRate(self,release * (sample_t)sampleRate);
  adsr_setSustainLevel(self,sustain);
  adsr_reset(self);
}

static inline void adsr_gate(Adsr* self, int gate) {
  if (gate)
    self->state = adsr_att;
  else if (self->state != adsr_idle)
    self->state = adsr_rel;
}

static inline void adsr_reset(Adsr *self) {
  self->state = adsr_idle;
  self->output = 0.0f;
}

static inline void adsr_loop(Adsr* self, int loop) {
  self->loop = loop;
}

static inline sample_t adsr_process(Adsr* self) {
  switch (self->state) {
  case adsr_idle:
    self->output = 0.0;
    break;
  case adsr_att:
    self->output = self->attackBase + self->output * self->attackCoef;
    if (self->output >= 1.0) {
      self->output = 1.0;
      self->state = adsr_dec;
    }
    break;
  case adsr_dec:
    self->output = self->decayBase + self->output * self->decayCoef;
    if (self->output <= self->sustainLevel) {
      self->output = self->sustainLevel;
      self->state = adsr_sus;
      if (self->loop) {
        self->state = adsr_att;
        self->output = self->attackBase;
      }
    }
    break;
  case adsr_sus:
    break;
  case adsr_rel:
    self->output = self->releaseBase + self->output * self->releaseCoef;
    if (self->output <= 0.0) {
      self->output = 0.0;
      self->state = adsr_idle;
    }
            
  }
  return self->output;
}

static inline void adsr_setAttackRate(Adsr* self, sample_t rate) {
  self->attackRate = rate;
  self->attackCoef = adsr_calcCoef(rate, self->targetRatioA);
  self->attackBase = (1.0 + self->targetRatioA) * (1.0 - self->attackCoef);
}

static inline void adsr_setDecayRate(Adsr* self, sample_t rate) {
  self->decayRate = rate;
  self->decayCoef = adsr_calcCoef(rate, self->targetRatioDR);
  self->decayBase = (self->sustainLevel - self->targetRatioDR) * (1.0 - self->decayCoef);
}

static inline void adsr_setReleaseRate(Adsr* self, sample_t rate) {
  self->releaseRate = rate;
  self->releaseCoef = adsr_calcCoef(rate, self->targetRatioDR);
  self->releaseBase = -self->targetRatioDR * (1.0 - self->releaseCoef);
}

static inline sample_t adsr_calcCoef(sample_t rate, sample_t targetRatio) {
  return exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

static inline void adsr_setSustainLevel(Adsr* self, sample_t level) {
  self->sustainLevel = level;
  self->decayBase = (self->sustainLevel - self->targetRatioDR) * (1.0 - self->decayCoef);
}

static inline void adsr_setTargetRatioA(Adsr* self, sample_t targetRatio) {
  if (targetRatio < 0.000000001)
    targetRatio = 0.000000001;  // -180 dB
  self->targetRatioA = targetRatio;
  self->attackBase = (1.0 + self->targetRatioA) * (1.0 - self->attackCoef);
}

static inline void adsr_setTargetRatioDR(Adsr* self, sample_t targetRatio) {
  if (targetRatio < 0.000000001)
    targetRatio = 0.000000001;  // -180 dB
  self->targetRatioDR = targetRatio;
  self->decayBase = (self->sustainLevel - self->targetRatioDR) * (1.0 - self->decayCoef);
  self->releaseBase = -self->targetRatioDR * (1.0 - self->releaseCoef);
}

#endif
