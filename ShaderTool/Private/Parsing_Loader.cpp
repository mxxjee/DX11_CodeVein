// Parsing_Loader.cpp

#include "ShaderTool_Define.h"
#include "Parsing_Loader.h"

namespace fs = std::filesystem;

LevelLoadResult ShaderTool::Parsing_Loader::Load_LevelData(const _string& _strFolderPath, _bool _bUseBinary)
{
    LevelLoadResult result;

    // 폴더가 존재하는지 확인
    if (!fs::exists(_strFolderPath))
    {
        COUT("[Parsing_Loader] 폴더가 존재하지 않습니다: " + _strFolderPath);
        return result;
    }

    COUT("[Parsing_Loader] 레벨 데이터 로드 시작: " + _strFolderPath);

    // 확장자 결정
    _string ext = _bUseBinary ? ".bin" : ".json";

    // ========== 맵 데이터 (필수) ==========
    _string mapPath = _strFolderPath + "/MapData" + ext;
    if (fs::exists(mapPath))
    {
        if (_bUseBinary)
            Load_MapData_Binary(mapPath, result);
        else
            Load_MapData_Json(mapPath, result);

        COUT("[Parsing_Loader] MapData 로드 완료 - " + to_string(result.vecMapObjects.size()) + "개 오브젝트");
    }
    else
    {
        COUT("[Parsing_Loader] MapData 없음 - 스킵");
    }

    // ========== 쉐이더 데이터 (선택) ==========
    _string shaderPath = _strFolderPath + "/ShaderData" + ext;
    if (fs::exists(shaderPath))
    {
        if (_bUseBinary)
            Load_ShaderData_Binary(shaderPath, result);
        else
            Load_ShaderData_Json(shaderPath, result);

        COUT("[Parsing_Loader] ShaderData 로드 완료 - " + to_string(result.umapShaderInfos.size()) + "개 설정");
    }

    // ========== 이펙트 데이터 (선택) ==========
    _string effectPath = _strFolderPath + "/EffectData" + ext;
    if (fs::exists(effectPath))
    {
        if (_bUseBinary)
            Load_EffectData_Binary(effectPath, result);
        else
            Load_EffectData_Json(effectPath, result);

        COUT("[Parsing_Loader] EffectData 로드 완료 - " + to_string(result.umapEffectInfos.size()) + "개 설정");
    }

    // ========== UI 데이터 (선택) ==========
    _string uiPath = _strFolderPath + "/UIData" + ext;
    if (fs::exists(uiPath))
    {
        if (_bUseBinary)
            Load_UIData_Binary(uiPath, result);
        else
            Load_UIData_Json(uiPath, result);

        COUT("[Parsing_Loader] UIData 로드 완료 - " + to_string(result.vecUIObjects.size()) + "개 UI");
    }

    // ========== 애니메이션 데이터 (선택) ==========
    _string animPath = _strFolderPath + "/AnimationData" + ext;
    if (fs::exists(animPath))
    {
        if (_bUseBinary)
            Load_AnimationData_Binary(animPath, result);
        else
            Load_AnimationData_Json(animPath, result);

        COUT("[Parsing_Loader] AnimationData 로드 완료 - " + to_string(result.umapAnimInfos.size()) + "개 설정");
    }

    COUT("[Parsing_Loader] 레벨 데이터 로드 완료!");

    return result;
}


// ==================== JSON 로드 함수들 ====================

void ShaderTool::Parsing_Loader::Load_MapData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        MapObjectInfo info;
        info.From_Json(objJson);
        _outResult.vecMapObjects.push_back(info);
    }
}

void ShaderTool::Parsing_Loader::Load_ShaderData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ordered_json root = Read_JsonFile(_strFilePath);
    if (root.empty() || !root.contains("data"))
        return;

    for (const auto& objJson : root["data"])
    {
        ShaderObjectInfo info;
        info.From_Json(objJson);

        // 이미 있으면 스킵 (프로토타입당 1개)
        if (_outResult.umapShaderInfos.find(info.strObjectKey) != _outResult.umapShaderInfos.end())
            continue;

        _outResult.umapShaderInfos[info.strObjectKey] = info;
    }
}

