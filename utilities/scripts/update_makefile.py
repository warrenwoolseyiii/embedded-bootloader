import re
import os

def update_makefile(makefile_path, original_path, new_path):
    with open(makefile_path, 'r') as file:
        filedata = file.read()

    filedata = filedata.replace(original_path, new_path)

    with open(makefile_path, 'w') as file:
        file.write(filedata)

makefile_path = 'implementations/samd20-optiboot-custom/samd20-optiboot-custom/Debug/Makefile'
original_path = r'C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin'
new_path = r'C:\ProgramData\Chocolatey\bin'

cmsis_path = os.path.join(os.getcwd(), "CMSIS_5.4.0", "CMSIS_5-5.4.0", "CMSIS", "Core", "Include")
samd20_dfp_path = os.path.join(os.getcwd(), "SAMD20_DFP", "samd20", "include")
original_cmsis_path = r'C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\CMSIS\5.4.0\CMSIS\Core\Include'
original_samd20_dfp_path = r'C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMD20_DFP\1.3.165\samd20\include'

update_makefile(makefile_path, original_cmsis_path, cmsis_path)
update_makefile(makefile_path, original_samd20_dfp_path, samd20_dfp_path)
update_makefile(makefile_path, original_path, new_path)
