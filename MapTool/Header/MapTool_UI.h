#pragma once
#include "ImguiWindow.h"
#include "Ladder.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

class CMapTool_UI : public ImguiWindow
{
private:
	CMapTool_UI(ID3D11Device* pD, ID3D11DeviceContext* pC);
	virtual ~CMapTool_UI() = default;

public:
	HRESULT Initialize(void* pArg);
	virtual _uint Update_Contents(_float fDT);

	void Spawn_Function_At(_float4 vPickPos, _wstring ObjName, MAP_TYPE eType, _uint iIndex);
	void Spawn_Player(_float4 vPickPos, _float fSpeed);
	void Spawn_Item_At(_float4 vPickPos, _uint iItemID, _uint iSpawnType);
	void Spawn_Ladder_At(_float4 vPickPos, _float fLength, _float fRotY);

private:
	//bool Save_Data_Json(const _string& strPath);
	void Load_Data_Json(const _string& strPath, const _wstring& wstrPrototype, const _wstring& wstrLayer);
	void Load_Data_Json_W(const _string& strPath, const _wstring& wstrPrototype, const _wstring& wstrLayer);

	void Save_LayerSP_To_Json(const _string& strFilePath);
	void Load_LayerSP_Json(const _string& strFilePath, int iTargetMapType);

	void Save_LayerTrigger_To_Json(const _string& strFilePath);
	void Load_LayerTrigger_Json(const _string& strFilePath);

	void Save_LayerItem_To_Json(const _string& strFilePath);
	void Load_LayerItem_Json(const _string& strFilePath);

	_bool Picking_PhysX(_float3* pOutPos, _float3* pOutNormal);
	void Draw_PhysX_Debug();

private:
	GameInstance* m_pGameInstance = { nullptr };
	GameObject* m_pSelectedObject = { nullptr };

	//파일 루트
	_string m_strCurrentPath;
	_string m_strSelectedFile;

	_bool m_bMonsterPlaceMode = false;
	_bool m_bPlayerPlaceMode = false;

public:
	static CMapTool_UI* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg);
	virtual void Free();


};

