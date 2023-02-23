#!/bin/bash

# Set the repository name and owner
REPO_OWNER="warrenwoolseyiii"
REPO_NAME="embedded-external-flash-memory"
# The input JSON file
input_file="utilities/version/version.json"
ext_flash_ver=$(cat $input_file | jq -r '.EXT_FLASH_VERSION')

#If the ser_prot_ver is not "latest", place "tags/" before the version number
if [ "$ext_flash_ver" != "latest" ]; then
    ext_flash_ver="tags/$ext_flash_ver"
fi

echo "C flash memory version: $ext_flash_ver"

# Get the latest release information from the API
LATEST_RELEASE=$(curl -s https://api.github.com/repos/$REPO_OWNER/$REPO_NAME/releases/$ext_flash_ver)

# Extract the URL for the latest release assets
ASSET_URL=$(echo $LATEST_RELEASE | jq -r '.assets[0].browser_download_url')

# Download the latest release asset
wget $ASSET_URL -O bl_assets.tar.gz

# Extract the C source files from the asset, place them in a file called tmp
mkdir tmp
tar -xzf bl_assets.tar.gz -C tmp

# Move the extracted files to the desired directory
mv tmp/src/* src/deps/embedded-external-flash-memory
rm bl_assets.tar.gz
rm -rf tmp
