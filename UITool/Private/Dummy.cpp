#include "UITool_Define.h"
#include "Dummy.h"
#include "CopyManager.h"

USING(UITool)

Dummy::Dummy()
{
}

Dummy::Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice,pContext)
{
}

Dummy::Dummy(const Dummy& original)
	: GameObject(original)
{
}

Dummy::~Dummy()
{
}




//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Dummy::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT UITool::Dummy::Initialize(void* arg)
{
    static _uint namenum = 0;

    GAMEOBJECT_DESC desc;

    if (arg == nullptr)
    {
        arg = &desc;
    }
    else
    {

    }

    CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Basic_Class_" + to_wstring(namenum++);

    CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Dummy::Update_Late(const _float fTimeDelta)
{
    if (m_pGameInstance->Get_EngineMode() == ENGINEMODE::CLIENT)
    {
        if (m_pGameInstance->KeyDown(DIK_SPACE))
        {
            //프로그레스바 이벤트발송
        }
    }
	return 0;
}
void Dummy::Take_Damage(_float fDamage)
{
    //내 피가 깎였으니 유아이도 깎여라!!
    //UI_MasterEvent masterEvent;
    //masterEvent.eType = UI_EVENT_TYPE::VALUE_SYNC;
    //masterEvent.m_iTargetID = m_iObjectID;
    //masterEvent.m_fValue=
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Dummy* UITool::Dummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Dummy* pInstance = new Dummy(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Dummy 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* UITool::Dummy::Clone(void* pArg)
{
	Dummy* pInstance = new Dummy(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Dummy 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void UITool::Dummy::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
