#include "Client_Define.h"
#include "UIObj_TextureSelector.h"

Client::UIObj_TextureSelector::UIObj_TextureSelector()
{
}

Client::UIObj_TextureSelector::UIObj_TextureSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObj_CustomizeSelector(pDevice, pContext)
{
}

Client::UIObj_TextureSelector::UIObj_TextureSelector(const UIObj_TextureSelector& original)
	:UIObj_CustomizeSelector(original)

{
}

Client::UIObj_TextureSelector::~UIObj_TextureSelector()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_TextureSelector::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_TextureSelector::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    return S_OK;
}
/// ///////////////////////////////

void Client::UIObj_TextureSelector::Free()
{
    __super::Free();
}


//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_TextureSelector* Client::UIObj_TextureSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_TextureSelector* pInstance = new UIObj_TextureSelector(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_TextureSelector 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_TextureSelector::Clone(void* pArg)
{
    UIObj_TextureSelector* pInstance = new UIObj_TextureSelector(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_TextureSelector 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

//////////////////////               
void        Client::UIObj_TextureSelector::After_ApplyData()
{
    __super::After_ApplyData();

    //타입에따라서 customtype value캐싱
    m_eCustomValueType = CUSTOMIZING_VALUE_TYPE::TEXTURE;

    //열릴창을 정의한다
    m_WindowName = L"ItemGrid";

    //미리보기용 
 
    
     m_pDisplay_ImgComp = dynamic_cast<UI_Image*>(Get_Component_FromName(Proto_UIImage));
    

    Set_GridDesc();

}

void Client::UIObj_TextureSelector::Set_GridDesc()
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
        }

        else if (m_iIdx == 1)
        {
            m_ProtoTexKey = Proto_UITexture("Customize_EyeBase");
            m_iTotalTex = 12;

        }

        else
        {
            m_ProtoTexKey = Proto_UITexture("Customize_EyeHighLight");
            m_iTotalTex = 12;

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
        m_iTotalTex = 6;
    }
    break;
    case FACEPAINT: 
    {
        m_ProtoTexKey = Proto_UITexture("Customize_FacePaint");
        m_iTotalTex = 17;
    }
        break;

    default:
        break;
    }
}