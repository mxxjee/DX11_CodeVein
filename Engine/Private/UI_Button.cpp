#include "Engine_Define.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UIObject.h"
#include "NewTexture.h"

#ifdef _DEBUG
#include "Editor_Utils.h"
#endif // _DEBUG

Engine::UI_Button::UI_Button()
	:UIComponent()
{
	for (_uint i = 0; i < _UINT(BUTTONSTATE::END); ++i)
	{
		m_TexKeys[i] = "";
		m_TexIdx[i] = 0;
	}
}

Engine::UI_Button::UI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIComponent(pDevice, pContext)
{
	for (_uint i = 0; i < _UINT(BUTTONSTATE::END); ++i)
	{
		m_TexKeys[i] = "";
		m_TexIdx[i] = 0;
	}
}

Engine::UI_Button::UI_Button(const UI_Button& original)
	:UIComponent(original)
{
	for (_uint i = 0; i < _UINT(BUTTONSTATE::END); ++i)
	{
		m_TexKeys[i] = "";
		m_TexIdx[i] = 0;
	}
}

Engine::UI_Button::~UI_Button()
{
}

HRESULT Engine::UI_Button::Initialize_Prototype()
{
	m_bAbleToBind = false;
	return S_OK;
}

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Button::Update(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Button::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/


#ifdef _DEBUG
void		Engine::UI_Button::Render_Imgui()
{

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("ButtonComponent");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();


	
	if (m_pProtoTexture)
	{
		EditorUtils::Draw_ResourceSelector("Default Texture", "Default TextureList", m_HierarchyFilter, m_pProtoTexture, &m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)], &m_TexIdx[_UINT(BUTTONSTATE::DEFAULT)]);
		if (ImGui::Button("Set Empty Image"))
		{
			string EmptyImg = "Textures/Empty";
			m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)] = EmptyImg;
			m_TexIdx[_UINT(BUTTONSTATE::DEFAULT)] = m_pProtoTexture->Get_TextureIdx(EmptyImg);
		}
		EditorUtils::Draw_ResourceSelector("Hover Texture", "Hover TextureList", m_HierarchyFilter, m_pProtoTexture, &m_TexKeys[_UINT(BUTTONSTATE::HOVER)], &m_TexIdx[_UINT(BUTTONSTATE::HOVER)]);
		if (ImGui::Button("##Set Empty Image-HOVER"))
		{
			string EmptyImg = "Textures/Empty";
			m_TexKeys[_UINT(BUTTONSTATE::HOVER)] = EmptyImg;
			m_TexIdx[_UINT(BUTTONSTATE::HOVER)] = m_pProtoTexture->Get_TextureIdx(EmptyImg);
		}

		EditorUtils::Draw_ResourceSelector("Pressed Texture", "Pressed TextureList", m_HierarchyFilter, m_pProtoTexture, &m_TexKeys[_UINT(BUTTONSTATE::PRESSED)], &m_TexIdx[_UINT(BUTTONSTATE::PRESSED)]);
		if (ImGui::Button("Set Empty Image-PRESSED"))
		{
			string EmptyImg = "Textures/Empty";
			m_TexKeys[_UINT(BUTTONSTATE::PRESSED)] = EmptyImg;
			m_TexIdx[_UINT(BUTTONSTATE::PRESSED)] = m_pProtoTexture->Get_TextureIdx(EmptyImg);
		}

		EditorUtils::Draw_ResourceSelector("Disabled Texture", "Disabled TextureList", m_HierarchyFilter, m_pProtoTexture, &m_TexKeys[_UINT(BUTTONSTATE::DISABLED)], &m_TexIdx[_UINT(BUTTONSTATE::DISABLED)]);
		if (ImGui::Button("##Set Empty Image - DISABLED"))
		{
			string EmptyImg = "Textures/Empty";
			m_TexKeys[_UINT(BUTTONSTATE::DISABLED)] = EmptyImg;
			m_TexIdx[_UINT(BUTTONSTATE::DISABLED)] = m_pProtoTexture->Get_TextureIdx(EmptyImg);
		}

	}
	
	Show_EventList();


	///////////////////m_HoverExitAnimClipNamex///////////////////////////////
	if (m_pGameInstance->Get_EngineMode() == EDITOR)
	{
		if (m_pUIImage)
		{
			m_pUIImage->Set_CurrentTexKey(m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)]);
			m_pUIImage->Set_TargetTexIdx(m_TexIdx[_UINT(BUTTONSTATE::DEFAULT)]);
		}
	}
}


