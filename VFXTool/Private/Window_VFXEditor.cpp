#include "VFXTool_Define.h"
#include "Window_VFXEditor.h"
#include "BasicParticle.h"
#include "BasicMesh.h"
#include "VIBuffer_Particle.h"
#include "VIBuffer_Trail.h"
#include "GameInstance.h"
#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "MeshEffect.h"
#include "SubEmitter.h"
#include "TrailEffect.h"
#include "SwordTrail.h"
#include "VFX_Parsing.h"
#include "Model.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "Editor_Utils.h"

Window_VFXEditor::Window_VFXEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ImguiWindow { pDevice, pContext }
    , m_pGameInstance { GameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void Window_VFXEditor::Set_Parser(VFX_Parsing* pParser)
{
	m_pParser = pParser;
	Safe_AddRef(m_pParser);
}

_string Window_VFXEditor::ConvertToRelativePath(const _wstring& wstrFilePath)
{
	_string strPath = wstringToString(wstrFilePath);

	// 역슬래시를 슬래시로 변환
	for (auto& ch : strPath)
	{
		if (ch == '\\')
			ch = '/';
	}

	// DataFiles 키워드 찾기
	size_t charPos = strPath.find("DataFiles");
	if (charPos != string::npos)
		return "../../" + strPath.substr(charPos);

	return strPath;
}

_uint Window_VFXEditor::Update_Contents(_float fTimeDelta)
{
	/* ========================================= */
	/*           Top Toolbar                     */
	/* ========================================= */
	Render_TopToolbar();

	ImGui::Separator();

	/* ========================================= */
	/*      Left Panel (Hierarchy) + Right Panel */
	/* ========================================= */
	const float leftPanelWidth = 250.f;
	float availableWidth = ImGui::GetContentRegionAvail().x;
	float rightPanelWidth = availableWidth - leftPanelWidth - 10.f;

	// 왼쪽 패널: Hierarchy TreeView
	ImGui::BeginChild("LeftPanel_Hierarchy", ImVec2(leftPanelWidth, 0), true);
	{
		Render_TreeView();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	// 오른쪽 패널: Detail Panel
	ImGui::BeginChild("RightPanel_Detail", ImVec2(rightPanelWidth, 0), true);
	{
		Render_DetailPanel();
	}
	ImGui::EndChild();

	/* ========================================= */
	/*             Popup Modals                  */
	/* ========================================= */
	Render_AddSystemPopup();
	Render_AddEffectPopup();
	Render_AddSubEmitterPopup();
	Render_AddTrailEffectPopup();
	Render_AddMeshEffectPopup();

	return 0;
}

/* ============================================== */
/*             Top Toolbar Render                 */
/* ============================================== */
void Window_VFXEditor::Render_TopToolbar()
{
	// Playback Controls
	if (ImGui::Button("Play"))
	{
		// 선택된 ParticleSystem의 Play() 호출
		if (m_pSelectedSystem != nullptr)
		{
			m_pSelectedSystem->OnSpawn(nullptr);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Pause"))
	{
		// 선택된 ParticleSystem의 Pause() 호출
		if (m_pSelectedSystem != nullptr)
		{
			 m_pSelectedSystem->Pause();
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		// 선택된 ParticleSystem의 Stop() 호출
		if (m_pSelectedSystem != nullptr)
		{
			 m_pSelectedSystem->OnDespawn();
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		// 선택된 ParticleSystem의 Reset() 호출
		if (m_pSelectedSystem != nullptr)
		{
			 m_pSelectedSystem->Reset();
		}
	}

	ImGui::SameLine(0.f, 50.f);

	// File Operations
	if (ImGui::Button("Save"))
	{
		// 현재 ParticleSystem을 JSON으로 저장
		SAVEEVENT event;
		event.eToolType = TOOLTYPE::VFX_TOOL;
		event.strPath = "../../DataFiles/Effects/" + wstringToString(m_vecParticleSystems[0]->Get_Name());
		m_pGameInstance->Publish(event);
	}
	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		// JSON에서 ParticleSystem 로드
		// 파일 다이얼로그 열기 및 텍스처 교체
		TCHAR exePath[MAX_PATH];
		GetModuleFileName(nullptr, exePath, MAX_PATH);
		PathRemoveFileSpec(exePath);
		PathAppend(exePath, TEXT("..\\..\\DataFiles\\Effects"));

		OPENFILENAME ofn;
		TCHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = TEXT("Effect Files(*.json)\0*.json;\0All Files\0*.*\0");
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = exePath;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileName(&ofn) == TRUE)
		{
			_string strRelativePath = ConvertToRelativePath(szFile);

			LOADEVENT event;
			event.eToolType = TOOLTYPE::VFX_TOOL;
			event.m_Path = strRelativePath;

			m_pGameInstance->Publish(event);

			for (auto& pSystem : m_vecParticleSystems)
				Safe_Release(pSystem);
			m_vecParticleSystems.clear();

			auto& vecCreatedSystems = m_pParser->Get_CreatedSystems();
			for (auto& pSystem : vecCreatedSystems)
			{
				Safe_AddRef(pSystem);                                                        
				m_vecParticleSystems.push_back(pSystem);
			}

			// ImGui 선택 옵션 초기화
			m_eSelectionType = SELECTION_TYPE::NONE;
			m_pSelectedSystem = nullptr;
			m_pSelectedEffect = nullptr;
			m_iSelectedSystemIndex = -1;
			m_iSelectedEffectIndex = -1;
			m_iSelectedSubEmitterIndex = -1;
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("New"))
	{
		m_bShowAddSystemPopup = true;
	}
}

/* ============================================== */
/*          Hierarchy TreeView Render             */
/* ============================================== */
void Window_VFXEditor::Render_TreeView()
{
	ImGui::Text("Hierarchy");
	ImGui::Separator();

	// [+ Add System] 버튼
	if (ImGui::Button("+ Add System"))
	{
		m_bShowAddSystemPopup = true;
	}

	ImGui::Separator();

	// ParticleSystem 목록 순회
	for (_int iSystemIdx = 0; iSystemIdx < (_int)m_vecParticleSystems.size(); ++iSystemIdx)
	{
		ParticleSystem* pSystem = m_vecParticleSystems[iSystemIdx];
		if (pSystem == nullptr)
			continue;

		// TreeNode 플래그 설정
		ImGuiTreeNodeFlags systemFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (m_eSelectionType == SELECTION_TYPE::PARTICLE_SYSTEM && m_iSelectedSystemIndex == iSystemIdx)
			systemFlags |= ImGuiTreeNodeFlags_Selected;

		// ParticleSystem 이름 표시
		bool bSystemNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)iSystemIdx, systemFlags, wstringToString(pSystem->Get_Name()).c_str());

		// 클릭 시 선택
		if (ImGui::IsItemClicked())
		{
			m_eSelectionType = SELECTION_TYPE::PARTICLE_SYSTEM;
			m_iSelectedSystemIndex = iSystemIdx;
			m_iSelectedEffectIndex = -1;
			m_iSelectedSubEmitterIndex = -1;
			m_pSelectedSystem = pSystem;
			m_pSelectedEffect = nullptr;

			// pSystem에서 캐시 데이터 로드
			m_bCachedAutoPlay = pSystem->Is_AutoPlay();
			m_bCachedLoop = pSystem->Is_Loop();
			m_fCachedTotalDuration = pSystem->Get_TotalDuration();
			m_fCachedDelayedTime = pSystem->Get_DelayedTime();
			m_vSystemRotation = pSystem->Get_RotationSpeed();
		}

		if (bSystemNodeOpen)
		{
			// ParticleEffect 목록 순회
			_uint iEffectCount = pSystem->Get_EffectCount();
			for (_int iEffectIdx = 0; iEffectIdx < (_int)iEffectCount; ++iEffectIdx)
			{
				ImGuiTreeNodeFlags effectFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				bool bIsSelected = (m_eSelectionType == SELECTION_TYPE::PARTICLE_EFFECT ||
					m_eSelectionType == SELECTION_TYPE::TRAIL_EFFECT ||
					m_eSelectionType == SELECTION_TYPE::MESH_EFFECT) &&
					m_iSelectedSystemIndex == iSystemIdx &&
					m_iSelectedEffectIndex == iEffectIdx;
				if (bIsSelected)
					effectFlags |= ImGuiTreeNodeFlags_Selected;

				// 이펙트 타입에 따라 [Mesh] / [Trail] / [Particle] 접두어 및 색상 표시
				ParticleEffect* pTempEffect = pSystem->Find_EffectByIndex(iEffectIdx);
				const char* prefix = "[Particle] ";
				ImVec4 typeColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);		// Particle: 하늘색
				if (pTempEffect && DCAST(BasicMesh*)(pTempEffect) != nullptr)
				{
					prefix = "[Mesh] ";
					typeColor = ImVec4(0.4f, 1.0f, 0.6f, 1.0f);		// Mesh: 초록색
				}
				else if (pTempEffect && pTempEffect->Get_IsMeshEffect())
				{
					prefix = "[Trail] ";
					typeColor = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);		// Trail: 노란색
				}

				// 선택된 항목은 더 밝게
				if (bIsSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(typeColor.x * 0.4f, typeColor.y * 0.4f, typeColor.z * 0.4f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(typeColor.x * 0.5f, typeColor.y * 0.5f, typeColor.z * 0.5f, 1.0f));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, typeColor);
				}

				char szLabel[256];
				sprintf_s(szLabel, "%s%s", prefix, wstringToString(pSystem->Get_EffectNameByIndex(iEffectIdx)).c_str());

				bool bEffectNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(iSystemIdx * 1000 + iEffectIdx), effectFlags, szLabel);

				ImGui::PopStyleColor(bIsSelected ? 3 : 1);

				if (ImGui::IsItemClicked())
				{
					m_iSelectedSystemIndex = iSystemIdx;
					m_iSelectedEffectIndex = iEffectIdx;
					m_iSelectedSubEmitterIndex = -1;
					m_pSelectedSystem = pSystem;
					m_pSelectedEffect = pSystem->Find_EffectByIndex(m_iSelectedEffectIndex);

					// MeshEffect / TrailEffect / ParticleEffect 구분
					if (DCAST(BasicMesh*)(m_pSelectedEffect) != nullptr)
					{
						m_eSelectionType = SELECTION_TYPE::MESH_EFFECT;
						m_tMeshDesc = CAST(BasicMesh*)(m_pSelectedEffect)->Get_MeshDesc();
						m_wstrModelTag = CAST(BasicMesh*)(m_pSelectedEffect)->Get_ModelName();
						m_bMeshUseEdgeGlow = m_pSelectedEffect->Get_UseEdgeGlow();
					}
					else if (m_pSelectedEffect->Get_IsMeshEffect())
					{
						m_eSelectionType = SELECTION_TYPE::TRAIL_EFFECT;
						m_tTrailDesc = CAST(TrailEffect*)(m_pSelectedEffect)->Get_TrailDesc();
					}
					else
					{
						m_eSelectionType = SELECTION_TYPE::PARTICLE_EFFECT;
						m_tParticleDesc = m_pSelectedEffect->Get_ParticleDesc();
					}

					m_strTextureName = stringToWstring(m_pSelectedEffect->Get_TextureName());
					m_strMaskTextureName = stringToWstring(m_pSelectedEffect->Get_MaskTextureName());
					m_strAlphaMaskTextureName = stringToWstring(m_pSelectedEffect->Get_AlphaMaskTextureName());
					m_strNoiseTextureName = stringToWstring(m_pSelectedEffect->Get_NoiseTextureName());
					m_strShaderTag = stringToWstring(m_pSelectedEffect->Get_ShaderTag());
					m_strComputeShaderTag = stringToWstring(m_pSelectedEffect->Get_ShaderComputeTag());
					m_iPassIndex = m_pSelectedEffect->Get_PassIndex();
					m_fCachedStartDelay = pSystem->Get_StartDelay(m_iSelectedEffectIndex);
					m_bUseEdgeGlow = m_pSelectedEffect->Get_UseEdgeGlow();
					m_bCachedEffectLoop = m_pSelectedEffect->Get_EffectLoop();
					m_bCachedBackCulling = m_pSelectedEffect->Get_BackCulling();
					m_fCachedMaskIntensity = m_pSelectedEffect->Get_MaskIntensity();
					m_iMaskSampler = m_pSelectedEffect->Get_MaskSampler();
					m_vCachedRotation = m_pSelectedEffect->Get_EffectRotation();
					m_vCachedDiffuseScroll = m_pSelectedEffect->Get_DiffuseUVScroll();
					m_vCachedFadeInOut = m_pSelectedEffect->Get_FadeInOut();
					m_bCachedEmissive = m_pSelectedEffect->Get_IsEmissive();
					m_vCachedEffectOffset = m_pSelectedEffect->Get_EffectOffset();
				}

				if (bEffectNodeOpen)
				{
					// SubEmitter 항목들 표시
					ParticleEffect* pCurrentEffect = pSystem->Find_EffectByIndex(iEffectIdx);
					if (pCurrentEffect != nullptr)
					{
						SubEmitter* pSubEmitter = pCurrentEffect->Get_SubEmitter();
						if (pSubEmitter != nullptr)
						{
							_uint iEntryCount = pSubEmitter->Get_EmitterCount();
							for (_int iEntryIdx = 0; iEntryIdx < (_int)iEntryCount; ++iEntryIdx)
							{
								ImGuiTreeNodeFlags entryFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
								if (m_eSelectionType == SELECTION_TYPE::SUBEMITTER_ENTRY &&
									m_iSelectedSystemIndex == iSystemIdx &&
									m_iSelectedEffectIndex == iEffectIdx &&
									m_iSelectedSubEmitterIndex == iEntryIdx)
								{
									entryFlags |= ImGuiTreeNodeFlags_Selected;
								}

								char szEntryName[128];
								sprintf_s(szEntryName, "- SubEmitter_%d", iEntryIdx);

								ImGui::TreeNodeEx((void*)(intptr_t)(iSystemIdx * 100000 + iEffectIdx * 1000 + iEntryIdx),
									entryFlags, szEntryName);

								if (ImGui::IsItemClicked())
								{
									m_eSelectionType = SELECTION_TYPE::SUBEMITTER_ENTRY;
									m_iSelectedSystemIndex = iSystemIdx;
									m_iSelectedEffectIndex = iEffectIdx;
									m_iSelectedSubEmitterIndex = iEntryIdx;
									m_tSubEmitterDesc = *pSubEmitter->Get_EmitterDescByIndex(m_iSelectedSubEmitterIndex);
								}
							}
						}
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}
}

/* ============================================== */
/*            Detail Panel Render                 */
/* ============================================== */
void Window_VFXEditor::Render_DetailPanel()
{
	ImGui::Text("Detail Panel");
	ImGui::Separator();

	switch (m_eSelectionType)
	{
	case SELECTION_TYPE::PARTICLE_SYSTEM:
		Render_Detail_ParticleSystem();
		break;
	case SELECTION_TYPE::PARTICLE_EFFECT:
		Render_Detail_ParticleEffect();
		break;
	case SELECTION_TYPE::TRAIL_EFFECT:
		Render_Detail_TrailEffect();
		break;
	case SELECTION_TYPE::MESH_EFFECT:
		Render_Detail_MeshEffect();
		break;
	case SELECTION_TYPE::SUBEMITTER_ENTRY:
		Render_Detail_SubEmitter();
		break;
	case SELECTION_TYPE::NONE:
	default:
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "Select an item from the Hierarchy to edit.");
		break;
	}
}

/* ============================================== */
/*       Detail: ParticleSystem                   */
/* ============================================== */
void Window_VFXEditor::Render_Detail_ParticleSystem()
{
	ImGui::Text("ParticleSystem Settings");
	ImGui::Separator();

	if (m_pSelectedSystem == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "No ParticleSystem selected.");
		return;
	}

	// Name
	ImGui::Text("Name: %s", wstringToString(m_pSelectedSystem->Get_Name()).c_str());

	ImGui::Separator();

	// AutoPlay
	if (ImGui::Checkbox("AutoPlay", &m_bCachedAutoPlay))
	{
		m_pSelectedSystem->Set_AutoPlay(m_bCachedAutoPlay);
	}

	// Loop
	if (ImGui::Checkbox("Loop", &m_bCachedLoop))
	{
		m_pSelectedSystem->Set_Loop(m_bCachedLoop);
	}

	// TotalDuration
	if (ImGui::DragFloat("Total Duration", &m_fCachedTotalDuration, 0.1f, 0.1f, 100.f))
	{
		m_pSelectedSystem->Set_TotalDuration(m_fCachedTotalDuration);
	}
	
	// Delayed Time
	if (ImGui::DragFloat("Delayed Time", &m_fCachedDelayedTime, 0.1f, 0.f, 10.f))
	{
		m_pSelectedSystem->Set_DelayedTime(m_fCachedDelayedTime);
	}

	// Rotation
	if (ImGui::DragFloat3("Rotation", RCAST(_float*)(&m_vSystemRotation), 1.f, -100.f, 100.f))
	{
		m_pSelectedSystem->Set_SystemRotation(m_vSystemRotation);
	}

	ImGui::Separator();

	// Effect 목록
	ImGui::Text("Effects:");
	_uint iEffectCount = m_pSelectedSystem->Get_EffectCount();
	for (_uint i = 0; i < iEffectCount; ++i)
	{
		char szEffectName[64];
		sprintf_s(szEffectName, "  - Effect_%d", i);
		ImGui::BulletText(wstringToString(m_pSelectedSystem->Get_EffectNameByIndex(i)).c_str());
	}

	// [+ Add Effect] / [+ Add Trail] / [+ Add Mesh] 버튼
	if (ImGui::Button("+ Add Effect"))
	{
		m_bShowAddEffectPopup = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Add Trail"))
	{
		m_bShowAddTrailEffectPopup = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Add Mesh"))
	{
		m_bShowAddMeshEffectPopup = true;
	}

	ImGui::Separator();

	// [Apply] 버튼
	if (ImGui::Button("Apply"))
	{
		// TODO: 모든 캐시된 값을 ParticleSystem에 적용
	}
}

/* ============================================== */
/*       Detail: ParticleEffect                   */
/* ============================================== */
void Window_VFXEditor::Render_Detail_ParticleEffect()
{
	ImGui::Text("ParticleEffect Settings");
	ImGui::Separator();

	if (m_pSelectedEffect == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "No ParticleEffect selected.");

		// 그래도 선택된 인덱스가 있으면 플레이스홀더 UI 표시
		if (m_iSelectedEffectIndex >= 0)
		{
			char szEffectName[64];
			sprintf_s(szEffectName, "Effect_%d", m_iSelectedEffectIndex);
			ImGui::Text("Name: %s", szEffectName);
		}
		return;
	}

	// StartDelay
	ImGui::DragFloat("Start Delay", &m_fCachedStartDelay, 0.01f, 0.f, 10.f);
	ImGui::DragFloat3("Rotation (X/Y/Z)##Particle", reinterpret_cast<float*>(&m_vCachedRotation), 1.f, -180.f, 180.f);

	ImGui::Separator();

	// [PARTICLE_DESC] CollapsingHeader
	if (ImGui::CollapsingHeader("PARTICLE_DESC"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		ImGui::Checkbox("Loop", &m_tParticleDesc.IsLoop);
		ImGui::SameLine(0.f, 50.f);
		ImGui::Checkbox("BillBoard", &m_tParticleDesc.IsBillboard);
		ImGui::InputInt("Instance Count", (_int*)&m_tParticleDesc.iNumInstance);

		ImGui::Separator();
		ImGui::DragFloat2("StartDelay (X/Y)", reinterpret_cast<float*>(&m_tParticleDesc.vStartDelay), 0.01f, 0.f, 10.f);
		ImGui::DragFloat2("Size (X/Y)", reinterpret_cast<float*>(&m_tParticleDesc.vSize), 0.01f, 0.01f, 10.f);
		ImGui::Checkbox("Vertical Fade", &m_tParticleDesc.IsVerticalFade);
		ImGui::Checkbox("Vertical Shrink", &m_tParticleDesc.IsVerticalShrink);
		ImGui::DragFloat("Scale Ratio", reinterpret_cast<float*>(&m_tParticleDesc.fParticleScale), 0.1f, 0.f, 100.f);
		ImGui::DragInt("Directional Scale", &m_tParticleDesc.iDirectionalScale, 1.f, -1, 1);
		ImGui::DragFloat2("Speed (Min/Max)", reinterpret_cast<float*>(&m_tParticleDesc.vSpeed), 0.01f, 0.0f, 50.f);
		ImGui::Checkbox("Radial Rotation", &m_tParticleDesc.IsRadialRotation);
		ImGui::DragFloat2("Rotation (Min/Max)", reinterpret_cast<float*>(&m_tParticleDesc.vRotation), 1.f, -180.f, 180.f);
		ImGui::DragFloat2("LifeTime (Min/Max)", reinterpret_cast<float*>(&m_tParticleDesc.vLifeTime), 0.01f, 0.01f, 10.f);
		ImGui::DragFloat2("Fade In/Out##PARTICLE", RCAST(float*)(&m_vCachedFadeInOut), 0.01f, 0.f, 1.f, "%.2f");

		ImGui::Separator();
		ImGui::DragFloat3("Center", reinterpret_cast<float*>(&m_tParticleDesc.vCenter), 0.01f);
		ImGui::DragFloat3("Range", reinterpret_cast<float*>(&m_tParticleDesc.vRange), 0.01f, 0.f, 100.f);
		ImGui::DragFloat3("Pivot", reinterpret_cast<float*>(&m_tParticleDesc.vPivot), 0.01f);
		ImGui::DragFloat2("Offset", RCAST(float*)(&m_vCachedEffectOffset), 0.01f, -1.f, 1.f, "%.2f");

		ImGui::Separator();
		ImGui::DragFloat("Gravity", &m_tParticleDesc.fGravity, 0.1f, 0.1f, 100.f);
		ImGui::SliderFloat("GlobalAlpha", &m_tParticleDesc.fAlpha, 0.f, 1.f);

		ImGui::Separator();
		ImGui::DragFloat("Shake_Strength", &m_tParticleDesc.fShakeStrength, 0.1f, 0.1f, 100.f);
	}

	// [COLOR] CollapsingHeader
	if (ImGui::CollapsingHeader("COLOR"))
	{
		_float4 vColor = m_pSelectedEffect->Get_Color();

		// HDR Intensity: RGB 중 가장 큰 값 기준으로 분리
		_float fMaxChannel = max(max(vColor.x, vColor.y), max(vColor.z, 1.f));
		_float4 vBaseColor = { vColor.x / fMaxChannel, vColor.y / fMaxChannel, vColor.z / fMaxChannel, vColor.w };
		_float fIntensity = fMaxChannel;

		bool bChanged = false;
		if (ImGui::ColorEdit4("Particle Color", reinterpret_cast<float*>(&vBaseColor)))
			bChanged = true;
		if (ImGui::DragFloat("Color Intensity", &fIntensity, 1.f, 0.f, 10000.f, "%1.f"))
			bChanged = true;
		if (ImGui::Checkbox("Use Emissive", &m_bCachedEmissive))
			bChanged = true;

		if (bChanged)
		{
			_float4 vFinalColor = { vBaseColor.x * fIntensity, vBaseColor.y * fIntensity, vBaseColor.z * fIntensity, vBaseColor.w };
			m_pSelectedEffect->Set_Color(vFinalColor);
			m_pSelectedEffect->Set_IsEmissive(m_bCachedEmissive);
		}
	}


	// [TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("TEXTURE"))
	{
		char szTexturePath[256] = {};
		if (!m_strTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");
		if (ImGui::Button("Browse##Texture"))
		{
			// 파일 다이얼로그 열기 및 텍스처 교체
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strTextureName = szFileName;

				CAST(BasicParticle*)(m_pSelectedEffect)->Change_Texture(m_strTexturePath);
			}
		}

		ImGui::InputInt("Frame Count", (_int*)&m_tParticleDesc.iFrameCount);
		ImGui::InputInt("Frame X Count", (_int*)&m_tParticleDesc.iCountX);
		ImGui::InputInt("Frame Y Count", (_int*)&m_tParticleDesc.iCountY);
		ImGui::InputInt("Transparent Index", (_int*)&m_tParticleDesc.iTransparentIndex);
		ImGui::DragFloat2("UV Scroll (X/Y)##PARTICLE", RCAST(_float*)(&m_vCachedDiffuseScroll), 0.01f, -10.f, 10.f);
	}

	// [ALPHAMASK TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("ALPHAMASK_TEXTURE"))
	{
		char szTexturePath[256] = {};
		if (!m_strAlphaMaskTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strAlphaMaskTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");
		if (ImGui::Button("Browse##AlphaMaskTexture"))
		{
			// 파일 다이얼로그 열기 및 텍스처 교체
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strAlphaMaskTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strAlphaMaskTextureName = szFileName;

				CAST(BasicParticle*)(m_pSelectedEffect)->Change_AlphaMaskTexture(m_strAlphaMaskTexturePath);
			}
		}
	}

	// [MASK TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("MASK_TEXTURE"))
	{
		char szTexturePath[256] = {};
		if (!m_strMaskTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strMaskTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");
		if (ImGui::Button("Browse##MaskTexture"))
		{
			// 파일 다이얼로그 열기 및 텍스처 교체
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strMaskTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strMaskTextureName = szFileName;

				CAST(BasicParticle*)(m_pSelectedEffect)->Change_MaskTexture(m_strMaskTexturePath);
			}
		}

		ImGui::InputInt("Mask Start Frame", (_int*)&m_tParticleDesc.iMaskStartFrame);
		ImGui::InputInt("Mask Frame Count", (_int*)&m_tParticleDesc.iMaskFrameCount);
		ImGui::InputInt("Mask Frame X Count", (_int*)&m_tParticleDesc.iMaskCountX);
		ImGui::InputInt("Mask Frame Y Count", (_int*)&m_tParticleDesc.iMaskCountY);
		ImGui::DragFloat2("UV Scroll (X/Y)", reinterpret_cast<float*>(&m_tParticleDesc.vMaskUVScroll), 0.1f, 0.f, 1.f);
		ImGui::DragFloat("Mask Intensity", &m_fCachedMaskIntensity, 0.1f, 1.f, 10.f);
	}

	// [NOISE TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("NOISE_TEXTURE"))
	{
		char szNoisePath[256] = {};
		if (!m_strNoiseTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strNoiseTextureName.c_str(), -1, szNoisePath, 256, nullptr, nullptr);
		ImGui::Text("Noise: %s", szNoisePath[0] ? szNoisePath : "(None)");
		if (ImGui::Button("Browse##Noise_Texture"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strNoiseTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strNoiseTextureName = szFileName;

				CAST(BasicParticle*)(m_pSelectedEffect)->Change_NoiseTexture(m_strNoiseTexturePath);
			}
		}

		ImGui::Checkbox("Edge Glow", &m_bUseEdgeGlow);
		if (m_bUseEdgeGlow)
			ImGui::DragFloat("Glow_Strength", &m_pSelectedEffect->Get_GlowStrength(), 0.01f, 0.f, 1.f);

		if (m_iPassIndex == 2)
			ImGui::SliderFloat("Distortion_Strength", &m_pSelectedEffect->Get_DistortionStrength(), 0.f, 1.f);
	}

	// [SHADER] CollapsingHeader
	if (ImGui::CollapsingHeader("SHADER"))
	{
		// 셰이더 프로토타입 목록 가져오기
		m_vecShaderPrototypes.clear(); 
		unordered_map<_wstring, Base*> umapPrototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::STATIC)];
		for (auto& Pair : umapPrototypes)
		{
			if (dynamic_cast<Shader*>(Pair.second) != nullptr)
			{
				_char buf[256];
				WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, buf, 256, nullptr, nullptr);
				m_vecShaderPrototypes.push_back(buf);
			}
		}

		vector<const _char*> items;
		for (const auto& str : m_vecShaderPrototypes)
			items.push_back(str.c_str());

		static int item_current = 0;

		if (ImGui::Combo("Shader Prototype", &item_current, items.data(), (_int)items.size()))
		{
			m_strShaderTag = stringToWstring(m_vecShaderPrototypes[item_current]);
			CAST(BasicParticle*)(m_pSelectedEffect)->Change_Shader(m_strShaderTag);
		}

		ImGui::Separator();
		ImGui::InputInt("Pass Index", &m_iPassIndex);
	}

	// [SHADER] CollapsingHeader
	if (ImGui::CollapsingHeader("COMPUTE SHADER"))
	{
		// 셰이더 프로토타입 목록 가져오기
		m_vecComputeShaderPrototypes.clear();
		unordered_map<_wstring, Base*> umapPrototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::STATIC)];
		for (auto& Pair : umapPrototypes)
		{
			if (dynamic_cast<ComputeShader*>(Pair.second) != nullptr)
			{
				_char buf[256];
				WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, buf, 256, nullptr, nullptr);
				m_vecComputeShaderPrototypes.push_back(buf);
			}
		}

		vector<const _char*> items;
		for (const auto& str : m_vecComputeShaderPrototypes)
			items.push_back(str.c_str());

		static int item_current_cs = 0;

		if (ImGui::Combo("Compute Shader Prototype", &item_current_cs, items.data(), (_int)items.size()))
		{
			m_strComputeShaderTag = stringToWstring(m_vecComputeShaderPrototypes[item_current_cs]);
			CAST(BasicParticle*)(m_pSelectedEffect)->Change_Compute_Shader(m_strComputeShaderTag);
		}

		if (m_strComputeShaderTag == L"Prototype_Component_Shader_Compute_Particle_Converge")
		{
			ImGui::DragFloat("Converge MaxDist", &m_tParticleDesc.fConvergeMaxDist, 0.01f, 0.f, 100.f);
		}
	}

	// [SUBEMITTER] CollapsingHeader
	if (ImGui::CollapsingHeader("SUBEMITTER"))
	{
		SubEmitter* pSubEmitter = m_pSelectedEffect->Get_SubEmitter();
		if (pSubEmitter != nullptr)
		{
			_uint iEntryCount = pSubEmitter->Get_EmitterCount();
			ImGui::Text("SubEmitter Entries: %d", iEntryCount);

			for (_uint i = 0; i < iEntryCount; ++i)
			{
				char szEntryLabel[64];
				sprintf_s(szEntryLabel, "Entry_%d", i);
				if (ImGui::Selectable(szEntryLabel, m_iSelectedSubEmitterIndex == (_int)i))
				{
					m_eSelectionType = SELECTION_TYPE::SUBEMITTER_ENTRY;
					m_iSelectedSubEmitterIndex = i;
					m_tSubEmitterDesc = *pSubEmitter->Get_EmitterDescByIndex(m_iSelectedSubEmitterIndex);
				}
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "No SubEmitter attached.");
		}

		if (ImGui::Button("+ Add Entry"))
		{
			m_bShowAddSubEmitterPopup = true;
		}
	}

	ImGui::Separator();

	// [Apply Settings] 버튼
	if (ImGui::Button("Apply Settings"))
	{
		// 모든 설정을 ParticleEffect에 적용
		m_pSelectedSystem->Set_StartDelay(m_iSelectedEffectIndex, m_fCachedStartDelay);
		static_cast<VIBuffer_Particle*>(m_pSelectedEffect->Get_Component_FromName(TEXT("Com_VIBuffer")))->Update_Particles(m_tParticleDesc);
		m_pSelectedEffect->Set_PassIndex(m_iPassIndex);
		m_pSelectedEffect->Set_EdgeGlow(m_bUseEdgeGlow);
		m_pSelectedEffect->Set_MaskIntensity(m_fCachedMaskIntensity);
		m_pSelectedEffect->Set_EffectRotation(m_vCachedRotation);
		m_pSelectedEffect->Set_DiffuseUVScroll(m_vCachedDiffuseScroll);
		m_pSelectedEffect->Set_FadeInOut(m_vCachedFadeInOut);
		m_pSelectedEffect->Set_EffectOffset(m_vCachedEffectOffset);

		// 다시 ImGui에 값을 갱신하기 위해 가져옴
		m_tParticleDesc = m_pSelectedEffect->Get_ParticleDesc();
		m_fCachedStartDelay = m_pSelectedSystem->Get_StartDelay(m_iSelectedEffectIndex);
		m_iPassIndex = m_pSelectedEffect->Get_PassIndex();
		m_bUseEdgeGlow = m_pSelectedEffect->Get_UseEdgeGlow();
		m_fCachedMaskIntensity = m_pSelectedEffect->Get_MaskIntensity();
		m_vCachedRotation = m_pSelectedEffect->Get_EffectRotation();
		m_vCachedDiffuseScroll = m_pSelectedEffect->Get_DiffuseUVScroll();
		m_vCachedFadeInOut = m_pSelectedEffect->Get_FadeInOut();
		m_bCachedEmissive = m_pSelectedEffect->Get_IsEmissive();
		m_vCachedEffectOffset = m_pSelectedEffect->Get_EffectOffset();
	}
}

