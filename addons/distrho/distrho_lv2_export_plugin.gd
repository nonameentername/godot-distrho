extends EditorExportPlugin
class_name DistrhoLv2ExportPlugin

const MODE_EXECUTABLE := 493  # Unix 0755

var lv2_feature_enabled: bool
var host_platform: String
var target_platform: String
var target_path: String
var build_type: String


func _export_begin(features: PackedStringArray, is_debug: bool, path: String, flags: int) -> void:
	host_platform = OS.get_name().to_lower()
	target_path = path.get_base_dir()

	for platform in ["linux", "macos", "windows"]:
		if platform in features:
			target_platform = platform

	if not target_platform in ["linux", "macos", "windows"]:
		print("Target platform not supported.")
		return

	build_type = "debug" if is_debug else "release"
	
	lv2_feature_enabled = "lv2" in features and target_platform

	if lv2_feature_enabled:
		var src_dir = (
			"res://addons/distrho/bin/%s/%s/bin/godot-distrho.lv2" % [target_platform, build_type]
		)
		copy_directory(src_dir, target_path)

		var src_file = (
			"res://addons/distrho/bin/%s/%s/bin/godot-plugin" % [target_platform, build_type]
		)

		var dest_file = target_path + "/" + "godot-plugin"

		if target_platform == "windows":
			src_file = src_file + ".exe"
			dest_file = dest_file + ".exe"

		var result = DirAccess.copy_absolute(
			src_file, dest_file, MODE_EXECUTABLE
		)
		if result != OK:
			print("Failed to copy file. Error code: ", result)

		src_file = "res://distrho_plugin_info.json"
		result = DirAccess.copy_absolute(src_file, target_path + "/" + "distrho_plugin_info.json")
		if result != OK:
			print("Failed to copy file. Error code: ", result)


func _export_end() -> void:
	if not lv2_feature_enabled:
		return

	if not target_platform in ["linux", "macos", "windows"]:
		return

	var plugin_extension = "dll" if target_platform == "windows" else "so"
	if target_platform == "macos":
		plugin_extension = "dylib"

	var host_extension = "dll" if host_platform == "windows" else "so"
	if host_platform == "macos":
		host_extension = "dylib"

	var generator_path := (
		"res://addons/distrho/bin/%s/%s/lv2_ttl_generator" % [host_platform, build_type]
	)
	var generator_library_path = (
		"res://addons/distrho/bin/%s/%s/bin/godot-distrho.lv2/godot-distrho_dsp.%s"
		% [host_platform, build_type, host_extension]
	)
	if host_platform == "windows":
		generator_path += ".exe"

	var generator_library_dir = generator_library_path.get_base_dir()
	var info_result = DirAccess.copy_absolute(
		"res://distrho_plugin_info.json", generator_library_dir + "/" + "distrho_plugin_info.json"
	)
	if info_result != OK:
		print("Failed to copy plugin info file. Error code: ", info_result)

	var output := []
	var result := OS.execute(
		ProjectSettings.globalize_path(generator_path),
		[ProjectSettings.globalize_path(generator_library_path), target_path, plugin_extension],
		output,
		true
	)
	if result != OK:
		print("Failed to execute ttl generator. Error code: ", result)


func _get_name() -> String:
	return "DistrhoLv2ExportPlugin"


func copy_directory(src: String, dest: String) -> Error:
	var src_dir := DirAccess.open(src)
	if src_dir == null:
		return DirAccess.get_open_error()

	var err := DirAccess.make_dir_recursive_absolute(dest)
	if err != OK and err != ERR_ALREADY_EXISTS:
		return err

	for directory in src_dir.get_directories():
		err = copy_directory(src.path_join(directory), dest.path_join(directory))
		if err != OK:
			return err

	for file in src_dir.get_files():
		err = DirAccess.copy_absolute(src.path_join(file), dest.path_join(file))
		if err != OK:
			return err

	return OK
