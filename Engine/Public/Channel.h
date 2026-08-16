#pragma once

#include "Base.h"

/* Channel :  이 뼈!는 어떤 시간에 어떤 상태를 취한다라는 것들을 저장하기위한거  */
/* KeyFrame : 뼈의 상태! */
/* 상태와 상태 사이를 보간하여 상태를 만들어준다. */
/* 만들어 낸 상태는 CBone의 m_Transformationmatrix에 전달해준다. */

NS_BEGIN(Engine)

class Channel final : public Base
{
private:
	explicit Channel();
	explicit Channel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Channel();

public:
	HRESULT Initialize_FBX(const aiNodeAnim* _aiNodeAnim, class Model* _model, myChannel* _myChannel);
    HRESULT Initialize_Binary(myChannel* _myChannel);
	void Update_TransformationMatrix(const vector<class Bone*>& _bones, const _float _currentTrackPosition, _uint* _frameIndex, _bool _isLoop, _float _duration);
    void Update_TransformationMatrix_Animation_Tool(const vector<class Bone*>& _bones, const _float _currentTrackPosition, _uint* _frameIndex, _bool _isLoop, _float _duration);//툴용
    void Interpolate_SRT(_float fTrackPosition, _bool _isLoop, _float fDuration, SRT_DESC& SRTOut, _vector& _outScale, _vector& _outRotation, _vector& _outPosition); //루트본 뼈 보간한 SRT를 구하기 위한 함수 , 기존 보간 업데이트와 동일함.

    _float4x4 Get_CurrentMatrix() { return m_matCurrentKeyFrameMatrix; }
    inline void Push_CurrentMatrix(vector<LERPCONTAIN>& _vecmMatrices) const { _vecmMatrices[m_iChannelIndex] = m_tCurrentKeyFrameInfo; }
    inline void Push_FirstMatrix(vector<LERPCONTAIN>& _vecmMatrices) const { _vecmMatrices[m_iChannelIndex] = m_tFirstKeyFrameInfo; }

    MYCHANNEL Get_ChannelData();

    _float Get_LastTrackPosition() const { return m_vecKeyFrames.back().fTrackPosition; }
    _int Get_ChannelIndex() const { return m_iChannelIndex; }
    _uint Get_NumKeyFrames() const { return m_iNumKeyFrames; }
    const vector<KEYFRAME>& Get_KeyFrame() const { return m_vecKeyFrames; }
    vector<KEYFRAME>& Get_KeyFrames() { return m_vecKeyFrames; }
private:
	_int m_iChannelIndex = {};
	_uint m_iNumKeyFrames = {};
	vector<KEYFRAME> m_vecKeyFrames;
    _float4x4 m_matCurrentKeyFrameMatrix = {};
    LERPCONTAIN m_tCurrentKeyFrameInfo = {};
    LERPCONTAIN m_tFirstKeyFrameInfo = {};

    _uint m_iNumScalingKeys = {};
    _uint m_iNumRotationKeys = {};
    _uint m_iNumPositionKeys = {};

public:
    static Channel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiNodeAnim* _aiNodeAnim, Model* _model, myChannel* _myChannel);
    static Channel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myChannel* _myChannel);

public:
	void Free() override final;

};

NS_END
