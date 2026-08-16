#include "UITool_Define.h"
#include "Sample_Pars_UItool.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "UITool_Macro.h"
#include "UIObject.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
UITool::Sample_Pars_UItool::Sample_Pars_UItool()
{
}

UITool::Sample_Pars_UItool::~Sample_Pars_UItool()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Sample_Pars_UItool::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();


    SAVEEVENT event;
    event.bSaveBinary = true;
    event.bSaveJson = true;
    event.eToolType = TOOLTYPE::UI_TOOL;

    m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::UI_TOOL)
                return;

            //모두저장 or 한 오브젝트만 저장
            if (e.eType == FILETYPE::ALL)
                Start_Save(e);

            else
                Start_Save_Prefab(e);
        });

    m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::UI_TOOL)
                return;

            if (e.eType == FILETYPE::ALL)
                Start_Load(e);

            else
                Start_Load_Prefab(e);
        });

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/



//////////////////////////////////////////////////////// 저장 함수 ////////////////////////////////////////////////////////
void UITool::Sample_Pars_UItool::Start_Save(const SAVEEVENT& _event)
{
    COUT("[Sample_Pars_UItool] 저장 시작");

    // 데이터 수집
    Collect_Data();

    // JSON 저장
    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            _event.Path+".json",
            *this,
            TOOLTYPE::UI_TOOL
        );
        COUT("[Sample_Pars_UItool] JSON 저장 완료");
    }

    // Binary 저장
    if (_event.bSaveBinary)
    {
        //m_pGameInstance->SaveAsBinary(
        //    "../Data/Level_Sample/MapData.sihomap",
        //    *this,
        //    TOOLTYPE::MAP_TOOL
        //);
        COUT("[Sample_Pars_UItool] Binary 저장 완료");
    }

    COUT("[Sample_Pars_UItool] 저장 완료 - " + to_string(vecData.size()) + "개 오브젝트");
}

void UITool::Sample_Pars_UItool::Collect_Data()
{
    vecData.clear();
    m_pGameInstance->Get_Current_Layers();

    //uimanager에는 가장 root인 오브젝트들만 등록되어있으니까 !! uimanager에서 가져와서 저장시킥자
    const unordered_map<size_t, UIObject*> AllUIs = m_pGameInstance->Get_AllUIObjects();

    for (auto& pair : AllUIs)
    {
        if (pair.second)
        {
            if (!pair.second->Is_Visible() )
                continue;

            UIObjectInfo Info = pair.second->Save_To_Json();
            vecData.push_back(Info);
        }
    }
   
}
/******************************************************* 저장 함수 *******************************************************/



//////////////////////////////////////////////////////// 로드 함수 ////////////////////////////////////////////////////////
void UITool::Sample_Pars_UItool::Start_Load(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_UItool] 로드 시작");

    vecData.clear();
    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(_event.Path, *this))
    {
        COUT("[Sample_Pars_UItool] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성
        Spawn_Objects();
    }
    else
    {
        COUT("[Sample_Pars_UItool] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_UItool] 로드 완료");
}

void UITool::Sample_Pars_UItool::Spawn_Objects()
{
    COUT("[Sample_Pars_UItool] UI 계층 구조 생성 시작");

    //최상위 Root 의 데이터만들어있음
    for (auto& info : vecData)
    {
        //root이므로 parent=nullptr
        Load_UI_Recursive(info, nullptr);
        COUT("[성공] 오브젝트 생성: " + info.strObjectKey);
    }

    COUT("[Sample_Pars_UItool] 오브젝트 생성 완료 - " + to_string(vecData.size()) + "개");
}
void Sample_Pars_UItool::Start_Save_Prefab(const SAVEEVENT& _event)
{
    UIObject* pData = static_cast<UIObject*>(_event.pData);
    CHECK_JUST_NULL(pData);

    vecData.clear();
    UIObjectInfo Info = pData->Save_To_Json();
    vecData.push_back(Info);

    // JSON 저장
    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            _event.Path + ".json",
            *this,
            TOOLTYPE::UI_TOOL
        );
        COUT("[Sample_Pars_UItool] JSON 저장 완료");
    }

}
void Sample_Pars_UItool::Start_Load_Prefab(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_UItool] 로드 시작");

    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(_event.Path, *this))
    {
        COUT("[Sample_Pars_UItool] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성
        Spawn_Objects();
    }
    else
    {
        COUT("[Sample_Pars_UItool] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_UItool] 로드 완료");
}
UIObject* Sample_Pars_UItool::Load_UI_Recursive(UIObjectInfo& info, UIObject* pParent)
{
    //프로토타입으로 객체생성
    UIObject* pSelf = nullptr;

    UIObject::UIOBJECT_DESC Desc;
    Desc.wstrName = stringToWstring(info.strObjectKey);

    Desc.fX = info.m_Local.m_fX;
    Desc.fY = info.m_Local.m_fY;
    Desc.fCX = info.m_Local.m_fSizeX;
    Desc.fCY = info.m_Local.m_fSizeY;
    Desc.iZOrder = info.iZOrder;
    Desc.fAlpha = info.m_Local.m_fAlpha;
    if (info.strParentName != "")
        Desc.pParent = pParent;


   
    Desc.bIsActive = info.bActive;
    Desc.bIsVisible = info.bVisible;

    Desc.iShaderNumber = info.iShaderPass;
    Desc.wstrShaderName = stringToWstring(info.ShaderComName);

    //나머지는 이후에세팅
    GameObject* pOut=nullptr;

    if (Desc.pParent)//레이어에등록X
    {
        Base* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::STATIC), L"Prototype_UIObject", &Desc);
        if (pBase)
            pOut = dynamic_cast<GameObject*>(pBase);
    }
            //레이어에등록
    else
        m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject", _UINT(LEVEL::MAIN), Layer_UIs, &pOut, &Desc);

    if (pOut)
    {
        pSelf = dynamic_cast<UIObject*>(pOut);
        if (pSelf)
            pSelf->Apply_Data_From_Info(info);


    }

    if (!pSelf)
        return nullptr;


    for (auto& childInfo : info.m_Children)
    {
        UIObject* pChild = Load_UI_Recursive(childInfo, pSelf);
    //    pSelf->Add_Child_OnLoad(pChild);

    }
    pSelf->After_ApplyData();
    //부모세팅
    return pSelf;
}
/******************************************************* 로드 함수 *******************************************************/




//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Pars_UItool* UITool::Sample_Pars_UItool::Create()
{
    Sample_Pars_UItool* pInstance = new Sample_Pars_UItool();

    pInstance->Initialize();

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void UITool::Sample_Pars_UItool::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/


