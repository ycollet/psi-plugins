midi_rnd-psi
============

midi_rnd is a simple MIDI random note generator for the inspirationally bereft.

It receives incoming note messages and returns a randomized note of the same 
velocity with the parameters defined by octave range and scale. The generated 
note is produced relative to the incoming key.

An attempt to map Note Off midi messages to previously recevied note on
messages is made but is only reliable for sequential input. As a result some 
NoteOn messages may be left hanging. 

Parameters
----------

**Channel**
randomize notes on this channel ( 0 for all channels)

**Upper/Lower** 
randomize notes received within this range

**Octaves** 
produce random notes within this range of octaves. The incoming note
is considered to be the center of this range. When this value is 0, the Random
Upper/Lower range is used instead

**Random upper/lower** 
When octaves is zero, generate random notes within this range only. The 
resulting random note is not relative to the incoming note
                
**Scale Type/Scale Root** 
Random notes will be produced within this key signature