void Engine::UI_Button::Show_EventList()
{
	Show_ClickEvents();
	char szBuff[128];
	strcpy_s(szBuff, sizeof(szBuff), m_ClickAnimClipName.c_str());
	if (ImGui::InputText("ClickEvent's AnimName", szBuff, sizeof(szBuff)))
	{
		m_ClickAnimClipName = szBuff;                                                                                                                                                                                                                                                                                                                              m_ClickAnimClipName = szBuff;
	}

	if (ImGui::Button("Create ClickEvent"))
		Add_Event_On_Imgui(BUTTONSTATE::PRESSED);

	ImGui::SameLine();
	if (ImGui::Button("Play ClickAnimation"))
	{
		//강제로바꿔서테스트하기
		m_eButtonState = BUTTONSTATE::DEFAULT;
		Change_ButtonState(BUTTONSTATE::PRESSED);
	}


	//////////////////////////////////////////////
	Show_HoverEvents();
	char HoverBuff[128] = {};
	strcpy_s(HoverBuff, sizeof(HoverBuff), m_HoverAnimClipName.c_str());
	if (ImGui::InputText("HoverEvent's AnimName", HoverBuff, sizeof(HoverBuff)))
	{
		m_HoverAnimClipName = HoverBuff;
	}
	if (ImGui::Button("Create HoverEvent"))
		Add_Event_On_Imgui(BUTTONSTATE::HOVER);
	ImGui::SameLine();
	if (ImGui::Button("Play HoverAnimation"))
	{
		m_eButtonState = BUTTONSTATE::DEFAULT;
		Change_ButtonState(BUTTONSTATE::HOVER);
	}

	///////////////////////////////////////////////////////////////////
	Show_HoverExitEvents();
	char ExitBuff[128] = {};
	strcpy_s(ExitBuff, sizeof(ExitBuff), m_HoverExitAnimClipName.c_str());
	if (ImGui::InputText("HoverExitEvent's AnimName", ExitBuff, sizeof(ExitBuff)))
	{
		m_HoverExitAnimClipName = ExitBuff;
	}

	if (ImGui::Button("Create HoverExitEvent"))
		Add_Event_On_Imgui(BUTTONSTATE::DEFAULT);

	ImGui::SameLine();
	if (ImGui::Button("Play HoverExitAnimation"))
	{
		m_eButtonState = BUTTONSTATE::DEFAULT;
		OnHoverExit();
	}
}
void Engine::UI_Button::Show_ClickEvents()
{
	for (size_t i = 0; i < OnClickEvents.size(); ++i)
	{
		auto& Event = OnClickEvents[i];


		char treeLabel[128];
		sprintf_s(treeLabel, "OnClickEvent [%d] : %s###ClickEvent_%d",
			(int)i, Event.m_ActionName.c_str(), (int)i);


		if (ImGui::TreeNode(treeLabel))
		{
			//Target Source (Enum Combo)

			char buff[128];
			strcpy_s(buff, sizeof(buff), OnClickEvents[i].m_ActionName.c_str());
			if (ImGui::InputText("Action Name", buff, sizeof(buff)))
			{
				OnClickEvents[i].m_ActionName = buff;
				OnClickEvents[i].m_ActionStrHash = hash<string>{}(OnClickEvents[i].m_ActionName);

			}

			const char* Combo[3] = { "COMPONENT","UIOBJECT","SYSTEM" };
			int curType = (int)OnClickEvents[i].m_EventTarget;
			if (ImGui::Combo("Event Target Type", &curType, Combo, IM_ARRAYSIZE(Combo)))
			{
				OnClickEvents[i].m_EventTarget = (UI_EVENT_TARGET)(curType);

			}

			if (ImGui::InputFloat("Publish Value", &OnClickEvents[i].m_fValue, sizeof(_float)))
			{

			}

			if (ImGui::Checkbox("Publish Flag", &OnClickEvents[i].m_bFlag))
			{

			}

			char PublishText[128];
			strcpy_s(PublishText, sizeof(PublishText), wstringToString(OnClickEvents[i].m_Text).c_str());
			if (ImGui::InputText("Publish Text", PublishText, sizeof(PublishText)))
			{
				OnClickEvents[i].m_Text = stringToWstring(PublishText);
			}


			// 삭제 버튼
			if (ImGui::Button("Remove Binding", ImVec2(-1, 0))) {
				OnClickEvents.erase(OnClickEvents.begin() + i);
				ImGui::TreePop();
				break;
			}

			ImGui::TreePop();
		}
	}
}
void Engine::UI_Button::Show_HoverEvents()
{
	for (size_t i = 0; i < OnHoverEvents.size(); ++i)
	{
		auto& Event = OnHoverEvents[i];


		char treeLabel[128];
		sprintf_s(treeLabel, "OnHoverEvents [%d] : %s###OnHoverEvents%d",
			(int)i, Event.m_ActionName.c_str(), (int)i);


		if (ImGui::TreeNode(treeLabel))
		{
			//Target Source (Enum Combo)

			char buff[128];
			strcpy_s(buff, sizeof(buff), OnHoverEvents[i].m_ActionName.c_str());
			if (ImGui::InputText("Action Name", buff, sizeof(buff)))
			{
				OnHoverEvents[i].m_ActionName = buff;
				OnHoverEvents[i].m_ActionStrHash = hash<string>{}(OnHoverEvents[i].m_ActionName);

			}
			const char* Combo[3] = { "COMPONENT","UIOBJECT","SYSTEM" };
			int curType = (int)OnHoverEvents[i].m_EventTarget;
			if (ImGui::Combo("Event Target Type", &curType, Combo, IM_ARRAYSIZE(Combo)))
			{
				OnHoverEvents[i].m_EventTarget = (UI_EVENT_TARGET)(curType);

			}
			if (ImGui::InputFloat("Publish Value", &OnHoverEvents[i].m_fValue, sizeof(_float)))
			{

			}

			if (ImGui::Checkbox("Publish Flag", &OnHoverEvents[i].m_bFlag))
			{

			}

			char PublishText[128]="";
			string strConvert = wstringToString(OnHoverEvents[i].m_Text);
			size_t copySize = min(strConvert.length(), sizeof(PublishText) - 1);

			memcpy_s(PublishText, sizeof(PublishText), strConvert.c_str(), copySize);
			PublishText[copySize] = '\0';
			strcpy_s(PublishText, sizeof(PublishText), strConvert.c_str());
			if (ImGui::InputText("Publish Text", PublishText, sizeof(PublishText)))
			{
				
				OnHoverEvents[i].m_Text = stringToWstring(PublishText);
			}


			// 삭제 버튼
			if (ImGui::Button("Remove Binding", ImVec2(-1, 0))) {
				OnHoverEvents.erase(OnHoverEvents.begin() + i);
				ImGui::TreePop();
				break;
			}

			ImGui::TreePop();
		}
	}
}
void Engine::UI_Button::Show_HoverExitEvents()
{
	for (size_t i = 0; i < HoverExitEvents.size(); ++i)
	{
		auto& Event = HoverExitEvents[i];


		char treeLabel[128];
		sprintf_s(treeLabel, "HoverExitEvents [%d] : %s###HoverExitEvents%d",
			(int)i, Event.m_ActionName.c_str(), (int)i);


		if (ImGui::TreeNode(treeLabel))
		{
			//Target Source (Enum Combo)

			char buff[128];
			strcpy_s(buff, sizeof(buff), HoverExitEvents[i].m_ActionName.c_str());
			if (ImGui::InputText("Action Name", buff, sizeof(buff)))
			{
				HoverExitEvents[i].m_ActionName = buff;
				HoverExitEvents[i].m_ActionStrHash = hash<string>{}(HoverExitEvents[i].m_ActionName);

			}
			const char* Combo[3] = { "COMPONENT","UIOBJECT","SYSTEM" };
			int curType = (int)OnClickEvents[i].m_EventTarget;
			if (ImGui::Combo("Event Target Type", &curType, Combo, IM_ARRAYSIZE(Combo)))
			{
				OnClickEvents[i].m_EventTarget = (UI_EVENT_TARGET)(curType);

			}
			if (ImGui::InputFloat("Publish Value", &HoverExitEvents[i].m_fValue, sizeof(_float)))
			{

			}

			if (ImGui::Checkbox("Publish Flag", &HoverExitEvents[i].m_bFlag))
			{

			}

			char PublishText[128];
			
			strcpy_s(PublishText, sizeof(PublishText), wstringToString(HoverExitEvents[i].m_Text).c_str());
			if (ImGui::InputText("Publish Text", PublishText, sizeof(PublishText)))
			{
				
				HoverExitEvents[i].m_Text = stringToWstring(PublishText);
			}


			// 삭제 버튼
			if (ImGui::Button("Remove Binding", ImVec2(-1, 0))) {
				HoverExitEvents.erase(HoverExitEvents.begin() + i);
				ImGui::TreePop();
				break;
			}

			ImGui::TreePop();
		}
	}
}
void Engine::UI_Button::Add_Event_On_Imgui(BUTTONSTATE eState)
{
	UI_MasterEvent Event;
	switch (eState)
	{
	case Engine::UI_Button::HOVER:
		OnHoverEvents.push_back(Event);
		break;

	case Engine::UI_Button::PRESSED:
		OnClickEvents.push_back(Event);
		break;

	case Engine::UI_Button::DEFAULT:
		HoverExitEvents.push_back(Event);
		break;
	default:
		break;
	}
}

