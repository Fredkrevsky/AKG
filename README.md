# AKG
Algorithms of Computer Graphics (6th semester BSUIR)

## Dependences

- [Conan](https://conan.io/) — packet manager for CMake.
- [SFML](https://www.sfml-dev.org/) — multimedia library.

## Build and run

   ```bash
   pip install conan
   conan profile detect --force
   conan build . --build=missing
   ./build/Release/src/Lab1
   ```
