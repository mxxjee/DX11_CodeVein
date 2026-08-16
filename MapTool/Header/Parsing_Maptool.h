#pragma once

#include "Serializable.h"
#include "MT_Defines.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

class Parsing_Maptool final : public Serializable
{
private:
	explicit Parsing_Maptool();
	virtual ~Parsing_Maptool();

public:
	HRESULT Initialize();

public:
	void Start_Save(const SAVEEVENT& _event);
	void Collect_Data();

	// Serializable을(를) 통해 상속됨
	ordered_json To_Json() override;
	void From_Json(const ordered_json& _jsonData) override;

	void To_Binary(ofstream& _file) override;
	void From_Binary(ifstream& _file) override;


public:
	// 저장할 데이터
	vector<MapObjectInfo> m_vecData = {};

private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static Parsing_Maptool* Create();

public:
	void Free() override final;

};

