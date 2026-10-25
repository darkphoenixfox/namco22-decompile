# Time Crisis 2 for PC

Namco's 1997 arcade light-gun shooter **Time Crisis II** (System 23), rebuilt to run natively on a PC: the game's own programs
-- the MIPS main CPU, the H8 sound / sub CPU and the gun I/O board's H8 -- are translated to C, and the hardware around them
(the 3D chips, the text layer, the C352 sound chip, the JVS link, the light gun) is our own code. Nothing interprets ROM code;
MAME was used only as a reference to check the translation against, instruction for instruction.

**No game data is included.** You need your own copy of MAME's ROM set `timecrs2.zip` (Time Crisis II, US TSS3 Ver. B).

## Download

| Package | For |
|---|---|
| `TimeCrisis2-x86_64.AppImage` | Linux (64-bit) and the **Steam Deck** -- `chmod +x` it and run it |
| `TimeCrisis2-windows-VERSION.zip` | Windows 10 / 11 (64-bit) -- unzip, put `timecrs2.zip` in `roms`, double-click `TimeCrisis2.exe` |

Keys, light guns, recoil and where the files go: [HOW TO PLAY](packaging/linux/HOW-TO-PLAY.txt).

## Building from source (Linux)

```bash
./build.sh /path/to/timecrs2.zip     # unpacks and checks the ROM, translates the two H8 programs, builds
./launch.sh                          # play (./launch.sh 2 = a 2x window, --fullscreen)
```
Needs gcc, cmake, pkg-config, python3 and the SDL2 / zlib / OpenGL development files; the game program is ~100 MB of generated C
(`gen/tc2_lifted_*.c`), so allow about 1 GB of memory per parallel compile (`JOBS=2 ./build.sh ...`).
Windows: `./build-windows.sh` (MinGW-w64 cross compiler) after `./build.sh`. Releases: `tools/release.sh VERSION`.

## What works

Attract, coins, play with the light gun / mouse / pad (stages 1 and 2 checked so far), sound, the operator's test menu, the battery
RAM (settings and rankings) saved between sessions, widescreen, any internal resolution, frame-rate options, gun recoil output, pad rumble on each shot (Display > Pad rumble on recoil),
your own crosshair picture (PNG files in a `crosshairs/` folder beside the game, Display > Crosshair image).
Two players: the menu's **Cabinet** page picks your side (Left/Red = Richard, Right/Blue = Keith), and the **Online** page links
two copies over the internet (zonesync.net) or a LAN for the arcade's LINK PLAY co-op -- the twin cabinets' C422 link, which MAME
does not emulate. **Two guns on one PC**: Online > "Second player on this computer" opens a second copy (one monitor each, or side by
side) that joins by itself; Controls > "Gun device" gives each copy its own gun, read directly from the device (Linux evdev, Windows
Raw Input) instead of the system pointer that merges them.

## Beyond MAME

Things this version does that MAME does not: the C404 chip's gamma table is applied (MAME does not use it), a GPU renderer at any
resolution and in widescreen, recoil output on the game's own solenoid signal.

## Stereo 3D and Leia SR displays (this fork)

