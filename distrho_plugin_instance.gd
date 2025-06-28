extends DistrhoPluginInstance

func _init() -> void:
    DistrhoServer.set_distrho_plugin(self)

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
    return "godt"
