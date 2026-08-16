#pragma once

#include "Base.h"

/* 특정 동작(공격, 대기, 걷기 등등 )을 위한 뼈대들의 시간에 따른 상태값(행렬)을 보관한다.  */
/* CChannel : 특정 뼈의 시간에 따른 상태값(행렬)을 보관 */




NS_BEGIN(Engine)
class GameInstance;

struct CACHED_EVENT_BASE
{
    virtual ~CACHED_EVENT_BASE() = default;
};

// 각 이벤트 타입별 캐시 구조체
template<typename T>
struct CACHED_EVENT_WRAPPER : public CACHED_EVENT_BASE
{
    T tEvent = {};
};

class ENGINE_DLL Animation final : public Base
{
public:
    typedef enum tagAnimationRole {



    }ANIMROLE;

private:
	explicit Animation();
	explicit Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Animation(const Animation& original);
	virtual ~Animation();

public:
	HRESULT Initialize_FBX(aiAnimation* _aiAnimation, class Model* _model, myAnimation* _myAnimation);
    HRESULT Initialize_Binary(myAnimation* _myAnimation);
    _bool Update_TransformationMatrices(const vector<class Bone*>& _bones, const _float fTimeDelta, const _bool _isLoop ,_float fAnimationSpeed);
    _bool Update_TransformationMatrices_SelfLoop(const vector<class Bone*>& _bones, const _float fTimeDelta);
    _bool Update_TransformationMatrices_AnimationTool(const vector<class Bone*>& _bones, const _float fTimeDelta, const _bool _isLoop, _float fAnimationSpeed);//툴용
    _bool Update_Animation_CS(_float fTimeDelta, _bool _isLoop, _float fAnimationSpeed);

#pragma region 애니메이션 이벤트 관련 함수
    void Evaluate_Notify(_float fPrevFrame,_float fCurrentFrame,_bool bIsLoop, _uint iOwnerId, _int iAnimIndex);
    _bool Passed_Notify(_float fPrevFrame, _float fCurrentFrame , _float fFrame, _bool bIsLoop);
    _bool IsRange_Notify(_float fCurrentFrame , _float fStartFrame, _float fEndFrame, _bool bIsLoop);
    void FireEvent(const ANIMNOTIFY_DESC& AnimNotifyDesc, ANIM_FRAMEPHASE ePhase , _uint iOwnerId , _int iAnimIndex,_float fPrevFrame,_float fCurrentFrame,_float fDuration);
    _uint Add_Notify(ANIMNOTIFY_DESC& AnimNotifyDesc); //노티파이 추가
    _bool Remove_Notify(_uint iNotifyId); //노티파이 제거
    ANIMNOTIFY_DESC* Find_Notify(_uint iNotifyId); //노티파이 찾기
    void Clear_Notifies();
    vector<ANIMNOTIFY_DESC>& Get_Notifies() { return m_vecNotify; }
    MYNOTIFY GetMyNotifyData(ANIMNOTIFY_DESC& AnimDesc);
    POOL_ID StringToEnum(_string& strEffectName);

    /* 캐싱 함수 */
    void BuildEventCache(ANIMNOTIFY_DESC& _animNotifyDesc);
    void FireEvent_Cached(ANIMNOTIFY_DESC& _animNotifyDesc, ANIM_FRAMEPHASE _ePhase, _uint _iOwnerId, _int _iAnimIndex, _float _fPrevFrame, _float _fCurrentFrame, _float _fDuration);
    
    
    //아래는 헬퍼함수
    template<typename T>
    _bool Get_Enum(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, T& enumOut)
    {
        _int iTemp = static_cast<_int>(enumOut);
        if (!Get_Int(umap, isNameRegistered, iTemp))
            return false;

        enumOut = static_cast<T>(iTemp);
        return true;
    }

    _bool Get_String(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _string& strOut);
    _bool Get_Float(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _float& fOut);
    _bool Get_Bool(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _bool& bOut);
    _bool Get_Int(const UMAP<_string, VariantValue>& umap, const _string& isNameRegistered, _int& iOut);

    //_bool Advance_Frame(_float _fTimeDelta, _float _fAnimationSpeed, _bool _isLoop);

#pragma endregion

	HRESULT Start_Animation();

    void Reset_TrackPostion();
    void Get_CurrentAnimation_KeyFrame() {}

    void Push_CurrentMatrices(vector<LERPCONTAIN>& _vecMatrices);
    void Push_FirstMatrices(vector<LERPCONTAIN>& _vecMatrices);

