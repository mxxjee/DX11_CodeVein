#pragma once

#include "ShaderTool_Define.h"
#include "ContainerObject.h"

NS_BEGIN(ShaderTool)

class Monster final : public ContainerObject
{
public:
	enum {
		IDLE = 0x00000001,
		RUN = 0x00000002,
		ATTACK = 0x00000004,
	};
private:
	Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Monster(const Monster& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~Monster() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);  /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize(void* pArg) override;   /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual _int Update_Priority(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual _int Update_Late(_float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
private:
	_uint				m_iState = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


NS_END