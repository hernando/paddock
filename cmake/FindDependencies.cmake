include(PaddockFindPackage)

paddock_find_package(Qt5 COMPONENTS Core Qml Quick  QuickWidgets QuickControls2
                     REQUIRED QUIET)
paddock_find_package(Qt5QuickCompiler REQUIRED QUIET)
paddock_find_package(liblo QUIET)

paddock_find_package_post()