# Luna Installer

The graphical wrapper around [Luna Manager's](https://github.com/luna/luna-manager) `install` command. Built using Qt and Electron.

## Building the Installer

### Prerequisites

#### Linux
 * git
 * npm
 * cmake >= 2.8.13
 * g++ >= 4.8.3
 * scss lub sassc
 * statically built qt (instructions are [here](https://github.com/luna/installer-electron/wiki/Statically-compiling-Qt): statically linked Qt is necessary when you want to distribute the installer. If you just want to run it locally, you can use the system Qt.)
 * statically build icu (`./configure --disable-shared --enable-static --with-data-package=static`)
 * fontconfig >= 2.8 (may be static, although it should be present on most linuxes)
 * python >=2.7 < 3
 * psutil (`pip install psutil`)


#### Mac OS
 * git
 * npm
 * cmake >= 2.8.13
 * sassc (or other scss to css compiler)


#### Windows
This is the path we followed to build the installer, so that it can be portable. You can rely on system libraries if you don't want to distribute the installer).
* install `msys2`, open it and update (`pacman -Syu` then restart and run `pacman -Syu` again)
* in the `msys2` install: `git`, `cmake`, `make`, `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-qt5-static`, `mingw-w64-x86_64-sassc`
* `mingw-w64-x86_64-qt5-static` appears to have broken dependencies. You'll need to manually install `mingw-w64-x86_64-openssl` and `mingw-w64-x86_64-jasper`
* OUTSIDE of `msys2` install: `python2.7`, `nodejs`, `npm`, and `mt.exe` (they are best installed in their default directories)
* install `psutil` (in `cmd.exe`: `C:\Python27\Scripts\pip.exe install psutil`)
* install `electron-packager` (`C:\Program Files\nodejs\npm.exe install -g electron-packager`)
* add `node` to the `PATH` (unfortunately, the `electron-packager` doesn't work otherwise). Add `export PATH="/c/Program Files/nodejs/:$PATH"` to `~/.bashrc` inside `msys`.


### Compiling

If not installed in the previous step, you need `electron-packager` to be visible on the `PATH`:
```
sudo npm install -g electron-packager
```

You need to have the repo cloned on your machine and a separate directory for the build files. It can look like the following:
```
git clone https://github.com/luna/installer-electron
mkdir build-installer
```

Moreover, you need to have the Luna Manager repo on your machine and build it:
```
git clone https://github.com/luna/luna-manager
cd luna-manager
stack install
```
Let's assume that you have clone the Luna Manager to `$LUNA_MANAGER_DIR`, the Installer to `$INSTALLER_DIR`.

Run the following to generate the makefiles:
```
cmake $INSTALLER_DIR -DCONSOLE_INSTALLER_EXE="$LUNA_MANAGER_DIR/executables/luna_manager" -DCMAKE_BUILD_TYPE=Release
```

If you are using statically linked Qt, remember to add a `-DCMAKE_PREFIX_PATH=/mingw64/qt-static` (on Windows). In general, if the libraries you are using are in non-standard locations, you suuply `-DCMAKE_PREFIX_PATH="path1;path2"`. Analogously, if your Python, npm or sassc are not on your `PATH`, you add `-DPYTHON_EXE="path_to_python"`, `-DNPM_EXE="path_to_npm"`, `-DSASS_EXE="path_to_scss"`. Additionally, on Windows you can add `-DMT_EXE="path_to_mt.exe"`.

If all passes without error, you simply run:
```
make
```
If everything was ok, you should see a folder `lunaInstallerApp` inside the `build-installer` folder, containing the executable.
