#include "Engine_Define.h"
#include "UIComponent.h"
#include "GameObject.h"
#include "UIObject.h"
#include "Shader.h"

bool UIComponent::m_bDrawDebug = false;
//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::UIComponent::UIComponent()
	:Component()
{
}

Engine::UIComponent::UIComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Component(pDevice,pContext)
{
}

Engine::UIComponent::UIComponent(const UIComponent& original)
	:Component(original),m_bAbleToBind(original.m_bAbleToBind)
{
}

Engine::UIComponent::~UIComponent()
{
	
}
HRESULT Engine::UIComponent::Initialize(void* arg)
{
	/*컴포넌트의 owner 설정해주기.*/
	if (arg != nullptr)
	{
		UICOMPDESC* pDesc = CAST(UICOMPDESC*)(arg);
		m_pOwner = dynamic_cast<UIObject*>(pDesc->pOwner);


		if (m_pOwner)
		{
			m_pShaderCom = m_pOwner->Get_Shader();
			if (m_pShaderCom)
				Safe_AddRef(m_pShaderCom);

		}
	}


	return S_OK;
}


#ifdef _DEBUG
vector<string> Engine::UIComponent::Get_ActionNames()
{
	vector<string> m_VecString;

	for (auto& pair : m_Actions)
	{
		m_VecString.push_back(pair.first);
	}

	return m_VecString;
}
#endif // _DEBUG





void Engine::UIComponent::Free()
{
	Safe_Release(m_pShaderCom);

	__super::Free();
}
/******************************************************* 생성자, 소멸자 *******************************************************/

#pragma region parsing
void Engine::UIComponent::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	CHECK_JUST_NULL(pOwner);
	Component::Load_Data(pOwner, Data);

	m_pOwner = dynamic_cast<UIObject*>(pOwner);

	m_pShaderCom = m_pOwner->Get_Shader();

	if (m_pShaderCom)
		Safe_AddRef(m_pShaderCom);

	*eType = UITYPE::ROOT;
}
#pragma endregion