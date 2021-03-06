include(PaddockFindPackage)

paddock_find_package(Qt5 COMPONENTS Core Qml Quick  QuickWidgets QuickControls2
                     REQUIRED QUIET)
paddock_find_package(Qt5QuickCompiler REQUIRED QUIET)

set(THREADS_PREFER_PTHREAD_FLAG ON)
paddock_find_package(Threads REQUIRED QUIET)

# Linux dependencies
paddock_find_package(alsa QUIET)
paddock_find_package(liblo QUIET)

paddock_find_package_post()