cmake_minimum_required(VERSION 3.28.3)


project(atmega128a
  LANGUAGES ASM C CXX
)

set(EXECUTABLE ${PROJECT_NAME}.elf)

#해당 폴더의 파일만 찾는다.
file(GLOB SRC_FILES CONFIGURE_DEPENDS
  *.c
  *.cpp
)

#해당 폴더를 포함해서 하위 폴더까지의 파일도 찾는다.
file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  src/*.c
  src/*.cpp
)

#Build the executable based on the source files
add_executable(${EXECUTABLE}
  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
  )

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DF_CPU=16000000L
  )

# List of include directories
target_include_directories(${EXECUTABLE} PRIVATE
  ${CMAKE_SOURCE_DIR}/src                      # 메인 소스 디렉터리
  ${CMAKE_SOURCE_DIR}/src/ap                   # src/ap 디렉터리
  ${CMAKE_SOURCE_DIR}/src/bsp                  # src/bsp 디렉터리
  ${CMAKE_SOURCE_DIR}/src/hw                   # src/hw 디렉터리
  ${CMAKE_SOURCE_DIR}/src/common               # src/common 디렉터리 (def.h 포함)
  ${CMAKE_SOURCE_DIR}/src/common/hw/include    # src/common/hw/include 디렉터리
  ${CMAKE_SOURCE_DIR}/src/driver               # src/driver 디렉터리
)

#Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
  -mmcu=atmega128a

  -fdata-sections
  -ffunction-sections
  -MMD
  -flto
  -fno-fat-lto-objects

  -Wall
  -Os
  -g3
  )

#Linker options
target_link_options(${EXECUTABLE} PRIVATE
  -mmcu=atmega128a

  -flto 
  -fuse-linker-plugin

  -lm
  -Wl,-Map=${PROJECT_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
  )


add_custom_command(TARGET ${EXECUTABLE} 
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex -R .eeprom ${EXECUTABLE} ${PROJECT_NAME}.hex
  COMMENT "Invoking: Make Hex"
  )

add_custom_command(TARGET ${EXECUTABLE} 
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ${EXECUTABLE} ${PROJECT_NAME}.eep
  COMMENT "Invoking: Make EEPROM"
  )