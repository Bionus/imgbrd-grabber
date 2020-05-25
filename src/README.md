# The `src` directory

This directory contains all actual source code for Grabber, to be built using [CMake](https://cmake.org/). Each sub-directory contains a different project.

Main projects:
* `lib`: the library containing Grabber's logic
* `gui`: all the GUI code for the main Grabber executable
* `sites`: contains the plugins for each Grabber source

Other projects:
* `cmake`: contains the plugins for CMake
* `cli`: an executable designed to be used with a CLI
* `crash-reporter`: an executable that should run when the GUI crashes
* `dist`: contains the files to be packaged together with the Grabber binaries
* `languages`: the GUI translation files

Tests:
* `tests`: unit tests
* `e2e`: end-to-end tests using the CLI