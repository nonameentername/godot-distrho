@tool
extends DistrhoPluginInstance


func _init() -> void:
	DistrhoPluginServer.set_distrho_plugin(self)

func get_uri() -> String:
	return "https://github.com/nonameentername/godot-distrho"

func get_label() -> String:
	return "godot-distrho"


func get_description() -> String:
	return "Godot DISTRHO plugin"


func get_maker() -> String:
	return "godot-distrho"


func get_homepage() -> String:
	return "https://github.com/nonameentername/godot-distrho"


func get_license() -> String:
	return "MIT"


func get_version() -> String:
	return "0.0.1"


func get_unique_id() -> String:
	#unique_id should only be 4 characters
	return "GDot"


func get_parameters() -> Array:
	var parameters = [
		{
			"hints": DistrhoParameter.HINT_NONE,
			"name": "Volume",
			"short_name": "vol",
			"symbol": "V",
			"unit": "dB",
			"description": "Volume used to change how loud sound is played",
			"default_value": 0.5,
			"min_value": 0,
			"max_value": 1,
			"enumeration_values": {},
			"designation": DistrhoParameter.PARAMETER_DESIGNATION_NONE,
			"midi_cc": 0,
			"group_id": DistrhoParameter.PORT_GROUP_NONE
		}
	]
	return parameters.map(DistrhoPluginServer.create_parameter)


func get_input_ports() -> Array:
	var ports = [
		{
			"hints": DistrhoAudioPort.HINT_NONE,
			"name": "Input Left",
			"symbol": "input_left",
			"group_id": DistrhoAudioPort.PORT_GROUP_STEREO
		},
		{
			"hints": DistrhoAudioPort.HINT_NONE,
			"name": "Input Right",
			"symbol": "input_right",
			"group_id": DistrhoAudioPort.PORT_GROUP_STEREO
		}
	]

	return ports.map(DistrhoPluginServer.create_audio_port)


func get_output_ports() -> Array:
	var ports = [
		{
			"hints": DistrhoAudioPort.HINT_NONE,
			"name": "Output Left",
			"symbol": "output_left",
			"group_id": DistrhoAudioPort.PORT_GROUP_STEREO
		},
		{
			"hints": DistrhoAudioPort.HINT_NONE,
			"name": "Output Right",
			"symbol": "output_right",
			"group_id": DistrhoAudioPort.PORT_GROUP_STEREO
		}
	]

	return ports.map(DistrhoPluginServer.create_audio_port)


func get_state_values() -> Dictionary:
	var midi_keys = {}
	
	for i in range(0, 88):
		midi_keys[str(i)] = "false"

	return midi_keys
