#include "Engine_Define.h"
#include "Animation.h"

#include "Channel.h"
#include "Model.h"
#include "GameInstance.h"
#include "Bone.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Animation::Animation()
{
}

Engine::Animation::Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

Engine::Animation::Animation(const Animation& original)
    : m_pGameInstance(original.m_pGameInstance)
    , m_bIsLoopAnim(original.m_bIsLoopAnim), m_bLinkedAnim(original.m_bLinkedAnim), m_bCompleteAnim(original.m_bCompleteAnim)
    , m_iNumChannels(original.m_iNumChannels), m_fTicksPerSecond(original.m_fTicksPerSecond), m_fDuration(original.m_fDuration), m_fCurrentFrame(original.m_fCurrentFrame)
    , m_iNextAnimIndex(original.m_iNextAnimIndex), m_fNextAnimLerpDuration(original.m_fNextAnimLerpDuration)
    , m_strName(original.m_strName)
    , m_vecChannels(original.m_vecChannels), m_vecCurrentKeyFrameIndices(original.m_vecCurrentKeyFrameIndices)
    , m_vecNotify(original.m_vecNotify), m_fEPS(original.m_fEPS), m_iNextNotifyId(original.m_iNextNotifyId)
{
    Safe_AddRef(m_pGameInstance);

    for (auto& channel : m_vecChannels)
    {
        Safe_AddRef(channel);
    }

    for (auto& notify : m_vecNotify)
    {
        notify.pCachedEvent = nullptr;
    }
}

Engine::Animation::~Animation()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Animation::Initialize_FBX(aiAnimation* _aiAnimation, Model* _model, myAnimation* _myAnimation)
{
    m_strName = _myAnimation->mName = _aiAnimation->mName.data;
    m_fDuration = _myAnimation->mDuration = (_float)_aiAnimation->mDuration;
    m_fTicksPerSecond = _myAnimation->mTickPerSecond = (_float)_aiAnimation->mTicksPerSecond;

    _uint numChannels = _aiAnimation->mNumChannels;

    m_vecChannels.reserve(numChannels);
    m_vecCurrentKeyFrameIndices.resize(numChannels);

    _myAnimation->mChannels.resize(numChannels);

    vector<_uint> deleteChannels;

    for (_uint i = 0; i < numChannels; ++i)
    {
        Channel* channel = Channel::Create(nullptr, nullptr, _aiAnimation->mChannels[i], _model, &_myAnimation->mChannels[i]);
        if (channel == nullptr)
        {
            deleteChannels.push_back(i);
            continue;
        }

        m_vecChannels.push_back(channel);
    }

    for (_int i = (_int)deleteChannels.size() - 1; i >= 0; --i)
    {
        _myAnimation->mChannels.erase(_myAnimation->mChannels.begin() + deleteChannels[i]);
        m_vecCurrentKeyFrameIndices.erase(m_vecCurrentKeyFrameIndices.begin() + deleteChannels[i]);
    }

    m_iNumChannels = _myAnimation->mNumChannels = (_uint)m_vecChannels.size();

    _myAnimation->mNextAnimIndex = -1;
    _myAnimation->mNextAnimLerpDuration = 0.2f;
    _myAnimation->mIsLoopAnim = false;
    _myAnimation->mLinkedAnim = false;
    _myAnimation->mCompleteAnim = false;


    //COUT(_aiAnimation->mNumMorphMeshChannels);

    return S_OK;
}

HRESULT Engine::Animation::Initialize_Binary(myAnimation* _myAnimation)
{
    m_strName = _myAnimation->mName;
    m_fDuration = _myAnimation->mDuration;
    m_fTicksPerSecond = _myAnimation->mTickPerSecond;

    m_iNextAnimIndex = _myAnimation->mNextAnimIndex;
    m_fNextAnimLerpDuration = _myAnimation->mNextAnimLerpDuration;
    m_bIsLoopAnim = _myAnimation->mIsLoopAnim;
    m_bLinkedAnim = _myAnimation->mLinkedAnim;
    m_bCompleteAnim = _myAnimation->mCompleteAnim;

    m_iNumChannels = _myAnimation->mNumChannels;

    m_vecChannels.reserve(m_iNumChannels);
    m_vecCurrentKeyFrameIndices.resize(m_iNumChannels);

    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        Channel* channel = Channel::Create(nullptr, nullptr, &_myAnimation->mChannels[i]);
        if (channel == nullptr)
            continue;

        m_vecChannels.push_back(channel);
    }

    m_iNumNotifies = (_uint)_myAnimation->mNotifyes.size();

    m_vecNotify.resize(m_iNumNotifies);

    for (_uint i = 0; i < m_iNumNotifies; ++i)
    {
        MYNOTIFY MyNotify = _myAnimation->mNotifyes[i];
        m_vecNotify[i].fFrame = MyNotify.fFrame;
        m_vecNotify[i].fStartFrame = MyNotify.fStartFrame;
        m_vecNotify[i].fEndFrame = MyNotify.fEndFrame;
        m_vecNotify[i].bActive = MyNotify.bActive;

        m_vecNotify[i].eNotify_Type = MyNotify.eNotify_Type;
        m_vecNotify[i].eNotify_Event = MyNotify.eNotify_Event;

        m_vecNotify[i].UmapEvent = MyNotify.UmapEvent;
        m_vecNotify[i].SocketName = MyNotify.SocketName;
        m_vecNotify[i].bAttached = MyNotify.bAttached;
        m_vecNotify[i].iNotifyId = MyNotify.iNotifyId;

        if (m_iNextNotifyId <= MyNotify.iNotifyId)
            m_iNextNotifyId = MyNotify.iNotifyId + 1;
    }

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 애니메이션 함수 ////////////////////////////////////////////////////////
_bool Engine::Animation::Update_TransformationMatrices(const vector<class Bone*>& _bones, const _float fTimeDelta, const _bool _isLoop, _float fAnimationSpeed)
{
    if (!m_IsStartFrame)
    {
        // 프레임마다 애니메이션의 현재 프레임 갱신
        m_fCurrentFrame += m_fTicksPerSecond * fAnimationSpeed * fTimeDelta; //애니메이션 재생 속도 함수 인자로 넘겨서 제어하려고 추가함

    }


    // 애니메이션의 최대 프레임보다 현재 프레임이 더 높아지면(애니메이션이 끝나면)
    if (m_fCurrentFrame >= m_fDuration)
    {
        // 루프가 아닐경우 끝났음을 리턴
        if (_isLoop == false)
        {
            return true;
        }
        if (m_iNextAnimIndex == -1)
        {

        }

        // 루프면 프레임을 처음으로
        m_fCurrentFrame = 0.f;
    }

    // 애니메이션이 움직여야 하는 본들 모두 업데이트
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecChannels[i]->Update_TransformationMatrix(_bones, m_fCurrentFrame, &m_vecCurrentKeyFrameIndices[i], m_bIsLoopAnim, m_fDuration);
    }

    m_IsStartFrame = false;
    return false;
}

