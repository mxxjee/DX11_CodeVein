#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class GameObject;
class Layer;

class GameObjectManager final : public Base
{
private:
	explicit GameObjectManager();
	explicit GameObjectManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~GameObjectManager();

	HRESULT Initialize(const _uint _levelMax);
public:
	_int	Update_Priority(const _float fTimeDelta);
	//업데이트(쓰레드용) - 오버헤드 심한애들
    _int	Update_Parallel(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	HRESULT Add_GameObject(const _uint _prototypeLevelID, const _wstring& _prototypename, const _uint _levelID, const _wstring& _layername,
        GameObject** _outobject = nullptr, void* pArg = nullptr);


	//이미 생성한 객체를 단지 레이어에 추가.
	HRESULT Add_GameObject(GameObject* pGameObject, const _uint _levelID, const _wstring& _layername);

	GameObject* Get_GameObject(const _uint _levelID, const _wstring& _layername, const _wstring& _gameobjectname) const;
	HRESULT Change_Level(const _uint _levelID);
	HRESULT Clear_Level(_uint _levelID);
    unordered_map<_wstring, Layer*>& Get_Current_Layers() {
        return m_umapLayers[m_iCurrentLevel];
    }
	Layer* Get_Layer(const _wstring& _layerName) { 
		if (!m_umapLayers[m_iCurrentLevel].contains(_layerName))
			return nullptr;
		return m_umapLayers[m_iCurrentLevel][_layerName];
	}

    void Delete_DeadGameObject();
	void Set_Parallel_Dirty();

private:
	class GameInstance* m_pGameInstance = { nullptr };

	_uint m_iLevelMax = {};
	_uint m_iCurrentLevel = {};
	
    _uint m_iNextLevel = {};
	unordered_map<_wstring, class Layer*>* m_umapLayers = { nullptr };
	_bool m_bDeadObjectIs = false;

	vector<EventHandle> m_vecEvent;
	vector<GameObject*> m_vecParallelTargets;
	_bool m_bParallelDirty = true;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static GameObjectManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint _levelMax);

public:
	void Free() override final;

};

NS_END
