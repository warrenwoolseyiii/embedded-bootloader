import os

makefile_path = "implementations/samd20-optiboot-custom/samd20-optiboot-custom/Debug/makedep.mk"
build_dir = "implementations/samd20-optiboot-custom/samd20-optiboot-custom/Debug/"

with open(makefile_path) as f:
    for line in f:
        # Strip newlines and leading/trailing spaces
        line = line.strip()

        # Only process non-empty lines
        if line:
            # Split the path into its individual directories
            path_parts = line.split(os.path.sep)

            # Traverse the directory tree, creating each directory as necessary
            current_dir = build_dir
            for part in path_parts[:-1]:
                current_dir = os.path.join(current_dir, part)
                os.makedirs(current_dir, exist_ok=True)
