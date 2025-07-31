#ifndef DISTRHO_PARAMETER_H
#define DISTRHO_PARAMETER_H

#include "godot_cpp/core/binder_common.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include <godot_cpp/classes/config_file.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class DistrhoParameter : public RefCounted {
    GDCLASS(DistrhoParameter, Object);

private:
    int hints;
    String name;
    String short_name;
    String symbol;
    String unit;
    String description;

    float default_value;
    float min_value;
    float max_value;

    Dictionary enumeration_values;
    int designation;

    int midi_cc;
    int group_id;

protected:
public:
    enum DistrhoParameterHint {
        HINT_NONE = 0x0,

        /**
           Parameter is automatable (real-time safe).
           @see Plugin::setParameterValue(uint32_t, float)
         */
        HINT_PARAMETER_IS_AUTOMATABLE = 0x01,

        /**
           Parameter value is boolean.@n
           It's always at either minimum or maximum value.
         */
        HINT_PARAMETER_IS_BOOLEAN = 0x02,

        /**
           Parameter value is integer.
         */
        HINT_PARAMETER_IS_INTEGER = 0x04,

        /**
           Parameter value is logarithmic.
         */
        HINT_PARAMETER_IS_LOGARITHMIC = 0x08,

        /**
           Parameter is of output type.@n
           When unset, parameter is assumed to be of input type.

           Parameter inputs are changed by the host and typically should not be changed by the plugin.@n
           One exception is when changing programs, see Plugin::loadProgram().@n
           The other exception is with parameter change requests, see Plugin::requestParameterValueChange().@n
           Outputs are changed by the plugin and never modified by the host.

           If you are targetting VST2, make sure to order your parameters so that all inputs are before any outputs.
         */
        HINT_PARAMETER_IS_OUTPUT = 0x10,

        /**
           Parameter value is a trigger.@n
           This means the value resets back to its default after each process/run call.@n
           Cannot be used for output parameters.

           @note Only officially supported under LV2. For other formats DPF simulates the behaviour.
        */
        HINT_PARAMETER_IS_TRIGGER = 0x20 | HINT_PARAMETER_IS_BOOLEAN,

        /**
           Parameter should be hidden from the host and user-visible GUIs.@n
           It is still saved and handled as any regular parameter, just not visible to the user
           (for example in a host generated GUI)
         */
        HINT_PARAMETER_IS_HIDDEN = 0x40
    };

    enum DistrhoParameterDesignation {
        /**
          Null or unset designation.
         */
        PARAMETER_DESIGNATION_NONE = 0,

        /**
          Bypass designation.@n
          When on (> 0.5f), it means the plugin must run in a bypassed state.
         */
        PARAMETER_DESIGNATION_BYPASS = 1
    };

    // TODO: remove duplicate code
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

    DistrhoParameter(DistrhoParameterHint p_hints, String p_name, String p_short_name, String p_symbol, String p_unit,
                     String p_description, float p_default_value, float p_min_value, float p_max_value,
                     Dictionary p_enumeration_values, int p_designation, int p_midi_cc, int p_group_id);

    DistrhoParameter() = default;
    ~DistrhoParameter();

    static DistrhoParameter *create(DistrhoParameterHint p_hints, String p_name, String p_short_name, String p_symbol,
                                    String p_unit, String p_description, float p_default_value, float p_min_value,
                                    float p_max_value, Dictionary p_enumeration_values, int p_designation,
                                    int p_midi_cc, int p_group_id);

    void set_hints(int p_hints);
    int get_hints();

    void set_name(String p_name);
    String get_name();

    void set_short_name(String p_short_name);
    String get_short_name();

    void set_symbol(String p_symbol);
    String get_symbol();

    void set_unit(String p_unit);
    String get_unit();

    void set_description(String p_description);
    String get_description();

    void set_default_value(float p_default_value);
    float get_default_value();

    void set_min_value(float p_min_value);
    float get_min_value();

    void set_max_value(float p_max_value);
    float get_max_value();

    void set_enumeration_values(Dictionary p_enumeration_values);
    Dictionary get_enumeration_values();

    void set_designation(int p_designation);
    int get_designation();

    void set_midi_cc(int p_midi_cc);
    int get_midi_cc();

    void set_group_id(int p_group_id);
    int get_group_id();

    static void _bind_methods();
};

} // namespace godot

#endif
