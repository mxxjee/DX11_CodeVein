#include "Client_Define.h"
#include "Loader.h"
#include "GameInstance.h"



//여기서 패스를 만든다
#pragma region GameObject
#include "Monster_Test.h"
#pragma endregion


#pragma region ClientComponent
#include "MinimapRenderComponent.h"
#include "Monster_Stat.h"
#include "Player_Stat.h"	
#include "Weapon_Stat.h"

#pragma endregion
#pragma region UIObject
#include "UIObject.h"
#include "UI_Test.h"
#include "UIObj_PlayerHUD.h"
#include "UIObj_ProgressBar.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Slot.h"
#include "UIObj_Text.h"


#include "UIObj_SlotGrid.h"
#include "UIObj_SubWindow.h"
#include "UIObj_QuickSlot.h"

#include "UIObj_Window_Inventory.h"
#include "UIObj_Window_PlayerMenu.h"
#include "UIObj_Window_Bloodcode.h"
#include "UIObj_SubDesc.h"
#include "UIObj_ShortCutMenu.h"
#include "UIObj_ShortCutQuickSlot.h"
#include "UIObj_PopUp_ItemGet.h"
#include "UIObj_PopUp_Interaction.h"
#include "UIObj_BossName.h"
#include "UIObj_HpBar.h"
#include "UIObj_Blur.h"
#include "UIObj_Minimap.h"
#include "UIObj_MonsterStatus.h"

#include "UIObj_Window_Title.h"
#include"UIObj_GlowButton.h"
#include "UIObj_Selector.h"

#include "UIObj_SceneSelectMenu.h"
#include "UIObj_SceneSlot.h"
#include "UIObj_SavePoint_Slot.h"
#include "UIObj_PopUp_MapTitle.h"

#include "UIObj_Haze.h"
#include "UIObj_ManaInfo.h"

#include "UIObj_NpcDialogue.h"
#include "UIObj_Focus_NPCMenu.h"
#include "UIObj_Npc_ChoiceMenu.h"

#include "UIObj_Ending.h"

////////SHOP//////
#include "UIObj_Window_Shop.h"
#include "UIObj_ShopCategory.h"
#include "UIObj_ShopSlotGrid.h"
#include "UIObj_ShopSlot.h"
#include "UIObj_Window_ShopDesc.h"
#include "UIObj_Window_ShopSubDesc.h"
#include "UIObj_Window_NeedInfo.h"

#include "NPC_Murasame.h"

#include "UIObj_CustomizingInfo.h"
#include "UIObj_CustomMenu.h"
#include "UIObj_CustomMenuButton.h"

#include "UIObj_Window_Palette.h"
#include "UIObj_PaletteArea.h"
#include "UIObj_PaletteDisplay.h"
#include "UIObj_VerticalSlider.h"

#include "UIObj_CustomizeGrid.h"
#include "UIObj_ColorSelector.h"

#include "UIObj_TextureSelector.h"
#include "UIObj_MeshSelector.h"
#include "UIObj_ItemGrid.h"

#include "UIObj_ValueSelector.h"
#include "UIObj_BossDead.h"

#include "UIObj_Window_LevelUp.h"

#pragma endregion

#pragma region Map
#include "SavePoint.h"
#include "MapSeal.h"
#include "StaticObject.h"
#include "Ladder.h"
#include "MT_Utils.h"
#include "VIBuffer_SkySphere.h"
#include "Sky_Sphere.h"
#include "GodRayMesh.h"
#include "GodRayObject.h"
#include "GodRaySun.h"
#include "Drum.h"
#include "Item.h"
#include "Item_Box.h"
#pragma endregion


#pragma region Camera
#include "Camera_Free.h"
#include "Camera_Object.h"
#include "Camera_Player.h"
#include "Camera_NPC.h"
#include "Camera_Customize.h"
#pragma endregion

#pragma region Player
#include "Player.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"
#include "Player_Outer.h"
#include "Player_BloodWeapon.h"
#include "Player_Injection.h"
#include "StateMachine.h"
//Weapon
#include "PWeapon_Bayonet.h"
#include "PWeapon_GreatSword.h"
#include "PWeapon_Halberd.h"
#include "PWeapon_Hammer.h"
#include "PWeapon_Sword.h"
#pragma endregion

#pragma region Yakumo
#include "Yakumo.h"
#include "Yakumo_Weapon.h"
#pragma endregion

#pragma region Monster
#include "St01_BossOliver.h"
#include "St01_Slime.h"
#include "Slave_Devil.h"

#include "MWeapon_Bayonet.h"
#include "MWeapon_GreatSword.h"
#include "MWeapon_Halberd.h"
#include "MWeapon_Hammer.h"
#include "MWeapon_Sword.h"

#include "Slave_Vampire.h"
#include "WolfGhost.h"
#include "WolfGhost_Attack.h"
#include "Projectile_Direct.h"
#include "Projectile_Homing.h"
#include "Projectile_WolfHomingIce.h"
#include "Projectile_Effect.h"
#include "Giant_Vampire.h"
#include "Monkey_Devil.h"
#include "Giant_WhiteDevil.h"
#include "GhostKnight_Halberd.h"
#include "Cinematic_EventShape.h"


#pragma endregion

#pragma region Effect
#include "ParticleSystem.h"
#include "BasicParticle.h"
#include "BasicTrail.h"
#include "Decal_Blood.h"
#include "BasicMesh.h"
#pragma endregion

#pragma region EventShape
#include "Monster_EventShape.h"
#pragma endregion

#pragma region Projectile
#include "Bayonet_Bullet.h"
#include "IndraCoil_Thunder.h"
#include "Homing_Heal.h"
#pragma endregion

#include "UIObj_Ending.h"


_bool Loader::m_bStaticComplete = false;
_bool Loader::m_bEffectComplete = false;
_bool Loader::m_bEffectLoadStart = false;
_bool Loader::m_bLevelCompleteStatic[_UINT(LEVEL::END)] = { false };



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Loader::Loader()
{
}

Client::Loader::Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Client::Loader::~Loader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
_uint APIENTRY ThreadMain(void* pArg)
{
	Loader* ploader = static_cast<Loader*>(pArg);

	CHECK_FAILED(ploader->Loading(), 1);

	return 0;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Initialize(LEVEL _eLevelName)
{
	m_eCreateLevel = _eLevelName;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

//m_pGameInstance->Clear_UIManager();

	return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



HRESULT Client::Loader::Loading()
{
	InitializeCriticalSection(&m_CriticalSection);

	HRESULT hr = CoInitializeEx(nullptr, 0);

	switch (m_eCreateLevel)
	{
	case LEVEL::STATIC:
		if (!m_bEffectLoadStart)
		{
			m_bEffectLoadStart = true;
			hr = Load_Effect();
		}
		else if (!m_bStaticComplete)
			hr = Load_Static();
		break;
	case LEVEL::LOGO:
		hr = Load_Logo();
		break;

	case LEVEL::MAIN:
		hr = Load_Main();
		break;

	case LEVEL::SAMPLE:
		hr = Load_Sample();
		break;

	case LEVEL::BASE:
		hr = Load_Base();
		break;

	case LEVEL::CHURCH:
		hr = Load_Church();
		break;

	case LEVEL::UIDEV:
		hr = Load_Customizing();
		break;

	case LEVEL::CUSTOMIZE:
		hr = Load_Customizing();
		break;
	}

	m_bIsComplete = true;

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
	{
		MSG_ON(L"레벨 로딩 실패!!!", L"Caution");
		return E_FAIL;
	}


	return S_OK;
}



//////////////////////////////////////////////////////// 전역 프로토타입 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Load_Static()
{
	LEVEL level = LEVEL::STATIC;

	_uint iStaticLevel = _uint(LEVEL::STATIC);
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"WolfGhost"),
	//	), E_FAIL);


	COUT("전역 컴포넌트 로딩중");
	/*Main으로 옮김*/
	///* For.Prototype_Component_VIBuffer_Rect */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
	//    VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

	///* For.Prototype_Component_VIBuffer_Rect */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
	//	VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_VICube */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VICube,
		VIBuffer_Cube::Create(m_pDevice, m_pContext)), E_FAIL);

	///* For.Prototype_Component_VISkyDome */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISkyDome,
	//	VIBuffer_Skydome::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_VISkySphere */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISkySphere,
		VIBuffer_SkySphere::Create(m_pDevice, m_pContext, 32, 16, 1.f)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISphere,
		VIBuffer_SkySphere::Create(m_pDevice, m_pContext, 32, 16, 10.f, true)), E_FAIL);

#pragma region UI
	///* For.Prototype_Component_UIRender */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIRender,
	//	UI_Render::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_UI_Button */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIButton,
		UI_Button::Create(m_pDevice, m_pContext)), E_FAIL);


	/* For.Prototype_Component_UI_Progress */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIProgress,
		UI_Progress::Create(m_pDevice, m_pContext)), E_FAIL);

	///* For.Prototype_Component_UI_Progress */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIAnimation,
	//	UI_Animation::Create(m_pDevice, m_pContext)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UISprite,
		UI_Sprite::Create(m_pDevice, m_pContext)), E_FAIL);

	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIText,
	//	UI_Text::Create(m_pDevice, m_pContext)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIEventReactor,
		UI_EventReactor::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_ UI_BitmapFont */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_BitmapText,
		UI_BitmapText::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_ UI_World */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIWorld,
		UI_WorldComponent::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_ UI_Slotgrid */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UISlotGrid,
		UI_SlotGrid::Create(m_pDevice, m_pContext)), E_FAIL);


#pragma endregion

#pragma region Client Component
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Minimap,
		MinimapRenderComponent::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma endregion
	COUT("전역 텍스쳐 로딩중");

	/* For.Prototype_Component_Texture_Logo */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UITexture(L"White"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Textures/White.png", 1)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"Sky_Sphere"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sky/Sky_Sphere_%02d.png", 2)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Rim/T_FX_ExternalNoise08.png", 1)), E_FAIL);

	/* For.Prototype_Component_Texture_PlayerFaceBrow */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceBrow"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Brows")), E_FAIL);

	/* For.Prototype_Component_Texture_PlayerFaceEyelash */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyelash"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Eyelash")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyewhite"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeWhite")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeDetail"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeDetail")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeHighlight"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeHighlight")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFacePaints"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/FacePaints")), E_FAIL);

#pragma region DECAL Texture
	// For Prototype_Component_Texture_Decal
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture("Decal"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_FX_BloodEffect_Color.png", 1)), E_FAIL);

	// For Prototype_Component_Texture_DecalMask
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture("Decal_Mask"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_FX_Blood_Floor_01.png", 1)), E_FAIL);
#pragma endregion

	COUT("전역 쉐이더 로딩중");
	SHADERENTRIES entries;
	SHADERENTRY entrySky[2] = {
		{"VS_MAIN", "PS_MAIN"},
		{"VS_MAIN_SUN", "PS_MAIN_SUN"},
	};
	entries.pEntries = entrySky;
	entries.iNumpass = 2;
	///* For.Prototype_Component_Shader_VTXCube */
	//CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Com_Shader_VTXCube, L"../../Shader/Shader_VtxCube.hlsl", VTXCUBE::Elements, VTXCUBE::iNumElements, entries), E_FAIL);

	///* For.Prototype_Component_Shader_VTXBlendMesh */
	//CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"VtxBlendMesh"), L"../../Shader/Shader_VtxBlendMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries), E_FAIL);

	/* For.Prototype_Component_Shader_SkyDome */
	CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Com_Shader_Sky, L"../../Shader/Shader_SkySphere.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries), E_FAIL);


	SHADERENTRY entryAnim[14] = {
		{"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
		{"VS_MAIN", "PS_MAIN_SHADOW"},   // Pass 1 : 그림자 그리기
		{"VS_MAIN", "PS_MAIN" },         // Pass 2 : 쉐이더 파싱 테스트용
		{"VS_MAIN_NORMAL", "PS_MAIN_NORMAL" },  // pass 3 : 노말 그리기
		{"VS_MAIN_SHADOW_CASCADE", "PS_MAIN_CASCADE_SHADOW" }, // pass 4 : 케스케이드 그림자 그리기
		{"VS_MAIN_NORMAL", "PS_MAIN_SAVEPOINT" }, // pass 5 : 세이브포인트용(emission, opacity 야매로 구성)
		{ "VS_MAIN_NORMAL", "PS_CHARACTER_EYE" },  // Pass 6 : 눈동자 전용 패스
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR" },  // Pass 7 : 몬스터 PBR 패스전용
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE" },  // Pass 8 : 몬스터 PBR 디졸브 패스전용
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_CV" },       // Pass 9 : GhostWolf용
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE_MONSTER" },  // Pass 10 : 눈깔빛나는 몬스터들 패스
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTVAMPIRE" },  // Pass 11 : 자이언트뱀파이어용 PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTWHITEDEVIL
		{ "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTWHITEDEVIL" },  // Pass 12 : 자이언트화이트데빌용
		{ "VS_MAIN_NORMAL", "PS_NPC_HAIR" }  // Pass 13 : NPC머리용(현재올리버만)
	};
	entries.pEntries = entryAnim;
	entries.iNumpass = sizeof(entryAnim) / sizeof(SHADERENTRY);

	///* For.Prototype_Component_Shader_VTXNorTex */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXNorTex,
	//	Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements, entries)), E_FAIL);

	/* For.Prototype_Component_Shader_VTXAnimMesh */
	CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Com_Shader_VTXAnimMesh, L"../../Shader/Shader_AnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries), E_FAIL);

	SHADERENTRY entryAlpha[11] = {
	   {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
	   {"VS_MAIN", "PS_MAIN_SHADOW"},  // Pass 1 : 그림자 그리기
	   {"VS_MAIN", "PS_MAIN_ALPHA"},    // Pass 2 : 알파 들어간거 그리기
	   {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL"}, // pass 3 : 노말 들어간거 그리기
	   {"VS_MAIN", "PS_MAIN_OPACITY"}, // pass 4 : Opacity 들어간거 그리기
	   {"VS_MAIN_SHADOW_CASCADE", "PS_MAIN_SHADOW_CASCADE"}, // pass 5 : Cascade 그림자 그리기
	   {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR"}, // pass 6 : 노말들어간 PBR
	   {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_MAP"}, // pass 7 : 노말들어간 PBR 맵전용 
	   {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_GODRAY"},  // pass 8 : 갓레이용
	   {"VS_MAIN", "PS_MAIN_GODRAYMESH"},  // pass 9 : 갓레이메쉬용
	   {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE"},  // pass 10 : 디졸브용
	};
	entries.pEntries = entryAlpha;
	entries.iNumpass = sizeof(entryAlpha) / sizeof(SHADERENTRY);

	/* For.Prototype_Component_Shader_VTXMesh */
	CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Com_Shader_VTXMesh, L"../../Shader/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries), E_FAIL);

#pragma region EFFECT
	SHADERENTRY entryParticleRect[3] = {
		{"VS_MAIN", "PS_MAIN"},
		{"VS_MAIN", "PS_SPRITE"},
		{"VS_MAIN", "PS_SCENE_DISTORTION"}
	};
	entries.pEntries = entryParticleRect;
	entries.iNumpass = 3;

	/* For.Prototype_Component_Shader_Particle_Rect */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Particle_Rect,
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex_Particle.hlsl", VTXPOSTEX_PARTICLE::Elements, VTXPOSTEX_PARTICLE::iNumElements, entries)), E_FAIL);

	/* For.Prototype_Component_Shader_Compute_Particle */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Compute_Particle,
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_SPREAD")), E_FAIL);

	/* For.Prototype_Component_Shader_Compute_Particle_Converge */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Compute_Particle_Converge,
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_CONVERGE")), E_FAIL);

	/* For.Prototype_Component_Shader_Compute_Particle_Dust */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Compute_Particle_Dust,
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_DUST")), E_FAIL);

	SHADERENTRY entryTrail[4] = {
	{"VS_MAIN", "PS_MAIN"},                 // Pass 0 : 텍스처 + 컬러 틴트 + 알파 페이드
	{"VS_MAIN", "PS_DISTORTION"},           // Pass 1 : 텍스처 + 왜곡 + 컬러 틴트 + 알파 페이드
	{"VS_MAIN", "PS_SCENE_DISTORTION"},     // Pass 2 : 장면 왜곡
	{"VS_MAIN", "PS_SOLID"}                 // Pass 3 : 솔리드 컬러 (텍스처 없이, 디버그)
	};
	entries.pEntries = entryTrail;
	entries.iNumpass = 4; 

	/* For.Prototype_Component_Shader_Trail */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Trail,
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Trail.hlsl", VTXTRAIL::Elements, VTXTRAIL::iNumElements, entries)), E_FAIL);

	// Decal 
	SHADERENTRY entryDecal[1] = {
	{"VS_MAIN", "PS_MAIN"}
	};
	entries.pEntries = entryDecal;
	entries.iNumpass = 1;

	/* For.Prototype_Component_Shader_Decal */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Decal,
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Decal.hlsl", VTXTRAIL::Elements, VTXTRAIL::iNumElements, entries)), E_FAIL);

	SHADERENTRY entryMeshEffect[4] = {
	{"VS_MAIN", "PS_MAIN"},
	{"VS_MAIN", "PS_SPRITE"},
	{"VS_MAIN", "PS_SCENE_DISTORTION"},
	{"VS_FIRE", "PS_FIRE"}
	};
	entries.pEntries = entryMeshEffect;
	entries.iNumpass = 4;

	/* For.Prototype_Component_Shader_MeshEffect */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_MeshEffect",
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_MeshEffect.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries)), E_FAIL);
#pragma endregion


	//SHADERENTRY poscolorentry[1] = {
	//	{"VS_MAIN", "PS_MAIN"}
	//};
	//SHADERENTRIES poscolorentries;
	//poscolorentries.pEntries = poscolorentry;
	//poscolorentries.iNumpass = 1;
	///* For.Prototype_Component_Shader_VTXPoscolor */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VtxPosColor",
	//	Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosColor.hlsl", VTXPOSCOR::Elements, VTXPOSCOR::iNumElements, poscolorentries)), E_FAIL);

	SHADERENTRY entryPlayerAnim[10] = {
		{"VS_MAIN", "PS_MAIN"},										// Pass 0 : 일반 그리기
		{"VS_MAIN_SHADOW", "PS_MAIN_SHADOW"},						// Pass 1 : 그림자 그리기
		{"VS_MAIN_NORMAL","PS_MAIN_NORMAL_PBR"},					// Pass 2 : PBR기본 패스
		{"VS_MAIN_NORMAL", "PS_CLOTH"},								// Pass 3 : 옷 전용 패스
		{"VS_MAIN_NORMAL","PS_HAIR"},								// Pass 4 : 헤어 전용 패스
		{"VS_MAIN_NORMAL","PS_FACE"},								// Pass 5 : 얼굴 전용 패스 : 그지같다진짜로
		{"VS_MAIN_NORMAL","PS_EYE"},								// Pass 6 : 눈 전용 패스 
		{ "VS_MAIN_NORMAL","PS_FACE_BACKUP" },						// Pass 7 : 임시패스
		{ "VS_MAIN_NORMAL","PS_MAIN_NORMAL_PBR_RIM" },				// Pass 8 : 림들어가는애들 패스
		{ "VS_MAIN_NORMAL","PS_MAIN_NORMAL_PBR_RIM_BLOODWEAPON" }	// Pass 9 : 림들어가는애들 패스(BloodWeapon 전용, Dissolve 분리해야해서 따로 팜)
	};
	entries.pEntries = entryPlayerAnim;
	entries.iNumpass = 10;

	CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"VTXPlayerAnimMesh"), L"../../Shader/Shader_PlayerAnimMesh.hlsl", VTXPLAYERANIMMESH::Elements, VTXPLAYERANIMMESH::iNumElements, entries), E_FAIL);

	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh",
	//	Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_PlayerAnimMesh.hlsl", VTXPLAYERANIMMESH::Elements, VTXPLAYERANIMMESH::iNumElements, entries)), E_FAIL);

	/////////////////////////////////Minimap/////////////////////////////////////////
	SHADERENTRY entryMinimap[] = {
	{"VS_DEBUG","PS_DEBUG"},
	{"VS_STAMP", "PS_STAMP"},
	{"VS_TERRAIN", "PS_TERRAIN"},
	{"VS_STAMP", "PS_Arrow"}
	};
	entries.pEntries = entryMinimap;
	entries.iNumpass = sizeof(entryMinimap) / sizeof(SHADERENTRY);

	CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"VTXMinimap"), L"../../Shader/Shader_Minimap.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries), E_FAIL);

	/////////////////////////////////////////////////////////////////////////

	/* 모델 애니메이션용 컴쉐 */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Bone", //Combined 계산
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_BONECOMBINED")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_LocalMatrix", //LocalMatrix 계산
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_EVALUATELOCAL")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Readback", //없어질 예정
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_READBACK")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_DrivenBones", //런타임에 SRT를 원하는 뼈에 적용
		ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_APPLYDRIVENBONES")), E_FAIL);

	/* 여기까지 */

	COUT("전역 폰트 로딩중");
	// CHECK_FAILED(m_pGameInstance->LoadSound("Click", L"../Resources/Sounds/Click.wav"), E_FAIL);

	COUT("전역 오브젝트 로딩중");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_MonsterStat,
		Monster_Stat::Create(m_pDevice, m_pContext)), E_FAIL);

	LIGHT_DESC Desc = {};
	Desc.eType = LIGHT::POINT;
	Desc.fRange = 5.f;
	Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.vDiffuse = _float4(1.f, 1.f, 7.f, 1.f);
	Desc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 0.0f);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Proto_Component_Point_Light", LightComponent::Create(m_pDevice, m_pContext, Desc)), E_FAIL);

