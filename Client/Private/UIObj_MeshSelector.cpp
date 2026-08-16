#include "Client_Define.h"
#include "UIObj_MeshSelector.h"

Client::UIObj_MeshSelector::UIObj_MeshSelector()
{
}

Client::UIObj_MeshSelector::UIObj_MeshSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObj_CustomizeSelector(pDevice, pContext)
{
}

Client::UIObj_MeshSelector::UIObj_MeshSelector(const UIObj_MeshSelector& original)
	:UIObj_CustomizeSelector(original)

{
}

Client::UIObj_MeshSelector::~UIObj_MeshSelector()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_MeshSelector::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_MeshSelector::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    return S_OK;
}



/// <summary>
/// ///////////////////////////////
/// </summary>

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_MeshSelector* Client::UIObj_MeshSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_MeshSelector* pInstance = new UIObj_MeshSelector(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_MeshSelector 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_MeshSelector::Clone(void* pArg)
{
    UIObj_MeshSelector* pInstance = new UIObj_MeshSelector(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_MeshSelector 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

//////////////////////               
void        Client::UIObj_MeshSelector::After_ApplyData()
{
    __super::After_ApplyData();

    //타입에따라서 customtype value캐싱
    m_eCustomValueType = CUSTOMIZING_VALUE_TYPE::MESH;

    //열릴창을 정의한다
    m_WindowName = L"ItemGrid";

    //미리보기용 

	m_pDisplay_ImgComp = dynamic_cast<UI_Image*>(Get_Component_FromName(Proto_UIImage));


    Set_GridDesc();
}

void Client::UIObj_MeshSelector::Set_GridDesc()
{
    switch (m_eCustomizeType)
    {
    case HAIR:
    {
        if (m_iIdx == 0)
        {
            m_ProtoTexKey = Proto_UITexture("Customize_Hair");
            m_iTotalTex = 5;

        }
    }
        break;
    case EYE:
    {
        if (m_iIdx == 0)
        {
            m_ProtoTexKey = Proto_UITexture("Customize_Eye");
            m_iTotalTex = 20;
        }

        else if (m_iIdx == 1)
        {
			m_ProtoTexKey = Proto_UITexture("Customize_EyeBase");
			m_iTotalTex = 20;

        }

        else
        {
            m_ProtoTexKey = Proto_UITexture("Customize_EyeHighLight");
            m_iTotalTex = 20;

        }
    }
        break;
    case EYEBROW:
    {
        m_ProtoTexKey = Proto_UITexture("Customize_Eyebrow");
        m_iTotalTex = 20;
    }
        break;
    case MAKEUP:
    {
        m_ProtoTexKey = Proto_UITexture("Customize_Eyelash");
        m_iTotalTex = 20;
    }
        break;
    case CLOTHES:
    {
        m_ProtoTexKey = Proto_UITexture("Customize_Clothes");
        m_iTotalTex =6;
    }
        break;
    case FACEPAINT:
        break;

    default:
        break;
    }
}

void Client::UIObj_MeshSelector::Free()
{
    __super::Free();
}