_bool Engine::Animation::Update_TransformationMatrices_SelfLoop(const vector<class Bone*>& _bones, const _float fTimeDelta)
{
    if (!m_IsStartFrame)
    {
        // 프레임마다 애니메이션의 현재 프레임 갱신
        m_fCurrentFrame += m_fTicksPerSecond * fTimeDelta;
    }

    // 연결되어있는 애니메이션의 경우
    // 조금 더 빠르게 다음 애니메이션으로 감
    if (m_bLinkedAnim && m_iNextAnimIndex != -1)
    {
        if (m_fCurrentFrame + 0.04f >= m_fDuration)
        {
            m_pModel->Set_Animation(m_iNextAnimIndex, m_fNextAnimLerpDuration);

            return true;
        }
    }

    // 끝나야 다음으로 넘어가는 애니메이션의 경우
    if (m_bCompleteAnim && m_bStackNextAnim)
    {
        if (m_fCurrentFrame + 0.033f >= m_fDuration)
        {
            for (_uint i = 0; i < m_iNumChannels; ++i)
            {
                m_vecChannels[i]->Update_TransformationMatrix(_bones, m_fCurrentFrame, &m_vecCurrentKeyFrameIndices[i], m_bIsLoopAnim, m_fDuration);
            }

            m_bStackNextAnim = false;

            return true;
        }
    }

    if (m_bIsLoopAnim)
    {
        if (m_fCurrentFrame + 0.033f >= m_fDuration)
        {
            m_fCurrentFrame = 0.f;

            for (_uint i = 0; i < m_iNumChannels; ++i)
            {
                m_vecChannels[i]->Update_TransformationMatrix(_bones, m_fCurrentFrame, &m_vecCurrentKeyFrameIndices[i], m_bIsLoopAnim, m_fDuration);
            }

            m_IsStartFrame = false;

            return true;
        }
    }

    _bool finished = false;
    // 애니메이션의 최대 프레임보다 현재 프레임이 더 높아지면(애니메이션이 끝나면)
    if (m_fCurrentFrame >= m_fDuration)
    {
        // 루프가 아닐경우 그냥 리턴
        if (m_bIsLoopAnim == false)
        {
            return true;
        }

        // 루프면 프레임을 처음으로
        m_fCurrentFrame = 0.f;

        finished = true;
    }

    // 애니메이션이 움직여야 하는 본들 모두 업데이트
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecChannels[i]->Update_TransformationMatrix(_bones, m_fCurrentFrame, &m_vecCurrentKeyFrameIndices[i], m_bIsLoopAnim, m_fDuration);
    }

    m_IsStartFrame = false;
    return finished;
}

_bool Engine::Animation::Update_TransformationMatrices_AnimationTool(const vector<class Bone*>& _bones, const _float fTimeDelta, const _bool _isLoop, _float fAnimationSpeed)
{
    if (!m_IsStartFrame)
    {
        // 프레임마다 애니메이션의 현재 프레임 갱신
        m_fCurrentFrame += m_fTicksPerSecond * fAnimationSpeed * fTimeDelta; //애니메이션 재생 속도 함수 인자로 넘겨서 제어하려고 추가함

    }

    // 애니메이션의 최대 프레임보다 현재 프레임이 더 높아지면(애니메이션이 끝나면)
    if (m_fCurrentFrame >= m_fDuration)
    {
        // 루프가 아닐경우 끝났음을 리턴
        if (_isLoop == false)
        {
            return true;
        }
        if (m_iNextAnimIndex == -1)
        {

        }

        // 루프면 프레임을 처음으로
        m_fCurrentFrame = 0.f;
    }

    // 애니메이션이 움직여야 하는 본들 모두 업데이트
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecChannels[i]->Update_TransformationMatrix_Animation_Tool(_bones, m_fCurrentFrame, &m_vecCurrentKeyFrameIndices[i], m_bIsLoopAnim, m_fDuration);
    }

    m_IsStartFrame = false;

    return false;
}

_bool Engine::Animation::Update_Animation_CS(_float fTimeDelta, _bool _isLoop, _float fAnimationSpeed)
{
    if (!m_IsStartFrame)
    {
        // 프레임마다 애니메이션의 현재 프레임 갱신
        m_fCurrentFrame += m_fTicksPerSecond * fAnimationSpeed * fTimeDelta; //애니메이션 재생 속도 함수 인자로 넘겨서 제어하려고 추가함

    }

    // 애니메이션의 최대 프레임보다 현재 프레임이 더 높아지면(애니메이션이 끝나면)
    if (m_fCurrentFrame >= m_fDuration)
    {
        // 루프가 아닐경우 끝났음을 리턴
        if (_isLoop == false)
        {
            return true;
        }
        if (m_iNextAnimIndex == -1)
        {

        }

        // 루프면 프레임을 처음으로
        m_fCurrentFrame = 0.f;
    }

    m_IsStartFrame = false;
    return false;
}

HRESULT Engine::Animation::Start_Animation()
{
    m_fCurrentFrame = 0.f;
    m_IsStartFrame = true;

    return S_OK;
}

void Engine::Animation::Reset_TrackPostion()
{
    m_fCurrentFrame = 0.f;

    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecCurrentKeyFrameIndices[i] = 0;
    }
}

void Engine::Animation::Push_CurrentMatrices(vector<LERPCONTAIN>& _vecMatrices)
{
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecChannels[i]->Push_CurrentMatrix(_vecMatrices);
    }
}

void Engine::Animation::Push_FirstMatrices(vector<LERPCONTAIN>& _vecMatrices)
{
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_vecChannels[i]->Push_FirstMatrix(_vecMatrices);
    }
}
void Engine::Animation::Build_BoneToChannelIndex(_uint iNumBones)
{
    m_vecBoneToChannel.assign(iNumBones, -1);

    for (_uint i = 0; i < (_uint)m_vecChannels.size(); ++i)
    {
        _int iBoneIndex = m_vecChannels[i]->Get_ChannelIndex();
        if (iBoneIndex < (_int)iNumBones)
            m_vecBoneToChannel[iBoneIndex] = (_int)i;
    }
}
_int Engine::Animation::Get_ChannelByBoneIndex(_uint iBoneIndex)
{
    if (iBoneIndex >= m_vecBoneToChannel.size())
        return -2;

    return m_vecBoneToChannel[iBoneIndex];
}
_bool Engine::Animation::Get_InterpolatedBonePosition(_uint iBoneIndex, _float fFrame, _bool _isLoop, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos)
{
    _int iChannelIndex = Get_ChannelByBoneIndex(iBoneIndex);
    if (iChannelIndex == -1)
    {
        return false;
    }

    SRT_DESC SRTDesc = {};     //SRTDesc에 받아오고

    m_vecChannels[iChannelIndex]->Interpolate_SRT(fFrame, _isLoop, m_fDuration, SRTDesc, vOutScale, vOutRot, vOutPos); 

    // 이것도 뺌
    //vOutPos = XMLoadFloat3(&SRTDesc.vPosition);

    return true;
}
_bool Engine::Animation::Get_InterpolatedBonePositionSocket(_uint iBoneIndex, _float fFrame, _bool _isLoop, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos)
{
    _int iChannelIndex = Get_ChannelByBoneIndex(iBoneIndex);
    if (iChannelIndex == -1)
    {
        Bone* pBone = m_pModel->Get_Bone_ByIndex(iBoneIndex);
        _float4x4 LoclaMatrix = pBone->Get_TransformationMatrix();
        XMMatrixDecompose(&vOutScale, &vOutRot, &vOutPos, XMLoadFloat4x4(&LoclaMatrix));
        //vOutRot = XMQuaternionNormalize(vOutRot);
        return true;
    }

    SRT_DESC SRTDesc = {};     //SRTDesc에 받아오고

    m_vecChannels[iChannelIndex]->Interpolate_SRT(fFrame, _isLoop, m_fDuration, SRTDesc, vOutScale, vOutRot, vOutPos);

    if (m_pModel && m_pModel->Is_RemoveRootTranslation() &&
        (_int)iBoneIndex == m_pModel->Get_RootBoneIndex())
    {
        vOutPos = XMVectorZero();
        vOutRot = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    }

    return true;
}
_float Engine::Animation::Get_LastTrackPosition(_uint iBoneIndex)
{
    _int iChannelIndex = Get_ChannelByBoneIndex(iBoneIndex);
    if (iChannelIndex == -2)
    {
        cout << "채널 인덱스가 이상함" << endl;
        return 0.f;
    }
    else if (iChannelIndex == -1)
    {
        return 0.f;
    }


    return m_vecChannels[iChannelIndex]->Get_LastTrackPosition();
}
/******************************************************* 애니메이션 함수 *******************************************************/




