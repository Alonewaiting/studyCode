#triangle 

set(TRIANGLE_SRC_PATH ${PROJECT_SOURCE_DIR}/D3D11Study/triangle)

set(TRIANGLE_SRC)
include_directories( ${PROJECT_SOURCE_DIR}/utils)
include_directories( ${PROJECT_SOURCE_DIR}/D3D11Study/base)
set(TRIANGLE_SRC 
    ${TRIANGLE_SRC_PATH}/main.cpp
   ${TRIANGLE_SRC_PATH}/D3D11RenderTriangle.cpp
    ${D3D11BASE_SRC}   
    )

set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" )
my_add_executable(triangle ${TRIANGLE_SRC})
target_link_libraries(triangle d3d11.lib d3dcompiler.lib)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set_target_properties(triangle PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()
# 拷贝文件夹到指定的目录
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/D3D11Study/shader ${PROJECT_SOURCE_DIR}/bin/shader)
