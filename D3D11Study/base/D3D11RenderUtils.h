/*!
 * \file D3D11RenderUtils.h
 *
 * \author dwy
 * \date 2023/1/2
 *
 * 
 */
 #pragma once
#include <d3dcompiler.h>
class D3D11RenderUtils {
public:
    static HRESULT compileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);


};