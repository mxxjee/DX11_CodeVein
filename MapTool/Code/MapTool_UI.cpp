#include "MT_Defines.h"
#include "MapTool_UI.h"

// Core Engine Headers
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "Camera_Object.h"
#include "MapObject.h"

// Components & Objects
#include "StaticObj.h"
#include "Mesh.h"
#include "Monster.h"
#include "Level_Main.h"
#include "Monster_EventShape.h"
#include "Item.h"
#include "Item_Box.h"

#pragma region Player Includes
#include "Player.h"
#include "Player_MasterRig.h"
#pragma endregion

using json = nlohmann::json;
namespace fs = std::filesystem;

unsigned int g_iGlobalObjCount = 0;

namespace
{
	// 내부 링크용 헬퍼 함수
	_string W2S(const _wstring& wstr) {
		if (wstr.empty()) return "";
		return _string(wstr.begin(), wstr.end());
	}

	_wstring S2W(const _string& str) {
		if (str.empty()) return L"";
		return _wstring(str.begin(), str.end());
	}
}

CMapTool_UI::CMapTool_UI(ID3D11Device* pD, ID3D11DeviceContext* pC)
	: ImguiWindow(pD, pC)
{
}

HRESULT CMapTool_UI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	m_pGameInstance = GameInstance::GetInstance();
	if (nullptr != m_pGameInstance)
		Safe_AddRef(m_pGameInstance);

	m_strCurrentPath = "../../DataFiles/Map_Data/MapData/";

	return S_OK;
}

