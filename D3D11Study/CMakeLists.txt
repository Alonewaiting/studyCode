#texure
set(TEXTURE_SRC_PATH ${PROJECT_SOURCE_DIR}/D3D11Study/texture)
set(TEXTURE_SRC)
include_directories( ${PROJECT_SOURCE_DIR}/utils)
set(TEXTURE_SRC 
    ${TEXTURE_SRC_PATH}/main.cpp)

set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" )
my_add_executable(texture ${TEXTURE_SRC})
target_link_libraries(texture d3d11.lib d3dcompiler.lib)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set_target_properties(texture PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()
# 拷贝文件夹到指定的目录
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/D3D11Study/shader ${PROJECT_SOURCE_DIR}/bin/shader)
