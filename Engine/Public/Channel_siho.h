#pragma once
#include "Base.h"

// Channel : 이 뼈는 어떤 시간에 어떤 상태를 취한다라는 것들을 저장하기 위함
// KeyFrame : 뼈의 상태
// 상태와 상태 사이를 보간하여 상태를 만들어준다.
// 만들어 낸 상태는 CBone의 m_fTransformationMatrix에 전달한다.

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel);
	HRESULT Initialize_Binary(ifstream& file);
	void Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

public:
	HRESULT Write_To_Binary(ofstream& file);
	HRESULT Read_From_Binary(ifstream& file);

private:
	_uint				m_iNumKeyFrames = {};
	_int				m_iBoneIndex = {};
	vector<KEYFRAME>	m_vecKeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, class CModel* pModel);
	static CChannel* Create_Binary(ifstream& file);
	virtual void Free() override;
};

NS_END