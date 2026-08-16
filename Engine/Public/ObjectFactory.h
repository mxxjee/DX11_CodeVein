#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class GameObject;
class GameInstance;

class ObjectFactory final : public Base
{
public:
	explicit ObjectFactory();
	virtual ~ObjectFactory();
	using CloneFunc = function<GameObject* (void*)>;

public:
	HRESULT Initialize();
	HRESULT Spawn_Object();

	// 폴더 경로와 생성할 레벨을 받아서 생성
	HRESULT Spawn_LevelData(const _string& _folderPath, _uint _level_for_create, _bool _useOnlyJson);
	// 로드 결과로 레벨을 받아서 생성
	HRESULT Spawn_LevelData(LevelLoadResult _result, _uint _level_for_create);

private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static ObjectFactory* Create();

public:
	void Free() override final;
	
};


NS_END