#pragma region UI
	/*CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_UIObject",
		UIObject::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);*/

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC),Proto_UIObject_PLAYERHUD,
		UIObj_PlayerHUD::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_PROGRESSBAR,
		UIObj_ProgressBar::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_GUARDBAR,
		UIObj_GuardBar::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_STAMINABAR,
		UIObj_Stamina::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_BITMAPTEXT,
		UIObj_BitmapFont::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SLOTGRID,
		UIObj_SlotGrid::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SLOT,
		UIObj_Slot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_QUICKSLOT,
		UIObj_QuickSlot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHORTCUTQUICKSLOT,
		UIObj_ShortCutQuickSlot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	//로딩때문에 main으로옮김
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_TEXT,
	//	UIObj_Text::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	////Windows////
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_INVENTORYWINDOW,
		UIObj_Window_Inventory::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_PLAYERMENUWINDOW,
		UIObj_Window_PlayerMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SUBWINDOW,
		UIObj_SubWindow::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SUBWINDOW_BLOODCODE,
		UIObj_Window_Bloodcode::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_TitleWindow,
		UIObj_Window_Title::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	////Descs
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SUBDESC,
		UIObj_SubDesc::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHORTCUTMENU,
		UIObj_ShortCutMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_PopUp_ItemGet,
		UIObj_PopUp_ItemGet::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
	

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_PopUp_Interaction,
		UIObj_PopUp_Interaction::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);



	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_BossName,
		UIObj_BossName::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_BOSSDEAD,
		UIObj_BossDead::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_HPBAR,
		UIObj_HpBar::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_BlurUI,
		UIObj_Blur::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_MINIMAP,
		UIObj_Minimap::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_MonsterStatus,
		UIObj_MonsterStatus::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	//glowbutton,select
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_GlowButton,
		UIObj_GlowButton::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_Selector,
		UIObj_Selector::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SceneSelectMenu,
		UIObj_SceneSelectMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SceneSlot,
		UIObj_SceneSlot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_Haze,
		UIObj_Haze::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SavePoint_Slot,
		UIObj_SavePoint_Slot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_ManaInfo,
		UIObj_ManaInfo::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_PopUp_MapTitle,
		UIObj_PopUp_MapTitle::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_NpcDialogue,
		UIObj_NpcDialogue::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_FocusMenu_NPC,
		UIObj_Focus_NPCMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_NPCMenu,
		UIObj_Npc_ChoiceMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	//////////////////Shop//////////////////////////

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_WINDOW_SHOP,
		UIObj_Window_Shop::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_CATEGORY,
		UIObj_ShopCategory::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_SLOTGRID,
		UIObj_ShopSlotGrid::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_SLOT,
		UIObj_ShopSlot::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_DESC_WINDOW,
		UIObj_Window_ShopDesc::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_DESC_SUBINFO,
		UIObj_Window_ShopSubDesc::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_SHOP_NEEDINFO,
		UIObj_Window_NeedInfo::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_LevelUpWindow,
		UIObj_Window_LevelUp::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

#pragma endregion
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject_SkySphere,
		Sky_Sphere::Create(m_pDevice, m_pContext, LEVEL::STATIC)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject_SphereSun,
		GodRaySun::Create(m_pDevice, m_pContext, LEVEL::STATIC)), E_FAIL);
#pragma region Player

	COUT("플레이어 전역 로딩 중");
	Load_Player();
	Load_Yakumo();
#pragma endregion

#pragma region EFFECT MODELS

	_matrix prematrix = XMMatrixIdentity();
	prematrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	_wstring folderPath = L"../../Resources/Model/Effect/";
	m_pGameInstance->Load_Folder(folderPath, _UINT(level), prematrix, L".siho");

#pragma endregion
		

#pragma region Sound
	//m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/경로/경로.wav");

	m_pGameInstance->LoadSound("SavePoint_Idle_Roop", L"../../Resources/Sounds/Gimmik_Object/SavePoint_Idle_Roop.ogg");
	m_pGameInstance->LoadSound("Battle_Boss_BGM", L"../../Resources/Sounds/BGM/Battle_Boss_BGM.wav");
	m_pGameInstance->LoadSound("UnderPassAmb", L"../../Resources/Sounds/Ambient/UnderPassAmb.wav");
	m_pGameInstance->LoadSound("BaseAmb", L"../../Resources/Sounds/Ambient/BaseAmb.wav");

	m_pGameInstance->LoadSound("Hammer_Krush", L"../../Resources/Sounds/Weapon_Sound/Hammer/Hammer_Krush_Iron.wav");
	m_pGameInstance->LoadSound("Swing_02", L"../../Resources/Sounds/Weapon_Sound/Hammer/Swing_02.ogg");

	m_pGameInstance->LoadSound("Jump", L"../../Resources/Sounds/Monster/Oliver/Jump.ogg");
	m_pGameInstance->LoadSound("Boss_Title", L"../../Resources/Sounds/System_Sound/Boss_Title.wav");
	m_pGameInstance->LoadSound("Ladder_Down_Fast", L"../../Resources/Sounds/Player/Ladder_Down_Fast.wav");
	m_pGameInstance->LoadSound("Hit_Bayonet", L"../../Resources/Sounds/Weapon_Sound/Bayonet/Hit_Bayonet.wav");
	// 사운드 랜덤 기능은 차차 ㅅ애각
	m_pGameInstance->LoadSound("Attack_Bark", L"../../Resources/Sounds/Monster/Oliver/Attack_Bark_1.wav");
	m_pGameInstance->LoadSound("Attack_Bark_2", L"../../Resources/Sounds/Monster/Oliver/Attack_Bark_2.wav");
	m_pGameInstance->LoadSound("Attack_Bark_3", L"../../Resources/Sounds/Monster/Oliver/Attack_Bark_3.wav");
	m_pGameInstance->LoadSound("Attack_Bark_4", L"../../Resources/Sounds/Monster/Oliver/Attack_Bark_4.wav");
	m_pGameInstance->LoadSound("Attack_Bark_5", L"../../Resources/Sounds/Monster/Oliver/Attack_Bark_5.wav");

	m_pGameInstance->LoadSound("Ketsugi_Chage", L"../../Resources/Sounds/Monster/Oliver/Ketsugi_Chage.wav");
	m_pGameInstance->LoadSound("Ketsugi_Fire", L"../../Resources/Sounds/Monster/Oliver/Ketsugi_Fire.wav");

	m_pGameInstance->LoadSound("OLIVER_LOST_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/Oliver/OLIVER_LOST_KETSUGI_00_Play (864343360).wav");
	m_pGameInstance->LoadSound_AddGroup("OLIVER_LOST_MOVE_00_Play (506222156)", "OLIVER_LOST_MOVE_00_Play", L"../../Resources/Sounds/Monster/Oliver/OLIVER_LOST_MOVE_00_Play (506222156).wav");
	m_pGameInstance->LoadSound_AddGroup("OLIVER_LOST_MOVE_00_Play (977748899)", "OLIVER_LOST_MOVE_00_Play", L"../../Resources/Sounds/Monster/Oliver/OLIVER_LOST_MOVE_00_Play (977748899).wav");

#pragma endregion

