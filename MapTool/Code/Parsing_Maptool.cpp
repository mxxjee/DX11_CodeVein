#include "MT_Defines.h"
#include "Parsing_Maptool.h"
#include "GameInstance.h"
#include "Layer.h"
#include "MapObject.h"

Parsing_Maptool::Parsing_Maptool()
{
}

Parsing_Maptool::~Parsing_Maptool()
{
}

HRESULT Parsing_Maptool::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e) {
		Start_Save(e);
		});

	return S_OK;
}

void Parsing_Maptool::Start_Save(const SAVEEVENT& _event)
{
    COUT("[Sample_Pars_Maptool] 저장 시작");

    // 데이터 수집
    Collect_Data();

    // JSON 저장
    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            _event.strSaveFilePath,
            *this,
            TOOLTYPE::MAP_TOOL
        );
        COUT("[Sample_Pars_Maptool] JSON 저장 완료");
    }

    // Binary 저장
    if (_event.bSaveBinary)
    {
        //m_pGameInstance->SaveAsBinary(
        //    "../Data/Level_Sample/MapData.sihomap",
        //    *this,
        //    TOOLTYPE::MAP_TOOL
        //);
        COUT("[Sample_Pars_Maptool] Binary 저장 완료");
    }

    COUT("[Sample_Pars_Maptool] 저장 완료 - " + to_string(m_vecData.size()) + "개 오브젝트");
}

void Parsing_Maptool::Collect_Data()
{
    m_vecData.clear();
    m_pGameInstance->Get_Current_Layers();

    // 현재 레벨의 모든 레이어 순회
    // 실제 맵툴에서는 저장할 레이어들을 지정해서 사용
    vector<_wstring> vecTargetLayers = {
        L"Layer_Enviroment",
        L"Layer_Object", // 단순 충돌체
        L"Layer_Trigger", // 몬스터 트리거
        L"Layer_VisualObj", // 물, 광원등.
        L"Layer_Function" // 기능성 오브젝트. 객체를 따로 가지고있음
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

            if (pObj->Is_Dead())
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

            // 사다리의 칸수, 몬스터의 종류 등 일부만 쓰는 정보를 담을 곳
            MapObject* pMapObj = dynamic_cast<MapObject*>(pObj);
            if (pMapObj)
            {
                LightComponent* pLightCom = pMapObj->Get_LightPtr();
                if (pLightCom != nullptr)
                {
                    ordered_json jLiveExtra = pMapObj->Get_ExtraData();

                    LIGHT_DESC* pLiveDesc = m_pGameInstance->Get_LightDesc(pLightCom->Get_LightIndex());
                    if (pLiveDesc != nullptr)
                    {
                        jLiveExtra["HasPointLight"] = true;

                        jLiveExtra["PointLight"]["eType"] = (int)pLiveDesc->eType;
                        jLiveExtra["PointLight"]["Diffuse"] = { pLiveDesc->vDiffuse.x, pLiveDesc->vDiffuse.y, pLiveDesc->vDiffuse.z, pLiveDesc->vDiffuse.w };
                        jLiveExtra["PointLight"]["Ambient"] = { pLiveDesc->vAmbient.x, pLiveDesc->vAmbient.y, pLiveDesc->vAmbient.z, pLiveDesc->vAmbient.w };
                        jLiveExtra["PointLight"]["Specular"] = { pLiveDesc->vSpecular.x, pLiveDesc->vSpecular.y, pLiveDesc->vSpecular.z, pLiveDesc->vSpecular.w };
                        jLiveExtra["PointLight"]["Direction"] = { pLiveDesc->vDirection.x, pLiveDesc->vDirection.y, pLiveDesc->vDirection.z, pLiveDesc->vDirection.w };
                        jLiveExtra["PointLight"]["Position"] = { pLiveDesc->vPosition.x, pLiveDesc->vPosition.y, pLiveDesc->vPosition.z, pLiveDesc->vPosition.w };
                        jLiveExtra["PointLight"]["Range"] = pLiveDesc->fRange;

                        pMapObj->Set_ExtraData(jLiveExtra);
                    }
                }

                info.jExtraData = pMapObj->Get_ExtraData();
            }

            m_vecData.push_back(info);
        }
    }
}


ordered_json Parsing_Maptool::To_Json()
{
	ordered_json dataArray = ordered_json::array();
	for (const auto& info : m_vecData)
	{
		dataArray.push_back(info.To_Json());
	}
	return dataArray;
}

void Parsing_Maptool::From_Json(const ordered_json& _jsonData)
{
	m_vecData.clear();

	for (const auto& objJson : _jsonData)
	{
		MapObjectInfo info;
		info.From_Json(objJson);
		m_vecData.push_back(info);
	}
}

void Parsing_Maptool::To_Binary(ofstream& _file)
{
}

void Parsing_Maptool::From_Binary(ifstream& _file)
{
}


Parsing_Maptool* Parsing_Maptool::Create()
{
	Parsing_Maptool* pInstance = new Parsing_Maptool;

    pInstance->Initialize();

	return pInstance;
}

void Parsing_Maptool::Free()
{
	__super::Free();

    Safe_Release(m_pGameInstance);
}