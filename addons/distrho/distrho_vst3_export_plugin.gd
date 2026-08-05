extends EditorExportPlugin
class_name DistrhoVst3ExportPlugin

const MODE_EXECUTABLE := 493  # Unix 0755

var vst3_feature_enabled: bool
var host_platform: String
var target_platform: String
var root_target_path: String
var build_type: String


func _export_begin(features: PackedStringArray, is_debug: bool, path: String, flags: int) -> void:
	host_platform = OS.get_name().to_lower()
	root_target_path = path.get_base_dir()

	for platform in ["linux", "macos", "windows"]:
		if platform in features:
			target_platform = platform

	if not target_platform in ["linux", "macos", "windows"]:
		print("Target platform not supported.")
		return

	build_type = "debug" if is_debug else "release"

	vst3_feature_enabled = "vst3" in features and target_platform


func _export_end() -> void:
	if not vst3_feature_enabled:
		return

	if target_platform != "linux":
		return

	var target_path := root_target_path.path_join("Contents/x86_64-linux")
	var plugin_name := root_target_path.get_file().trim_suffix(".vst3")

	var src_dir = (
		"res://addons/distrho/bin/%s/%s/bin/godot-distrho.vst3" % [target_platform, build_type]
	)
	copy_directory(src_dir, root_target_path)

	var src_file = (
		"res://addons/distrho/bin/%s/%s/bin/godot-plugin" % [target_platform, build_type]
	)
	var result = DirAccess.copy_absolute(
		src_file, root_target_path + "/" + "godot-plugin", MODE_EXECUTABLE
	)
	if result != OK:
		print("Failed to copy file. Error code: ", result)

	src_file = "res://distrho_plugin_info.json"
	result = DirAccess.copy_absolute(src_file, root_target_path + "/" + "distrho_plugin_info.json")
	if result != OK:
		print("Failed to copy file. Error code: ", result)

	var err = move_root_files(root_target_path, target_path)
	if err != OK:
		push_error("Failed to organize VST3 bundle: %s" % error_string(err))

	var library_path := target_path.path_join("godot-distrho.so")
	var renamed_library_path := target_path.path_join(plugin_name + ".so")

	err = DirAccess.rename_absolute(library_path, renamed_library_path)
	if err != OK:
		push_error(
			"Failed to rename %s to %s: %s"
			% [library_path, renamed_library_path, error_string(err)]
		)


func _get_name() -> String:
	return "DistrhoVst3ExportPlugin"


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


func move_root_files(src: String, dest: String) -> Error:
	var src_dir := DirAccess.open(src)
	if src_dir == null:
		return DirAccess.get_open_error()

	for file in src_dir.get_files():
		var src_file := src.path_join(file)
		var dest_file := dest.path_join(file)

		var err := DirAccess.rename_absolute(src_file, dest_file)
		if err != OK:
			return err

	return OK
