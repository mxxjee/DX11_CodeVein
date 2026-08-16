#pragma once

#include "Engine_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;
class Model;
class Component;
class LightComponent;
class Collider;

/* 순수 상속용 클래스 */
class ENGINE_DLL MapObject abstract : public GameObject
{
public:
	struct MAPOBJECT_DESC : public GameObject::GAMEOBJECT_DESC
	{
		ordered_json jExtraData;
		_wstring prototypename = L"";
		_float4 vRotationQuat = { 0.f, 0.f, 0.f, 1.f };
	};

protected:
	explicit MapObject();
	explicit MapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit MapObject(const MapObject& original);
	virtual ~MapObject();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;

public:
	// 중간부모 클래스에 만들어둬서 자식 클래스에서는 선언 안 해도 됨
	// 굳이굳이 final 해뒀음
	Shader* Get_Shader() override final { return m_pShaderCom; }
	Model* Get_Model() override final { return m_pModelCom; }

	void Set_Shader(Shader* _shader) override final { m_pShaderCom = _shader; }
	void Set_Shader(const _wstring& _shaderName) override final;
	void Set_Model(Model* _model) override final { m_pModelCom = _model; }

	virtual ordered_json Get_ExtraData() { return m_jExtraData; }
	virtual LightComponent*& Get_LightPtr() { return m_pLightComponent; }
	virtual Collider*& Get_ColliderPtr() { return m_pColliderCom; }

	void Set_ExtraData(const ordered_json& j)
	{
		m_jExtraData = j;
	}

protected:
	inline HRESULT Add_Shader(const _wstring& _shaderName) {
		return Add_Component(0, _shaderName, Com_Shader, RCAST(Component**)(&m_pShaderCom));
	}
	inline HRESULT Add_Model(const _wstring& _modelName) {
		return Add_Component(m_iLevel, _modelName, Com_Model, RCAST(Component**)(&m_pModelCom));
	}

protected:
	//이거상속해서제대로 쓰려면 자식에 같은 변수명이 없어야함
	Model* m_pModelCom = { nullptr };
	Shader* m_pShaderCom = { nullptr };
 	LightComponent* m_pLightComponent = { nullptr };
	Collider* m_pColliderCom = { nullptr };
	ordered_json m_jExtraData;

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	void Free() override;

};

NS_END
