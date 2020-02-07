cmake_minimum_required(VERSION 3.15..3.16 FATAL_ERROR)

set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in")

project(Filewatcher
    VERSION ${project_version}
    DESCRIPTION ${project_description}
    LANGUAGES C CXX 
)

include (GNUInstallDirs)


include(FetchContent)

FetchContent_Declare(
  poco
  GIT_REPOSITORY "https://github.com/pocoproject/poco.git"
  GIT_TAG        b95393dcc3640807838e8323b4e600e54d2e8116 # poco-1.9.4-release
)
FetchContent_GetProperties(poco)
if(NOT poco_POPULATED)
    message(STATUS "Downloading POCO...")
    FetchContent_Populate(poco)

    option(POCO_STATIC "Build a static library" ON)
    option(DISABLE_CPP14 "Do not use C++14 features" ON)
    option(ENABLE_ENCODINGS "Enable Encodings" OFF)
    option(ENABLE_ENCODINGS_COMPILER "Enable Encodings Compiler" OFF)
    option(ENABLE_XML "Enable XML" OFF)
    option(ENABLE_JSON "Enable JSON" OFF)
    option(ENABLE_MONGODB "Enable MongoDB" OFF)
    option(ENABLE_REDIS "Enable Redis" OFF)
    option(ENABLE_PDF "Enable PDF" OFF)
    option(ENABLE_UTIL "Enable Util" ON)
    option(ENABLE_NET "Enable Net" OFF)
    option(ENABLE_NETSSL "Enable NetSSL" OFF)
    option(ENABLE_NETSSL_WIN "Enable NetSSL Windows" OFF)
    option(ENABLE_CRYPTO "Enable Crypto" OFF)
    option(ENABLE_DATA "Enable Data" OFF)
    option(ENABLE_DATA_SQLITE "Enable Data SQlite" OFF)
    option(ENABLE_DATA_MYSQL "Enable Data MySQL" OFF)
    option(ENABLE_DATA_ODBC "Enable Data ODBC" OFF)
    option(ENABLE_SEVENZIP "Enable SevenZip" OFF)
    option(ENABLE_ZIP "Enable Zip" OFF)
    option(ENABLE_APACHECONNECTOR "Enable ApacheConnector" OFF)
    option(ENABLE_CPPPARSER "Enable C++ parser" OFF)
    option(ENABLE_POCODOC "Enable Poco Documentation Generator" OFF)
    option(ENABLE_PAGECOMPILER "Enable PageCompiler" OFF)
    option(ENABLE_PAGECOMPILER_FILE2PAGE "Enable File2Page" OFF)

    add_subdirectory(${poco_SOURCE_DIR} ${poco_BINARY_DIR})
endif()

if(NOT TARGET POCO::Foundation)
    add_library(POCO::Foundation ALIAS Foundation)
endif()

FetchContent_Declare(
    ghc_filesystem
    GIT_REPOSITORY https://github.com/gulrak/filesystem.git
    # GIT_TAG v${ghc_filesystem_version}
    GIT_TAG 8facfa004441255dfa7f83dd21ea7bed70786656  # v1.2.10
)
FetchContent_GetProperties(ghc_filesystem)
if(NOT ghc_filesystem_POPULATED)
    message(STATUS "Downloading gulrak's filesystem...")
    FetchContent_Populate(ghc_filesystem)
endif()

if(NOT TARGET filesystem)
  add_library(filesystem INTERFACE)
  set(FILESYSTEM_INC_DIR ${ghc_filesystem_SOURCE_DIR}/include)
  target_include_directories(filesystem SYSTEM INTERFACE ${FILESYSTEM_INC_DIR})
endif()

FetchContent_Declare(
    lyra
    GIT_REPOSITORY https://github.com/bfgroup/Lyra.git
    # GIT_TAG ${lyra_version}
    GIT_TAG 91e7ec8bbef10f92fd364d91e0b2aec603414dfe # 1.2
)
FetchContent_GetProperties(lyra)
if(NOT lyra_POPULATED)
    message(STATUS "Downloading Lyra...")
    FetchContent_Populate(lyra)
endif()

if(NOT TARGET Lyra)
  add_library(Lyra INTERFACE)
  set(LYRA_INC_DIR ${lyra_SOURCE_DIR}/data/single_include)
  target_include_directories(Lyra SYSTEM INTERFACE ${LYRA_INC_DIR})
endif()



add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/filewatcher.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Lyra
    PRIVATE filesystem
    PRIVATE POCO::Foundation
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES CXX_EXTENSIONS OFF
    CXX_STANDARD 11
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (NOT ${CMAKE_CROSSCOMPILING})
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wextra>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wconversion>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wsign-conversion>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:--pedantic-errors>
            $<$<CXX_COMPILER_ID:MSVC>:/WX>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endif()

# Add the subprojects to be compiled
#add_subdirectory(tests)
