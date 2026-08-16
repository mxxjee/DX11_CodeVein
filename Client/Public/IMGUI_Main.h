#pragma once

#include "Client_Define.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

NS_BEGIN(Client)

class IMGUI_Main final : public ImguiWindow
{
private:
	explicit IMGUI_Main();
	explicit IMGUI_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~IMGUI_Main();

public:
	HRESULT Initialize(void* arg);
	_uint Update_Contents(_float fTimeDelta) override final;
	void Draw_Hierarchy();
	void Draw_FileMenu();
	_int Render(const _float fTimeDelta);

	void Draw_Prototypes();			// 프로토타입 리스트 띄우기
	void Update_PrototypeList();

	void Draw_FastSpeed();
	void Draw_ObjectPool();
	void Update_ObjectPool();

	HRESULT Ready_Character();

#ifdef _DEBUG
	void Draw_PhysX_Debug();
#endif // _DEBUG

	void Draw_CameraSet();

	void Save_CinematicPreset(const string& filePath);

	void Load_CinematicPreset(const string& filePath);

private:
	UMAP<_wstring, class GameObject*> m_pPrototypes;
	GameObject* m_pSelectedPrototype = {};
	_wstring m_wstrSelectedPrototype = {};
	_bool m_bShowPrototypes = { false };
	
	UMAP<_wstring, POOL_ID> m_umapObjectPool;
	POOL_ID m_eSelectedPool = { POOL_ID::END };
	_wstring m_wstrSelectedPool = {};

	_bool m_bTurnBack = { false };

	static const _uint MAX_TEST_KEYFRAMES = 32;
	CINEMATIC_KEYFRAME m_testKeyFrames[MAX_TEST_KEYFRAMES] = {};
	_int m_iTestKeyCount = 2;       // 테스트에 사용할 키프레임 수
	_bool m_bTestAutoReturn = true;

private:
	GameInstance* m_pGameInstance = { nullptr };
	class PoolingManager* m_pPoolingManager = { nullptr };

public:
	static IMGUI_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);

public:
	void Free() override final;
};

NS_END
