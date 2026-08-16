#include "Client_Define.h"
#include "Sample_Parsing.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_Parsing::Sample_Parsing()
{
}

Client::Sample_Parsing::~Sample_Parsing()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Parsing::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e)
        {
            Start_Save(e);
        });

    m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
        {
            Start_Load(e);
        });

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 파싱 세이브 함수 ////////////////////////////////////////////////////////
void Client::Sample_Parsing::Start_Save(SAVEEVENT _event)
{
    COUT("세이브 시작");

    switch (_event.eToolType)
    {
    case TOOLTYPE::MAP_TOOL:
        m_TempType = TOOLTYPE::MAP_TOOL;
        Collect_Data_Transform();
        break;

    case TOOLTYPE::SHADER_TOOL:
        m_TempType = TOOLTYPE::SHADER_TOOL;
        Collect_Data_Shader();
        break;
    }
    
    // JSON저장
    if(_event.bSaveJson == true)
    {
        m_pGameInstance->SaveAsJson("../DataFiles/Sample/Sample_Shader.json", *this, _event.eToolType);
        // 툴에서 쓸 때는ㅌ
        // m_pGameInstance->SaveAsJson("../DataFiles/Sample/Sample_Map.json", *this, TOOLTYPE::MAP_TOOL);
        //                                                                           ㄴ 이런식으로 툴 타입 지정
    }

    // Binary저장 
    if (_event.bSaveBinary == true)
    {

    }

    COUT("세이브 종료");
}

void Client::Sample_Parsing::Collect_Data_Transform()
{
    m_umapSaveData.clear();

    UMAP<_wstring, class Layer*> layers = m_pGameInstance->Get_Current_Layers();

    // 모든 레이어를 순회
    for (auto& layer : layers)
    {
        // 레이어의 이름을 추출
        _wstring layername = layer.second->Get_LayerName();
        // 레이어가 가지고 있는 오브젝트들의 정보를 추출
        vector<TRANSFORMDATA> objects;

        // 정보 추출을 위해 레이어가 가지고 있는 오브젝트들을 순회
        for (auto& object : layer.second->Get_GameObjects())
        {
            _float4x4 matrix{};
            XMStoreFloat4x4(&matrix, object.second->Get_Transform()->Get_WorldMatrix());
            TRANSFORMDATA data{};
            data.wstrObjName = object.second->Get_Name();
            data.strObjType = object.second->Get_TypeName();
            data.matObjmatrix = matrix;
            objects.push_back(data);
        }

        m_umapSaveData[layername] = objects;
    }
}

void Client::Sample_Parsing::Collect_Data_Shader()
{
    m_umapShaderData_Character.clear();
    m_umapShaderData_Map.clear();

    UMAP<_wstring, class Layer*> layers = m_pGameInstance->Get_Current_Layers();

    // 모든 레이어 순회
    for (auto& [layername, layer] : layers)
    {
        UMAP<_wstring, GameObject*> objects = layer->Get_GameObjects();

        // 레이어의 오브젝트들 순회
        for (auto& [objectname, object] : objects)
        {
            switch (object->Get_OBJType())
            {
            case OBJTYPE::TYPE_MAP:
            {
                // 오브젝트가 모델을 가지고 있는지 검사
                Model* model = object->Get_Model();
                CHECK_JUST_NULL(model);
                
                // 모델이 있으면 모델 이름 확인
                _string modelname = wstringToString(model->Get_PrototypeName());
                
                // 같은 이름의 모델이 있으면 패스
                if (m_umapShaderData_Map.contains(modelname))
                    continue;

                // 모델이 사용하는 셰이더 정보 획득
                _wstring shaderName = object->Get_ShaderName();

                // 이름이 없으면 패스
                if (shaderName.empty())
                    continue;

                // 데이터에 삽입
                SHADERDATA data{};
                data.wstrShaderProto = shaderName;
                data.vecPasses = object->Get_Passes();

                m_umapShaderData_Map[modelname] = data;

                break;
            }

            case OBJTYPE::TYPE_CHARACTER:
            {
                // 오브젝트의 타입 이름 확인
                _string typeName = object->Get_TypeName();

                // 타입 이름 없으면 스킵
                if (typeName.empty())
                    continue;

                // 이미 저장된 타입이면 스킵 (중복 방지)
                if (m_umapShaderData_Character.contains(typeName))
                    continue;

                // 셰이더 정보 수집
                _wstring shaderName = object->Get_ShaderName();
                if (shaderName.empty())
                    continue;

                SHADERDATA data = {};
                data.wstrShaderProto = shaderName;
                data.vecPasses = object->Get_Passes();

                m_umapShaderData_Character[typeName] = data;
                break;
            }

            default:
            {
                // 오브젝트가 모델을 가지고 있는지 검사
                Model* model = object->Get_Model();
                CHECK_JUST_NULL(model);

                // 모델이 있으면 모델 이름 확인
                _string modelname = wstringToString(model->Get_PrototypeName());

                // 같은 이름의 모델이 있으면 패스
                if (m_umapShaderData_Map.contains(modelname))
                    continue;

                // 오브젝트의 모델이 사용하는 셰이더 정보 획득
                _wstring shaderName = object->Get_ShaderName();

                // 이름이 없으면 패스
                if (shaderName.empty())
                    continue;

                // 데이터에 삽입
                SHADERDATA data{};
                data.wstrShaderProto = shaderName;
                data.vecPasses = object->Get_Passes();

                m_umapShaderData_Map[modelname] = data;
            }
            }
        }
    }
}

