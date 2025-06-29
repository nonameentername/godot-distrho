#ifndef DISTRHO_AUDIO_PORT_H
#define DISTRHO_AUDIO_PORT_H

#include "godot_cpp/core/binder_common.hpp"
#include <godot_cpp/classes/config_file.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class DistrhoAudioPort : public RefCounted {
    GDCLASS(DistrhoAudioPort, Object);

private:
    int hints;
    String name;
    String symbol;
    int group_id;

protected:
public:
    enum DistrhoAudioPortHint {
        HINT_NONE = 0x0,

        /**
           Audio port can be used as control voltage (LV2 and JACK standalone only).
         */
        HINT_AUDIO_PORT_IS_CV = 0x1,

        /**
           Audio port should be used as sidechan (LV2 and VST3 only).
           This hint should not be used with CV style ports.
           @note non-sidechain audio ports must exist in the plugin if this flag is set.
         */
        HINT_AUDIO_PORT_IS_SIDECHAIN = 0x2,

        /**
           CV port has bipolar range (-1 to +1, or -5 to +5 if scaled).
           This is merely a hint to tell the host what value range to expect.
         */
        HINT_CV_PORT_HAS_BIPOLAR_RANGE = 0x10,

        /**
           CV port has negative unipolar range (-1 to 0, or -10 to 0 if scaled).
           This is merely a hint to tell the host what value range to expect.
         */
        HINT_CV_PORT_HAS_NEGATIVE_UNIPOLAR_RANGE = 0x20,

        /**
           CV port has positive unipolar range (0 to +1, or 0 to +10 if scaled).
           This is merely a hint to tell the host what value range to expect.
         */
        HINT_CV_PORT_HAS_POSITIVE_UNIPOLAR_RANGE = 0x40,

        /**
           CV port has scaled range to match real values (-5 to +5v bipolar, +/-10 to 0v unipolar).
           One other range flag is required if this flag is set.

           When enabled, this makes the port a mod:CVPort, compatible with the MOD Devices platform.
         */
        HINT_CV_PORT_HAS_SCALED_RANGE = 0x80,

        /**
           CV port is optional, allowing hosts that do no CV ports to load the plugin.
           When loaded in hosts that don't support CV, the float* buffer for this port will be null.
         */
        HINT_CV_PORT_IS_OPTIONAL = 0x100
    };

    enum DistrhoPredefinedPortGroupsIds {
        /**
          Null or unset port group.
          */
        PORT_GROUP_NONE = -1,

        /**
          A single channel audio group.
          */
        PORT_GROUP_MONO = -2,

        /**
          A 2-channel discrete stereo audio group,
          where the 1st audio port is the left channel and the 2nd port is the right channel.
          */
        PORT_GROUP_STEREO = -3
    };

    DistrhoAudioPort(DistrhoAudioPortHint p_hints, String p_name, String p_symbol, int p_group_id);
    DistrhoAudioPort() = default;
    ~DistrhoAudioPort();

    static DistrhoAudioPort *create(DistrhoAudioPortHint p_hints, String p_name, String p_symbol, int p_group_id);

    int get_hints();
    String get_name();
    String get_symbol();
    int get_group_id();

    static void _bind_methods();
};

} // namespace godot

#endif
