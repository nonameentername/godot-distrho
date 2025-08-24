extends SceneTree


func _init():
	var main_ui_scene = preload("res://main_ui.tscn")
	change_scene_to_packed(main_ui_scene)

	await create_timer(2.0).timeout

	var main_scene = preload("res://main.tscn")
	change_scene_to_packed(main_scene)

	await create_timer(2.0).timeout

	var result = DistrhoPluginServer.get_property_list()

	print("output result is ", len(result))

	unload_current_scene()
	quit()
