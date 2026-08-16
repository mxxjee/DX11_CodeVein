#include "Engine_Define.h"
#include "UIAnimClip.h"
#include "UIObject.h"


Engine::UIAnimClip::UIAnimClip()
{
}

Engine::UIAnimClip::UIAnimClip(const UIAnimClip& rhs)
	:ClipName(rhs.ClipName), m_fTotalDuration(rhs.m_fTotalDuration),m_bLoop(rhs.m_bLoop),
	m_EndFunction(rhs.m_EndFunction), m_pOwner(rhs.m_pOwner)
{
	m_AnimTracks.reserve(rhs.m_AnimTracks.size());
	for (auto& pSrcTrack : rhs.m_AnimTracks)
	{
		UIAnimTrack* pNewTrack = new UIAnimTrack(*pSrcTrack);
		m_AnimTracks.push_back(pNewTrack);
	}




}

Engine::UIAnimClip::~UIAnimClip()
{
}

HRESULT Engine::UIAnimClip::Initialize(void* _arg)
{
	static int Number = 0;

	if (_arg)
	{
		UIAnimClipDesc* pDesc = static_cast<UIAnimClipDesc*>(_arg);
		ClipName = pDesc->ClipName;

	
	}
	else
	{
		ClipName = "DefaultAnim" + to_string(Number++);
	}

	m_AnimTracks.reserve(10);

	return S_OK;
}

void Engine::UIAnimClip::Play_Init()
{
	for (auto& pTrack : m_AnimTracks)
	{
		if (pTrack->m_eEase == EASETYPE::CURRENT)
		{
			UITransform pTransform = m_pOwner->Get_LocalTransform();

			switch (pTrack->m_eAnimType)
			{
			case UIANIMTYPE::SCALE:
			{
				pTrack->m_vStartValue = _float3(pTransform.m_fSizeX, pTransform.m_fSizeY, 1.f);
				m_pOwner->Set_Size(pTrack->m_vStartValue.x, pTrack->m_vStartValue.y);

			}
			break;

			case UIANIMTYPE::ROTATION:
			{
				pTrack->m_vStartValue = _float3(0.f,0.f, pTransform.m_fRotationZ);
				m_pOwner->Set_Rotation(pTrack->m_vStartValue.z);

			}
				break;


			case UIANIMTYPE::POSITION:
			{
				
				pTrack->m_vStartValue = _float3(pTransform.m_fX, pTransform.m_fY, 1.f);
				m_pOwner->Set_Position(pTrack->m_vStartValue.x, pTrack->m_vStartValue.y);
			}
				break;

			case UIANIMTYPE::ALPHA:
			{

				pTrack->m_vStartValue = _float3(pTransform.m_fAlpha, 0.f, 1.f);
				m_pOwner->Set_Alpha(pTrack->m_vStartValue.x);
			}
			break;

			case UIANIMTYPE::UVSCROLL:
			{

				pTrack->m_vStartValue = _float3(pTransform.m_UVOffset.x, pTransform.m_UVOffset.y, 1.f);

			}
			break;

			case UIANIMTYPE::COLOR:
			{

				pTrack->m_vStartValue = _float3(pTransform.m_UVOffset.x, pTransform.m_UVOffset.y, 1.f);
				m_pOwner->Set_Color(_float4(pTrack->m_vStartValue.x, pTrack->m_vStartValue.y, pTrack->m_vStartValue.z, 1.f));
			}
			break;

			default:
				break;
			}
		}
	}

}

void Engine::UIAnimClip::End_Anim()
{
	for (auto& pTrack : m_AnimTracks)
	{

		UITransform pTransform = m_pOwner->Get_LocalTransform();

		switch (pTrack->m_eAnimType)
		{
		case UIANIMTYPE::SCALE:
		{
			//m_pOwner->Set_Size(pTrack->m_vEndValue.x, pTrack->m_vEndValue.y);

		}
		break;

		case UIANIMTYPE::ROTATION:
		{
			//m_pOwner->Set_Rotation(pTrack->m_vEndValue.z);

		}
		break;


		case UIANIMTYPE::POSITION:
		{
			//m_pOwner->Set_Position(pTrack->m_vEndValue.x, pTrack->m_vEndValue.y);
		}
		break;

		case UIANIMTYPE::ALPHA:
		{
			m_pOwner->Set_Alpha(pTrack->m_vEndValue.x);
		}
		break;

		case UIANIMTYPE::COLOR:
		{

			m_pOwner->Set_Color(_float4(pTrack->m_vEndValue.x, pTrack->m_vEndValue.y, pTrack->m_vEndValue.z, 1.f));
		}
		break;

		default:
			break;
		}

	}
}