//////////////////////////////////////////////////////// 애니메이션 데이터 저장 함수 ////////////////////////////////////////////////////////
MYANIMATION Engine::Animation::Get_AnimationData()
{
    MYANIMATION myAnim;
    myAnim.mNumChannels = m_iNumChannels;
    myAnim.mDuration = m_fDuration;
    myAnim.mTickPerSecond = m_fTicksPerSecond;
    myAnim.mName = m_strName;

    myAnim.mNextAnimIndex = m_iNextAnimIndex;
    myAnim.mNextAnimLerpDuration = m_fNextAnimLerpDuration;
    myAnim.mIsLoopAnim = m_bIsLoopAnim;
    myAnim.mLinkedAnim = m_bLinkedAnim;
    myAnim.mCompleteAnim = m_bCompleteAnim;

    myAnim.mNotifyes.reserve(m_vecNotify.size());
    myAnim.mChannels.reserve(m_iNumChannels);

    MYNOTIFY Mynotify;
    for (_uint i = 0; i < m_vecNotify.size(); ++i)
    {
        Mynotify = GetMyNotifyData(m_vecNotify[i]);
        myAnim.mNotifyes.push_back(Mynotify);
    }

    MYCHANNEL myChannel;
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        myChannel = m_vecChannels[i]->Get_ChannelData();
        myAnim.mChannels.push_back(myChannel);
    }

    return myAnim;
}
/******************************************************* 애니메이션 데이터 저장 함수 *******************************************************/

void Engine::Animation::Evaluate_Notify(_float fPrevFrame, _float fCurrentFrame, _bool bIsLoop, _uint iOwnerId, _int iAnimIndex)
{

    for (size_t i = 0; i < m_vecNotify.size(); ++i)
    {
        if (m_vecNotify[i].eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY) //단발 
        {
            _bool bAcitve = Passed_Notify(fPrevFrame, fCurrentFrame, m_vecNotify[i].fFrame, bIsLoop);

            if (bAcitve)
            {
                //구조체를 여기서 만들어진 상태에서
                FireEvent_Cached(m_vecNotify[i], ANIM_FRAMEPHASE::START, iOwnerId, iAnimIndex, fPrevFrame, fCurrentFrame, m_fDuration);
            }

        }
        else //지속 NOTIFY_STATE일때
        {
            _bool bStartEvent = Passed_Notify(fPrevFrame, fCurrentFrame, m_vecNotify[i].fStartFrame, bIsLoop);
            _bool bEndEvent = Passed_Notify(fPrevFrame, fCurrentFrame, m_vecNotify[i].fEndFrame, bIsLoop);

            if (bStartEvent) //이벤트 시작 지점
            {
                //여기서 이제 시작 이벤트 호출 
                m_vecNotify[i].bActive = true;
                FireEvent_Cached(m_vecNotify[i], ANIM_FRAMEPHASE::START, iOwnerId, iAnimIndex, fPrevFrame, fCurrentFrame, m_fDuration);
            }

            if (m_vecNotify[i].bActive == true) //이벤트 기간동안 계속 진행될것들
            {
                //계속 진행될 이벤트 호출
                _bool IsRange = IsRange_Notify(fCurrentFrame, m_vecNotify[i].fStartFrame, m_vecNotify[i].fEndFrame, bIsLoop);

                if (IsRange)
                {
                    FireEvent_Cached(m_vecNotify[i], ANIM_FRAMEPHASE::UPDATE, iOwnerId, iAnimIndex, fPrevFrame, fCurrentFrame, m_fDuration);
                    //매 프레임 이벤트 호출 ,여기서는 구조체에 _bool 변수 하나 만들어서 bActive는 여기서 사용하고 
                    //구조체에 넘길 변수만들어서 받는 사람이 해당 bool변수가 true인동안에는 계속호출하도록
                }

            }

            if (bEndEvent)
            {
                //여기서 이제 끝날때 이벤트 호출
                FireEvent_Cached(m_vecNotify[i], ANIM_FRAMEPHASE::END, iOwnerId, iAnimIndex, fPrevFrame, fCurrentFrame, m_fDuration);
                m_vecNotify[i].bActive = false;
            }

        }

    }

}

_bool Engine::Animation::Passed_Notify(_float fPrevFrame, _float fCurrentFrame, _float fFrame, _bool bIsLoop)
{
    _bool bFrameReset = fCurrentFrame < fPrevFrame; //현재 시간이 이전시간보다 작아진 경우에는 루프애니메이션일때 0으로 초기화된경우

    _bool bPass = false;

    if (!bIsLoop || !bFrameReset) //루프 애니메이션 아닌경우 + 루프 애니메이션이어도 bFrameReset이 일어나지 않았으면 if문 진입 ||문
    {

        if (fPrevFrame < fFrame && fFrame <= fCurrentFrame + m_fEPS)
        {
            bPass = true;
        }
    }
    else //루프애니메이션일때 프레임 초기화가 일어난경우
    {
        //첫번째 조건식은 mFrame을 duration-1 즉,마지막구간에 심어놨을때 FrameReset이 일어난 프레임에서 m_fCurretFrame은 이미 0을 넘어있을수있으므로 fPrevFrame보다는 크고 전체 재생길이보다는 작을때 체크
        //두번째 조건식은 mFrame을 0주위구간에 배치했을때 bFrameReset이 일어난 프레임에서 0~m_fCurrentFrame을 지나갔는지 체크
        if ((fPrevFrame < fFrame && fFrame <= m_fDuration) || (0.0f <= fFrame && fFrame <= fCurrentFrame + m_fEPS))
        {
            bPass = true;
        }
    }

    return bPass;
}

_bool Engine::Animation::IsRange_Notify(_float fCurrentFrame, _float fStartFrame, _float fEndFrame, _bool bIsLoop)
{
    if (!bIsLoop) //루프아닐때
    {
        return (fStartFrame <= fCurrentFrame && fCurrentFrame <= fEndFrame);
    }

    if (fStartFrame <= fEndFrame) //루프일때 동일하게 조건주고
    {
        return (fStartFrame <= fCurrentFrame && fCurrentFrame <= fEndFrame);
    }

    //여기까지 오면 이제 fStartFrame이 280이고 fEndFrame이 30과 같이 0을 넘어섰을때
    return (fCurrentFrame >= fStartFrame) || (fCurrentFrame <= fEndFrame);
}

