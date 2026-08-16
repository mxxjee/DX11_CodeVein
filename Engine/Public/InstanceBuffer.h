#pragma once

#include "Base.h"

NS_BEGIN(Engine)

// 같은 모델 / 메시를 사용하는 객체들을 모아서 한 번에 Draw하는 클래스
// 같은 모델들을 모아놓는 역할
class ENGINE_DLL InstanceBuffer final : public Base
{
private:
    explicit InstanceBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~InstanceBuffer();

public:
    HRESULT Initialize(_uint _maxInstanceCount);

    // 매 프레임 인스턴스 데이터 갱신
    HRESULT Update_InstanceData(const vector<_float4x4>& _worldMatrices);
    HRESULT Update_RawData(const _float4x4* _matrices, _uint _count);

    // 버퍼 바인딩 (VB 슬롯 1번에)
    void Bind_Buffer(_uint _vertexStride);

    // 인스턴싱 Draw
    void Render_Instanced(_uint _indexCount);

    _uint Get_InstanceCount() const { return m_iCurrentInstanceCount; }

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    ID3D11Buffer* m_pInstanceBuffer = { nullptr };

    _uint m_iMaxInstanceCount = {};
    _uint m_iCurrentInstanceCount = {};

public:
    static InstanceBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _maxInstanceCount);
    
public:
    virtual void Free() override final;
};

NS_END