This fork ([darkphoenixfox/namco22-decompile](https://github.com/darkphoenixfox/namco22-decompile)) adds stereo 3D,
glasses-free 3D on Leia SR displays such as the **Samsung Odyssey 3D**, and head tracking. The game's own camera is kept: each eye is
the arcade's view moved a few centimetres sideways, drawn by the same GPU renderer at the same quality, and the two eyes average to the
arcade picture exactly. It is not a free camera.

### Turning it on

**Display > 3D** (or **F9** to cycle):

| Mode | For |
|---|---|
| OFF | the ordinary picture |
| Side by side (left eye left) | 3D TVs and displays with a side-by-side mode: each half of the screen is the whole picture squeezed to half width |
| Cross-eyed (eyes swapped) | free-viewing the pair without glasses |
| Anaglyph (red / cyan glasses) | a quick check on any monitor |
| Leia SR display (Odyssey 3D) | a Leia SR display driven directly: its eye tracker and weaver interlace the picture for your eyes (Windows) |

Also on the Display page:

- **3D depth (eye separation)**: 65 mm by default, the human eye distance; 0 is flat.
- **3D screen distance**: how far into the scene the screen surface sits, 4 m by default. Things nearer than this come out of the
  screen. Set it near where the enemies stand for the most comfortable view and the most accurate aim.

The HUD, the text, the crosshair, the shot impacts and the menu are kept flat at the screen surface. The game draws some HUD art and
the impacts as 3D models at made-up depths; those are recognised (by model number, and anything nearer than about 45 cm) and not moved.

**F12** in 3D saves both eyes at full resolution side by side (a stereo pair) in `screenshots/`.

### Leia SR displays (Samsung Odyssey 3D)

Needs the **Leia SR platform** installed (it comes with the display's software; check that the `SR Service` is running) and
**`tc2_leia.dll` beside the game**. Choose **Display > 3D > Leia SR display**: the game connects to the SR Service, moves its window
onto the 3D display and weaves the picture itself, so the Odyssey 3D Hub and its side-by-side conversion are not needed. Without the
DLL or the platform the mode says why and the game stays in 2D.

The crosshair, the gun border and the menu are drawn into both eyes alike, so they sit flat on the screen and stay sharp.

### Head tracking (Leia SR displays)

The display's eye tracker can do more than steer the 3D:

- **3D look-around**: moving your head moves the camera with it, the screen acting as a window onto the arcade's view. Lean to see a
  little further round a corner. 0-200 % of your head's movement (100 % = 10 cm of head movement moves the camera 10 cm in the game),
  capped at 25 cm; if the tracker loses your eyes the view eases back to the arcade's. Shots stay true while you lean: the gun's
  aim is traced into the picture you see and handed to the game as the same spot in the arcade's own view (the crosshair stays where
  the gun points).
- **3D duck to take cover**: your head becomes the arcade's foot pedal. Upright = pedal held (out of cover); duck more than the set
  distance (15 cm by default, 5-40 cm) below your upright height = pedal released (take cover, reload). Your upright height is learned
  as you play; ducking out of the tracker's view counts as taking cover. The keys, mouse and gun buttons still work the pedal too.
- **F10** makes where your head is now the centre: look-around measures from there and duck-to-cover takes it as upright. Saved
  between sessions. **Shift+F10** goes back to the display's own default position. Sit as you will play and press F10 once.

### Light gun border (Sinden)

**Gun border** (Controls or Display page, **F8**) now goes from 0.5 % to 10 % in 0.5 % steps (Left / Right; Enter and F8 step whole
percents as before), and **Display > Gun border position** chooses:

- **round the picture**: as before, the picture shrinks inside the frame;
- **over the 4:3 game, on top**: a 4:3 frame round exactly the area the gun aims at, drawn over the picture without shrinking it (also
  in widescreen). In 3D it is drawn into both eyes alike, so the screen shows one sharp frame.

### Windows display options

- **Window mode > Borderless (for 3D apps)**: a borderless window over the whole monitor that other programs' windows can sit in front
  of (an exactly monitor-sized OpenGL window is taken as fullscreen by NVIDIA's driver and covers everything).
- Fullscreen is no longer "always on top".
- The game is now **DPI-aware**: on a display scaled by Windows (a 4K panel at 150 %) it draws at the panel's own resolution instead
  of being stretched, which is sharper and is required for Leia SR weaving.

### Building on Windows (MSYS2)

Without a Linux machine, the game builds natively with [MSYS2](https://www.msys2.org/). In the **MSYS2 UCRT64** shell:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-{gcc,cmake,ninja,SDL2,zlib,pkgconf,python}
cd tc2
python3 tools/setup_roms.py /path/to/timecrs2.zip     # unpacks and checks the ROM into extracted/
python3 tools/make_prog.py extracted
cmake -S . -B build-win/native -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-win/native --target tc2 -j8         # allow ~1 GB of memory per parallel compile
```

The result is `build-win/native/tc2.exe`, needing no DLLs beyond Windows'. Put `timecrs2.zip` in a `roms/` folder beside it.

**`tc2_leia.dll`** is built with Microsoft's compiler (Leia's SDK is MSVC C++) from the
[LeiaSR SDK](https://www.leiainc.com/) and Visual Studio 2022 or its Build Tools with the C++ workload, from a Windows command prompt:

```bat
tc2\src\leia\build.bat C:\path\to\LeiaSDK C:\path\to\the\game\folder
```

The game loads it at run time only when Leia mode is chosen, so builds and computers without it are unaffected.

### Credits

The HUD models drawn at made-up depths and the game's scale (2/3 of a unit to the metre) come from the
[Time Crisis II VR port](https://github.com/DR-89/time-crisis-2-vr) by DR-89 (MIT); the take-cover icon (model 3039) and the near-depth
rule were found here.
