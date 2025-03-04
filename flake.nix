{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }: let
    systems = [ "x86_64-linux" "aarch64" ];

    forEachSystem = f: with nixpkgs.lib; foldAttrs mergeAttrs {}
      (lists.forEach systems (system:
        mapAttrs (name: value: { ${system} = value; }) (f system)
      ));
  in forEachSystem (system: let
    pkgs = nixpkgs.legacyPackages.${system};
  in rec {
    devShells.default = pkgs.mkShell {
      inputsFrom = [
        packages.default
      ];

      packages = with pkgs; [ ccache doxygen ];

      shellHook = ''
        export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath (with pkgs; [
          wayland
          libxkbcommon
          mesa
          xorg.libX11
          xorg.libXinerama
        ])}:$LD_LIBRARY_PATH

        export CC=clang CXX=clang++
      '';
    };

    packages.default = pkgs.stdenv.mkDerivation {
      name  = "vita";
      pname = "vita";

      src = ./.;

      nativeBuildInputs = with pkgs; [
        clang
        cmake
        ninja
        pkg-config
        wayland-scanner
      ];

      # TODO(binaryskunk): i need to make this prettier, lol
      buildInputs = with pkgs; [ wayland.dev libxkbcommon.dev mesa.dev wayland-protocols xorg.libX11 xorg.libxcb xorg.libXcursor xorg.libXrandr xorg.libXinerama xorg.libXi xorg.libXext xorg.libXi xorg.libXft xorg.libXrender xorg.xcbutil xorg.xcbutilimage xorg.xcbutilkeysyms xorg.xcbutilrenderutil ];
    };
  });
}