/* ============================================== */
/*       Detail: SubEmitter Entry                 */
/* ============================================== */
void Window_VFXEditor::Render_Detail_SubEmitter()
{
	ImGui::Text("SubEmitter Entry Settings");
	ImGui::Separator();

	if (m_iSelectedSubEmitterIndex < 0)
	{
		ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "No SubEmitter Entry selected.");
		return;
	}

	// Type 라디오버튼
	ImGui::Text("Trigger Type:");
	int iCurrentType = static_cast<int>(m_tSubEmitterDesc.eType);
	if (ImGui::RadioButton("BIRTH", &iCurrentType, 0))
	{
		m_tSubEmitterDesc.eType = SubEmitter::SUBEMMITER_TYPE::BIRTH;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("DEATH", &iCurrentType, 1))
	{
		m_tSubEmitterDesc.eType = SubEmitter::SUBEMMITER_TYPE::DEATH;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("COLLISION", &iCurrentType, 2))
	{
		m_tSubEmitterDesc.eType = SubEmitter::SUBEMMITER_TYPE::COLLISION;
	}

	ImGui::Separator();

	// Target Effect 콤보박스
	ImGui::Text("Target Effect:");

	if (m_pSelectedSystem != nullptr)
	{
		_uint iEffectCount = m_pSelectedSystem->Get_EffectCount();

		if (iEffectCount > 0)
		{
			// Effect 이름들을 vector<string>으로 변환
			vector<string> vecEffectNames;
			for (_uint i = 0; i < iEffectCount; i++)
			{
				_wstring wstrName = m_pSelectedSystem->Get_EffectNameByIndex(i);
				vecEffectNames.push_back(wstringToString(wstrName));
			}

			// ImGui::Combo에 쓰기 위해 const char* 로 변환
			vector<const _char*> vecEffects;
			for (const auto& wstrName : vecEffectNames)
				vecEffects.push_back(wstrName.c_str());

			// 콤보박스 생성
			static int iSelectedEffectIndex = 0;
			if (ImGui::Combo("Target Effect", &iSelectedEffectIndex, vecEffects.data(), (_int)vecEffects.size()))
			{
				m_tSubEmitterDesc.pEffect = m_pSelectedSystem->Find_EffectByIndex(iSelectedEffectIndex);
			}
		}
	}

	ImGui::Separator();

	// EmitProbability 슬라이더
	ImGui::SliderFloat("Emit Probability", &m_tSubEmitterDesc.fEmitProbability, 0.f, 1.f);

	// InheritPosition 체크박스
	ImGui::Checkbox("Inherit Position", &m_tSubEmitterDesc.bInheritPosition);

	// InheritScale 체크박스
	ImGui::Checkbox("Inherit Scale", &m_tSubEmitterDesc.bInheritScale);

	ImGui::Separator();

	// [Apply] [Delete] 버튼
	if (ImGui::Button("Apply"))
	{
		// SubEmitter Entry 업데이트
		m_pSelectedEffect->Get_SubEmitter()->Update_Entry(m_tSubEmitterDesc, m_iSelectedSubEmitterIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		// TODO: SubEmitter Entry 삭제
		// 삭제 후 선택 초기화
		m_eSelectionType = SELECTION_TYPE::PARTICLE_EFFECT;
		m_iSelectedSubEmitterIndex = -1;
	}
}