_uint CMapTool_UI::Update_Contents(_float fDT)
{
	static char		szSaveFileName[MAX_PATH] = "MapData";
	static int		iLoadType = 0;

	static int iSelectedTargetMap = 0;
	const char* szMapNames[] = { "BASE (Type: 1)", "UNDER (Type: 2)", "CHURCH (Type: 3)" };
	const char* szMapPaths[] = {
		"../../DataFiles/Level_Base",
		"../../DataFiles/Level_Main",
		"../../DataFiles/Level_Church"
	};
	int iMapTypes[] = { 1, 2, 3 };

	static float	fPlayerSpeed = 30.0f;
	static float	fMonsterSpeed = 4.0f;

	static int		iCurrentFuncIndex = 0;
	static int		iLadderLength = 6;
	static float	fLadderRot = 0.f;

	if (ImGui::CollapsingHeader("Map Data System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		// UI 최상단에 통합 맵 선택 콤보박스 배치
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.f), "Target Map (Save & Load)");
		ImGui::Combo("##TargetMap", &iSelectedTargetMap, szMapNames, IM_ARRAYSIZE(szMapNames));

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Save Map Data");
		ImGui::InputText("File Name", szSaveFileName, MAX_PATH);

		if (ImGui::Button("Save to JSON", ImVec2(ImGui::GetContentRegionAvail().x, 30)))
		{
			_string strPath = _string(szMapPaths[iSelectedTargetMap]) + "/" + _string(szSaveFileName) + ".json";
			_string strSpPath = "../../DataFiles/Level_All/" + _string(szSaveFileName) + "_SP.json";
			_string strTriggerPath = _string(szMapPaths[iSelectedTargetMap]) + "/" + _string(szSaveFileName) + "_Trigger.json";
			_string strItemPath = _string(szMapPaths[iSelectedTargetMap]) + "/" + _string(szSaveFileName) + "_Item.json";

			SAVEEVENT data;
			data.bSaveJson = true;
			data.eToolType = TOOLTYPE::MAP_TOOL;
			data.strSaveFilePath = strPath;
			m_pGameInstance->Publish<SAVEEVENT>(data);

			Save_LayerSP_To_Json(strSpPath);
			Save_LayerTrigger_To_Json(strTriggerPath);
			Save_LayerItem_To_Json(strItemPath);

			MSG_ON(L"저장 완료", L"성공");
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Load Combination Data");
		if (ImGui::Button("Load Level & SP", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
			COUT("조합 로드");

			m_pGameInstance->Spawn_LevelData(szMapPaths[iSelectedTargetMap], _UINT(LEVEL::TOOL), true);

			_string strSpPath = "../../DataFiles/Level_All/" + _string(szSaveFileName) + "_SP.json";
			Load_LayerSP_Json(strSpPath, iMapTypes[iSelectedTargetMap]);

			_string strTriggerPath = _string(szMapPaths[iSelectedTargetMap]) + "/" + _string(szSaveFileName) + "_Trigger.json";
			Load_LayerTrigger_Json(strTriggerPath);

			_string strItemPath = _string(szMapPaths[iSelectedTargetMap]) + "/" + _string(szSaveFileName) + "_Item.json";
			Load_LayerItem_Json(strItemPath);
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Load Map Data (Single Json)");

		const char* szLoadTypes[] = {
			"Static Object (Cullable)",
			"World Object (Non-Cullable)",
			"Visual Object",
			"BackGround Object"
		};
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Combo("##LoadType", &iLoadType, szLoadTypes, IM_ARRAYSIZE(szLoadTypes));

		ImGui::Spacing();

		ImGui::BeginChild("FileBrowser", ImVec2(0, 150), true);
		CMT_Utils::Render_File_Browser(m_strCurrentPath, ".json", [&](const fs::path& selectedFile)
			{
				_wstring wstrProto, wstrLayer;
				switch (iLoadType)
				{
				case 0: wstrProto = L"Prototype_GameObject_Static"; wstrLayer = L"Layer_Object"; break;
				case 1: wstrProto = L"Prototype_GameObject_NonCul"; wstrLayer = L"Layer_WorldObj"; break;
				case 2: wstrProto = L"Prototype_GameObject_Static"; wstrLayer = L"Layer_VisualObj"; break;
				case 3: wstrProto = L"Prototype_GameObject_Static"; wstrLayer = L"Layer_BackGroundObj"; break;
				}
				Load_Data_Json_W(selectedFile.string(), wstrProto, wstrLayer);
			});
		ImGui::EndChild();
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Actor Placement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.f), "Player Spawn");
		bool bIsPlayerMode = (g_eToolMode == TOOL_MODE::MAP_PLAYER);
		if (ImGui::Checkbox("Placement Mode (Player)", &bIsPlayerMode))
		{
			g_eToolMode = bIsPlayerMode ? TOOL_MODE::MAP_PLAYER : TOOL_MODE::NONE;
		}
		ImGui::SliderFloat("Player Speed", &fPlayerSpeed, 1.0f, 100.0f, "%.1f");

		if (g_eToolMode == TOOL_MODE::MAP_PLAYER) {
			ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Placing Player");
		}
	}

	static int iSelectedMapType = 1;
	static int iSelectedMapIndex = 0;
	static int iItemId = 5000;
	static int iItemSpawnType = 0;

	if (ImGui::CollapsingHeader("Function Object Placement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.f), "Function Objects");

		static const char* szFuncItems[] = { "Save Point", "Map Seal", "Ladder", "Trigger Collider", "Npc", "Item"};
		ImGui::Combo("Select Type", &iCurrentFuncIndex, szFuncItems, IM_ARRAYSIZE(szFuncItems));

		if (iCurrentFuncIndex == 0 || iCurrentFuncIndex == 1)
		{
			const char* szMapTypes[] = { "NONE", "ST00_BASE", "ST01_UNDER", "ST02_CHURCH", "ST03_", "ST04_", "END" };
			ImGui::Combo("Target Map", &iSelectedMapType, szMapTypes, IM_ARRAYSIZE(szMapTypes));

			ImGui::InputInt("Target Index", &iSelectedMapIndex);
			if (iSelectedMapIndex < 0) iSelectedMapIndex = 0;
		}

		if (iCurrentFuncIndex == 2) {
			ImGui::SliderInt("Ladder Length", &iLadderLength, 2, 30, "%d");
		}

		if (iCurrentFuncIndex == 5)
		{
			ImGui::InputInt("Item ID", &iItemId);
			ImGui::RadioButton("Direct Item", &iItemSpawnType, 0); ImGui::SameLine();
			ImGui::RadioButton("Box Item", &iItemSpawnType, 1);
		}

		bool bIsFuncMode = (g_eToolMode == TOOL_MODE::MAP_FUNCTION);
		if (ImGui::Checkbox("Placement Mode (Function)", &bIsFuncMode))
		{
			g_eToolMode = bIsFuncMode ? TOOL_MODE::MAP_FUNCTION : TOOL_MODE::NONE;
		}

		if (g_eToolMode == TOOL_MODE::MAP_FUNCTION) {
			ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " Placing: %s", szFuncItems[iCurrentFuncIndex]);
		}
	}

	if (!ImGui::GetIO().WantCaptureMouse && m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
	{
		if (g_eToolMode == TOOL_MODE::MAP_FUNCTION && iCurrentFuncIndex == 2)
		{
			_float3 vHitPos, vHitNormal;
			if (Picking_PhysX(&vHitPos, &vHitNormal))
			{
				_float4 vPickPos = _float4(vHitPos.x, vHitPos.y, vHitPos.z, 1.f);
				_float fYawRadian = atan2(vHitNormal.x, vHitNormal.z) + XM_PI;
				Spawn_Ladder_At(vPickPos, (_float)iLadderLength, fYawRadian);
			}
		}
		else
		{
			_float4 vPickPos;
			if (m_pGameInstance->PickingObject(&vPickPos) && vPickPos.w != 0.f)
			{
				switch (g_eToolMode)
				{
				case TOOL_MODE::MAP_PLAYER:
					Spawn_Player(vPickPos, fPlayerSpeed);
					g_eToolMode = TOOL_MODE::NONE;
					break;
				case TOOL_MODE::MAP_FUNCTION:
				{
					_wstring wstrProtoTag = L"";
					switch (iCurrentFuncIndex)
					{
					case 0: Spawn_Function_At(vPickPos, L"Prototype_GameObject_SavePoint", (MAP_TYPE)iSelectedMapType, (_uint)iSelectedMapIndex); break;
					case 1: Spawn_Function_At(vPickPos, L"Prototype_GameObject_MapSeal", (MAP_TYPE)iSelectedMapType, (_uint)iSelectedMapIndex); break;
					case 5: Spawn_Item_At(vPickPos, iItemId, iItemSpawnType); break;
					default: wstrProtoTag = L"Prototype_GameObject_SavePoint"; break;
					}
				}
				break;
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("PhysX Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Draw_PhysX_Debug();
	}

	return 0;
}

void CMapTool_UI::Spawn_Function_At(_float4 vPickPos, _wstring ObjName, MAP_TYPE eType, _uint iIndex)
{
	MapObject::MAPOBJECT_DESC desc;
	desc.vPosition = _float4(vPickPos.x, vPickPos.y, vPickPos.z, 1.f);

	desc.jExtraData["MyMapType"] = eType;
	desc.jExtraData["MyIndex"] = iIndex;

	_wstring wstrLayerTag = L"Layer_Function";

	if (ObjName == L"Prototype_GameObject_SavePoint")
	{
		wstrLayerTag = L"Layer_SP";
	}

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(UINT(LEVEL::TOOL), ObjName,
		UINT(LEVEL::TOOL), wstrLayerTag, nullptr, &desc)))
	{
		MSG_BOX("객체 생성 실패");
	}
}

void CMapTool_UI::Spawn_Ladder_At(_float4 vPickPos, _float fLength, _float fRotY)
{
	MapObject::MAPOBJECT_DESC desc;
	desc.vPosition = _float4(vPickPos.x, vPickPos.y, vPickPos.z, 1.f);
	desc.vRotation = { 0.f, fRotY, 0.f};

	desc.jExtraData["LadderLength"] = fLength;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(UINT(LEVEL::TOOL), L"Prototype_GameObject_Ladder",
		UINT(LEVEL::TOOL), L"Layer_Function", nullptr, &desc)))
	{
		MSG_BOX("사다리 생성 실패");
	}
}

void CMapTool_UI::Spawn_Player(_float4 vPickPos, _float fSpeed)
{
	CMonster::MONSTER_DESC desc{};
	////지상_지하도시
	desc.vPosition = { vPickPos.x, vPickPos.y + 5.f, vPickPos.z, vPickPos.w };

	desc.fSpeed = fSpeed;
	desc.fRotationSpeed = XMConvertToRadians(90.f);

	PHYSX_CONTROLLER_DESC controllerDesc;
	controllerDesc.vPosition = { desc.vPosition.x, desc.vPosition.y, desc.vPosition.z };
	controllerDesc.pOwner = nullptr;

	desc.tControllerDesc = controllerDesc;

	MSG_FAIL(FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::TOOL),
		L"Prototype_GameObject_Player", _UINT(LEVEL::TOOL), L"Layer_Player", nullptr, &desc)),
		L"몬스터 생성 실패", L"실패");

	GameObject* pNewPlayer = m_pGameInstance->Get_GameObject(UINT(LEVEL::TOOL), L"Layer_Player", L"Player_0");
	if (!pNewPlayer) return;

	// 카메라 설정
	Camera* pCamera = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_00));
	if (nullptr == pCamera)
	{
		if (FAILED(m_pGameInstance->Add_Camera(UINT(LEVEL::TOOL), L"Prototype_GameObject_Camera_Object",
			UINT(LEVEL::TOOL), _UINT(CAMERA::PLAYER_FOCUS_00))))
		{
			MSG_BOX("카메라 생성 실패");
			return;
		}
		pCamera = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_00));
	}

	if (auto* pFollowCam = dynamic_cast<CCamera_Object*>(pCamera))
	{
		pFollowCam->Set_Target(pNewPlayer);
	}
}

