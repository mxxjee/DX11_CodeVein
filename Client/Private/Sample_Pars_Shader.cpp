// Sample_Pars_Shader.cpp

#include "Client_Define.h"
#include "Sample_Pars_Shader.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_Pars_Shader::Sample_Pars_Shader()
{
}

Client::Sample_Pars_Shader::~Sample_Pars_Shader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Pars_Shader::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();

    // 저장 이벤트 구독
    m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::SHADER_TOOL)
                return;

            Start_Save(e);
        });

    // 로드 이벤트 구독
    m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::SHADER_TOOL)
                return;

            Start_Load(e);
        });

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/



//////////////////////////////////////////////////////// 저장 함수 ////////////////////////////////////////////////////////
void Client::Sample_Pars_Shader::Start_Save(const SAVEEVENT& _event)
{
    COUT("[Sample_Pars_Shader] 저장 시작");

    Collect_Data();

    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            "../../DataFiles/Level_Sample/ShaderData.json",
            *this,
            TOOLTYPE::SHADER_TOOL
        );
        COUT("[Sample_Pars_Shader] JSON 저장 완료");
    }

    if (_event.bSaveBinary)
    {
        // TODO: Binary 저장 구현 후 활성화
        COUT("[Sample_Pars_Shader] Binary 저장 (미구현)");
    }

    COUT("[Sample_Pars_Shader] 저장 완료 - " + to_string(vecData.size()) + "개 쉐이더 설정");
}

void Client::Sample_Pars_Shader::Collect_Data()
{
    vecData.clear();

    // 이미 수집한 ObjectKey 추적 (중복 방지)
    unordered_set<_string> setCollectedKeys;

    //저장할 레이어이름 넣기
    vector<_wstring> vecTargetLayers = {
        L"Layer_Sample"
        //L"Layer_BackGround",
        //L"Layer_Environment",
        //L"Layer_Player"
        // 필요한 레이어 추가
    };

    for (const auto& layerName : vecTargetLayers)
    {
        Layer* pLayer = m_pGameInstance->Get_Layer(layerName);
        if (!pLayer)
            continue;

        const auto& mapObjects = pLayer->Get_GameObjects();

        for (auto& [tag, pObj] : mapObjects)
        {
            if (!pObj)
                continue;

            Model* pModel = pObj->Get_Model();
            if (!pModel)
                continue;

            // ObjectKey 추출
            _string strObjectKey = ExtractObjectKey(pModel->Get_FilePath());
            if (strObjectKey.empty())
                continue;

            // 이미 수집한 키면 스킵 (프로토타입당 1개만 저장)
            if (setCollectedKeys.find(strObjectKey) != setCollectedKeys.end())
                continue;

            setCollectedKeys.insert(strObjectKey);

            ShaderObjectInfo info; //최종적으로 저장될부분
            info.strObjectKey = strObjectKey; //주소넣고
            info.iObjectType = _UINT(pObj->Get_OBJType());

            // 쉐이더 프로토타입명 가져오기
            Shader* pShader = pObj->Get_Shader(); //각 클래스(아래)에서 구현하기
            if (pShader)
            {
                info.wstrShaderPrototype = pShader->Get_PrototypeName();
            }
            else
            {
                // 기본 쉐이더 설정 (예시)
                info.wstrShaderPrototype = L"Prototype_Component_Shader_VTXAnimMesh";
            }

            // 메쉬별 패스 정보 가져오기
            _uint iNumMeshes = pModel->Get_NumMeshes();
            info.vecMeshPasses.resize(iNumMeshes, 0);  // 기본 패스 0으로 초기화

             // 패스 설정
            info.vecMeshPasses = pObj->Get_Passes();

            vecData.push_back(info);

            COUT("[수집] 쉐이더 정보: " + strObjectKey + " / 메쉬 수: " + to_string(iNumMeshes));
        }
    }
}
/******************************************************* 저장 함수 *******************************************************/



//////////////////////////////////////////////////////// 로드 함수 ////////////////////////////////////////////////////////
void Client::Sample_Pars_Shader::Start_Load(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_Shader] 로드 시작");

    if (m_pGameInstance->LoadFromJson("../../DataFiles/Level_Sample/ShaderData.json", *this))
    {
        COUT("[Sample_Pars_Shader] JSON 로드 성공 - " + to_string(vecData.size()) + "개 쉐이더 설정");

        // 로드한 데이터 출력 (디버깅용)
        for (const auto& info : vecData)
        {
            COUT("  - " + info.strObjectKey + " : " + wstringToString(info.wstrShaderPrototype));
        }
    }
    else
    {
        COUT("[Sample_Pars_Shader] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_Shader] 로드 완료");
}
/******************************************************* 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Pars_Shader* Client::Sample_Pars_Shader::Create()
{
    Sample_Pars_Shader* pInstance = new Sample_Pars_Shader();

    pInstance->Initialize();

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_Pars_Shader::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/