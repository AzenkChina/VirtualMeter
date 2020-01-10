# this one is important
set(CMAKE_SYSTEM_NAME "Generic")

# specify the cross compiler
set(CROSS "arm-none-eabi")
set(CMAKE_ASM_COMPILER "${CROSS}-gcc")
set(CMAKE_C_COMPILER "${CROSS}-gcc")
set(CMAKE_CXX_COMPILER "${CROSS}-g++")
set(CMAKE_AR "${CROSS}-ar")
set(CMAKE_OBJCOPY "${CROSS}-objcopy")
set(CMAKE_OBJDUMP "${CROSS}-objdump")
set(SIZE "${CROSS}-size")

# void compiler check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)