ordered_json Client::Sample_Parsing::To_Json()
{
    switch (m_TempType)
    {
    case TOOLTYPE::MAP_TOOL:
        return To_Json_Transform();

    case TOOLTYPE::SHADER_TOOL:
        return To_Json_Shader();
    }

    return ordered_json{};
}

ordered_json Client::Sample_Parsing::To_Json_Transform()
{
    ordered_json root;

    // 각 레이어를 순회
    for (auto& [layerName_w, objects] : m_umapSaveData)
    {
        // wstring -> string 변환
        string layerName(layerName_w.begin(), layerName_w.end());

        // 해당 레이어의 오브젝트들을 json 배열로
        ordered_json objectsJson = ordered_json::array();

        for (auto& objData : objects)
        {
            ordered_json objJson;

            // ObjectName wstring -> string변환 후 대입
            string objName(objData.wstrObjName.begin(), objData.wstrObjName.end());
            objJson["name"] = objName;
            objJson["type"] = objData.strObjType;

            // 4x4 행렬을 16개 float 배열로 저장
            ordered_json matrixJson = ordered_json::array();
            const _float* pMatrix = RCAST(const _float*)(&objData.matObjmatrix);
            for (int i = 0; i < 16; ++i)
            {
                matrixJson.push_back(pMatrix[i]);
            }
            objJson["matrix"] = matrixJson;

            objectsJson.push_back(objJson);
        }

        root[layerName] = objectsJson;
    }

    return root;
}

ordered_json Client::Sample_Parsing::To_Json_Shader()
{
    ordered_json root;

    // Character 데이터
    ordered_json characterJson;
    for (auto& [typeName, shaderData] : m_umapShaderData_Character)
    {
        ordered_json shaderJson;

        // 셰이더 프로토타입명 (wstring -> string)
        string shaderProto(shaderData.wstrShaderProto.begin(), shaderData.wstrShaderProto.end());
        shaderJson["shaderProto"] = shaderProto;

        // 패스 배열
        ordered_json passesJson = ordered_json::array();
        for (auto& pass : shaderData.vecPasses)
        {
            passesJson.push_back(pass);
        }
        shaderJson["passes"] = passesJson;

        characterJson[typeName] = shaderJson;
    }
    root["character"] = characterJson;

    // Map 데이터
    ordered_json mapJson;
    for (auto& [modelName, shaderData] : m_umapShaderData_Map)
    {
        ordered_json shaderJson;

        // 셰이더 프로토타입명 (wstring -> string)
        string shaderProto(shaderData.wstrShaderProto.begin(), shaderData.wstrShaderProto.end());
        shaderJson["shaderProto"] = shaderProto;

        // 패스 배열
        ordered_json passesJson = ordered_json::array();
        for (auto& pass : shaderData.vecPasses)
        {
            passesJson.push_back(pass);
        }
        shaderJson["passes"] = passesJson;

        mapJson[modelName] = shaderJson;
    }
    root["map"] = mapJson;

    return root;
}

void Client::Sample_Parsing::To_Binary(std::ofstream& file)
{
}
/******************************************************* 파싱 세이브 함수 *******************************************************/



//////////////////////////////////////////////////////// 파싱 로드 함수 ////////////////////////////////////////////////////////
void Client::Sample_Parsing::Start_Load(LOADEVENT _event)
{
    COUT("로드시작");

    if (m_pGameInstance->LoadFromJson("../DataFiles/Sample/Sample.json", *this))
    {
        Spawn_OBJ();
    }

    COUT("로드종료");
}

void Client::Sample_Parsing::From_Json(const ordered_json& _jsonData)
{
    switch (m_TempType)
    {
    case TOOLTYPE::MAP_TOOL:
        From_Json_Transform(_jsonData);
        break;

    case TOOLTYPE::SHADER_TOOL:
        From_Json_Shader(_jsonData);
        break;
    }

}