void Engine::Animation::FireEvent(const ANIMNOTIFY_DESC& AnimNotifyDesc, ANIM_FRAMEPHASE ePhase, _uint iOwnerId, _int iAnimIndex, _float fPrevFrame, _float fCurrentFrame, _float fDuration)
{
    switch (AnimNotifyDesc.eNotify_Event)
    {
    case Engine::ANIM_EVENT_TYPE::PLAY_SOUND:
    {
        SOUND_EVENT SoundDesc = {};
        SoundDesc.ePhase = ePhase;
        SoundDesc.iOwnerId = iOwnerId;
        SoundDesc.iAnimationIndex = iAnimIndex;
        SoundDesc.fPrevFrame = fPrevFrame, SoundDesc.fCurrentFrame = fCurrentFrame, SoundDesc.fDurtaion = fDuration;

        Get_String(AnimNotifyDesc.UmapEvent, "SoundName", SoundDesc.SoundName);
        Get_Float(AnimNotifyDesc.UmapEvent, "Volume", SoundDesc.fVolume);
        Get_Bool(AnimNotifyDesc.UmapEvent, "Loop", SoundDesc.bLoop);
        m_pGameInstance->Publish<SOUND_EVENT>(SoundDesc);
        break;
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_PARTICLE:
    {
        PARTICLE_EVENT ParticleDesc = {};
        ParticleDesc.ePhase = ePhase;
        ParticleDesc.iOwnerId = iOwnerId;
        ParticleDesc.iAnimationIndex = iAnimIndex;
        ParticleDesc.fPrevFrame = fPrevFrame, ParticleDesc.fCurrentFrame = fCurrentFrame, ParticleDesc.fDurtaion = fDuration;
        ParticleDesc.ePoolId = StringToEnum(ParticleDesc.ParticleSystemName);

        Get_String(AnimNotifyDesc.UmapEvent, "ParticleSystemName", ParticleDesc.ParticleSystemName);
        Get_String(AnimNotifyDesc.UmapEvent, "SocketName", ParticleDesc.SocketName);
        Get_String(AnimNotifyDesc.UmapEvent, "EndSocketName", ParticleDesc.EndSocketName);
        Get_Bool(AnimNotifyDesc.UmapEvent, "Attached", ParticleDesc.bAttached);
        Get_Bool(AnimNotifyDesc.UmapEvent, "Follow", ParticleDesc.bFollow);
        Get_Bool(AnimNotifyDesc.UmapEvent, "ParticleFolow", ParticleDesc.bParticleFollow);
        Get_Bool(AnimNotifyDesc.UmapEvent, "UseOnlyPosition", ParticleDesc.bOnlyPosition);
        Get_Bool(AnimNotifyDesc.UmapEvent, "UseOwnerRotation", ParticleDesc.bOwnerRotation);
        
        m_pGameInstance->Publish<PARTICLE_EVENT>(ParticleDesc);
        break;
    }
    case Engine::ANIM_EVENT_TYPE::ACTIVE_COLLIDER:
    {
        COLLIDER_EVENT ColliderDesc = {};
        ColliderDesc.ePhase = ePhase;
        ColliderDesc.iOwnerId = iOwnerId;
        ColliderDesc.iAnimationIndex = iAnimIndex;
        ColliderDesc.fPrevFrame = fPrevFrame; ColliderDesc.fCurrentFrame = fCurrentFrame; ColliderDesc.fDurtaion = fDuration;

        Get_Enum(AnimNotifyDesc.UmapEvent, "ColGroup", ColliderDesc.eColGroup);
        Get_Enum(AnimNotifyDesc.UmapEvent, "AttackerType", ColliderDesc.eAttackerType);
        Get_Float(AnimNotifyDesc.UmapEvent, "AttackRadius", ColliderDesc.fAttackRadius);
        Get_Float(AnimNotifyDesc.UmapEvent, "AttackDamage", ColliderDesc.fAttackDamage);
        Get_Float(AnimNotifyDesc.UmapEvent, "AttackHalfHeight", ColliderDesc.fAttackHalfHeight);
        m_pGameInstance->Publish<COLLIDER_EVENT>(ColliderDesc);
        break;
    }
    case Engine::ANIM_EVENT_TYPE::CAMERA:
    {
        CAMERA_EVENT CameraDesc = {};
        CameraDesc.ePhase = ePhase;
        CameraDesc.iOwnerId = iOwnerId;
        CameraDesc.iAnimationIndex = iAnimIndex;
        CameraDesc.fPrevFrame = fPrevFrame, CameraDesc.fCurrentFrame = fCurrentFrame, CameraDesc.fDurtaion = fDuration;
        
        CameraShake shakedesc;

        Get_Enum(AnimNotifyDesc.UmapEvent, "Action", CameraDesc.eCameraAction);
        Get_Enum(AnimNotifyDesc.UmapEvent, "ShakePriority", shakedesc.ePriority);
        //Get_Float(AnimNotifyDesc.UmapEvent, "ShakePower", shakedesc.fIntensity);
        Get_Float(AnimNotifyDesc.UmapEvent, "ShakeDuration", shakedesc.fDuration);

        CameraDesc.tShake = shakedesc;

        m_pGameInstance->Publish<CAMERA_EVENT>(CameraDesc);
        break;
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_TRAIL:
    {
        TRAIL_EVENT TrailDesc{};
        TrailDesc.ePhase = ePhase;
        TrailDesc.iOwnerId = iOwnerId;
        TrailDesc.iAnimationIndex = iAnimIndex;
        TrailDesc.fPrevFrame = fPrevFrame, TrailDesc.fCurrentFrame = fCurrentFrame, TrailDesc.fDurtaion = fDuration;
        TrailDesc.ePoolId = StringToEnum(TrailDesc.TrailEffectName);

        Get_String(AnimNotifyDesc.UmapEvent, "TrailEffectName", TrailDesc.TrailEffectName);
        Get_String(AnimNotifyDesc.UmapEvent, "RootBoneName", TrailDesc.RootBoneName);
        Get_String(AnimNotifyDesc.UmapEvent, "TipBoneName", TrailDesc.TipBoneName);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bEnable", TrailDesc.bEnable);
        m_pGameInstance->Publish<TRAIL_EVENT>(TrailDesc);

        break;
    }
    case Engine::ANIM_EVENT_TYPE::PLAYER_ANIM:
    {
        PLAYERANIM_EVENT PlayerAnimDesc = {};
        PlayerAnimDesc.ePhase = ePhase;
        PlayerAnimDesc.iOwnerId = iOwnerId;
        PlayerAnimDesc.iAnimationIndex = iAnimIndex;
        PlayerAnimDesc.fPrevFrame = fPrevFrame, PlayerAnimDesc.fCurrentFrame = fCurrentFrame, PlayerAnimDesc.fDurtaion = fDuration;


        Get_Bool(AnimNotifyDesc.UmapEvent, "bInputArea", PlayerAnimDesc.bInputArea);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCanMove", PlayerAnimDesc.bCanMove);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCanCombo", PlayerAnimDesc.bCanCombo);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCanEscape", PlayerAnimDesc.bCanEscape);

        Get_Bool(AnimNotifyDesc.UmapEvent, "bSuperArmor", PlayerAnimDesc.bSuperArmor);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bInvincible", PlayerAnimDesc.bInvincible);
        Get_Float(AnimNotifyDesc.UmapEvent, "fStaminaCost", PlayerAnimDesc.fStaminaCost);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bLockOnHomingRotation", PlayerAnimDesc.bLockOnHomingRotation);
        Get_Float(AnimNotifyDesc.UmapEvent, "fHomingRotationSpeedRatio", PlayerAnimDesc.fHomingRotationSpeedRatio);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bBlockRegenStamina", PlayerAnimDesc.bBlockRegenStamina);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bUseItem", PlayerAnimDesc.bUseItem);

        Get_Bool(AnimNotifyDesc.UmapEvent, "bWeaponVisible", PlayerAnimDesc.bWeaponVisible);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bBloodWeaponVisible", PlayerAnimDesc.bBloodWeaponVisible);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCanParry", PlayerAnimDesc.bCanParry);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCreateProjectile", PlayerAnimDesc.bCreateProjectile);
        
        m_pGameInstance->Publish<PLAYERANIM_EVENT>(PlayerAnimDesc);

        break;
    }
    case Engine::ANIM_EVENT_TYPE::MONSTER_ANIM:
    {
        MONSTER_ANIM_EVENT MonsterAnimDesc = {};

        MonsterAnimDesc.ePhase = ePhase;
        MonsterAnimDesc.iOwnerId = iOwnerId;
        MonsterAnimDesc.iAnimationIndex = iAnimIndex;
        MonsterAnimDesc.fPrevFrame = fPrevFrame, MonsterAnimDesc.fCurrentFrame = fCurrentFrame, MonsterAnimDesc.fDurtaion = fDuration;

        // 1. 상태 고정 (ANS_Stiff)
        Get_Bool(AnimNotifyDesc.UmapEvent, "bCancelable", MonsterAnimDesc.bCancelable);

        // 2. 슈퍼아머 (ANS_SuperArmor)
        Get_Bool(AnimNotifyDesc.UmapEvent, "bEnableSuperArmor", MonsterAnimDesc.bEnableSuperArmor);
        Get_Float(AnimNotifyDesc.UmapEvent, "fResistance", MonsterAnimDesc.fResistance);

        // 3. 시선 고정 해제 (ANS_EnemyAIDisableLookAt)
        Get_Bool(AnimNotifyDesc.UmapEvent, "bDisableLookAt", MonsterAnimDesc.bDisableLookAt);

        // 4. 단발성 거리 보정 (ANS_AIMoveOneShotMotion)
        Get_Float(AnimNotifyDesc.UmapEvent, "fMaxMoveDistance", MonsterAnimDesc.fMaxMoveDistance);
        Get_Float(AnimNotifyDesc.UmapEvent, "fTargetOffset", MonsterAnimDesc.fTargetOffset);

        // 5. 유도 회전 (ANS_AIHomingRotation)
        Get_Float(AnimNotifyDesc.UmapEvent, "fHomingSpeedDegree", MonsterAnimDesc.fHomingSpeedDegree);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bEnableSlide", MonsterAnimDesc.bEnableSlide);
        Get_Bool(AnimNotifyDesc.UmapEvent, "bEnableHoming", MonsterAnimDesc.bEnableHoming);

        // 6. 턴 인터럽트 (AN_AITurnInterrupt)
        Get_Float(AnimNotifyDesc.UmapEvent, "fYawThreshold", MonsterAnimDesc.fYawThreshold);
        Get_Float(AnimNotifyDesc.UmapEvent, "fTurnSpeed", MonsterAnimDesc.fTurnSpeed);
        Get_Float(AnimNotifyDesc.UmapEvent, "fTimeLimit", MonsterAnimDesc.fTimeLimit);

        // 7. 콤보 체커 (AN_AIComboCheck)
        Get_Int(AnimNotifyDesc.UmapEvent, "iProbabilityRate", MonsterAnimDesc.iProbabilityRate);
        Get_Float(AnimNotifyDesc.UmapEvent, "fComboMaxDistance", MonsterAnimDesc.fComboMaxDistance);
        Get_Int(AnimNotifyDesc.UmapEvent, "iNextState", MonsterAnimDesc.iNextState);

        // 8. 광역기 장판 마커 (Marker:Field_Attack)
        Get_String(AnimNotifyDesc.UmapEvent, "strMarkerID", MonsterAnimDesc.strMarkerID);

        // 이벤트 발행
        m_pGameInstance->Publish<MONSTER_ANIM_EVENT>(MonsterAnimDesc);

        break;
    }
    case Engine::ANIM_EVENT_TYPE::DISSOLVE_FLAG:
    {
        DISSOLVE_EVENT DissolveDesc{};
        DissolveDesc.ePhase = ePhase;
        DissolveDesc.iOwnerId = iOwnerId;
        DissolveDesc.iAnimationIndex = iAnimIndex;

        Get_Bool(AnimNotifyDesc.UmapEvent, "Dissolve", DissolveDesc.bDissolve);
        Get_Bool(AnimNotifyDesc.UmapEvent, "Weapon_Dissolve", DissolveDesc.bWeapon_Dissolve);
        Get_Bool(AnimNotifyDesc.UmapEvent, "Weapon_Target", DissolveDesc.bIsWeaponTarget);
        Get_Float(AnimNotifyDesc.UmapEvent, "Dissolve Time", DissolveDesc.fDissolveMax);

        m_pGameInstance->Publish<DISSOLVE_EVENT>(DissolveDesc);

        break;
    }
    }

}




