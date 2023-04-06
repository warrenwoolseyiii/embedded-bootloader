#!/bin/bash
uncrustify -c utilities/format/format.cfg --no-backup src/*.c
uncrustify -c utilities/format/format.cfg --no-backup src/*.h
uncrustify -c utilities/format/format.cfg --no-backup src/app/*.c
uncrustify -c utilities/format/format.cfg --no-backup src/app/*.h
uncrustify -c utilities/format/format.cfg --no-backup src/user/*.c
uncrustify -c utilities/format/format.cfg --no-backup src/user/*.h
uncrustify -c utilities/format/format.cfg --no-backup implementations/samd20-optiboot-custom/samd20-optiboot-custom/*.c
uncrustify -c utilities/format/format.cfg --no-backup implementations/samd20-optiboot-custom/samd20-optiboot-custom/*.h
dos2unix src/*.c
dos2unix src/*.h
dos2unix src/app/*.c
dos2unix src/app/*.h
dos2unix src/user/*.c
dos2unix src/user/*.h
git add src/*.c
git add src/*.h
git add src/app/*.c
git add src/app/*.h
git add src/user/*.c
git add src/user/*.h
git add implementations/samd20-optiboot-custom/samd20-optiboot-custom/*.c
git add implementations/samd20-optiboot-custom/samd20-optiboot-custom/*.h
