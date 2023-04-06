# The input JSON file
$input_file = "utilities/version/version.json"

# Parse the JSON file to extract the constants
$json = Get-Content $input_file | ConvertFrom-Json
$major = $json.BL_VERSION_MAJOR
$minor = $json.BL_VERSION_MINOR
$rev = $json.BL_VERSION_REV

# Echo the version string
echo "$major.$minor.$rev"