void CMapTool_UI::Spawn_Item_At(_float4 vPickPos, _uint iItemID, _uint iSpawnType)
{
	MapObject::MAPOBJECT_DESC desc;
	//desc.vPosition = _float4(vPickPos.x, vPickPos.y, vPickPos.z, 1.f);
	desc.vPosition = (iSpawnType == 1) ? _float4(vPickPos.x, vPickPos.y, vPickPos.z, 1.f) : _float4(vPickPos.x, vPickPos.y + 0.1f, vPickPos.z, 1.f);
	desc.jExtraData["ItemID"] = iItemID;
	_wstring wstrProtoTag = (iSpawnType == 1) ? L"Prototype_GameObject_Item_Box" : L"Prototype_GameObject_Item";
	_wstring wstrLayerTag = L"Layer_Item";

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(UINT(LEVEL::TOOL), wstrProtoTag,
		UINT(LEVEL::TOOL), wstrLayerTag, nullptr, &desc)))
	{
		MSG_BOX("아이템 객체 생성 실패");
	}
}
void CMapTool_UI::Load_Data_Json(const _string& strPath, const _wstring& wstrPrototype, const _wstring& wstrLayer)
{
	ifstream file(strPath);
	if (!file.is_open()) {
		MessageBoxA(nullptr, "맵 파일 없음", "System Message", MB_OK);
		return;
	}

	json root;
	try { file >> root; }
	catch (const json::parse_error&) {
		MessageBoxA(nullptr, "JSON 파싱 에러", "System Message", MB_OK);
		return;
	}

	if (!root.is_array()) {
		MessageBoxA(nullptr, "올바르지 않은 포맷 (Array 필요)", "System Message", MB_OK);
		return;
	}

	_uint iSuccessCount = 0;
	const _float UNIT_SCALE = 0.01f;

	bool isModFile = (fs::path(strPath).filename().string().find("MOD_") == 0);

	for (auto& objNode : root)
	{
		if (!objNode.contains("MeshName") || !objNode.contains("Translation")) continue;

		_string meshName = objNode["MeshName"].get<_string>();
		_string strTag = isModFile ? meshName : "Prototype_Component_Model_" + meshName;

		auto& trans = objNode["Translation"];
		auto& rot = objNode["Rotation"];
		auto& scl = objNode["Scale"];

		MapObject::MAPOBJECT_DESC desc{};
		desc.wstrShaderName = L"Prototype_Component_Shader_VTXMeshInstance";
		desc.vPosition = _float4(
			trans["X"].get<_float>() * UNIT_SCALE,
			trans["Y"].get<_float>() * UNIT_SCALE,
			trans["Z"].get<_float>() * UNIT_SCALE,
			1.f
		);

		if (rot.contains("W")) {
			desc.vRotationQuat = { rot["X"], rot["Y"], rot["Z"], rot["W"] };
		}
		else if (rot.contains("Pitch")) {
			_float fPitch = XMConvertToRadians(rot["Pitch"]);
			_float fYaw = XMConvertToRadians(rot["Yaw"]);
			_float fRoll = XMConvertToRadians(rot["Roll"]);

			_matrix matRot = XMMatrixRotationZ(fRoll) * XMMatrixRotationX(fPitch) * XMMatrixRotationY(fYaw - XMConvertToRadians(180.f));
			XMStoreFloat4(&desc.vRotationQuat, XMQuaternionRotationMatrix(matRot));
		}
		else {
			XMStoreFloat4(&desc.vRotationQuat, XMQuaternionIdentity());
		}

		desc.vScale = _float3(scl["X"], scl["Y"], scl["Z"]);
		desc.prototypename = S2W(strTag);
		desc.wstrName = L"Static_" + std::to_wstring(g_iGlobalObjCount++);

		if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer((_uint)LEVEL::TOOL, wstrPrototype,
			(_uint)LEVEL::TOOL, wstrLayer, nullptr, &desc)))
		{
			iSuccessCount++;
		}
	}

	file.close();
	std::string strResult = std::to_string(iSuccessCount) + "개 로드 완료!";
	MessageBoxA(nullptr, strResult.c_str(), "System Message", MB_OK);
}