_int Engine::UIAnimClip::Update(_float CurrentTime,const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pOwner, 0);
	
	for (auto& pAnimTrack : m_AnimTracks)
	{
		_float fEndTime = pAnimTrack->m_fStartTime + pAnimTrack->m_fDuration;

		/*범위에 있을때 */
		if (pAnimTrack->m_fStartTime <= CurrentTime && fEndTime >=CurrentTime)
		{
			//트랙 내부에서의 localtime구하기(lerp에 사용함)
			_float	fLocalTime = CurrentTime - pAnimTrack->m_fStartTime;
		
			if (m_bPingpoing)
			{
				_float fHalf = pAnimTrack->m_fDuration * 0.5f;

				if (fLocalTime <= fHalf)
					fLocalTime *= 2.0f;

				else
					fLocalTime = (pAnimTrack->m_fDuration - fLocalTime) * 2.f;

			}
			_float fRatio = fLocalTime / pAnimTrack->m_fDuration;

                                                                                                                                                                                                                                                                                                                                                                                                                         			fRatio = clamp<float>(fRatio, 0.f, 1.f);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
			_vector vStartVec = XMLoadFloat3(&pAnimTrack->m_vStartValue);
			_vector vSEndVec = XMLoadFloat3(&pAnimTrack->m_vEndValue);
			_float3 vResult;
			XMStoreFloat3(&vResult, vStartVec);

			switch (pAnimTrack->m_eAnimType)
			{
			case UIANIMTYPE::POSITION:
			{

				if(pAnimTrack->m_eEase==EASETYPE::LINEAR || pAnimTrack->m_eEase == EASETYPE::CURRENT)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));


				else if (pAnimTrack->m_eEase == EASETYPE::SHAKE)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, ShakeDecay(fRatio)));


				else if (pAnimTrack->m_eEase == EASETYPE::SMOOTH)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, SmootherStep(fRatio)));
				
				else if (pAnimTrack->m_eEase == EASETYPE::BOUNCE)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, EaseOutBounce(fRatio)));

				else
				{
					if (fRatio == 1)
						XMStoreFloat3(&vResult, vSEndVec);
				}
				m_pOwner->Set_Position(vResult.x, vResult.y);

			}
				break;

			case UIANIMTYPE::ROTATION:
			{
				XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));
				m_pOwner->Set_Rotation(vResult.z);

			}
				break;


			case UIANIMTYPE::SCALE:
			{
				if (pAnimTrack->m_eEase == EASETYPE::LINEAR || pAnimTrack->m_eEase == EASETYPE::CURRENT)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));
				
				else if (pAnimTrack->m_eEase == EASETYPE::SHAKE)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, ShakeDecay(fRatio)));


				else if (pAnimTrack->m_eEase == EASETYPE::SMOOTH)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, SmootherStep(fRatio)));

				else if (pAnimTrack->m_eEase == EASETYPE::BOUNCE)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, EaseOutBounce(fRatio)));

				m_pOwner->Set_Size(vResult.x,vResult.y);

			}
			break;

			case UIANIMTYPE::ALPHA:
			{

				if (pAnimTrack->m_eEase == EASETYPE::LINEAR || pAnimTrack->m_eEase == EASETYPE::CURRENT)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));

				else
				{
					if (fRatio == 1)
						XMStoreFloat3(&vResult, vSEndVec);
				}
				m_pOwner->Set_Alpha(vResult.x);


			}
			break;

			case UIANIMTYPE::PROGRESS:
			{

				if (pAnimTrack->m_eEase == EASETYPE::LINEAR || pAnimTrack->m_eEase == EASETYPE::CURRENT)
					XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));

				else
				{
					if (fRatio == 1)
						XMStoreFloat3(&vResult, vSEndVec);
				}
				
				m_pOwner->Set_AnimValue(UIANIMTYPE::PROGRESS, vResult);


			}
			break;

			case UIANIMTYPE::UVSCROLL:
			{
				_float2 vCurrentOffset;
				vCurrentOffset.x = pAnimTrack->m_vStartValue.x + (pAnimTrack->m_vEndValue.x - pAnimTrack->m_vStartValue.x) * fRatio;
				vCurrentOffset.y = pAnimTrack->m_vStartValue.y + (pAnimTrack->m_vEndValue.y - pAnimTrack->m_vStartValue.y) * fRatio;
				m_pOwner->Set_UVOffSet(vCurrentOffset);

			}
			break;

			case UIANIMTYPE::COLOR:
			{
				XMStoreFloat3(&vResult, XMVectorLerp(vStartVec, vSEndVec, fRatio));
				m_pOwner->Set_Color(_float4(vResult.x,vResult.y,vResult.z,1.f));


			}
			break;

			default:
				break;
			}

		}

	}

	return 0;
}