/* ============================================== */
/*       Detail: TrailEffect                      */
/* ============================================== */
void Window_VFXEditor::Render_Detail_TrailEffect()
{
	ImGui::Text("TrailEffect Settings");
	ImGui::Separator();

	if (m_pSelectedEffect == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "No TrailEffect selected.");
		return;
	}

	// StartDelay
	ImGui::DragFloat("Start Delay", &m_fCachedStartDelay, 0.01f, 0.f, 10.f);

	ImGui::Separator();

	// [TRAIL_DESC] CollapsingHeader
	if (ImGui::CollapsingHeader("TRAIL_DESC", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Max Points", (_int*)&m_tTrailDesc.iMaxPoints, 1, 4, 256);
		ImGui::DragFloat("Life Time", &m_tTrailDesc.fLifeTime, 0.01f, 0.01f, 10.f);
		ImGui::DragFloat("Min Distance", &m_tTrailDesc.fMinDistance, 0.001f, 0.001f, 1.f);
		ImGui::DragFloat("Texture Rotation", &m_tTrailDesc.fTexRotation, 1.f, -180.f, 180.f);
	}

	// [COLOR] CollapsingHeader
	if (ImGui::CollapsingHeader("COLOR", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_float4 vColor = m_pSelectedEffect->Get_Color();

		_float fMaxChannel = max(max(vColor.x, vColor.y), max(vColor.z, 1.f));
		_float4 vBaseColor = { vColor.x / fMaxChannel, vColor.y / fMaxChannel, vColor.z / fMaxChannel, vColor.w };
		_float fIntensity = fMaxChannel;

		bool bChanged = false;
		if (ImGui::ColorEdit4("Trail Color", reinterpret_cast<float*>(&vBaseColor)))
			bChanged = true;
		if (ImGui::DragFloat("Color Intensity", &fIntensity, 0.01f, 0.f, 50.f, "%.2f"))
			bChanged = true;

		if (bChanged)
		{
			_float4 vFinalColor = { vBaseColor.x * fIntensity, vBaseColor.y * fIntensity, vBaseColor.z * fIntensity, vBaseColor.w };
			m_pSelectedEffect->Set_Color(vFinalColor);
		}
	}

	// [TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("TEXTURE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char szTexturePath[256] = {};
		if (!m_strTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");
		if (ImGui::Button("Browse##TrailTexture"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strTextureName = szFileName;

				static_cast<SwordTrail*>(m_pSelectedEffect)->Change_Texture(m_strTexturePath);
			}
		}
	}

	// [NOISE TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("NOISE TEXTURE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char szNoisePath[256] = {};
		if (!m_strNoiseTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strNoiseTextureName.c_str(), -1, szNoisePath, 256, nullptr, nullptr);
		ImGui::Text("Noise: %s", szNoisePath[0] ? szNoisePath : "(None)");
		if (ImGui::Button("Browse##NoiseTexture"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strNoiseTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strNoiseTextureName = szFileName;

				static_cast<SwordTrail*>(m_pSelectedEffect)->Change_NoiseTexture(m_strNoiseTexturePath);
			}
		}

		ImGui::SliderFloat("Distortion Strength", &static_cast<SwordTrail*>(m_pSelectedEffect)->Get_DistortionStrength(), 0.f, 2.f);
	}

	// [NOISE TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("MASK TEXTURE##TRAIL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char szMaskPath[256] = {};
		if (!m_strMaskTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strMaskTextureName.c_str(), -1, szMaskPath, 256, nullptr, nullptr);
		ImGui::Text("Mask: %s", szMaskPath[0] ? szMaskPath : "(None)");
		if (ImGui::Button("Browse##MaskTexture"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strMaskTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strMaskTextureName = szFileName;

				CAST(SwordTrail*)(m_pSelectedEffect)->Change_MaskTexture(m_strMaskTexturePath);
			}
		}
	}

	// [SHADER] CollapsingHeader
	if (ImGui::CollapsingHeader("SHADER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		m_vecShaderPrototypes.clear();
		unordered_map<_wstring, Base*> umapPrototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::STATIC)];
		for (auto& Pair : umapPrototypes)
		{
			if (dynamic_cast<Shader*>(Pair.second) != nullptr)
			{
				_char buf[256];
				WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, buf, 256, nullptr, nullptr);
				m_vecShaderPrototypes.push_back(buf);
			}
		}

		vector<const _char*> items;
		for (const auto& str : m_vecShaderPrototypes)
			items.push_back(str.c_str());

		static int item_current_trail = 0;

		if (ImGui::Combo("Shader Prototype##Trail", &item_current_trail, items.data(), (_int)items.size()))
		{
			m_strShaderTag = stringToWstring(m_vecShaderPrototypes[item_current_trail]);
			static_cast<SwordTrail*>(m_pSelectedEffect)->Change_Shader(m_strShaderTag);
		}

		ImGui::Separator();
		ImGui::InputInt("Pass Index##Trail", &m_iPassIndex);
	}

	// [SUBEMITTER] CollapsingHeader
	if (ImGui::CollapsingHeader("SUBEMITTER##Trail"))
	{
		SubEmitter* pSubEmitter = m_pSelectedEffect->Get_SubEmitter();
		if (pSubEmitter != nullptr)
		{
			_uint iEntryCount = pSubEmitter->Get_EmitterCount();
			ImGui::Text("SubEmitter Entries: %d", iEntryCount);

			for (_uint i = 0; i < iEntryCount; ++i)
			{
				char szEntryLabel[64];
				sprintf_s(szEntryLabel, "Entry_%d##Trail", i);
				if (ImGui::Selectable(szEntryLabel, m_iSelectedSubEmitterIndex == (_int)i))
				{
					m_eSelectionType = SELECTION_TYPE::SUBEMITTER_ENTRY;
					m_iSelectedSubEmitterIndex = i;
					m_tSubEmitterDesc = *pSubEmitter->Get_EmitterDescByIndex(m_iSelectedSubEmitterIndex);
				}
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "No SubEmitter attached.");
		}

		if (ImGui::Button("+ Add Entry##Trail"))
		{
			m_bShowAddSubEmitterPopup = true;
		}
	}

	ImGui::Separator();

	// [Apply Trail Settings] 버튼
	if (ImGui::Button("Apply Trail Settings"))
	{
		m_pSelectedSystem->Set_StartDelay(m_iSelectedEffectIndex, m_fCachedStartDelay);
		static_cast<TrailEffect*>(m_pSelectedEffect)->Set_TrailDesc(m_tTrailDesc);
		m_pSelectedEffect->Set_PassIndex(m_iPassIndex);

		// 캐시 갱신
		m_tTrailDesc = static_cast<TrailEffect*>(m_pSelectedEffect)->Get_TrailDesc();
		m_fCachedStartDelay = m_pSelectedSystem->Get_StartDelay(m_iSelectedEffectIndex);
		m_iPassIndex = m_pSelectedEffect->Get_PassIndex();
	}
}