void CMapTool_UI::Load_Data_Json_W(const _string& strPath, const _wstring& wstrPrototype, const _wstring& wstrLayer)
{
	ifstream file(strPath);
	if (!file.is_open()) {
		MessageBoxA(nullptr, "맵 파일 없음", "System Message", MB_OK);
		return;
	}

	json root;
	try { file >> root; }
	catch (const json::parse_error&) {
		MessageBoxA(nullptr, "JSON 파싱 에러", "System Message", MB_OK);
		return;
	}

	if (!root.is_array()) {
		MessageBoxA(nullptr, "올바르지 않은 포맷 (Array 필요)", "System Message", MB_OK);
		return;
	}

	_uint iSuccessCount = 0;
	const _float UNIT_SCALE = 0.01f;
	bool isModFile = (fs::path(strPath).filename().string().find("MOD_") == 0);

	for (auto& objNode : root)
	{
		if (!objNode.contains("MeshName")) continue;

		_string meshName = objNode["MeshName"].get<_string>();
		_string strTag = isModFile ? meshName : "Prototype_Component_Model_" + meshName;

		MapObject::MAPOBJECT_DESC desc{};
		desc.wstrShaderName = L"Prototype_Component_Shader_VTXMeshInstance";
		desc.prototypename = S2W(strTag);
		desc.wstrName = L"Static_" + std::to_wstring(g_iGlobalObjCount++);

		if (objNode.contains("ExtraData"))
		{
			desc.jExtraData = objNode["ExtraData"];
		}

		// WorldMatrix 형식
		if (objNode.contains("WorldMatrix"))
		{
			auto& matArr = objNode["WorldMatrix"];
			if (matArr.size() != 16) continue;

			_float4x4 matWorld{};
			for (_uint i = 0; i < 16; ++i)
				matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

			matWorld._41 *= UNIT_SCALE;
			matWorld._42 *= UNIT_SCALE;
			matWorld._43 *= UNIT_SCALE;

			desc.bSetWorldPos = true;
			desc.matWorldPos = matWorld;
		}
		else if (objNode.contains("Translation"))
		{
			auto& trans = objNode["Translation"];
			auto& rot = objNode["Rotation"];
			auto& scl = objNode["Scale"];

			_vector vTrans = XMVectorSet(
				trans["X"].get<_float>() * UNIT_SCALE,
				trans["Y"].get<_float>() * UNIT_SCALE,
				trans["Z"].get<_float>() * UNIT_SCALE,
				1.f
			);

			_vector vScale = XMVectorSet(
				scl["X"].get<_float>(),
				scl["Y"].get<_float>(),
				scl["Z"].get<_float>(),
				0.f
			);

			_vector vRotQuat;
			if (rot.contains("W"))
			{
				vRotQuat = XMVectorSet(rot["X"], rot["Y"], rot["Z"], rot["W"]);
			}
			else if (rot.contains("Pitch"))
			{
				_float fPitch = XMConvertToRadians(rot["Pitch"]);
				_float fYaw = XMConvertToRadians(rot["Yaw"]);
				_float fRoll = XMConvertToRadians(rot["Roll"]);
				_matrix matRot = XMMatrixRotationZ(fRoll) * XMMatrixRotationX(fPitch) * XMMatrixRotationY(fYaw - XMConvertToRadians(180.f));
				vRotQuat = XMQuaternionRotationMatrix(matRot);
			}
			else
			{
				vRotQuat = XMQuaternionIdentity();
			}

			_matrix matWorld = XMMatrixAffineTransformation(vScale, XMVectorZero(), vRotQuat, vTrans);

			desc.bSetWorldPos = true;
			XMStoreFloat4x4(&desc.matWorldPos, matWorld);
		}
		else continue;

		if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer((_uint)LEVEL::TOOL, wstrPrototype,
			(_uint)LEVEL::TOOL, wstrLayer, nullptr, &desc)))
		{
			iSuccessCount++;
		}
	}

	file.close();
	std::string strResult = std::to_string(iSuccessCount) + "개 로드 완료!";
	MessageBoxA(nullptr, strResult.c_str(), "System Message", MB_OK);
}