//////////////////////////////////////////////////////// 애니메이션 이벤트 데이터 캐싱 함수  ////////////////////////////////////////////////////////
template<typename T>
void FillHeader_And_Publish(GameInstance* _pGI, CACHED_EVENT_BASE* _pBase,
    ANIM_FRAMEPHASE _ePhase, _uint _iOwnerId, _int _iAnimIndex,
    _float _fPrevFrame, _float _fCurrentFrame, _float _fDuration, _bool _bStack = false)
{
    auto* pWrapper = static_cast<CACHED_EVENT_WRAPPER<T>*>(_pBase);
    pWrapper->tEvent.ePhase = _ePhase;
    pWrapper->tEvent.iOwnerId = _iOwnerId;
    pWrapper->tEvent.iAnimationIndex = _iAnimIndex;
    pWrapper->tEvent.fPrevFrame = _fPrevFrame;
    pWrapper->tEvent.fCurrentFrame = _fCurrentFrame;
    pWrapper->tEvent.fDurtaion = _fDuration;

    if (_bStack)
        _pGI->Publish_Stack<T>(pWrapper->tEvent);
    else
        _pGI->Publish<T>(pWrapper->tEvent);
}

void Engine::Animation::BuildEventCache(ANIMNOTIFY_DESC& _animNotifyDesc)
{
    const auto& umap = _animNotifyDesc.UmapEvent;

    switch (_animNotifyDesc.eNotify_Event)
    {
    case Engine::ANIM_EVENT_TYPE::PLAY_SOUND:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<SOUND_EVENT>();
        Get_Bool(umap, "Loop", pCache->tEvent.bLoop);
        Get_Bool(umap, "InGroup", pCache->tEvent.bInGroup);
        Get_Bool(umap, "RandomPlay", pCache->tEvent.bRandomPlay);
        Get_Bool(umap, "GroupPlay", pCache->tEvent.bGroupPlay);

        Get_Float(umap, "Volume", pCache->tEvent.fVolume);
        Get_String(umap, "SoundName", pCache->tEvent.SoundName);
        Get_String(umap, "GroupName", pCache->tEvent.strGroupName);
        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_PARTICLE:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<PARTICLE_EVENT>();
        Get_String(umap, "ParticleSystemName", pCache->tEvent.ParticleSystemName);
        Get_Bool(umap, "Attached", pCache->tEvent.bAttached);
        Get_Bool(umap, "Follow", pCache->tEvent.bFollow);
        Get_Bool(umap, "ParticleFollow", pCache->tEvent.bParticleFollow);
        Get_Bool(umap, "UseOnlyPosition", pCache->tEvent.bOnlyPosition);
        Get_Bool(umap, "UseOwnerRotation", pCache->tEvent.bOwnerRotation);
        Get_String(umap, "SocketName", pCache->tEvent.SocketName);
        Get_String(umap, "EndSocketName", pCache->tEvent.EndSocketName);
        pCache->tEvent.ePoolId = StringToEnum(pCache->tEvent.ParticleSystemName);
        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::ACTIVE_COLLIDER:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<COLLIDER_EVENT>();
        Get_Enum(umap, "ColGroup", pCache->tEvent.eColGroup);
        Get_Enum(umap, "AttackerType", pCache->tEvent.eAttackerType);
        Get_Float(umap, "AttackRadius", pCache->tEvent.fAttackRadius);
        Get_Float(umap, "AttackDamage", pCache->tEvent.fAttackDamage);
        Get_Float(umap, "AttackHalfHeight", pCache->tEvent.fAttackHalfHeight);
        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::CAMERA:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<CAMERA_EVENT>();
        Get_Enum(umap, "Action", pCache->tEvent.eCameraAction);

        // ShakeMode 분기 (0 = Preset, 1 = Modify)
        _int iShakeMode = 0;
        Get_Int(umap, "ShakeMode", iShakeMode);

        if (iShakeMode == 0)
        {
            // Preset 모드 - enum 값으로 CameraShake 통째로 세팅
            _int iPreset = 0;
            Get_Int(umap, "ShakePreset", iPreset);
            pCache->tEvent.tShake = ShakePreset::GetPresetShake((ShakePreset::SHAKE_PRESET)iPreset);
        }
        else
        {
            // Modify 모드 - 기존 방식대로 개별 값 로드
            Get_Enum(umap, "ShakePriority", pCache->tEvent.tShake.ePriority);
            Get_Float(umap, "ShakeDuration", pCache->tEvent.tShake.fDuration);
            Get_Float(umap, "ShakeBlendOut", pCache->tEvent.tShake.fBlendOutTime);

            // 축별 위치 진동
            Get_Float(umap, "ShakeAmpX", pCache->tEvent.tShake.fAmpX);
            Get_Float(umap, "ShakeAmpY", pCache->tEvent.tShake.fAmpY);
            Get_Float(umap, "ShakeAmpZ", pCache->tEvent.tShake.fAmpZ);
            Get_Float(umap, "ShakeFreqX", pCache->tEvent.tShake.fFreqX);
            Get_Float(umap, "ShakeFreqY", pCache->tEvent.tShake.fFreqY);
            Get_Float(umap, "ShakeFreqZ", pCache->tEvent.tShake.fFreqZ);

            // 회전 진동
            Get_Float(umap, "ShakeAmpPitch", pCache->tEvent.tShake.fAmpPitch);
            Get_Float(umap, "ShakeAmpYaw", pCache->tEvent.tShake.fAmpYaw);
            Get_Float(umap, "ShakeFreqPitch", pCache->tEvent.tShake.fFreqPitch);
            Get_Float(umap, "ShakeFreqYaw", pCache->tEvent.tShake.fFreqYaw);

            // FOV 진동
            Get_Float(umap, "ShakeAmpFov", pCache->tEvent.tShake.fAmpFov);
            Get_Float(umap, "ShakeFreqFov", pCache->tEvent.tShake.fFreqFov);

            // 킥
            Get_Float(umap, "ShakeKickStrength", pCache->tEvent.tShake.fKickStrength);
            Get_Float(umap, "ShakeKickDecay", pCache->tEvent.tShake.fKickDecay);
        }

        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_TRAIL:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<TRAIL_EVENT>();
        Get_String(umap, "TrailEffectName", pCache->tEvent.TrailEffectName);
        Get_String(umap, "RootBoneName", pCache->tEvent.RootBoneName);
        Get_String(umap, "TipBoneName", pCache->tEvent.TipBoneName);
        Get_Bool(umap, "bEnable", pCache->tEvent.bEnable);
        pCache->tEvent.ePoolId = StringToEnum(pCache->tEvent.TrailEffectName);
        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::PLAYER_ANIM:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<PLAYERANIM_EVENT>();
        Get_Bool(umap, "bInputArea", pCache->tEvent.bInputArea);
        Get_Bool(umap, "bCanMove", pCache->tEvent.bCanMove);
        Get_Bool(umap, "bCanCombo", pCache->tEvent.bCanCombo);
        Get_Bool(umap, "bCanEscape", pCache->tEvent.bCanEscape);
        Get_Bool(umap, "bSuperArmor", pCache->tEvent.bSuperArmor);
        Get_Bool(umap, "bInvincible", pCache->tEvent.bInvincible);
        Get_Float(umap, "fStaminaCost", pCache->tEvent.fStaminaCost);
        Get_Bool(umap, "bLockOnHomingRotation", pCache->tEvent.bLockOnHomingRotation);
        Get_Float(umap, "fHomingRotationSpeedRatio", pCache->tEvent.fHomingRotationSpeedRatio);
        Get_Bool(umap, "bBlockRegenStamina", pCache->tEvent.bBlockRegenStamina);
        Get_Bool(umap, "bWeaponVisible", pCache->tEvent.bWeaponVisible);
        Get_Bool(umap, "bBloodWeaponVisible", pCache->tEvent.bBloodWeaponVisible);
        Get_Bool(umap, "bCanParry", pCache->tEvent.bCanParry);
        Get_Bool(umap, "bCreateProjectile", pCache->tEvent.bCreateProjectile);
        Get_Bool(umap, "bUseItem", pCache->tEvent.bUseItem);
        Get_Float(umap, "fFalterResistance", pCache->tEvent.fFalterResistance);
        Get_Bool(umap, "bChangeEquipWeapon", pCache->tEvent.bChangeEquipWeapon);
        Get_Float(umap, "fAnimationSpeed", pCache->tEvent.fAnimationSpeed);
        Get_Bool(umap, "bControlAnimSpeed", pCache->tEvent.bControlAnimSpeed);
        Get_Bool(umap, "bKetsugiStart", pCache->tEvent.bKetsugiStart);
        Get_Bool(umap, "bInjectionVisible", pCache->tEvent.bInjectionVisible);

        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::MONSTER_ANIM:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<MONSTER_ANIM_EVENT>();
        // 1. 상태 고정
        Get_Bool(umap, "bCancelable", pCache->tEvent.bCancelable);
        Get_Bool(umap, "bChangeValue", pCache->tEvent.bChangeValue);
        Get_Bool(umap, "bComboAttack", pCache->tEvent.bComboAttack);
        // 2. 슈퍼아머
        Get_Bool(umap, "bEnableSuperArmor", pCache->tEvent.bEnableSuperArmor);
        Get_Float(umap, "fResistance", pCache->tEvent.fResistance);
        // 3. 시선 고정 해제
        Get_Bool(umap, "bDisableLookAt", pCache->tEvent.bDisableLookAt);
        // 4. 단발성 거리 보정
        Get_Float(umap, "fMaxMoveDistance", pCache->tEvent.fMaxMoveDistance);
        Get_Float(umap, "fTargetOffset", pCache->tEvent.fTargetOffset);
        // 5. 유도 회전
        Get_Float(umap, "fHomingSpeedDegree", pCache->tEvent.fHomingSpeedDegree);
        Get_Bool(umap, "bEnableSlide", pCache->tEvent.bEnableSlide);
        Get_Bool(umap, "bEnableHoming", pCache->tEvent.bEnableHoming);
        // 6. 턴 인터럽트
        Get_Float(umap, "fYawThreshold", pCache->tEvent.fYawThreshold);
        Get_Float(umap, "fTurnSpeed", pCache->tEvent.fTurnSpeed);
        Get_Float(umap, "fTimeLimit", pCache->tEvent.fTimeLimit);
        // 7. 콤보 체커
        Get_Int(umap, "iProbabilityRate", pCache->tEvent.iProbabilityRate);
        Get_Float(umap, "fComboMaxDistance", pCache->tEvent.fComboMaxDistance);
        Get_Int(umap, "iNextState", pCache->tEvent.iNextState);
        // 8. 광역기 장판 마커
        Get_String(umap, "strMarkerID", pCache->tEvent.strMarkerID);
        _animNotifyDesc.pCachedEvent = pCache;
        break;
    }
    case Engine::ANIM_EVENT_TYPE::DISSOLVE_FLAG:
    {
        auto* pCache = new CACHED_EVENT_WRAPPER<DISSOLVE_EVENT>();

        Get_Bool(umap, "Dissolve", pCache->tEvent.bDissolve);
        Get_Bool(umap, "Weapon_Dissolve", pCache->tEvent.bWeapon_Dissolve);
        Get_Bool(umap, "Weapon_Target", pCache->tEvent.bIsWeaponTarget);
        Get_Float(umap, "Dissolve Time", pCache->tEvent.fDissolveMax);

        _animNotifyDesc.pCachedEvent = pCache;

        break;
    }
    }
}


