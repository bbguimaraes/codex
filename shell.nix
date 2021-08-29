{ pkgs ? import <nixpkgs> {} }:

pkgs.gcc11Stdenv.mkDerivation {
  name = "gcc11";
  buildInputs = [
    pkgs.autoconf
    pkgs.automake
    pkgs.cmake
    pkgs.gcc11
    pkgs.git
    pkgs.pkgconfig
    pkgs.qt5.qtbase
  ];
}