CMapTool_UI* CMapTool_UI::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg)
{
	CMapTool_UI* pIns = new CMapTool_UI(pD, pC);
	if (FAILED(pIns->Initialize(pArg)))
	{
		MSG_BOX("imgui_Tool_Controler 생성 실패");
		Safe_Release(pIns);
	}
	return pIns;
}

void CMapTool_UI::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}

_bool CMapTool_UI::Picking_PhysX(_float3* pOutPos, _float3* pOutNormal)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pGameInstance->Get_Hwnd(), &ptMouse);

	_float fWinCX, fWinCY;
	m_pGameInstance->Get_Winsize(&fWinCX, &fWinCY);

	_float fX = ((2.0f * ptMouse.x) / fWinCX) - 1.0f;
	_float fY = -((2.0f * ptMouse.y) / fWinCY) + 1.0f;

	_float4x4 matProjInvFloat = m_pGameInstance->Get_PipeLineInversMatrix(D3DTRANSFORM::D3DTS_PROJ);
	_float4x4 matViewInvFloat = m_pGameInstance->Get_PipeLineInversMatrix(D3DTRANSFORM::D3DTS_VIEW);

	_matrix matProjInv = XMLoadFloat4x4(&matProjInvFloat);
	_matrix matViewInv = XMLoadFloat4x4(&matViewInvFloat);

	_vector vMousePos = XMVectorSet(fX, fY, 1.0f, 1.0f);
	vMousePos = XMVector3TransformCoord(vMousePos, matProjInv);
	vMousePos = XMVector3TransformCoord(vMousePos, matViewInv);

	_vector vCamPos = matViewInv.r[3];
	_vector vMouseDir = XMVector3Normalize(vMousePos - vCamPos);

	physx::PxVec3 pxOrigin(vCamPos.m128_f32[0], vCamPos.m128_f32[1], vCamPos.m128_f32[2]);
	physx::PxVec3 pxDir(vMouseDir.m128_f32[0], vMouseDir.m128_f32[1], vMouseDir.m128_f32[2]);
	physx::PxReal maxDistance = 1000.0f;
	physx::PxRaycastBuffer hit;

	if (m_pGameInstance->Get_Scene()->raycast(pxOrigin, pxDir, maxDistance, hit))
	{
		if (pOutPos)	*pOutPos = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
		if (pOutNormal) *pOutNormal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
		return true;
	}

	return false;
}

