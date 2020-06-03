# The `scripts` directory

This directory is used to store useful scripts that make developing on Grabber easier. This is **not** for end-user scripts for Grabber itself.

Useful scripts:
* `build.sh [TARGET]...` to build Grabber
* `package.sh DIR` to copy all common artifacts in a target directory
* `windows-setup/` contains the [InnoSetup](https://jrsoftware.org/isinfo.php) installer script for Windows

_Note: most of those scripts are made to be run from the root of the repository (ex: `./scripts/build.sh`)._