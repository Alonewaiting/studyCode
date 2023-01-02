#include "D3D11RenderTexture.h"
#include "D3D11RenderUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
D3D11RenderTexture::D3D11RenderTexture()
{

}

bool D3D11RenderTexture::loadContext()
{
    //create vertex and vertex shader
    VertexPos vertices[] =
    {
        { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    };
    D3D11_BUFFER_DESC vertexDesc{ 0 };
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(VertexPos) * ARRAYSIZE(vertices);

    D3D11_SUBRESOURCE_DATA resourceData;
    resourceData.pSysMem = vertices;
    auto result = m_pd3dDevice->CreateBuffer(&vertexDesc,&resourceData,m_pVertexBuffer.GetAddressOf());
    if (FAILED(result)) {
        OutputDebugStringA("Failed to Create vetex Buffer!");
        return false;
    }
    Microsoft::WRL::ComPtr < ID3D11VertexShader> textureVS;
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif
    LPCSTR profile = (m_pd3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
    Microsoft::WRL::ComPtr <ID3DBlob> vsBuffer;
    result = D3D11RenderUtils::compileShader(L"../D3D11Study/shader/texture2dSample.hlsl", "VS_Main", "vs_4_0", vsBuffer.GetAddressOf());
    if (FAILED(result))
    {
        OutputDebugStringA("Failed compiling vertex shader ");
        return false;
    }
    result = m_pd3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, textureVS.GetAddressOf());

    //create fragment shader
    Microsoft::WRL::ComPtr<ID3DBlob> fsBuffer;
    Microsoft::WRL::ComPtr <ID3D11PixelShader> texturePS;
    result = D3D11RenderUtils::compileShader(L"../D3D11Study/shader/texture2dSample.hlsl", "PS_Main", "ps_4_0", &fsBuffer);
    if (FAILED(result))
    {
        OutputDebugStringA("Failed compiling fragment shader ");
        return false;
    }
    result = m_pd3dDevice->CreatePixelShader(fsBuffer->GetBufferPointer(), fsBuffer->GetBufferSize(), 0, texturePS.GetAddressOf());
    //input
    D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);
    result = m_pd3dDevice->CreateInputLayout(solidColorLayout,
        totalLayoutElements, vsBuffer->GetBufferPointer(),
        vsBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf());

    unsigned int stride = sizeof(VertexPos);
    unsigned int offset = 0;

    
    m_pImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pImmediateContext->VSSetShader(textureVS.Get(), 0, 0);
    m_pImmediateContext->PSSetShader(texturePS.Get(), 0, 0);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> inputTexture;

    bool forceSRGB = true;
    bool enableMips = true;
    int imageWidth, imageHeight, imageChannel;
    stbi_set_flip_vertically_on_load(true);
    std::string imagePath = "../Resource/test.jpg";
    //"D:\\myCode\\studyCode\\Resource\\test.jpg"
    auto image = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageChannel, 4);
    CD3D11_TEXTURE2D_DESC texDesc(forceSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM,
        imageWidth, imageHeight, 1,
        enableMips ? 0 : 1,
        D3D11_BIND_SHADER_RESOURCE | (enableMips ? D3D11_BIND_RENDER_TARGET : 0),
        D3D11_USAGE_DEFAULT, 0, 1, 0,
        enableMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    m_pd3dDevice->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf());
  
    // 上传纹理数据
    m_pImmediateContext->UpdateSubresource(tex.Get(), 0, nullptr, image, imageWidth * sizeof(uint32_t), 0);
    // 创建SRV
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D,
        forceSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM);
    m_pd3dDevice->CreateShaderResourceView(tex.Get(), &srvDesc, m_pTexSRV.ReleaseAndGetAddressOf());
    // 生成mipmap
    if (enableMips)
        m_pImmediateContext->GenerateMips(m_pTexSRV.Get());
    //color map sample 
    D3D11_SAMPLER_DESC colorMapDesc;
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = m_pd3dDevice->CreateSamplerState(&colorMapDesc, m_pTexSampleState.GetAddressOf());
    if (FAILED(result)) {
        OutputDebugStringA("Failed CreateSamplerState ");
        return false;
    }
    return true;
}

bool D3D11RenderTexture::unloadContext()
{
    return true;
}

void D3D11RenderTexture::update(float dt)
{

}

void D3D11RenderTexture::render()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
    m_pImmediateContext->IASetInputLayout(m_pInputLayout.Get());
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);
    m_pImmediateContext->PSSetShaderResources(0, 1, m_pTexSRV.GetAddressOf());
    m_pImmediateContext->PSSetSamplers(0, 1, m_pTexSampleState.GetAddressOf());
    m_pImmediateContext->Draw(6, 0);
    m_pSwapChain->Present(0, 0);
}

D3D11RenderTexture::~D3D11RenderTexture()
{

}