#pragma region Sound_Player
	//공용 사운드
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Player_Move_00");
	m_pGameInstance->LoadSound("Player_Jump", L"../../Resources/Sounds/Player/Player_Jump/SE_PLAYER_JUMP003 (1025351182).wav");
	m_pGameInstance->LoadSound("Player_Jump_1", L"../../Resources/Sounds/Player/Player_Jump/SE_PLAYER_JUMP004 (257635268).wav");
	//구르기 사운드
	m_pGameInstance->LoadSound("Player_Rolling", L"../../Resources/Sounds/Player/Player_Rolling/PLAYER_ROLLING_00_Play (912639894).wav");
	//구르기 Plus 버전
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_MOVING_SPEED_UP_00_Play (106004373)", "Renketsu_Moving", L"../../Resources/Sounds/Player/Renketsu_Moving/RENKETSU_MOVING_SPEED_UP_00_Play (106004373).wav");
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_MOVING_SPEED_UP_00_Play (472144820)", "Renketsu_Moving", L"../../Resources/Sounds/Player/Renketsu_Moving/RENKETSU_MOVING_SPEED_UP_00_Play (472144820).wav");
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_MOVING_SPEED_UP_00_Play (789455192)", "Renketsu_Moving", L"../../Resources/Sounds/Player/Renketsu_Moving/RENKETSU_MOVING_SPEED_UP_00_Play (789455192).wav");
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_MOVING_SPEED_UP_00_Play (879409305)", "Renketsu_Moving", L"../../Resources/Sounds/Player/Renketsu_Moving/RENKETSU_MOVING_SPEED_UP_00_Play (879409305).wav");
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_MOVING_SPEED_UP_00_Play (1072707946)", "Renketsu_Moving", L"../../Resources/Sounds/Player/Renketsu_Moving/RENKETSU_MOVING_SPEED_UP_00_Play (1072707946).wav");
	//가드
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Guard_Break");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Guard_Hit");
	//공용 Grip
	m_pGameInstance->LoadSound_AddGroup("Common_Grip/Grip_00_Play (255814455)", "Common_Grip", L"../../Resources/Sounds/Player/Common_Grip/Grip_00_Play (255814455).wav");
	m_pGameInstance->LoadSound_AddGroup("Common_Grip/Grip_00_Play (899183684)", "Common_Grip", L"../../Resources/Sounds/Player/Common_Grip/Grip_00_Play (899183684).wav");
	//차징 사운드
	m_pGameInstance->LoadSound_AddGroup("CHAGE_SET_00_Play (692353922)", "Charge_Set", L"../../Resources/Sounds/Player/Charge_Set/CHAGE_SET_00_Play (692353922).wav");
	m_pGameInstance->LoadSound_AddGroup("CHAGE_SET_00_Play (936178323)", "Charge_Set", L"../../Resources/Sounds/Player/Charge_Set/CHAGE_SET_00_Play (936178323).wav");
	//스페셜 웨폰?(특수 사운드느낌) / 현재 롱소드,해머 강공격에 사용중
	m_pGameInstance->LoadSound("THammer_AttackStrong10B_Move_000", L"../../Resources/Sounds/Player/Special_Weapon/SE_THammer_AttackStrong10B_Move_000_Play (557141981).wav");
	m_pGameInstance->LoadSound("Sword_AttackStrong04B_SWING", L"../../Resources/Sounds/Player/Special_Weapon/SE_Sword_AttackStrong04B_SWING_Play (475502008).wav");
	m_pGameInstance->LoadSound("TLSword_AttackStrong06B_Swing", L"../../Resources/Sounds/Player/Special_Weapon/SE_TLSword_AttackStrong06B_Swing_000_Play (942877607).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Sword_TLSword_AttackStrong04B_Impact_000_Play (598162180)", "TLSword_AttackStrong04B_Impact_000", L"../../Resources/Sounds/Player/Special_Weapon/SE_Sword_TLSword_AttackStrong04B_Impact_000_Play (598162180).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Sword_TLSword_AttackStrong04B_Impact_000_Play (649070194)", "TLSword_AttackStrong04B_Impact_000", L"../../Resources/Sounds/Player/Special_Weapon/SE_Sword_TLSword_AttackStrong04B_Impact_000_Play (649070194).wav");
	//웨폰 체인지
	m_pGameInstance->LoadSound("Play_SE_KINU_002", L"../../Resources/Sounds/Player/WeaponChange/Play_SE_KINU_002 (614246692).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_SE_WEAPON_CHANGE_000 (317559364)", "Weapon_Change", L"../../Resources/Sounds/Player/WeaponChange/Play_SE_WEAPON_CHANGE_000 (317559364).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_SE_WEAPON_CHANGE_000 (464827501)", "Weapon_Change", L"../../Resources/Sounds/Player/WeaponChange/Play_SE_WEAPON_CHANGE_000 (464827501).wav");
	//아이템 사용
	m_pGameInstance->LoadSound("ITEM_USE_000", L"../../Resources/Sounds/Player/UseItem/SE_ITEM_USE_000_Play (941780097).wav");
	m_pGameInstance->LoadSound("ITEM_USE_001", L"../../Resources/Sounds/Player/UseItem/SE_ITEM_USE_001_Play (844243759).wav");
	m_pGameInstance->LoadSound("ITEM_USE_002", L"../../Resources/Sounds/Player/UseItem/SE_ITEM_USE_002_Play (479989756).wav");
	m_pGameInstance->LoadSound("ITEM_USE_003", L"../../Resources/Sounds/Player/UseItem/SE_ITEM_USE_003_Play (703993430).wav");
	m_pGameInstance->LoadSound("RENKETSU_SHOOT_START_00", L"../../Resources/Sounds/Player/UseItem/RENKETSU_SHOOT_START_00_Play (702836776).wav");
	//회복
	m_pGameInstance->LoadSound_AddGroup("PLAYER_RECOVERY_00_Play (470327749)", "PLAYER_RECOVERY_00", L"../../Resources/Sounds/Player/UseItem/PLAYER_RECOVERY_00_Play (470327749).wav");
	m_pGameInstance->LoadSound_AddGroup("PLAYER_RECOVERY_00_Play (839878244)", "PLAYER_RECOVERY_00", L"../../Resources/Sounds/Player/UseItem/PLAYER_RECOVERY_00_Play (839878244).wav");
	//데미지,죽음
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Damage01");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Damage03");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/DamageLaunch");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/DownDeath");
	//흡혈아장(Drape)
	m_pGameInstance->LoadSound("SP_BLOOD_ACT_02MASK_SHORT_Play", L"../../Resources/Sounds/Player/Drape/SP_BLOOD_ACT_02MASK_SHORT_Play (504438221).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_01START_Play", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_01START_Play (301825753).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_01START_SHORT", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_01START_SHORT_Play (399775658).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_03HIT_SHORT", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_03HIT_SHORT_Play (567384341).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_04FX_SHORT", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_04FX_SHORT_Play (742022689).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_05FINISH_SHORT", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_05FINISH_SHORT_Play (297351383).wav");
	m_pGameInstance->LoadSound("SP_DRAPE_ACT_02BARK_SHORT", L"../../Resources/Sounds/Player/Drape/SP_DRAPE_ACT_02BARK_SHORT_Play (860846107).wav");
	//뒤잡(BackStab)
	m_pGameInstance->LoadSound_AddGroup("PLAYER_BACK_STAB_00_Play (137393409)", "BackStab", L"../../Resources/Sounds/Player/BackStab/PLAYER_BACK_STAB_00_Play (137393409).wav");
	m_pGameInstance->LoadSound_AddGroup("PLAYER_BACK_STAB_00_Play (184989451)", "BackStab", L"../../Resources/Sounds/Player/BackStab/PLAYER_BACK_STAB_00_Play (184989451).wav");
	m_pGameInstance->LoadSound_AddGroup("PLAYER_BACK_STAB_UNEXPLODED_00_Play (669182811)", "BackStab", L"../../Resources/Sounds/Player/BackStab/PLAYER_BACK_STAB_UNEXPLODED_00_Play (669182811).wav");
	//체크포인트
	m_pGameInstance->LoadSound("PLAYER_WARP_START_00", L"../../Resources/Sounds/Player/CheckPoint/PLAYER_WARP_START_00_Play (1051445690).wav");
	m_pGameInstance->LoadSound("PLAYER_WARP_OUT_00", L"../../Resources/Sounds/Player/CheckPoint/PLAYER_WARP_OUT_00_Play (400349490).wav");
	//m_pGameInstance->LoadSound_AddGroup("PLAYER_WARP_START_00_Play (724298741)", "PLAYER_WARP_START_00", L"../../Resources/Sounds/Player/CheckPoint/PLAYER_WARP_START_00_Play (724298741).wav");
	//m_pGameInstance->LoadSound_AddGroup("PLAYER_WARP_START_00_Play (1051445690)", "PLAYER_WARP_START_00", L"../../Resources/Sounds/Player/CheckPoint/PLAYER_WARP_START_00_Play (316791396).wav");
	//아이템 줍기
	m_pGameInstance->LoadSound("Play_SE_KINU_001", L"../../Resources/Sounds/Player/UseItem/Play_SE_KINU_001 (257635268).wav");
	//사다리
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Ladder");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/LadderFoot");
	//스킬
	//어썰트
	m_pGameInstance->LoadSound("ACTION_RENKETSU_SWING_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_SWING_01_Play (1066288558).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_01_Play (35996116)", "ACTION_RENKETSU_ATTACK_START_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_01_Play (35996116).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_01_Play (128858528)", "ACTION_RENKETSU_ATTACK_START_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_01_Play (128858528).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_01_Play (658292661)", "ACTION_RENKETSU_ATTACK_START_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_01_Play (658292661).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_01_Play (750871306)", "ACTION_RENKETSU_ATTACK_START_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_01_Play (750871306).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_JUMP_01_Play (871772103)", "ACTION_RENKETSU_JUMP_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_JUMP_01_Play (871772103).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_JUMP_01_Play (1025351182)", "ACTION_RENKETSU_JUMP_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_JUMP_01_Play (1025351182).wav");
	m_pGameInstance->LoadSound("ACTION_RENKETSU_ATTACK_IMPACT_07", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_IMPACT_07_Play (530569340).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_06_Play (310399942)", "ACTION_RENKETSU_ATTACK_IMPACT_06", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_IMPACT_06_Play (310399942).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_06_Play (920650146)", "ACTION_RENKETSU_ATTACK_IMPACT_06", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_IMPACT_06_Play (920650146).wav");
	m_pGameInstance->LoadSound("ACTION_RENKETSU_SWORD_SWING_00", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_SWORD_SWING_00_Play (911338993).wav");
	//강룡
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_00_Play (4514371)", "ACTION_RENKETSU_ATTACK_START_00", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_00_Play (4514371).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_00_Play (43343830)", "ACTION_RENKETSU_ATTACK_START_00", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_00_Play (43343830).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_00_Play (89275775)", "ACTION_RENKETSU_ATTACK_START_00", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_00_Play (89275775).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_START_00_Play (290939535)", "ACTION_RENKETSU_ATTACK_START_00", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_ATTACK_START_00_Play (290939535).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_SWORD_SCRUB_01_Play (203839620)", "ACTION_RENKETSU_SWORD_SCRUB_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_SWORD_SCRUB_01_Play (203839620).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_SWORD_SCRUB_01_Play (256603654)", "ACTION_RENKETSU_SWORD_SCRUB_01", L"../../Resources/Sounds/Player/Renketsu/ACTION_RENKETSU_SWORD_SCRUB_01_Play (256603654).wav");
	m_pGameInstance->LoadSound("SE_PLAYER_DODGE000", L"../../Resources/Sounds/Player/Renketsu/SE_PLAYER_DODGE000 (640448017).wav");
	m_pGameInstance->LoadSound("ActionRenketsu_AttackStrong06_01", L"../../Resources/Sounds/Player/Renketsu/ActionRenketsu_AttackStrong06_01_Play (473982282).wav");
	//인드라코일
	m_pGameInstance->LoadSound("SE_Renketsu_IndraCoil_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_IndraCoil_000_Play (438664699).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_IndraCoil_001", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_IndraCoil_001_Play (1000668137).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_IndraCoil_002", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_IndraCoil_002_Play (657747396).wav");
	m_pGameInstance->LoadSound("Play_SE_KETSUGI000", L"../../Resources/Sounds/Player/Renketsu/Play_SE_KETSUGI000 (351782496).wav");
	m_pGameInstance->LoadSound("RENKETSU_SHOOT_BLOOD_START_00", L"../../Resources/Sounds/Player/Renketsu/RENKETSU_SHOOT_BLOOD_START_00_Play (148891639).wav");
	m_pGameInstance->LoadSound("RENKETSU_SHOOT_BLOOD_START_01", L"../../Resources/Sounds/Player/Renketsu/RENKETSU_SHOOT_BLOOD_START_01_Play (817093464).wav");
	m_pGameInstance->LoadSound("KETUGI_ATTACK_IMPACT_00", L"../../Resources/Sounds/Player/Renketsu/KETUGI_ATTACK_IMPACT_00_Play (157246829).wav");
	m_pGameInstance->LoadSound("KETUGI_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Renketsu/KETUGI_ATTACK_IMPACT_01_Play (702754007).wav");
	//레일건 
	m_pGameInstance->LoadSound("Action_Renketsu_Railgun_00", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_00_Play (373317157).wav");
	m_pGameInstance->LoadSound("Action_Renketsu_Railgun_01", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_01_Play (1034667076).wav");
	m_pGameInstance->LoadSound("Action_Renketsu_Railgun_Charge_00", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_Charge_00_Play (650551003).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_Railgun_02_Play (776798933)", "Action_Renketsu_Railgun_02", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_02_Play (776798933).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_Railgun_02_Play (221670522)", "Action_Renketsu_Railgun_02", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_02_Play (221670522).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_Railgun_02_Play (615475186)", "Action_Renketsu_Railgun_02", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_02_Play (615475186).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_Railgun_Hit_00_Play (634694194)", "Action_Renketsu_Railgun_Hit_00", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_Hit_00_Play (634694194).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_Railgun_Hit_00_Play (66557307)", "Action_Renketsu_Railgun_Hit_00", L"../../Resources/Sounds/Player/Railgun/Action_Renketsu_Railgun_Hit_00_Play (66557307).wav");
	//로얄하트
	m_pGameInstance->LoadSound("SE_Renketsu_RoyalHeart_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_RoyalHeart_000_Play (867833098).wav");
	//스트라이크 라이저
	m_pGameInstance->LoadSound("SE_STRIKE_RISER_003", L"../../Resources/Sounds/Player/Renketsu/SE_STRIKE_RISER_003_Play (666034078).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_STRIKE_RISER_001_Play (599616515)", "SE_STRIKE_RISER_001", L"../../Resources/Sounds/Player/Renketsu/SE_STRIKE_RISER_001_Play (599616515).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_STRIKE_RISER_001_Play (1035636344)", "SE_STRIKE_RISER_001", L"../../Resources/Sounds/Player/Renketsu/SE_STRIKE_RISER_001_Play (1035636344).wav");
	//인첸트(불)
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_BloodWeapon_000_Play (359521257)", "SE_Renketsu_BloodWeapon_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_BloodWeapon_000_Play (359521257).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_BloodWeapon_000_Play (800919507)", "SE_Renketsu_BloodWeapon_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_BloodWeapon_000_Play (800919507).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_BloodWeapon_000_Play (981150807)", "SE_Renketsu_BloodWeapon_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_BloodWeapon_000_Play (981150807).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_FlameWeapon_000", L"../../Resources/Sounds/Player/Renketsu/SE_Renketsu_FlameWeapon_000_Play (184004031).wav");
	//염검전무
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_SummonBlade_00_Play (49491416)", "Action_Renketsu_SummonBlade_00", L"../../Resources/Sounds/Player/Renketsu/Action_Renketsu_SummonBlade_00_Play (49491416).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_SummonBlade_00_Play (562810354)", "Action_Renketsu_SummonBlade_00", L"../../Resources/Sounds/Player/Renketsu/Action_Renketsu_SummonBlade_00_Play (562810354).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_SummonBlade_00_Play (655469590)", "Action_Renketsu_SummonBlade_00", L"../../Resources/Sounds/Player/Renketsu/Action_Renketsu_SummonBlade_00_Play (655469590).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_SummonBlade_00_Play (1029685309)", "Action_Renketsu_SummonBlade_00", L"../../Resources/Sounds/Player/Renketsu/Action_Renketsu_SummonBlade_00_Play (1029685309).wav");
	m_pGameInstance->LoadSound("Action_Renketsu_SummonBlade_Hit_00", L"../../Resources/Sounds/Player/Renketsu/Action_Renketsu_SummonBlade_Hit_00_Play (468978622).wav");

	//바요네트
	m_pGameInstance->LoadSound("Bayonet_Catch", L"../../Resources/Sounds/Player/Bayonet/Play_SE_BAYONET_CATCH (317559364).wav");
	m_pGameInstance->LoadSound_AddGroup("Bayonet_Grip_00_Play (227077081)", "Bayonet_Grip", L"../../Resources/Sounds/Player/Bayonet/Bayonet_Grip_00_Play (227077081).wav");
	m_pGameInstance->LoadSound_AddGroup("Bayonet_Grip_00_Play (2694160)", "Bayonet_Grip", L"../../Resources/Sounds/Player/Bayonet/Bayonet_Grip_00_Play (2694160).wav");

	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (176714771)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (176714771).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (496768895)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (496768895).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (582242725)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (582242725).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (618518820)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (618518820).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (683115321)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (683115321).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_SWING (986971179)", "Bayonet_Swing", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_SWING (986971179).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_BAYONET_ATTACK (807836299)", "Bayonet_Attack", L"../../Resources/Sounds/Player/Bayonet/Play_BAYONET_ATTACK (807836299).wav");
	m_pGameInstance->LoadSound_AddGroup("BAYONET_FIRE_01_Play (440669387)", "Bayonet_Attack", L"../../Resources/Sounds/Player/Bayonet/BAYONET_FIRE_01_Play (440669387).wav");
	//소드
	m_pGameInstance->LoadSound_AddGroup("Long_Sword_Grip_00_Play (227077081)", "Sword_Grip", L"../../Resources/Sounds/Player/Sword/Long_Sword_Grip_00_Play (227077081).wav");
	m_pGameInstance->LoadSound_AddGroup("Long_Sword_Grip_00_Play (2694160)", "Sword_Grip", L"../../Resources/Sounds/Player/Sword/Long_Sword_Grip_00_Play (2694160).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD000 (72873868)", "Sword_Swing_0", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD000 (72873868).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD000 (911338993)", "Sword_Swing_0", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD000 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD001 (877857987)", "Sword_Swing_1", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD001 (877857987).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD001 (911338993)", "Sword_Swing_1", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD001 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD002 (326049898)", "Sword_Swing_2", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD002 (326049898).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD002 (911338993)", "Sword_Swing_2", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD002 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD003 (760827491)", "Sword_Swing_3", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD003 (760827491).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD003 (911338993)", "Sword_Swing_3", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD003 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD004 (417169913)", "Sword_Swing_4", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD004 (417169913).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD004 (911338993)", "Sword_Swing_4", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD004 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD005 (17290216)", "Sword_Swing_5", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD005 (17290216).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_LONG_SWORD005 (911338993)", "Sword_Swing_5", L"../../Resources/Sounds/Player/Sword/SE_LONG_SWORD005 (911338993).wav");
	m_pGameInstance->LoadSound_AddGroup("PLAYER_SWORD_GUARD_HIT_Play (17072689)", "Sword_Guard_Hit", L"../../Resources/Sounds/Player/Player_Sword_Guard_Hit_Play/PLAYER_SWORD_GUARD_HIT_Play (17072689).wav");
	m_pGameInstance->LoadSound_AddGroup("PLAYER_SWORD_GUARD_HIT_Play (794585465)", "Sword_Guard_Hit", L"../../Resources/Sounds/Player/Player_Sword_Guard_Hit_Play/PLAYER_SWORD_GUARD_HIT_Play (794585465).wav");
	//롱소드(GreatSword)
	m_pGameInstance->LoadSound_AddGroup("Large_Sword_Grip_00_Play (2694160)", "GreatSword_Grip", L"../../Resources/Sounds/Player/GreatSword/Large_Sword_Grip_00_Play (2694160).wav");
	m_pGameInstance->LoadSound_AddGroup("Large_Sword_Grip_00_Play (227077081)", "GreatSword_Grip", L"../../Resources/Sounds/Player/GreatSword/Large_Sword_Grip_00_Play (227077081).wav");
	m_pGameInstance->LoadSound("GreatSword_Catch", L"../../Resources/Sounds/Player/GreatSword/LARGE_SWORD_CATCH_00_Play (887998188).wav");
	m_pGameInstance->LoadSound("GreatSword_Shoulder", L"../../Resources/Sounds/Player/GreatSword/LARGE_SWORD_SHOULDER_ON_00_Play (929220464).wav");

	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD000 (521919710)", "GreatSword_Swing_0", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD000 (521919710).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD000 (747746587)", "GreatSword_Swing_0", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD000 (747746587).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD000 (964913328)", "GreatSword_Swing_0", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD000 (964913328).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD001 (747746587)", "GreatSword_Swing_1", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD001 (747746587).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD001 (952879533)", "GreatSword_Swing_1", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD001 (952879533).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD001 (964913328)", "GreatSword_Swing_1", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD001 (964913328).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD002 (736440655)", "GreatSword_Swing_2", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD002 (736440655).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD002 (747746587)", "GreatSword_Swing_2", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD002 (747746587).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD002 (964913328)", "GreatSword_Swing_2", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD002 (964913328).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD003 (56983674)", "GreatSword_Swing_3", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD003 (56983674).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD003 (747746587)", "GreatSword_Swing_3", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD003 (747746587).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD003 (896406727)", "GreatSword_Swing_3", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD003 (896406727).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD006 (564326330)", "GreatSword_Swing_6", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD006 (564326330).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD006 (618417097)", "GreatSword_Swing_6", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD006 (618417097).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_TWO_HANDED_SWORD006 (1000801986)", "GreatSword_Swing_6", L"../../Resources/Sounds/Player/GreatSword/SE_TWO_HANDED_SWORD006 (1000801986).wav");

	//헬버드
	m_pGameInstance->LoadSound_AddGroup("Harberd_Grip_00_Play (2694160)", "Halberd_Grip", L"../../Resources/Sounds/Player/Halberd/Harberd_Grip_00_Play (2694160).wav");
	m_pGameInstance->LoadSound_AddGroup("Harberd_Grip_00_Play (227077081)", "Halberd_Grip", L"../../Resources/Sounds/Player/Halberd/Harberd_Grip_00_Play (227077081).wav");
	m_pGameInstance->LoadSound("Halberd_Catch", L"../../Resources/Sounds/Player/Halberd/Play_HALBERD_CATCH (1046216925).wav");

	m_pGameInstance->LoadSound_AddGroup("HALBERD_SWING_IMPACT_00_Play (235689974)", "Halberd_Impact_0", L"../../Resources/Sounds/Player/Halberd_Swing_Impact/HALBERD_SWING_IMPACT_00_Play (235689974).wav");
	m_pGameInstance->LoadSound_AddGroup("HALBERD_SWING_IMPACT_00_Play (603558091)", "Halberd_Impact_0", L"../../Resources/Sounds/Player/Halberd_Swing_Impact/HALBERD_SWING_IMPACT_00_Play (603558091).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_000 (521919710)", "Halberd_Swing_0", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_000 (521919710).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_000 (651453318)", "Halberd_Swing_0", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_000 (651453318).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_001 (709820153)", "Halberd_Swing_1", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_001 (709820153).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_001 (952879533)", "Halberd_Swing_1", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_001 (952879533).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_002 (362347847)", "Halberd_Swing_2", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_002 (362347847).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_002 (736440655)", "Halberd_Swing_2", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_002 (736440655).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_003 (56983674)", "Halberd_Swing_3", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_003 (56983674).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_HALBERD_003 (829245529)", "Halberd_Swing_3", L"../../Resources/Sounds/Player/Halberd/SE_HALBERD_003 (829245529).wav");

	//해머
	m_pGameInstance->LoadSound_AddGroup("Hammer_Grip_00_Play (2694160)", "Hammer_Grip", L"../../Resources/Sounds/Player/Hammer/Hammer_Grip_00_Play (2694160).wav");
	m_pGameInstance->LoadSound_AddGroup("Hammer_Grip_00_Play (227077081)", "Hammer_Grip", L"../../Resources/Sounds/Player/Hammer/Hammer_Grip_00_Play (227077081).wav");
	m_pGameInstance->LoadSound("Hammer_Catch", L"../../Resources/Sounds/Player/Hammer/HAMMER_CATCH_00_Play (286566350).wav");

	m_pGameInstance->LoadSound("Hammer_Swing_0", L"../../Resources/Sounds/Player/Hammer/Play_SE_HAMMER_000 (521919710).wav");
	m_pGameInstance->LoadSound("Hammer_Swing_1", L"../../Resources/Sounds/Player/Hammer/Play_SE_HAMMER_001 (228986056).wav");
	m_pGameInstance->LoadSound("Hammer_Swing_2", L"../../Resources/Sounds/Player/Hammer/Play_SE_HAMMER_002 (736440655).wav");
	m_pGameInstance->LoadSound("Hammer_Swing_3", L"../../Resources/Sounds/Player/Hammer/Play_SE_HAMMER_003 (56983674).wav");
	m_pGameInstance->LoadSound("Hammer_Swing_5", L"../../Resources/Sounds/Player/Hammer/Play_SE_HAMMER_005 (425265454).wav");
	m_pGameInstance->LoadSound("SE_Hammer_Swing_0", L"../../Resources/Sounds/Player/Hammer/SE_HAMMER_SWING_000_Play (1049905274).wav");

	m_pGameInstance->LoadSound_AddGroup("HAMMER_SWING_IMPACT_00_Play (108166114)", "HAMMER_SWING_IMPACT_00", L"../../Resources/Sounds/Player/Hammer/HAMMER_SWING_IMPACT_00_Play (108166114).wav");
	m_pGameInstance->LoadSound_AddGroup("HAMMER_SWING_IMPACT_00_Play (918247742)", "HAMMER_SWING_IMPACT_00", L"../../Resources/Sounds/Player/Hammer/HAMMER_SWING_IMPACT_00_Play (918247742).wav");

	//무기별 타격 사운드
	//공용
	//m_pGameInstance->LoadSound_AddGroup("69713224", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/69713224.wav");
	m_pGameInstance->LoadSound_AddGroup("318419368", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/318419368.wav");
	//m_pGameInstance->LoadSound_AddGroup("496797553", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/496797553.wav");
	//m_pGameInstance->LoadSound_AddGroup("497001196", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/497001196.wav");
	m_pGameInstance->LoadSound_AddGroup("611925224", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/611925224.wav");
	m_pGameInstance->LoadSound_AddGroup("762596108", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/762596108.wav");
	//m_pGameInstance->LoadSound_AddGroup("978912779", "Weapon_MonsterClash", L"../../Resources/Sounds/Player/Weapon_MonsterClash/978912779.wav");

	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_MassiveSmash_HIT_00_Play (148764356)", "Weapon_Clash", L"../../Resources/Sounds/Player/Weapon_Clash/Action_Renketsu_MassiveSmash_HIT_00_Play (148764356).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_MassiveSmash_HIT_00_Play (340342261)", "Weapon_Clash", L"../../Resources/Sounds/Player/Weapon_Clash/Action_Renketsu_MassiveSmash_HIT_00_Play (340342261).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_MassiveSmash_HIT_00_Play (384737489)", "Weapon_Clash", L"../../Resources/Sounds/Player/Weapon_Clash/Action_Renketsu_MassiveSmash_HIT_00_Play (384737489).wav");
	m_pGameInstance->LoadSound_AddGroup("Action_Renketsu_MassiveSmash_HIT_00_Play (714429037)", "Weapon_Clash", L"../../Resources/Sounds/Player/Weapon_Clash/Action_Renketsu_MassiveSmash_HIT_00_Play (714429037).wav");

	//바요네트
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_01_Play (148764356)", "ACTION_RENKETSU_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_ATTACK_IMPACT_01_Play (148764356).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_01_Play (340342261)", "ACTION_RENKETSU_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_ATTACK_IMPACT_01_Play (340342261).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_01_Play (625726713)", "ACTION_RENKETSU_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_ATTACK_IMPACT_01_Play (625726713).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_01_Play (770050064)", "ACTION_RENKETSU_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_ATTACK_IMPACT_01_Play (770050064).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_ATTACK_IMPACT_01_Play (959695395)", "ACTION_RENKETSU_ATTACK_IMPACT_01", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_ATTACK_IMPACT_01_Play (959695395).wav");
	
	m_pGameInstance->LoadSound_AddGroup("LONG_SWORD_SWING_01_Play (221395432)", "LONG_SWORD_SWING_01_Play", L"../../Resources/Sounds/Player/Weapon_Clash/LONG_SWORD_SWING_01_Play (221395432).wav");
	m_pGameInstance->LoadSound_AddGroup("LONG_SWORD_SWING_01_Play (576013755)", "LONG_SWORD_SWING_01_Play", L"../../Resources/Sounds/Player/Weapon_Clash/LONG_SWORD_SWING_01_Play (576013755).wav");

	//소드
	m_pGameInstance->LoadSound("LONG_SWORD_HIT_03_Play (268436447)", L"../../Resources/Sounds/Player/Weapon_Clash/LONG_SWORD_HIT_03_Play (268436447).wav");
	//롱소드
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_SWING_04_Play (388937930)", "ACTION_RENKETSU_SWING_04_Play", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_SWING_04_Play (388937930).wav");
	m_pGameInstance->LoadSound_AddGroup("ACTION_RENKETSU_SWING_04_Play (770050064)", "ACTION_RENKETSU_SWING_04_Play", L"../../Resources/Sounds/Player/Weapon_Clash/ACTION_RENKETSU_SWING_04_Play (770050064).wav");
	m_pGameInstance->LoadSound("469905387", L"../../Resources/Sounds/Player/Weapon_Clash/469905387.wav");
	//해머
	m_pGameInstance->LoadSound_AddGroup("Hammer_384737489", "Hammer_Hit", L"../../Resources/Sounds/Player/Weapon_Clash/Hammer_384737489.wav");
	m_pGameInstance->LoadSound_AddGroup("Hammer_469905387", "Hammer_Hit", L"../../Resources/Sounds/Player/Weapon_Clash/Hammer_469905387.wav");
	m_pGameInstance->LoadSound_AddGroup("Hammer_684217693", "Hammer_Hit", L"../../Resources/Sounds/Player/Weapon_Clash/Hammer_684217693.wav");
	m_pGameInstance->LoadSound_AddGroup("Hammer_148764356", "Hammer_Hit", L"../../Resources/Sounds/Player/Weapon_Clash/Hammer_148764356.wav");


	m_pGameInstance->LoadSound_AddGroup("Hit_SlaveDevilV1Weapon_Hammer_Strong_Play (921863112)", "Hit_SlaveDevilV1Weapon_Hammer_Strong_Play", L"../../Resources/Sounds/Monster/HIT_COMMON//Hit_SlaveDevilV1Weapon_Hammer_Strong_Play (921863112).wav");
	m_pGameInstance->LoadSound_AddGroup("Hit_SlaveDevilV1Weapon_Hammer_Strong_Play (521140115)", "Hit_SlaveDevilV1Weapon_Hammer_Strong_Play", L"../../Resources/Sounds/Monster/HIT_COMMON//Hit_SlaveDevilV1Weapon_Hammer_Strong_Play (521140115).wav");

	//헬버드
	m_pGameInstance->LoadSound_AddGroup("148764356", "Hallerd_Slash", L"../../Resources/Sounds/Player/Weapon_Clash/148764356.wav");
	m_pGameInstance->LoadSound_AddGroup("1069781546", "Hallerd_Slash", L"../../Resources/Sounds/Player/Weapon_Clash/1069781546.wav");

	//야쿠모
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_SHOOT_FIRE_00_Play (266910190)", "RENKETSU_SHOOT_FIRE_00", L"../../Resources/Sounds/Player/Renketsu/RENKETSU_SHOOT_FIRE_00_Play (266910190).wav");
	m_pGameInstance->LoadSound_AddGroup("RENKETSU_SHOOT_FIRE_00_Play (621471732)", "RENKETSU_SHOOT_FIRE_00", L"../../Resources/Sounds/Player/Renketsu/RENKETSU_SHOOT_FIRE_00_Play (621471732).wav");
	m_pGameInstance->LoadSound("RENKETSU_SHOOT_FIRE_01", L"../../Resources/Sounds/Player/Renketsu/RENKETSU_SHOOT_FIRE_01_Play (699562222).wav");


	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/Damage03_Male");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/DamageLaunch_Male");
	m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Player/DownDeath_Male");
#pragma endregion 

#pragma region Monster_TSKGSOLDIER
	//  m_pGameInstance->LoadSound_AddGroup("개별명", "그룹명", L"../../Resources/Sounds/Monster/TSKGSOLDIER/이름.wav");
	//  m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/Monster/TSKGSOLDIER/이름.wav");

	m_pGameInstance->LoadSound("ATTACK_KICK_IMPACT_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/ATTACK_KICK_IMPACT_00_Play (401717769).wav");
	m_pGameInstance->LoadSound("DOWN_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_00_Play (1070691473).wav");
	m_pGameInstance->LoadSound("JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/JUMP_00_Play (453349782).wav");
	m_pGameInstance->LoadSound("KETSUGI_CHAGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/KETSUGI_CHAGE_00_Play (741607759).wav");
	m_pGameInstance->LoadSound("KETSUGI_IMPACT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/KETSUGI_IMPACT_Play (741025857).wav");
	m_pGameInstance->LoadSound("SWING_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/SWING_01_Play (1038508511).wav");
	m_pGameInstance->LoadSound("SWORD_SWING_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/SWORD_SWING_00_Play (1038508511).wav");
	m_pGameInstance->LoadSound("WEAPON_SWING_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/WEAPON_SWING_00_Play (56983674).wav");

	m_pGameInstance->LoadSound_AddGroup("BLOOD_ABSORPTION_00_Play (202564825)", "BLOOD_ABSORPTION_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BLOOD_ABSORPTION_00_Play (202564825).wav");
	m_pGameInstance->LoadSound_AddGroup("BLOOD_ABSORPTION_00_Play (3481147)", "BLOOD_ABSORPTION_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BLOOD_ABSORPTION_00_Play (3481147).wav");
	m_pGameInstance->LoadSound_AddGroup("BLOOD_ABSORPTION_00_Play (390000528)", "BLOOD_ABSORPTION_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BLOOD_ABSORPTION_00_Play (390000528).wav");
	m_pGameInstance->LoadSound_AddGroup("BLOOD_ABSORPTION_00_Play (4162533)", "BLOOD_ABSORPTION_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BLOOD_ABSORPTION_00_Play (4162533).wav");
	m_pGameInstance->LoadSound_AddGroup("BLOOD_ABSORPTION_00_Play (671360593)", "BLOOD_ABSORPTION_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BLOOD_ABSORPTION_00_Play (671360593).wav");

	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (1038108975)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (1038108975).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (109835998)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (109835998).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (239323220)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (239323220).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (253183162)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (253183162).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (26002520)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (26002520).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (321763820)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (321763820).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (44557119)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (44557119).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (743071981)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (743071981).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (830871582)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (830871582).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_01_Play (905461955)", "DOWN_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_01_Play (905461955).wav");

	m_pGameInstance->LoadSound_AddGroup("DOWN_02_Play (1038087973)", "DOWN_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_02_Play (1038087973).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_02_Play (109835998)", "DOWN_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_02_Play (109835998).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_02_Play (26002520)", "DOWN_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_02_Play (26002520).wav");
	m_pGameInstance->LoadSound_AddGroup("DOWN_02_Play (3941386)", "DOWN_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/DOWN_02_Play (3941386).wav");

	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (1023104314)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (1023104314).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (442899671)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (442899671).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (4429121)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (4429121).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (497879923)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (497879923).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (658880782)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (6733536)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (6733536).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (739264218)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (739264218).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (775452738)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (775452738).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (781754008)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (781754008).wav");
	m_pGameInstance->LoadSound_AddGroup("FOOT_STEPS_Play (804724340)", "FOOT_STEPS_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/FOOT_STEPS_Play (804724340).wav");

	m_pGameInstance->LoadSound_AddGroup("GUN_HOLD_00_Play (234030925)", "GUN_HOLD_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/GUN_HOLD_00_Play (234030925).wav");
	m_pGameInstance->LoadSound_AddGroup("GUN_HOLD_00_Play (904200453)", "GUN_HOLD_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/GUN_HOLD_00_Play (904200453).wav");

	m_pGameInstance->LoadSound_AddGroup("HALBERD_IMPACT_00_Play (391473778)", "HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/HALBERD_IMPACT_00_Play (391473778).wav");
	m_pGameInstance->LoadSound_AddGroup("HALBERD_IMPACT_00_Play (942469651)", "HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/HALBERD_IMPACT_00_Play (942469651).wav");

	m_pGameInstance->LoadSound_AddGroup("HAMMER_IMPACT_00_Play (363777976)", "HAMMER_IMPACT_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/HAMMER_IMPACT_00_Play (363777976).wav");
	m_pGameInstance->LoadSound_AddGroup("HAMMER_IMPACT_00_Play (391816627)", "HAMMER_IMPACT_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/HAMMER_IMPACT_00_Play (391816627).wav");

	m_pGameInstance->LoadSound_AddGroup("KETSUGI_HIT_FIRE_Play (867199616)", "KETSUGI_HIT_FIRE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/KETSUGI_HIT_FIRE_Play (867199616).wav");
	m_pGameInstance->LoadSound_AddGroup("KETSUGI_HIT_FIRE_Play (925218210)", "KETSUGI_HIT_FIRE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/KETSUGI_HIT_FIRE_Play (925218210).wav");

	m_pGameInstance->LoadSound_AddGroup("Land_01_Play (1070739147)", "Land_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_01_Play (1070739147).wav");
	m_pGameInstance->LoadSound_AddGroup("Land_01_Play (384200197)", "Land_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_01_Play (384200197).wav");
	m_pGameInstance->LoadSound_AddGroup("Land_01_Play (915085003)", "Land_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_01_Play (915085003).wav");

	m_pGameInstance->LoadSound_AddGroup("Land_02_Play (1070739147)", "Land_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_02_Play (1070739147).wav");
	m_pGameInstance->LoadSound_AddGroup("Land_02_Play (384200197)", "Land_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_02_Play (384200197).wav");
	m_pGameInstance->LoadSound_AddGroup("Land_02_Play (915085003)", "Land_02_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/Land_02_Play (915085003).wav");

	m_pGameInstance->LoadSound_AddGroup("MOVE_00_Play (1051447142)", "MOVE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/MOVE_00_Play (1051447142).wav");
	m_pGameInstance->LoadSound_AddGroup("MOVE_00_Play (117084600)", "MOVE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/MOVE_00_Play (117084600).wav");
	m_pGameInstance->LoadSound_AddGroup("MOVE_00_Play (71972908)", "MOVE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/MOVE_00_Play (71972908).wav");

	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (14414100)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (14414100).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (174773673)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (174773673).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (25017559)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (25017559).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (324411196)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (324411196).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (338987932)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (338987932).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (468920101)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (468920101).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (539053146)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (539053146).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (600291240)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (600291240).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (716090049)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (716090049).wav");
	m_pGameInstance->LoadSound_AddGroup("NEW_DODGE_00_Play (953912517)", "NEW_DODGE_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/NEW_DODGE_00_Play (953912517).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (1071527876)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (1071527876).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (159256975)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (159256975).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (186840319)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (186840319).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (475490630)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (475490630).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (689672505)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (689672505).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (756973706)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (756973706).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (781809979)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (781809979).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (783768701)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (783768701).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (825047148)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (825047148).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_GUN_SHOT_Play (959335567)", "BARK_ATTACK_GUN_SHOT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_GUN_SHOT_Play (959335567).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (103177931)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (103177931).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (137467408)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (137467408).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (263176569)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (263176569).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (41963418)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (41963418).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (66998415)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (66998415).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (711778198)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (711778198).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (791990064)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (791990064).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (913843395)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (913843395).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (950439561)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (950439561).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_Play (986468635)", "BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_LARGE_Play (986468635).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (1019226986)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (1019226986).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (289960847)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (289960847).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (3632636)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (3632636).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (367018143)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (367018143).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (44801218)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (44801218).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (483237670)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (483237670).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (62921349)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (62921349).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (651165751)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (651165751).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (836113460)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (836113460).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_Play (910623828)", "BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ATTACK_SMALL_Play (910623828).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (1024887639)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (1024887639).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (173757889)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (173757889).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (198127343)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (198127343).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (476035695)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (476035695).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (507367106)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (507367106).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (569821496)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (569821496).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (618882535)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (618882535).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (780554171)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (780554171).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (886354993)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (886354993).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_Play (914935532)", "BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_LARGE_Play (914935532).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (1015358191)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (1015358191).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (145705984)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (145705984).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (218371676)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (218371676).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (373591530)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (373591530).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (492631423)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (492631423).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (538429117)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (538429117).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (662951940)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (662951940).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (733308521)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (733308521).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (828006422)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (828006422).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_Play (935996575)", "BARK_DAMAGE_SMALL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SMALL_Play (935996575).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (1056476860)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (1056476860).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (168040074)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (168040074).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (267334061)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (267334061).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (32453130)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (32453130).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (489483026)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (489483026).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (509114618)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (509114618).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (699471071)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (699471071).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (82968467)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (82968467).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (878903514)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (878903514).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_00_Play (896330704)", "BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_00_Play (896330704).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (1021240057)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (1021240057).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (242900726)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (242900726).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (26364846)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (26364846).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (31784523)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (31784523).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (348594899)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (348594899).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (542924043)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (542924043).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (591805835)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (591805835).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (758127325)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (758127325).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (762945112)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (762945112).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SPECIAL_SUCK_01_Play (975419447)", "BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DAMAGE_SPECIAL_SUCK_01_Play (975419447).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (12146671)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (12146671).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (126590498)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (126590498).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (189061385)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (189061385).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (224948718)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (224948718).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (289351734)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (289351734).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (489264698)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (489264698).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (557498874)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (557498874).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (766138034)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (766138034).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (77579748)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (77579748).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_00_Play (985872424)", "BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DEATH_00_Play (985872424).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (1049355463)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (1049355463).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (128609976)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (128609976).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (251473149)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (251473149).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (477595722)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (477595722).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (531314091)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (531314091).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (535670251)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (535670251).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (6115766)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (6115766).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (769841283)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (769841283).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (849430566)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (849430566).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DOWN_Play (97594375)", "BARK_DOWN_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_DOWN_Play (97594375).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (1016887411)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (1016887411).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (11618638)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (11618638).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (524410420)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (524410420).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (525733321)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (525733321).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (557995114)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (557995114).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (584627129)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (584627129).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (666198908)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (666198908).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (7423542)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (7423542).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (833993028)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (833993028).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_EAT_Play (856487673)", "BARK_EAT_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_EAT_Play (856487673).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (151127345)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (151127345).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (20191724)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (20191724).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (241519630)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (241519630).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (247015987)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (247015987).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (287679289)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (287679289).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (303423592)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (303423592).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (638744341)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (638744341).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (766732148)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (766732148).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (826470935)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (826470935).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_JUMP_00_Play (850131978)", "BARK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_JUMP_00_Play (850131978).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (262367759)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (262367759).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (3008614)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (3008614).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (431418972)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (431418972).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (483237670)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (483237670).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (56979999)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (56979999).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (602168656)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (602168656).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (727941082)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (727941082).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (818853008)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (818853008).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (876793605)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (876793605).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_PATROL_Play (955007476)", "BARK_PATROL_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_PATROL_Play (955007476).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (166003079)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (166003079).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (376833773)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (376833773).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (381030427)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (381030427).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (386920025)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (386920025).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (515714316)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (515714316).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (666657024)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (666657024).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (785661130)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (785661130).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (942628280)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (942628280).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (9738728)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (9738728).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play_01 (99029464)", "BARK_ROAR_Play_01", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_ROAR_Play_01 (99029464).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (1014949231)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (1014949231).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (164817135)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (164817135).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (324122736)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (324122736).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (449539755)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (449539755).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (47995064)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (47995064).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (62420235)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (62420235).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (642446018)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (642446018).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (703446186)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (703446186).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (895390065)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (895390065).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_Play (95370928)", "BARK_STANDBY_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/BARK_STANDBY_Play (95370928).wav");

	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (127754894)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (127754894).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (143016382)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (143016382).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (156897543)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (156897543).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (39160066)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (39160066).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (412248106)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (412248106).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (419325803)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (419325803).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (474704129)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (474704129).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (537145105)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (537145105).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (542484438)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (542484438).wav");
	m_pGameInstance->LoadSound_AddGroup("STUN_BARK_Play (660146812)", "STUN_BARK_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/STUN_BARK_Play (660146812).wav");

	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (149334518)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (149334518).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (186838182)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (186838182).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (304639280)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (304639280).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (502933091)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (502933091).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (583906323)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (583906323).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (684824235)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (684824235).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (795177783)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (795177783).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (884507304)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (884507304).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (887814)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (887814).wav");
	m_pGameInstance->LoadSound_AddGroup("TSKGSOLDIER_BARK_GETUP_Play (922706817)", "TSKGSOLDIER_BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSKGSOLDIER/BARK/TSKGSOLDIER_BARK_GETUP_Play (922706817).wav");

#pragma endregion

#pragma region Monster_TSTDSOLDIER
	//  m_pGameInstance->LoadSound_AddGroup("개별명", "그룹명", L"../../Resources/Sounds/Monster/TSTDSOLDIER/이름.wav");
	//  m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/Monster/TSTDSOLDIER/이름.wav");

	m_pGameInstance->LoadSound("TSTDSOLDIER_BUFF_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BUFF_00_Play (935998027).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_EXPLOSION_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_EXPLOSION_00_Play (751771054).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_EXPLOSION_ATTACK_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_EXPLOSION_ATTACK_00_Play (170003521).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_EXPLOSION_ATTACK_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_EXPLOSION_ATTACK_01_Play (170003521).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_EXPLOSION_MOVE_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_EXPLOSION_MOVE_00_Play (487916843).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_KNIFE_MOVE_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_KNIFE_MOVE_00_Play (480434569).wav");
	m_pGameInstance->LoadSound("TSTDSOLDIER_SWORD_SWING_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_SWING_00_Play (393276700).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (1022202065)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (1022202065).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (1024876913)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (1024876913).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (1049761732)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (1049761732).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (360113460)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (360113460).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (487033911)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (487033911).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (676734136)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (676734136).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (684525684)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (684525684).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (719495240)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (719495240).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (857254485)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (857254485).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_Play (889999031)", "BARK_ATTACK_JUMP_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK_ATTACK_JUMP_00_Play (889999031).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (205599826)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (205599826).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (319162792)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (319162792).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (360113460)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (360113460).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (376250914)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (376250914).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (406799976)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (406799976).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (537662542)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (537662542).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (673172605)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (673172605).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (738203499)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (738203499).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (768860047)", "TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_BARK_ATTACK_JUMP_00_Play_01 (768860047).wav");

	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (106669010)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (106669010).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (198579812)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (198579812).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (206285119)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (206285119).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (30000463)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (30000463).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (360772499)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (360772499).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (514070231)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (514070231).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (59493892)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (59493892).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (651793700)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (651793700).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (679726939)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (679726939).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_ATTACK_LARGE_Play (810059921)", "TStdSoldier_BARK_ATTACK_LARGE_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_BARK_ATTACK_LARGE_Play (810059921).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_DIGGING_GROUND_00_Play (1000469172)", "TSTDSOLDIER_DIGGING_GROUND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_DIGGING_GROUND_00_Play (1000469172).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_DIGGING_GROUND_00_Play (432391520)", "TSTDSOLDIER_DIGGING_GROUND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_DIGGING_GROUND_00_Play (432391520).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (170037727)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (170037727).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (447039296)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (447039296).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (505107313)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (505107313).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (658880782)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (664757634)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (664757634).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (6733536)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (6733536).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (726181377)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (726181377).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (791771586)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (791771586).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (823936837)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (823936837).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_FOOT_STEPS_LADDER_Play (949950453)", "TSTDSOLDIER_FOOT_STEPS_LADDER_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_FOOT_STEPS_LADDER_Play (949950453).wav");

	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (137557177)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (137557177).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (16106289)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (16106289).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (170037727)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (170037727).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (505107313)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (505107313).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (658880782)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (664757634)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (664757634).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (689910584)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (689910584).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (791771586)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (791771586).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (794246156)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (794246156).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_JUMP_LAND_00_Play (949950453)", "TStdSoldier_JUMP_LAND_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TStdSoldier_JUMP_LAND_00_Play (949950453).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (137557177)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (137557177).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (152117678)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (152117678).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (4429121)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (4429121).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (447039296)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (447039296).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (658880782)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (6733536)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (6733536).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (761152570)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (761152570).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (781754008)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (781754008).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (867315613)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (867315613).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_SWORD_IMPACT_01_Play (949950453)", "TSTDSOLDIER_SWORD_IMPACT_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/TSTDSOLDIER_SWORD_IMPACT_01_Play (949950453).wav");


	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (1046258606)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (1046258606).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (1059752423)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (1059752423).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (1065559228)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (1065559228).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (134418509)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (134418509).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (217719101)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (217719101).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (421021574)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (421021574).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (614558486)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (614558486).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (78055350)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (78055350).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (798279454)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (798279454).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DIG_Play (798280413)", "BARK_DIG_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_DIG_Play (798280413).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (106669015)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (106669015).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (290099644)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (290099644).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (291823351)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (291823351).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (452925633)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (452925633).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (490762958)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (490762958).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (563551291)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (563551291).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (663034958)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (663034958).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (755990988)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (755990988).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (794990445)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (794990445).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_Play (796411951)", "BARK_GETUP_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_GETUP_Play (796411951).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (104982264)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (104982264).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (141653610)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (141653610).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (187557362)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (187557362).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (231432990)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (231432990).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (315570244)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (315570244).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (351241797)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (351241797).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (402500320)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (402500320).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (77211834)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (77211834).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (922501201)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (922501201).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ROAR_Play (94933738)", "BARK_ROAR_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_ROAR_Play (94933738).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (1013550974)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (1013550974).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (327736282)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (327736282).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (412078657)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (412078657).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (519424521)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (519424521).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (523624291)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (523624291).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (643052968)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (643052968).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (666311836)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (666311836).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (750523645)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (750523645).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (83442652)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (83442652).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_TALK_Play (939601689)", "BARK_TALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_TALK_Play (939601689).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (1008080577)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (1008080577).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (1023497654)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (1023497654).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (190149332)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (190149332).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (336540729)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (336540729).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (342038851)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (342038851).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (503425452)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (503425452).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (781819472)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (781819472).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (864057206)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (864057206).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (896491124)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (896491124).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_WALK_Play (950289708)", "BARK_WALK_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/BARK_WALK_Play (950289708).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (1069815618)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (1069815618).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (119741645)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (119741645).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (38916922)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (38916922).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (475062803)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (475062803).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (623619859)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (623619859).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (652397843)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (652397843).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (738063664)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (738063664).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (746646737)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (746646737).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (91541766)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (91541766).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (936758432)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_00_Play (936758432).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (102282391)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (102282391).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (114970253)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (114970253).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (336336642)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (336336642).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (41376098)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (41376098).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (672251467)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (672251467).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (714526729)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (714526729).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (888076953)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (888076953).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (914068832)", "TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_KETSUGI_01_Play (914068832).wav");

	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (1015761036)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (1015761036).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (1026662721)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (1026662721).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (126829189)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (126829189).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (314755944)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (314755944).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (37175657)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (37175657).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (453649746)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (453649746).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (485145188)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (485145188).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (516022969)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (516022969).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (972108469)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (972108469).wav");
	m_pGameInstance->LoadSound_AddGroup("TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (991213110)", "TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01", L"../../Resources/Sounds/Monster/TSTDSOLDIER/BARK/TSTDSOLDIER_BARK_ATTACK_SMALL_Play_01 (991213110).wav");


#pragma endregion

#pragma region Monster_MILITARY_VAMPIRE
	//  m_pGameInstance->LoadSound_AddGroup("개별명", "그룹명", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/이름.wav");
	//  m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/이름.wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRE_BARK_FightThreat_02_Play_Oliver", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (450503806).wav");

	m_pGameInstance->LoadSound("LAND_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/LAND_00_Play (710564601).wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRER_JUMP_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRER_JUMP_00_Play (453349782).wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRE_Blood_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Blood_01_Play (355046738).wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRE_FIST_ATTACK_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_FIST_ATTACK_Play (654052583).wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRE_KETSUGI_CHAGE_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_KETSUGI_CHAGE_01_Play (266382464).wav");
	m_pGameInstance->LoadSound("MILITARY_VAMPIRE_SHIELD_GUARD_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_SHIELD_GUARD_Play (1037236901).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (1030857221)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (1030857221).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (299534052)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (299534052).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (340342261)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (340342261).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (391473778)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (391473778).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (590672794)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (590672794).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (781754008)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (781754008).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (794246156)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (794246156).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (804724340)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (804724340).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (917503266)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (917503266).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (949950453)", "MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIREL_HALBERD_KRUSH_01_Play (949950453).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (1030857221)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (1030857221).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (137557177)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (137557177).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (148764356)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (148764356).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (152117678)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (152117678).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (210842394)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (210842394).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (320762079)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (320762079).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (340342261)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (340342261).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (4429121)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (4429121).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (621768150)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (621768150).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_ATTACK001_01_Play (959695395)", "MILITARY_VAMPIRE_ATTACK001_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_ATTACK001_01_Play (959695395).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (321333134)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (321333134).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (429300866)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (429300866).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (43255188)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (43255188).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (480860252)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (480860252).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (524734272)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (524734272).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (704209671)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (704209671).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (714385123)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (714385123).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (879197995)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (879197995).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (915767363)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (915767363).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Attack_Bark_01_Play (934601370)", "MILITARY_VAMPIRE_Attack_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Attack_Bark_01_Play (934601370).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (1010423144)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (1010423144).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (524745834)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (524745834).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (565015555)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (565015555).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (571786620)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (571786620).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (632041566)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (632041566).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (692361497)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (692361497).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (753740849)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (753740849).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (828308319)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (828308319).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (924311846)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (924311846).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Bark_Death_01_Play (99518946)", "MILITARY_VAMPIRE_Bark_Death_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Bark_Death_01_Play (99518946).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (1066306612)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (1066306612).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (189198651)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (189198651).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (39678469)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (39678469).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (450503806)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (450503806).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (474605213)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (474605213).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (624345664)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (624345664).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (793416331)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (793416331).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (867367143)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (867367143).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_02_Play (988510589)", "MILITARY_VAMPIRE_BARK_FightThreat_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_02_Play (988510589).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (1034834676)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (1034834676).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (1045723383)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (1045723383).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (28647248)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (28647248).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (39678469)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (39678469).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (645662477)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (645662477).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (665376206)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (665376206).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (68431852)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (68431852).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (692591452)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (692591452).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_FightThreat_Play (806473133)", "MILITARY_VAMPIRE_BARK_FightThreat_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_FightThreat_Play (806473133).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (155533560)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (155533560).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (377057480)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (377057480).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (384108633)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (384108633).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (70963526)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (70963526).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (784751413)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (784751413).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (82155560)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (82155560).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (842125972)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (842125972).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (842685621)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (842685621).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (852620525)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (852620525).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_STANDBY_01_Play (869540900)", "MILITARY_VAMPIRE_BARK_STANDBY_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_STANDBY_01_Play (869540900).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (1034834676)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (1034834676).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (112353766)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (112353766).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (145498484)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (145498484).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (295253785)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (295253785).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (434004085)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (434004085).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (474605213)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (474605213).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (806473133)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (806473133).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (848915684)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (848915684).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (906100872)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (906100872).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_SUMMON_Play (984016937)", "MILITARY_VAMPIRE_BARK_SUMMON_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_BARK_SUMMON_Play (984016937).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (1070725503)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (1070725503).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (2466108)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (2466108).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (291385379)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (291385379).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (505007451)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (505007451).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (559035915)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (559035915).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (694599689)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (694599689).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (759709648)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (759709648).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (821169143)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (821169143).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (854257078)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (854257078).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Damage_Bark_00_Play (881753792)", "MILITARY_VAMPIRE_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Damage_Bark_00_Play (881753792).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (121165358)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (121165358).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (137557177)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (137557177).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (447039296)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (447039296).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (505107313)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (505107313).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (621768150)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (621768150).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (658880782)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (664757634)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (664757634).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (823936837)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (823936837).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (917503266)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (917503266).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_01_Play (949950453)", "MILITARY_VAMPIRE_DOWN_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_01_Play (949950453).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (1024162920)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (1024162920).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (109835998)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (109835998).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (249895099)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (249895099).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (253183162)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (253183162).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (321763820)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (321763820).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (34570490)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (34570490).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (412809390)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (412809390).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (560015608)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (560015608).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (607122041)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (607122041).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_DOWN_02_Play (743071981)", "MILITARY_VAMPIRE_DOWN_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_DOWN_02_Play (743071981).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (114932708)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (114932708).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (257019501)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (257019501).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (313988678)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (313988678).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (412170404)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (412170404).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (59252471)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (59252471).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (645506609)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (645506609).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (662419366)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (662419366).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (711065304)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (711065304).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (888247021)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (888247021).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_EAT_01_Play (966742368)", "MILITARY_VAMPIRE_EAT_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_EAT_01_Play (966742368).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (1023104314)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (1023104314).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (170037727)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (170037727).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (380784195)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (380784195).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (442899671)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (442899671).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (510953552)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (510953552).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (541398582)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (541398582).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (658880782)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (658880782).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (6733536)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (6733536).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (689910584)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (689910584).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Foot_Step_01_Play (791771586)", "MILITARY_VAMPIRE_Foot_Step_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Foot_Step_01_Play (791771586).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (1030857221)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (1030857221).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (121165358)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (121165358).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (148764356)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (148764356).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (363777976)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (363777976).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (590672794)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (590672794).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (889038688)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (889038688).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (917503266)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (917503266).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (942469651)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (942469651).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (959695395)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (959695395).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (963112033)", "MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_HAMMER_KRUSH_01_Play (963112033).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Land_01_Play (380784195)", "MILITARY_VAMPIRE_Land_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Land_01_Play (380784195).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Land_01_Play (497879923)", "MILITARY_VAMPIRE_Land_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Land_01_Play (497879923).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Land_01_Play (739264218)", "MILITARY_VAMPIRE_Land_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Land_01_Play (739264218).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Land_01_Play (844423958)", "MILITARY_VAMPIRE_Land_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Land_01_Play (844423958).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (100908208)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (100908208).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (117949448)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (117949448).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (278314298)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (278314298).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (331289772)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (331289772).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (4319170)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (4319170).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (505007451)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (505007451).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (638036801)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (638036801).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (816144689)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (816144689).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (854257078)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (854257078).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (931565740)", "MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Shield_Damage_Bark_00_Play (931565740).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (1029232437)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (1029232437).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (18342230)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (18342230).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (185513939)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (185513939).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (286669786)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (286669786).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (408182286)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (408182286).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (577475114)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (577475114).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (643908194)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (643908194).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (790061046)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (790061046).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_Stun_Bark_01_Play (807512955)", "MILITARY_VAMPIRE_Stun_Bark_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_Stun_Bark_01_Play (807512955).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_SWING_02_Play (1038508511)", "MILITARY_VAMPIRE_SWING_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_SWING_02_Play (1038508511).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_SWING_02_Play (384762702)", "MILITARY_VAMPIRE_SWING_02_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_SWING_02_Play (384762702).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_SWING_03_Play (1038508511)", "MILITARY_VAMPIRE_SWING_03_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_SWING_03_Play (1038508511).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_SWING_03_Play (407267113)", "MILITARY_VAMPIRE_SWING_03_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/MILITARY_VAMPIRE_SWING_03_Play (407267113).wav");

	m_pGameInstance->LoadSound_AddGroup("TMilitary_TFist_AttackKamikaze_00_Play (490752720)", "TMilitary_TFist_AttackKamikaze_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/TMilitary_TFist_AttackKamikaze_00_Play (490752720).wav");
	m_pGameInstance->LoadSound_AddGroup("TMilitary_TFist_AttackKamikaze_00_Play (792309225)", "TMilitary_TFist_AttackKamikaze_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/TMilitary_TFist_AttackKamikaze_00_Play (792309225).wav");
	m_pGameInstance->LoadSound_AddGroup("TMilitary_TFist_AttackKamikaze_00_Play (947990068)", "TMilitary_TFist_AttackKamikaze_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/TMilitary_TFist_AttackKamikaze_00_Play (947990068).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1033000440)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1033000440).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1059002489)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1059002489).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1068641587)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (1068641587).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (132262207)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (132262207).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (190596836)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (190596836).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (282287698)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (282287698).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (314755944)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (314755944).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (351154344)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (351154344).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (377252155)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (377252155).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (951931978)", "MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_ATTACK_BAYONET_Play (951931978).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (1044063259)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (1044063259).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (333725283)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (333725283).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (340781484)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (340781484).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (483237670)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (483237670).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (72660999)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (72660999).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (872649881)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (872649881).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (880185926)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (880185926).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (919995301)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (919995301).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (975578788)", "MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_REPEL_Play (975578788).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (1055911261)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (1055911261).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (12650337)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (12650337).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (246015684)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (246015684).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (265314275)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (265314275).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (279993412)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (279993412).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (431167501)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (431167501).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (616522456)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (616522456).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (647086529)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (647086529).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (739364333)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_00_Play (739364333).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (1070991511)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (1070991511).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (345070670)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (345070670).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (6105030)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (6105030).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (659288138)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (659288138).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (68827131)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (68827131).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (778175326)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (778175326).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (837557939)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (837557939).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (964890067)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (964890067).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (980100453)", "MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DAMAGE_SPECIAL_SUCK_01_Play (980100453).wav");

	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (29668391)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (29668391).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (312328326)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (312328326).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (483237670)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (483237670).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (521998193)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (521998193).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (631503646)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (631503646).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (745178363)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (745178363).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (769718948)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (769718948).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (784128907)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (784128907).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (877612977)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (877612977).wav");
	m_pGameInstance->LoadSound_AddGroup("MILITARY_VAMPIRE_BARK_DODGE_Play (9133666)", "MILITARY_VAMPIRE_BARK_DODGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/MILITARY_VAMPIRE_BARK_DODGE_Play (9133666).wav");

	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (29709118)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (29709118).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (335412047)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (335412047).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (370847964)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (370847964).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (47305458)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (47305458).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (539787943)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (539787943).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (669184515)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (669184515).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (675950292)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (675950292).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (94868117)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (94868117).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_BARK_DAMAGE_LARGE_Play (991001981)", "TStdSoldier_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/MILITARY_VAMPIRE/BARK/TStdSoldier_BARK_DAMAGE_LARGE_Play (991001981).wav");


#pragma endregion

#pragma region Monster_COMMON
	//  m_pGameInstance->LoadSound_AddGroup("개별명", "그룹명", L"../../Resources/Sounds/Monster/COMMON/이름.wav");
	//  m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/Monster/COMMON/이름.wav");
	m_pGameInstance->LoadSound("BOSS_DEAD_COMMON_00_Play", L"../../Resources/Sounds/Monster/COMMON/BOSS_DEAD_COMMON_00_Play (681003425).wav");
	m_pGameInstance->LoadSound("ENEMY_CONCENTRATION_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_CONCENTRATION_00_Play (9346435).wav");
	m_pGameInstance->LoadSound("ENEMY_WARP_OUT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_WARP_OUT_00_Play (527800861).wav");
	m_pGameInstance->LoadSound("SE_MONSTER_MOVE_L_01_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_L_01_Play (995962630).wav");
	m_pGameInstance->LoadSound("SE_MONSTER_MOVE_M_00_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_00_Play (1036423939).wav");
	m_pGameInstance->LoadSound("SE_MONSTER_MOVE_M_02_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_02_Play (957476134).wav");
	m_pGameInstance->LoadSound("SE_MONSTER_MOVE_M_03_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_03_Play (237060002).wav");
	m_pGameInstance->LoadSound("SE_MONSTER_MOVE_M_04_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_04_Play (243228236).wav");

	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (1025429832)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (1025429832).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (102637054)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (102637054).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (194780921)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (194780921).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (212255574)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (212255574).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (423962136)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (423962136).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_EAT_00_Play (648637723)", "ENEMY_EAT_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_EAT_00_Play (648637723).wav");

	m_pGameInstance->LoadSound_AddGroup("ENEMY_FLICK_00_Play (864706422)", "ENEMY_FLICK_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_FLICK_00_Play (864706422).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_FLICK_00_Play (912647010)", "ENEMY_FLICK_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_FLICK_00_Play (912647010).wav");

	m_pGameInstance->LoadSound_AddGroup("ENEMY_SCATTERING_00_Play (238653402)", "ENEMY_SCATTERING_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_SCATTERING_00_Play (238653402).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_SCATTERING_00_Play (238830797)", "ENEMY_SCATTERING_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_SCATTERING_00_Play (238830797).wav");
	m_pGameInstance->LoadSound_AddGroup("ENEMY_SCATTERING_00_Play (927021493)", "ENEMY_SCATTERING_00_Play", L"../../Resources/Sounds/Monster/COMMON/ENEMY_SCATTERING_00_Play (927021493).wav");

	m_pGameInstance->LoadSound_AddGroup("SE_MONSTER_MOVE_M_05_Play (117084600)", "SE_MONSTER_MOVE_M_05_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_05_Play (117084600).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_MONSTER_MOVE_M_05_Play (243228236)", "SE_MONSTER_MOVE_M_05_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_05_Play (243228236).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_MONSTER_MOVE_M_05_Play (577643379)", "SE_MONSTER_MOVE_M_05_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_05_Play (577643379).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_MONSTER_MOVE_M_05_Play (71972908)", "SE_MONSTER_MOVE_M_05_Play", L"../../Resources/Sounds/Monster/COMMON/SE_MONSTER_MOVE_M_05_Play (71972908).wav");

	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_MOVE_00_Play (1051447142)", "TStdSoldier_MOVE_00_Play", L"../../Resources/Sounds/Monster/COMMON/TStdSoldier_MOVE_00_Play (1051447142).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_MOVE_00_Play (117084600)", "TStdSoldier_MOVE_00_Play", L"../../Resources/Sounds/Monster/COMMON/TStdSoldier_MOVE_00_Play (117084600).wav");
	m_pGameInstance->LoadSound_AddGroup("TStdSoldier_MOVE_00_Play (71972908)", "TStdSoldier_MOVE_00_Play", L"../../Resources/Sounds/Monster/COMMON/TStdSoldier_MOVE_00_Play (71972908).wav");

	m_pGameInstance->LoadSound("GiantWhite_TCmn_KetsugiBuffSelf_N_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/GiantWhite_TCmn_KetsugiBuffSelf_N_Play (934913780).wav");
	m_pGameInstance->LoadSound("Renketsu_MonkeyDevil_Shoot_Hit_00_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/Renketsu_MonkeyDevil_Shoot_Hit_00_Play (320866552).wav");
	m_pGameInstance->LoadSound("Renketsu_Spawner_Common_07_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/Renketsu_Spawner_Common_07_Play (770147103).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_Explosion_Common_002_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Explosion_Common_002_Play (756096637).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_MonkeyDevil_Shoot_Hit_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_MonkeyDevil_Shoot_Hit_000_Play (1051481878).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_MonkeyDevil_Shoot_Move_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_MonkeyDevil_Shoot_Move_000_Play (845586054).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_Move_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Move_Common_000_Play (248690337).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_Move_Common_001_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Move_Common_001_Play (320190215).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_ShellDevil_BreathShoot_Hit_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevil_BreathShoot_Hit_000_Play (51018729).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_ShellDevil_BreathShoot_Move_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevil_BreathShoot_Move_000_Play (221599742).wav");
	m_pGameInstance->LoadSound("SE_Renketsu_ShellDevil_BreathShoot_Spawner_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevil_BreathShoot_Spawner_000_Play (155912646).wav");

	m_pGameInstance->LoadSound_AddGroup("KETSUGI_BARRIER_OUT_01_Play (130688578)", "KETSUGI_BARRIER_OUT_01_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/KETSUGI_BARRIER_OUT_01_Play (130688578).wav");
	m_pGameInstance->LoadSound_AddGroup("KETSUGI_BARRIER_OUT_01_Play (199326196)", "KETSUGI_BARRIER_OUT_01_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/KETSUGI_BARRIER_OUT_01_Play (199326196).wav");

	m_pGameInstance->LoadSound_AddGroup("KETSUGI_BARRIER_START_00_Play (225732037)", "KETSUGI_BARRIER_START_00_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/KETSUGI_BARRIER_START_00_Play (225732037).wav");
	m_pGameInstance->LoadSound_AddGroup("KETSUGI_BARRIER_START_00_Play (789117008)", "KETSUGI_BARRIER_START_00_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/KETSUGI_BARRIER_START_00_Play (789117008).wav");

	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_Sand_Explosion_Common_000_Play (1070278221)", "SE_Renketsu_Sand_Explosion_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Sand_Explosion_Common_000_Play (1070278221).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_Sand_Explosion_Common_000_Play (911065584)", "SE_Renketsu_Sand_Explosion_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Sand_Explosion_Common_000_Play (911065584).wav");

	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (446956504)", "SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (446956504).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (50712877)", "SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (50712877).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (624206753)", "SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_ShellDevilV3_BreathShoot_Spawner_000_Play (624206753).wav");

	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_Spawner_Common_000_Play (251549781)", "SE_Renketsu_Spawner_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Spawner_Common_000_Play (251549781).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_Spawner_Common_000_Play (410334917)", "SE_Renketsu_Spawner_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Spawner_Common_000_Play (410334917).wav");
	m_pGameInstance->LoadSound_AddGroup("SE_Renketsu_Spawner_Common_000_Play (873258319)", "SE_Renketsu_Spawner_Common_000_Play", L"../../Resources/Sounds/Monster/COMMON/RENKETSU/SE_Renketsu_Spawner_Common_000_Play (873258319).wav");


#pragma endregion

#pragma region Monster_동행자대사
	//  m_pGameInstance->LoadSound_AddGroup("개별명", "그룹명", L"../../Resources/Sounds/NPC/Yakumo/이름.wav");
	//  m_pGameInstance->LoadSound("이름", L"../../Resources/Sounds/NPC/Yakumo/이름.wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_01_yakumo_Play (320406884)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_01_yakumo_Play (320406884).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_02_yakumo_Play (359694412)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_02_yakumo_Play (359694412).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_03_yakumo_Play (1019462034)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_03_yakumo_Play (1019462034).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_04_yakumo_Play (66722537)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_04_yakumo_Play (66722537).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_05_yakumo_Play (37588836)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_05_yakumo_Play (37588836).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_AdventureStart_06_yakumo_Play (695632934)", "Adv_AdventureStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_AdventureStart_06_yakumo_Play (695632934).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_Anxious_01_yakumo_Play (819890213)", "Adv_Anxious_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_Anxious_01_yakumo_Play (819890213).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_Anxious_02_yakumo_Play (683516573)", "Adv_Anxious_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_Anxious_02_yakumo_Play (683516573).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_GetItem_01_yakumo_Play (738465868)", "Adv_GetItem_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_GetItem_01_yakumo_Play (738465868).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_GetItem_02_yakumo_Play (86645746)", "Adv_GetItem_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_GetItem_02_yakumo_Play (86645746).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_InBattlePcFewRecover_01_yakumo_Play (237094082)", "Adv_InBattlePcFewRecover_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_InBattlePcFewRecover_01_yakumo_Play (237094082).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_InBattlePcFewRecover_02_yakumo_Play (44123806)", "Adv_InBattlePcFewRecover_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_InBattlePcFewRecover_02_yakumo_Play (44123806).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_NoMove_01_yakumo_Play (706758789)", "Adv_NoMove_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NoMove_01_yakumo_Play (706758789).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_NoMove_02_yakumo_Play (31209975)", "Adv_NoMove_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NoMove_02_yakumo_Play (31209975).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_NormalPath_01_yakumo_Play (537409378)", "Adv_NormalPath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NormalPath_01_yakumo_Play (537409378).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_NormalPath_02_yakumo_Play (565912785)", "Adv_NormalPath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NormalPath_02_yakumo_Play (565912785).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_NormalPath_03_yakumo_Play (805901725)", "Adv_NormalPath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NormalPath_03_yakumo_Play (805901725).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_NormalPath_04_yakumo_Play (903378049)", "Adv_NormalPath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NormalPath_04_yakumo_Play (903378049).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_NormalPath_05_yakumo_Play (796154869)", "Adv_NormalPath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_NormalPath_05_yakumo_Play (796154869).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_01_yakumo_Play (454867881)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_01_yakumo_Play (454867881).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_02_yakumo_Play (912732606)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_02_yakumo_Play (912732606).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_03_yakumo_Play (450143734)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_03_yakumo_Play (450143734).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_04_yakumo_Play (736923817)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_04_yakumo_Play (736923817).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_05_yakumo_Play (392160133)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_05_yakumo_Play (392160133).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_PcRecoverable_06_yakumo_Play (414957240)", "Adv_PcRecoverable_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_PcRecoverable_06_yakumo_Play (414957240).wav");

	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_01_yakumo_Play (813533808)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_01_yakumo_Play (813533808).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_02_yakumo_Play (917265066)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_02_yakumo_Play (917265066).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_03_yakumo_Play (699233085)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_03_yakumo_Play (699233085).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_04_yakumo_Play (715050023)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_04_yakumo_Play (715050023).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_05_yakumo_Play (1025120696)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_05_yakumo_Play (1025120696).wav");
	m_pGameInstance->LoadSound_AddGroup("Adv_RevivalCheer_06_yakumo_Play (555981830)", "Adv_RevivalCheer_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Adv_RevivalCheer_06_yakumo_Play (555981830).wav");

	m_pGameInstance->LoadSound_AddGroup("Btl_BossBattleClear_01_yakumo_Play (575778218)", "Btl_BossBattleClear_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_BossBattleClear_01_yakumo_Play (575778218).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_BossBattleClear_02_yakumo_Play (653925961)", "Btl_BossBattleClear_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_BossBattleClear_02_yakumo_Play (653925961).wav");

	m_pGameInstance->LoadSound_AddGroup("Btl_BossBattleStart_01_yakumo_Play (1032494524)", "Btl_BossBattleStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_BossBattleStart_01_yakumo_Play (1032494524).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_BossBattleStart_02_yakumo_Play (67421380)", "Btl_BossBattleStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_BossBattleStart_02_yakumo_Play (67421380).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_BossBattleStart_03_yakumo_Play (643318004)", "Btl_BossBattleStart_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_BossBattleStart_03_yakumo_Play (643318004).wav");

	m_pGameInstance->LoadSound_AddGroup("Btl_DownDeath_01_yakumo_Play (850687068)", "Btl_DownDeath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_DownDeath_01_yakumo_Play (850687068).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_DownDeath_02_yakumo_Play (417205864)", "Btl_DownDeath_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_DownDeath_02_yakumo_Play (417205864).wav");

	m_pGameInstance->LoadSound_AddGroup("Btl_Praise_01_yakumo_Play (206567451)", "Btl_Praise_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_Praise_01_yakumo_Play (206567451).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_Praise_02_yakumo_Play (765789266)", "Btl_Praise_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_Praise_02_yakumo_Play (765789266).wav");
	m_pGameInstance->LoadSound_AddGroup("Btl_Praise_03_yakumo_Play (862101387)", "Btl_Praise_01_yakumo_Play", L"../../Resources/Sounds/NPC/Yakumo/Btl_Praise_03_yakumo_Play (862101387).wav");

	m_pGameInstance->LoadSound("AttackNormal_Play", L"../../Resources/Sounds/NPC/Yakumo/AttackNormal_Play (767310086).wav");
	m_pGameInstance->LoadSound("AttackRepel_Play", L"../../Resources/Sounds/NPC/Yakumo/AttackRepel_Play (235151746).wav");
	m_pGameInstance->LoadSound("AttackStrong_Play", L"../../Resources/Sounds/NPC/Yakumo/AttackStrong_Play (452770939).wav");
	m_pGameInstance->LoadSound("AttackUnique_Play", L"../../Resources/Sounds/NPC/Yakumo/AttackUnique_Play (523704269).wav");

	m_pGameInstance->LoadSound("Damage01_Play", L"../../Resources/Sounds/NPC/Yakumo/Damage01_Play (242212819).wav");
	m_pGameInstance->LoadSound("Damage03_Play", L"../../Resources/Sounds/NPC/Yakumo/Damage03_Play (104789454).wav");

#pragma endregion

#pragma region Monster_001Normal
	m_pGameInstance->LoadSound("Play_001_Normal_Attack_Gun_Hit", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Attack_Gun_Hit (837384431).wav");
	m_pGameInstance->LoadSound("Play_SE_001_NOMAL_SWING_01", L"../../Resources/Sounds/Monster/001Normal/Se/Play_SE_001_NOMAL_SWING_01 (1038508511).wav");

	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Attack_Bark (447726846)", "Play_001_Normal_Attack_Bark", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Attack_Bark (447726846).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Attack_Bark (534642231)", "Play_001_Normal_Attack_Bark", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Attack_Bark (534642231).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Attack_Bark (535293998)", "Play_001_Normal_Attack_Bark", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Attack_Bark (535293998).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Attack_Bark (649087954)", "Play_001_Normal_Attack_Bark", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Attack_Bark (649087954).wav");

	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Land (380784195)", "Play_001_Normal_Land", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Land (380784195).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Land (497879923)", "Play_001_Normal_Land", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Land (497879923).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Land (739264218)", "Play_001_Normal_Land", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Land (739264218).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_001_Normal_Land (844423958)", "Play_001_Normal_Land", L"../../Resources/Sounds/Monster/001Normal/Se/Play_001_Normal_Land (844423958).wav");

	m_pGameInstance->LoadSound_AddGroup("Play_SE_001_NOMAL_HAMMER_KRUSH_000 (650436947)", "Play_SE_001_NOMAL_HAMMER_KRUSH_000", L"../../Resources/Sounds/Monster/001Normal/Se/Play_SE_001_NOMAL_HAMMER_KRUSH_000 (650436947).wav");
	m_pGameInstance->LoadSound_AddGroup("Play_SE_001_NOMAL_HAMMER_KRUSH_000 (942469651)", "Play_SE_001_NOMAL_HAMMER_KRUSH_000", L"../../Resources/Sounds/Monster/001Normal/Se/Play_SE_001_NOMAL_HAMMER_KRUSH_000 (942469651).wav");

#pragma endregion

#pragma region Monster_BLACK_KNIGHT
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (128381662)", "BLACK_KNIGHT_BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (128381662).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (532433361)", "BLACK_KNIGHT_BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (532433361).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (830087101)", "BLACK_KNIGHT_BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (830087101).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (954680520)", "BLACK_KNIGHT_BARK_ATTACK_SMALL_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_ATTACK_SMALL_Play (954680520).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (152436079)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (152436079).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (168300268)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (168300268).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (261433005)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (261433005).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (37688267)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (37688267).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (471474161)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (471474161).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (757513560)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (757513560).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (819123531)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (819123531).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (82632657)", "BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DAMAGE_LARGE_Play (82632657).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (118108751)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (118108751).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (4743494)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (4743494).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (563737043)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (563737043).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (599205854)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (599205854).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (631302569)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (631302569).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (674489521)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (674489521).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (724436087)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (724436087).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (870108478)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (870108478).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_00_Play (992784701)", "BLACK_KNIGHT_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_00_Play (992784701).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_01_Play (1062495068)", "BLACK_KNIGHT_BARK_DEATH_01_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_01_Play (1062495068).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_01_Play (37216521)", "BLACK_KNIGHT_BARK_DEATH_01_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_01_Play (37216521).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_01_Play (75310764)", "BLACK_KNIGHT_BARK_DEATH_01_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_01_Play (75310764).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (118108751)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (118108751).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (37216521)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (37216521).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (4743494)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (4743494).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (563737043)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (563737043).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (674489521)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (674489521).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (870108478)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (870108478).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_DEATH_02_Play (992784701)", "BLACK_KNIGHT_BARK_DEATH_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_DEATH_02_Play (992784701).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (1006477220)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (1006477220).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (1011572414)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (1011572414).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (154920311)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (154920311).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (240259157)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (240259157).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (379459324)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (379459324).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (63868130)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (63868130).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (676419047)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (676419047).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (710930067)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (710930067).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (739639015)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (739639015).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (917975911)", "BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BARK_SPECIAL_SUCK_00_Play (917975911).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (1003508628)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (1003508628).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (231167747)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (231167747).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (266116600)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (266116600).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (480103718)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (480103718).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (622038547)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (622038547).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (736252057)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (736252057).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (744920218)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (744920218).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLACK_STANDBY_00_Play (989497445)", "BLACK_KNIGHT_BLACK_STANDBY_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLACK_STANDBY_00_Play (989497445).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLESS_BARK_00_Play (106259144)", "BLACK_KNIGHT_BLESS_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLESS_BARK_00_Play (106259144).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLESS_BARK_00_Play (465245934)", "BLACK_KNIGHT_BLESS_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLESS_BARK_00_Play (465245934).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLESS_BARK_00_Play (52514719)", "BLACK_KNIGHT_BLESS_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLESS_BARK_00_Play (52514719).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BLESS_BARK_00_Play (766285243)", "BLACK_KNIGHT_BLESS_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_BLESS_BARK_00_Play (766285243).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (1018666884)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (1018666884).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (106126665)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (106126665).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (289618272)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (289618272).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (484605547)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (484605547).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (523660020)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (523660020).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DAMAGE_REPEL_00_Play (791564669)", "BLACK_KNIGHT_DAMAGE_REPEL_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_DAMAGE_REPEL_00_Play (791564669).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (118718349)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (118718349).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (140965907)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (140965907).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (294718869)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (294718869).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (342230115)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (342230115).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (458885236)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (458885236).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (49357014)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (49357014).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (528060271)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (528060271).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (844846120)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (844846120).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_JUMP_BARK_00_Play (961909272)", "BLACK_KNIGHT_JUMP_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_JUMP_BARK_00_Play (961909272).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (1009732566)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (1009732566).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (106259144)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (106259144).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (465245934)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (465245934).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (473797446)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (473797446).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (52514719)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (52514719).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (766285243)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (766285243).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_STUN_BARK_00_Play (927499981)", "BLACK_KNIGHT_STUN_BARK_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BARK/BLACK_KNIGHT_STUN_BARK_00_Play (927499981).wav");

	m_pGameInstance->LoadSound("BLACK_KNIGHT_AttackShootTwoHand_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_AttackShootTwoHand_00_Play (1026692975).wav");
	m_pGameInstance->LoadSound("BLACK_KNIGHT_ATTACK_WARP_02_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_WARP_02_Play (851292610).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (1055759892)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (1055759892).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (1067068203)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (1067068203).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (112202980)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (112202980).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (1626836)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (1626836).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (647540034)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (647540034).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (664143033)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (664143033).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (715499499)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (715499499).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (770352481)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (770352481).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (802582016)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (802582016).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_ATTACK_SWING_00_Play (893551178)", "BLACK_KNIGHT_ATTACK_SWING_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_ATTACK_SWING_00_Play (893551178).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BATONET_CATCH_Play (1046216925)", "BLACK_KNIGHT_BATONET_CATCH_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_BATONET_CATCH_Play (1046216925).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BATONET_CATCH_Play (198844890)", "BLACK_KNIGHT_BATONET_CATCH_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_BATONET_CATCH_Play (198844890).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_BATONET_CATCH_Play (933938523)", "BLACK_KNIGHT_BATONET_CATCH_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_BATONET_CATCH_Play (933938523).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (1038087973)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (1038087973).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (11571024)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (11571024).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (239323220)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (239323220).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (253183162)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (253183162).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (338230413)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (338230413).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (34570490)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (34570490).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (492969395)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (492969395).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (545107343)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (545107343).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (553997229)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (553997229).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_00_Play (717115119)", "BLACK_KNIGHT_DOWN_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_00_Play (717115119).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (253183162)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (253183162).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (321763820)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (321763820).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (412809390)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (412809390).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (421936207)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (421936207).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (496557301)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (496557301).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (553997229)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (553997229).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (681511910)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (681511910).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (755361094)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (755361094).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_03_Play (905461955)", "BLACK_KNIGHT_DOWN_03_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_03_Play (905461955).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (113945756)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (113945756).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (239323220)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (239323220).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (249895099)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (249895099).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (412809390)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (412809390).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (496557301)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (496557301).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (545107343)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (545107343).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (560015608)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (560015608).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (595360218)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (595360218).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (717115119)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (717115119).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_DOWN_IMPACT_00_Play (965019506)", "BLACK_KNIGHT_DOWN_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_DOWN_IMPACT_00_Play (965019506).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (121165358)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (121165358).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (152117678)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (152117678).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (193641056)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (193641056).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (299534052)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (299534052).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (541398582)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (541398582).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (621768150)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (621768150).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (685977998)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (685977998).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (742949830)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (742949830).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (761152570)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (761152570).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_FOOT_STEP_Play (794246156)", "BLACK_KNIGHT_FOOT_STEP_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_FOOT_STEP_Play (794246156).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (148764356)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (148764356).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (331001060)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (331001060).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (340342261)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (340342261).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (349813364)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (349813364).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (498059320)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (498059320).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (618366601)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (618366601).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_HALBERD_IMPACT_00_Play (959695395)", "BLACK_KNIGHT_HALBERD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_HALBERD_IMPACT_00_Play (959695395).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_MOVE_00_Play (117084600)", "BLACK_KNIGHT_MOVE_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_MOVE_00_Play (117084600).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_MOVE_00_Play (577643379)", "BLACK_KNIGHT_MOVE_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_MOVE_00_Play (577643379).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_MOVE_00_Play (71972908)", "BLACK_KNIGHT_MOVE_00_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_MOVE_00_Play (71972908).wav");

	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_MOVE_01_Play (448255051)", "BLACK_KNIGHT_MOVE_01_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_MOVE_01_Play (448255051).wav");
	m_pGameInstance->LoadSound_AddGroup("BLACK_KNIGHT_MOVE_01_Play (574579490)", "BLACK_KNIGHT_MOVE_01_Play", L"../../Resources/Sounds/Monster/BLACK_KNIGHT/BLACK_KNIGHT_MOVE_01_Play (574579490).wav");
	
	m_pGameInstance->LoadSound("AMB_CHARACTER_CUSTOMIZE_000_Play", L"../../Resources/Sounds/Ambient/AMB_CHARACTER_CUSTOMIZE_000_Play (122390773).wav");
	m_pGameInstance->LoadSound("Play_musium", L"../../Resources/Sounds/BGM/Play_musium (114441702).wav");

	
#pragma endregion


#pragma region UI_Sound
	m_pGameInstance->LoadSound("Enter1_01", L"../../Resources/Sounds/System_Sound/Enter1_01.wav");//버튼선택시?
	m_pGameInstance->LoadSound("Cancle", L"../../Resources/Sounds/System_Sound/Cancle.wav");//뒤로가기시

	m_pGameInstance->LoadSound("Title_Enter", L"../../Resources/Sounds/System_Sound/Title_Enter.wav");//커마씬진입
	m_pGameInstance->LoadSound("Item_Ticker", L"../../Resources/Sounds/System_Sound/Item_Ticker.wav");//아이템 획득 ui띄울떄

	m_pGameInstance->LoadSound("ArrowButton", L"../../Resources/Sounds/System_Sound/ArrowButton.wav");//화살표클릭
	m_pGameInstance->LoadSound("MapInfo_UI", L"../../Resources/Sounds/System_Sound/MapInfo_UI.wav");//화살표클릭
	m_pGameInstance->LoadSound("Hover", L"../../Resources/Sounds/System_Sound/Hover.wav");//화살표클릭
	m_pGameInstance->LoadSound("Window_Open", L"../../Resources/Sounds/System_Sound/Window_Open.wav");//창열때
	m_pGameInstance->LoadSound("Select", L"../../Resources/Sounds/System_Sound/Select.wav");//창열때

	m_pGameInstance->LoadSound("LevelUp", L"../../Resources/Sounds/System_Sound/LevelUp.wav");//창열때

	m_pGameInstance->LoadSound("Tagui_Disappear", L"../../Resources/Sounds/System_Sound/Tagui_Disappear.wav");//창열때

#pragma endregion
	COUT("전역 로딩 완료");



	m_bIsComplete = true;
	m_bStaticComplete = true;


#ifdef _DEBUG

#else
	Dumping_Time();		//릴리즈때 너무 로딩빨리되서 이거추가..
#endif // _DEBUG

	return S_OK;
}
/******************************************************* 전역 프로토타입 *******************************************************/

 

//////////////////////////////////////////////////////// 로고 프로토타입 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Load_Logo()
{
	LEVEL level = LEVEL::LOGO;
	COUT("로고 컴포넌트 로딩중");
	/* 전역인데 로드시간 줄일려고 일로 뺌 */
	
	m_pGameInstance->LoadSound("Opening", L"../../Resources/Sounds/BGM/OP.ogg");

	/* For.Prototype_Component_Texture_BitmapFont */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"BitmapFont"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Font/Bitmapfont/")), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Collider_AABB, Collider::Create(m_pDevice, m_pContext, COLLIDER::AABB)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Collider_OBB, Collider::Create(m_pDevice, m_pContext, COLLIDER::OBB)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Collider_Sphere, Collider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE)), E_FAIL);


	//Bitmapfont
	CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_Medium, L"../../Resources/Font/BitmapFont/Rajdhani/Rajdhani.fnt", "Rajdhani/Rajdhani_0"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_LightGray, L"../../Resources/Font/BitmapFont/Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline.fnt", "Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Squada_One, L"../../Resources/Font/BitmapFont/Squada One/Squada One.fnt", "Squada One/Squada One"), E_FAIL);



	CHECK_FAILED(m_pGameInstance->Load_Font(Font_SquadaOne, L"../../Resources/Font/SquadaOne-Regular.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Bold, L"../../Resources/Font/Rajdhani Bold.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Light, L"../../Resources/Font/Rajdhani-Light.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Medium, L"../../Resources/Font/Rajdhani-Medium.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_SemiBold, L"../../Resources/Font/Rajdhani-SemiBold.spritefont"), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon310, L"../../Resources/Font/Yoon310.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon320, L"../../Resources/Font/Yoon320.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon330, L"../../Resources/Font/Yoon330.spritefont"), E_FAIL);


	//로딩때문에 main으로옮김
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_A2Z_5_Medium, L"../../Resources/Font/A2Z5Medium.spritefont"), E_FAIL);

	COUT("로고 텍스쳐 로딩중");

	COUT("로고 동영상 로딩중");
	/* For.Prototype_Component_Video_Title */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::LOGO), Proto_Video_Title,
	//    Video::Create(m_pDevice, m_pContext, L"../Resources/Video/Opening.mp4")), E_FAIL);


	COUT("로고 사운드 로딩중");
	//CHECK_FAILED(m_pGameInstance->LoadSound("Title_Theme", L"../Resources/Sounds/Title/Title_Theme.mp3"), E_FAIL);


	COUT("로고 오브젝트 로딩중");


	COUT("로고 UI오브젝트 로딩중");
	/* For.Prototype_UIObject_VideoPlayer */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject(L"Title"),
	//    VideoPlayer::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("로고 로딩 완료");

#ifdef _DEBUG

#else
	Dumping_Time();		//릴리즈때 너무 로딩빨리되서 이거추가..
#endif // _DEBUG
	m_bIsComplete = true;

	return S_OK;
}
/******************************************************* 로고 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 메인 프로토타입 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Load_Main()
{
	LEVEL level = LEVEL::MAIN;
	_uint iMainLevel = _uint(LEVEL::MAIN);


	//SHADERENTRY entry[3] = {
	//{"VS_MAIN", "PS_MAIN" },
	//{"VS_MAIN", "PS_MAIN_SHADOW"},
	//{"VS_MAIN", "PS_MAIN" },
	//};
	//SHADERENTRIES entries;
	//entries.pEntries = entry;
	//entries.iNumpass = 3;
	//CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"SampleNonlight"), L"../../Shader/Shader_NonLight_Sample.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries), E_FAIL);

	COUT("카메라 로딩중");

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Free,
		Camera_Free::Create(m_pDevice, m_pContext, level));
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iMainLevel, L"Prototype_GameObject_Camera_Object", CCamera_Object::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iMainLevel, Proto_GameObject_Camera_Player, Camera_Player::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("모델 로딩중");

#pragma region 모델로드
	_matrix Modelprematrix = XMMatrixIdentity();
	Modelprematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"UnvisibleCube"),
	//	Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map/Ground/UnvisibleCube.siho", Modelprematrix)), E_FAIL);
	
	_matrix prematrix = XMMatrixIdentity();
	//_wstring folderPath = L"../../Resources/Model/Map/Tutorial/";
	//_wstring folderPath = L"../../Resources/Model/Map/Under/";
	//L"../../Resources/ModelTest/Export/"
	//L"../../Resources/Model/Map/test/"
	_wstring folderPath = L"../../Resources/Model/Map/test4/";
	_uint i = 0;
	for (const auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		// .siho 파일만 처리
		if (entry.is_regular_file() && entry.path().extension() == L".siho")
		{
			++i;
			// 현재 경로 저장
			fs::path currentPath = entry.path();

			// 파일 이름 추출 (확장자 제외)
			wstring fileName = currentPath.stem().wstring();
			//if (fileName == L"SM_FloorA_st02a1_3db03f97")
			//	cout << 1;
			// Prototype 이름 생성: "Prototype_Component_Model_Wonder_Acute"
			wstring prototypeName = L"Prototype_Component_Model_" + fileName;

			// 전체 파일 경로
			wstring filePath = entry.path().wstring();

			// Prototype 등록
			MSG_FAIL(m_pGameInstance->Add_Prototype(_UINT(level), prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, prematrix))
				, L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", E_FAIL);
		}
	}
	folderPath = L"../../Resources/Model/Map/Ground/";
	m_pGameInstance->Load_Folder(folderPath, _UINT(level));
	
	prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Oliver_Phase1"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Oliver_Collins/Phase1/Oliver1.fbx", XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f)), MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Oliver_Phase2"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Oliver_Collins/Phase2/Oliver_Phase2.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);
	
	prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(90.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SlimeDevil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlimeDevil/Slime1.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SlaveDevil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlaveDevil/SlaveDevil_Only.siho", PreMatrix_X_PLUS, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Slave_Vampire"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Slave_Vampire/Slave_Vampire_Mesh1.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	


	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SavePoint"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", PreMatrix_Identity, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"MapSeal"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/MapSeal/MapSeal.siho", PreMatrix_Identity, MODELROLE::STANDALONE)), E_FAIL);

	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"MapSeal"),
	//	Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/MapSeal/MapSeal.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderTop"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Top.siho", PreMatrix_Identity)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderMiddle"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Middle.siho", PreMatrix_Identity)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderBottom"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Bottom.siho", PreMatrix_Identity)), E_FAIL);

	prematrix = XMMatrixIdentity();
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GodRay1"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/ModelTest/GodRay/GodRay1.siho", prematrix)), E_FAIL);

	prematrix = XMMatrixIdentity();
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GodRay2"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/ModelTest/GodRay/GodRay2.siho", prematrix)), E_FAIL);

	prematrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	//아이템들
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Item"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/Item.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"ItemBox_Body"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Body.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"ItemBox_Cover"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Cover.siho", prematrix)), E_FAIL);


#pragma endregion
	COUT("플레이어 모델 로딩중");
	
	Load_Model(level);

#pragma region Player
	const _wstring Proto_Com_Model_PlayerWeapon_BlackBayonet = L"Prototype_Component_Model_PlayerWeapon_BlackBayonet";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackSword = L"Prototype_Component_Model_PlayerWeapon_BlackSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackHalberd = L"Prototype_Component_Model_PlayerWeapon_BlackHalberd";
	const _wstring Proto_Com_Model_PlayerWeapon_WhiteHammer = L"Prototype_Component_Model_PlayerWeapon_WhiteHammer";

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackBayonet,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Bayonet/BlackBayonet.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackGreatSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/BlackGreatSword.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Halberd/BlackHalberd.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_WhiteHammer,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Hammer/WhiteHammer.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Sword/BlackSword.siho", prematrix)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_StateMachine,
		StateMachine::Create(m_pDevice, m_pContext)), E_FAIL);

	const _wstring Proto_Com_Model_BossWeapon_OliverP01 = L"Prototype_Component_Model_BossWeapon_OliverP01";
	const _wstring Proto_Com_Model_BossWeapon_OliverP02 = L"Prototype_Component_Model_BossWeapon_OliverP02";
	const _wstring Proto_Com_Model_MonsterWeapon_SlaveSword = L"Prototype_Component_Model_MonsterWeapon_SlaveSword";
	const _wstring Proto_Com_Model_MonsterWeapon_SlaveSword_X90 = L"Prototype_Component_Model_MonsterWeapon_SlaveSword_X90";


	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	/* For.Proto_Com_Model_BossWeapon_OliverP01 */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_BossWeapon_OliverP01,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/OliverWeapon/Phase01_Weapon.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_BossWeapon_OliverP02 */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_BossWeapon_OliverP02,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/OliverWeapon/Phase02_Weapon.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_SlaveSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/SlaveSword/SlaveSword.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_SlaveSword_X90,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/SlaveSword/SlaveSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Monster_EventShape,
		Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region Monster
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_St01_BossOliver",
		St01_BossOliver::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_St01_Slime",
		St01_Slime::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Slave_Devil",
		Slave_Devil::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Bayonet"),
		MWeapon_Bayonet::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_GreatSword"),
		MWeapon_GreatSword::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Halberd"),
		MWeapon_Halberd::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Hammer"),
		MWeapon_Hammer::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Sword"),
		MWeapon_Sword::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Slave_Vampire"),
		Slave_Vampire::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Monster_EventShape", Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_GodRay",
		GodRayObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_GodRayMesh",
		GodRayMesh::Create(m_pDevice, m_pContext, level)), E_FAIL);

