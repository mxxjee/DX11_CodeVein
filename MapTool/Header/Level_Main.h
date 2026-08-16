#pragma once
#include "MT_Defines.h"
#include "Level.h"

#ifdef _DEBUG
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#else
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#endif

NS_BEGIN(Engine)
class Model;
NS_END

class CLevel_Main final : public Level
{
private:
	CLevel_Main();
	CLevel_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevel_Main(const CLevel_Main& original);
	virtual ~CLevel_Main() = default;

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Object();
	HRESULT Ready_Player();
	HRESULT Ready_Monster();

	HRESULT Ready_Light();
	HRESULT Ready_UI();
	HRESULT Ready_Collider();

private:
	class Parsing_Maptool* m_pMapParser;
	_wstring m_strLevelName = L""; 
	bool	m_bDrawDebug = false;

public:

	static CLevel_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	void Initialize_Level() override;
	void Free() override final;

};

