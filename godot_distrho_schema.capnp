@0xd793ef6eb1602207;

# DistrhoPluginServer (godot)

struct SetStateValueRequest {
  key @0 :Text;
  value @1 :Text;
}

struct SetStateValueResponse {
}

struct ShutdownRequest {
}

struct ShutdownResponse {
  result @0 :Bool;
}

# DistrhoUIServer (godot)

struct GetNativeWindowIdRequest {
}

struct GetNativeWindowIdResponse {
  id @0 :UInt64;
}

struct ParameterChangedRequest {
  index @0 :UInt32;
  value @1 :Float32;
}

struct ParameterChangedResponse {
}

struct StateChangedRequest {
  key @0 :Text;
  value @1 :Text;
}

struct StateChangedResponse {
}

# GodotDistrhoUIServer (distrho)

struct EditParameterRequest {
  index @0 :UInt32;
  started @1 :Bool;
}

struct EditParameterResponse {
}

struct SendNoteRequest {
  channel @0 :UInt8;
  note @1 :UInt8;
  velocity @2 :UInt8;
}

struct SendNoteResponse {
}

struct SetStateRequest {
  key @0 :Text;
  value @1 :Text;
}

struct SetStateResponse {
  result @0 :Bool;
}

# GodotDistrhoPluginServer (distrho)

struct UpdateStateValueRequest {
  key @0 :Text;
  value @1 :Text;
}

struct UpdateStateValueResponse {
  result @0 :Bool;
}
