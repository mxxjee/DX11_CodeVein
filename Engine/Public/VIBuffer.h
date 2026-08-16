#pragma once

#include "Component.h"

/* 정점과 인덱스 버퍼를 만드는 모듵 클래스들의 부모클래스다. */

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer abstract : public Component
{
protected:
	explicit VIBuffer();
	explicit VIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer(const VIBuffer& original);
	virtual ~VIBuffer();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Bind_Resource();

    _float3* Get_VertexPositions() { return m_pVertexPositions; }
    //_float3* Get_VertexAnimPositions() { return m_pSkinnedVertexPositions; }
    _uint* Get_Indices() { return m_pIndices; }
    _uint  Get_NumIndices() { return m_iNumIndices; }

    _bool IsAnimation() { return m_bIsAnimated; }

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
    _float3*					m_pVertexPositions = { nullptr };
    //_float3*					m_pSkinnedVertexPositions = { nullptr };	// 메쉬 피킹용 일단 남겨는 둠 혹시 모르잖아
    _uint*                      m_pIndices = { nullptr };
	_uint						m_iNumVertices = {};
	_uint						m_iVertexStride = {};
	_uint						m_iNumIndices = {};
	_uint						m_iIndexStride = {};
	_uint						m_iNumVertexBuffers = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveType = {};

    _bool m_bIsAnimated = {false};

public:
	static VIBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg) override;

public:
	void Free() override;

};

NS_END
