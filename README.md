# embedded-bootloader
Open source generic bootloader that can be utilized by various microcontrollers. This project provides a simple bootloader based on the original dual optiboot bootloader by [low power labs](https://github.com/LowPowerLab/DualOptiboot). The basic principle of operation is the following:
- An application writes a full firmware image to an external flash memory chip formatted in the way our bootloader expects
- The application resets and enters the bootloader
- The bootloader searches flash memory for the header, if it finds a valid header it will attempt to load the program from external flash memory into ROM
- If successful, the bootloader will try to jump to the application

There are also stubs for checking the presence of an application outside of bootloader space, and a stub for implementing a serial bootloader as well.

# project structure
## source code
This project actually makes up both ends of the bootloader, providing generic source code for the bootloader, and the application side which we call `hex_writer`. Shown below in the file tree is an explanation of the different subdirs and folders.
```
src -|
     | - app - -> this contains the source code for the hex_writer module.
     | - deps - -> this contains any external dependencies that the bootloader relies on.
     | - user - -> this contains the user stubs which must be populated for your implementation.
     | bl_version.h -> version of the bootloader.
     | optboot.c / .h -> source code for the bootloader.
     | printf.c / .h -> lightweight printf library if you need debugging.
```
## unit tests
In the `test` directory, you will find full unit test coverage of the generic bootloader implementation. `test/user_imp.c` can provide an example implementation for your custom bootloader as well. See the README in the `test` directory on how to run unit tests.
## utilities
Contains a bunch of versioning scripts for auto versioning, and scripts to download a targeted version of the [embedded-external-flash-memory](https://github.com/warrenwoolseyiii/embedded-external-flash-memory) project, which is a dependency of this bootloader.
## implementations
Any implementations that are written for a personal or allowable open source project will be placed in here. Feel free to message the owners of the repo if you'd like help writing an implementation for yourself.

# developing from the source
Feel free to fork, or clone this repo directly. If you'd like to contribute you can make a pull request into `develop` with your improvements.
If you'd like to create your own implementation, simply clone or fork the repo and create a new implementation in the `implementations` folder following the naming convention of the other projects. You will need to include the files from:
```
- src/*.c, src/*.h
- src/user/*.h
- src/deps/*
```
## bootloader steps
1. Copy the source code from `src/user/user_hooks.c` into a new file of your choosing. I usually name mine `XXX_cust_user_imp.c`.
2. Fill in the processor specific code for each of the stubs in your user implementation file.
3. Adjust the #define fields in `src/user/user_settings.h` to fit your needs.
4. Call `optiboot()` from the main function of your bootloader program to run the bootloader.
## hex writer steps
The hex writer is prepackaged source code that automatically takes intel 16 hex files and writes them directly to external flash memory in the format the bootloader is expecting. It is recommeneded that you implement the hex writer in your application so that the firmware images are properly formatted. The hex writer also has dependencies to the `embedded-external-flash-memory` project.
1. Place the src/app/hex_record_writer.* files in your application.
2. Place the src/deps/embedded-external-flash-memory/* files in your application.
3. Implement the stubs for the flash memory dependencies (see that repo's README for more information).
4. See the README in the src/app/ folder for details on implementation

# developing from releases
Releases package the source code for the bootloader and the hex writer program into tar.gz assets. You can follow the same steps in the `developing from source` section to setup your application. In the future there will be an example script that will download the latest release and extract the files. We use a script that does something simlar for the embedded-external-flash-memory file in the `utilities/scripts/dl_deps.sh`. 