_int Engine::UIAnimClip::Update_Late(_float CurrentTime,const _float fTimeDelta)
{
	/*CHECK_NULL_RESULT(m_pOwner, 0);
	if (CurrentTime > m_fTotalDuration)
	{
		Finish_Clip();
	}
		return 0;*/

	return 0;
}


HRESULT Engine::UIAnimClip::Add_AnimTrack(UIAnimTrack* AnimTrack)
{
	m_AnimTracks.push_back(AnimTrack);
	
	sort(m_AnimTracks.begin(), m_AnimTracks.end(), [](const UIAnimTrack* A, const UIAnimTrack*B)
		{

			return A->m_fStartTime < B->m_fStartTime;
		});
	

	Update_TotalDuration();

    return S_OK;
}

HRESULT Engine::UIAnimClip::Remove_AnimTrack(_uint iIdx)
{
	if (iIdx >= m_AnimTracks.size())
		return E_FAIL;

	auto iter = m_AnimTracks.begin() + iIdx;
	Safe_Delete((*iter));
	m_AnimTracks.erase(iter);
	
	return S_OK;
}

void Engine::UIAnimClip::Compute_TotalDuration(_float* fOut)
{
	/*제일 긴 길이 찾아서 갱신*/
	float fDuration = FLT_MIN;
	for (auto& Track : m_AnimTracks)
	{
		float fTrackEndTime = Track->m_fStartTime + Track->m_fDuration;
		if (fDuration < fTrackEndTime)
			fDuration = fTrackEndTime;

	}

	*fOut = fDuration;

}

void Engine::UIAnimClip::Update_TotalDuration()
{
	Compute_TotalDuration(&m_fTotalDuration);
}