void Engine::Animation::FireEvent_Cached(ANIMNOTIFY_DESC& _animNotifyDesc, ANIM_FRAMEPHASE _ePhase,
    _uint _iOwnerId, _int _iAnimIndex, _float _fPrevFrame, _float _fCurrentFrame, _float _fDuration)
{
    if (_animNotifyDesc.pCachedEvent == nullptr)
        BuildEventCache(_animNotifyDesc);

    auto* pBase = _animNotifyDesc.pCachedEvent;

    switch (_animNotifyDesc.eNotify_Event)
    {
    case ANIM_EVENT_TYPE::PLAY_SOUND:
        FillHeader_And_Publish<SOUND_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::SPAWN_PARTICLE:
        FillHeader_And_Publish<PARTICLE_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration, true);
        break;
    case ANIM_EVENT_TYPE::ACTIVE_COLLIDER:
        FillHeader_And_Publish<COLLIDER_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::CAMERA:
        FillHeader_And_Publish<CAMERA_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::SPAWN_TRAIL:
        FillHeader_And_Publish<TRAIL_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::PLAYER_ANIM:
        FillHeader_And_Publish<PLAYERANIM_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::MONSTER_ANIM:
        FillHeader_And_Publish<MONSTER_ANIM_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    case ANIM_EVENT_TYPE::DISSOLVE_FLAG:
        FillHeader_And_Publish<DISSOLVE_EVENT>(m_pGameInstance, pBase, _ePhase, _iOwnerId, _iAnimIndex, _fPrevFrame, _fCurrentFrame, _fDuration);
        break;
    }
}
/******************************************************* 애니메이션 이벤트 데이터 캐싱 함수 *******************************************************/


