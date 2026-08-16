#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class RenderStateManager final : public Base
{
private:
    explicit RenderStateManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~RenderStateManager();

public:
    HRESULT Initialize();

    // 래스터라이저 스테이트
    void Set_RasterizerState(RSSET _rasterrizerState);
    void Set_RasterizerState(const _string& _name);

    // 뎁스 스텐실 스테이트
    void Set_DepthStencilState(DSSSET _depthStencilState, _uint _stencilRef = 0);
    void Set_DepthStencilState(const _string& _name, _uint _stencilRef = 0);

    // 블렌드 스테이트
    void Set_BlendState(BSSET _blendState);
    void Set_BlendState(const _string& _name);

    //스테이트 가져오기
    ID3D11RasterizerState* Get_RS(const _string& _name);
    ID3D11RasterizerState* Get_RS(RSSET _rasterrizerState);
    ID3D11DepthStencilState* Get_DSS(const _string& _name);
    ID3D11DepthStencilState* Get_DSS(DSSSET _depthStencilState);
    ID3D11BlendState* Get_BS(const _string& _name);
    ID3D11BlendState* Get_BS(BSSET _blendState);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    unordered_map<_string, ID3D11RasterizerState*>   m_umapRS;
    vector<ID3D11RasterizerState*> m_vecRS;
    unordered_map<_string, ID3D11DepthStencilState*> m_umapDSS;
    vector<ID3D11DepthStencilState*> m_vecDSS;
    unordered_map<_string, ID3D11BlendState*>        m_umapBS;
    vector<ID3D11BlendState*> m_vecBS;

public:
    static RenderStateManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Free() override final;
};

NS_END
