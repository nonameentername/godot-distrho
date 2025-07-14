extends Node2D


func _ready() -> void:
	print(
		"godot-distrho version: ",
		DistrhoPluginServer.get_version(),
		" build: ",
		DistrhoPluginServer.get_build()
	)


func _on_note_on_button_pressed() -> void:
	DistrhoUIServer.send_note_on(0, 60, 90)


func _on_note_off_button_pressed() -> void:
	DistrhoUIServer.send_note_off(0, 60)
