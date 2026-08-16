#pragma once

#include "Serializable.h"
#include "Client_Define.h"


NS_BEGIN(Engine)
class GameInstance;
class UIObject;
NS_END

NS_BEGIN(Client)

class Parser_UITool final: public Engine::Serializable
{
	struct ProtoTypeBundle
	{
		wstring PrototypeName = L"";
		_uint	iType = 0;
		_uint	TypeHash = 0;
		LEVEL  m_iLevel = LEVEL::STATIC;

		vector<string>	m_ChildProtos;//자식클래스이름


	};
private:
    explicit Parser_UITool();
    virtual ~Parser_UITool();

public:
    HRESULT Initialize();



    // 로드 관련
    void Start_Load(const LOADEVENT& _event);
    UIObject* Load_UI_Recursive(const LOADEVENT& _event, UIObjectInfo& info, UIObject* pParent);
	void Spawn_Objects(const LOADEVENT& _event);
	static void Set_LoadLevel(LEVEL i) { m_iLoadLevel = i; }
	void Start_Save_Prefab(const SAVEEVENT& _event);
	void Start_Load_Prefab(const LOADEVENT& _event);


	void	Start_Load_Pooling(const LOADEVENT_UIPOOLING& e);
	void	Spawn_PoolingObjects(const LOADEVENT_UIPOOLING& e);

	void		UnSubcribe_Evnets();
public:
	UIObject* Load_UI_Recursive_Pooling(UIObjectInfo& info, UIObject* pParent);
	UIObject* Load_UI_Recursive_Persistent(UIObjectInfo& info, UIObject* pParent);

public:
	// ========== Serializable 인터페이스 구현 ==========
	virtual ordered_json To_Json() override
	{
		ordered_json dataArray = ordered_json::array();
		for (const auto& info : vecData)
		{
			dataArray.push_back(info.To_Json());
		}
		return dataArray;
	}

	virtual void From_Json(const ordered_json& _jsonData) override
	{
		vecData.clear();

		for (const auto& objJson : _jsonData)
		{
			UIObjectInfo info;
			info.From_Json(objJson);
			vecData.push_back(info);
		}
	}

	virtual void To_Binary(ofstream& _file) override
	{

	}

	virtual void From_Binary(ifstream& _file) override
	{

	}
public:
	// 저장할 데이터
	vector<UIObjectInfo> vecData = {};


private:
	wstring		Get_PrototypeName(string BaseType, string UIType, LEVEL* pOut);

private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static Parser_UITool* Create();
	void Free() override final;

private:
	unordered_map<string, ProtoTypeBundle>	m_BaseKeyHashs;
	unordered_map<string, ProtoTypeBundle>	m_UITypeKeyHashs;

private:
	static LEVEL		m_iLoadLevel ;
	vector<int>		m_EventHandles;

};


NS_END