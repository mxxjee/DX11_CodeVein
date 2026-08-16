#include "Mt_Defines.h"
#include "UI_Test.h"
#include "UI_Image.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
UI_Test::UI_Test()
{
}

UI_Test::UI_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: UIObject(pDevice, pContext)
{
}

UI_Test::UI_Test(const UI_Test& original)
	: UIObject(original)
{
}

UI_Test::~UI_Test()
{
}

HRESULT UI_Test::Initialize_Prototype(LEVEL _level)
{
	UIObject::Initialize_Prototype(_UINT(_level));

	return S_OK;
}

HRESULT UI_Test::Initialize(void* arg)
{
	static _uint namenum = 0;
	m_iNumber = namenum;
	
	wstring TexKey = L"Prototype_Component_UITexture_BackSky"; 

	if (FAILED(UIObject::Initialize(arg)))
		return E_FAIL;

	UI_Image::UIIMAGEDESC Desc;
	Desc.pOwner = this;
	Desc.TextureComponentKey = TexKey;

	if (FAILED(Add_Component(0, L"Prototype_Component_UI_Image", L"Com_Renderer", (Component**)&m_pUIRenderer, &Desc)))
		return E_FAIL;
	
	return S_OK;
}

_int UI_Test::Update_Priority(const _float fTimeDelta)
{

	return 0;
}

_int UI_Test::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::PRIORITY);

	return 0;
}

_int UI_Test::Update_Late(const _float fTimeDelta)
{
	UIObject::Update_Late(fTimeDelta);

	return 0;
}

HRESULT UI_Test::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}

UI_Test* UI_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	UI_Test* pInstance = new UI_Test(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"UI_Test 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* UI_Test::Clone(void* pArg)
{
	UI_Test* pInstance = new UI_Test(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UI_Test 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void UI_Test::Free()
{
	__super::Free();
}