#pragma endregion


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Item", Item::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Item_Box", Item_Box::Create(m_pDevice, m_pContext, level)), E_FAIL);

	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Ladder",
		CLadder::Create(m_pDevice, m_pContext, level));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_SavePoint",
		SavePoint::Create(m_pDevice, m_pContext, level));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_MapSeal",
		MapSeal::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Drum",
		Drum::Create(m_pDevice, m_pContext, level));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Collider_Cinematic"), Cinematic_EventShape::Create(m_pDevice, m_pContext, level)), E_FAIL);
	
	//COUT("오브젝트 로딩중");
	//prematrix = XMMatrixIdentity();
	///* 피직스 충돌 테스트용 */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SampleModel"),
	//	Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Wonder_Acute/Wonder_Acute.siho", prematrix)), E_FAIL);

	///* 피직스 충돌 테스트용 */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Monster_Test"), Monster_Test::Create(m_pDevice, m_pContext, level)), E_FAIL);

#pragma region Monster_SLIME
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_00_Play (90969854)", "SLIME_ATTACK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_00_Play (90969854).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_00_Play (154470184)", "SLIME_ATTACK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_00_Play (154470184).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_00_Play (215176271)", "SLIME_ATTACK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_00_Play (215176271).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_00_Play (1021132522)", "SLIME_ATTACK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_00_Play (1021132522).wav");

	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_BARK_00_Play (48704939)", "SLIME_ATTACK_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_BARK_00_Play (48704939).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_BARK_00_Play (328305066)", "SLIME_ATTACK_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_BARK_00_Play (328305066).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_ATTACK_BARK_00_Play (727684884)", "SLIME_ATTACK_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_BARK_00_Play (727684884).wav");

	m_pGameInstance->LoadSound_AddGroup("SLIME_DAMAGE_BARK_00_Play (239508907)", "SLIME_DAMAGE_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_DAMAGE_BARK_00_Play (239508907).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_DAMAGE_BARK_00_Play (654198486)", "SLIME_DAMAGE_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_DAMAGE_BARK_00_Play (654198486).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_DAMAGE_BARK_00_Play (902904582)", "SLIME_DAMAGE_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_DAMAGE_BARK_00_Play (902904582).wav");

	m_pGameInstance->LoadSound_AddGroup("SLIME_MOVE_01_Play (50670967)", "SLIME_MOVE_01_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_MOVE_01_Play (50670967).wav");
	m_pGameInstance->LoadSound_AddGroup("SLIME_MOVE_01_Play (125239686)", "SLIME_MOVE_01_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_MOVE_01_Play (125239686).wav");

	m_pGameInstance->LoadSound("SLIME_ATTACK_HIT_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_HIT_00_Play (452216834).wav");
	m_pGameInstance->LoadSound("SLIME_ATTACK_HIT_01_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_ATTACK_HIT_01_Play (414201988).wav");
	m_pGameInstance->LoadSound("SLIME_LAND_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_LAND_00_Play (1033350437).wav");
	m_pGameInstance->LoadSound("SLIME_DEATH_BARK_00_Play", L"../../Resources/Sounds/Monster/SLIME/SLIME_DEATH_BARK_00_Play (815189479).wav");

