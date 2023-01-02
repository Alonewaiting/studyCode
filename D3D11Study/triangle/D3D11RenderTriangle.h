/*!
 * \file D3D11RenderTriangle.h
 *
 * \author dwy
 * \date 2023/1/2
 *
 * 
 */
 #pragma once
#include "D3D11RenderBase.h"
class D3D11RenderTriangle :public D3D11RenderBase {
public:
    D3D11RenderTriangle();
    virtual bool loadContext() override;
    virtual bool unloadContext() override;
    virtual void update(float dt) override;
    virtual void render() override;
    virtual ~D3D11RenderTriangle();
private:
    Microsoft::WRL::ComPtr < ID3D11Buffer> m_pVertexBuffer;
    Microsoft::WRL::ComPtr < ID3D11Buffer> m_pIndexBuffer;
    Microsoft::WRL::ComPtr < ID3D11InputLayout> m_pInputLayout;
    Microsoft::WRL::ComPtr < ID3D11ShaderResourceView> m_pTexSRV;
    Microsoft::WRL::ComPtr < ID3D11SamplerState> m_pTexSampleState;
};