    //루트애니메이션 관련
    void Build_BoneToChannelIndex(_uint iNumBones); //애니메이션 생성되고 초기화때
    _int Get_ChannelByBoneIndex(_uint iBoneIndex);  //뼈 인덱스로 몇번 채널인지 찾기
    _bool Get_InterpolatedBonePosition(_uint iBoneIndex, _float fFrame, _bool _isLoop, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos); //루트용 
    _bool Get_InterpolatedBonePositionSocket(_uint iBoneIndex, _float fFrame, _bool _isLoop, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos); //소켓용
    _float Get_LastTrackPosition(_uint iBoneIndex);


#pragma region Get함수
    _bool&  Get_IsLoopAnim() { return m_bIsLoopAnim; }
    _bool&  Get_IsLinkedAnim() { return m_bLinkedAnim; }
    _bool&  Get_IsCompleteAnim() { return m_bCompleteAnim; }
    _float  Get_Duration() const { return m_fDuration; }
    _float  Get_CurrentFrame() const { return m_fCurrentFrame; }
    _float  Get_NextLerpDuration() const { return m_fNextAnimLerpDuration; }
    _float  Get_Animation_Speed() const { return m_fTicksPerSecond; }
    _int    Get_NextAnimIndex() const { return m_iNextAnimIndex; }
    _string Get_AnimationName() const { return m_strName; }
    MYANIMATION Get_AnimationData();
    vector<class Channel*>& Get_Channel() { return m_vecChannels; }
    //시호 추가 함수
    vector<_uint>& Get_CurrentKeyFrameIndices() { return m_vecCurrentKeyFrameIndices; }


    //민지쓰 추가
    _float Get_PlayTimeRatio() const {
        if (m_fDuration <= 0.f) return 0.f;
        return m_fCurrentFrame / m_fDuration;
    }
#pragma endregion

#pragma region Set함수
    void Set_IsLoopAnim(const _bool _loop) { m_bIsLoopAnim = _loop; }
    void Set_IsLinkedAnim(const _bool _link) { m_bLinkedAnim = _link; }
    void Set_IsCompleteAnim(const _bool _complete) { m_bCompleteAnim = _complete; }
    void Set_NextAnim(const _uint _nextanimIndex) { m_iNextAnimIndex = _nextanimIndex; }
    void Set_ParentModel(class Model* _model) { m_pModel = _model; }
    void Set_NextLerpDuration(_float _duration) { m_fNextAnimLerpDuration = _duration; }
    void Set_StackNextAnimation(_bool _nexton) { m_bStackNextAnim = _nexton; }
    void Set_Animation_Speed(const _float _speed) { m_fTicksPerSecond = _speed; }

    //시호 추가 함수
    void Set_CurrentFrame(_float fCurrentFrame) { m_fCurrentFrame = fCurrentFrame; } //툴에서 드래그를 통한 현재 재생위치를 변경하기 위해서
  
#pragma endregion



private:
	_bool m_IsStartFrame = { false };
    _int m_iNextAnimIndex = { -1 };
	_float m_fDuration = {};
	_float m_fCurrentFrame = {};
	_float m_fTicksPerSecond = {};
    _float m_fNextAnimLerpDuration = { 0.04f };
	_string m_strName = {};
    _bool m_bIsLoopAnim = { false };
    _bool m_bLinkedAnim = { false };
    _bool m_bCompleteAnim = { false };
    _bool m_bStackNextAnim = { false };

    class Model* m_pModel = { nullptr };

	/* 이 동작을 구현해주기위해 사용해야하는 뼈의 갯수 */
	_uint m_iNumChannels = {};
	vector<class Channel*> m_vecChannels;

    vector<_uint> m_vecCurrentKeyFrameIndices = {};

    //애니메이션 이벤트 관련 
    vector<ANIMNOTIFY_DESC>     m_vecNotify;
    _double                     m_fEPS = { 1e-4 }; //소수점 오차 줄이기 위해서 , 0.0001값
    _uint                       m_iNextNotifyId = { 1 }; //툴용 아이디 찾기
    _uint                       m_iNumNotifies = {};

    //루트애니메이션 관련 
    vector<_int>                m_vecBoneToChannel; //뼈인덱스를 넘겨서 그 뼈인덱스를 가지고 있는 채널이 몇번이지 찾기용도

private:
    GameInstance*           m_pGameInstance = { nullptr };

public:
    static Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiAnimation* _aiAnimation, Model* _model, myAnimation* _myAnimation);
    static Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myAnimation* _myAnimation);
    Animation* Clone();

public:
	void Free() override final;

};

NS_END