_uint Engine::Animation::Add_Notify(ANIMNOTIFY_DESC& AnimNotifyDesc)
{
    AnimNotifyDesc.iNotifyId = m_iNextNotifyId++;

    m_vecNotify.push_back(AnimNotifyDesc);

    return m_vecNotify.back().iNotifyId;
}

_bool Engine::Animation::Remove_Notify(_uint iNotifyId)
{
    if (iNotifyId == g_INVALID)
        return false;

    for (auto iter = m_vecNotify.begin(); iter != m_vecNotify.end(); ++iter)
    {
        if (iter->iNotifyId == iNotifyId)
        {
            Safe_Delete(iter->pCachedEvent); // 캐시 해제
            m_vecNotify.erase(iter);
            return true;
        }
    }
    return false;
}

ANIMNOTIFY_DESC* Engine::Animation::Find_Notify(_uint iNotifyId)
{
    if (iNotifyId == g_INVALID)
        return nullptr;

    for (auto& NotifyDesc : m_vecNotify)
    {
        if (NotifyDesc.iNotifyId == iNotifyId)
            return &NotifyDesc;
    }

    return nullptr;
}

void Engine::Animation::Clear_Notifies()
{
    for (auto& notify : m_vecNotify)
    {
        Safe_Delete(notify.pCachedEvent); // 캐시 해제
    }
    m_vecNotify.clear();
}

MYNOTIFY Engine::Animation::GetMyNotifyData(ANIMNOTIFY_DESC& AnimDesc)
{
    MYNOTIFY Mynotify{};

    Mynotify.fFrame = AnimDesc.fFrame;
    Mynotify.fStartFrame = AnimDesc.fStartFrame;
    Mynotify.fEndFrame = AnimDesc.fEndFrame;
    Mynotify.bActive = AnimDesc.bActive;

    Mynotify.eNotify_Type = AnimDesc.eNotify_Type;
    Mynotify.eNotify_Event = AnimDesc.eNotify_Event;

    Mynotify.UmapEvent = AnimDesc.UmapEvent;
    Mynotify.SocketName = AnimDesc.SocketName;
    Mynotify.bAttached = AnimDesc.bAttached;
    Mynotify.iNotifyId = AnimDesc.iNotifyId;

    return Mynotify;
}

