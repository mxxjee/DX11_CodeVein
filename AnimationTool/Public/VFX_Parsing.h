#pragma once
#include "AnimationTool_Define.h"
#include "Serializable.h"

NS_BEGIN(Engine)
class GameInstance;
class ParticleSystem;
NS_END

NS_BEGIN(AnimationTool)

class VFX_Parsing final : public Serializable
{
private:
	explicit VFX_Parsing();
	virtual ~VFX_Parsing() = default;

public:
	HRESULT Initialize();

	void Start_Load(const LOADEVENT& _event);
	void Load_EffectDatas();

public:
	// Json 직렬화(디버깅용)
	virtual ordered_json To_Json() override;
	virtual void From_Json(const ordered_json& _jsonData) override;

	// Binary 직렬화(릴리즈, 대부분 로드용) ====> 지금은 안씀
	virtual void To_Binary(ofstream& _file) override {};
	virtual void From_Binary(ifstream& _file) override {};

public:
	// 저장할 데이터
	vector<ParticleSystemInfo>			m_vecSystemInfo;

private:
	GameInstance*	m_pGameInstance = { nullptr };

public:
	static VFX_Parsing* Create();
	virtual void Free() override;
};

NS_END