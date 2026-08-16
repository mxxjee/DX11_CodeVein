#include "UITool_Define.h"
#include "CopyManager.h"
#include "UIObject.h"
#include "GameInstance.h"
#include "Sample_Pars_UItool.h"


int CopyManager::CopyNumber = 0;

IMPLEMENT_SINGLETON(CopyManager)

CopyManager::CopyManager()
{
}

CopyManager::CopyManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

CopyManager::~CopyManager()
{
}
HRESULT CopyManager::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	return S_OK;
}

_int	CopyManager::Update_Late(const _float fTimeDelta)
{
	CHECK_FALSE_RESULT(bUseCopyObject,0);
	if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_C))
	{
		UIObject* pSelectObj = m_pGameInstance->Get_UISelectObject();
		if (pSelectObj)
			Set_CopyObject(pSelectObj);

	}

	if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_V))
	{
		Paste_CopyObject();

	}

	return 0;
}
void CopyManager::Set_CopyObject(UIObject* pObj)
{
	CHECK_JUST_NULL(pObj);

	m_SaveInfo = pObj->Save_To_Json();


}

void CopyManager::Paste_CopyObject()
{
	//그대로생성
	if (m_pParser)
	{
		m_SaveInfo.strObjectKey = m_SaveInfo.strObjectKey + "_Copy" + to_string(CopyNumber++);
		m_SaveInfo.m_Local.m_fY += 10;

		m_pParser->Load_UI_Recursive(m_SaveInfo, nullptr);
	}




}

void CopyManager::Free()
{
	
}
