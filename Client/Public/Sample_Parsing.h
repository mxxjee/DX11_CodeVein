#pragma once

#include "Client_Define.h"
#include "Serializable.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)

class Sample_Parsing final : public Serializable
{
private:
	explicit Sample_Parsing();
	virtual ~Sample_Parsing();

public:
	HRESULT Initialize();

public:
    ordered_json To_Json() override final;
    ordered_json To_Json_Transform();
    ordered_json To_Json_Shader();
    void From_Json(const ordered_json& _jsonData) override final;

    void From_Json_Transform(const ordered_json& _jsonData);
    void From_Json_Shader(const ordered_json& _jsonData);

    void To_Binary(ofstream& _file) override;
    void From_Binary(ifstream& _file) override;

    void Start_Save(SAVEEVENT _event);
    void Collect_Data_Transform();
    void Collect_Data_Shader();

    void Start_Load(LOADEVENT _event);
    void Spawn_OBJ();


private:
    // 예시용, 맵툴
    struct TRANSFORMDATA {
        _wstring wstrObjName = {};  // 오브젝트 이름(JSON에서 확인용)
        _string strObjType = {};    // 오브젝트 타입
        _float4x4 matObjmatrix = {};    // 오브젝트의 월드 위치
    };
    // 
    UMAP<_wstring, vector<TRANSFORMDATA>> m_umapSaveData;

    // 예시용, 쉐이더툴
    struct SHADERDATA {
        _wstring wstrShaderProto = {};    // 셰이더 프로토타입명
        vector<_int> vecPasses = {};      // 메쉬별 패스
    };

    UMAP<_string, SHADERDATA> m_umapShaderData_Character;
    UMAP<_string, SHADERDATA> m_umapShaderData_Map;

    TOOLTYPE m_TempType = { TOOLTYPE::TOOL_END };

private:
    GameInstance* m_pGameInstance = { nullptr };

public:
	static Sample_Parsing* Create();

public:
	void Free() override final;
};

NS_END
