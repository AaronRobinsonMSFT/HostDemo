Advanced .NET Core Hosting Demo
================

This project demonstrates a possible way for a Java process to host .NET Core using the `nethost` library. Documentation on the `nethost` library can be found [here](https://github.com/dotnet/core-setup/blob/master/Documentation/design-docs/native-hosting.md).

Key Features
------------

Demonstrates how to locate and initialize .NET Core 3.0 from a non-.NET Core process and subsequently load and call into  a .NET Core assembly.

Addtional comments are contained in source and project files.

Requirements
------------

* .NET Core 3.0 (At least Preview 7) - [https://dot.net](https://github.com/dotnet/core-sdk#installers-and-binaries)

* Java JDK - https://jdk.java.net/

   * The bitness and platform of the install JDK should match the RID of the installed .NET Core 3.0. For example, if the user installed the 64-bit Windows .NET Core 3.0 SDK, then the JDK should also be the 64-bit Windows version. 

* CMake - https://cmake.org/

Build and Run
-------------

1) In order to build and run, all requirements must be installed. The following must also be done:

    * The `JDKRoot` property in `./Directory.Build.props` should be updated.

    * The `cmake` binary must be on the path.

1) Navigate to the root directory and run `dotnet build`.

1) Navigate to the output directory for the configuration (i.e. `Debug` is the default).

    * Windows: `cd bin\Debug`

    * Non-Windows: `cd bin/Debug`

1) Run the Java entry point. If the `java` binary is not on the path, the below command should be updated with the absolute path to the `java` command instead.

    * `java JavaHost`

The expected output will come from the `DotNetLib` class library.

Scenario pain points
--------------
See the `dotnetbridge.c` for examples on how to mitigate each of the following:

* The `nethost.h` should include defines for the `.runtimeconfig.json` extension

* API defintions for calling conventions instead of looking through documentation

* Helper macros for narrow vs wide characters. These are provided on Windows via the `T()` macros
