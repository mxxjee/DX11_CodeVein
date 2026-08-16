// Sample_Pars_Maptool.h

#pragma once
#include "Serializable.h"
#include "Client_Define.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)

class Sample_Pars_Maptool final : public Serializable
{
private:
	explicit Sample_Pars_Maptool();
	virtual ~Sample_Pars_Maptool();

public:
	HRESULT Initialize();

	void Start_Save(const SAVEEVENT& _event);
	void Collect_Data();

	void Start_Load(const LOADEVENT& _event);
	void Spawn_Objects();

public:
	// 저장할 데이터
	vector<MapObjectInfo> vecData = {};

public:
	// ========== Serializable 인터페이스 구현 ==========
	ordered_json To_Json() override
	{
		ordered_json dataArray = ordered_json::array();
		for (const auto& info : vecData)
		{
			dataArray.push_back(info.To_Json());
		}
		return dataArray;
	}

	void From_Json(const ordered_json& _jsonData) override
	{
		vecData.clear();

		for (const auto& objJson : _jsonData)
		{
			MapObjectInfo info;
			info.From_Json(objJson);
			vecData.push_back(info);
		}
	}

	void To_Binary(ofstream& _file) override
	{
		// 개수 저장
		size_t count = vecData.size();
		_file.write(RCAST(_pchar)(&count), sizeof(size_t));

		for (const auto& info : vecData)
		{
			// ObjectKey
			size_t keyLen = info.strObjectKey.size();
			_file.write(RCAST(_pchar)(&keyLen), sizeof(size_t));
			_file.write(info.strObjectKey.c_str(), keyLen);

			// PrototypeName (wstring)
			_string protoStr = wstringToString(info.wstrPrototypeName);
			size_t protoLen = protoStr.size();
			_file.write(RCAST(_pchar)(&protoLen), sizeof(size_t));
			_file.write(protoStr.c_str(), protoLen);

			// LayerName (wstring)
			_string layerStr = wstringToString(info.wstrLayerName);
			size_t layerLen = layerStr.size();
			_file.write(RCAST(_pchar)(&layerLen), sizeof(size_t));
			_file.write(layerStr.c_str(), layerLen);

			// WorldMatrix
			_file.write(RCAST(_cchar)(&info.matWorld), sizeof(_float4x4));

			// ObjectType
			_file.write(RCAST(_cchar)(&info.iObjectType), sizeof(_uint));
		}
	}

	void From_Binary(ifstream& _file) override
	{
		vecData.clear();

		// 개수 읽기
		size_t count = 0;
		_file.read(RCAST(_pchar)(&count), sizeof(size_t));

		vecData.reserve(count);

		for (size_t i = 0; i < count; ++i)
		{
			MapObjectInfo info;

			// ObjectKey
			size_t keyLen = 0;
			_file.read(RCAST(_pchar)(&keyLen), sizeof(size_t));
			info.strObjectKey.resize(keyLen);
			_file.read(&info.strObjectKey[0], keyLen);

			// PrototypeName
			size_t protoLen = 0;
			_file.read(RCAST(_pchar)(&protoLen), sizeof(size_t));
			_string protoStr;
			protoStr.resize(protoLen);
			_file.read(&protoStr[0], protoLen);
			info.wstrPrototypeName = stringToWstring(protoStr);

			// LayerName
			size_t layerLen = 0;
			_file.read(RCAST(_pchar)(&layerLen), sizeof(size_t));
			_string layerStr;
			layerStr.resize(layerLen);
			_file.read(&layerStr[0], layerLen);
			info.wstrLayerName = stringToWstring(layerStr);

			// WorldMatrix
			_file.read(RCAST(_pchar)(&info.matWorld), sizeof(_float4x4));

			// ObjectType
			_file.read(RCAST(_pchar)(&info.iObjectType), sizeof(_uint));

			vecData.push_back(info);
		}
	}

private:
	GameInstance* m_pGameInstance = { nullptr };


public:
	static Sample_Pars_Maptool* Create();

	void Free() override final;
};

NS_END