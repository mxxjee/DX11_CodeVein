#pragma once
#include "Engine_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;
class Texture;
class VIBuffer_Cube;

class ENGINE_DLL Decal abstract : public GameObject
{
public:
	typedef struct tagDecalDesc : GameObject::GAMEOBJECT_DESC
	{
		_float3 vPosition = {};
		_float3 vNormal = { 0.f, 1.f, 0.f }; 
		_float3 vScale = { 1.f, 1.f, 1.f };
		_float fMaxLifeTime = { 5.f };
		_uint iTexIndex = {};
	}DECAL_DESC;

protected:
	explicit Decal();
	explicit Decal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Decal(const Decal& original);
	virtual ~Decal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

public:
	void Reset(const DECAL_DESC& desc);		// 데칼 풀링용

protected:
	virtual HRESULT Ready_Components() { return S_OK; }

protected:
	Shader*			m_pShaderCom = { nullptr };
	VIBuffer_Cube*	m_pVIBufferCom = { nullptr };
	Texture*		m_pTextureCom = { nullptr };
	Texture*		m_pMaskTextureCom = { nullptr };

	_float4			m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_float			m_fLifeTime = {};
	_float			m_fMaxLifeTime = {};
	_float			m_fAlpha = { 1.f };
	_uint			m_iTexIndex = {};
	_uint			m_iMaskIndex = {};

public:
	static Decal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END