#pragma endregion

#pragma region Cinematic
	m_pGameInstance->LoadSound("Yakumo_Appeared", L"../../Resources/Sounds/Cinematic/Yakumo_Appeared.wav");

#pragma endregion Cinematic


	CHECK_FAILED(m_pGameInstance->Add_Prototype(iMainLevel, L"Prototype_GameObject_Static", StaticObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("메인 레벨 프로토타입 로딩 완료");
#ifdef _DEBUG

#else
	Dumping_Time();		//릴리즈때 너무 로딩빨리되서 이거추가..
#endif // _DEBUG
	return S_OK;
}
/******************************************************* 메인 프로토타입 *******************************************************/


//////////////////////////////////////////////////////// 거점 프로토타입 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Load_Base()
{
	LEVEL level = LEVEL::BASE;
	_uint iBaseLevel = _uint(LEVEL::BASE);

	COUT("카메라 로딩중");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iBaseLevel, Proto_GameObject_Camera_Free, Camera_Free::Create(m_pDevice, m_pContext, level)),E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iBaseLevel, Proto_GameObject_Camera_Player, Camera_Player::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iBaseLevel, Proto_GameObject_Camera_NPC, Camera_NPC::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("모델 로딩중");

#pragma region 모델로드
	_matrix Modelprematrix = XMMatrixIdentity();
	Modelprematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	_matrix prematrix = XMMatrixIdentity();
	_wstring folderPath = L"../../Resources/Model/Map/Base/";
	_uint i = 0;
	for (const auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == L".siho")
		{
			++i;

			fs::path currentPath = entry.path();
			wstring fileName = currentPath.stem().wstring();
			wstring prototypeName = L"Prototype_Component_Model_" + fileName;

			wstring filePath = entry.path().wstring();

			MSG_FAIL(m_pGameInstance->Add_Prototype(iBaseLevel, prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, prematrix))
				, L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", E_FAIL);
		}
	}

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iBaseLevel, Proto_Model(L"SavePoint"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", PreMatrix_Identity, MODELROLE::STANDALONE)), E_FAIL);

#pragma endregion

	COUT("NPC 모델 로딩중");
	_matrix NPCprematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	const _wstring NPC_MURASAME = L"Prototype_Component_Model_NPC_Murasame";
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), NPC_MURASAME,
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/NPC/Murasame/Murasame.fbx", NPCprematrix, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"NPC_Murasame"),
		NPC_Murasame::Create(m_pDevice, m_pContext, level)), E_FAIL);