#endif // _DEBUG


HRESULT Engine::UI_Button::Initialize(void* arg)
{
	if (arg == nullptr)
		return S_OK;

	BUTTON_DESC* pButtonDesc = static_cast<BUTTON_DESC*>(arg);
	m_pOwner = pButtonDesc->pOwner;

	if (m_pOwner != nullptr)
	{

		m_pUIImage = dynamic_cast<UI_Image*>(m_pOwner->Get_UIRenderComponent());
		if (m_pUIImage)
		{
			m_pProtoTexture = m_pUIImage->Get_ProtoTextureComp();
			Safe_AddRef(m_pUIImage);

		}
		m_pOwner->Set_ButtonComponent(this);
	}



	if (arg != nullptr && m_pProtoTexture)
	{
		CHECK_FAILED(__super::Initialize(arg), E_FAIL);
		/*Transform 참조시 레퍼런스 카운트 증가*/
		BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(arg);

		if (pDesc)
		{
			if (pDesc->DefaultTexKey != "")
			{
				m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)] = pDesc->DefaultTexKey;
				m_TexIdx[_UINT(BUTTONSTATE::DEFAULT)] = m_pProtoTexture->Get_TextureIdx(pDesc->DefaultTexKey);

			}

			if (pDesc->HoverTexKey != "")
			{
				m_TexKeys[_UINT(BUTTONSTATE::HOVER)] = pDesc->HoverTexKey;
				m_TexIdx[_UINT(BUTTONSTATE::HOVER)] = m_pProtoTexture->Get_TextureIdx(pDesc->HoverTexKey);

			}

			if (pDesc->ClickTexKey != "")
			{
				m_TexKeys[_UINT(BUTTONSTATE::PRESSED)] = pDesc->ClickTexKey;
				m_TexIdx[_UINT(BUTTONSTATE::PRESSED)] = m_pProtoTexture->Get_TextureIdx(pDesc->ClickTexKey);

			}

			if (pDesc->DisableKey != "")
			{
				m_TexKeys[_UINT(BUTTONSTATE::DISABLED)] = pDesc->DisableKey;
				m_TexIdx[_UINT(BUTTONSTATE::DISABLED)] = m_pProtoTexture->Get_TextureIdx(pDesc->DisableKey);

			}

		}

	}
	


	Enter_State(BUTTONSTATE::DEFAULT);
	return S_OK;
}

