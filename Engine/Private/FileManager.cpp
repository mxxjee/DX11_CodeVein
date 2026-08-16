#include "Engine_Define.h"
#include "FileManager.h"

#include "GameInstance.h"
#include "Serializable.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::FileManager::FileManager()
{
}

Engine::FileManager::FileManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::FileManager::~FileManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::FileManager::Initialize()
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 데이터 파싱 함수 ////////////////////////////////////////////////////////
_bool Engine::FileManager::SaveAsJson(const _string& _filePath, Serializable& data, const TOOLTYPE _toolType, const _string& version)
{
    _string tooltype = {};
    switch (_toolType)
    {
    case TOOLTYPE::MAP_TOOL:
        tooltype = "MAP_TOOL";
        break;

    case TOOLTYPE::UI_TOOL:
        tooltype = "UI_TOOL";
        break;

    case TOOLTYPE::SHADER_TOOL:
        tooltype = "SHADER_TOOL";
        break;

    case TOOLTYPE::ANIMATION_TOOL:
        tooltype = "ANIMATION_TOOL";
        break;

    case TOOLTYPE::VFX_TOOL:
        tooltype = "VFX_TOOL";
        break;
    }
    try
    {
        // 디렉토리가 없으면 생성
        filesystem::path path(_filePath);
        if (path.has_parent_path())
        {
            filesystem::create_directories(path.parent_path());
        }

        // 헤더 생성
        FileHeader header;
        header.version = version;
        header.toolType = tooltype;
        header.createdAt = GetCurrentTimeToString();
        header.modifiedAt = header.createdAt;

        // 최종 JSON 구성
        ordered_json root;
        root["header"] = header.To_Json();
        root["data"] = data.To_Json();

        // 파일 쓰기
        ofstream file(_filePath);
        if (!file.is_open())
        {
            cerr << "[FileManager] Failed to open file for writing: " << _filePath << endl;
            return false;
        }

        file << root.dump(4); // 들여쓰기 4칸
        file.close();

        COUT("[FileManager] Saved: " << _filePath);
        return true;
    }
    catch (const exception& e)
    {
        cerr << "[FileManager] Save error: " << e.what() << endl;
        return false;
    }
}


_string Engine::FileManager::GetCurrentTimeToString()
{
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);

    tm tm = {};

    // Windows용
    localtime_s(&tm, &time);

    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}
/******************************************************* 데이터 파싱 함수 *******************************************************/



//////////////////////////////////////////////////////// 툴별 로드 함수 ////////////////////////////////////////////////////////
void Engine::FileManager::Load_MapData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        // 맵툴 정보 지역변수 생성
        MapObjectInfo info;
        // Json파일 읽어오기(구조체 안에 함수 있음)
        info.From_Json(objJson);
        // 출력할 정보에 pushback
        _outResult.vecMapObjects.push_back(info);
    }
}

void Engine::FileManager::Load_ShaderData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        // 쉐이더 툴 정보 읽어오기
        ShaderObjectInfo info;
        // Json파일 읽어오기
        info.From_Json(objJson);

        // 이미 있으면 스킵 (프로토타입당 1개)
        if (_outResult.umapShaderInfos.contains(info.strObjectKey))
            continue;

        // 대입
        _outResult.umapShaderInfos[info.strObjectKey] = info;
    }
}

void Engine::FileManager::Load_EffectData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    //ordered_json root = Read_JsonFile(_strFilePath);
    //if (root.empty() || !root.contains("data"))
    //    return;

    //for (const auto& objJson : root["data"])
    //{
    //    EffectObjectInfo info;
    //    info.From_Json(objJson);

    //    if (_outResult.umapEffectInfos.find(info.strObjectKey) != _outResult.umapEffectInfos.end())
    //        continue;

    //    _outResult.umapEffectInfos[info.strObjectKey] = info;
    //}
}

void Engine::FileManager::Load_UIData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        UIObjectInfo info;
        info.From_Json(objJson);
        _outResult.vecUIObjects.push_back(info);
    }
}

