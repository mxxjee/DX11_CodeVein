#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class Player_Injection final : public PartObject
{
public:
	typedef struct tagPlayerInjectionDesc : public PartObject::PARTOBJECT_DESC
	{
		const _float4x4*	pSocketMatrix = { nullptr }; //부착할 뼈의 행렬
		_int				iSocketIndex = { -1 };

	}INJECTION_DESC;

private:
	explicit Player_Injection();
	explicit Player_Injection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Injection(const Player_Injection& original);
	virtual ~Player_Injection();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_ShaderValue();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	const _float4x4*	m_pSocketMatrix = { nullptr }; //구조체 받아서 저장할 변수
	_int				m_iSocketIndex = {};

public:
	static Player_Injection* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
NS_END