UIAnimClip* Engine::UIAnimClip::Create(void* arg)
{
	UIAnimClip* pInstance = new UIAnimClip();

	MSG_FAIL(pInstance->Initialize(arg), L"UIAnimClip 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


void Engine::UIAnimClip::Free()
{
	for (auto& pClip : m_AnimTracks)
	{
		Safe_Delete(pClip);
	}
    __super::Free();

}


HRESULT Engine::UIAnimClip::Save_AnimClip(string LocalFolder)
{
	ofstream fout(std::filesystem::path(LocalFolder), std::ios::binary);

	if (!fout.is_open())
		return E_FAIL;

	//이름저장[길이,문자]
	fs::directory_entry fileEntry = fs::directory_entry(LocalFolder);

	string FileName = fileEntry.path().stem().string();

	if (FileName != ClipName)
		ClipName = FileName;

	_uint iNameLength = (_uint)ClipName.length();
	fout.write((char*)&iNameLength, sizeof(_uint));
	fout.write(ClipName.c_str(), iNameLength);

	//기본속성
	fout.write((char*)&m_fTotalDuration, sizeof(_float));
	fout.write((char*)&m_bLoop, sizeof(_uint));


	//이벤트 저장
	

	//트랙저장(개수+실제데이터)
	_uint iTrackNumber =(_uint)m_AnimTracks.size();
	fout.write((char*)&iTrackNumber, sizeof(_uint));
	for (size_t i = 0; i < iTrackNumber; ++i)
	{
		UIAnimTrack* pTrack = m_AnimTracks[i];

		// 멤버
		fout.write((char*)&pTrack->m_eAnimType, sizeof(UIANIMTYPE));
		fout.write((char*)&pTrack->m_fStartTime, sizeof(float));
		fout.write((char*)&pTrack->m_fDuration, sizeof(float));
		fout.write((char*)&pTrack->m_vStartValue, sizeof(_float3));
		fout.write((char*)&pTrack->m_vEndValue, sizeof(_float3));
		fout.write((char*)&pTrack->m_eEase, sizeof(EASETYPE));
	}
	fout.close();
	return S_OK;
}

HRESULT Engine::UIAnimClip::Load_AnimClip(string LocalFolder)
{
	std::ifstream fin(LocalFolder, std::ios::binary);

	if (!fin.is_open()) return E_FAIL;

	//이름읽어오기
	_uint iNameLength = 0;
	fin.read((char*)&iNameLength, sizeof(_uint));

	ClipName.resize(iNameLength);
	fin.read(&ClipName[0], iNameLength);


	//속성채우기
	fin.read((char*)&m_fTotalDuration, sizeof(_float));
	fin.read((char*)&m_bLoop, sizeof(_uint));

	//트랙읽엉오기
	_uint iTrackCount = 0;
	fin.read((char*)&iTrackCount, sizeof(_uint));
	for (_uint i = 0; i < iTrackCount; ++i)
	{
		UIAnimTrack* pTrack = new UIAnimTrack(); // 생성자에서 초기화 보장

		// 멤버
		fin.read((char*)&pTrack->m_eAnimType, sizeof(UIANIMTYPE));
		fin.read((char*)&pTrack->m_fStartTime, sizeof(float));
		fin.read((char*)&pTrack->m_fDuration, sizeof(float));
		fin.read((char*)&pTrack->m_vStartValue, sizeof(_float3));
		fin.read((char*)&pTrack->m_vEndValue, sizeof(_float3));
		fin.read((char*)&pTrack->m_eEase, sizeof(EASETYPE));

		m_AnimTracks.push_back(pTrack);
	}

	fin.clear();
	fin.close();
	return S_OK;
}

void Engine::UIAnimClip::Set_Owner(UIObject* pOwner)
{
	m_pOwner = pOwner;

}

void Engine::UIAnimClip::Finish_Clip()
{
	//루프일때도 한사이클이 완료되었음을 표시하기 위해 사용

	CHECK_TRUE(ClipName == "");
	End_Anim();//끝값으로 설정

	if (m_EndFunction != nullptr)
	{

		// std::function은 내부에 실제 호출 대상이 있는지 bool로 판정 가능하네
		if (static_cast<bool>(m_EndFunction))
		{
			m_EndFunction();
		}
	}

	

}

void Engine::UIAnimClip::End_Clip()
{
	for (auto& pTrack : m_AnimTracks)
	{

		if (pTrack->m_eEase == EASETYPE::IMME)
		{
			if (pTrack->m_eAnimType == UIANIMTYPE::ALPHA)
				m_pOwner->Set_Alpha(pTrack->m_vEndValue.x);
		}

	}
}

void Engine::UIAnimClip::Bind_EndFunction(function<void()> Func)
{
	if (m_EndFunction==nullptr)
		m_EndFunction = Func;

}





void Engine::UIAnimClip::Copy_From(UIAnimClip* pSource)
{
	CHECK_JUST_NULL(pSource);

	ClipName = pSource->ClipName;
	m_fTotalDuration = pSource->m_fTotalDuration;
	m_bLoop = pSource->m_bLoop;
	m_pOwner = pSource->m_pOwner;
	//기존트랙삭제
	for (auto& pTrack : m_AnimTracks)
		Safe_Delete(pTrack);

	m_AnimTracks.clear();

	for (auto& pSrcTrack : *pSource->Get_Tracks())
	{
		UIAnimTrack* pNewTrack = new UIAnimTrack(*pSrcTrack); // 구조체 복사 생성자 활용
		m_AnimTracks.push_back(pNewTrack);
	}

}

UIAnimClip* Engine::UIAnimClip::Clone()
{
	UIAnimClip* pAnimClip = new UIAnimClip(*this);

#ifdef _DEBUG
	pAnimClip->pOriginalClip = this;
#endif // _DEBUG
	return pAnimClip;
}

#pragma region parsing

ordered_json	Engine::UIAnimClip::Save_Data()
{
	ordered_json j;
	j["ClipName"] = ClipName;
	j["Loop"] =		m_bLoop;
	j["PingPong"] = m_bPingpoing;
	j["TotalDuration"] = m_fTotalDuration;

	j["AnimTrack"] = ordered_json::array();

	for (auto& AnimTrack : m_AnimTracks)
		j["AnimTrack"].push_back(AnimTrack->To_Json());


	return j;

}
void Engine::UIAnimClip::Load_Data(ordered_json& json)
{
	ClipName = json.value("ClipName", "");
	m_bLoop = json.value("Loop", false);
	m_bPingpoing = json.value("PingPong", false);
	m_fTotalDuration = json.value("TotalDuration", 1.f);

	if (json.contains("AnimTrack") && json["AnimTrack"].is_array())
	{
		for (auto TrackData : json["AnimTrack"])
		{
			UIAnimTrack* pTrack = new UIAnimTrack;
			pTrack->Load_From(TrackData);
			m_AnimTracks.push_back(pTrack);

		}
	
	}

	
}
#pragma endregion