#pragma region Player

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Monster_EventShape,
		Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);

	m_pGameInstance->Add_Prototype(iBaseLevel, L"Prototype_GameObject_SavePoint", SavePoint::Create(m_pDevice, m_pContext, level));

#pragma endregion

	m_pGameInstance->LoadSound("murasame_talk_normal13_0020_murasame_Play", L"../../Resources/Sounds/NPC/Murasame/murasame_talk_normal13_0020_murasame_Play (848581471).wav");
	m_pGameInstance->LoadSound("murasame_talk_buildup01_e0020_murasame_Play", L"../../Resources/Sounds/NPC/Murasame/murasame_talk_buildup01_e0020_murasame_Play (269540720).wav");
	m_pGameInstance->LoadSound("murasame_talk_buy01_s0020_murasame_Play", L"../../Resources/Sounds/NPC/Murasame/murasame_talk_buy01_s0020_murasame_Play (97775517).wav");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iBaseLevel, L"Prototype_GameObject_Static", StaticObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("메인 레벨 프로토타입 로딩 완료");
#ifdef _DEBUG

#else
	Dumping_Time();		//릴리즈때 너무 로딩빨리되서 이거추가..
#endif // _DEBUG
	return S_OK;
}
/******************************************************* 거점 프로토타입 *******************************************************/

