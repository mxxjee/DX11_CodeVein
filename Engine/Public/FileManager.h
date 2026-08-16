#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Serializable;

class ENGINE_DLL FileManager final : public Base
{
private:
	explicit FileManager();
	explicit FileManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~FileManager();

public:
	HRESULT Initialize();

#pragma region 개별 저장 + 불러오기
public:
	_bool SaveAsJson(const _string& _filePath, Serializable& data, const TOOLTYPE _toolType, const _string& _version = "1.0.0");

	template<typename T>
	_bool LoadFromJson(const string& _filePath, T& _outData, FileHeader* _outHeader = nullptr);


	static void Load_MapData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
	static void Load_ShaderData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
	static void Load_EffectData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
	static void Load_UIData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
	static void Load_AnimationData_Json(const _string& _strFilePath, LevelLoadResult& _outResult);
#pragma endregion 개별 저장 + 불러오기


#pragma region 폴더 통째로 불러오기(레벨 단위)
	static LevelLoadResult Load_LevelData(const _string& _folderPath, _bool _useOnlyJson = false);
#pragma endregion 폴더 통째로 불러오기(레벨 단위)


#pragma region 유틸리티
	static ordered_json Read_JsonFile(const _string& _strFilePath);
#pragma endregion


private:
	_string GetCurrentTimeToString();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static FileManager* Create();
	static FileManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;
};

NS_END

#include "FileManager.inl"
