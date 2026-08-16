#include "Engine_Define.h"
#include "UI_Animation.h"
#include "UIObject.h"

#ifdef _DEBUG
#include "Editor_Utils.h"
#include "ImguiManager.h"
#include "ImguiWindow.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif // _DEBUG


Engine::UI_Animation::UI_Animation()
	:UIComponent()
{
}

Engine::UI_Animation::UI_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIComponent(pDevice,pContext)
{
}

Engine::UI_Animation::UI_Animation(const UI_Animation& original)
	:UIComponent(original)
{
	
	m_iSavePriority = 2;

}

Engine::UI_Animation::~UI_Animation()
{
}

HRESULT Engine::UI_Animation::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT Engine::UI_Animation::Initialize(void* arg)
{
	if (arg == nullptr)
		return S_OK;

	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	return S_OK;
}

void Engine::UI_Animation::Add_AnimClips(UIAnimClip* Clip)
{
	Clip->Set_Owner(m_pOwner);
	m_AnimClips.push_back(Clip);
	m_MapAnimClips.emplace(Clip->Get_ClipName(), Clip);

#ifdef _DEBUG
	string ClipName = Clip->Get_ClipName();
	m_ClipNames.push_back(Clip->Get_ClipName());
	
#endif // _DEBUG

}

HRESULT Engine::UI_Animation::Play_AnimClip(string ClipName)
{
	auto iter = m_MapAnimClips.find(ClipName);
	if (iter != m_MapAnimClips.end())
	{
		
		m_CurrentClip = iter->second;
		m_fPlayTime = 0.f;
		m_bIsPlaying = true;
		m_pOwner->Set_Dirty(true);
		for (int i = 0; i < m_AnimClips.size(); ++i)
		{
			if (m_CurrentClip == m_AnimClips[i])
				m_CurrentClipIdx = i;
		}

		if (m_CurrentClip)
			m_CurrentClip->Play_Init();


		return S_OK;
	}
	

	return E_FAIL;
}

HRESULT Engine::UI_Animation::Play_AnimClip(_uint iIdx)
{
	if (iIdx >= m_AnimClips.size())
		return E_FAIL;

	m_CurrentClipIdx = iIdx;
	m_CurrentClip = m_AnimClips[iIdx];
	m_fPlayTime = 0.f;
	m_bIsPlaying = true;
	m_pOwner->Set_Dirty(true);
	if (m_CurrentClip)
		m_CurrentClip->Play_Init();


	return S_OK;
}

HRESULT Engine::UI_Animation::Play_AnimClip()
{
	m_fPlayTime = 0.f;
	m_bIsPlaying = true;
	m_pOwner->Set_Dirty(true);
	if (m_CurrentClip)
		m_CurrentClip->Play_Init();


	return S_OK;	
}

HRESULT Engine::UI_Animation::Stop_Animation()
{
	//m_fPlayTime = 0.f;
	m_bIsPlaying = false;

	return S_OK;	
}

UIAnimClip* Engine::UI_Animation::Get_AnimationClip(const string& Name)
{
	auto iter = m_MapAnimClips.find(Name);
	if (iter != m_MapAnimClips.end())
		return iter->second;

	return nullptr;
}