void CMapTool_UI::Save_LayerSP_To_Json(const _string& strFilePath)
{
	nlohmann::ordered_json root;
	ifstream inFile(strFilePath);

	if (inFile.is_open())
	{
		try { inFile >> root; }
		catch (...) {}
		inFile.close();
	}

	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	tm tm = {};
	localtime_s(&tm, &time);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
	_string strCurrentTime = oss.str();

	if (!root.contains("data") || !root["data"].is_array())
	{
		root["header"]["version"] = "1.0.0";
		root["header"]["toolType"] = "MAP_TOOL";
		root["header"]["createdAt"] = strCurrentTime;
		root["header"]["modifiedAt"] = strCurrentTime;
		root["data"] = nlohmann::json::array();
	}
	else
	{
		root["header"]["modifiedAt"] = strCurrentTime;
	}

	Layer* pSPLayer = m_pGameInstance->Get_Layer(L"Layer_SP");

	if (nullptr == pSPLayer)
		return;

	auto& SPMap = pSPLayer->Get_GameObjects();

	for (auto& pair : SPMap)
	{
		GameObject* pObj = pair.second;
		MapObject* pMapObj = dynamic_cast<MapObject*>(pObj);
		if (!pMapObj) continue;

		_float4x4 matWorld;
		XMStoreFloat4x4(&matWorld, pMapObj->Get_WorldMatrix());

		json jExtra = pMapObj->Get_ExtraData();
		int iMapType = jExtra["MyMapType"];
		int iIndex = jExtra["MyIndex"];

		bool bFoundAndUpdated = false;

		for (auto& item : root["data"])
		{
			if (item.contains("ExtraData") &&
				item["ExtraData"]["MyMapType"] == iMapType &&
				item["ExtraData"]["MyIndex"] == iIndex)
			{
				item["WorldMatrix"] = {
					matWorld._11, matWorld._12, matWorld._13, matWorld._14,
					matWorld._21, matWorld._22, matWorld._23, matWorld._24,
					matWorld._31, matWorld._32, matWorld._33, matWorld._34,
					matWorld._41, matWorld._42, matWorld._43, matWorld._44
				};
				bFoundAndUpdated = true;
				break;
			}
		}

		if (!bFoundAndUpdated)
		{
			json newItem;
			newItem["ObjectKey"] = "Model/Map_Static/SavePoint/SavePoint";
			newItem["PrototypeName"] = "Prototype_GameObject_SavePoint";
			newItem["LayerName"] = "Layer_SP";
			newItem["ObjectType"] = 1;
			newItem["WorldMatrix"] = {
				matWorld._11, matWorld._12, matWorld._13, matWorld._14,
				matWorld._21, matWorld._22, matWorld._23, matWorld._24,
				matWorld._31, matWorld._32, matWorld._33, matWorld._34,
				matWorld._41, matWorld._42, matWorld._43, matWorld._44
			};

			newItem["ExtraData"] = jExtra;

			root["data"].push_back(newItem);
		}
	}

	ofstream outFile(strFilePath);
	if (outFile.is_open())
	{
		outFile << root.dump(4);
		outFile.close();
	}
}

