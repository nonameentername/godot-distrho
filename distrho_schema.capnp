@0xd793ef6eb1602207;

struct ParameterRanges {
  def @0 :Float32;
  min @1 :Float32;
  max @2 :Float32;
}

struct Parameter {
  hints @0 :UInt32;
  name @1 :Text;
  shortName @2 :Text;
  symbol @3 :Text;
  unit @4 :Text;
  description @5 :Text;
}

interface PluginInterface {
  getLabel @0 () -> (label :Text);
  getDescription @1 () -> (description :Text);
  getMaker @2 () -> (maker :Text);
  getHomePage @3 () -> (homepage :Text);
  getLicense @4 () -> (license :Text);

  getVersion @5 () -> (version :UInt32);
  getUniqueId @6 () -> (uniqueId :Int64);

  initParameter @7 (index :UInt32) -> (parameter :Parameter);
  getParameterValue @8 (index :UInt32) -> (value :Float32);
  setParameterValue @9 (index :UInt32, value :Float32) -> ();
}
