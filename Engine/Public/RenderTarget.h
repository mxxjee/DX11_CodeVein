#pragma once

#include "Base.h"
#include "RenderTargetManager.h"

NS_BEGIN(Engine)

class RenderTarget final : public Base
{
private:
    explicit RenderTarget();
    explicit RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~RenderTarget();

public:
    HRESULT Initialize(_float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    HRESULT Initialize_Shadow(_float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
       
    HRESULT Bind_ShaderResource(class Shader* _shader, const _string& _constName);
    HRESULT Bind_ShaderResource_FullSlot(class Shader* _shader, _uint _slotNum, _uint _stageMask);

    void ClearRTV();
    HRESULT ResizeBuffer(_float _width, _float _height);

    void Copy_Resource(ID3D11Resource* pDest);

#pragma region Getter
    ID3D11RenderTargetView* Get_RTV() { return m_pRTV; }
    ID3D11DepthStencilView* Get_DSV() { return m_pDSV; }
    ID3D11ShaderResourceView* Get_SRV() { return m_pSRV; }
    DXGI_FORMAT Get_Format() const { return m_Format; }
    _float4 Get_ClearColor() const { return m_vClearColor; }
#pragma endregion


private:
    ID3D11Texture2D* m_pTexture2D = { nullptr };
    ID3D11RenderTargetView* m_pRTV = { nullptr };
    ID3D11DepthStencilView* m_pDSV = { nullptr };
    ID3D11ShaderResourceView* m_pSRV = { nullptr };

    _float4 m_vClearColor = {};

    /* 디버그용 함수 및 변수들 */
#ifdef _DEBUG
public:
    HRESULT Ready_Debug(_float _fX, _float _fY, _float _sizeX, _float _sizeY);
    HRESULT Render(class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);

private:
    _float4x4 m_WorldMatrix = {};
#endif

private:
    DXGI_FORMAT m_Format = {};

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static RenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    static RenderTarget* Create_Shadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);

public:
    void Free() override final;

};

NS_END
