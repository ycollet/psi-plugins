midi_gate-psi
=============

This is a stereo gate MIDI based on the example LV2 midi gate plugin by Dave Robillard 

![Image](./screenshot.png?raw=true)

Parameters
----------

**Attack**    the time it takes to fully open the gate after once a note-on is 
              recieved

**Hold**      the time the gate is kept open

**Release**   the time it takes to release the gate once the hold time has been 
attained

**Reduction** The amount of signal reduction (aka. Range)

**Output select** 0 = gate, 1 = bypass.

**Gate direction**  0 - gate open on Note On, 1 - gate open on Note Off

