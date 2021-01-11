# SudokuScanAndSolve
Scanning Sudokus from Paper or Image, Creating, Playing, Solving.


## Build

Builds are tested for Mac, Windows, Ubuntu Linux, and Android cross compiled on Ubuntu Linux.

### Prerequisites

For sure you need a command shell, git, cmake, a c++17 compliant compiler, Qt5.15 development install. Besides, it's a bit hard to tell what exactly is needed for building OpenCV. But OpenCV's cmake config will probably tell you if something is missing.
If you want to cross compile for Android, you also need a recent Android SDK and NDK.

### OpenCV submodule

First thing to do is init and update the submodule: `git submodule update --init --recursive`.
Depending on what platform you want to build for, you should have a look into build_`<Platform>`.config inside OpenCV folder. Paths etc are set like they are on my machines, but will probably be different on yours.
You can either call the OpenCV build script now , or let cmake do it later (if it notices that there is no OpenCV-4.5.1 folder).
If you want to do it now, cd into OpenCV folder and call the script:
`./build.sh <Platform> 4.5.1`.
As mentioned above, config step might tell you if something is missing. Build process can take some time.

### Desktop Build

cd into Qt folder. Then

```
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=<PATH_TO_QT> ..
cmake --build .
```

As an example, `PATH_TO_QT` is `/opt/Qt/5.15.2/gcc_64` on my machine.


### Android Build

cd into Qt folder. Have a look into `android_build.sh`. Again, you will probably have to adapt some paths etc there to fit to your computer. Then do `./android_build.sh`. apks and bundle will be created in folder `android_build/android-build/build/outputs/`.


## Third Party:

- Qt
- OpenCV for Sudoku recognition
- Google's Material design for icons
