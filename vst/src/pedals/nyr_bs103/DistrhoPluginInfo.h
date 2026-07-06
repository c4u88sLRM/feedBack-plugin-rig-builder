#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND   "RigBuilder"
#define DISTRHO_PLUGIN_NAME    "NYR BS103"
#define DISTRHO_PLUGIN_URI     "urn:rigbuilder:bassynth"
#define DISTRHO_PLUGIN_CLAP_ID "rigbuilder.bassynth"

#define DISTRHO_PLUGIN_BRAND_ID  Rgbd
#define DISTRHO_PLUGIN_UNIQUE_ID BS31

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_UI_USE_NANOVG        1
#define DISTRHO_UI_DEFAULT_WIDTH     360
#define DISTRHO_UI_DEFAULT_HEIGHT    440
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_PROGRAMS 0
#define DISTRHO_PLUGIN_WANT_STATE    0
// MUST stay an "Fx" category: this is an input-processing stompbox (it
// pitch-tracks the bass DI), not a MIDI instrument. Declaring it
// "Instrument|Synth" made the effects host classify it as a VSTi and
// skip it / not route the DI into it — i.e. the pedal "did nothing".
// Every other pedal (incl. the pitch ones, e.g. bass_sub_octave) is "Fx|…".
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Pitch"

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
