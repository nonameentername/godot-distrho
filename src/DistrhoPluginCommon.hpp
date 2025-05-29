#pragma once

#include "DistrhoDetails.hpp"

static constexpr const char* const kVersionString = "v0.0.1";
static constexpr const uint32_t kVersionNumber = d_version(0, 0, 1);

#define DISTRHO_PLUGIN_BRAND   "godot-distrho"
#define DISTRHO_PLUGIN_NAME    "godot-distrho"
#define DISTRHO_PLUGIN_URI     "https://github.com/nonameentername/godot-distrho"
#define DISTRHO_PLUGIN_CLAP_ID "godot-distrho"

#define DISTRHO_PLUGIN_HAS_UI          1
#define DISTRHO_PLUGIN_IS_RT_SAFE      1
#define DISTRHO_PLUGIN_WANT_PROGRAMS   0
#define DISTRHO_PLUGIN_WANT_STATE      1
#define DISTRHO_UI_FILE_BROWSER        1
#define DISTRHO_UI_USE_NANOVG          0

#define DISTRHO_PLUGIN_EXTRA_IO        { 2, 2 },

#define DISTRHO_PLUGIN_CLAP_FEATURES   "audio-effect", "multi-effects", "mono"
#define DISTRHO_PLUGIN_LV2_CATEGORY    "lv2:SimulatorPlugin"
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Dynamics|Mono"

#define DISTRHO_PLUGIN_BRAND_ID Gdot
#define DISTRHO_PLUGIN_UNIQUE_ID Gdot

#define DISTRHO_UI_DEFAULT_WIDTH  940
#define DISTRHO_UI_DEFAULT_HEIGHT 398

enum Parameters {
    kParameterCount
};

enum States {
    kStateCount
};

static const Parameter kParameters[] = {
};

static constexpr const uint kNumParameters = ARRAY_SIZE(kParameters);

static_assert(kNumParameters == kParameterCount, "Matched num params");
