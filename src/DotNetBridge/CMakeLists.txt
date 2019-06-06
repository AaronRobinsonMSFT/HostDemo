cmake_minimum_required (VERSION 3.0)
project (dotnetbridge)

# Add project's inc/ directory
include_directories(./inc)

# Add the generated inc/ directory
include_directories(${GENERATED_INC})

# Add JDK to path
include_directories(${JDK_INC})
if (WIN32)
    add_compile_definitions(WINDOWS)
    include_directories(${JDK_INC}/win32)
endif()

# Add paths to the nethost directory
include_directories(${NETHOST_DIR})
link_directories(${NETHOST_DIR})

add_library(dotnetbridge SHARED dotnetbridge.c)

# Link against the nethost library
target_link_libraries(dotnetbridge nethost)

install(TARGETS dotnetbridge DESTINATION $<CONFIG>/${CMAKE_GENERATOR_PLATFORM})
install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION $<CONFIG>/${CMAKE_GENERATOR_PLATFORM} OPTIONAL)