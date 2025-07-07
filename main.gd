extends Node2D


func _ready() -> void:
	print(
		"godot-distrho version: ",
		DistrhoServer.get_version(),
		" build: ",
		DistrhoServer.get_build()
	)
	DistrhoServer.midi_event.connect(_on_midi_event)
	DistrhoServer.midi_note_on.connect(_on_midi_note_on)
	DistrhoServer.midi_note_off.connect(_on_midi_note_off)
	DistrhoServer.midi_cc.connect(_on_midi_cc)
	DistrhoServer.midi_program_change.connect(_on_midi_program_change)


func _on_midi_event(midi_event: DistrhoMidiEvent) -> void:
	print(
		"MIDI Event: channel: ",
		midi_event.channel,
		" status: ",
		midi_event.status,
		" data1: ",
		midi_event.data1,
		" data2: ",
		midi_event.data2,
		" frame: ",
		midi_event.frame
	)
	DistrhoServer.send_midi_event(midi_event)


func _on_midi_note_on(channel: int, note: int, velocity: int, frame: int) -> void:
	print(
		"Note On: channel: ", channel, " note: ", note, " velocity: ", velocity, " frame: ", frame
	)


func _on_midi_note_off(channel: int, note: int, velocity: int, frame: int) -> void:
	print(
		"Note Off: channel: ", channel, " note: ", note, " velocity: ", velocity, " frame: ", frame
	)


func _on_midi_cc(channel: int, controller: int, value: int, frame: int) -> void:
	print(
		"CC: channel: ", channel, " controller: ", controller, " value: ", value, " frame: ", frame
	)


func _on_midi_program_change(channel: int, program: int, frame: int) -> void:
	print("Program Change: channel: ", channel, " program: ", program, " frame: ", frame)
