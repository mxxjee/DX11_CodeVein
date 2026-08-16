#include "ShaderTool_Define.h"
#include "Level_Logo.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Shadow.h"
#include "Shader_Parsing.h"
#include "Pos_Parsing.h"
#include "ImguiManager.h"
#include "ShaderImgui.h"

#pragma region PhysXTest
#include "Player.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"

#include "Mesh.h"
#include "MT_Utils.h"
#include "MapObject.h"
#include "GodRay_Sun.h"
#include "GodRayMesh.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::Level_Logo::Level_Logo()
{
}

ShaderTool::Level_Logo::Level_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

ShaderTool::Level_Logo::~Level_Logo()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Level_Logo::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);
	m_pGameInstance->Build_OcTree(_float3(0.f, 0.f, 0.f), 500.f, 5);
	//CHECK_FAILED(Ready_UI(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Terrain(), E_FAIL);
	CHECK_FAILED(Ready_Option(), E_FAIL);
	CHECK_FAILED(Ready_Map(), E_FAIL);
	CHECK_FAILED(Ready_Sky(), E_FAIL);
	CHECK_FAILED(Ready_GodRay(), E_FAIL);
	CHECK_FAILED(Ready_Monster(), E_FAIL);



	m_pGameInstance->Set_UIManager_Active(true);
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 준비 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Level_Logo::Ready_UI()
{
	_wstring layer = Layer_UIs;

	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Camera()
{
	_wstring layer = Layer_Camera;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));
	m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Player()
{
	AnimToolData m_AnimToolData = {};

	_wstring layername = L"Layer_Player";

	//플레이어 설정해줄떄 컨트롤러, 마테리얼(텍스쳐 말고 피직스 물리세계 내 재질)도 같이넘겨줌
	Player::PLAYER_DESC desc;
	//스폰포인트_지하도시
	//desc.vPosition = _float4(-279.f, -33.f, 72.f, 1.f);
	//desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	//지상_지하도시
	desc.vPosition = _float4(-155.f, 10.f, 100.f, 1.f);
	//지상_연습장
	//desc.vPosition = _float4(6.f, -28.f, 1.f, 1.f);

	//지하-devil slave
	//desc.vPosition = _float4(-247.713f, -21.034f, 89.f, 1.f);

	//지하_시작점
	//desc.vPosition = _float4(-216.f, -19.f, 25.f, 1.f);
	//desc.vPosition = _float4(-265.87f, -26.76f, 27.84f, 1.f);
	desc.fSpeed = 30.f;

	PHYSX_CONTROLLER_DESC controllerDesc;
	controllerDesc.vPosition.x = desc.vPosition.x;
	controllerDesc.vPosition.y = desc.vPosition.y;

	_float fAngleDegree = 90.f;
	controllerDesc.fSlopeLimit = cosf(XMConvertToRadians(fAngleDegree));
	//controllerDesc.fSlopeLimit = 0.7f;
	controllerDesc.vPosition.z = desc.vPosition.z;
	controllerDesc.fSlopeLimit = 0.9f;
	controllerDesc.fStepOffset = 1.f;
	controllerDesc.fContactOffset = 0.03f;
	desc.tControllerDesc = controllerDesc;


	Player* pPlayer = nullptr;
	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Player"), m_iLevel, layername, RCAST(GameObject**)(&pPlayer), &desc), E_FAIL);

	Player_MasterRig* pMasterRig = DCAST(Player_MasterRig*)(pPlayer->Find_PartObject(TEXT("Part_MasterRig")));

	m_AnimToolData.pModelCom = dynamic_cast<Model*>(pMasterRig->Get_Component_FromName(Com_Model));

	return S_OK;
}
HRESULT  ShaderTool::Level_Logo::Ready_Monster()
{
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Devil"), m_iLevel, L"Layer_Monster", nullptr);

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Light()
{
	LIGHT_DESC Desc;
	Desc.eType = LIGHT::DIRECTIONAL;
	Desc.vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
	Desc.vAmbient = _float4{ 0.5f, 0.5f, 0.5f, 1.f };
	Desc.vSpecular = _float4{ 0.8f, 0.85f, 0.9f, 1.f }; //0.3으로 해놨었음 
	Desc.vDirection = _float4{ 1.f, -0.7f, 0.15f, 0.f }; //_float4{ 0.f, -0.6f, 1.f, 0.f }; //

	//임시라서 그냥 자체 생산?
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.fRange = 100.f;
	Desc.vPosition = _float4(-279.969f, -6.72f, 66.114f, 1.f);
	Desc.vDiffuse = _float4(20.f, 18.f, 16.f, 1.f);
	Desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	Desc.vDirection = _float4(0.18f, -1.f, 0.06f, 1.f);
	Desc.fInnerCone = cos(XMConvertToRadians(5.5f));
	Desc.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	//약하게 바닥 밝히는 빛추가
	LIGHT_DESC DescLight;
	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.fRange = 95.4f;
	DescLight.vPosition = _float4(-56.272f, 28.35f, -28.211f, 1.f);
	DescLight.vDiffuse = _float4(1.f, 0.9f, 0.8f, 1.f);
	DescLight.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	DescLight.vDirection = _float4(0.f, -0.69f, -1.f, 1.f);
	DescLight.fInnerCone = cos(XMConvertToRadians(1.0f));
	DescLight.fOuterCone = cos(XMConvertToRadians(29.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-56.272f, 28.35f, -96.081f, 1.f);
	DescLight.vDirection = _float4(0.03f, -1.f, 1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	//성당조명
	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.fRange = 24.1f;
	Desc.vPosition = _float4(-68.42f, 31.33f, -29.355f, 1.f);
	Desc.vDiffuse = _float4(20.f, 18.f, 16.f, 1.f);
	Desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	Desc.vDirection = _float4(0.1f, -0.02f, -0.2f, 1.f);
	Desc.fInnerCone = cos(XMConvertToRadians(16.5f));
	Desc.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.vPosition = _float4(-68.42f, 23.71f, -29.355f, 1.f);
	Desc.vDirection = _float4(0.1f, -0.02f, -0.2f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.vPosition = _float4(-44.380f, 31.33f, -29.355f, 1.f);
	Desc.vDirection = _float4(-0.39f, -0.1f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.vPosition = _float4(-44.380f, 23.71f, -29.355f, 1.f);
	Desc.vDirection = _float4(-0.39f, -0.1f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.vPosition = _float4(-56.18f, 25.24f, -27.685f, 1.f);
	Desc.vDirection = _float4(0.f, -0.4f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.vPosition = _float4(-56.18f, 32.94f, -27.685f, 1.f);
	Desc.vDirection = _float4(0.f, -0.4f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	//엔딩조명
	Desc.eType = LIGHT::SPOTLIGHT;
	Desc.fRange = 56.2f;
	Desc.vPosition = _float4(-55.996f, 6.19f, -72.593f, 1.f);
	Desc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
	Desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	Desc.vDirection = _float4(0.f, -1.f, 0.f, 1.f);
	Desc.fInnerCone = cos(XMConvertToRadians(5.5f));
	Desc.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::POINT;
	Desc.fRange = 2.7f;
	Desc.vDiffuse = _float4(1000.f, 900.f, 800.f, 1.f);
	Desc.vPosition = _float4(-55.996f, 6.19f, -72.593f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	Desc.eType = LIGHT::POINT;
	Desc.fRange = 5.2f;
	Desc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
	Desc.vPosition = _float4(-55.996f, -12.41f, -69.013f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vPosition = _float4{ -20.f, 14.f, 0.f, 1.f };
	ShadowDesc.vAt = _float4{ -75.92f, 13.01f, -85.25f, 1.f }; // _float4{ -61.06f, 42.32f, -9.0f, 1.f }
	ShadowDesc.vOrthSize = 500;//XMConvertToRadians(45.0f);
	ShadowDesc.fNearZ = 0.1f;
	ShadowDesc.fFarZ = 500.f;
	ShadowDesc.fDepth = 500;

	if (FAILED(m_pGameInstance->Add_ShadowLight(ShadowDesc)))
		return E_FAIL;

	SHADOW_DESC		ShadowDesc1{};
	ShadowDesc1.vPosition = _float4{ 0.f, 1.f, -1.f, 1.f }; //높이서 내려다봄(안들어감)
	ShadowDesc1.vAt = _float4{ 0.f, 0.f, 0.f, 1.f };
	ShadowDesc1.vOrthSize = 10.f;
	ShadowDesc1.fNearZ = 0.1f;
	ShadowDesc1.fFarZ = 500.f;
	ShadowDesc1.fDepth = 200.f;
	ShadowDesc1.vAddPos = _float4{ -20.f, 20.f, 0.f, 1.f };

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;
	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Terrain()
{
	_wstring layer = Layer_GameObject;

	GameObject::GAMEOBJECT_DESC Desc;
	//Desc.wstrName = L"Terrain";
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_Terrain"), m_iLevel, layer, nullptr, &Desc);

	Desc.wstrName = L"LightArrow";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_LightArrow"), m_iLevel, layer, nullptr, &Desc);

	//Desc.wstrName = L"Map";
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_BossMap"), m_iLevel, layer, nullptr, &Desc);

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Option()
{
	/* Sample_Parsing 오브젝트 */
	m_pParsing_Shader = Shader_parsing::Create();
	m_pParsing_Pos = Pos_Parsing::Create();
	return S_OK;
}
HRESULT  ShaderTool::Level_Logo::Ready_Map()
{
	//첫번째맵
	//m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Main/ShaderTool", m_iLevel, true);
	//베이스
	//m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Base", m_iLevel, true);
	//성당
	m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Church", m_iLevel, true);
	Spawn_SavePoint(LEVEL::LOGO);
	m_bMapCreate = true;
	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_GodRay()
{
	_wstring layer = Layer_GameObject;

	GameObject::GAMEOBJECT_DESC Desc;
	//Main
	//Desc.vPosition = _float4{ -281.6f, -0.8f, 64.5f, 1.f };
	//Desc.vRotation = _float3{ XMConvertToRadians(-6.5f), XMConvertToRadians(0.f), XMConvertToRadians(11.5f) };
	//Desc.vScale = _float3{ 7.f, 1.f, 1.f };
	//Desc.wstrName = L"GodRayMesh";
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	//Desc.wstrName = L"GodRayMesh";
	//Desc.vRotation = { XMConvertToRadians(-13.f), XMConvertToRadians(60.f), XMConvertToRadians(0.5f) };
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	//Desc.wstrName = L"GodRayMesh";
	//Desc.vRotation = { XMConvertToRadians(-6.5f), XMConvertToRadians(120.f), XMConvertToRadians(-11.f) };
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	//Church

	//12개 만든다
#pragma region 1번창가(입구낮은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(53.5f) };
	Desc.vScale = _float3{ 3.76f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -83.9f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
	
	Desc.vRotation = _float3{ XMConvertToRadians(-17.5f), XMConvertToRadians(15.f), XMConvertToRadians(50.5f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
	
	Desc.vRotation = _float3{ XMConvertToRadians(20.f), XMConvertToRadians(-15.f), XMConvertToRadians(52.5f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -89.2f, 24.1f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 2번창가(입구높은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(55.5f) };
	Desc.vScale = _float3{ 4.65f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-19.5f), XMConvertToRadians(15.f), XMConvertToRadians(53.f) };
	Desc.vScale = _float3{ 3.55f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(20.f), XMConvertToRadians(-15.f), XMConvertToRadians(53.f) };
	Desc.vScale = _float3{ 4.89f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 3번창가(안쪽낮은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(53.5f) };
	Desc.vScale = _float3{ 3.76f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-54.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-50.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 4번창가(안쪽높은거)
	Desc.vRotation = _float3{ XMConvertToRadians(-1.f), XMConvertToRadians(0.5f), XMConvertToRadians(55.5f) };
	Desc.vScale = _float3{ 4.09f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-56.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 5.45f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-54.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 5.22f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 엔딩쪽
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(60.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(120.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Spawn_SavePoint(LEVEL _level)
{
	int iTargetMapType = 0;
	switch (_level)
	{
	//case LEVEL::BASE:	iTargetMapType = 1; break;
	case LEVEL::LOGO:	iTargetMapType = 2; break;
		//case LEVEL::CHURCH:	iTargetMapType = 3; break; 
	default:
		return S_OK;
	}

	std::ifstream file("../../DataFiles/Level_All/MapData_SP.json");
	if (!file.is_open())
		return S_OK;

	nlohmann::json root;
	try { file >> root; }
	catch (...) {
		return E_FAIL;
	}

	if (!root.contains("data") || !root["data"].is_array())
		return S_OK;

	for (auto& objNode : root["data"])
	{
		if (objNode.contains("ExtraData") && objNode["ExtraData"].contains("MyMapType"))
		{
			int iSavedMapType = objNode["ExtraData"]["MyMapType"].get<int>();

			if (iSavedMapType != iTargetMapType)
				continue;
		}
		else
		{
			continue;
		}

		if (!objNode.contains("WorldMatrix")) continue;

		MapObject::MAPOBJECT_DESC desc{};

		auto& matArr = objNode["WorldMatrix"];
		if (matArr.size() == 16)
		{
			_float4x4 matWorld{};
			for (_uint i = 0; i < 16; ++i)
				matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

			desc.bSetWorldPos = true;
			desc.matWorldPos = matWorld;
		}

		if (objNode.contains("ExtraData"))
		{
			desc.jExtraData = objNode["ExtraData"];
		}

		_wstring wstrProto = L"Prototype_GameObject_SavePoint";
		if (objNode.contains("PrototypeName"))
		{
			std::string strProto = objNode["PrototypeName"].get<std::string>();
			wstrProto = _wstring(strProto.begin(), strProto.end());
		}

		_wstring wstrLayer = L"Layer_SP";
		if (objNode.contains("LayerName"))
		{
			std::string strLayer = objNode["LayerName"].get<std::string>();
			wstrLayer = _wstring(strLayer.begin(), strLayer.end());
		}

		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(_level), wstrProto, _UINT(_level), wstrLayer, nullptr, &desc)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
HRESULT ShaderTool::Level_Logo::Ready_Sky()
{
	m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject_SkySphere, m_iLevel, L"Layer_Sky");

	GodRay_Sun::GodRayDesc Desc;
	//Desc.vSunPos = _float4{ -122.29f, 105.36f, -153.458f, 1.f };//Main
	//Desc.vSunPos = _float4{ -185.89, 95.86, -43.45, 1.f }; //Base
	//Desc.vSunPos = _float4{ -296.82, 102.69f, -69.81f, 1.f }; //Church
	Desc.vSunPos = _float4{ -56.23, 27.98f, -16.22f, 1.f }; //Church Boss
	m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject_SphereSun, m_iLevel, L"Layer_Sky", nullptr, &Desc);

	//GameObject* temp = m_pGameInstance->Get_GameObject(m_iLevel, L"Layer_Sky", L"GodRay_Sun_0");
	return S_OK;
}
/******************************************************* UI 준비 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
void ShaderTool::Level_Logo::ChangeChurchBoss()
{
	if (m_pGameInstance->KeyDown(DIK_C))
	{
		m_pGameInstance->ChangeChurchBoss();
		GameObject* Sun = m_pGameInstance->Get_GameObject(_UINT(LEVEL::LOGO), L"Layer_Sky", L"GodRay_Sun_0");
		Sun->Get_Transform()->Set_State(DIRECTION::POSITION, _float4{ -56.23, 27.98f, -16.22f, 1.f });
	}
}
/******************************************************* UI 준비 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Level_Logo::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Level_Logo::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Parallel(fTimeDelta);

	m_pGameInstance->Update(fTimeDelta);
	ChangeChurchBoss();
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Level_Logo::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Level_Logo::Render(const _float fTimeDelta)
{
	if (m_bMapCreate)
	{
		m_pGameInstance->Set_Capture(true);
		m_bMapCreate = false;
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Logo* ShaderTool::Level_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Logo* pInstance = new Level_Logo(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Logo 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void ShaderTool::Level_Logo::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Logo 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Level_Logo::Free()
{
	Safe_Release(m_pParsing_Shader);
	Safe_Release(m_pParsing_Pos);
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
