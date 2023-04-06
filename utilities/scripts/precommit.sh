#!/bin/sh
echo "Running pre-commit hook..."
# Change the current working directory to the root of the project
./utilities/scripts/dl_deps.sh
./utilities/version/version.sh
./utilities/format/format.sh