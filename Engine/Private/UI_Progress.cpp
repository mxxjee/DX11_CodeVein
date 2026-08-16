#include "Engine_Define.h"
#include "UI_Progress.h"
#include "UI_Image.h"
#include "UIObject.h"

#ifdef _DEBUG
#include "Editor_Utils.h"
#endif // _DEBUG


Engine::UI_Progress::UI_Progress()
	:UIComponent()
{
}

Engine::UI_Progress::UI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIComponent(pDevice, pContext)
{
}

Engine::UI_Progress::UI_Progress(const UI_Progress& original)
	:UIComponent(original)
{
}

Engine::UI_Progress::~UI_Progress()
{
}

HRESULT Engine::UI_Progress::Initialize_Prototype()
{
	m_Actions["UpdateHP"] =_UINT(PROGRESS_ACTION::SET_HP);
	
    return S_OK;
}

HRESULT Engine::UI_Progress::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	/*Transform 참조시 레퍼런스 카운트 증가*/
	if (m_pOwner != nullptr)
	{
		m_pUIImage = dynamic_cast<UI_Image*>(m_pOwner->Get_UIRenderComponent());
		if (m_pUIImage)
		{
			m_pProtoTexture = m_pUIImage->Get_ProtoTextureComp();
			Safe_AddRef(m_pUIImage);

		

		}

		if (arg)
		{
			UI_PROGRESSDESC* pDesc = static_cast<UI_PROGRESSDESC*>(arg);
			if (pDesc)
			{
				m_fMax = pDesc->fMax;
				m_fCurrent = pDesc->fCurrent;
				m_bLerpReach = pDesc->bLerpReach;
				m_fSpeed = pDesc->fSpeed;
				m_eType = pDesc->m_eProgressType;
			}

			if (m_pProtoTexture)
			{
				m_FillTextureKey = pDesc->TextureKey;// m_pUIImage->Get_CurrentTexKey();
				m_TargetIdx = m_pProtoTexture->Get_TextureIdx(m_FillTextureKey); //m_pUIImage->Get_CurrentTexIdx();

			}
		
		}


		if (m_pUIImage)
		{
			m_pUIImage->Set_CurrentTexKey(m_FillTextureKey);
			m_pUIImage->Set_TargetTexIdx(m_TargetIdx);
		}
	}



    return S_OK;
}

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Progress::Update(const _float fTimeDelta)
{

	
	
#ifdef _DEBUG
	if (m_pGameInstance->Get_EngineMode() == EDITOR)
	{
		m_fTargetRatio = m_PreviewCurrent / m_PreviewMax;
	}
	CHECK_NULL_RESULT(m_fMax, 0);
	CHECK_NULL_RESULT(m_fCurrent, 0);

	if (m_fCurrent != nullptr && m_fMax != nullptr)
	{
		m_fTargetRatio = (_float)*m_fCurrent / *m_fMax;

	}
	

#else
	if (m_fCurrent != nullptr && m_fMax != nullptr)
	{
		m_fTargetRatio = *m_fCurrent / *m_fMax;
	}

#endif // _DEBUG

	if (m_bDelayCompute)
	{
		if (m_fPreRatio != m_fTargetRatio)
			m_bStartDelay = true;

	}


	if (m_bStartDelay)
	{
		m_fCurrentDelayTime += fTimeDelta;
		if (m_fCurrentDelayTime >= m_fDelayTime)
		{
			m_bStartDelay = false;
			m_fCurrentDelayTime = 0.f;
		}
	}



	



	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Progress::Update_Late(const _float fTimeDelta)
{
	if (m_bStartDelay)
		return 0;

	//이게 true라면 서서히줄어드는거
	if (m_bLerpReach)
	{
		m_fRenderRatio = m_fRatio=fLerp(m_fRatio, m_fTargetRatio, m_fSpeed * fTimeDelta);
		m_fPreRatio = m_fTargetRatio;

		if (fabs(m_fRatio - m_fTargetRatio) < 0.01f)
		{
			m_fRatio = m_fTargetRatio;
		}

	}
	else
	{
		m_fRenderRatio = m_fRatio = m_fTargetRatio;
		m_fPreRatio = m_fTargetRatio;
	}
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/

//////////////////////////////////////////////////////// 바인드함수 ////////////////////////////////////////////////////////
HRESULT Engine::UI_Progress::Bind_Resource(Shader* pShader, UI_BUFFER_PACKET* pPacket)
{
	CHECK_NULLPTR(pPacket);
	CHECK_NULLPTR(pShader);
	
	pPacket->g_UIProgress = m_fRenderRatio;

	const UITransform& Transform = m_pOwner->Get_CombinedTransform();
	_float fLeftStart = Transform.m_fX - (Transform.m_fSizeX * 0.5f);
	pPacket->g_fClipX = g_fClipX=fLeftStart + (Transform.m_fSizeX * m_fRenderRatio);
	
	
	//CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_UIProgress, (_float*)&m_fRenderRatio, sizeof(_float)), E_FAIL);



	return 0;
}
/******************************************************* 바인드함수 *******************************************************/

//////////////////////////////이벤트 실행시키는 함수///////////////////
void		Engine::UI_Progress::Execute(const string& strAction, void* pData)
{
	auto iter =m_Actions.find(strAction);
	if (iter != m_Actions.end())
		return;

	UI_MasterEvent* pDesc = static_cast<UI_MasterEvent*>(pData);
	PROGRESS_ACTION ProgressAction = PROGRESS_ACTION(iter->second);

	switch (ProgressAction)
	{
	case PROGRESS_ACTION::SET_HP:
		*m_fCurrent = pDesc->m_fValue;
		break;

	default:
		break;
	}
	
}
/********************************************************************/
UI_Progress* Engine::UI_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Progress* pInstance = new UI_Progress(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Progress 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

#ifdef _DEBUG
void	Engine::UI_Progress::Render_Imgui()
{

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("ProgressComponent");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	EditorUtils::Draw_ResourceSelector("Default Texture", "Default TextureList", m_HierarchyFilter, m_pProtoTexture, &m_FillTextureKey, &m_TargetIdx);
	if (ImGui::Checkbox("Use LerpReach", &m_bLerpReach))
	{
	}

	if (m_bLerpReach)
	{
		if (ImGui::DragFloat("Speed", &m_fSpeed, 0.1f, 0.f, 100.f))
		{

		}
	}
	//Fillmode설정
	int			CurrentType = _UINT(m_eType);
	if (ImGui::Combo("FillMode", &CurrentType, ProgressTypestr, IM_ARRAYSIZE(ProgressTypestr)))
	{
		m_eType = ProgressType(CurrentType);
		m_pOwner->Set_PassNum(5 + CurrentType);

	
	}

	//MAx값설정
	if (ImGui::DragFloat("Max", &m_PreviewMax, 0.1f))
	{

	}

	//현재값 설정
	if(ImGui::DragFloat("CurrentValue", &m_PreviewCurrent, 0.01f, 0.f, m_PreviewMax))
	{

	}

	
	////////////////////////////////////////////////바꾼대로 적용시키기/////////////////////
	if (m_pGameInstance->Get_EngineMode() == EDITOR)
	{
		if (m_pUIImage)
		{
			m_pUIImage->Set_CurrentTexKey(m_FillTextureKey);
			m_pUIImage->Set_TargetTexIdx(m_TargetIdx);
		}
	}
}

#endif // _DEBUG


Component* Engine::UI_Progress::Clone(void* arg)
{
	UI_Progress* pInstance = new UI_Progress(*this);
	MSG_FAIL(pInstance->Initialize(arg), L"UI_Progress 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}


void Engine::UI_Progress::Free()
{
	Safe_Release(m_pUIImage);
	__super::Free();
}

#pragma region parsing

void Engine::UI_Progress::Save_Data(ordered_json& pJson)
{
	string ProgressType = "";
	switch (m_eType)
	{
	case Engine::UI_Progress::ProgressType::HORIZONTAL:
		ProgressType = "HORIZONTAL";
		break;

	case Engine::UI_Progress::ProgressType::HORIZONTAL_RE:
		ProgressType = "HORIZONTAL_RE";
		break;

	case Engine::UI_Progress::ProgressType::VERTICAL:
		ProgressType = "VERTICAL";
		break;

	case Engine::UI_Progress::ProgressType::VERTICAL_RE:
		ProgressType = "VERTICAL_RE";
		break;

	case Engine::UI_Progress::ProgressType::RADIAL:
		ProgressType = "RADIAL";
		break;

	case Engine::UI_Progress::ProgressType::CENTER:
		ProgressType = "CENTER";
		break;

	default:
		break;
	}

	pJson["ProgressType"] = ProgressType;
	pJson["FillTexture"] = m_FillTextureKey;
	pJson["LerpReach"] = m_bLerpReach;
	pJson["Speed"] = m_fSpeed;

	pJson["DelayCompute"] = m_bDelayCompute;
	pJson["DelayTime"] = m_fDelayTime;
}

void Engine::UI_Progress::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data,eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;

	if (root.contains("ProgressType"))
	{
		string strProgressType = root["ProgressType"];
		if (strProgressType == "HORIZONTAL")
			m_eType = ProgressType::HORIZONTAL;

		else if (strProgressType == "HORIZONTAL_RE")
			m_eType = ProgressType::HORIZONTAL;


		else if (strProgressType == "VERTICAL")
			m_eType = ProgressType::VERTICAL;


		else if (strProgressType == "VERTICAL_RE")
			m_eType = ProgressType::VERTICAL_RE;


		else if (strProgressType == "RADIAL")
			m_eType = ProgressType::RADIAL;

		else if (strProgressType == "CENTER")
			m_eType = ProgressType::CENTER;

	}


	m_FillTextureKey = root.value("FillTexture", "UI4_Main/White");
	m_bLerpReach = root.value("LerpReach", false);
	m_fSpeed = root.value("Speed", 5.f);

	m_bDelayCompute = root.value("DelayCompute", false);
	m_fDelayTime = root.value("DelayTime", 0.f);

	//나머지값 세팅
	m_pUIImage = dynamic_cast<UI_Image*>(m_pOwner->Get_UIRenderComponent());
	if (m_pUIImage)
	{
		m_pProtoTexture = m_pUIImage->Get_ProtoTextureComp();
		Safe_AddRef(m_pUIImage);

	}

	if (m_pProtoTexture)
	{
		m_TargetIdx = m_pProtoTexture->Get_TextureIdx(m_FillTextureKey); //m_pUIImage->Get_CurrentTexIdx();
		m_pUIImage->Set_CurrentTexKey(m_FillTextureKey);
		m_pUIImage->Set_TargetTexIdx(m_TargetIdx);
	}


	*eType = UITYPE::PROGRESSBAR;


}
#pragma endregion

///////////////////////////////////ETC///////////////
void Engine::UI_Progress::Compute_Ratio()
{
	if (m_fMax != nullptr && m_fCurrent != nullptr)
		m_fRatio = *m_fCurrent / *m_fMax;
}

void Engine::UI_Progress::Reset()
{
	m_fRatio = 0.f;
	m_fTargetRatio = 0.f;
	m_fRenderRatio = 0.f;
}