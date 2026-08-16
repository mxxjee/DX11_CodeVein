#include "Client_Define.h"
#include "IMGUI_Main.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Character.h"
#include "PhysX_Function.h"
#include "PoolingManager.h"
#include "GameClock.h"
#include "Player.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::IMGUI_Main::IMGUI_Main()
{
}

Client::IMGUI_Main::IMGUI_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ImguiWindow(pDevice, pContext)
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

Client::IMGUI_Main::~IMGUI_Main()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::IMGUI_Main::Initialize(void* arg)
{
	m_WindowTitle = "Inspector";
	m_pPoolingManager = PoolingManager::Get_Instance();

	ImGui::SetNextWindowSize(ImVec2(300.0f, 400.0f), ImGuiCond_FirstUseEver);

	m_pGameInstance->Subscribe<IMGUI_EVENT>([this](IMGUI_EVENT _event) {
		if (_event.eType == IMGUI_EVENT_TYPE::UPDATE_PROTOTYPE)
		{
			//Update_PrototypeList();
			Update_ObjectPool();
		}

		});

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 메인 Draw 함수 ////////////////////////////////////////////////////////
_uint Client::IMGUI_Main::Update_Contents(_float fTimeDelta)
{
	if (ImGui::BeginTabBar("##IMGUI_Main_TabBar"))
	{
		ImGuiIO& io = ImGui::GetIO();

		//ImGui::Begin("Performance Monitor");
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		//ImGui::End();


		if (ImGui::BeginTabItem("Object Pool"))
		{
			Draw_FastSpeed();
			Draw_ObjectPool();
			ImGui::EndTabItem();
		}

		//if (ImGui::BeginTabItem("Prototypes"))
		//{
		//	//if (ImGui::Button("Create_Character"))
		//	//	Ready_Character();

		//	if (ImGui::Button("Update List"))
		//		Update_PrototypeList();

		//	ImGui::SameLine();
		//	ImGui::Checkbox("TurnBack", &m_bTurnBack);

		//	Draw_Prototypes();
		//	ImGui::EndTabItem();
		//}

		if (ImGui::BeginTabItem("Hierarchy"))
		{
			Draw_FileMenu();
			Draw_Hierarchy();
			ImGui::EndTabItem();
		}


#ifdef _DEBUG
		if (ImGui::BeginTabItem("PhysX"))
		{
			Draw_PhysX_Debug();
			ImGui::EndTabItem();
		}
#endif

		if (ImGui::BeginTabItem("Camera"))
		{
			Draw_CameraSet();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("File"))
		{

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	return 0;
}
/******************************************************* 메인 Draw 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이어 목록 Draw함수 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_Hierarchy()
{
	unordered_map<_wstring, class Layer*> layers = m_pGameInstance->Get_Current_Layers();

	for (auto& layer : layers)
	{
		if (ImGui::TreeNode(wstringToString(layer.first).c_str()))
		{
			unordered_map<_wstring, class GameObject*> objects = layer.second->Get_GameObjects();

			if (ImGui::BeginChild(("##LayerChild_" + wstringToString(layer.first)).c_str(), ImVec2(0, 150), ImGuiChildFlags_Borders))
			{
				for (auto& object : objects)
				{
					// 각 오브젝트를 선택 가능한 항목으로 표시
					if (ImGui::Selectable(wstringToString(object.second->Get_Name()).c_str()))
					{
						IMGUI_EVENT event;
						event.eType = IMGUI_EVENT_TYPE::CHANGE_OBJ;
						event.pGameObject = object.second;
						m_pGameInstance->Publish(event);
						break;
					}
				}
			}
			ImGui::EndChild();

			ImGui::TreePop();
		}
	}
}
/******************************************************* 레이어 목록 Draw함수 *******************************************************/


//////////////////////////////////////////////////////// 세이브로드 함수 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_FileMenu()
{
	if (ImGui::Button("Save")) {
		//SAVEEVENT save;
		//save.eToolType = TOOLTYPE::MAP_TOOL;
		//m_pGameInstance->Publish<SAVEEVENT>(save);
		//save.eToolType = TOOLTYPE::SHADER_TOOL;
		//m_pGameInstance->Publish<SAVEEVENT>(save);
		COUT("세이브 기능 제거됨");
	}

	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		//LOADEVENT load;
		//load.eToolType = TOOLTYPE::MAP_TOOL;
		//m_pGameInstace->Publish<LOADEVENT>(load);

		//COUT("조합 로드"); //"../../DataFiles/Level_Main/ST01"
		////"../../DataFiles/Map_Data/Level_ShaderTest"
		////"../../DataFiles/Level_Main"
		//m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Main", _UINT(LEVEL::MAIN), true);

		//Layer* layer = m_pGameInstance->Get_Layer(Layer_Enviroment);
		//if (layer)
		//	layer->Set_PartObjLayer(true);

		//맵로드다됐음 알리기.
		//SYSTEM_EVENT Event;
		//Event.eType = SYSTEM_EVENT_TYPE::END_LOAD;
		//m_pGameInstance->Publish(Event);
	}
}
/******************************************************* 세이브로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 프로토타입 리스트 띄우기 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_Prototypes()
{
	// 프로토타입 리스트 ImGui로 표시
	static char searchPrototype[256] = {};
	ImGui::InputText("##PrototypeSearch", searchPrototype, IM_ARRAYSIZE(searchPrototype));

	// ListBox로 프로토타입 리스트 표시
	if (ImGui::BeginListBox("##PrototypeList", ImVec2(-1, 200)))
	{
		for (auto& [name, base] : m_pPrototypes)
		{
			string narrowName(wstringToString(name));
			// 검색 필터링
			if (searchPrototype[0] != '\0' && narrowName.find(searchPrototype) == string::npos) continue;
			_bool bIsSelected = (m_pSelectedPrototype == base);
			if (ImGui::Selectable(narrowName.c_str(), bIsSelected))
			{
				m_pSelectedPrototype = base;
				m_wstrSelectedPrototype = m_pSelectedPrototype->Get_PrototypeName();
			}
			if (bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// 선택된 몬스터로 바꾸기(지우고 새로 생성)
	if (ImGui::Button("Change_Monster", ImVec2(120, 20)))
	{
		if (m_pSelectedPrototype == nullptr)
			return;

		_uint level = m_pGameInstance->Get_Current_LevelID();
		GameObject* object = nullptr;
		m_pGameInstance->Add_GameObject_To_Layer(level, m_wstrSelectedPrototype, level, L"Layer_Monster", &object);

		_float4 pos = _float4{ 22.5f, -28.5f, 1.f, 1.f };
		object->Set_State(DIRECTION::POSITION, pos);
		object->Rotation(0.f, 0.f, 0.f);
		object->Get_Controller()->setFootPosition(ToPxExtendedVec3(_float3(pos.x, pos.y, pos.z)));
		if(!m_bTurnBack)
			object->Rotation(0.f, XMConvertToRadians(-90.f), 0.f);
		else
			object->Rotation(0.f, XMConvertToRadians(90.f), 0.f);

		
		IMGUI_EVENT event;
		event.eType = IMGUI_EVENT_TYPE::CHANGE_OBJ;
		event.pGameObject = object;
		event.bDeleteObject = false;
		m_pGameInstance->Publish(event);
	}

}

void Client::IMGUI_Main::Update_PrototypeList()
{
	m_pPrototypes.clear();

	// 프로토타입 리스트 가져오기
	UMAP<_wstring, class Base*> prototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::SAMPLE)];
	// 그래도 없으면 리턴
	if (prototypes.empty())
		return;

	// 순회하면서 게임오브젝트만 가져오기
	for (auto& [name, base] : prototypes)
	{
		Character* gameObject = DCAST(Character*)(base);
		// 게임오브젝트이고(컴포넌트가 아니고), 파트오브젝트가 아닐경우에 등록
		if (gameObject && !gameObject->Is_PartObj())
		{
			m_pPrototypes.emplace(name, gameObject);
		}
	}

}
/******************************************************* 프로토타입 리스트 띄우기 *******************************************************/



//////////////////////////////////////////////////////// 오브젝트 풀 리스트 띄우기 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_FastSpeed()
{
	static _float speed = 1.f;
	if (ImGui::Button("1", ImVec2(20, 20)))
	{
		speed = 1.f;
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(speed);
	}
	ImGui::SameLine();
	if (ImGui::Button("2", ImVec2(20, 20)))
	{
		speed = 2.f;
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(speed);
	}
	ImGui::SameLine();
	if (ImGui::Button("3", ImVec2(20, 20)))
	{
		speed = 3.f;
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(speed);
	}
	ImGui::SameLine();
	if (ImGui::Button("4", ImVec2(20, 20)))
	{
		speed = 4.f;
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(speed);
	}
	ImGui::SameLine();
	ImGui::Text("Set_GameSpeed");
	if (ImGui::DragFloat("0.1 ~ 5", &speed, 0.1f, 0.1f, 5.f, "%.1f"))
	{
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(speed);
	}
}
/******************************************************* 프로토타입 리스트 띄우기 *******************************************************/



//////////////////////////////////////////////////////// 오브젝트 풀 리스트 띄우기 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_ObjectPool()
{
	// 프로토타입 리스트 ImGui로 표시
	static char searchPrototype[256] = {};
	ImGui::InputText("##PoolSearch", searchPrototype, IM_ARRAYSIZE(searchPrototype));

	// ListBox로 프로토타입 리스트 표시
	if (ImGui::BeginListBox("##PoolList", ImVec2(-1, 200)))
	{
		for (auto& [name, poolID] : m_umapObjectPool)
		{
			string narrowName(wstringToString(name));
			// 검색 필터링
			if (searchPrototype[0] != '\0' && narrowName.find(searchPrototype) == string::npos) continue;
			_bool bIsSelected = (m_eSelectedPool == poolID);
			if (ImGui::Selectable(narrowName.c_str(), bIsSelected))
			{
				m_eSelectedPool = poolID;
				m_wstrSelectedPool = name;
			}
			if (bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// 선택된 몬스터로 바꾸기
	if (ImGui::Button("Change_Monster", ImVec2(120, 20)))
	{
		if (m_eSelectedPool == POOL_ID::END)
			return;

		GameObject::GAMEOBJECT_DESC desc;
		desc.vPosition = _float4{ 22.5f, -28.5f, 1.f, 1.f };
		desc.vRotation = m_bTurnBack ? _float3{ 0.f, 90.f, 0.f } : _float3{ 0.f, -90.f, 0.f };

		// 오브젝트 pool 목록에서 꺼내오기
		GameObject* object = m_pPoolingManager->Acquire(m_eSelectedPool, &desc);

		IMGUI_EVENT event;
		event.eType = IMGUI_EVENT_TYPE::CHANGE_OBJ;
		event.pGameObject = object;
		event.bDeleteObject = true;
		m_pGameInstance->Publish(event);
	}
}

void Client::IMGUI_Main::Update_ObjectPool()
{
	if (!m_pPoolingManager)
		return;

	m_umapObjectPool = m_pPoolingManager->Get_CurrentObjectPoolName();
}
/******************************************************* 오브젝트 풀 리스트 띄우기 *******************************************************/

HRESULT Client::IMGUI_Main::Ready_Character()
{
	return S_OK;
}


//////////////////////////////////////////////////////// 피직스 리스트 띄우기 ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Client::IMGUI_Main::Draw_PhysX_Debug()
{
	// PhysX Debug Visualization 마스터 토글
	_bool bEnabled = m_pGameInstance->Is_DebugVisualization();

	if (ImGui::Checkbox("Debug Visualization", &bEnabled))
	{
		m_pGameInstance->Toggle_DebugVisualization();
	}

	// 마스터가 꺼져있으면 개별 옵션 비활성화 표시
	if (!bEnabled)
	{
		ImGui::TextDisabled("Enable Debug Visualization to configure options");
		return;
	}

	ImGui::Separator();
	ImGui::Text("Collision");

	// 콜리전 셰이프 (보라색 와이어프레임)
	_bool bShapes = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eCOLLISION_SHAPES);
	if (ImGui::Checkbox("Collision Shapes", &bShapes))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eCOLLISION_SHAPES, bShapes);

	// AABB 바운딩 박스 (노란색 박스)
	_bool bAABBs = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eCOLLISION_AABBS);
	if (ImGui::Checkbox("AABB Bounds", &bAABBs))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eCOLLISION_AABBS, bAABBs);

	ImGui::Separator();
	ImGui::Text("Contacts");

	// 접촉점 (빨간 점)
	_bool bContactPt = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eCONTACT_POINT);
	if (ImGui::Checkbox("Contact Points", &bContactPt))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eCONTACT_POINT, bContactPt);

	// 접촉 법선
	_bool bContactNorm = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eCONTACT_NORMAL);
	if (ImGui::Checkbox("Contact Normals", &bContactNorm))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eCONTACT_NORMAL, bContactNorm);

	ImGui::Separator();
	ImGui::Text("Actor");

	// 액터 좌표축 (RGB 화살표)
	_bool bAxes = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eACTOR_AXES);
	if (ImGui::Checkbox("Actor Axes", &bAxes))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eACTOR_AXES, bAxes);

	ImGui::Separator();
	ImGui::Text("Velocity");

	// 선속도 벡터
	_bool bLinVel = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eBODY_LIN_VELOCITY);
	if (ImGui::Checkbox("Linear Velocity", &bLinVel))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eBODY_LIN_VELOCITY, bLinVel);

	// 각속도 벡터
	_bool bAngVel = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eBODY_ANG_VELOCITY);
	if (ImGui::Checkbox("Angular Velocity", &bAngVel))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eBODY_ANG_VELOCITY, bAngVel);

	ImGui::Separator();
	ImGui::Text("Joints");

	// 조인트 로컬 프레임
	_bool bJointFrames = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eJOINT_LOCAL_FRAMES);
	if (ImGui::Checkbox("Joint Local Frames", &bJointFrames))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, bJointFrames);

	// 조인트 리밋
	_bool bJointLimits = m_pGameInstance->Get_VisualizationParam(PxVisualizationParameter::eJOINT_LIMITS);
	if (ImGui::Checkbox("Joint Limits", &bJointLimits))
		m_pGameInstance->Set_VisualizationParam(PxVisualizationParameter::eJOINT_LIMITS, bJointLimits);

	ImGui::Separator();

	// 현재 디버그 라인 수 표시 (성능 모니터링)
	const PxRenderBuffer* pRB = m_pGameInstance->Get_PhysXRenderBuffer();
	if (pRB)
	{
		ImGui::Text("Lines: %d", pRB->getNbLines());
		ImGui::Text("Triangles: %d", pRB->getNbTriangles());
		ImGui::Text("Points: %d", pRB->getNbPoints());
	}
}
#endif // _DEBUG
/******************************************************* 피직스 리스트 띄우기 *******************************************************/



