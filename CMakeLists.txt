cmake_minimum_required(VERSION 3.14)
project(05_cloud_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch springbootplusplus_web (HTTP request/response, dispatcher, etc.)
FetchContent_Declare(
    springbootplusplus_web
    GIT_REPOSITORY https://github.com/compilerNayan/springbootplusplus_web.git
    GIT_TAG        main
)

# Fetch 03_device_connectivity (IWiFiConnectionStatusProvider, etc.)
FetchContent_Declare(
    03_device_connectivity
    GIT_REPOSITORY https://github.com/compilerNayan/03_device_connectivity.git
    GIT_TAG        main
)

# Fetch cpp_core (IRunnable, StandardDefines, etc.)
FetchContent_Declare(
    cpp_core
    GIT_REPOSITORY https://github.com/compilerNayan/cpp_core.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(springbootplusplus_web 03_device_connectivity cpp_core)

# Header-only INTERFACE library; exposes springbootplusplus_web
add_library(05_cloud_server INTERFACE)

set_target_properties(05_cloud_server PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(05_cloud_server INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link springbootplusplus_web (provides HttpRequestManager, IServer, ILogger, etc.)
if(TARGET springbootplusplus_web::springbootplusplus_web)
    target_link_libraries(05_cloud_server INTERFACE springbootplusplus_web::springbootplusplus_web)
elseif(TARGET springbootplusplus_web)
    target_link_libraries(05_cloud_server INTERFACE springbootplusplus_web)
else()
    FetchContent_GetProperties(springbootplusplus_web)
    if(springbootplusplus_web_SOURCE_DIR)
        target_include_directories(05_cloud_server INTERFACE
            $<BUILD_INTERFACE:${springbootplusplus_web_SOURCE_DIR}/src>
        )
    endif()
endif()

# Link cpp_core (provides IRunnable, StandardDefines)
if(TARGET cpp_core::cpp_core)
    target_link_libraries(05_cloud_server INTERFACE cpp_core::cpp_core)
elseif(TARGET cpp_core)
    target_link_libraries(05_cloud_server INTERFACE cpp_core)
endif()

# Link 03_device_connectivity (provides IWiFiConnectionStatusProvider)
if(TARGET 03_device_connectivity::03_device_connectivity)
    target_link_libraries(05_cloud_server INTERFACE 03_device_connectivity::03_device_connectivity)
elseif(TARGET 03_device_connectivity)
    target_link_libraries(05_cloud_server INTERFACE 03_device_connectivity)
else()
    FetchContent_GetProperties(03_device_connectivity)
    if(03_device_connectivity_SOURCE_DIR)
        target_include_directories(05_cloud_server INTERFACE
            $<BUILD_INTERFACE:${03_device_connectivity_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${03_device_connectivity_SOURCE_DIR}>
        )
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS 05_cloud_server
    EXPORT 05_cloud_serverTargets
)

install(EXPORT 05_cloud_serverTargets
    FILE 05_cloud_serverTargets.cmake
    NAMESPACE 05_cloud_server::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/05_cloud_server
)
