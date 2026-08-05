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
		var result = DirAccess.copy_absolute(
			src_file, target_path + "/" + "godot-plugin", MODE_EXECUTABLE
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

	if target_platform in ["linux", "macos", "windows"] and host_platform != target_platform:
		print("Target platform does not match host.  Will not export ttl files")
		return
	elif not target_platform in ["linux", "macos", "windows"]:
		return

	var src_file = "res://addons/distrho/bin/%s/%s/lv2_ttl_generator" % [host_platform, build_type]
	var dest_file = target_path + "/" + "lv2_ttl_generator"

	if host_platform == "windows":
		src_file = src_file + ".exe"
		dest_file = dest_file + ".exe"

	var result = DirAccess.copy_absolute(src_file, dest_file, MODE_EXECUTABLE)
	if result != OK:
		print("Failed to copy file. Error code: ", result)

	var src_script: String
	var dest_script: String

	if host_platform == "windows":
		src_script = (
			"res://addons/distrho/bin/%s/%s/run_windows_ttl_generator.bat"
			% [host_platform, build_type]
		)
		dest_script = target_path + "/" + "run_windows_ttl_generator.bat"
	else:
		src_script = (
			"res://addons/distrho/bin/%s/%s/run_%s_ttl_generator.sh"
			% [host_platform, build_type, host_platform]
		)
		dest_script = target_path + "/" + "run_%s_ttl_generator.sh" % [host_platform]

	result = DirAccess.copy_absolute(src_script, dest_script, MODE_EXECUTABLE)
	if result != OK:
		print("Failed to copy file. Error code: ", result)

	var output = []

	result = OS.execute(dest_script, [], output)
	if result != OK:
		print("Failed to execute. Error code: ", result)

	DirAccess.remove_absolute(dest_file)
	DirAccess.remove_absolute(dest_script)


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
