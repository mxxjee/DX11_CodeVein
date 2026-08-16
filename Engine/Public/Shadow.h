#pragma once

#include "Base.h"

NS_BEGIN(Engine)

//그림자인데 원근 카메라처럼 세팅함
class Shadow final : public Base
{
private:
    explicit Shadow();
    explicit Shadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Shadow();

public:
    HRESULT Add_ShadowLight(const SHADOW_DESC& _shadowDesc);
    inline _float4x4 Get_LightMatrix(D3DTRANSFORM _transform) { return m_LightMarices[_transform]; }
    void ResizeBuffers(_float _width, _float _height);
    //inline _float4x4 Get_LightInverseMatrix(D3DTRANSFORM _transfrom) { return m_matInverseMatrices[_transfrom]; }

    //cascade용 일단구현
    HRESULT Add_ShadowCascadeLight(_int _iNumCascade, const SHADOW_DESC& _shadowDesc, _float _Bias[]);
    inline _float4x4* Get_ShadowCascade_Matrix(D3DTRANSFORM _transform) { return m_LightCascadeMarices[_transform]; }
    _float4& Get_ShadowCascade_Split() { return Split; }
    _float* Get_ShadowCascade_Bias() { return m_bias; }
    _int Get_ShadowCascade_Num() { return m_iNumCascade; }
    _float4& Get_ShadowLightDir() { return m_vLightDir; }

    void Follow_ShadowCascade_Matrix(_vector _at);

private:
    _float4x4 m_LightMarices[_UINT(D3DTS_END)] = {};
    SHADOW_DESC m_tShadowDesc = {};

    //cascade용
    _int m_iNumCascade = {};
    _float4x4 m_LightCascadeMarices[_UINT(D3DTS_END)][4] = {};
    SHADOW_DESC m_tShadowCascadeDesces[4] = {};
    _float m_OutSplits[5] = {};
    _float4 Split;
    _float m_bias[4] = {};
    _float4 m_vAddPos = {};
    //정적그림자용
    _float4 m_vLightDir = {};

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static Shadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;

};

NS_END
