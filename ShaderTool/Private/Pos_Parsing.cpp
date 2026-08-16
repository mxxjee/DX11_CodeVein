#include "ShaderTool_Define.h"
#include "Pos_Parsing.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::Pos_Parsing::Pos_Parsing()
{
}

ShaderTool::Pos_Parsing::~Pos_Parsing()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Pos_Parsing::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();


    SAVEEVENT event;
    event.bSaveBinary = true;
    event.bSaveJson = true;
    event.eToolType = TOOLTYPE::MAP_TOOL;

    m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e)
    {
        if (e.eToolType != TOOLTYPE::MAP_TOOL)
            return;

        Start_Save(e);
    });

    m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
    {
        if (e.eToolType != TOOLTYPE::MAP_TOOL)
            return;

        Start_Load(e);
    });

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/



//////////////////////////////////////////////////////// 저장 함수 ////////////////////////////////////////////////////////
void ShaderTool::Pos_Parsing::Start_Save(const SAVEEVENT& _event)
{
    COUT("[Pos_Parsing] 저장 시작");

    // 데이터 수집
    Collect_Data();

    // JSON 저장
    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            "../../DataFiles/Level_Sample/MapData.json",
            *this,
            TOOLTYPE::MAP_TOOL
        );
        COUT("[Pos_Parsing] JSON 저장 완료");
    }

    // Binary 저장
    if (_event.bSaveBinary)
    {
        //m_pGameInstance->SaveAsBinary(
        //    "../Data/Level_Sample/MapData.sihomap",
        //    *this,
        //    TOOLTYPE::MAP_TOOL
        //);
        COUT("[Pos_Parsing] Binary 저장 완료");
    }

    COUT("[Pos_Parsing] 저장 완료 - " + to_string(vecData.size()) + "개 오브젝트");
}

void ShaderTool::Pos_Parsing::Collect_Data()
{
    vecData.clear();
    m_pGameInstance->Get_Current_Layers();

    // 현재 레벨의 모든 레이어 순회
    // 실제 맵툴에서는 저장할 레이어들을 지정해서 사용
    vector<_wstring> vecTargetLayers = {
           L"Layer_GameObject"
        //L"Layer_BackGround",
        //L"Layer_Environment",
        //L"Layer_Player"
        // 필요한 레이어 추가
    };
    for (const auto& layerName : vecTargetLayers)
    {
        Layer* pLayer = m_pGameInstance->Get_Layer(layerName);
        if (!pLayer)
        {
            MSG_ON((L"저장하려는 레이어 \"" + layerName + L"\" 이 없습니다!").c_str(), L"저장 오류");
            continue;
        }

        const auto& mapObjects = pLayer->Get_GameObjects();

        for (auto& [tag, pObj] : mapObjects)
        {
            if (!pObj)
                continue;

            // 모델 컴포넌트 가져오기
            Model* pModel = pObj->Get_Model();
            if (!pModel)
                continue;

            MapObjectInfo info;

            // ObjectKey 추출(진짜개중요함 이걸로 오브젝트에 쉐이더나 이것저것 붙이기)
            info.strObjectKey = ExtractObjectKey(pModel->Get_FilePath());

            // 파일 경로가 비어있으면 태그로 대체
            if (info.strObjectKey.empty())
            {
                info.strObjectKey = wstringToString(tag);
                COUT("[경고] 모델 파일 경로 없음, 태그 사용: " + info.strObjectKey);
            }

            info.wstrPrototypeName = pObj->Get_PrototypeName();
            info.wstrLayerName = layerName;
            XMStoreFloat4x4(&info.matWorld, pObj->Get_Transform()->Get_WorldMatrix());
            info.iObjectType = _UINT(pObj->Get_OBJType());

            vecData.push_back(info);
        }
    }
}
/******************************************************* 저장 함수 *******************************************************/



//////////////////////////////////////////////////////// 로드 함수 ////////////////////////////////////////////////////////
void ShaderTool::Pos_Parsing::Start_Load(const LOADEVENT& _event)
{
    COUT("[Pos_Parsing] 로드 시작");

    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson("../../DataFiles/Level_Sample/MapData.json", *this))
    {
        COUT("[Pos_Parsing] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성
        Spawn_Objects();
    }
    else
    {
        COUT("[Pos_Parsing] JSON 로드 실패!");
    }

    COUT("[Pos_Parsing] 로드 완료");
}

void ShaderTool::Pos_Parsing::Spawn_Objects()
{
    COUT("[Pos_Parsing] 오브젝트 생성 시작");

    for (const auto& info : vecData)
    {
        // 프로토타입으로 오브젝트 클론
        GameObject* pObj = nullptr;
        GameObject::GAMEOBJECT_DESC desc;
        desc.bSetWorldPos = true;
        desc.matWorldPos = info.matWorld;

        m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::LOGO), info.wstrPrototypeName, _UINT(LEVEL::LOGO), info.wstrLayerName, &pObj, &desc);
        if (!pObj)
        {
            COUT("[오류] 프로토타입 클론 실패: " + wstringToString(info.wstrPrototypeName));
            continue;
        }

        COUT("[성공] 오브젝트 생성: " + info.strObjectKey);
    }

    COUT("[Pos_Parsing] 오브젝트 생성 완료 - " + to_string(vecData.size()) + "개");
}
/******************************************************* 로드 함수 *******************************************************/




//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Pos_Parsing* ShaderTool::Pos_Parsing::Create()
{
    Pos_Parsing* pInstance = new Pos_Parsing();

    pInstance->Initialize();

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Pos_Parsing::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
