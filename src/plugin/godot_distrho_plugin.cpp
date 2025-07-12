#include "godot_distrho_plugin.h"
#include "DistrhoDetails.hpp"
#include "distrho_shared_memory_audio.h"
#include "godot_distrho_client.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <kj/string.h>

using namespace boost::interprocess;
using namespace boost::posix_time;

START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin(GodotDistrhoClient *p_client, uint32_t parameterCount, uint32_t programCount,
                                       uint32_t stateCount)
    : Plugin(parameterCount, programCount, stateCount) {
    client = p_client;
}

GodotDistrhoPlugin::~GodotDistrhoPlugin() {
}

const char *GodotDistrhoPlugin::getLabel() const {
    return client->getLabel();
}

const char *GodotDistrhoPlugin::getDescription() const {
    return client->getDescription();
}

const char *GodotDistrhoPlugin::getMaker() const {
    return client->getMaker();
}

const char *GodotDistrhoPlugin::getHomePage() const {
    return client->getHomePage();
}

const char *GodotDistrhoPlugin::getLicense() const {
    return client->getLicense();
}

uint32_t GodotDistrhoPlugin::getVersion() const {
    return client->getVersion();
}

int64_t GodotDistrhoPlugin::getUniqueId() const {
    return client->getUniqueId();
}

void GodotDistrhoPlugin::initAudioPort(const bool input, const uint32_t index, AudioPort &port) {
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
    client->initParameter(index, parameter);
}

float GodotDistrhoPlugin::getParameterValue(const uint32_t index) const {
    return client->getParameterValue(index);
}

void GodotDistrhoPlugin::setParameterValue(const uint32_t index, const float value) {
    client->setParameterValue(index, value);
}

void GodotDistrhoPlugin::activate() {
    client->activate();
}

void GodotDistrhoPlugin::run(const float **inputs, float **outputs, uint32_t numSamples, const MidiEvent *midiEvents,
                             uint32_t midiEventCount) {
    static MidiEvent midi_output[godot::MIDI_BUFFER_SIZE];
    int midi_output_size = 0;

    client->run(inputs, outputs, numSamples, midiEvents, midiEventCount, midi_output, midi_output_size);

    for (int i = 0; i < midi_output_size; i++) {
        writeMidiEvent(midi_output[i]);
    }
}

Plugin *createPlugin() {
    GodotDistrhoClient *client = new GodotDistrhoClient(DistrhoCommon::PLUGIN_TYPE);

    uint32_t parameterCount = client->get_parameter_count();
    uint32_t programCount = client->get_program_count();
    uint32_t stateCount = client->get_state_count();

    GodotDistrhoPlugin *const plugin = new GodotDistrhoPlugin(client, parameterCount, programCount, stateCount);
    return plugin;
}

END_NAMESPACE_DISTRHO
