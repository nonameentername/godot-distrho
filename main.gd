extends Node2D


func _ready() -> void:
	print(
		"godot-distrho version: ",
		DistrhoServer.get_version(),
		" build: ",
		DistrhoServer.get_build()
	)
