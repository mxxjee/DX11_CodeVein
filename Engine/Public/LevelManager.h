#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Level;

class LevelManager final : public Base
{
private:
	explicit LevelManager();
	explicit LevelManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~LevelManager();

	HRESULT Initialize(_uint _levelcount, class GameObjectManager* _gameobjectmanaer);

public:
	HRESULT Add_Level(_uint _levelname, Level* level);
	HRESULT Add_PlayerLevel(Level* level);
	HRESULT Change_Level(_uint _levelname);
	HRESULT Delete_Level(_uint _levelname);

	HRESULT Update_Priority(const _float fTimeDelta);
	HRESULT Update(const _float fTimeDelta);
	HRESULT Update_Late(const _float fTimeDelta);
	HRESULT Render_Level(const _float fTimeDelta);

	HRESULT Update_Player_Priority(const _float fTimeDelta);
	HRESULT Update_Player(const _float fTimeDelta);
	HRESULT Update_Player_Late(const _float fTimeDelta);
	HRESULT Render_Player(const _float fTimeDelta);

    Level* Get_Current_Level() { return m_pCurrentLevel; }
	_uint  Get_Current_LevelID() { return m_iCurrentLevelID; }

    Level* Get_Level(_uint _levelID) {
        MSG_NULL(m_vecLevels[_levelID], L"찾으려는 레벨이 현재 Null입니다", L"검색 실패", nullptr);

        return m_vecLevels[_levelID];
    }


private:
	class GameInstance* m_pGameInstance = { nullptr };
	class GameObjectManager* m_pGameObjectManager = { nullptr };

	Level* m_pCurrentLevel = { nullptr };
	Level* m_pPlayerLevel = { nullptr };

	_uint m_iCurrentLevelID = {};

	vector<Level*> m_vecLevels;
	_uint m_iLevelCount = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static LevelManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _levelcount, GameObjectManager* _gameobjectmanaer);

public:
	void Free() override final;

};

NS_END
