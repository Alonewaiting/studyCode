#include "D3D11RenderTriangle.h"
#include "D3D11RenderUtils.h"
D3D11RenderTriangle::D3D11RenderTriangle()
{

}

bool D3D11RenderTriangle::loadContext()
{
    struct TrianglePosColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };
    TrianglePosColor triangle[] = {
        {DirectX::XMFLOAT3(0.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { DirectX::XMFLOAT3(0.5f, -0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
    };
    D3D11_BUFFER_DESC vertexDesc{ 0 };
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(triangle);
    vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA resourceData;
    resourceData.pSysMem = triangle;
    auto result = m_pd3dDevice->CreateBuffer(&vertexDesc, &resourceData, m_pVertexBuffer.GetAddressOf());
    if (FAILED(result)) {
        OutputDebugStringA("Failed to Create vetex Buffer!");
        return false;
    }
    Microsoft::WRL::ComPtr < ID3D11VertexShader> triangleVS;
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
    result = D3D11RenderUtils::compileShader(L"../D3D11Study/shader/triangle.hlsl", "VS_Main", "vs_4_0", vsBuffer.GetAddressOf());
    if (FAILED(result))
    {
        OutputDebugStringA("Failed compiling vertex shader ");
        return false;
    }
    result = m_pd3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, triangleVS.GetAddressOf());

    //create fragment shader
    Microsoft::WRL::ComPtr<ID3DBlob> fsBuffer;
    Microsoft::WRL::ComPtr <ID3D11PixelShader> trianglePS;
    result = D3D11RenderUtils::compileShader(L"../D3D11Study/shader/triangle.hlsl", "PS_Main", "ps_4_0", &fsBuffer);
    if (FAILED(result))
    {
        OutputDebugStringA("Failed compiling fragment shader ");
        return false;
    }
    result = m_pd3dDevice->CreatePixelShader(fsBuffer->GetBufferPointer(), fsBuffer->GetBufferSize(), 0, trianglePS.GetAddressOf());
    //input
    D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);
    result = m_pd3dDevice->CreateInputLayout(solidColorLayout,
        totalLayoutElements, vsBuffer->GetBufferPointer(),
        vsBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf());

    unsigned int stride = sizeof(TrianglePosColor);
    unsigned int offset = 0;


    m_pImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pImmediateContext->VSSetShader(triangleVS.Get(), 0, 0);
    m_pImmediateContext->PSSetShader(trianglePS.Get(), 0, 0);

    return true;
}

bool D3D11RenderTriangle::unloadContext()
{
    return true;
}

void D3D11RenderTriangle::update(float dt)
{

}

void D3D11RenderTriangle::render()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
    m_pImmediateContext->IASetInputLayout(m_pInputLayout.Get());
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);
    //m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pImmediateContext->Draw(3, 0);
    m_pSwapChain->Present(0, 0);
}

D3D11RenderTriangle::~D3D11RenderTriangle()
{

}

