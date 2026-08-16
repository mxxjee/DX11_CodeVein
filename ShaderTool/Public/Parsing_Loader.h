#pragma once
#include "ShaderTool_Define.h"
#include "FileParsing_Data.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(ShaderTool)

// 레벨 폴더에서 여러 JSON/Binary 파일을 읽어 조합하는 클래스
class Parsing_Loader final : public Base
{
public:
    // ==================== 메인 로드 함수 ====================
    // 레벨 폴더 경로를 받아서 조합된 데이터 반환
    // _strFolderPath: "../Data/Level_Main_Map"
    // _bUseBinary: true면 .bin 파일 우선, false면 .json 파일 우선
    static LevelLoadResult Load_LevelData(const _string& _strFolderPath, _bool _bUseBinary = false);

private:
    // ==================== 각 툴별 JSON 로드 ====================
    static void Load_MapData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_ShaderData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_EffectData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_UIData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_AnimationData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);

    // ==================== 각 툴별 Binary 로드 ====================
    static void Load_MapData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_ShaderData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_EffectData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_UIData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult);
    static void Load_AnimationData_Binary(const _string& _strFilePath, LevelLoadResult& _outResult);

    // ==================== 유틸리티 ====================
    static ordered_json Read_JsonFile(const _string& _strFilePath);

public:
    void Free() override final;
};

NS_END