# [NEVER REMOVE THIS LINE] WARNING: this file is generated, please edit kendryte-package.json file instead.
##### include(reset) #####
### reset basic var
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# set(CMAKE_SYSTEM "Generic")
set(CMAKE_SYSTEM_NAME "Generic")

# set(CMAKE_HOST_SYSTEM "Generic")
# set(CMAKE_HOST_SYSTEM_NAME "Generic")
#
set(CMAKE_CROSSCOMPILING TRUE)
# set(CMAKE_C_COMPILER_FORCED TRUE)
# set(CMAKE_CXX_COMPILER_FORCED TRUE)
# set(CMAKE_ASM_COMPILER_FORCED TRUE)
#

set(SOURCE_FILES "")
set(LDFLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "")
set(CMAKE_MODULE_LINKER_FLAGS "")


cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME "kendryte-standalone-sdk")

##### flags from config json #####
add_compile_flags(C
  "-std=gnu11"
  "-Wno-pointer-to-int-cast"
  "-Wno-old-style-declaration"
)
add_compile_flags(CXX
  "-std=gnu++17"
)
add_compile_flags(BOTH
  "-mcmodel=medany"
  "-fno-common"
  "-ffunction-sections"
  "-fdata-sections"
  "-fstrict-volatile-bitfields"
  "-fno-zero-initialized-in-bss"
  "-Os"
  "-ggdb"
  "-Wall"
  "-Werror=all"
  "-Wno-error=unused-function"
  "-Wno-error=unused-but-set-variable"
  "-Wno-error=unused-variable"
  "-Wno-error=deprecated-declarations"
  "-Wextra"
  "-Werror=frame-larger-than=65536"
  "-Wno-unused-parameter"
  "-Wno-sign-compare"
  "-Wno-error=missing-braces"
  "-Wno-error=return-type"
  "-Wno-error=pointer-sign"
  "-Wno-missing-braces"
  "-Wno-strict-aliasing"
  "-Wno-implicit-fallthrough"
  "-Wno-missing-field-initializers"
)
add_compile_flags(LD
  "-static"
  "-Wl,-static"
  "-Wl,-EL"
)
##### internal flags #####
##### include(coreFlags) #####
add_compile_flags(LD -nostartfiles -Wl,--gc-sections)
add_compile_flags(BOTH -mcmodel=medany)


add_compile_flags(LD
  -T "${CMAKE_CURRENT_LIST_DIR}/lds/kendryte.ld"
)
##### include(fix9985) #####
### http://www.cmake.org/Bug/view.php?id=9985
string(REPLACE "-rdynamic" "" _C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
string(REPLACE "-rdynamic" "" _CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${_C_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${_CXX_FLAGS}")


##### Main Section #####
message("======== PROJECT ========")
project(${PROJECT_NAME})
## add include from self and dependency
include_directories(
  "${CMAKE_CURRENT_LIST_DIR}/lib/drivers/include"
  "${CMAKE_CURRENT_LIST_DIR}/lib/bsp/include"
  "${CMAKE_CURRENT_LIST_DIR}/lib/utils/include"
)
## there is no headers
## add source from config json
add_source_files(
  "${CMAKE_CURRENT_LIST_DIR}/config/fpioa-config.c"
  "${CMAKE_CURRENT_LIST_DIR}/config/fpioa-config.h"
)
add_source_files(
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.c"
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.h"
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.s"
  "${CMAKE_CURRENT_LIST_DIR}/lib/*.S"
)
##### include(asm.cmake) #####
FILE(GLOB_RECURSE ASSEMBLY_FILES
	"${CMAKE_CURRENT_LIST_DIR}/*.s"
	"${CMAKE_CURRENT_LIST_DIR}/*.S"
)

SET_PROPERTY(SOURCE ${ASSEMBLY_FILES} PROPERTY LANGUAGE C)
SET_SOURCE_FILES_PROPERTIES(${ASSEMBLY_FILES} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D __riscv64")
SET_TARGET_PROPERTIES(${PROEJCT_NAME} PROPERTIES LINKER_LANGUAGE C)

##### include(asm.cmake) END #####
## final create executable or library
add_library(${PROJECT_NAME} ${SOURCE_FILES})
## set properties
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE "C")
## no definitions
target_link_libraries(${PROJECT_NAME} -Wl,--start-group gcc m c -Wl,--end-group)
##### include(afterProject) #####
removeDuplicateSubstring("${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)
removeDuplicateSubstring("${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)


##### include(fix9985) #####
### http://www.cmake.org/Bug/view.php?id=9985
string(REPLACE "-rdynamic" "" _C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
string(REPLACE "-rdynamic" "" _CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${_C_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${_CXX_FLAGS}")


##### include(dumpConfig) #####
set(CMAKE_SYSTEM_NAME "Generic")

message("")
message("Project: ${PROJECT_NAME}")
message("  LIST_FILE=${CMAKE_PARENT_LIST_FILE}")
message("  TOOLCHAIN=${TOOLCHAIN}")
message("  KENDRYTE_IDE=${KENDRYTE_IDE}")
message("  BUILDING_SDK=${BUILDING_SDK}")
message("  _CMAKE_TOOLCHAIN_PREFIX=${_CMAKE_TOOLCHAIN_PREFIX}")
message("  _CMAKE_TOOLCHAIN_LOCATION=${_CMAKE_TOOLCHAIN_LOCATION}")


message("")
message("  CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("  CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
message("  CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
message("  CMAKE_LINKER=${CMAKE_LINKER}")
message("  CMAKE_OBJCOPY=${CMAKE_OBJCOPY}")
message("  CMAKE_OBJDUMP=${CMAKE_OBJDUMP}")
message("  CMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}")

message("")
message("  CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
message("  CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
message("  LDFLAGS=${LDFLAGS}")
message("  CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

message("Makefile created.")
message("")
message("")
