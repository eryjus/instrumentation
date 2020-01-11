# Bochs Instrumenatation for CenturyOS

## Additional components

I had to add the following to my own host system to get Bochs to compile:
* `sudo dnf install dtk2-devel`
* `sudo dnf install SDK-devel`
* `sudo dnf install SDK2-devel`

You may need more or less depending on your own environment.  Maybe one day I will try this on a newly minted system and see what else I need to do to get this running.


## Build

This module is intended to be installed and built as part of Bochs.  When you install this project on your own system, you need to have a copy of Bochs source already installed.  Then this project is intended to be copied into the folder: `bochs-<ver>/instrument/`.

Then I use the following configure command for Bochs:

```bash
./configure --enable-smp --enable-cpu-level=6 --enable-all-optimizations --enable-x86-64  --enable-pci --enable-vmx --enable-debugger --enable-disasm --enable-debugger-gui --enable-logging --enable-fpu --enable-3dnow --enable-sb16=dummy --enable-cdrom --enable-x86-debugger --enable-iodebug --disable-plugins --disable-docbook --with-x --with-x11 --with-term --with-sdl2 --enable-instrumentation="instrument/instrumentation"
```

and then make was simply `make`.

The I added some symlink to my own `~/bin` folder to point to the newly made executable.  This is of course optional for you -- do whatever you need to do for your own host system.

