extends Node2D


func _ready() -> void:
	print(
		"godot-distrho version: ",
		DistrhoPluginServer.get_version(),
		" build: ",
		DistrhoPluginServer.get_build()
	)
	DistrhoUIServer.parameter_changed.connect(_on_parameter_changed)


func _on_parameter_changed(index: int, value: float) -> void:
	print("UI: Parameter Changed: index: ", index, " value: ", value)


func _on_note_on_button_pressed() -> void:
	DistrhoUIServer.send_note_on(0, 60, 90)


func _on_note_off_button_pressed() -> void:
	DistrhoUIServer.send_note_off(0, 60)


func _on_v_slider_value_changed(value: float) -> void:
	DistrhoUIServer.set_parameter_value(0, value)
