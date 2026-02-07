@0xd793ef6eb1602207;

# DistrhoPluginServer (godot)

struct GetLabelRequest {
}

struct GetLabelResponse {
  label @0 :Text;
}

struct GetDescriptionRequest {
}

struct GetDescriptionResponse {
  description @0 :Text;
}

struct GetMakerRequest {
}

struct GetMakerResponse {
  maker @0 :Text;
}

struct GetHomePageRequest {
}

struct GetHomePageResponse {
  homePage @0 :Text;
}

struct GetLicenseRequest {
}

struct GetLicenseResponse {
  license @0 :Text;
}

struct GetVersionRequest {
}

struct GetVersionResponse {
  major @0 :UInt32;
  minor @1 :UInt32;
  patch @2 :UInt32;
}

struct GetUniqueIdRequest {
}

struct GetUniqueIdResponse {
  uniqueId @0 :Text;
}

struct GetNumberOfInputPortsRequest {
}

struct GetNumberOfInputPortsResponse {
  numberOfInputPorts @0 :UInt32;
}

struct GetInputPortRequest {
  index @0 :UInt32;
}

struct GetInputPortResponse {
  hints @0 :UInt32;
  name @1 :Text;
  symbol @2 :Text;
  groupId @3 :UInt32;
  result @4 :Bool;
}

struct GetNumberOfOutputPortsRequest {
}

struct GetNumberOfOutputPortsResponse {
  numberOfOutputPorts @0 :UInt32;
}

struct GetOutputPortRequest {
  index @0 :UInt32;
}

struct GetOutputPortResponse {
  hints @0 :UInt32;
  name @1 :Text;
  symbol @2 :Text;
  groupId @3 :UInt32;
  result @4 :Bool;
}

struct GetParameterRequest {
  index @0 :UInt32;
}

struct GetParameterResponse {
  hints @0 :UInt32;
  name @1 :Text;
  shortName @2 :Text;
  symbol @3 :Text;
  unit @4 :Text;
  description @5 :Text;
  defaultValue @6 :Float32;
  minValue @7 :Float32;
  maxValue @8 :Float32;
  designation @9 :UInt32;
  midiCC @10 :UInt32;
  groupId @11 :UInt32;
  enumerationCount @12 :UInt32;
  result @13 :Bool;
}

struct GetParameterEnumRequest {
  parameterIndex @0 :UInt32;
  index @1 :UInt32;
}

struct GetParameterEnumResponse {
  label @0 :Text;
  value @1 :Float32;
}

struct GetParameterValueRequest {
  index @0 :UInt32;
}

struct GetParameterValueResponse {
  value @0 :Float32;
}

#TODO: same as GodotDistrhoUIServer.  Maybe rename?
#struct SetParameterValueRequest {
#  index @0 :UInt32;
#  value @1 :Float32;
#}
#
#struct SetParameterValueResponse {
#}

struct GetParameterCountRequest {
}

struct GetParameterCountResponse {
  count @0 :UInt32;
}

struct GetProgramCountRequest {
}

struct GetProgramCountResponse {
  count @0 :UInt32;
}

struct GetStateCountRequest {
}

struct GetStateCountResponse {
  count @0 :UInt32;
}

struct ShutdownRequest {
}

struct ShutdownResponse {
  result @0 :Bool;
}

# DistrhoUIServer (godot)

struct GetSomeTextRequest {
}

struct GetSomeTextResponse {
  text @0 :Text;
}

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

# GodotDistrhoUIServer (distrho)

struct EditParameterRequest {
  index @0 :UInt32;
  started @1 :Bool;
}

struct EditParameterResponse {
}

struct SetParameterValueRequest {
  index @0 :UInt32;
  value @1 :Float32;
}

struct SetParameterValueResponse {
}

struct SendNoteRequest {
  channel @0 :UInt8;
  note @1 :UInt8;
  velocity @2 :UInt8;
}

struct SendNoteResponse {
}
