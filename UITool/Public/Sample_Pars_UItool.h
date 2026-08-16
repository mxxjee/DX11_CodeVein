#pragma once
#include "Serializable.h"
#include "UITool_Define.h"

NS_BEGIN(Engine)
class GameInstance;
class UIObject;
NS_END

NS_BEGIN(UITool)
class Sample_Pars_UItool :
    public Serializable
{
private:
	explicit Sample_Pars_UItool();
	virtual ~Sample_Pars_UItool();

public:
	HRESULT Initialize();

	void Start_Save(const SAVEEVENT& _event);
	void Collect_Data();

	void Start_Load(const LOADEVENT& _event);
	void Spawn_Objects();

	void Start_Save_Prefab(const SAVEEVENT& _event);
	void Start_Load_Prefab(const LOADEVENT& _event);

	UIObject* Load_UI_Recursive(UIObjectInfo& info, UIObject* pParent);


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
			UIObjectInfo info;
			info.From_Json(objJson);
			vecData.push_back(info);
		}
	}

	void To_Binary(ofstream& _file) override
	{

	}

	void From_Binary(ifstream& _file) override
	{
		
	}
public:
	// 저장할 데이터
	vector<UIObjectInfo> vecData = {};


private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static Sample_Pars_UItool* Create();
	void Free() override final;
};

NS_END