{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      with pkgs;
      {
        devShells.default = clangStdenv.mkDerivation {
          name = "Tracer-Env";

          nativeBuildInputs = [
            scons
            ninja
            ccls
            clang-tools
            lldb
          ];
          
          buildInputs = [    
          ];
        };
      }
    );
}