void Engine::FileManager::Load_AnimationData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        AnimationObjectInfo info;
        info.From_Json(objJson);

        if (_outResult.umapAnimInfos.find(info.strObjectKey) != _outResult.umapAnimInfos.end())
            continue;

        _outResult.umapAnimInfos[info.strObjectKey] = info;
    }
}
/******************************************************* 툴별 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 폴더 로드 함수 ////////////////////////////////////////////////////////
LevelLoadResult Engine::FileManager::Load_LevelData(const _string& _folderPath, _bool _useOnlyJson)
{
    // 출력할 결과물 지역변수 생성
    LevelLoadResult result;

    // 폴더가 존재하는지 확인
    if (!fs::exists(_folderPath))
    {
        COUT("[Parsing_Loader] 폴더가 존재하지 않습니다: " + _folderPath);
        return result;
    }

    COUT("[Parsing_Loader] 레벨 데이터 로드 시작: " + _folderPath);

    // 확장자 결정
    _string ext = "";
    if (_useOnlyJson)
        ext = ".json";
    else
    {
        ext = ".sihosav";
        // 바이너리 파일 있으면 그걸로 부르고 없으면 json불러서 바이너리화 시킬 예정
    }

    // ========== 맵 데이터(없으면 로드 스킵함) ==========
    _string mapPath = _folderPath + "/MapData" + ext;   // 폴더의 데이터 저장파일.json .sihosav 불러오기
    if (fs::exists(mapPath))
    {
        //if (_useOnlyJson)
            Load_MapData_Json(mapPath, result);
        //else
        //    Load_MapData_Binary(mapPath, result);

        COUT("[Parsing_Loader] MapData 로드 완료 - " + to_string(result.vecMapObjects.size()) + "개 오브젝트");
        result.bReadMap = true;
    }
    else
    {
        COUT("[Parsing_Loader] MapData 없음 - 로드 스킵");
    }

    // ========== 쉐이더 데이터 (선택) ==========
    _string shaderPath = _folderPath + "/ShaderData" + ext;
    if (fs::exists(shaderPath))
    {
        //if (_useOnlyJson)
            Load_ShaderData_Json(shaderPath, result);
        /*else
            Load_ShaderData_Binary(shaderPath, result);*/

        COUT("[Parsing_Loader] ShaderData 로드 완료 - " + to_string(result.umapShaderInfos.size()) + "개 설정");
        // 쉐이더 읽었으니까 로드하겠다고 변경
        result.bReadShader = true;
    }

    // ========== 이펙트 데이터 (선택) ==========
    _string effectPath = _folderPath + "/EffectData" + ext;
    if (fs::exists(effectPath))
    {
        //if (_useOnlyJson)
            Load_EffectData_Json(effectPath, result);
        /*else
            Load_EffectData_Binary(effectPath, result);*/

        COUT("[Parsing_Loader] EffectData 로드 완료 - " + to_string(result.umapEffectInfos.size()) + "개 설정");
        result.bReadEffect = true;
    }

    // ========== UI 데이터 (선택) ==========
    _string uiPath = _folderPath + "/UIData" + ext;
    if (fs::exists(uiPath))
    {
        //if (_useOnlyJson)
            Load_UIData_Json(uiPath, result);
        //else
            //Load_UIData_Binary(uiPath, result);

        COUT("[Parsing_Loader] UIData 로드 완료 - " + to_string(result.vecUIObjects.size()) + "개 UI");
        result.bReadUI = true;
    }

    // ========== 애니메이션 데이터 (선택) ==========
    _string animPath = _folderPath + "/AnimationData" + ext;
    if (fs::exists(animPath))
    {
        //if (_useOnlyJson)
            Load_AnimationData_Json(animPath, result);
        //else
            //Load_AnimationData_Binary(animPath, result);

        COUT("[Parsing_Loader] AnimationData 로드 완료 - " + to_string(result.umapAnimInfos.size()) + "개 설정");
        result.bReadAnimation = true;
    }

    COUT("[Parsing_Loader] 레벨 데이터 로드 완료!");

    return result;
}
/******************************************************* 폴더 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 유틸리티 함수 ////////////////////////////////////////////////////////
ordered_json Engine::FileManager::Read_JsonFile(const _string& _strFilePath)
{
    try
    {
        ifstream file(_strFilePath);
        if (!file.is_open())
            return ordered_json{};

        ordered_json root = ordered_json::parse(file);
        file.close();
        return root;
    }
    catch (const exception& e)
    {
        COUT("[Parsing_Loader] JSON 파싱 에러: " + _string(e.what()));
        return ordered_json{};
    }
}
/******************************************************* 유틸리티 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
FileManager* Engine::FileManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	FileManager* pInstance = new FileManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"FileManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::FileManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