void CMapTool_UI::Load_LayerSP_Json(const _string& strFilePath, int iTargetMapType)
{
	ifstream file(strFilePath);

	if (!file.is_open()) return;

	json root;
	try { file >> root; }
	catch (const json::parse_error&) {
		MessageBoxA(nullptr, "SP JSON 파싱 에러", "System Message", MB_OK);
		return;
	}

	if (!root.contains("data") || !root["data"].is_array()) return;

	_uint iSuccessCount = 0;

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
			wstrProto = S2W(objNode["PrototypeName"].get<std::string>());

		_wstring wstrLayer = L"Layer_SP";
		if (objNode.contains("LayerName"))
			wstrLayer = S2W(objNode["LayerName"].get<std::string>());

		if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer((_uint)LEVEL::TOOL, wstrProto,
			(_uint)LEVEL::TOOL, wstrLayer, nullptr, &desc)))
		{
			iSuccessCount++;
		}
	}

	std::string strResult = "SavePoint " + std::to_string(iSuccessCount) + "개 로드 완료!";

}

void CMapTool_UI::Save_LayerTrigger_To_Json(const _string& strFilePath)
{
	nlohmann::ordered_json root;

	// 파일이 이미 있다면 읽어와서 덮어쓰기 준비
	ifstream inFile(strFilePath);
	if (inFile.is_open()) {
		try { inFile >> root; }
		catch (...) {}
		inFile.close();
	}

	root["data"] = nlohmann::json::array(); // 초기화

	Layer* pTriggerLayer = m_pGameInstance->Get_Layer(L"Layer_Trigger");
	if (pTriggerLayer != nullptr)
	{
		for (auto& pair : pTriggerLayer->Get_GameObjects())
		{
			Monster_EventShape* pTrigger = dynamic_cast<Monster_EventShape*>(pair.second);
			if (!pTrigger) continue;

			_float4x4 matWorld;
			XMStoreFloat4x4(&matWorld, pTrigger->Get_Transform()->Get_WorldMatrix());

			nlohmann::ordered_json newItem;
			newItem["ObjectKey"] = "EventShape_MonsterSpawn";
			newItem["PrototypeName"] = "Prototype_GameObject_Monster_EventShape";
			newItem["LayerName"] = "Layer_Trigger";

			// 행렬 저장
			newItem["WorldMatrix"] = {
				matWorld._11, matWorld._12, matWorld._13, matWorld._14,
				matWorld._21, matWorld._22, matWorld._23, matWorld._24,
				matWorld._31, matWorld._32, matWorld._33, matWorld._34,
				matWorld._41, matWorld._42, matWorld._43, matWorld._44
			};

			// 스폰 리스트 데이터 저장
			newItem["ExtraData"] = pTrigger->Get_ExtraData();

			root["data"].push_back(newItem);
		}
	}

	ofstream outFile(strFilePath);
	if (outFile.is_open()) {
		outFile << root.dump(4);
		outFile.close();
	}
}

void CMapTool_UI::Load_LayerTrigger_Json(const _string& strFilePath)
{
	ifstream file(strFilePath);
	if (!file.is_open()) return;

	json root;
	try { file >> root; }
	catch (...) { return; }

	if (!root.contains("data") || !root["data"].is_array()) return;

	_uint iSuccessCount = 0;
	for (auto& objNode : root["data"])
	{
		if (!objNode.contains("WorldMatrix")) continue;

		// 월드 행렬 파싱
		auto& matArr = objNode["WorldMatrix"];
		_float4x4 matWorld{};
		for (_uint i = 0; i < 16; ++i)
			matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

		// 행렬에서 좌표크기 추출
		_matrix matW = XMLoadFloat4x4(&matWorld);
		_vector vScale, vRot, vPos;
		XMMatrixDecompose(&vScale, &vRot, &vPos, matW);

		Monster_EventShape::MonsterEventShapeDesc desc;
		desc.pOwner = nullptr;
		desc.eColliderType = COLLIDER::AABB;

		XMStoreFloat3(&desc.vOffSet, vPos); // 좌표 적용

		_float3 fScale;
		XMStoreFloat3(&fScale, vScale);
		desc.Extents = _float3(fScale.x * 0.5f, fScale.y * 0.5f, fScale.z * 0.5f); // 크기 적용

		// Json 데이터
		if (objNode.contains("ExtraData"))
		{
			desc.jExtraData = objNode["ExtraData"];
		}

		// 트리거 생성
		if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer(
			(_uint)LEVEL::TOOL, L"Prototype_GameObject_Monster_EventShape",
			(_uint)LEVEL::TOOL, L"Layer_Trigger", nullptr, &desc)))
		{
			iSuccessCount++;
		}
	}
	COUT(("[System] Trigger " + to_string(iSuccessCount) + "개 로드 완료!").c_str());
}

