# Engine modules

## Core
Provides all basic types for other modules.

* Primitive types
* Memory management
* Logging
* Container types
    * Array
    * Map
    * Path
    * String
* Math
    * Color
    * Vectors
    * Matrix
    * Time

## Platform
Responsible for handling platform specific actions in an abstract interface.

Also runs the application logic. Platform implementations must declare the entrypoint and run the application logic.

## Graphics
Abstract interface to interact with the rendering hardware.

* Graphics interface
* Adapter
* Command list
