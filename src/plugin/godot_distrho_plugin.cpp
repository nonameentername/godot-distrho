#include "godot_distrho_plugin.h"
#include "DistrhoDetails.hpp"
#include "distrho_shared_memory_audio.h"
#include "godot_distrho_plugin_client.h"
#include "godot_distrho_plugin_server.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <kj/string.h>

using namespace boost::interprocess;
using namespace boost::posix_time;

START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin(GodotDistrhoPluginClient *p_client, GodotDistrhoPluginServer *p_server,
                                       uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
    : Plugin(parameterCount, programCount, stateCount) {
    client = p_client;
    server = p_server;
}

GodotDistrhoPlugin::~GodotDistrhoPlugin() {
    if (server != NULL) {
        delete server;
        server = NULL;
    }

    if (client != NULL) {
        client->shutdown();
        delete client;
        client = NULL;
    }
}

const char *GodotDistrhoPlugin::getLabel() const {
    if (client != NULL) {
        return client->getLabel();
    } else {
        return "";
    }
}

const char *GodotDistrhoPlugin::getDescription() const {
    if (client != NULL) {
        return client->getDescription();
    } else {
        return "";
    }
}

const char *GodotDistrhoPlugin::getMaker() const {
    if (client != NULL) {
        return client->getMaker();
    } else {
        return "";
    }
}

const char *GodotDistrhoPlugin::getHomePage() const {
    if (client != NULL) {
        return client->getHomePage();
    } else {
        return "";
    }
}

const char *GodotDistrhoPlugin::getLicense() const {
    if (client != NULL) {
        return client->getLicense();
    } else {
        return "";
    }
}

uint32_t GodotDistrhoPlugin::getVersion() const {
    if (client != NULL) {
        return client->getVersion();
    } else {
        return d_version(0, 0, 1);
    }
}

int64_t GodotDistrhoPlugin::getUniqueId() const {
    if (client != NULL) {
        return client->getUniqueId();
    } else {
        return d_cconst('n', 'o', 'n', 'e');
    }
}

void GodotDistrhoPlugin::initAudioPort(const bool input, const uint32_t index, AudioPort &port) {
    if (client == NULL) {
        return;
    }

    if (input) {
        if (client->get_input_port(index, port)) {
            return;
        }
    } else {
        if (client->get_output_port(index, port)) {
            return;
        }
    }

    Plugin::initAudioPort(input, index, port);
}

void GodotDistrhoPlugin::initParameter(const uint32_t index, Parameter &parameter) {
    if (client != NULL) {
        client->initParameter(index, parameter);
    }
}

float GodotDistrhoPlugin::getParameterValue(const uint32_t index) const {
    if (client != NULL) {
        return client->getParameterValue(index);
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
        client->activate();
    }
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
    GodotDistrhoPluginServer *server = new GodotDistrhoPluginServer(client->get_godot_rpc_memory());

    uint32_t parameterCount = client->get_parameter_count();
    uint32_t programCount = client->get_program_count();
    uint32_t stateCount = client->get_state_count();

    GodotDistrhoPlugin *const plugin = new GodotDistrhoPlugin(client, server, parameterCount, programCount, stateCount);
    return plugin;
}

END_NAMESPACE_DISTRHO
