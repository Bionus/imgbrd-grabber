# The `dist` directory

This directory is used to store files that must be packaged together with the binary. They can be package-related (metadata about the application), or Grabber-specific files.

The `common` directory is shared for all platforms.
Others, such as `windows` or `macos`, are platform-dependent.

So for example, on the Windows platform, all the contents of `common` and `windows` **must** be packaged together with the `Grabber.exe` binary file.