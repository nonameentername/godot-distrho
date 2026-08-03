@tool
extends EditorPlugin

var distrho_lv2_export_plugin: DistrhoLv2ExportPlugin


func _enter_tree() -> void:
	distrho_lv2_export_plugin = preload("distrho_lv2_export_plugin.gd").new()
	add_export_plugin(distrho_lv2_export_plugin)


func _exit_tree() -> void:
	remove_export_plugin(distrho_lv2_export_plugin)


func _has_main_screen() -> bool:
	return false


func _get_plugin_name() -> String:
	return "GodotDistrho"
