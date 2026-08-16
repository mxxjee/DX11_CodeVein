#pragma once

#include "ShaderTool_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class Collider;
class LightComponent;
NS_END

NS_BEGIN(ShaderTool)

class Monster_Body final : public ContainerObject
{
public:
	typedef struct tagBodyDesc : public GameObject::GAMEOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };

	}BODY_DESC;

protected:
	Monster_Body();
	Monster_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Monster_Body(const Monster_Body& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~Monster_Body() = default;

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual _int Update_Priority(_float fTimeDelta);
	virtual _int Update_Parallel(const _float fTimeDelta);
	virtual _int Update(_float fTimeDelta);
	virtual _int Update_Late(_float fTimeDelta);
	virtual HRESULT Render(const _float fTimeDelta);
	//virtual Shader* Get_Shader() { return m_pShaderCom; }
private:
	//Model* m_pModelCom = { nullptr };
	//Shader* m_pShaderCom = { nullptr };

private:
	const _uint* m_pParentState = { nullptr };
	_float2 m_vEmissionUV;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Monster_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END