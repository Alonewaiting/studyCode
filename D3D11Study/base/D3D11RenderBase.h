/*!
 * \file D3D11RenderBase.h
 *
 * \author dwy
 * \date 2023/1/2
 *
 * 
 */
#pragma once
#include <string>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <wrl/client.h>
#include "D3D11RenderData.h"
class D3D11RenderBase {
public:
    D3D11RenderBase();
    bool initD3D11(HINSTANCE hInstance, HWND hWnd);
    void shutDown();								//release
    std::wstring getMainWinName() const;
    void setMainWinName(const std::wstring& name);
    virtual bool loadContext();
    virtual bool unloadContext();
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual ~D3D11RenderBase();

protected:
    D3D11RenderBase(const D3D11RenderBase &) = default;
    D3D11RenderBase& operator=(const D3D11RenderBase&) = default;
protected:
    std::wstring                                        m_mainWndCaption;
    Size2d                                              m_winSize;
    HINSTANCE				                            m_hInstance;
    HWND					                            m_hWnd;
    D3D_DRIVER_TYPE			                            m_driverType;
    D3D_FEATURE_LEVEL		                            m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device           >     m_pd3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext    >     m_pImmediateContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain         >     m_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView >     m_pRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D        >     m_pDepthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView >     m_pDepthStencilView;
};
