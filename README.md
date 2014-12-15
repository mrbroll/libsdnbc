#libsdnbc

This is a c library containing various data structures and modules that I use in my personal projects.

So far this library contains implementations for:

-   automatic reference counting pointer
-   dictionary
-   gap buffer

This project is not meant to be installed as a system library (yet). It's pretty small. I just include the entire project as a dependency in other projects.

###How To Build

As a shared library

clone into new directory and cd into it, then run:

__Unix__
```
./gradlew releaseSdnbcSharedLibrary
```
__Windows__
```
gradlew.bat releaseSdnbcSharedLibrary
```

The library will be output as <project root>/build/binaries/sdnbcsharedlibrary/release/libsdnbc.so

>   extension may be different depending on your system

As a static library

__Unix__
```
./gradlew releaseSdnbcStaticLibrary
```
__Windows__
```
gradlew.bat releaseSdnbcStaticLibrary
```

The library will be output as <project root>/build/binaries/sdnbcStaticLibrary/release/libsdnbc.a

>   extension may be different depending on your system

To see how to run tests or build debug variants run:

__Unix__
```
./gradlew tasks
```
__Windows__
```
gradlew.bat tasks
```
