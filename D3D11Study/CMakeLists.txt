set(D3D11RenderPath ${CMAKE_CURRENT_SOURCE_DIR}/D3D11Study)
set(D3D11BASE_SRC
	${D3D11RenderPath}/base/D3D11RenderBase.cpp
	${D3D11RenderPath}/base/D3D11RenderUtils.cpp)
#texure
add_subdirectory(${D3D11RenderPath}/texture)
#triangle
add_subdirectory(${D3D11RenderPath}/triangle)
