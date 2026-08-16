#pragma once

#include "Client_Define.h"
#include "Level.h"
#include "PlayerStatus.h"

NS_BEGIN(Engine)
class ParticleSystem;
class Model;
class Serializable;
NS_END

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

NS_BEGIN(Client)

class Level_Client abstract : public Level
{
protected:
	explicit Level_Client();
	explicit Level_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Client();


public:
	HRESULT Initialize(LEVEL _level);

	//HRESULT Create_PhysX_Model_Actor(Model* pModel, _fmatrix WorldMatrix);

protected:
	virtual HRESULT Ready_UIObject();	// UI는 여기서만 추가(통합하기 위해)
	virtual HRESULT Ready_Effects() final;	// 이펙트도 이 부모 함수에서 불러오기(통합)
	virtual HRESULT Ready_Values();	//UI세팅
	void Publish_ExitEvent(_bool bFadeScreenExit = true, _bool bLoadingExit = true);
	virtual HRESULT Ready_Sky() final;

	HRESULT Spawn_SavePoint(LEVEL _level);
	HRESULT Spawn_MonsterTrigger(LEVEL _level, const _string& strFolderPath, const _string& strFileName);
	HRESULT Spawn_Item(LEVEL _level, const _string& strFolderPath, const _string& strFileName);
	void	Teleport_With_SavePoint();	//매 씬진입때마다 호출한다. 내부적으로 세이브포인트를 통해 타고들어왔는지 확인

protected:

#ifdef _DEBUG

	//키입력을 통한 스테이터스와 ui 연동 확인
	void		Test_PlayerStatus(const _float fTimeDelta);

	//키입력을통한 아이템확인
	void		Test_Inventory(const _float fTimeDelta);

	//겨우살이 ui테스트용
	void		Test_SavePoint(const _float fTimeDelta);


	void		Test_Haze();

	void		Test_WeaponChange();

#endif // _DEBUG

protected:
	/*Status 테스트용*/
	PlayerStatus	m_PlayerStatus;
	class Player*	m_pPlayer = { nullptr }; //이거 PlayerStatus 사용하던거 플레이어거로 바꾸기 위해서


	_bool m_bMapCreate = false;

#pragma region PhysX
	_bool	m_bDrawDebug = false;
	physx::PxScene* m_pPxScene = nullptr;
	physx::PxControllerManager* m_pControllerManager = nullptr;
	physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
#pragma endregion PhysX

	_wstring m_strLevelName = L"";
	_bool m_bToggleCamera = { false };
	class PoolingManager* m_pPoolingManager = { nullptr };

#pragma region Parser
	class Serializable* m_pParsing_Map = { nullptr };
	//class Parser_UITool* m_pParsing_UI = { nullptr }; 메인앱에서만 생성
	class VFX_Parsing* m_pParsing_VFX = { nullptr };
#pragma endregion Parser

public:
	static Level_Client* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

public:
	void Free() override;

private:
	_uint			m_iLevelChangeHandle = 0;

protected:
	Alarm			m_LevelEnterAlarm;	//씬들어올때마다 UI활성 알람설정
	wstring			m_LevelTitle = L"";

};

NS_END