HRESULT Client::Loader::Load_Church()
{
	LEVEL level = LEVEL::CHURCH;
	_uint iLevel = _uint(LEVEL::CHURCH);

	COUT("카메라 로딩중");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iLevel, Proto_GameObject_Camera_Free, Camera_Free::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iLevel, L"Prototype_GameObject_Camera_Object", CCamera_Object::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iLevel, Proto_GameObject_Camera_Player, Camera_Player::Create(m_pDevice, m_pContext, level)), E_FAIL);

	COUT("모델 로딩중");

#pragma region 모델로드
	_matrix Modelprematrix = XMMatrixIdentity();
	Modelprematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	_matrix prematrix = XMMatrixIdentity();
	_wstring folderPath = L"../../Resources/Model/Map/Duomo/";
	_uint i = 0;
	for (const auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == L".siho")
		{
			++i;

			fs::path currentPath = entry.path();
			wstring fileName = currentPath.stem().wstring();
			wstring prototypeName = L"Prototype_Component_Model_" + fileName;

			wstring filePath = entry.path().wstring();

			MSG_FAIL(m_pGameInstance->Add_Prototype(iLevel, prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, prematrix))
				, L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", E_FAIL);
		}
	}

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iLevel, Proto_Model(L"SavePoint"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", PreMatrix_Identity, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderTop"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Top.siho", PreMatrix_Identity)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderMiddle"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Middle.siho", PreMatrix_Identity)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderBottom"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Bottom.siho", PreMatrix_Identity)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GhostKnight_Halberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GhostKnight/GhostKnight_Halberd.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"WolfGhost"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/WolfGhost/WolfGhost_NoAnim.siho", PreMatrix_XY_PLUS, MODELROLE::STANDALONE)), E_FAIL);

	//prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GiantVampire"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GiantVampire/GiantVampire1.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Monkey_Devil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/MonkeyDevil/MyMonkey.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Giant_WhiteDevil"), //PreMatrix_XY
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GiantWhiteDevil/GiantWhiteDevil_Mesh1.siho", PreMatrix_X_PLUS, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_StateMachine,
		StateMachine::Create(m_pDevice, m_pContext)), E_FAIL);

	prematrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	//아이템들
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Item"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/Item.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"ItemBox_Body"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Body.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"ItemBox_Cover"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Cover.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Slave_Vampire"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Slave_Vampire/Slave_Vampire_Mesh1.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Slave_Vampire"),
		Slave_Vampire::Create(m_pDevice, m_pContext, level)), E_FAIL);

#pragma endregion

	COUT("플레이어 모델 로딩중");
	Load_Model(level);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"WolfGhost"), WolfGhost::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"WolfGhost_Attack"), WolfGhost_Attack::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Direct"), Projectile_Direct::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Homing"), Projectile_Homing::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_WolfHomingIce"), Projectile_WolfHomingIce::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Effect"), Projectile_Effect::Create(m_pDevice, m_pContext, level)), E_FAIL);

	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("GhostKnight_Halberd"), GhostKnight_Halberd::Create(m_pDevice, m_pContext, level)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Giant_Vampire"), Giant_Vampire::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Monkey_Devil"), Monkey_Devil::Create(m_pDevice, m_pContext, level)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Giant_WhiteDevil"), Giant_WhiteDevil::Create(m_pDevice, m_pContext, level)), E_FAIL);

	
#pragma region Player

	const _wstring Proto_Com_Model_PlayerWeapon_BlackBayonet = L"Prototype_Component_Model_PlayerWeapon_BlackBayonet";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackSword = L"Prototype_Component_Model_PlayerWeapon_BlackSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackHalberd = L"Prototype_Component_Model_PlayerWeapon_BlackHalberd";
	const _wstring Proto_Com_Model_PlayerWeapon_WhiteHammer = L"Prototype_Component_Model_PlayerWeapon_WhiteHammer";
	const _wstring Proto_Com_Model_MonsterWeapon_SlaveSword_X90 = L"Prototype_Component_Model_MonsterWeapon_SlaveSword_X90";


	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackBayonet,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Bayonet/BlackBayonet.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackGreatSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/BlackGreatSword.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Halberd/BlackHalberd.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_WhiteHammer,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Hammer/WhiteHammer.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Sword/BlackSword.siho", prematrix)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model("MonsterWeapon_GhostHalberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/GhostHalberd/Ghost_Halberd.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Monster_EventShape,
		Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_SlaveSword_X90,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/SlaveSword/SlaveSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);



	m_pGameInstance->Add_Prototype(iLevel, L"Prototype_GameObject_SavePoint", SavePoint::Create(m_pDevice, m_pContext, level));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Ladder", CLadder::Create(m_pDevice, m_pContext, level));
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Item", Item::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Item_Box", Item_Box::Create(m_pDevice, m_pContext, level)), E_FAIL);

#pragma endregion

#pragma region 몬스터 weapoin
	const _wstring Proto_Com_Model_MonsterWeapon_MilitaryLargeHalberd = L"Prototype_Component_Model_MonsterWeapon_MilitaryLargeHalberd";

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_MilitaryLargeHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/MilitaryLargeHalberd/MilitaryLargeHalberd.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Bayonet"),
		MWeapon_Bayonet::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_GreatSword"),
		MWeapon_GreatSword::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Halberd"),
		MWeapon_Halberd::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Hammer"),
		MWeapon_Hammer::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Sword"),
		MWeapon_Sword::Create(m_pDevice, m_pContext, level));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"WhiteLargeHalberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/WhiteLargeHalberd/WhiteLargeHalberd.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

#pragma endregion


#pragma region 갓레이 메쉬
	
	prematrix = XMMatrixIdentity();
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GodRay1"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/ModelTest/GodRay/GodRay1.siho", prematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_GodRayMesh",
		GodRayMesh::Create(m_pDevice, m_pContext, level)), E_FAIL);
#pragma endregion

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(iLevel), Proto_UIObject_Ending, UIObj_Ending::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iLevel, L"Prototype_GameObject_Static", StaticObject::Create(m_pDevice, m_pContext, level)), E_FAIL);
	

	m_pGameInstance->LoadSound("DuomoAmb", L"../../Resources/Sounds/Ambient/DuomoAmb.wav");

#pragma region Monster_MONKEY
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Foot_step_Play (136867407)", "MONKEY_Foot_step_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Foot_step_Play (136867407).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Foot_step_Play (202011474)", "MONKEY_Foot_step_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Foot_step_Play (202011474).wav");

	m_pGameInstance->LoadSound_AddGroup("MONKEY_SWORD_IMPACT_00_Play (157607225)", "MONKEY_SWORD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_SWORD_IMPACT_00_Play (157607225).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_SWORD_IMPACT_00_Play (391473778)", "MONKEY_SWORD_IMPACT_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_SWORD_IMPACT_00_Play (391473778).wav");

	m_pGameInstance->LoadSound_AddGroup("MONKEY_Attack_SWING_00_Play (30693307)", "MONKEY_Attack_SWING_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Attack_SWING_00_Play (30693307).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Attack_SWING_00_Play (742465554)", "MONKEY_Attack_SWING_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Attack_SWING_00_Play (742465554).wav");

	m_pGameInstance->LoadSound("MONKEY_FIST_ATTACK_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_FIST_ATTACK_Play (199572130).wav");
	m_pGameInstance->LoadSound("MONKEY_hand_step_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_hand_step_Play (287144807).wav");
	m_pGameInstance->LoadSound("MONKEY_JUMP_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_JUMP_00_Play (133638995).wav");
	m_pGameInstance->LoadSound("MONKEY_MOVE_00_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_MOVE_00_Play (422112899).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_MD_00_Play (300730350)", "BARK_ATTACK_JUMP_00_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_JUMP_00_MD_00_Play (300730350).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_MD_00_Play (323346650)", "BARK_ATTACK_JUMP_00_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_JUMP_00_MD_00_Play (323346650).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_MD_00_Play (770994877)", "BARK_ATTACK_JUMP_00_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_JUMP_00_MD_00_Play (770994877).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_JUMP_00_MD_00_Play (890928510)", "BARK_ATTACK_JUMP_00_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_JUMP_00_MD_00_Play (890928510).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_MD_00_Play (129566119)", "BARK_ATTACK_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_LARGE_MD_00_Play (129566119).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_MD_00_Play (611739473)", "BARK_ATTACK_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_LARGE_MD_00_Play (611739473).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_MD_00_Play (647570206)", "BARK_ATTACK_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_LARGE_MD_00_Play (647570206).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_LARGE_MD_00_Play (751505250)", "BARK_ATTACK_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_LARGE_MD_00_Play (751505250).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_MD_00_Play (313453477)", "BARK_ATTACK_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_SMALL_MD_00_Play (313453477).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_MD_00_Play (825638987)", "BARK_ATTACK_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_SMALL_MD_00_Play (825638987).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_MD_00_Play (839321967)", "BARK_ATTACK_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_SMALL_MD_00_Play (839321967).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_ATTACK_SMALL_MD_00_Play (894925771)", "BARK_ATTACK_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_ATTACK_SMALL_MD_00_Play (894925771).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_MD_00_Play (428110058)", "BARK_DAMAGE_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_LARGE_MD_00_Play (428110058).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_MD_00_Play (540645713)", "BARK_DAMAGE_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_LARGE_MD_00_Play (540645713).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_MD_00_Play (718818376)", "BARK_DAMAGE_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_LARGE_MD_00_Play (718818376).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_LARGE_MD_00_Play (991325136)", "BARK_DAMAGE_LARGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_LARGE_MD_00_Play (991325136).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_MD_00_Play (157543470)", "BARK_DAMAGE_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_SMALL_MD_00_Play (157543470).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_MD_00_Play (211733369)", "BARK_DAMAGE_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_SMALL_MD_00_Play (211733369).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_MD_00_Play (330791719)", "BARK_DAMAGE_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_SMALL_MD_00_Play (330791719).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DAMAGE_SMALL_MD_00_Play (749468248)", "BARK_DAMAGE_SMALL_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DAMAGE_SMALL_MD_00_Play (749468248).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_01_MD_00_Play (420631618)", "BARK_DEATH_01_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DEATH_01_MD_00_Play (420631618).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DEATH_01_MD_00_Play (656196827)", "BARK_DEATH_01_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DEATH_01_MD_00_Play (656196827).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_DODGE_MD_00_Play (590261096)", "BARK_DODGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DODGE_MD_00_Play (590261096).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DODGE_MD_00_Play (671349467)", "BARK_DODGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DODGE_MD_00_Play (671349467).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_DODGE_MD_00_Play (931772368)", "BARK_DODGE_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_DODGE_MD_00_Play (931772368).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_MD_00_Play (105080582)", "BARK_GETUP_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_GETUP_MD_00_Play (105080582).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_MD_00_Play (822785536)", "BARK_GETUP_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_GETUP_MD_00_Play (822785536).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_GETUP_MD_00_Play (1051541999)", "BARK_GETUP_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_GETUP_MD_00_Play (1051541999).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_01_MD_00_Play (372264574)", "BARK_STANDBY_01_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_STANDBY_01_MD_00_Play (372264574).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_01_MD_00_Play (1055817112)", "BARK_STANDBY_01_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_STANDBY_01_MD_00_Play (1055817112).wav");

	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_MD_00_Play (97320368)", "BARK_STANDBY_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_STANDBY_MD_00_Play (97320368).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_MD_00_Play (794373921)", "BARK_STANDBY_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_STANDBY_MD_00_Play (794373921).wav");
	m_pGameInstance->LoadSound_AddGroup("BARK_STANDBY_MD_00_Play (998943503)", "BARK_STANDBY_MD_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/BARK_STANDBY_MD_00_Play (998943503).wav");

	m_pGameInstance->LoadSound_AddGroup("MONKEY_BARK_DEATH_00_Play (533824560)", "MONKEY_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/MONKEY_BARK_DEATH_00_Play (533824560).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_BARK_DEATH_00_Play (927687689)", "MONKEY_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/MONKEY_BARK_DEATH_00_Play (927687689).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_BARK_DEATH_00_Play (937761331)", "MONKEY_BARK_DEATH_00_Play", L"../../Resources/Sounds/Monster/MONKEY/BARK/MONKEY_BARK_DEATH_00_Play (937761331).wav");

	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (1024162920)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (1024162920).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (1038087973)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (1038087973).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (214524688)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (214524688).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (321763820)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (321763820).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (34570490)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (34570490).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (595360218)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (595360218).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (68574660)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (68574660).wav");
	m_pGameInstance->LoadSound_AddGroup("MONKEY_Down_Play (717115119)", "MONKEY_Down_Play", L"../../Resources/Sounds/Monster/MONKEY/MONKEY_Down_Play (717115119).wav");
#pragma endregion

