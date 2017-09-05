sidechain_gate-psi
==================

This is a stereo gate with optional sidechain input based on the Gate plugin by 
Steve Harris. 

![Image](./screenshot.png?raw=true)

Parameters
----------

**High Cut**    
sets the upper cutoff frequency of the input signal

**Low Cut**     
sets the lower cutoff frequency of the input signal

**Threshold**
signals that fall below the threshold will be reduced in level

**Reduction**
the amount of signal reduction (aka. Range).

**Hysteresis**
sets the closing threshold. This parameter will always be less 
than the threshold. Modifications to the threshold will cause 
this parameter to be adjusted by the same amount.

**Attack**:      
the time it takes to fully open the gate after the signal exceeds the 
threshold

**Hold**
the time the gate is kept open after the signal falls below the 
threshold

**Release**:     Time it takes to reach maximum attenuation after the signal 
                 falls below the threshold.

**Output select**: -1 = input listen, 0 = gate, 1 = bypass.
**Input select**:  0 = input signal, 1 = sidechain input.


