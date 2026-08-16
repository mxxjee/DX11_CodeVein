#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Layer final : public Base
{
private:
	explicit Layer();
	explicit Layer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Layer();

	HRESULT Initialize();
public:
	_int	Update_Priority(const _float fTimeDelta);
    _int    Update_Parallel(const _float fTimeDelta);
	_int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	HRESULT Add_GameObject(class GameObject* pGameObject);

    void Set_Name(const _wstring& _name) { m_wstrName = _name; }
	
    GameObject* Get_GameObject(const _wstring& gameobjectname) const;
	unordered_map<_wstring, class GameObject*>& Get_GameObjects() { return m_umapGameObjects; }
	vector<GameObject*>& Get_GameObjects_Vector() { return m_vecGameObjects; }
	vector<GameObject*>& Get_VecGameObjects() { return m_vecGameObjects; }
    _wstring Get_Name() { return m_wstrName; }
	_wstring Get_LayerName() { return m_wstrName; }

	// 파트오브젝트 레이어는 업데이트 자체를 하지 않기 위해
	_bool Is_PartObjLayer() const { return m_bIsPartObjLayer; }
	void Set_PartObjLayer(_bool _partobjLayer) { m_bIsPartObjLayer = _partobjLayer; }

    _bool Delete_DeadGameObject();
	_bool Remove_GameObject(class GameObject* pGameObject);	//단지 그냥 맵에서지우는함수

private:
	unordered_map<_wstring, class GameObject*> m_umapGameObjects;
	vector<GameObject*> m_vecGameObjects;
    _wstring m_wstrName = {};

	_bool m_bIsPartObjLayer = { false };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static Layer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
