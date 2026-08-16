#pragma once

#include "UITool_Define.h"
#include "Character.h"


NS_BEGIN(UITool)

class TestModel :
    public Character
{
public:
	struct TestModelDesc : GameObject::GAMEOBJECT_DESC
	{
		wstring modelName;
	};

private:
	explicit TestModel();
	explicit TestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit TestModel(const TestModel& original);
	virtual ~TestModel();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;

private:
	HRESULT Ready_Components(wstring ModelName);
	HRESULT Bind_ShaderResources();

public:
	static TestModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;


};
NS_END

