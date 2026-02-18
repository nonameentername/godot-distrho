#include "godot_distrho_plugin.h"
#include "DistrhoDetails.hpp"
#include "distrho_shared_memory_audio.h"
#include "godot_distrho_plugin_client.h"
#include "godot_distrho_plugin_server.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <kj/string.h>
#include <string>

using namespace boost::interprocess;
using namespace boost::posix_time;

START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin(GodotDistrhoPluginState *p_state,
                                       uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
    : Plugin(parameterCount, programCount, stateCount) {
    client = NULL;
    server = NULL;
    state = p_state;
}

GodotDistrhoPlugin::~GodotDistrhoPlugin() {
    if (client != NULL) {
        client->shutdown();
    }

    if (server != NULL) {
        delete server;
        server = NULL;
    }

    if (client != NULL) {
        delete client;
        client = NULL;
    }

    if (state != NULL) {
        delete state;
    }
}

const char *GodotDistrhoPlugin::getLabel() const {
    return state->label.c_str();
}

const char *GodotDistrhoPlugin::getDescription() const {
    return state->description.c_str();
}

const char *GodotDistrhoPlugin::getMaker() const {
    return state->maker.c_str();;
}

const char *GodotDistrhoPlugin::getHomePage() const {
    return state->home_page.c_str();;
}

const char *GodotDistrhoPlugin::getLicense() const {
    return state->license.c_str();
}

uint32_t GodotDistrhoPlugin::getVersion() const {
    return state->version;
}

int64_t GodotDistrhoPlugin::getUniqueId() const {
    return state->unique_id;
}

void GodotDistrhoPlugin::initAudioPort(const bool input, const uint32_t index, AudioPort &port) {
    if (input) {
        if (!state->input_ports[index]->name.isEmpty()) {
            port = *state->input_ports[index];
            return;
        }
    } else {
        if (!state->output_ports[index]->name.isEmpty()) {
            port = *state->output_ports[index];
            return;
        }
    }

    Plugin::initAudioPort(input, index, port);
}

void GodotDistrhoPlugin::initParameter(const uint32_t index, Parameter &parameter) {
    parameter = *state->parameters[index];
}

float GodotDistrhoPlugin::getParameterValue(const uint32_t index) const {
    if (client != NULL) {
        return client->getParameterValue(index);
    } else if (index < state->parameters.size() && state->parameters[index] != NULL) {
        return state->parameters[index]->ranges.def;
    } else {
        return 0;
    }
}

void GodotDistrhoPlugin::setParameterValue(const uint32_t index, const float value) {
    if (client != NULL) {
        client->setParameterValue(index, value);
    }
}

void GodotDistrhoPlugin::activate() {
    if (client == NULL) {
        client = new GodotDistrhoPluginClient(DistrhoCommon::PLUGIN_TYPE);
    }

    if (server == NULL) {
        server = new GodotDistrhoPluginServer(this, client->get_godot_rpc_memory());
    }
}

void GodotDistrhoPlugin::setState(const char* key, const char* value) {
    if (client != NULL) {
        client->set_state_value(key, value);
    }
}

void GodotDistrhoPlugin::initState(uint32_t p_index, State& p_state) {
    p_state.key = state->state_values[p_index]->key;
    p_state.defaultValue = state->state_values[p_index]->label;
}

void GodotDistrhoPlugin::run(const float **inputs, float **outputs, uint32_t numSamples, const MidiEvent *midiEvents,
                             uint32_t midiEventCount) {
    static MidiEvent midi_output[godot::MIDI_BUFFER_SIZE];
    int midi_output_size = 0;

    if (client != NULL) {
        client->run(inputs, outputs, numSamples, midiEvents, midiEventCount, midi_output, midi_output_size);
    }

    for (int i = 0; i < midi_output_size; i++) {
        writeMidiEvent(midi_output[i]);
    }
}

Plugin *createPlugin() {
    GodotDistrhoPluginClient *client = new GodotDistrhoPluginClient(DistrhoCommon::PLUGIN_TYPE);
    GodotDistrhoPluginServer *server = new GodotDistrhoPluginServer(NULL, client->get_godot_rpc_memory());
    GodotDistrhoPluginState *state = new GodotDistrhoPluginState();

    uint32_t parameter_count = client->get_parameter_count();
    uint32_t program_count = client->get_program_count();
    uint32_t state_count = client->get_state_count();

    state->label = client->getLabel();
    state->description = client->getDescription();
    state->maker = client->getMaker();
    state->home_page = client->getHomePage();
    state->license = client->getLicense();
    state->version = client->getVersion();
    state->unique_id = client->getUniqueId();

    state->parameters.reserve(parameter_count);

    for (size_t i = 0; i < parameter_count; ++i) {
        state->parameters.push_back(std::make_unique<Parameter>());
        client->initParameter(i, *state->parameters[i]);
    }

    state->input_ports.reserve(DISTRHO_PLUGIN_NUM_INPUTS);

    for (int i = 0; i < DISTRHO_PLUGIN_NUM_INPUTS; i++) {
        state->input_ports.push_back(std::make_unique<AudioPort>());
        client->get_input_port(i, *state->input_ports[i]);
    }

    state->output_ports.reserve(DISTRHO_PLUGIN_NUM_OUTPUTS);

    for (int i = 0; i < DISTRHO_PLUGIN_NUM_OUTPUTS; i++) {
        state->output_ports.push_back(std::make_unique<AudioPort>());
        client->get_output_port(i, *state->output_ports[i]);
    }

    state->state_values.reserve(state_count);

    for (int i = 0; i < state_count; i++) {
        state->state_values.push_back(std::make_unique<State>());
        client->get_initial_state_value(i, *state->state_values[i]);
    }

    GodotDistrhoPlugin *const plugin = new GodotDistrhoPlugin(state, parameter_count, program_count, state_count);

    client->shutdown();

    delete server;
    server = NULL;

    delete client;
    client = NULL;

    return plugin;
}

END_NAMESPACE_DISTRHO
