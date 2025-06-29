@0xd793ef6eb1602207;

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
