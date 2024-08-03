{
  inputs = 
  {
    nixpkgs.url = "nixpkgs/nixos-unstable";
  };
  outputs = { nixpkgs, ... }: 
  let 
    allSystems =  [
      "x86_64-linux"
      "aarch64-darwin"
      "aarch64-linux"
    ];
    forAllSystems = fn:
      nixpkgs.lib.genAttrs allSystems
      (system: fn 
        { 
          inherit system;
          pkgs = import nixpkgs { inherit system; }; 
        }
      );
  in
  {
    devShells = forAllSystems ({ pkgs, system }: {
      default = pkgs.mkShell  {
        name = "snack";
        nativeBuildInputs = 
        with pkgs;
        [
          clang
        ];
      };
    });
  };
}
