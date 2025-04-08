# Uxn

An assembler and emulator for a modified, harvard architecture version the [Uxn stack-machine](https://wiki.xxiivv.com/site/uxn.html), written in C. 
Currently the assembler is completely non-functional. Rom loading is clunky and will change with the introduction of a new file format. 

## Build

### Linux/OS X

To build the Uxn emulator, you must install [SDL2](https://wiki.libsdl.org/) for your distro. If you are using a package manager:

```sh
sudo pacman -Sy sdl2             # Arch
sudo apt install libsdl2-dev     # Ubuntu
sudo xbps-install SDL2-devel     # Void Linux
brew install sdl2                # OS X
```

Build the assembler and emulator by running the `build.sh` script. The assembler(`uxnasm`) and emulator(`uxnemu`) are created in the `./bin` folder.

```sh
./build.sh 
	--debug # Add debug flags to compiler
	--format # Format source code
	--install # Copy to ~/bin
```

If you wish to build the emulator without graphics mode:

```sh
cc src/devices/datetime.c src/devices/system.c src/devices/console.c src/devices/file.c src/uxn.c -DNDEBUG -Os -g0 -s src/uxncli.c -o bin/uxncli
```

### Plan 9 

To build and install the Uxn emulator on [9front](http://9front.org/), via [npe](https://git.sr.ht/~ft/npe):

```rc
mk install
```

If the build fails on 9front because of missing headers or functions, try again after `rm -r /sys/include/npe`.

### Windows

Uxn can be built on Windows with [MSYS2](https://www.msys2.org/). Install by downloading from their website or with Chocolatey with `choco install msys2`. In the MSYS shell, type:

```sh
pacman -S git mingw-w64-x86_64-gcc mingw64/mingw-w64-x86_64-SDL2
export PATH="${PATH}:/mingw64/bin"
git clone https://git.sr.ht/~rabbits/uxn
cd uxn
./build.sh
```

If you'd like to work with the Console device in `uxnemu.exe`, run `./build.sh --console` instead: this will bring up an extra window for console I/O unless you run `uxnemu.exe` in Command Prompt or PowerShell.

## Getting Started

### Emulator

To launch a `.rom` in the emulator, point the emulator to the target rom file:

```sh
bin/uxnemu bin/piano.rom
```

You can also use the emulator without graphics by using `uxncli`. You can find additional roms [here](https://sr.ht/~rabbits/uxn/sources), you can find prebuilt rom files [here](https://itch.io/c/248074/uxn-roms). 

### Assembler 

The following command will create an Uxn-compatible rom from an [uxntal file](https://wiki.xxiivv.com/site/uxntal.html). Point the assembler to a `.tal` file, followed by and the rom name:

```sh
bin/uxnasm projects/examples/demos/life.tal bin/life.rom
```

### I/O

You can send events from Uxn to another application, or another instance of uxn, with the Unix pipe. For a companion application that translates notes data into midi, see the [shim](https://git.sr.ht/~rabbits/shim).

```sh
uxnemu orca.rom | shim
```

## GUI Emulator Options

- `-2x` Force medium scale
- `-3x` Force large scale
- `-f`  Force fullscreen mode
- `--`  Force next argument to be read as ROM name. (This is useful if your ROM file is named `-v`, `-2x`, and so forth.)

## GUI Emulator Controls

- `F1` toggle zoom
- `F2` toggle debugger
- `F3` quit
- `F4` reboot
- `F5` reboot(soft)
- `F11` toggle fullscreen
- `F12` toggle decorations

### GUI Buttons

- `LCTRL` A
- `LALT` B
- `LSHIFT` SEL 
- `HOME` START
