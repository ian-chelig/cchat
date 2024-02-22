{
  inputs,
  cell,
}: let
  inherit (inputs.std) std lib;
  inherit (inputs) nixpkgs;

  l = nixpkgs.lib // builtins;

  dev = lib.dev.mkShell {
    packages = with nixpkgs; [
      man-pages-posix
      man-pages
    ];

    imports = [
      "${inputs.std.inputs.devshell}/extra/language/c.nix"
    ];

    language.c.compiler = nixpkgs.gcc;



   commands = with nixpkgs; [
        {
          package = gdb;
          category = "build tools";
        }
        {
          package = clang-tools;
          category = "build tools";
        }
        {
          package = gnumake;
          category = "build tools";
        }
        {
          package = std.cli.default;
          category = "dev tools";
        }
      ];
  };
in {
  inherit dev;
  default = dev;
}