void ShaderTool::Parsing_Loader::Load_EffectData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
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

void ShaderTool::Parsing_Loader::Load_UIData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
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

void ShaderTool::Parsing_Loader::Load_AnimationData_Json(const _string& _strFilePath, LevelLoadResult& _outResult)
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


// ==================== Binary 로드 함수들 ====================
// (MapTool_DataExporter의 From_Binary와 동일한 로직)

void ShaderTool::Parsing_Loader::Load_MapData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ifstream file(_strFilePath, ios::binary);
    if (!file.is_open())
        return;

    size_t count = 0;
    file.read(RCAST(char*)(&count), sizeof(size_t));

    _outResult.vecMapObjects.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        MapObjectInfo info;

        // ObjectKey
        size_t keyLen = 0;
        file.read(RCAST(char*)(&keyLen), sizeof(size_t));
        info.strObjectKey.resize(keyLen);
        file.read(&info.strObjectKey[0], keyLen);

        // PrototypeName
        size_t protoLen = 0;
        file.read(RCAST(char*)(&protoLen), sizeof(size_t));
        _string protoStr;
        protoStr.resize(protoLen);
        file.read(&protoStr[0], protoLen);
        info.wstrPrototypeName = stringToWstring(protoStr);

        // LayerName
        size_t layerLen = 0;
        file.read(RCAST(char*)(&layerLen), sizeof(size_t));
        _string layerStr;
        layerStr.resize(layerLen);
        file.read(&layerStr[0], layerLen);
        info.wstrLayerName = stringToWstring(layerStr);

        // WorldMatrix
        file.read(RCAST(char*)(&info.matWorld), sizeof(_float4x4));

        // ObjectType
        file.read(RCAST(char*)(&info.iObjectType), sizeof(_uint));

        _outResult.vecMapObjects.push_back(info);
    }

    file.close();
}

void ShaderTool::Parsing_Loader::Load_ShaderData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    ifstream file(_strFilePath, ios::binary);
    if (!file.is_open())
        return;

    size_t count = 0;
    file.read(RCAST(char*)(&count), sizeof(size_t));

    for (size_t i = 0; i < count; ++i)
    {
        ShaderObjectInfo info;

        // ObjectKey
        size_t keyLen = 0;
        file.read(RCAST(char*)(&keyLen), sizeof(size_t));
        info.strObjectKey.resize(keyLen);
        file.read(&info.strObjectKey[0], keyLen);

        // ObjectType
        file.read(RCAST(char*)(&info.iObjectType), sizeof(_uint));

        // ShaderPrototype
        size_t shaderLen = 0;
        file.read(RCAST(char*)(&shaderLen), sizeof(size_t));
        _string shaderStr;
        shaderStr.resize(shaderLen);
        file.read(&shaderStr[0], shaderLen);
        info.wstrShaderPrototype = stringToWstring(shaderStr);

        // MeshPasses
        size_t passCount = 0;
        file.read(RCAST(char*)(&passCount), sizeof(size_t));
        if (passCount > 0)
        {
            info.vecMeshPasses.resize(passCount);
            file.read(RCAST(char*)(info.vecMeshPasses.data()), passCount * sizeof(_uint));
        }

        if (_outResult.umapShaderInfos.find(info.strObjectKey) == _outResult.umapShaderInfos.end())
            _outResult.umapShaderInfos[info.strObjectKey] = info;
    }

    file.close();
}

void ShaderTool::Parsing_Loader::Load_EffectData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    // TODO: 필요시 구현
}

void ShaderTool::Parsing_Loader::Load_UIData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    // TODO: 필요시 구현
}

void ShaderTool::Parsing_Loader::Load_AnimationData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult)
{
    // TODO: 필요시 구현
}


// ==================== 유틸리티 ====================

ordered_json ShaderTool::Parsing_Loader::Read_JsonFile(const _string& _strFilePath)
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



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Parsing_Loader::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/