UI_Button* Engine::UI_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Button* pInstance = new UI_Button(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Button 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}


Component* Engine::UI_Button::Clone(void* arg)
{
	UI_Button* pInstance = new UI_Button(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_Button 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

void Engine::UI_Button::Change_ButtonState(BUTTONSTATE eState)
{
	if (eState != m_eButtonState)
	{
		m_ePreButtonState = m_eButtonState;
		m_eButtonState = eState;
		Enter_State(m_eButtonState);
		 
	}
}

void Engine::UI_Button::Add_Event(BUTTONSTATE eState, UI_MasterEvent& Event)
{
	switch (eState)
	{

	case Engine::UI_Button::HOVER:
		OnHoverEvents.push_back(Event);
		break;

	case Engine::UI_Button::PRESSED:
		OnClickEvents.push_back(Event);
		break;

	case Engine::UI_Button::DEFAULT:
		HoverExitEvents.push_back(Event);
		break;
	default:
		break;
	}
}

void Engine::UI_Button::Enter_State(BUTTONSTATE eState)
{
	switch (eState)
	{
	case Engine::UI_Button::DEFAULT:

		break;
	case Engine::UI_Button::HOVER:
	{
		if (OnHoverFunc)
			OnHoverFunc();

		for (auto& pEvent : OnHoverEvents)
		{
			pEvent.m_iObjID = m_pOwner->Get_ObjectID();
			m_pGameInstance->Publish(pEvent);
		}
			

		//애님실행
		m_pOwner->Play_Animation(m_HoverAnimClipName);
	}
	break;
	case Engine::UI_Button::PRESSED:
	{
		if (OnClickFunc)
			OnClickFunc();


		for (auto& pEvent : OnClickEvents)
		{
			pEvent.m_iObjID = m_pOwner->Get_ObjectID();
			m_pGameInstance->Publish(pEvent);
		}
			

		m_pOwner->Play_Animation(m_ClickAnimClipName);
	}

	break;
	case Engine::UI_Button::DISABLED:
		break;
	case Engine::UI_Button::END:
		break;
	default:
		break;
	}


	if (m_pUIImage)
	{
		m_pUIImage->Set_CurrentTexKey(m_TexKeys[_UINT((BUTTONSTATE)eState)]);
		m_pUIImage->Set_TargetTexIdx(m_TexIdx[_UINT((BUTTONSTATE)eState)]);
	}

}

void Engine::UI_Button::OnHoverExit()
{
	//이벤트실행
	if (OnHoverExitFunc)
		OnHoverExitFunc();

	for (auto& pEvent : HoverExitEvents)
	{
		pEvent.m_iObjID = m_pOwner->Get_ObjectID();
		m_pGameInstance->Publish(pEvent);
	}

	m_pOwner->Play_Animation(m_HoverExitAnimClipName);
	//이후 다시 default상태로
	Change_ButtonState(BUTTONSTATE::DEFAULT);

}

void Engine::UI_Button::Free()
{
	Safe_Release(m_pUIImage);
	__super::Free();
}

#pragma region parsing
void Engine::UI_Button::Save_Data(ordered_json& pJson)
{

	pJson["Default Texture"] = m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)];
	pJson["Hover Texture"] = m_TexKeys[_UINT(BUTTONSTATE::HOVER)];
	pJson["Pressed Texture"] = m_TexKeys[_UINT(BUTTONSTATE::PRESSED)];
	pJson["Disable Texture"] = m_TexKeys[_UINT(BUTTONSTATE::DISABLED)];

	pJson["OnClickEvents"] = ordered_json::array();
	for (auto& pEvent : OnClickEvents)
	{
		json jOut = pEvent.To_Json();
		pJson["OnClickEvents"].push_back(jOut);
	}

	pJson["OnHoverEvents"] = ordered_json::array();
	for (auto& pEvent : OnHoverEvents)
	{
		json jOut = pEvent.To_Json();
		pJson["OnHoverEvents"].push_back(jOut);
	}

	pJson["HoverExitEvents"] = ordered_json::array();
	for (auto& pEvent : HoverExitEvents)
	{
		json jOut = pEvent.To_Json();
		pJson["HoverExitEvents"].push_back(jOut);
	}

	pJson["ClickAnimClipName"] = m_ClickAnimClipName;
	pJson["HoverAnimClipName"] = m_HoverAnimClipName;
	pJson["HoverExitAnimClipName"] = m_HoverExitAnimClipName;

}
void Engine::UI_Button::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data,eType);
	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;
	m_pOwner = dynamic_cast<UIObject*>(pOwner);

	

	m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)] = root.value("Default Texture", DEFAULT_TEXTUREKEY);
	m_TexKeys[_UINT(BUTTONSTATE::HOVER)] = root.value("Hover Texture", DEFAULT_TEXTUREKEY);
	m_TexKeys[_UINT(BUTTONSTATE::PRESSED)] = root.value("Pressed Texture", DEFAULT_TEXTUREKEY);
	m_TexKeys[_UINT(BUTTONSTATE::DISABLED)] = root.value("Disable Texture", DEFAULT_TEXTUREKEY);


	//이벤트읽기
	if (root.contains("OnClickEvents") && root["OnClickEvents"].is_array())
	{
		for (auto Data : root["OnClickEvents"])
		{
			UI_MasterEvent Event;
			Event.From_Json(Data);
			OnClickEvents.push_back(Event);

		}
	}

	//Hover이벤트읽기
	if (root.contains("OnHoverEvents") && root["OnHoverEvents"].is_array())
	{
		for (auto Data : root["OnHoverEvents"])
		{
			UI_MasterEvent Event;
			Event.From_Json(Data);
			OnHoverEvents.push_back(Event);

		}
	}

	//HoverExit이벤트읽기
	if (root.contains("HoverExitEvents") && root["HoverExitEvents"].is_array())
	{
		for (auto Data : root["HoverExitEvents"])
		{
			UI_MasterEvent Event;
			Event.From_Json(Data);
			HoverExitEvents.push_back(Event);

		}
	}

	//나머지 처리
	//오너 이미지 연결
	m_pUIImage = dynamic_cast<UI_Image*>(m_pOwner->Get_UIRenderComponent());
	if (m_pUIImage)
	{
		m_pProtoTexture = m_pUIImage->Get_ProtoTextureComp();
		Safe_AddRef(m_pUIImage);

	}

	//인덱스설정
	if (m_pProtoTexture)
	{
		m_TexIdx[_UINT(BUTTONSTATE::DEFAULT)] = m_pProtoTexture->Get_TextureIdx(m_TexKeys[_UINT(BUTTONSTATE::DEFAULT)]);
		m_TexIdx[_UINT(BUTTONSTATE::HOVER)] = m_pProtoTexture->Get_TextureIdx(m_TexKeys[_UINT(BUTTONSTATE::HOVER)]);
		m_TexIdx[_UINT(BUTTONSTATE::PRESSED)] = m_pProtoTexture->Get_TextureIdx(m_TexKeys[_UINT(BUTTONSTATE::PRESSED)]);
		m_TexIdx[_UINT(BUTTONSTATE::DISABLED)] = m_pProtoTexture->Get_TextureIdx(m_TexKeys[_UINT(BUTTONSTATE::DISABLED)]);

	}


	//애님설정
	m_ClickAnimClipName = root.value("ClickAnimClipName", "");
	m_HoverAnimClipName = root.value("HoverAnimClipName", "");
	m_HoverExitAnimClipName = root.value("HoverExitAnimClipName", "");

	*eType = UITYPE::BUTTON;

}
#pragma endregion