//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Animation::Update(const _float fTimeDelta)
{
	CHECK_FALSE_RESULT(m_bIsPlaying,0);
	CHECK_NULL_RESULT(m_CurrentClip, 0);
	_float fSafeTime = (fTimeDelta > 0.1f) ? 0.016f : fTimeDelta;

	m_CurrentClip->Update(m_fPlayTime, fSafeTime);
	m_fPlayTime += fSafeTime;



	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Animation::Update_Late(const _float fTimeDelta)
{
	CHECK_FALSE_RESULT(m_bIsPlaying,0);
	CHECK_NULL_RESULT(m_CurrentClip, 0);


	m_CurrentClip->Update_Late(m_fPlayTime,fTimeDelta);

	if (m_CurrentClip->Get_TotalDuration() <= m_fPlayTime)
	{
		m_CurrentClip->Finish_Clip();

		if (m_CurrentClip->Is_Loop())
		{
			m_CurrentClip->End_Clip();
			Reset_PlayTime();
			
		}

		else
		{
			Reset_PlayTime();
			m_CurrentClip->End_Clip();
			m_bIsPlaying = false;

			if(m_pGameInstance->Get_EngineMode()==ENGINEMODE::CLIENT)
				m_CurrentClip = nullptr;

			m_pOwner->Set_Color(m_pOwner->Get_OriginColor());
		}
	}
	return 0;
}
void Engine::UI_Animation::Reset_Animation()
{
	
	m_fPlayTime = 0.f;
	m_bIsPlaying = false;


	if (m_CurrentClip)
	{
		m_CurrentClip->Play_Init();
	}

	if (m_pOwner)
	{
	
	
		m_pOwner->Set_Dirty(true);
	}
}
void Engine::UI_Animation::Change_Clip(UIAnimClip* pClip, _uint iIdx)
{
	CHECK_JUST_NULL(pClip);
	CHECK_TRUE(iIdx >= m_AnimClips.size());

	m_AnimClips[iIdx] = pClip;
	
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/

#ifdef _DEBUG
void Engine::UI_Animation::Render_Imgui()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f)); // 하늘색 포인트
	ImGui::PopStyleColor();
	ImGui::Separator();
	ImGui::Text(" [ Component: UI Animation ] ");
	ImGui::Spacing();

	//현재 상태표시
	string CurrentClipName = m_CurrentClip ? m_CurrentClip->Get_ClipName() : "No Clip Selected";

	ImGui::TextDisabled("Status:");
	ImGui::SameLine();
	ImGui::Text(m_bIsPlaying ? "Playing" : "Stopped");


	//현재 선택된 애니메이션
	ImGui::SetNextItemWidth(-1); // 가로 꽉 채우기
	
	char buff[128];
	strcpy_s(buff, sizeof(buff), CurrentClipName.c_str());

	if (!strcmp(buff,""))
		strcpy_s(buff, sizeof(buff), "None");

	if (ImGui::Button(buff, ImVec2(-1, 30))) // 높이감을 줘서 클릭하기 쉽게
	{
		ImGui::OpenPopup("AnimationClipList");
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Click to switch between owned animation clips.");

	//현재 이 애니메이션 컴포넌트에 애니메이션 리스트표시(Add,delete)
	//리스트 중 하나 누르면 애니메이션 정보표시
	Render_CurrentAnimationList();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	//관리 섹션 (Load / Create)
	// 버튼을 가로로 나란히 배치 (SameLine 활용)
	float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.3f;

	if (ImGui::Button("Load Clip", ImVec2(buttonWidth, 0)))
	{
		On_LoadAnimationFile(); // 
	}
	ImGui::SameLine();

	if (ImGui::Button("Save Clip", ImVec2(buttonWidth, 0)))
	{
		On_SaveAnimationFile(); // 
	}
	ImGui::SameLine();
	
	if (ImGui::Button("Create New", ImVec2(buttonWidth, 0)))
	{
		On_CreateNewClip();
	}

	ImGui::Spacing();
}
void Engine::UI_Animation::Render_CurrentAnimationList()
{

	if (ImGui::BeginPopup("AnimationClipList"))
	{
		EditorUtils::Render_Search(m_HierarchyFilter);
		ImGui::Separator();

		m_ComboItems.clear();
		for (auto& pair : m_MapAnimClips)
			m_ComboItems.push_back(pair.first.c_str());
		if (m_CurrentClip)
		{
			char szBuff[128] = {};
			strcpy_s(szBuff, sizeof(szBuff), m_CurrentClip->Get_ClipName().c_str());
			if (ImGui::InputText("ClipName", szBuff, sizeof(szBuff),ImGuiInputTextFlags_EnterReturnsTrue))
			{//map변경m_AnimClips
				auto iter = m_MapAnimClips.find(m_CurrentClip->Get_ClipName());
				if (iter != m_MapAnimClips.end())
				{
					m_MapAnimClips.erase(iter);
					m_MapAnimClips.emplace(szBuff, m_CurrentClip);

				

				}
				m_CurrentClip->Set_ClipName(szBuff);
				Update_All_ClipName();
			}
		}
		int iTempIdx = (int)m_CurrentClipIdx;
		if (ImGui::BeginCombo("ClipList##UniqueAnimCombo", m_CurrentClip ? m_CurrentClip->Get_ClipName().c_str() : "None"))
		{
			for (int i = 0; i < (int)m_AnimClips.size(); ++i)
			{
				bool isSelected = (m_CurrentClipIdx == i);


				if (ImGui::Selectable(m_AnimClips[i]->Get_ClipName().c_str(), isSelected))
				{

					ImguiWindow* pWindow = ImguiManager::GetInstance()->Find_Window("AnimationClipEditor");
					if (pWindow)
						pWindow->Set_Open(true);

					Play_AnimClip(i); // 여기서 인덱스 기반으로 플레이 및 현재 클립 세팅
				}

				// 포커스 설정 (키보드 화살표 이동 시 필요)
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::EndPopup();
	}
}
void Engine::UI_Animation::On_LoadAnimationFile()
{
	TCHAR exePath[MAX_PATH];
	GetModuleFileName(nullptr, exePath, MAX_PATH);

	// exePath = "C:\Project\Bin\Game.exe"
	PathRemoveFileSpec(exePath);
	// exePath = "C:\Project\Bin"

	PathAppend(exePath, TEXT("..\\..\\Resources\\UI\\UIAnims"));

	// 파일 열기 다이얼로그
	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_pGameInstance->Get_Hwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("UI Animation Files(*.uianim)\0*.uianim\0All Files\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = exePath;  // 초기 경로
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		// 파일이 선택되었을 때
		Load_AnimClip_From_FilePath(wstringToString(szFile));

		
	}
}
void Engine::UI_Animation::On_SaveAnimationFile()
{
	CHECK_JUST_NULL(m_CurrentClip);

	TCHAR exePath[MAX_PATH];
	GetModuleFileName(nullptr, exePath, MAX_PATH);

	// exePath = "C:\Project\Bin\Game.exe"
	PathRemoveFileSpec(exePath);
	// exePath = "C:\Project\Bin"

	PathAppend(exePath, TEXT("..\\..\\Resources\\UI\\UIAnims"));

	// 파일 열기 다이얼로그
	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };
	wstring WClipName = stringToWstring(m_CurrentClip->Get_ClipName());
	lstrcpy(szFile, WClipName.c_str());


	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_pGameInstance->Get_Hwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("UI Animation Files(*.uianim)\0*.uianim\0All Files\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = exePath;  // 초기 경로
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		// 파일이 선택되었을 때
		Save_AnimClip_From_FilePath(wstringToString(szFile));


	}
}
void Engine::UI_Animation::Load_AnimClip_From_FilePath(string path)
{
	UIAnimClip* pLoadClip = UIAnimClip::Create(nullptr);
	pLoadClip->Load_AnimClip(path);

	Add_AnimClips(pLoadClip);

}
void Engine::UI_Animation::Save_AnimClip_From_FilePath(string path)
{
	CHECK_JUST_NULL(m_CurrentClip);
	path.append(".uianim");
	m_CurrentClip->Save_AnimClip(path);

}
void Engine::UI_Animation::On_CreateNewClip()
{
	//새로운 클립을 만들고, animtion editor창을 열게한다.

	UIAnimClip* pClip = UIAnimClip::Create(nullptr);
	Add_AnimClips(pClip);

	ImguiWindow* pWindow = ImguiManager::GetInstance()->Find_Window("AnimationClipEditor");
	if (pWindow)
		pWindow->Set_Open(true);

	m_CurrentClip = pClip;
	m_CurrentClipIdx = (int)m_AnimClips.size() - 1;

}
void Engine::UI_Animation::Update_ClipName(_uint idx, string NewName)
{
	CHECK_TRUE(idx >= m_AnimClips.size());

	m_AnimClips[idx]->Set_ClipName(NewName);
}
void Engine::UI_Animation::Update_All_ClipName()
{
	m_ClipNames.clear();
	
	for (auto& pClip : m_AnimClips)
	{
		m_ClipNames.push_back(pClip->Get_ClipName());
	}
}
void Engine::UI_Animation::Set_PreviewClip(UIAnimClip* pPreviewClip)
{
	m_CurrentClip = pPreviewClip; // 임시로 사본을 현재 클립으로 설정
	m_fPlayTime = 0.f;
	m_bIsPlaying = false;

}
#endif // _DEBUG

UI_Animation* Engine::UI_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Animation* pInstance = new UI_Animation(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Animation 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_Animation::Clone(void* arg)
{
	UI_Animation* pInstance = new UI_Animation(*this);
	MSG_FAIL(pInstance->Initialize(arg), L"UI_Animation 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

void Engine::UI_Animation::Free()
{
	for (auto& Clip : m_AnimClips)
		Safe_Release(Clip);

	__super::Free();
}

#pragma region parsing
void	Engine::UI_Animation::Save_Data(ordered_json& pJson)
{
	pJson["AnimClip"] = ordered_json::array();
	for (auto& AnimClip : m_AnimClips)
	{
		ordered_json clipData = AnimClip->Save_Data();
		
		COUT("Clip: " << AnimClip->Get_ClipName() << " | Tracks: " << AnimClip->Get_Tracks()->size());
		COUT("JSON Output: " << clipData.dump());
		
		pJson["AnimClip"].push_back(clipData);
	}



}
void Engine::UI_Animation::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data, eType);
	CHECK_JUST_NULL(pOwner);

	ordered_json root = Data.ComJson;
	
	if (root.contains("AnimClip") && root["AnimClip"].is_array())
	{
		for (auto& pData : root["AnimClip"])
		{
			UIAnimClip* pAnimClip = UIAnimClip::Create(nullptr);
			pAnimClip->Load_Data(pData);
			Add_AnimClips(pAnimClip);

		}
	}

	vOriginColor = m_pOwner->Get_OriginColor();
	*eType = UITYPE::ANIMATION;


}
#pragma endregion