/* ============================================== */
/*            Add System Popup                    */
/* ============================================== */
void Window_VFXEditor::Render_AddSystemPopup()
{
	if (m_bShowAddSystemPopup)
	{
		ImGui::OpenPopup("Add ParticleSystem");
		m_bShowAddSystemPopup = false;
	}

	if (ImGui::BeginPopupModal("Add ParticleSystem", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter the name for the new ParticleSystem:");
		ImGui::InputText("##NewSystemName", m_szNewName, IM_ARRAYSIZE(m_szNewName));

		ImGui::Separator();

		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			// 새 ParticleSystem 복사 및 m_vecParticleSystems에 추가
			ParticleSystem::PARTICLESYSTEM_DESC particleSystemDesc{};
			particleSystemDesc.wstrName = stringToWstring(m_szNewName);
			particleSystemDesc.bIsAutoPlay = false;
			particleSystemDesc.bIsLoop = false;
			particleSystemDesc.fTotalDuration = 3.f;
			particleSystemDesc.fRotationSpeed = 1.f;

			GameObject* pNewParticleSystem = { nullptr };
			HRESULT hr = m_pGameInstance->Add_GameObject_To_Layer(
				_UINT(LEVEL::MAIN), L"Prototype_GameObject_ParticleSystem",
				_UINT(LEVEL::MAIN), L"Layer_Effect", &pNewParticleSystem, &particleSystemDesc);

			if (SUCCEEDED(hr) && pNewParticleSystem != nullptr)
				m_vecParticleSystems.push_back(CAST(ParticleSystem*)(pNewParticleSystem));

			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/* ============================================== */
/*            Add Effect Popup                    */
/* ============================================== */
void Window_VFXEditor::Render_AddEffectPopup()
{
	if (m_bShowAddEffectPopup)
	{
		ImGui::OpenPopup("Add ParticleEffect");
		m_bShowAddEffectPopup = false;
	}

	if (ImGui::BeginPopupModal("Add ParticleEffect", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter the name for the new ParticleEffect:");
		ImGui::InputText("##NewEffectName", m_szNewName, IM_ARRAYSIZE(m_szNewName));

		ImGui::Separator();

		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			// 새 ParticleEffect 생성 및 현재 선택된 시스템에 추가
			if (m_pSelectedSystem != nullptr)
			{
				ParticleEffect* pNewEffect = CAST(BasicParticle*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
					TEXT("Prototype_GameObject_BasicParticle")));

				m_pSelectedSystem->Add_Effect(stringToWstring(m_szNewName), pNewEffect, EFFECTTYPE::EFFECT_PARTICLE);
				Safe_Release(pNewEffect);
			}

			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/* ============================================== */
/*         Add SubEmitter Popup                   */
/* ============================================== */
void Window_VFXEditor::Render_AddSubEmitterPopup()
{
	if (m_bShowAddSubEmitterPopup)
	{
		ImGui::OpenPopup("Add SubEmitter Entry");
		m_bShowAddSubEmitterPopup = false;
	}

	if (ImGui::BeginPopupModal("Add SubEmitter Entry", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (m_pSelectedEffect == nullptr || m_pSelectedSystem == nullptr)
		{
			ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), "No Effect selected!");
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return;
		}

		ImGui::Text("Configure the new SubEmitter Entry:");

		// Type 선택
		static int newType = 0;
		ImGui::Text("Type:");
		ImGui::RadioButton("BIRTH##New", &newType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("DEATH##New", &newType, 1);
		ImGui::SameLine();
		ImGui::RadioButton("COLLISION##New", &newType, 2);

		// Target Effect 선택
		// Effect 이름들을 vector<string>으로 변환
		vector<string> vecEffectNames;
		if (m_pSelectedSystem == nullptr)
			return;

		_uint iEffectCount = m_pSelectedSystem->Get_EffectCount();
		for (_uint i = 0; i < iEffectCount; i++)
		{
			_wstring wstrName = m_pSelectedSystem->Get_EffectNameByIndex(i);
			vecEffectNames.push_back(wstringToString(wstrName));
		}

		// ImGui::Combo에 쓰기 위해 const char* 로 변환
		vector<const _char*> vecEffects;
		for (const auto& wstrName : vecEffectNames)
			vecEffects.push_back(wstrName.c_str());

		// 콤보박스 생성
		static int iSelectedTargetIndex = 0;

		if (iSelectedTargetIndex >= (_int)iEffectCount)
			iSelectedTargetIndex = 0;

		ImGui::Combo("Target Effect##New", &iSelectedTargetIndex, vecEffects.data(), (_int)vecEffects.size());
		ImGui::Separator();

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			// 새 SubEmitter Entry 생성 및 추가
			SubEmitter* pSubEmitter = m_pSelectedEffect->Get_SubEmitter();
			if (pSubEmitter == nullptr)
				pSubEmitter = m_pSelectedEffect->Create_SubEmitter();

			ParticleEffect* pTargetEffect = m_pSelectedSystem->Find_EffectByIndex(iSelectedTargetIndex);

			SubEmitter::SUBEMITTER_DESC newSubEmitterDesc{};
			newSubEmitterDesc.eType = static_cast<SubEmitter::SUBEMMITER_TYPE>(newType);
			newSubEmitterDesc.strEffectName = m_pSelectedSystem->Get_EffectNameByIndex(iSelectedTargetIndex);
			newSubEmitterDesc.pEffect = pTargetEffect;
			newSubEmitterDesc.fEmitProbability = 1.f;
			newSubEmitterDesc.bInheritPosition = true;
			newSubEmitterDesc.bInheritScale = false;

			if (pSubEmitter != nullptr)
			{
				pSubEmitter->Add_Entry(newSubEmitterDesc);
				pTargetEffect->Set_SubEmitter(true);
			}

			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/* ============================================== */
/*         Add TrailEffect Popup                  */
/* ============================================== */
void Window_VFXEditor::Render_AddTrailEffectPopup()
{
	if (m_bShowAddTrailEffectPopup)
	{
		ImGui::OpenPopup("Add TrailEffect");
		m_bShowAddTrailEffectPopup = false;
	}

	if (ImGui::BeginPopupModal("Add TrailEffect", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter the name for the new TrailEffect:");
		ImGui::InputText("##NewTrailName", m_szNewName, IM_ARRAYSIZE(m_szNewName));

		ImGui::Separator();

		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			if (m_pSelectedSystem != nullptr)
			{
				ParticleEffect* pNewTrail = CAST(SwordTrail*)(m_pGameInstance->Clone_Prototype(
					PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
					TEXT("Prototype_GameObject_TrailEffect")));

				m_pSelectedSystem->Add_Effect(stringToWstring(m_szNewName), pNewTrail, EFFECTTYPE::EFFECT_TRAIL);
				Safe_Release(pNewTrail);
			}

			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/* ============================================== */
/*       Detail: MeshEffect                       */
/* ============================================== */
void Window_VFXEditor::Render_Detail_MeshEffect()
{
	ImGui::Text("MeshEffect Settings");
	ImGui::Separator();

	if (m_pSelectedEffect == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "No MeshEffect selected.");
		return;
	}

	// StartDelay
	ImGui::Checkbox("Loop##Mesh", &m_bCachedEffectLoop);
	ImGui::Checkbox("BackCulling##Mesh", &m_bCachedBackCulling);
	ImGui::Separator();
	ImGui::DragFloat("Start Delay##Mesh", &m_fCachedStartDelay, 0.01f, 0.f, 10.f);

	// [PARTICLE_DESC] CollapsingHeader
	if (ImGui::CollapsingHeader("MESH_EFFECT"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		ImGui::Checkbox("Align Player Look##Mesh", (bool*)&m_tMeshDesc.bAlignToPlayerLook);
		ImGui::Separator();

		ImGui::DragFloat3("Position (X/Y/Z)", RCAST(float*)(&m_tMeshDesc.vPosition), 0.01f, -10.f, 10.f);
		ImGui::DragFloat3("Scale (X/Y/Z)", RCAST(float*)(&m_tMeshDesc.vScale), 0.01f, 0.01f, 10.f);

		if (ImGui::DragFloat3("Rotation (X/Y/Z)", RCAST(float*)(&m_vCachedRotation), 1.f, -180.f, 180.f))
			m_pSelectedEffect->Set_EffectRotation(m_vCachedRotation);

		ImGui::DragFloat("LifeTime", &m_tMeshDesc.fLifeTime, 0.01f, 0.01f, 10.f);
		ImGui::DragFloat("Scale Ratio##MESH", &m_tMeshDesc.fMeshScale, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("Scale Delay", &m_tMeshDesc.fScaleDelay, 0.1f, 0.f, 100.f);
		
		_bool bRight = m_tMeshDesc.iScaleAxis & 1;
		_bool bUp = m_tMeshDesc.iScaleAxis & 2;
		_bool bLook = m_tMeshDesc.iScaleAxis & 4;
		ImGui::CheckboxFlags("Scale Right", &m_tMeshDesc.iScaleAxis, 1);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Scale Up", &m_tMeshDesc.iScaleAxis, 2);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Scale Look", &m_tMeshDesc.iScaleAxis, 4);

		ImGui::DragFloat2("Fade In/Out", RCAST(float*)(&m_vCachedFadeInOut), 0.01f, 0.f, 1.f, "%.2f");

		ImGui::Text("Rotation Speed");
		ImGui::DragInt("Right(X)", &m_tMeshDesc.iRotateRight, 1);
		ImGui::DragInt("Up(Y)", &m_tMeshDesc.iRotateUp, 1);
		ImGui::DragInt("Look(Z)", &m_tMeshDesc.iRotateLook, 1);
	}

	// [MODEL] CollapsingHeader
	if (ImGui::CollapsingHeader("MODEL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char szModelTag[256] = {};
		if (!m_wstrModelTag.empty())
			WideCharToMultiByte(CP_ACP, 0, m_wstrModelTag.c_str(), -1, szModelTag, 256, nullptr, nullptr);
		ImGui::Text("Model: %s", szModelTag[0] ? szModelTag : "(None)");

		if (ImGui::Button("Browse##Model"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Model\\Effect"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Model Files(*.siho;*.fbx)\0*.siho;*.fbx\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				fs::path selectedPath(szFile);
				_wstring fileName = selectedPath.stem().wstring();
				_wstring prototypeName = L"Prototype_Component_Model_" + fileName;

				// 프로토타입이 없으면 동적으로 로드
				auto& umapPrototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::MAIN)];
				if (umapPrototypes.find(prototypeName) == umapPrototypes.end())
				{
					_matrix prematrix = XMMatrixIdentity();
					prematrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

					m_pGameInstance->Add_Prototype(_UINT(LEVEL::MAIN), prototypeName,
						Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, selectedPath.wstring(), prematrix));
				}

				m_wstrModelTag = prototypeName;
				CAST(BasicMesh*)(m_pSelectedEffect)->Change_Model(m_wstrModelTag);
			}
		}
	}

	// [COLOR] CollapsingHeader
	if (ImGui::CollapsingHeader("COLOR##Mesh"))
	{
		_float4 vColor = m_pSelectedEffect->Get_Color();

		_float fMaxChannel = max(max(vColor.x, vColor.y), max(vColor.z, 1.f));
		_float4 vBaseColor = { vColor.x / fMaxChannel, vColor.y / fMaxChannel, vColor.z / fMaxChannel, vColor.w };
		_float fIntensity = fMaxChannel;

		bool bChanged = false;
		if (ImGui::ColorEdit4("Mesh Color", RCAST(float*)(&vBaseColor)))
			bChanged = true;
		if (ImGui::DragFloat("Color Intensity##Mesh", &fIntensity, 0.01f, 0.f, 50.f, "%.2f"))
			bChanged = true;
		if (ImGui::Checkbox("Use Emissive", &m_bCachedEmissive))
			bChanged = true;

		if (bChanged)
		{
			_float4 vFinalColor = { vBaseColor.x * fIntensity, vBaseColor.y * fIntensity, vBaseColor.z * fIntensity, vBaseColor.w };
			m_pSelectedEffect->Set_Color(vFinalColor);
			m_pSelectedEffect->Set_IsEmissive(m_bCachedEmissive);
		}

		ImGui::Separator();
		ImGui::Text("Hot Glow");
		if (ImGui::ColorEdit3("Hot Color##Mesh", RCAST(float*)(&m_vHotColor), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float) ||
			ImGui::DragFloat("Hot Power##Mesh", &m_fHotPower, 0.1f, 0.1f, 10.f, "%.1f"))
			m_pSelectedEffect->Set_HotColor(m_vHotColor, m_fHotPower);
	}

	// [TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("TEXTURE##Mesh"))
	{
		char szTexturePath[256] = {};
		if (!m_strTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");

		if (ImGui::Button("Browse##MeshTexture"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strTextureName = szFileName;

				CAST(BasicMesh*)(m_pSelectedEffect)->Change_Texture(m_strTexturePath);
			}
		}

		ImGui::InputInt("Frame Count", (_int*)&m_tMeshDesc.iFrameCount);
		ImGui::InputInt("Frame X Count", (_int*)&m_tMeshDesc.iCountX);
		ImGui::InputInt("Frame Y Count", (_int*)&m_tMeshDesc.iCountY);
		ImGui::DragFloat2("Diffuse UV Scroll (X/Y)", RCAST(_float*)(&m_vCachedDiffuseScroll), 0.01f, -10.f, 10.f);
	}

	// [ALPHAMASK TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("ALPHAMASK_TEXTURE##Mesh"))
	{
		char szTexturePath[256] = {};
		if (!m_strAlphaMaskTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strAlphaMaskTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");

		if (ImGui::Button("Browse##MeshAlphaMask"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strAlphaMaskTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strAlphaMaskTextureName = szFileName;

				CAST(BasicMesh*)(m_pSelectedEffect)->Change_AlphaMaskTexture(m_strAlphaMaskTexturePath);
			}
		}
	}

	// [MASK TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("MASK_TEXTURE##Mesh"))
	{
		char szTexturePath[256] = {};
		if (!m_strMaskTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strMaskTextureName.c_str(), -1, szTexturePath, 256, nullptr, nullptr);
		ImGui::Text("Texture: %s", szTexturePath[0] ? szTexturePath : "(None)");

		if (ImGui::Button("Browse##MeshMask"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strMaskTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strMaskTextureName = szFileName;

				CAST(BasicMesh*)(m_pSelectedEffect)->Change_MaskTexture(m_strMaskTexturePath);
			}
		}

		ImGui::InputInt("Mask Start Frame", (_int*)&m_tMeshDesc.iMaskStartFrame);
		ImGui::InputInt("Mask Frame Count", (_int*)&m_tMeshDesc.iMaskFrameCount);
		ImGui::InputInt("Mask Frame X Count", (_int*)&m_tMeshDesc.iMaskCountX);
		ImGui::InputInt("Mask Frame Y Count", (_int*)&m_tMeshDesc.iMaskCountY);
		ImGui::DragFloat2("Mask UV Scale (X/Y)", RCAST(_float*)(&m_tMeshDesc.vMaskUVScale), 0.01f, 0.01f, 10.f);
		ImGui::DragFloat2("Mask UV Scroll (X/Y)", RCAST(_float*)(&m_tMeshDesc.vMaskUVScroll), 0.1f, -10.f, 10.f);
		ImGui::DragFloat2("Mask UV Offset (X/Y)", RCAST(_float*)(&m_tMeshDesc.vMaskUVStartOffset), 0.1f, -10.f, 10.f);
		ImGui::Checkbox("Mask Sampler", RCAST(_bool*)(&m_iMaskSampler));
		ImGui::DragFloat("Mask Intensity", &m_fCachedMaskIntensity, 0.1f, 1.f, 10.f);
	}

	// [NOISE TEXTURE] CollapsingHeader
	if (ImGui::CollapsingHeader("NOISE_TEXTURE##Mesh"))
	{
		char szNoisePath[256] = {};
		if (!m_strNoiseTextureName.empty())
			WideCharToMultiByte(CP_ACP, 0, m_strNoiseTextureName.c_str(), -1, szNoisePath, 256, nullptr, nullptr);
		ImGui::Text("Noise: %s", szNoisePath[0] ? szNoisePath : "(None)");

		if (ImGui::Button("Browse##MeshNoise"))
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			PathAppend(exePath, TEXT("..\\..\\Resources\\Effects"));

			OPENFILENAME ofn;
			TCHAR szFile[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Texture Files(*.png;*.dds)\0*.png;*.dds\0All Files\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = exePath;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				m_strNoiseTexturePath = szFile;
				_tchar* szFileName = PathFindFileName(szFile);
				m_strNoiseTextureName = szFileName;

				static_cast<BasicMesh*>(m_pSelectedEffect)->Change_NoiseTexture(m_strNoiseTexturePath);
			}
		}

		ImGui::Checkbox("Directional Dissolve", RCAST(_bool*)(&m_tMeshDesc.bDirectionalDissolve));
		if (m_tMeshDesc.bDirectionalDissolve)
		{
			ImGui::Checkbox("Dissolve Flip Y", RCAST(_bool*)(&m_tMeshDesc.bDissolveFlipY));
			ImGui::DragFloat("Noise Weight", &m_tMeshDesc.fNoiseWeight, 0.01f, 0.f, 1.f);
			ImGui::DragFloat("Edge Width", &m_tMeshDesc.fDissolveEdgeWidth, 0.01f, 0.f, 1.f);
			ImGui::ColorEdit3("Edge Color##MESH", RCAST(_float*)(&m_tMeshDesc.vDissolveEdgeColor), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
		}

		ImGui::Checkbox("Edge Glow##Mesh", &m_bMeshUseEdgeGlow);
		if (m_bMeshUseEdgeGlow)
			ImGui::DragFloat("Glow_Strength##Mesh", &m_pSelectedEffect->Get_GlowStrength(), 0.01f, 0.f, 1.f);

		if (m_iPassIndex == 2)
			ImGui::SliderFloat("Distortion_Strength##Mesh", &m_pSelectedEffect->Get_DistortionStrength(), 0.f, 1.f);
		else if (m_iPassIndex == 3)
			ImGui::DragFloat("Displace Strength", &m_tMeshDesc.fDisplaceStrength, 0.01f, -10.f, 10.f);

	}

	// [SHADER] CollapsingHeader
	if (ImGui::CollapsingHeader("SHADER##Mesh"))
	{
		m_vecShaderPrototypes.clear();
		unordered_map<_wstring, Base*> umapPrototypes = m_pGameInstance->Get_Prototypes()[_UINT(LEVEL::STATIC)];
		for (auto& Pair : umapPrototypes)
		{
			if (DCAST(Shader*)(Pair.second) != nullptr)
			{
				_char buf[256];
				WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, buf, 256, nullptr, nullptr);
				m_vecShaderPrototypes.push_back(buf);
			}
		}

		vector<const _char*> items;
		for (const auto& str : m_vecShaderPrototypes)
			items.push_back(str.c_str());

		static int item_current_mesh = 0;

		if (ImGui::Combo("Shader Prototype##Mesh", &item_current_mesh, items.data(), (_int)items.size()))
		{
			m_strShaderTag = stringToWstring(m_vecShaderPrototypes[item_current_mesh]);
			CAST(BasicMesh*)(m_pSelectedEffect)->Change_Shader(m_strShaderTag);
		}

		ImGui::Separator();
		ImGui::InputInt("Pass Index##Mesh", &m_iPassIndex);
	}

	ImGui::Separator();

	// [Apply Mesh Settings] 버튼
	if (ImGui::Button("Apply Mesh Settings"))
	{
		m_pSelectedSystem->Set_StartDelay(m_iSelectedEffectIndex, m_fCachedStartDelay);
		CAST(MeshEffect*)(m_pSelectedEffect)->Set_MeshDesc(m_tMeshDesc);
		m_pSelectedEffect->Set_PassIndex(m_iPassIndex);
		m_pSelectedEffect->Set_EdgeGlow(m_bMeshUseEdgeGlow);
		m_pSelectedEffect->Set_EffectLoop(m_bCachedEffectLoop);
		m_pSelectedEffect->Set_BackCulling(m_bCachedBackCulling);
		m_pSelectedEffect->Set_LifeTime(m_tMeshDesc.fLifeTime);
		m_pSelectedEffect->Set_MaskIntensity(m_fCachedMaskIntensity);
		m_pSelectedEffect->Set_MaskSampler(m_iMaskSampler);
		m_pSelectedEffect->Set_EffectRotation(m_vCachedRotation);
		m_pSelectedEffect->Set_DiffuseUVScroll(m_vCachedDiffuseScroll);
		m_pSelectedEffect->Set_FadeInOut(m_vCachedFadeInOut);

		// 캐시 갱신
		m_tMeshDesc = CAST(BasicMesh*)(m_pSelectedEffect)->Get_MeshDesc();
		m_fCachedStartDelay = m_pSelectedSystem->Get_StartDelay(m_iSelectedEffectIndex);
		m_iPassIndex = m_pSelectedEffect->Get_PassIndex();
		m_bMeshUseEdgeGlow = m_pSelectedEffect->Get_UseEdgeGlow();
		m_bCachedEffectLoop = m_pSelectedEffect->Get_EffectLoop();
		m_bCachedBackCulling = m_pSelectedEffect->Get_BackCulling();
		m_fCachedMaskIntensity = m_pSelectedEffect->Get_MaskIntensity();
		m_iMaskSampler = m_pSelectedEffect->Get_MaskSampler();
		m_vCachedRotation = m_pSelectedEffect->Get_EffectRotation();
		m_vCachedDiffuseScroll = m_pSelectedEffect->Get_DiffuseUVScroll();
		m_vCachedFadeInOut = m_pSelectedEffect->Get_FadeInOut();
		m_bCachedEmissive = m_pSelectedEffect->Get_IsEmissive();
	}

	CAST(MeshEffect*)(m_pSelectedEffect)->Set_MeshDesc(m_tMeshDesc);
}

/* ============================================== */
/*         Add MeshEffect Popup                   */
/* ============================================== */
void Window_VFXEditor::Render_AddMeshEffectPopup()
{
	if (m_bShowAddMeshEffectPopup)
	{
		ImGui::OpenPopup("Add MeshEffect");
		m_bShowAddMeshEffectPopup = false;
	}

	if (ImGui::BeginPopupModal("Add MeshEffect", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter the name for the new MeshEffect:");
		ImGui::InputText("##NewMeshEffectName", m_szNewName, IM_ARRAYSIZE(m_szNewName));

		ImGui::Separator();

		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			if (m_pSelectedSystem != nullptr)
			{
				ParticleEffect* pNewMeshEffect = CAST(BasicMesh*)(m_pGameInstance->Clone_Prototype(
					PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
					TEXT("Prototype_GameObject_BasicMesh")));

				m_pSelectedSystem->Add_Effect(stringToWstring(m_szNewName), pNewMeshEffect, EFFECTTYPE::EFFECT_MESH);
				Safe_Release(pNewMeshEffect);
			}

			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			memset(m_szNewName, 0, sizeof(m_szNewName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

Window_VFXEditor* Window_VFXEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    Window_VFXEditor* pInstance = new Window_VFXEditor(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : Window_VFXEditor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Window_VFXEditor::Free()
{
    __super::Free();

	for (auto& pParticleSystem : m_vecParticleSystems)
		Safe_Release(pParticleSystem);
	m_vecParticleSystems.clear();

	Safe_Release(m_pParser);
    Safe_Release(m_pGameInstance);
}