void CMapTool_UI::Save_LayerItem_To_Json(const _string& strFilePath)
{
	nlohmann::ordered_json root;

	ifstream inFile(strFilePath);
	if (inFile.is_open()) {
		try { inFile >> root; }
		catch (...) {}
		inFile.close();
	}

	root["data"] = nlohmann::json::array();

	Layer* pLayer = m_pGameInstance->Get_Layer(L"Layer_Item");
	if (pLayer != nullptr)
	{
		for (auto& pair : pLayer->Get_GameObjects())
		{
			MapObject* pMapObj = dynamic_cast<MapObject*>(pair.second);
			if (!pMapObj) continue;

			nlohmann::ordered_json newItem;
			newItem["LayerName"] = "Layer_Item";

			if (dynamic_cast<Item_Box*>(pair.second) != nullptr)
			{
				newItem["PrototypeName"] = "Prototype_GameObject_Item_Box";
				newItem["ObjectKey"] = "Model/Map_Static/Item/ItemBox_Body";
			}
			else if (dynamic_cast<Item*>(pair.second) != nullptr)
			{
				newItem["PrototypeName"] = "Prototype_GameObject_Item";
				newItem["ObjectKey"] = "Model/Map_Static/Item/Item_Debug";
			}
			else
			{
				continue;
			}

			_float4x4 matWorld;
			XMStoreFloat4x4(&matWorld, pMapObj->Get_Transform()->Get_WorldMatrix());

			newItem["WorldMatrix"] = {
				matWorld._11, matWorld._12, matWorld._13, matWorld._14,
				matWorld._21, matWorld._22, matWorld._23, matWorld._24,
				matWorld._31, matWorld._32, matWorld._33, matWorld._34,
				matWorld._41, matWorld._42, matWorld._43, matWorld._44
			};

			newItem["ExtraData"] = pMapObj->Get_ExtraData();

			root["data"].push_back(newItem);
		}
	}

	ofstream outFile(strFilePath);
	if (outFile.is_open()) {
		outFile << root.dump(4);
		outFile.close();
	}
}

void CMapTool_UI::Load_LayerItem_Json(const _string& strFilePath)
{
	ifstream file(strFilePath);
	if (!file.is_open()) return;

	json root;
	try { file >> root; }
	catch (...) { return; }

	if (!root.contains("data") || !root["data"].is_array()) return;

	_uint iSuccessCount = 0;
	for (auto& objNode : root["data"])
	{
		if (!objNode.contains("WorldMatrix")) continue;
		MapObject::MAPOBJECT_DESC pDesc;

		auto& matArr = objNode["WorldMatrix"];
		_float4x4 matWorld{};
		for (_uint i = 0; i < 16; ++i)
			matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

		pDesc.bSetWorldPos = true; 
		pDesc.matWorldPos = matWorld;

		if (objNode.contains("ExtraData"))
		{
			pDesc.jExtraData = objNode["ExtraData"];
		}

		_wstring wstrProtoName = L"Prototype_GameObject_Item";
		if (objNode.contains("PrototypeName"))
		{
			_string strName = objNode["PrototypeName"].get<string>();
			wstrProtoName = _wstring(strName.begin(), strName.end());
		}

		if (SUCCEEDED(m_pGameInstance->Add_GameObject_To_Layer(
			(_uint)LEVEL::TOOL, wstrProtoName,
			(_uint)LEVEL::TOOL, L"Layer_Item", nullptr, &pDesc)))
		{
			iSuccessCount++;
		}
	}
	COUT(("[System] Item " + to_string(iSuccessCount) + "개 로드 완료!").c_str());
}

#ifdef _DEBUG
void CMapTool_UI::Draw_PhysX_Debug()
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