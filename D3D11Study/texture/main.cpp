#include <windows.h>
#include <exception>
#include <iostream>
#include <d3d11.h>
#include <wrl/client.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
constexpr size_t width =1200;
constexpr size_t height = 1800;
struct VertexPos { 
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 tex0;
};

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
    if (!srcFile || !entryPoint || !profile || !blob)
        return E_INVALIDARG;

    *blob = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif

    const D3D_SHADER_MACRO defines[] =
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint, profile,
        flags, 0, &shaderBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return hr;
    }

    *blob = shaderBlob;

    return hr;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    // 1. 定义一个窗口类
    static TCHAR szAppName[] = TEXT("GetStarted");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    // 2. 向系统注册窗口类
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("当前程序要求应用于Windows NT!"),
            szAppName, MB_ICONERROR);
        return 0;
    }

    // 3. 创建窗口
    hwnd = CreateWindow(szAppName,                  // window class name
        TEXT("GUI program start!"), // window caption
        WS_OVERLAPPEDWINDOW,        // window style
        0,              // initial x position
        0,              // initial y position
        width,              // initial x size
        height,              // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        hInstance,                  // program instance handle
        NULL);                     // creation parameters


    //d3d11 
    D3D_DRIVER_TYPE driveType[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_SOFTWARE
    };
    auto totalDriveType = ARRAYSIZE(driveType);
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    auto totalFeatureLevels = ARRAYSIZE(featureLevels);

    //Swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc{0};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    
    //
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice_;
    Microsoft::WRL::ComPtr <ID3D11DeviceContext> d3dContext_;
    Microsoft::WRL::ComPtr <IDXGISwapChain> swapChain_;
    D3D_DRIVER_TYPE driverType_;
    D3D_FEATURE_LEVEL featureLevel_;
    unsigned int creationFlags = 0;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    //create swapchain
    HRESULT result;
    unsigned int driver = 0;
    for (driver = 0; driver < totalDriveType; ++driver)
    {
        result = D3D11CreateDeviceAndSwapChain(0, driveType[driver], 0, creationFlags,
            featureLevels, totalFeatureLevels,
            D3D11_SDK_VERSION, &swapChainDesc, &swapChain_,
            &d3dDevice_, &featureLevel_, &d3dContext_);
        if (SUCCEEDED(result))
        {
            driverType_ = driveType[driver];
            break;
        }
    }
    if (FAILED(result))
    {
        std::cout<<"Failed to create the Direct3D device!"<<std::endl;
        return false;
    }
    //render resource
    //vertex
    VertexPos vertices[] =
    {
       { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    };
    D3D11_BUFFER_DESC vertexDesc {0};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(VertexPos) * ARRAYSIZE(vertices);

    D3D11_SUBRESOURCE_DATA resourceData;
    resourceData.pSysMem = vertices;

    Microsoft::WRL::ComPtr < ID3D11Buffer> vertexBuffer;
    result = d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, vertexBuffer.GetAddressOf());
    if (FAILED(result)) {
        std::cout << "Failed to Create vetex Buffer!" << std::endl;
        return false;
    }
    //shader
    Microsoft::WRL::ComPtr < ID3D11VertexShader> solidColorVS; 
    Microsoft::WRL::ComPtr < ID3D11PixelShader> solidColorPS;
    Microsoft::WRL::ComPtr < ID3D11InputLayout> inputLayout;
    
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif
    // Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
    LPCSTR profile = (d3dDevice_->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
    Microsoft::WRL::ComPtr <ID3DBlob> vsBuffer;
    HRESULT hr = CompileShader(L"./shader/vertexShader.hlsl", "VSMain", "vs_4_0", vsBuffer.GetAddressOf());
   
    if (FAILED(hr))
    {
        printf("Failed compiling vertex shader %08X\n", hr);
        return -1;
    }
    Microsoft::WRL::ComPtr<ID3DBlob> fsBuffer;
    hr = CompileShader(L"./shader/fragmentShader.fs","PS_Main", "ps_4_0", &fsBuffer);
    if (FAILED(hr))
    {
        printf("Failed compiling fragment shader %08X\n", hr);
        return -1;
    }

    result = d3dDevice_->CreateVertexShader(vsBuffer->GetBufferPointer(),vsBuffer->GetBufferSize(), 0, solidColorVS.GetAddressOf());
    result = d3dDevice_->CreatePixelShader(fsBuffer->GetBufferPointer(),fsBuffer->GetBufferSize(),0, solidColorPS.GetAddressOf());
    //input
    D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);
    result = d3dDevice_->CreateInputLayout(solidColorLayout,
        totalLayoutElements, vsBuffer->GetBufferPointer(),
        vsBuffer->GetBufferSize(), inputLayout.GetAddressOf());

    unsigned int stride = sizeof(VertexPos);
    unsigned int offset = 0;

    
    d3dContext_->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    d3dContext_->VSSetShader(solidColorVS.Get(), 0, 0);
    d3dContext_->PSSetShader(solidColorPS.Get(), 0, 0);
    //render resource viewer
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorMap;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> inputTexture;

    bool forceSRGB = true;
    bool enableMips = true;
    int imageWidth,imageHeight,imageChannel;
    stbi_set_flip_vertically_on_load(true);
    std::string imagePath = "../Resource/test.jpg";
    //"D:\\myCode\\studyCode\\Resource\\test.jpg"
    auto image = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageChannel, 4);
    CD3D11_TEXTURE2D_DESC texDesc(forceSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM,
        width, height, 1,
        enableMips ? 0 : 1,
        D3D11_BIND_SHADER_RESOURCE | (enableMips ? D3D11_BIND_RENDER_TARGET : 0),
        D3D11_USAGE_DEFAULT, 0, 1, 0,
        enableMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    d3dDevice_->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf());
    Microsoft::WRL::ComPtr <ID3D11DeviceContext> m_pDeviceContext;
    d3dDevice_->GetImmediateContext(m_pDeviceContext.GetAddressOf());
    // 上传纹理数据
    m_pDeviceContext->UpdateSubresource(tex.Get(), 0, nullptr, image, width * sizeof(uint32_t), 0);
    // 创建SRV
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D,
        forceSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM);
    d3dDevice_->CreateShaderResourceView(tex.Get(), &srvDesc, colorMap.ReleaseAndGetAddressOf());
    // 生成mipmap
    if (enableMips)
        m_pDeviceContext->GenerateMips(colorMap.Get());

    //color map sample 
    Microsoft::WRL::ComPtr < ID3D11SamplerState> colorMapSampler_;
    D3D11_SAMPLER_DESC colorMapDesc;
    ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = d3dDevice_->CreateSamplerState(&colorMapDesc, colorMapSampler_.GetAddressOf());

    //render target
    Microsoft::WRL::ComPtr < ID3D11RenderTargetView> backBufferTarget_;
    Microsoft::WRL::ComPtr < ID3D11Texture2D > backBufferTexture;
    result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBufferTexture.GetAddressOf());
    if (FAILED(result)) {
        std::cout<< "Failed to get the swap chain back buffer!"<<std::endl;
        return false;
    }
    result = d3dDevice_->CreateRenderTargetView(backBufferTexture.Get(), 0, backBufferTarget_.GetAddressOf());
    if (FAILED(result))
    {
        std::cout << "Failed to create the render target view!" <<std::endl;
        return false;
    }
    d3dContext_->OMSetRenderTargets(1, backBufferTarget_.GetAddressOf(), 0);
    //view port
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    d3dContext_->RSSetViewports(1, &viewport);
    // 4. 显示窗口并更新
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 5. 开始进行消息循环， 从消息队列中取出消息进行翻译并分发
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      
        float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
        d3dContext_->IASetInputLayout(inputLayout.Get());
        d3dContext_->ClearRenderTargetView(backBufferTarget_.Get(), clearColor);
        d3dContext_->PSSetShaderResources(0, 1, colorMap.GetAddressOf());
        d3dContext_->PSSetSamplers(0, 1, colorMapSampler_.GetAddressOf());
        d3dContext_->Draw(6, 0);
        swapChain_->Present(0, 0);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
    case WM_CREATE:

        return 0;

    case WM_PAINT:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
