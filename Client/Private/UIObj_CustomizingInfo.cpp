#include "Client_Define.h"
#include "UIObj_CustomizingInfo.h"
#include "UIObj_Text.h"


Client::UIObj_CustomizingInfo::UIObj_CustomizingInfo()
{
}

Client::UIObj_CustomizingInfo::UIObj_CustomizingInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_CustomizingInfo::UIObj_CustomizingInfo(const UIObj_CustomizingInfo& original)
    :UIObject(original)
{
}

Client::UIObj_CustomizingInfo::~UIObj_CustomizingInfo()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_CustomizingInfo::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_CustomizingInfo::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    CHECK_FAILED(Ready_Info_Data(), E_FAIL);


    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< CustomizingInfoUIEvent>([this](const CustomizingInfoUIEvent& e)
        {
            //json파싱한 정보로 세팅한다.
            auto iter = m_CustomInfos.find(e.m_eType);
            if (iter == m_CustomInfos.end())
                return;


            Customizing_Info m_Info = iter->second;

            m_pTitle_Text->Set_Text(m_Info.m_InfoTitle);
            m_pSub_Text->Set_Text(m_Info.m_Desc);

            m_pTitle->Set_Position(m_Info.TitlePos.x,
                                    m_Info.TitlePos.y);


            m_pBackground->Set_Position(m_Info.Background_Pos.x, m_Info.Background_Pos.y);
            m_pBackground->Set_Size(m_Info.Background_Scale.x, m_Info.Background_Scale.y);


            Set_Active(true, false);


        }));


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


void        Client::UIObj_CustomizingInfo::After_ApplyData()
{
    m_pTitle = Get_Child(L"Title");

    m_pTitle_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Info_Title_Text"));
    m_pSub_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Info_Sub_Text"));


    m_pBackground = Get_Child(L"Info_Title_Background");

}

//////////////////////////////////////////////////////////////////////////////////////////////

UIObj_CustomizingInfo* Client::UIObj_CustomizingInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_CustomizingInfo* pInstance = new UIObj_CustomizingInfo(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_CustomizingInfo 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
GameObject* Client::UIObj_CustomizingInfo::Clone(void* pArg)
{

    UIObj_CustomizingInfo* pInstance = new UIObj_CustomizingInfo(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_CustomizingInfo 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////////////////////



void Client::UIObj_CustomizingInfo::Free()
{
    __super::Free();
}

HRESULT Client::UIObj_CustomizingInfo::Ready_Info_Data()
{
    ifstream file("../../DataFiles/Customizing_Data/Customizing_Info_Desc.json");
    if (!file.is_open())
        return E_FAIL;

    ordered_json root = ordered_json::parse(file);
    auto& customizingList = root["CustomizingInfo"];
    int i = 0;

    for (auto& pData : customizingList)
    {
        Customizing_Info Custom_Info;
        Custom_Info.m_eType = CUSTOMIZING_TYPE(i);

        /*일단 값읽자.*/
        string Title = pData["Title"];
        string Description = pData["Description"];

        auto& Layout = pData["Layout"];

		Custom_Info.TitlePos = _float2(Layout["TitlePosition"]["x"], Layout["TitlePosition"]["y"]);
		Custom_Info.Background_Pos = _float2(Layout["Background"]["Position"]["x"],
			Layout["Background"]["Position"]["y"]);

		Custom_Info.Background_Scale = _float2(Layout["Background"]["Scale"]["x"],
			Layout["Background"]["Scale"]["y"]);




        Custom_Info.m_InfoTitle = stringToWstring(Title);
        Custom_Info.m_Desc = stringToWstring(Description);


        m_CustomInfos.emplace(Custom_Info.m_eType, Custom_Info);
        ++i;

    }
    return S_OK;
}
