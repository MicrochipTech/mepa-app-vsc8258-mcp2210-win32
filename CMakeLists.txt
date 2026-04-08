cmake_minimum_required(VERSION 3.11)
project(mepa_app_malibu_mcp2210 C)

message (STATUS "[message] Project name: ${CMAKE_PROJECT_NAME}")

###### MEPA Library ######
# Define Paths
set(MEPA_REPO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sw-mepa/")
set(MEPA_DIR "${MEPA_REPO_DIR}/mepa")

# Add mepa_api
include(FetchContent)
set(MEPA_vtss ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_10g ON CACHE BOOL "" FORCE)
set(BUILD_MEPA_DEMO OFF CACHE BOOL "" FORCE)
set(MESA_OPSYS_LINUX OFF CACHE BOOL "" FORCE)
# set(MESA_OPSYS_WIN32 ON CACHE BOOL "" FORCE) # Not Needed Anymore.
set(MEPA_vtss_opt_1g ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_ts ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_macsec ON CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE Debug)

FetchContent_Declare(mepa SOURCE_DIR "${MEPA_DIR}")

#  Bring the targets into the project
FetchContent_MakeAvailable(mepa)

###### MCP2210-HIDAPI Library ######
# Add mcp2210-hidapi; Use CACHE STRING to allow user to specify path to pre-built hidapi.dll and hidapi.lib
set(ARCH "x64" CACHE STRING "Target architecture")
set(MCP2210_HIDAPI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mcp2210-hidapi")

# Add mcp2210-hidapi library src
add_subdirectory(${MCP2210_HIDAPI_DIR}/src) 

# Copy .dll to where executable will be built
configure_file(${MCP2210_HIDAPI_DIR}/include/hidapi/${ARCH}/hidapi.dll ${PROJECT_BINARY_DIR} COPYONLY)

# option(MEPA_1G_PHY "Builds mepa app for lan884x or 1G single PHY" OFF)
# option(MEPA_10G_PHY "Builds mepa app for malibu EVB" ON)

###### Sample Application
message(STATUS "Building MEPA Sample Application for Windows (x64)")

add_executable(${CMAKE_PROJECT_NAME}-win32
    ${CMAKE_CURRENT_SOURCE_DIR}/mepa_appl_custom_malibu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mcp2210_hidapi_mepa_hal.c 
)

# Add Library src/mcp2210-hidapi to this target
target_link_libraries(${CMAKE_PROJECT_NAME}-win32 
    PUBLIC mcp2210-hidapi ${MCP2210_HIDAPI_DIR}/include/hidapi/${ARCH}/hidapi.lib
    PRIVATE mepa
)

# Include Directories
target_include_directories(${CMAKE_PROJECT_NAME}-win32 PUBLIC
    ${MEPA_DIR}/include
    ${MEPA_REPO_DIR}/me/include
    ${MCP2210_HIDAPI_DIR}/include/hidapi/include
    ${MCP2210_HIDAPI_DIR}/include/mcp2210-hidapi
)

# Compile Options (Flags)
target_compile_options(${CMAKE_PROJECT_NAME}-win32 PUBLIC
    -g 
    -Wall 
    -Wextra
    -Wno-error=implicit-function-declaration 
    -Wno-error=int-conversion 
    -Wno-error=incompatible-pointer-types
)