void Client::Sample_Parsing::From_Json_Transform(const ordered_json& _jsonData)
{
    m_umapSaveData.clear();

    // JSON에 저장된 모든 레이어를 순회
    for (auto& [key, value] : _jsonData.items())
    {
        _wstring layerName(key.begin(), key.end());

        vector<TRANSFORMDATA> objects;

        // JSON에 저장된 레이어의 모든 오브젝트를 순회
        for (auto& objJson : value)
        {
            TRANSFORMDATA data = {};

            // 이름 읽기
            if (objJson.contains("name"))
            {
                _string name = objJson["name"];
                data.wstrObjName = stringToWstring(name);
            }

            // 타입 읽기
            if (objJson.contains("type"))
            {
                data.strObjType = objJson["type"];
            }

            // 행렬 읽기
            if (objJson.contains("matrix"))
            {
                auto& matrixJson = objJson["matrix"];
                _float* pMatrix = RCAST(_float*)(&data.matObjmatrix);
                for (_uint i = 0; i < 16; ++i)
                {
                    pMatrix[i] = matrixJson[i].get<_float>();
                }
            }

            objects.push_back(data);
        }

        m_umapSaveData[layerName] = objects;
    }
}

void Client::Sample_Parsing::From_Json_Shader(const ordered_json& _jsonData)
{
    m_umapShaderData_Character.clear();
    m_umapShaderData_Map.clear();

    // [Character 데이터 읽기]
    if (_jsonData.contains("character"))
    {
        for (auto& [typeName, shaderJson] : _jsonData["character"].items())
        {
            SHADERDATA data = {};

            // 셰이더 프로토타입명 읽기
            if (shaderJson.contains("shaderProto"))
            {
                _string shaderProto = shaderJson["shaderProto"];
                data.wstrShaderProto = stringToWstring(shaderProto);
            }

            // 패스 배열 읽기
            if (shaderJson.contains("passes"))
            {
                for (auto& pass : shaderJson["passes"])
                {
                    data.vecPasses.push_back(pass.get<_int>());
                }
            }

            m_umapShaderData_Character[typeName] = data;
        }
    }

    // [Map 데이터 읽기]
    if (_jsonData.contains("map"))
    {
        for (auto& [modelName, shaderJson] : _jsonData["map"].items())
        {
            SHADERDATA data = {};

            // 셰이더 프로토타입명 읽기
            if (shaderJson.contains("shaderProto"))
            {
                _string shaderProto = shaderJson["shaderProto"];
                data.wstrShaderProto = stringToWstring(shaderProto);
            }

            // 패스 배열 읽기
            if (shaderJson.contains("passes"))
            {
                for (auto& pass : shaderJson["passes"])
                {
                    data.vecPasses.push_back(pass.get<_int>());
                }
            }

            m_umapShaderData_Map[modelName] = data;
        }
    }
}


void Client::Sample_Parsing::From_Binary(std::ifstream& file)
{
}

void Client::Sample_Parsing::Spawn_OBJ()
{
    // 레이어 개수만큼 순회
    for (auto& [layerName, objects] : m_umapSaveData)
    {
        // 각 레이어에 오브젝트 배치
        for (auto& objData : objects)
        {
            if (objData.strObjType.empty())
                continue;

            // DESC 설정
            GameObject::GAMEOBJECT_DESC objdesc;
            objdesc.bSetWorldPos = true;
            objdesc.matWorldPos = objData.matObjmatrix;

            // 오브젝트 생성
            _wstring protoname = stringToWstring(objData.strObjType);
            GameObject* pObject = nullptr;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(
                _UINT(LEVEL::SAMPLE), protoname,
                _UINT(LEVEL::SAMPLE), layerName,
                &pObject, &objdesc)))
            {
                continue;
            }

            //// [Character 셰이더 매칭]
            //if (m_umapShaderData_Character.contains(objData.strObjType))
            //{
            //    SHADERDATA& shaderData = m_umapShaderData_Character[objData.strObjType];
            //    Apply_ShaderData(pObject, shaderData);
            //}
            //// [Map 셰이더 매칭] - 모델명 기준
            //else if (pObject->Get_Model() != nullptr)
            //{
            //    _string modelName = wstringToString(pObject->Get_Model()->Get_PrototypeName());

            //    if (m_umapShaderData_Map.contains(modelName))
            //    {
            //        SHADERDATA& shaderData = m_umapShaderData_Map[modelName];
            //        Apply_ShaderData(pObject, shaderData);
            //    }
            //}
        }
    }
}
/******************************************************* 파싱 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Parsing* Client::Sample_Parsing::Create()
{
    Sample_Parsing* pInstance = new Sample_Parsing;

	MSG_FAIL(pInstance->Initialize(), L"Sample_Parsing 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_Parsing::Free()
{
	__super::Free();

    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/