POOL_ID Engine::Animation::StringToEnum(_string& _strEffectName)
{
    static const UMAP<_string, POOL_ID> s_mapEffectToPool =
    {
        { "BasicSlash",                             POOL_ID::EFFECT_PLAYER_SLASH },
        { "Block_Effect",                           POOL_ID::EFFECT_PLAYER_GUARD },
        { "Dodge_Particle",                         POOL_ID::EFFECT_HALBERD_DODGE },
        { "Heal_Effect",                            POOL_ID::EFFECT_HEAL },
        { "Backstab",                               POOL_ID::EFFECT_BACKSTAB },
        { "BackStab_Hand",                          POOL_ID::EFFECT_BACKSTAB_HAND },
        { "Special_ATK_Blood",                      POOL_ID::EFFECT_SPECIALATK_BLOOD },
        { "Special_ATK_Blood2",                     POOL_ID::EFFECT_SPECIALATK_BLOOD2 },

        { "Skill_Royal_Heart",                      POOL_ID::EFFECT_SKILL_ROYAL_HEART },

        { "Skill_ShadowAssault_Aura",               POOL_ID::EFFECT_SKILL_SA_AURA },
        { "Skill_ShadowAssault_Body",               POOL_ID::EFFECT_SKILL_SA_BODY },
        { "Skill_ShadowAssault_Hit",                POOL_ID::EFFECT_SKILL_SA_HIT },
        { "Skill_ShadowAssault_Slash",              POOL_ID::EFFECT_SKILL_SA_SLASH },
        { "Skill_ShadowAssault_Wind",               POOL_ID::EFFECT_SKILL_SA_WIND },

        { "Skill_Kangryong_Aura",                   POOL_ID::EFFECT_SKILL_KR_AURA },
        { "Skill_Kangryong_Body",                   POOL_ID::EFFECT_SKILL_KR_BODY1 },
        { "Skill_Kangryong_Body1",                  POOL_ID::EFFECT_SKILL_KR_BODY2 },
        { "Skill_Kangryong_Wind",                   POOL_ID::EFFECT_SKILL_KR_WIND },
        { "Skill_KR_AfterImpact",                   POOL_ID::EFFECT_SKILL_KR_AFTERIMPACT },
        { "Skill_KR_Impact",                        POOL_ID::EFFECT_SKILL_KR_IMPACT },
        { "Skill_KR_SwordParticles",                POOL_ID::EFFECT_SKILL_KR_PARTICLES1 },
        { "Skill_KR_SwordParticles2",               POOL_ID::EFFECT_SKILL_KR_PARTICLES2 },

        { "Skill_RB_Aura",                          POOL_ID::EFFECT_SKILL_RB_AURA },
        { "Skill_RB_Laser",                         POOL_ID::EFFECT_SKILL_RB_LASER },
        { "Skill_RB_Shot",                          POOL_ID::EFFECT_SKILL_RB_SHOT },

        { "Skill_IndraCoil_Hand",                   POOL_ID::EFFECT_SKILL_IC_HAND },

        { "Skill_BladeDance",                       POOL_ID::EFFECT_SKILL_BLADEDANCING },

        { "Skill_Enchant_Hand",                     POOL_ID::EFFECT_SKILL_ENCHANT_HAND },
        { "Skill_Enchant_Weapon",                   POOL_ID::EFFECT_SKILL_ENCHANT_WEAPON },
        { "Skill_Enchant_Particle",                 POOL_ID::EFFECT_SKILL_ENCHANT_PARTICLES },

        { "Hit_Effect",                             POOL_ID::EFFECT_HIT },
        { "Hit_Blood",                              POOL_ID::EFFECT_HIT_BLOOD },
        { "Shotgun",                                POOL_ID::EFFECT_SHOTGUN },
        { "Shotgun_Barrel",                         POOL_ID::EFFECT_SHOTGUN_BARREL },
        { "Enemy_Slash",                            POOL_ID::EFFECT_ENEMY_SLASH },
        { "Enemy_Atk_Impact",                       POOL_ID::EFFECT_ENEMY_ATK_IMPACT },
        { "Enemy_ATK_Impact_New",                   POOL_ID::EFFECT_ENEMY_ATK_IMPACTNEW },
        { "Enemy_FieldSplash",                      POOL_ID::EFFECT_ENEMY_FIELDSPLASH },
        { "Oliver_Slash",                           POOL_ID::EFFECT_OLIVER_SLASH },
        { "Oliver_Transition",                      POOL_ID::EFFECT_OLIVER_TRANSITION },
        { "Oliver_SpecialATK",                      POOL_ID::EFFECT_OLIVER_SPECIALATK },
        { "Oliver_SpecialATK_Hand",                 POOL_ID::EFFECT_OLIVER_SPECIALATK_HAND },
        { "Oliver_Scream",                          POOL_ID::EFFECT_OLIVER_SCREAM },
        { "Mistle",                                 POOL_ID::EFFECT_MISTLE },
        { "Death_Effect",                           POOL_ID::EFFECT_DEATH_PARTICLE },
        { "Dust_Walk",                              POOL_ID::EFFECT_PLAYER_WALK },
        { "Dust_Dodge",                             POOL_ID::EFFECT_PLAYER_DODGE },
        { "strong_charge",                          POOL_ID::EFFECT_STRONG_ATTACK_CHARGE },
        { "Strike_Riser",                           POOL_ID::EFFECT_SKILL_STRIKE_RISER },
        { "Strike_Riser_Hand",                      POOL_ID::EFFECT_SKILL_STRIKE_RISER_HAND },
        { "BloodBoom_Hand",                         POOL_ID::EFFECT_WOLF_BLOODBOOM_HAND },
        { "FireBlast",                              POOL_ID::EFFECT_WOLF_FIREBLAST_ATTACK },
        { "FireBlast_Hand",                         POOL_ID::EFFECT_WOLF_FIREBLAST_HAND },
        { "FireBlast_Plate",                        POOL_ID::EFFECT_WOLF_FIREBLAST_PLATE },
        { "Thunder_Hand_Smoke",                     POOL_ID::EFFECT_WOLF_THUNDER_HAND_AURA },
        { "Thunder_Hand_Circle",                    POOL_ID::EFFECT_WOLF_THUNDER_HAND_CIRCLE },
        { "Ice_Hand",                               POOL_ID::EFFECT_WOLF_ICE_HAND },
        { "HellFireBolt_Hand",                      POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND },
        { "HellFireBolt_Hand_ShokeWave",            POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND_SHOKE },
        { "Wolf_VoidLaser",                         POOL_ID::EFFECT_WOLF_VOIDLASER },
        { "Wolf_VoidLaser_Smoke",                   POOL_ID::EFFECT_WOLF_VOIDLASER_SMOKE },
        { "Wolf_SolarLaser",                        POOL_ID::EFFECT_WOLF_SOLARLASER },
        { "Wold_SolarLaser_Particle",               POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE },
        { "Wolf_TP_Start",                          POOL_ID::EFFECT_WOLF_TP_START },
        { "Wolf_TP_Bottom",                         POOL_ID::EFFECT_WOLF_TP_BOTTOM },
        { "Wolf_TP_End",                            POOL_ID::EFFECT_WOLF_TP_END },
        { "WD_Transition",                          POOL_ID::EFFECT_WHITEDEVIL_TRANSITION},
        { "WD_Phase2_Aura",                         POOL_ID::EFFECT_WHITEDEVIL_AURA},
        { "Phase2_Slash",                           POOL_ID::EFFECT_WHITEDEVIL_SLASH},
        { "Dissolve_Particle",                      POOL_ID::EFFECT_DISSOLVE_PARTICLE },
        { "Giant_Vampire_Shield",                   POOL_ID::EFFECT_ENEMY_SHIELD_RED },
        { "GK_Special_ATK_Particle",                POOL_ID::EFFECT_GHOSTKNIGHT_PARTICLE },
        { "GK_Special_ATK",                         POOL_ID::EFFECT_GHOSTKNIGHT_ATK },
        { "Use_Item",                               POOL_ID::EFFECT_USE_ITEM },
        { "OnceHeal",                               POOL_ID::EFFECT_ONCE_HEAL },
        { "OnceHeal_Aura",                          POOL_ID::EFFECT_ONCE_HEAL_AURA },
    };

    auto it = s_mapEffectToPool.find(_strEffectName);
    if (it != s_mapEffectToPool.end())
        return it->second;

    return POOL_ID::END;
}

_bool Engine::Animation::Get_String(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _string& strOut)
{
    //holds_alternative : 객체가 현재 지정된 타입<T>의 값을 저장하고 있는지 확인하는 검사 도구 / 일치하면 true , 그렇지 않으면 false
    //get : get<T>의 값을 참조로 반환
    auto it = umap.find(isNameRegistered);
    if (it == umap.end()) return false;
    if (!std::holds_alternative<_string>(it->second)) return false;
    strOut = std::get<_string>(it->second);
    return true;
}

_bool Engine::Animation::Get_Float(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _float& fOut)
{
    auto it = umap.find(isNameRegistered);
    if (it == umap.end()) return false;
    if (!std::holds_alternative<_float>(it->second)) return false;
    fOut = std::get<_float>(it->second);
    return true;
}

_bool Engine::Animation::Get_Bool(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _bool& bOut)
{
    auto it = umap.find(isNameRegistered);
    if (it == umap.end()) return false;
    if (!std::holds_alternative<_bool>(it->second)) return false;
    bOut = std::get<_bool>(it->second);
    return true;
}

_bool Engine::Animation::Get_Int(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _int& iOut)
{
    auto it = umap.find(isNameRegistered);
    if (it == umap.end()) return false;
    if (!std::holds_alternative<_int>(it->second)) return false;
    iOut = std::get<_int>(it->second);
    return true;
}


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Animation* Engine::Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiAnimation* _aiAnimation, Model* _model, myAnimation* _myAnimation)
{
    Animation* pInstance = new Animation(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_FBX(_aiAnimation, _model, _myAnimation), L"Animation 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Animation* Engine::Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myAnimation* _myAnimation)
{
    Animation* pInstance = new Animation(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Binary(_myAnimation), L"Animation 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Animation* Engine::Animation::Clone()
{
    return new Animation(*this);
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Animation::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    for (auto& channel : m_vecChannels)
    {
        Safe_Release(channel);
    }
    m_vecChannels.clear();

    for (auto& notify : m_vecNotify)
    {
        Safe_Delete(notify.pCachedEvent);
    }
    m_vecNotify.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