//////////////////////////////////////////////////////// 카메라 흔들림 테스트 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Draw_CameraSet()
{
	if (ImGui::CollapsingHeader("Camera Shake Test"))
	{
		static CameraShake shake = ShakePreset::HitStop();
		static _int iCurrentPriority = static_cast<_int>(shake.ePriority);

		const char* szPriorityNames[] = {
			"NONE",
			"NORMAL_ATTACK",
			"STRONG_ATTACK",
			"EARTH_QUAKE"
		};

		// 기본
		ImGui::SeparatorText("Basic");
		ImGui::SliderFloat("Duration", &shake.fDuration, 0.05f, 5.f, "%.2f");
		ImGui::SliderFloat("BlendOut Time", &shake.fBlendOutTime, 0.f, 1.f, "%.3f");
		ImGui::Combo("Priority", &iCurrentPriority, szPriorityNames, IM_ARRAYSIZE(szPriorityNames));

		// 위치 진동
		ImGui::SeparatorText("Location Oscillation");
		ImGui::SliderFloat("Amp X (Forward)", &shake.fAmpX, 0.f, 2.f, "%.3f");
		ImGui::SliderFloat("Amp Y (Vertical)", &shake.fAmpY, 0.f, 2.f, "%.3f");
		ImGui::SliderFloat("Amp Z (Lateral)", &shake.fAmpZ, 0.f, 2.f, "%.3f");
		ImGui::SliderFloat("Freq X", &shake.fFreqX, 1.f, 200.f, "%.1f");
		ImGui::SliderFloat("Freq Y", &shake.fFreqY, 1.f, 200.f, "%.1f");
		ImGui::SliderFloat("Freq Z", &shake.fFreqZ, 1.f, 200.f, "%.1f");

		// 회전 진동
		ImGui::SeparatorText("Rotation Oscillation");
		_float ampPitchDeg = XMConvertToDegrees(shake.fAmpPitch);
		_float ampYawDeg = XMConvertToDegrees(shake.fAmpYaw);

		if (ImGui::SliderFloat("Amp Pitch (deg)", &ampPitchDeg, 0.f, 5.f, "%.2f"))
			shake.fAmpPitch = XMConvertToRadians(ampPitchDeg);

		if (ImGui::SliderFloat("Amp Yaw (deg)", &ampYawDeg, 0.f, 5.f, "%.2f"))
			shake.fAmpYaw = XMConvertToRadians(ampYawDeg);

		ImGui::SliderFloat("Freq Pitch", &shake.fFreqPitch, 1.f, 200.f, "%.1f");
		ImGui::SliderFloat("Freq Yaw", &shake.fFreqYaw, 1.f, 200.f, "%.1f");

		// FOV 진동
		ImGui::SeparatorText("FOV Oscillation");
		ImGui::SliderFloat("Amp FOV", &shake.fAmpFov, 0.f, 5.f, "%.2f");
		ImGui::SliderFloat("Freq FOV", &shake.fFreqFov, 1.f, 200.f, "%.1f");

		// 임팩트 킥
		ImGui::SeparatorText("Impact Kick");
		ImGui::SliderFloat("Kick Strength", &shake.fKickStrength, 0.f, 2.f, "%.3f");
		ImGui::SliderFloat("Kick Decay", &shake.fKickDecay, 1.f, 50.f, "%.1f");
		ImGui::DragFloat4("Kick Direction", &shake.vKickDirection.x, 0.01f, -1.f, 1.f, "%.3f");

		if (ImGui::Button("Normalize Dir"))
		{
			XMVECTOR vDir = XMLoadFloat4(&shake.vKickDirection);
			vDir = XMVectorSetW(vDir, 0.f);
			if (XMVectorGetX(XMVector3Length(vDir)) > 0.0001f)
			{
				vDir = XMVector3Normalize(vDir);
				XMStoreFloat4(&shake.vKickDirection, vDir);
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		// 발행 버튼
		if (ImGui::Button("Fire Shake", ImVec2(-1.f, 30.f)))
		{
			shake.ePriority = static_cast<SHAKE_PRIORITY>(iCurrentPriority);

			CameraEvent shakeEvent;
			shakeEvent.eCameraAction = CAMERA_ACTION::SHAKE;
			shakeEvent.tShake = shake;
			m_pGameInstance->Publish(shakeEvent);
		}

		// 프리셋 버튼
		if (ImGui::TreeNode("Presets"))
		{
			if (ImGui::Button("HitStop"))
			{
				shake = ShakePreset::HitStop();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("HitStop_PlayerWeapon"))
			{
				shake = ShakePreset::HitStop_PlayerWeapon();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("HitStop_LSword"))
			{
				shake = ShakePreset::HitStop_LSword();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}

			if (ImGui::Button("FieldStamp_01"))
			{
				shake = ShakePreset::FieldStamp_01();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("FieldStamp_01_2"))
			{
				shake = ShakePreset::FieldStamp_01_2();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("FieldStamp_02"))
			{
				shake = ShakePreset::FieldStamp_02();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}

			if (ImGui::Button("FieldStamp_03"))
			{
				shake = ShakePreset::FieldStamp_03();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("Renketsu_01"))
			{
				shake = ShakePreset::Renketsu_01();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::SameLine();
			if (ImGui::Button("Renketsu_02"))
			{
				shake = ShakePreset::Renketsu_02();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}

			if (ImGui::Button("SP Suck"))
			{
				shake = ShakePreset::SPSuck();
				iCurrentPriority = static_cast<_int>(shake.ePriority);
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Cinematic Keyframe Test"))
	{
		ImGui::SliderInt("KeyFrame_Number", &m_iTestKeyCount, 2, MAX_TEST_KEYFRAMES);
		ImGui::Checkbox("AutoFallBack", &m_bTestAutoReturn);
		ImGui::Separator();

		for (_int i = 0; i < m_iTestKeyCount; ++i)
		{
			auto& key = m_testKeyFrames[i];
			string label = "Key " + to_string(i);

			if (ImGui::TreeNode((label).c_str()))
			{
				// ==================== 키프레임 조작 버튼 ====================
				bool needInsert = false;
				bool needDelete = false;
				bool needDuplicate = false;
				bool needMoveUp = false;
				bool needMoveDown = false;

				if (m_iTestKeyCount < MAX_TEST_KEYFRAMES)
				{
					if (ImGui::SmallButton(("Insert##" + label).c_str()))
						needInsert = true;
					ImGui::SameLine();
					if (ImGui::SmallButton(("Duplicate##" + label).c_str()))
						needDuplicate = true;
					ImGui::SameLine();
				}
				if (m_iTestKeyCount > 2 && i > 0)
				{
					if (ImGui::SmallButton(("Delete##" + label).c_str()))
						needDelete = true;
					ImGui::SameLine();
				}
				if (i > 1)
				{
					if (ImGui::SmallButton(("Up##" + label).c_str()))
						needMoveUp = true;
					ImGui::SameLine();
				}
				if (i > 0 && i < m_iTestKeyCount - 1)
				{
					if (ImGui::SmallButton(("Down##" + label).c_str()))
						needMoveDown = true;
				}

				ImGui::Separator();

				// ==================== Mode ====================
				_int mode = CAST(_int)(key.eMode);
				const char* szModes[] = { "RELATIVE", "ABSOLUTE", "PLAYER_OFFSET", "YAKUMO_OFFSET" };
				if (ImGui::Combo(("Mode##" + label).c_str(), &mode, szModes, 4)) 
					key.eMode = static_cast<KEYFRAME_MODE>(mode);

				// ==================== Duration ====================
				if (i == 0)
				{
					ImGui::TextDisabled("Duration: 0 (Start Point)");
					key.fDuration = 0.f;
				}
				else
				{
					ImGui::SliderFloat(("Duration##" + label).c_str(),
						&key.fDuration, 0.1f, 3.f, "%.2fs");
				}

				// ==================== Ease ====================
				if (i > 0)
				{
					_int easeType = CAST(_int)(key.eEaseType);
					const char* szEaseNames[] = {
						"Linear",
						"SmoothStep",
						"SmootherStep",
						"EaseIn Quad",
						"EaseOut Quad",
						"EaseInOut Quad",
						"EaseIn Cubic",
						"EaseOut Cubic",
						"EaseInOut Cubic",
						"EaseOut Elastic",
						"EaseOut Back",
						"EaseOut Bounce"
					};
					if (ImGui::Combo(("Ease##" + label).c_str(), &easeType,
						szEaseNames, IM_ARRAYSIZE(szEaseNames)))
						key.eEaseType = CAST(EASE_TYPE)(easeType);
				}

				ImGui::Spacing();

				// ==================== Mode Settings ====================
				if (key.eMode == KEYFRAME_MODE::RELATIVEMODE)
				{
					ImGui::SeparatorText("Offset from Start Camera");
					ImGui::DragFloat3(("Pos Offset##" + label).c_str(),
						&key.vPosOffset.x, 0.1f, -10.f, 10.f, "%.2f");

					_float pitchDeg = XMConvertToDegrees(key.fPitchOffset);
					_float yawDeg = XMConvertToDegrees(key.fYawOffset);

					if (ImGui::SliderFloat(("Pitch Offset##" + label).c_str(),
						&pitchDeg, -80.f, 80.f, "%.1f deg"))
						key.fPitchOffset = XMConvertToRadians(pitchDeg);

					if (ImGui::SliderFloat(("Yaw Offset##" + label).c_str(),
						&yawDeg, -180.f, 180.f, "%.1f deg"))
						key.fYawOffset = XMConvertToRadians(yawDeg);
				}
				else if (key.eMode == KEYFRAME_MODE::PLAYER_OFFSET || key.eMode == KEYFRAME_MODE::YAKUMO_OFFSET) // 통합
				{
					// 모드에 따라 안내 텍스트 분기
					if (key.eMode == KEYFRAME_MODE::PLAYER_OFFSET)
						ImGui::SeparatorText("Offset from Player");
					else
						ImGui::SeparatorText("Offset from Yakumo"); // 추가됨

					ImGui::TextDisabled("X = Right, Y = Up, Z = Forward(+)/Back(-)");
					ImGui::DragFloat3(("Pos Offset##" + label).c_str(),
						&key.vPosOffset.x, 0.1f, -10.f, 10.f, "%.2f");

					const char* szLookAtNames[] = { "Custom Target", "Player", "Yakumo" };
					_int iLookAt = static_cast<_int>(key.eLookAt);
					if (ImGui::Combo(("LookAt##" + label).c_str(), &iLookAt, szLookAtNames, IM_ARRAYSIZE(szLookAtNames)))
						key.eLookAt = static_cast<CINEMATIC_LOOKAT>(iLookAt);

					if (key.eLookAt == CINEMATIC_LOOKAT::CUSTOM_TARGET)
					{
						ImGui::DragFloat3(("LookAt Target##" + label).c_str(),
							&key.vlookAtTarget.x, 0.1f, -100.f, 100.f, "%.2f");
					}
				}
				else // ABSOLUTEMODE
				{
					ImGui::SeparatorText("World Position");
					ImGui::DragFloat3(("World Pos##" + label).c_str(),
						&key.vAbsPosition.x, 0.1f, -100.f, 100.f, "%.2f");

					const char* szLookAtNames[] = { "Custom Target", "Player", "Yakumo" };
					_int iLookAt = static_cast<_int>(key.eLookAt);
					if (ImGui::Combo(("LookAt##" + label).c_str(), &iLookAt, szLookAtNames, IM_ARRAYSIZE(szLookAtNames)))
						key.eLookAt = static_cast<CINEMATIC_LOOKAT>(iLookAt);

					if (key.eLookAt == CINEMATIC_LOOKAT::CUSTOM_TARGET)
					{
						ImGui::DragFloat3(("LookAt Target##" + label).c_str(),
							&key.vlookAtTarget.x, 0.1f, -100.f, 100.f, "%.2f");
					}
				}

				// ==================== FOV ====================
				_float fovDeg = (key.fFov > 0.f) ? XMConvertToDegrees(key.fFov) : 0.f;
				if (ImGui::SliderFloat(("FOV##" + label).c_str(),
					&fovDeg, 0.f, 120.f, fovDeg > 0.f ? "%.2f deg" : "Stay Current"))
					key.fFov = (fovDeg > 0.f) ? XMConvertToRadians(fovDeg) : 0.f;

				// ==================== Shake ====================
				// 수정됨 : 이벤트 타입 이름 배열에 추가
				static _int iNewEventType = 0;
				const char* szEventTypeNames[] = {
					"Camera Shake",
					"Character Control",
					// 새 이벤트 타입 추가시 여기에 이름 추가
				};

				ImGui::Combo(("New Event Type##" + label).c_str(), &iNewEventType,
					szEventTypeNames, IM_ARRAYSIZE(szEventTypeNames));

				ImGui::SameLine();

				if (ImGui::Button(("Add Event##" + label).c_str()))
				{
					CINEMATIC_EVENT newEvent = {};
					newEvent.fTriggerTime = 0.f;
					newEvent.bFired = false;

					switch (static_cast<CINEMATIC_EVENT_TYPE>(iNewEventType))
					{
					case CINEMATIC_EVENT_TYPE::CAMERA_SHAKE:
						newEvent.eType = CINEMATIC_EVENT_TYPE::CAMERA_SHAKE;
						newEvent.tPayload = CameraShake{};
						break;
					case CINEMATIC_EVENT_TYPE::CHARACTER_CONTROL:
						newEvent.eType = CINEMATIC_EVENT_TYPE::CHARACTER_CONTROL;
						newEvent.tPayload = CINEMATIC_CHARACTER_CONTROLL{};
						break;
					default:
						break;
					}

					key.vecEvents.push_back(newEvent);
				}

				// 2. 현재 등록된 이벤트들 순회하며 UI 렌더링
				for (size_t evIdx = 0; evIdx < key.vecEvents.size(); ++evIdx)
				{
					auto& ev = key.vecEvents[evIdx];
					string evLabel = label + "_Ev" + to_string(evIdx);

					ImGui::PushID(evIdx);

					// 이벤트 타입 이름 출력 및 삭제 버튼
					string eventName = "Event " + to_string(evIdx) + " : ";
					if (ev.eType == CINEMATIC_EVENT_TYPE::CAMERA_SHAKE) eventName += "Camera Shake";
					else if (ev.eType == CINEMATIC_EVENT_TYPE::CHARACTER_CONTROL) eventName += "Character Control"; // 추가됨

					if (ImGui::TreeNode(eventName.c_str()))
					{
						ImGui::SliderFloat("Trigger Time", &ev.fTriggerTime, 0.f, key.fDuration, "%.2fs");

						if (ev.eType == CINEMATIC_EVENT_TYPE::CAMERA_SHAKE)
						{
							if (holds_alternative<CameraShake>(ev.tPayload))
							{
								auto& shake = get<CameraShake>(ev.tPayload);

								const char* szPriorityNames[] = { "NONE", "NORMAL_ATTACK", "STRONG_ATTACK", "EARTH_QUAKE" };
								_int iCurrentPriority = static_cast<_int>(shake.ePriority);

								ImGui::SeparatorText("Basic");
								ImGui::SliderFloat("Duration", &shake.fDuration, 0.05f, 5.f, "%.2f");
								ImGui::SliderFloat("BlendOut Time", &shake.fBlendOutTime, 0.f, 1.f, "%.3f");
								if (ImGui::Combo("Priority", &iCurrentPriority, szPriorityNames, IM_ARRAYSIZE(szPriorityNames)))
									shake.ePriority = static_cast<SHAKE_PRIORITY>(iCurrentPriority);

								ImGui::SeparatorText("Location Oscillation");
								ImGui::SliderFloat("Amp X (Forward)", &shake.fAmpX, 0.f, 2.f, "%.3f");
								ImGui::SliderFloat("Amp Y (Vertical)", &shake.fAmpY, 0.f, 2.f, "%.3f");
								ImGui::SliderFloat("Amp Z (Lateral)", &shake.fAmpZ, 0.f, 2.f, "%.3f");
								ImGui::SliderFloat("Freq X", &shake.fFreqX, 1.f, 200.f, "%.1f");
								ImGui::SliderFloat("Freq Y", &shake.fFreqY, 1.f, 200.f, "%.1f");
								ImGui::SliderFloat("Freq Z", &shake.fFreqZ, 1.f, 200.f, "%.1f");

								ImGui::SeparatorText("Rotation Oscillation");
								_float ampPitchDeg = XMConvertToDegrees(shake.fAmpPitch);
								_float ampYawDeg = XMConvertToDegrees(shake.fAmpYaw);

								if (ImGui::SliderFloat("Amp Pitch (deg)", &ampPitchDeg, 0.f, 5.f, "%.2f"))
									shake.fAmpPitch = XMConvertToRadians(ampPitchDeg);

								if (ImGui::SliderFloat("Amp Yaw (deg)", &ampYawDeg, 0.f, 5.f, "%.2f"))
									shake.fAmpYaw = XMConvertToRadians(ampYawDeg);

								ImGui::SliderFloat("Freq Pitch", &shake.fFreqPitch, 1.f, 200.f, "%.1f");
								ImGui::SliderFloat("Freq Yaw", &shake.fFreqYaw, 1.f, 200.f, "%.1f");

								ImGui::SeparatorText("FOV Oscillation");
								ImGui::SliderFloat("Amp FOV", &shake.fAmpFov, 0.f, 5.f, "%.2f");
								ImGui::SliderFloat("Freq FOV", &shake.fFreqFov, 1.f, 200.f, "%.1f");

								ImGui::SeparatorText("Impact Kick");
								ImGui::SliderFloat("Kick Strength", &shake.fKickStrength, 0.f, 2.f, "%.3f");
								ImGui::SliderFloat("Kick Decay", &shake.fKickDecay, 1.f, 50.f, "%.1f");
								ImGui::DragFloat4("Kick Direction", &shake.vKickDirection.x, 0.01f, -1.f, 1.f, "%.3f");
							}
						}
						else if (ev.eType == CINEMATIC_EVENT_TYPE::CHARACTER_CONTROL)
						{
							if (holds_alternative<CINEMATIC_CHARACTER_CONTROLL>(ev.tPayload))
							{
								auto& control = get<CINEMATIC_CHARACTER_CONTROLL>(ev.tPayload);

								ImGui::InputInt("Event Number", reinterpret_cast<int*>(&control.iNumber));
								ImGui::DragFloat3("Position", &control.vPosition.x, 0.1f, -100.f, 100.f, "%.2f");
							}
						}

						// 이벤트 삭제 버튼
						if (ImGui::Button("Delete This Event"))
						{
							key.vecEvents.erase(key.vecEvents.begin() + evIdx);
							ImGui::TreePop();
							ImGui::PopID();
							break;
						}

						ImGui::TreePop();
					}
					ImGui::PopID();
				}

				ImGui::TreePop();

				// ==================== 조작 실행 ====================
				if (needInsert)
				{
					for (_int j = m_iTestKeyCount; j > i; --j)
						m_testKeyFrames[j] = m_testKeyFrames[j - 1];

					m_testKeyFrames[i] = {};
					m_testKeyFrames[i].fDuration = 0.3f;
					m_iTestKeyCount++;
					break;
				}
				if (needDuplicate)
				{
					for (_int j = m_iTestKeyCount; j > i + 1; --j)
						m_testKeyFrames[j] = m_testKeyFrames[j - 1];

					m_testKeyFrames[i + 1] = m_testKeyFrames[i];
					m_iTestKeyCount++;
					break;
				}
				if (needDelete)
				{
					for (_int j = i; j < m_iTestKeyCount - 1; ++j)
						m_testKeyFrames[j] = m_testKeyFrames[j + 1];

					m_testKeyFrames[m_iTestKeyCount - 1] = {};
					m_iTestKeyCount--;
					break;
				}
				if (needMoveUp)
				{
					CINEMATIC_KEYFRAME temp = m_testKeyFrames[i];
					m_testKeyFrames[i] = m_testKeyFrames[i - 1];
					m_testKeyFrames[i - 1] = temp;
					break;
				}
				if (needMoveDown)
				{
					CINEMATIC_KEYFRAME temp = m_testKeyFrames[i];
					m_testKeyFrames[i] = m_testKeyFrames[i + 1];
					m_testKeyFrames[i + 1] = temp;
					break;
				}
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		static char presetName[128] = "default";
		ImGui::InputText("Preset Name", presetName, IM_ARRAYSIZE(presetName));

		if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4.f, 24.f)))
		{
			string path = "../../DataFiles/CinematicPreset/" + string(presetName) + ".json";
			Save_CinematicPreset(path);
		}

		ImGui::SameLine();

		if (ImGui::Button("Load", ImVec2(-1.f, 24.f)))
		{
			string path = "../../DataFiles/CinematicPreset/" + string(presetName) + ".json";
			Load_CinematicPreset(path);
		}

		ImGui::Spacing();
		ImGui::Separator();

		if (ImGui::Button("Start Cinematic", ImVec2(-1.f, 30.f)))
		{
			vector<CINEMATIC_KEYFRAME> keys;
			for (_int i = 0; i < m_iTestKeyCount; ++i)
				keys.push_back(m_testKeyFrames[i]);

			CameraEvent event;
			event.eCameraState = CAMERA_STATE::CINEMATIC;
			event.vecKeyframes = keys;
			event.bCinematicAutoReturn = m_bTestAutoReturn;
			m_pGameInstance->Publish(event);


			/* 임시로 플레이어 상태 변경 */
			//Player* player = CAST(Player*)(m_pGameInstance->Get_Player());
			//player->Change_State(Player::SPECIALSUCK);
		}

		if (ImGui::TreeNode("Cinematic Preset"))
		{
			if (ImGui::Button("Parry"))
			{
				m_iTestKeyCount = 3;

				m_testKeyFrames[0] = {};
				m_testKeyFrames[0].eMode = KEYFRAME_MODE::RELATIVEMODE;
				m_testKeyFrames[0].fDuration = 0.f;

				m_testKeyFrames[1] = {};
				m_testKeyFrames[1].eMode = KEYFRAME_MODE::RELATIVEMODE;
				m_testKeyFrames[1].vPosOffset = { 1.5f, 0.3f, -1.0f };
				m_testKeyFrames[1].fYawOffset = XMConvertToRadians(25.f);
				m_testKeyFrames[1].fPitchOffset = XMConvertToRadians(-5.f);
				m_testKeyFrames[1].fDuration = 0.3f;

				CINEMATIC_EVENT shakeEvent = {};
				shakeEvent.eType = CINEMATIC_EVENT_TYPE::CAMERA_SHAKE;
				shakeEvent.fTriggerTime = 0.f; // 도착하자마자 실행

				CameraShake shake = {};
				shake.fDuration = 0.15f;
				shake.fAmpX = 0.04f;
				shake.fAmpY = 0.06f;
				shake.fFreqX = 18.f;
				shake.fFreqY = 14.f;
				shakeEvent.tPayload = shake;

				m_testKeyFrames[1].vecEvents.push_back(shakeEvent);

				m_testKeyFrames[2] = m_testKeyFrames[1];
				m_testKeyFrames[2].fDuration = 0.4f;
				m_testKeyFrames[2].vecEvents.clear(); // [수정됨] 이벤트 초기화
			}

			ImGui::SameLine();

			// ZoomIn 프레임은 shake가 없었으므로 기존 코드 유지
			if (ImGui::Button("ZoomIn"))
			{
				m_iTestKeyCount = 2;

				m_testKeyFrames[0] = {};
				m_testKeyFrames[0].eMode = KEYFRAME_MODE::RELATIVEMODE;
				m_testKeyFrames[0].fDuration = 0.f;

				m_testKeyFrames[1] = {};
				m_testKeyFrames[1].eMode = KEYFRAME_MODE::RELATIVEMODE;
				m_testKeyFrames[1].vPosOffset = { 0.f, 0.2f, 1.5f };
				m_testKeyFrames[1].fPitchOffset = XMConvertToRadians(-3.f);
				m_testKeyFrames[1].fFov = XMConvertToRadians(35.f);
				m_testKeyFrames[1].fDuration = 0.5f;
			}

			ImGui::SameLine();

			if (ImGui::Button("LOOK DOWN"))
			{
				m_iTestKeyCount = 3;

				m_testKeyFrames[0] = {};
				m_testKeyFrames[0].eMode = KEYFRAME_MODE::RELATIVEMODE;
				m_testKeyFrames[0].fDuration = 0.f;

				m_testKeyFrames[1] = {};
				m_testKeyFrames[1].eMode = KEYFRAME_MODE::PLAYER_OFFSET;
				m_testKeyFrames[1].vPosOffset = { 0.f, 2.5f, -3.f };
				m_testKeyFrames[1].eLookAt = CINEMATIC_LOOKAT::PLAYER;
				m_testKeyFrames[1].fDuration = 0.5f;

				m_testKeyFrames[2] = m_testKeyFrames[1];
				m_testKeyFrames[2].fDuration = 0.6f;
				m_testKeyFrames[2].vecEvents.clear(); // [수정됨] 이벤트 초기화
			}

			ImGui::TreePop();
		}
	}
}

void Client::IMGUI_Main::Save_CinematicPreset(const string& filePath)
{
	json root;
	root["keyCount"] = m_iTestKeyCount;
	root["autoReturn"] = m_bTestAutoReturn;

	json keyArray = json::array();
	for (_int i = 0; i < m_iTestKeyCount; ++i)
	{
		const auto& key = m_testKeyFrames[i];
		json k;

		k["mode"] = static_cast<_int>(key.eMode);
		k["duration"] = key.fDuration;
		k["ease"] = static_cast<_int>(key.eEaseType);
		k["fov"] = key.fFov;

		// RELATIVE
		k["posOffset"] = { key.vPosOffset.x, key.vPosOffset.y, key.vPosOffset.z };
		k["pitchOffset"] = key.fPitchOffset;
		k["yawOffset"] = key.fYawOffset;

		// ABSOLUTE
		k["absPosition"] = { key.vAbsPosition.x, key.vAbsPosition.y, key.vAbsPosition.z };
		k["lookAtTarget"] = { key.vlookAtTarget.x, key.vlookAtTarget.y, key.vlookAtTarget.z };
		k["lookAt"] = static_cast<_int>(key.eLookAt);

		json eventsArray = json::array();
		for (const auto& ev : key.vecEvents)
		{
			json eventJson;
			eventJson["type"] = static_cast<_int>(ev.eType);
			eventJson["triggerTime"] = ev.fTriggerTime;

			// std::variant 페이로드 타입 확인 및 저장
			if (holds_alternative<CameraShake>(ev.tPayload))
			{
				const auto& shake = get<CameraShake>(ev.tPayload);
				json s;
				s["duration"] = shake.fDuration;
				s["ampX"] = shake.fAmpX;
				s["ampY"] = shake.fAmpY;
				s["ampZ"] = shake.fAmpZ;
				s["freqX"] = shake.fFreqX;
				s["freqY"] = shake.fFreqY;
				s["freqZ"] = shake.fFreqZ;
				s["ampPitch"] = shake.fAmpPitch;
				s["ampYaw"] = shake.fAmpYaw;
				s["freqPitch"] = shake.fFreqPitch;
				s["freqYaw"] = shake.fFreqYaw;
				s["ampFov"] = shake.fAmpFov;
				s["freqFov"] = shake.fFreqFov;
				s["kickStrength"] = shake.fKickStrength;
				s["kickDecay"] = shake.fKickDecay;
				s["blendOut"] = shake.fBlendOutTime;
				s["kickDir"] = { shake.vKickDirection.x, shake.vKickDirection.y, shake.vKickDirection.z, shake.vKickDirection.w };
				s["priority"] = static_cast<_int>(shake.ePriority);

				eventJson["shakePayload"] = s;
			}
			if (holds_alternative<CINEMATIC_CHARACTER_CONTROLL>(ev.tPayload))
			{
				const auto& control = get<CINEMATIC_CHARACTER_CONTROLL>(ev.tPayload);
				json c;
				c["number"] = control.iNumber;
				c["position"] = { control.vPosition.x, control.vPosition.y, control.vPosition.z };
				eventJson["controlPayload"] = c;
			}
			// 향후 다른 이벤트(예: 사운드 재생, 파티클 생성 등)가 추가되면 여기에 else if 로 추가

			eventsArray.push_back(eventJson);
		}
		k["events"] = eventsArray;

		keyArray.push_back(k);
	}
	root["keyframes"] = keyArray;

	ofstream file(filePath);
	if (file.is_open())
	{
		file << root.dump(2);
		file.close();
	}
}

void Client::IMGUI_Main::Load_CinematicPreset(const string& filePath)
{
	ifstream file(filePath);
	if (!file.is_open())
		return;

	json root;
	file >> root;
	file.close();

	m_iTestKeyCount = root.value("keyCount", 2);
	m_bTestAutoReturn = root.value("autoReturn", true);

	if (m_iTestKeyCount > MAX_TEST_KEYFRAMES)
		m_iTestKeyCount = MAX_TEST_KEYFRAMES;

	// 초기화
	for (_int i = 0; i < MAX_TEST_KEYFRAMES; ++i)
		m_testKeyFrames[i] = {};

	const auto& keyArray = root["keyframes"];
	for (_int i = 0; i < m_iTestKeyCount && i < (_int)keyArray.size(); ++i)
	{
		const auto& k = keyArray[i];
		auto& key = m_testKeyFrames[i];

		key.eMode = static_cast<KEYFRAME_MODE>(k.value("mode", 0));
		key.fDuration = k.value("duration", 0.f);
		key.eEaseType = static_cast<EASE_TYPE>(k.value("ease", 0));
		key.fFov = k.value("fov", 0.f);

		// RELATIVE
		if (k.contains("posOffset"))
		{
			key.vPosOffset.x = k["posOffset"][0];
			key.vPosOffset.y = k["posOffset"][1];
			key.vPosOffset.z = k["posOffset"][2];
		}
		key.fPitchOffset = k.value("pitchOffset", 0.f);
		key.fYawOffset = k.value("yawOffset", 0.f);

		// ABSOLUTE
		if (k.contains("absPosition"))
		{
			key.vAbsPosition.x = k["absPosition"][0];
			key.vAbsPosition.y = k["absPosition"][1];
			key.vAbsPosition.z = k["absPosition"][2];
		}
		if (k.contains("lookAtTarget"))
		{
			key.vlookAtTarget.x = k["lookAtTarget"][0];
			key.vlookAtTarget.y = k["lookAtTarget"][1];
			key.vlookAtTarget.z = k["lookAtTarget"][2];
		}
		if (k.contains("lookAt"))
		{
			key.eLookAt = CAST(CINEMATIC_LOOKAT)(k.value("lookAt", 0));
		}
		else if (k.contains("lookAtPlayer"))
		{
			key.eLookAt = k.value("lookAtPlayer", false)
				? CINEMATIC_LOOKAT::PLAYER
				: CINEMATIC_LOOKAT::CUSTOM_TARGET;
		}

		if (k.contains("events"))
		{
			for (const auto& evJson : k["events"])
			{
				CINEMATIC_EVENT ev = {};
				ev.eType = static_cast<CINEMATIC_EVENT_TYPE>(evJson.value("type", 0));
				ev.fTriggerTime = evJson.value("triggerTime", 0.f);
				ev.bFired = false;

				// 타입에 따른 페이로드 복원
				if (evJson.contains("shakePayload"))
				{
					CameraShake shake = {};
					const auto& s = evJson["shakePayload"];
					shake.fDuration = s.value("duration", 0.f);
					shake.fAmpX = s.value("ampX", 0.f);
					shake.fAmpY = s.value("ampY", 0.f);
					shake.fAmpZ = s.value("ampZ", 0.f);
					shake.fFreqX = s.value("freqX", 0.f);
					shake.fFreqY = s.value("freqY", 0.f);
					shake.fFreqZ = s.value("freqZ", 0.f);
					shake.fAmpPitch = s.value("ampPitch", 0.f);
					shake.fAmpYaw = s.value("ampYaw", 0.f);
					shake.fFreqPitch = s.value("freqPitch", 0.f);
					shake.fFreqYaw = s.value("freqYaw", 0.f);
					shake.fAmpFov = s.value("ampFov", 0.f);
					shake.fFreqFov = s.value("freqFov", 0.f);
					shake.fKickStrength = s.value("kickStrength", 0.f);
					shake.fKickDecay = s.value("kickDecay", 0.f);
					shake.fBlendOutTime = s.value("blendOut", 0.f);

					if (s.contains("kickDir"))
					{
						shake.vKickDirection.x = s["kickDir"][0];
						shake.vKickDirection.y = s["kickDir"][1];
						shake.vKickDirection.z = s["kickDir"][2];
						shake.vKickDirection.w = s["kickDir"][3];
					}
					shake.ePriority = CAST(SHAKE_PRIORITY)(s.value("priority", 0));

					ev.tPayload = shake;
				}
				if (evJson.contains("controlPayload"))
				{
					CINEMATIC_CHARACTER_CONTROLL control = {};
					const auto& c = evJson["controlPayload"];
					control.iNumber = c.value("number", 0u);
					if (c.contains("position"))
					{
						control.vPosition.x = c["position"][0];
						control.vPosition.y = c["position"][1];
						control.vPosition.z = c["position"][2];
					}
					ev.tPayload = control;
				}

				key.vecEvents.push_back(ev);
			}
		}
	}
}
/******************************************************* 카메라 흔들림 테스트 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
_int Client::IMGUI_Main::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
IMGUI_Main* Client::IMGUI_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg)
{
	IMGUI_Main* pInstance = new IMGUI_Main(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(arg), L"IMGUI_Main 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::IMGUI_Main::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/