#pragma region Sound_Aurora

	m_pGameInstance->LoadSound("2St_Battle_Boss_BGM", L"../../Resources/Sounds/BGM/2St_Battle_Boss_BGM.wav");

	m_pGameInstance->LoadSound("AURORA_KETUGI_FIRE_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETUGI_FIRE_00.wav");
	m_pGameInstance->LoadSound("AURORA_KETUGI_ICE_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETUGI_ICE_00.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_00.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_02", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_02.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_03", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_03.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_09", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_09.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_KETSUGI_04_1", "AURORA_KETSUGI_04", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_04_1.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_KETSUGI_04_2", "AURORA_KETSUGI_04", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_04_2.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_KETSUGI_06_1", "AURORA_KETSUGI_06", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_06_1.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_KETSUGI_06_2", "AURORA_KETSUGI_06", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_06_2.wav");

	m_pGameInstance->LoadSound("AURORA_ATTACK_KETUGI_01", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_01.wav");
	m_pGameInstance->LoadSound("AURORA_ATTACK_KETUGI_02", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_02.wav");
	m_pGameInstance->LoadSound("AURORA_ATTACK_KETUGI_03", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_03.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_04_1", "AURORA_ATTACK_KETUGI_04", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_04_1.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_04_2", "AURORA_ATTACK_KETUGI_04", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_04_2.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_00_1", "AURORA_ATTACK_KETUGI_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_00_1.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_00_2", "AURORA_ATTACK_KETUGI_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_00_2.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_BloodBoom_01", "Aurora_BloodBoom", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_BloodBoom_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_BloodBoom_02", "Aurora_BloodBoom", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_BloodBoom_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_BloodBoom_03", "Aurora_BloodBoom", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_BloodBoom_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_BloodBoom_04", "Aurora_BloodBoom", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_BloodBoom_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_FireBlast_01", "Aurora_FireBlast", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_FireBlast_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_FireBlast_02", "Aurora_FireBlast", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_FireBlast_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_FireBlast_03", "Aurora_FireBlast", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_FireBlast_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_FireBlast_04", "Aurora_FireBlast", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_FireBlast_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_IceHoming_01", "Aurora_IceHoming", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_IceHoming_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_IceHoming_02", "Aurora_IceHoming", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_IceHoming_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_IceHoming_03", "Aurora_IceHoming", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_IceHoming_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_IceHoming_04", "Aurora_IceHoming", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_IceHoming_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_Thunder_01", "Aurora_Thunder", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_Thunder_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Thunder_02", "Aurora_Thunder", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_Thunder_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Thunder_03", "Aurora_Thunder", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_Thunder_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Thunder_04", "Aurora_Thunder", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_Thunder_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidLaser_01", "Aurora_VoidLaser", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidLaser_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidLaser_02", "Aurora_VoidLaser", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidLaser_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidLaser_03", "Aurora_VoidLaser", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidLaser_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidLaser_04", "Aurora_VoidLaser", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidLaser_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidSphere_01", "Aurora_VoidSphere", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidSphere_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidSphere_02", "Aurora_VoidSphere", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidSphere_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidSphere_03", "Aurora_VoidSphere", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidSphere_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_VoidSphere_04", "Aurora_VoidSphere", L"../../Resources/Sounds/Monster/Aurora/Bark/Aurora_VoidSphere_04.wav");

	m_pGameInstance->LoadSound_AddGroup("Aurora_Warp_01", "Aurora_Warp", L"../../Resources/Sounds/Monster/Aurora/Bark/Los_Warp_aurora_lost_Play (336748960).wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Warp_02", "Aurora_Warp", L"../../Resources/Sounds/Monster/Aurora/Bark/Los_Warp_aurora_lost_Play (351063013).wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Warp_03", "Aurora_Warp", L"../../Resources/Sounds/Monster/Aurora/Bark/Los_Warp_aurora_lost_Play (443857912).wav");
	m_pGameInstance->LoadSound_AddGroup("Aurora_Warp_04", "Aurora_Warp", L"../../Resources/Sounds/Monster/Aurora/Bark/Los_Warp_aurora_lost_Play (1050402081).wav");

	m_pGameInstance->LoadSound("AURORA_TELEPORT_01", L"../../Resources/Sounds/Monster/Aurora/AURORA_TELEPORT_01.wav");
	m_pGameInstance->LoadSound("AURORA_TELEPORT_02", L"../../Resources/Sounds/Monster/Aurora/AURORA_TELEPORT_02.wav");
	m_pGameInstance->LoadSound("AURORA_WARP_END_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_WARP_END_00.wav");

	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_CHARGE_00", "AURORA_ATTACK_KETUGI_CHARGE", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_CHARGE_00.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_CHARGE_01", "AURORA_ATTACK_KETUGI_CHARGE", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_CHARGE_01.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_ATTACK_KETUGI_CHARGE_02", "AURORA_ATTACK_KETUGI_CHARGE", L"../../Resources/Sounds/Monster/Aurora/AURORA_ATTACK_KETUGI_CHARGE_02.wav");

	m_pGameInstance->LoadSound("AURORA_MOVABLE_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_MOVABLE_00_1.wav");
	m_pGameInstance->LoadSound("AURORA_MOVABLE_01", L"../../Resources/Sounds/Monster/Aurora/AURORA_MOVABLE_01_1.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_MOVABLE_03_4", "AURORA_MOVABLE_03", L"../../Resources/Sounds/Monster/Aurora/AURORA_MOVABLE_03_4.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_MOVABLE_03_5", "AURORA_MOVABLE_03", L"../../Resources/Sounds/Monster/Aurora/AURORA_MOVABLE_03_5.wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_MOVABLE_03_6", "AURORA_MOVABLE_03", L"../../Resources/Sounds/Monster/Aurora/AURORA_MOVABLE_03_6.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_DARK_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_DARK_00.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_DARK_01", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_DARK_01.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_DARK_START_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_DARK_START_00.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_DARK_START_01", L"../../Resources/Sounds/Monster/Aurora/AURORA_KETSUGI_DARK_START_01.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_ATTACK_FIRERANGE_BOOB", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_ATTACK_FIRERANGE_BOOB.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_ATTACK_FIRERANGE_MOVE", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_ATTACK_FIRERANGE_MOVE.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_BLOODBOOM", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_BLOODBOOM.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_FIREBULLET_HIT", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_FIREBULLET_HIT.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_FIREBULLET_MOVE", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_FIREBULLET_MOVE.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_FIRELASER_BALL_HIT", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_FIRELASER_BALL_HIT.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_FIRERASER_MOVE", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_FIRERASER_MOVE.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_ICEBULLET_HITGROUND", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_ICEBULLET_HITGROUND.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_ICEBULLET_HITPLAYER", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_ICEBULLET_HITPLAYER.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_ICEBULLET_MOVE", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_ICEBULLET_MOVE.wav");

	m_pGameInstance->LoadSound("AURORA_KETSUGI_THUNDER", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_THUNDER.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_THUNDER_SPHERE_BULLETHIT", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_THUNDER_SPHERE_BULLETHIT.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_THUNDER_SPHERE_BULLETMOVE", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_THUNDER_SPHERE_BULLETMOVE.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_THUNDERRASER_MAIN", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_THUNDERRASER_MAIN.wav");
	m_pGameInstance->LoadSound("AURORA_KETSUGI_THUNDERRASER_SPARK", L"../../Resources/Sounds/Monster/Aurora/Bullet/AURORA_KETSUGI_THUNDERRASER_SPARK.wav");

	m_pGameInstance->LoadSound("AURORA_DOWN_00", L"../../Resources/Sounds/Monster/Aurora/AURORA_DOWN_00.wav");
	m_pGameInstance->LoadSound("AURORA_BARK_START", L"../../Resources/Sounds/Monster/Aurora/AURORA_BARK_START.wav");

	m_pGameInstance->LoadSound_AddGroup("Los_BossDead_aurora_lost_01", "Los_BossDead_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDead_aurora_lost_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Los_BossDead_aurora_lost_02", "Los_BossDead_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDead_aurora_lost_02.wav");

	m_pGameInstance->LoadSound_AddGroup("125435435", "Los_BossDown_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDown_aurora_lost_Play (125435435).wav");
	m_pGameInstance->LoadSound_AddGroup("320244559", "Los_BossDown_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDown_aurora_lost_Play (320244559).wav");
	m_pGameInstance->LoadSound_AddGroup("484819438", "Los_BossDown_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDown_aurora_lost_Play (484819438).wav");
	m_pGameInstance->LoadSound_AddGroup("1044426940", "Los_BossDown_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_BossDown_aurora_lost_Play (1044426940).wav");
		
	m_pGameInstance->LoadSound_AddGroup("Los_DamageSuck_aurora_lost_01", "Los_DamageSuck_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_DamageSuck_aurora_lost_01.wav");
	m_pGameInstance->LoadSound_AddGroup("Los_DamageSuck_aurora_lost_02", "Los_DamageSuck_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_DamageSuck_aurora_lost_02.wav");
	m_pGameInstance->LoadSound_AddGroup("Los_DamageSuck_aurora_lost_03", "Los_DamageSuck_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_DamageSuck_aurora_lost_03.wav");
	m_pGameInstance->LoadSound_AddGroup("Los_DamageSuck_aurora_lost_04", "Los_DamageSuck_aurora_lost", L"../../Resources/Sounds/Monster/Aurora/BARK/Los_DamageSuck_aurora_lost_04.wav");

	m_pGameInstance->LoadSound_AddGroup("AURORA_BARK_DEATH_Play (413497399)", "AURORA_BARK_DEATH_Play", L"../../Resources/Sounds/Monster/Aurora/BARK/AURORA_BARK_DEATH_Play (413497399).wav");
	m_pGameInstance->LoadSound_AddGroup("AURORA_BARK_DEATH_Play (659517566)", "AURORA_BARK_DEATH_Play", L"../../Resources/Sounds/Monster/Aurora/BARK/AURORA_BARK_DEATH_Play (659517566).wav");
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Collider_Cinematic"), Cinematic_EventShape::Create(m_pDevice, m_pContext, level)), E_FAIL);
#pragma endregion


	COUT("성당 레벨 프로토타입 로딩 완료");
#ifdef _DEBUG

#else
	Dumping_Time();		//릴리즈때 너무 로딩빨리되서 이거추가..
#endif // _DEBUG
	return S_OK;
}

//////////////////////////////////////////////////////// 스테이지별 공통 ////////////////////////////////////////////////////////
HRESULT Client::Loader::Load_Model(LEVEL _level)
{

	return S_OK;
}


HRESULT Client::Loader::Load_Effect()
{
	_uint iStaticLevel = _uint(LEVEL::STATIC);

#pragma region Effect
	COUT("이펙트 텍스처 로딩 중");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, TEXT("Prototype_Component_Texture_Effects"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/"))), E_FAIL);

	// For Prototype_Component_Texture_Dissolve
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, Proto_Texture("Dissolve"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_FX_ExternalNoise08.png", 1)), E_FAIL);

	// For Prototype_Component_Texture_Dissolve_Player
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, Proto_Texture("Dissolve_Player"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_CloudPattern_Shadow_SmokeWave_UI.png", 1)), E_FAIL);

	COUT("이펙트 텍스처 로딩 완료");

	COUT("이펙트 버퍼 로딩 중");

	/* For.Prototype_Component_VIBuffer_Particle_Basic */
	VIBuffer_Particle_Rect::PARTICLE_RECT_DESC	BasicDesc{};
	BasicDesc.IsLoop = true;
	BasicDesc.IsBillboard = true;
	BasicDesc.iNumInstance = 1;
	BasicDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BasicDesc.vSize = _float2(1.f, 1.f);
	BasicDesc.vRange = _float3(1.f, 1.f, 1.f);
	BasicDesc.vSpeed = _float2(0.3f, 0.5f);
	BasicDesc.vRotation = _float2(0.01f, 0.01f);
	BasicDesc.vLifeTime = _float2(1.5f, 2.f);
	BasicDesc.vPivot = _float3(0.f, -0.5f, 0.f);
	BasicDesc.fGravity = 0.f;
	BasicDesc.fAlpha = 1.f;
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, TEXT("Prototype_Component_VIBuffer_Particle_Basic"),
		VIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &BasicDesc)), E_FAIL);

	/* For.Prototype_Component_VIBuffer_Trail */
	VIBuffer_Trail::TRAIL_DESC TrailDesc{};
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_Component_VIBuffer_Trail",
		VIBuffer_Trail::Create(m_pDevice, m_pContext, &TrailDesc)), E_FAIL);

	COUT("이펙트 버퍼 로딩 완료");

	COUT("이펙트 객체 원형 로딩 중");

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_GameObject_BasicParticle", BasicParticle::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_GameObject_ParticleSystem", ParticleSystem::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_GameObject_TrailEffect", BasicTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_GameObject_DecalBlood", Decal_Blood::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(iStaticLevel, L"Prototype_GameObject_BasicMesh", BasicMesh::Create(m_pDevice, m_pContext)), E_FAIL);

	COUT("이펙트 객체 원형 로딩 완료");

#pragma endregion

	m_bEffectComplete = true;

	return S_OK;
}

HRESULT Client::Loader::Load_Player()
{
	LEVEL level = LEVEL::PLAYER;

	_uint iStaticLevel = _uint(LEVEL::PLAYER);

	_matrix prematrix = XMMatrixIdentity();
	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	const _wstring Proto_Com_Model_PlayerMasterBone = L"Prototype_Component_Model_PlayerMasterBone";
	const _wstring Proto_Com_Model_PlayerBody_Base = L"Prototype_Component_Model_PlayerBody_Base";
	const _wstring Proto_Com_Model_PlayerBody_Base1 = L"Prototype_Component_Model_PlayerBody_Base1";
	const _wstring Proto_Com_Model_PlayerBody_Base2 = L"Prototype_Component_Model_PlayerBody_Base2";
	const _wstring Proto_Com_Model_PlayerBody_Base3 = L"Prototype_Component_Model_PlayerBody_Base3";
	const _wstring Proto_Com_Model_PlayerBody_Base4 = L"Prototype_Component_Model_PlayerBody_Base4";
	const _wstring Proto_Com_Model_PlayerBody_Base5 = L"Prototype_Component_Model_PlayerBody_Base5";
	const _wstring Proto_Com_Model_PlayerBody_Base6 = L"Prototype_Component_Model_PlayerBody_Base6";

	const _wstring Proto_Com_Model_PlayerHair_Base = L"Prototype_Component_Model_PlayerHair_Base";
	const _wstring Proto_Com_Model_PlayerHair_Base1 = L"Prototype_Component_Model_PlayerHair_Base1";
	const _wstring Proto_Com_Model_PlayerHair_Base2 = L"Prototype_Component_Model_PlayerHair_Base2";
	const _wstring Proto_Com_Model_PlayerHair_Base3 = L"Prototype_Component_Model_PlayerHair_Base3";
	const _wstring Proto_Com_Model_PlayerHair_Base4 = L"Prototype_Component_Model_PlayerHair_Base4";
	const _wstring Proto_Com_Model_PlayerHead_Base = L"Prototype_Component_Model_PlayerHead_Base";
	const _wstring Proto_Com_Model_PlayerOuter_Drape = L"Prototype_Component_Model_PlayerOuter_Drape";
	const _wstring Proto_Com_Model_PlayerBloodWeapon_DrapeWolf = L"Prototype_Component_Model_PlayerBloodWeapon_DrapeWolf";

	const _wstring Proto_Com_Model_PlayerWeapon_BlackBayonet = L"Prototype_Component_Model_PlayerWeapon_BlackBayonet";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackSword = L"Prototype_Component_Model_PlayerWeapon_BlackSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackHalberd = L"Prototype_Component_Model_PlayerWeapon_BlackHalberd";
	const _wstring Proto_Com_Model_PlayerWeapon_WhiteHammer = L"Prototype_Component_Model_PlayerWeapon_WhiteHammer";

	const _wstring Proto_Com_Model_Player_Injection = L"Prototype_Component_Model_Player_Injection"; //주사기

	/* For.Prototype_Component_Model_PlayerMasterBone */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerMasterBone,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/MasterBone.siho", prematrix, MODELROLE::MASTER)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerBody_Base */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base1,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female2.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base2,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female3.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base3,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female4.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base4,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female5.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base5,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female7.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base6,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female8.siho", prematrix, MODELROLE::PART)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerHair_Base */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base1,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female2.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base2,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female3.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base3,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female4.siho", prematrix, MODELROLE::PART)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base4,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female10.siho", prematrix, MODELROLE::PART)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerHead_Base */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHead_Base,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Face/Face_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerOuter_Drape */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerOuter_Drape,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Outers/Drape/Drape5.siho", prematrix, MODELROLE::PART)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerBloodWeapon_DrapeWolf */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBloodWeapon_DrapeWolf,
		Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/BloodWeapons/DrapeWolf.siho", prematrix, MODELROLE::PART)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Proto_Com_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackBayonet,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Bayonet/BlackBayonet.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_PlayerWeapon_BlackGreatSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackGreatSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/BlackGreatSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_PlayerWeapon_BlackHalberd */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Halberd/BlackHalberd.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_PlayerWeapon_WhiteHammer */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_WhiteHammer,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Hammer/WhiteHammer.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_PlayerWeapon_BlackSword */		
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Sword/BlackSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	//*XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixTranslation(0.f, 0.f, 0.1f)

	/* For.Proto_Com_Model_Player_Injection */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_Player_Injection,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Injection/Injection.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player"),
		Player::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_MasterRig"),
		Player_MasterRig::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Body"),
		Player_Body::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Hair"),
		Player_Hair::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Head"),
		Player_Head::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Outer"),
		Player_Outer::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Injection"),
		Player_Injection::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_BloodWeapon"),
		Player_BloodWeapon::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_Bayonet"),
		PWeapon_Bayonet::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_GreatSword"),
		PWeapon_GreatSword::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_Halberd"),
		PWeapon_Halberd::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_Hammer"),
		PWeapon_Hammer::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_Sword"),
		PWeapon_Sword::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Bayonet_Bullet"),
		Bayonet_Bullet::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"IndraCoil_Thunder"),
		IndraCoil_Thunder::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Homing_Heal"),
		Homing_Heal::Create(m_pDevice, m_pContext, level));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_StateMachine,
		StateMachine::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_PlayerStat,
		Player_Stat::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_WeaponStat,
		Weapon_Stat::Create(m_pDevice, m_pContext)), E_FAIL);




	return S_OK;
}

HRESULT Client::Loader::Load_Yakumo()
{
	LEVEL level = LEVEL::PLAYER;

	_uint iStaticLevel = _uint(LEVEL::PLAYER);

	_matrix prematrix = XMMatrixIdentity();
	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	const _wstring Proto_Com_Model_Kakumo = L"Prototype_Component_Model_Yakumo";
	const _wstring Proto_Com_Model_YakumoWeapon_BlackGreatSword = L"Prototype_Component_Model_YakumoWeapon_BlackGreatSword";

	/* For.Prototype_Component_Model_PlayerMasterBone */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_Kakumo,
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Models/Yakumo/Yakumo.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Proto_Com_Model_PlayerWeapon_BlackGreatSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_YakumoWeapon_BlackGreatSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/Black/BlackGreatSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Yakumo"),
		Yakumo::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"YakumoWeapon"),
		Yakumo_Weapon::Create(m_pDevice, m_pContext, level));



	return S_OK;
}

HRESULT Client::Loader::Load_Customizing()
{
	LEVEL level = LEVEL::STATIC;
	LEVEL customizeLevel = LEVEL::CUSTOMIZE;

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_Texture(L"Sky_Sphere_Customize"),
		Texture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sky/Sky_%02d.png", 8)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_Com_VISkySphere,
		VIBuffer_SkySphere::Create(m_pDevice, m_pContext, 128, 128, 100.f)), E_FAIL);

#pragma region UIObject
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_CUSTOMIZINGMENU,
		UIObj_CustomMenu::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_CUSTOMIZINGMENU_BUTTON,
		UIObj_CustomMenuButton::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_CUSTOMMENU_INFO,
		UIObj_CustomizingInfo::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_PALEETE_WINDOW,
		UIObj_Window_Palette::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_PALLETE_AREA,
		UIObj_PaletteArea::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_PALLETE_DISPLAY,
		UIObj_PaletteDisplay::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_VERTICAL_SLIDER,
		UIObj_VerticalSlider::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_CUSTOMIZE_GRID,
		UIObj_CustomizeGrid::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_COLOR_SELECTOR,
		UIObj_ColorSelector::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_TEXTURE_SELECTOR,
		UIObj_TextureSelector::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_MESH_SELECTOR,
		UIObj_MeshSelector::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_ITEMGRID,
		UIObj_ItemGrid::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);



	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UIObject_VALUESELECTOR,
		UIObj_ValueSelector::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);

#pragma endregion UIObject


#pragma region Camera
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_GameObject_Camera_Free,
		Camera_Free::Create(m_pDevice, m_pContext, customizeLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_GameObject_Camera_Customize,
		Camera_Customize::Create(m_pDevice, m_pContext, customizeLevel)), E_FAIL);
#pragma endregion Camera

#pragma region NewTexture
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_Eyebrow"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Brow/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_Eye"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Eye/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_EyeBase"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/EyeBase/"))), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_EyeHighLight"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/EyeHighLight/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_Eyelash"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Eyelash/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_Hair"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Hair/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_Clothes"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Inner/"))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(customizeLevel), Proto_UITexture("Customize_FacePaint"),
		NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/SpecialColor/"))), E_FAIL);

#pragma endregion


	return S_OK;
}


void Client::Loader::Dumping_Time()
{
	Sleep(5000);
}
/******************************************************* 스테이지별 공통 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Loader* Client::Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _eLevelName)
{
	Loader* pInstance = new Loader(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_eLevelName), L"Loader 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Loader::Free()
{
	__super::Free();

	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	//CoUninitialize();
}
/******************************************************* 객체 반환 함수 *******************************************************/
