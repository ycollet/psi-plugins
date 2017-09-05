psi-plugins
===========

psi-plugins is a small collection of LV2 plugins ideal for (but not limited to)
electronic music. It uses the ntk widget library (possibly provided by your 
distribution as non-ntk-devel/ntk-dev)

They are:

__super_welle__: a 4 oscillator / 16 voice virtual analog unison synthesizer 

![Image](./super_welle.lv2/screenshot.png?raw=true)

__midi_gate__: an enveloped stereo MIDI gate

![Image](./midi_gate-psi.lv2/screenshot.png?raw=true)

__sidechain_gate__: stereo sidechain gate (with hysteresis) based on Steve 
Harris's Gate plugin

![Image](./sidechain_gate-psi.lv2/screenshot.png?raw=true)

__midi_rnd__: MIDI randomizer. Randomize incoming MIDI within a range, or 
relative within an octave range.

Install
-------
```
./waf configure
./waf
sudo ./waf install
```

To specify a custom prefix and/or LV2 directory:
```
./waf configure --prefix=/usr --lv2dir=/usr/lib64/lv2
./waf
sudo ./waf install
```
