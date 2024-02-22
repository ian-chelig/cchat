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

   commands = with nixpkgs; [
        {
          package = cmake;
          category = "build tools";
        }
        {
          package = gdb;
          category = "build tools";
        }
        {
          package = clang-tools;
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
