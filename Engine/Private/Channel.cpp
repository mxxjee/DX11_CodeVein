#include "Engine_Define.h"
#include "Channel.h"

#include "Model.h"
#include "Bone.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Channel::Channel()
{
}

Engine::Channel::Channel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

Engine::Channel::~Channel()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Channel::Initialize_FBX(const aiNodeAnim* _aiNodeAnim, Model* _model, myChannel* _myChannel)
{
	m_iChannelIndex = _myChannel->mChannelIndex = _model->Get_BoneIndex(_aiNodeAnim->mNodeName.data);
	if (m_iChannelIndex == -1)
	{
		MSG_ON(L"뭔가잘못됨", L"잘못됨");
		BREAK;
		return E_FAIL;
	}
	
	m_iNumKeyFrames = max(_aiNodeAnim->mNumScalingKeys, _aiNodeAnim->mNumRotationKeys);
	m_iNumKeyFrames = _myChannel->mNumKeyFrames = max(m_iNumKeyFrames, _aiNodeAnim->mNumPositionKeys); //Rotation으로 되어있었음. 
    m_iNumScalingKeys = _myChannel->mNumScalingKeys  = _aiNodeAnim->mNumScalingKeys;
    m_iNumRotationKeys = _myChannel->mNumRotationKeys = _aiNodeAnim->mNumRotationKeys;
    m_iNumPositionKeys = _myChannel->mNumPositionKeys = _aiNodeAnim->mNumPositionKeys;


	m_vecKeyFrames.reserve(m_iNumKeyFrames);
    _myChannel->mScalingKeys.resize(_aiNodeAnim->mNumScalingKeys);
    _myChannel->mRotationKeys.resize(_aiNodeAnim->mNumRotationKeys);
    _myChannel->mPositionKeys.resize(_aiNodeAnim->mNumPositionKeys);

	_float3 scale = {};
	_float4 rotation = {};
	_float3 position = {};

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME keyframe{};

		if (_aiNodeAnim->mNumScalingKeys > i)
		{
			memcpy(&scale, &_aiNodeAnim->mScalingKeys[i].mValue, sizeof(_float3));
            memcpy(&_myChannel->mScalingKeys[i].mValue, &scale, sizeof(_float3));
			//const aiVector3D& scalekey = _aiNodeAnim->mScalingKeys[i].mValue;
			//keyframe.vScale = { scalekey.x, scalekey.y, scalekey.z };

			keyframe.fTrackPosition = _myChannel->mScalingKeys[i].mTime = (_float)_aiNodeAnim->mScalingKeys[i].mTime;
		}

		if (_aiNodeAnim->mNumRotationKeys > i)
		{
			rotation.x = _myChannel->mRotationKeys[i].mValue.x = _aiNodeAnim->mRotationKeys[i].mValue.x;
			rotation.y = _myChannel->mRotationKeys[i].mValue.y = _aiNodeAnim->mRotationKeys[i].mValue.y;
			rotation.z = _myChannel->mRotationKeys[i].mValue.z = _aiNodeAnim->mRotationKeys[i].mValue.z;
			rotation.w = _myChannel->mRotationKeys[i].mValue.w = _aiNodeAnim->mRotationKeys[i].mValue.w;

			//const aiQuaternion& rotationkey = _aiNodeAnim->mRotationKeys[i].mValue;
			//keyframe.vRotation = { rotationkey.x, rotationkey.y, rotationkey.z, rotationkey.w };

			keyframe.fTrackPosition = _myChannel->mRotationKeys[i].mTime = (_float)_aiNodeAnim->mRotationKeys[i].mTime;
		}

		if (_aiNodeAnim->mNumPositionKeys > i)
		{
			memcpy(&position, &_aiNodeAnim->mPositionKeys[i].mValue, sizeof(_float3));
            memcpy(&_myChannel->mPositionKeys[i].mValue, &position, sizeof(_float3));
            //const aiVector3D& positionkey = _aiNodeAnim->mPositionKeys[i].mValue;
			//keyframe.vPosition = { positionkey.x, positionkey.y, positionkey.z };

			keyframe.fTrackPosition = _myChannel->mPositionKeys[i].mTime = (_float)_aiNodeAnim->mPositionKeys[i].mTime;
		}

		keyframe.vScale = scale;
		keyframe.vRotation = rotation;
		keyframe.vPosition = position;

        if (i == 0)
        {
            m_tFirstKeyFrameInfo.vScale = scale;
            m_tFirstKeyFrameInfo.vRotation = rotation;
            m_tFirstKeyFrameInfo.vPosition = position;
        }
		m_vecKeyFrames.push_back(keyframe);
	}

	return S_OK;
}

HRESULT Engine::Channel::Initialize_Binary(myChannel* _myChannel)
{
    m_iChannelIndex = _myChannel->mChannelIndex;
    if (m_iChannelIndex == -1)
    {
        MSG_ON(L"뭔가잘못됨", L"잘못됨");
        BREAK;
        return E_FAIL;
    }

    m_iNumKeyFrames = _myChannel->mNumKeyFrames;
    m_iNumScalingKeys = _myChannel->mNumScalingKeys;
    m_iNumRotationKeys = _myChannel->mNumRotationKeys;
    m_iNumPositionKeys = _myChannel->mNumPositionKeys;

    m_vecKeyFrames.reserve(m_iNumKeyFrames);

    _float3 scale = {};
    _float4 rotation = {};
    _float3 position = {};

    for (_uint i = 0; i < m_iNumKeyFrames; ++i)
    {
        KEYFRAME keyframe{};

        if (_myChannel->mNumScalingKeys > i)
        {
            memcpy(&scale, &_myChannel->mScalingKeys[i].mValue, sizeof(_float3));
            //const aiVector3D& scalekey = _aiNodeAnim->mScalingKeys[i].mValue;
            //keyframe.vScale = { scalekey.x, scalekey.y, scalekey.z };

            keyframe.fTrackPosition = _myChannel->mScalingKeys[i].mTime;
        }

        if (_myChannel->mNumRotationKeys > i)
        {
            rotation.x = _myChannel->mRotationKeys[i].mValue.x;
            rotation.y = _myChannel->mRotationKeys[i].mValue.y;
            rotation.z = _myChannel->mRotationKeys[i].mValue.z;
            rotation.w = _myChannel->mRotationKeys[i].mValue.w;

            //const aiQuaternion& rotationkey = _aiNodeAnim->mRotationKeys[i].mValue;
            //keyframe.vRotation = { rotationkey.x, rotationkey.y, rotationkey.z, rotationkey.w };

            keyframe.fTrackPosition = _myChannel->mRotationKeys[i].mTime;
        }

        if (_myChannel->mNumPositionKeys > i)
        {
            memcpy(&position, &_myChannel->mPositionKeys[i].mValue, sizeof(_float3));
            //memcpy(&_myChannel->mPositionKeys[i].mValue, &position, sizeof(_float3));
            //const aiVector3D& positionkey = _aiNodeAnim->mPositionKeys[i].mValue;
            //keyframe.vPosition = { positionkey.x, positionkey.y, positionkey.z };

            keyframe.fTrackPosition = _myChannel->mPositionKeys[i].mTime;
        }

        keyframe.vScale = scale;
        keyframe.vRotation = rotation;
        keyframe.vPosition = position;

        if (i == 0)
        {
            m_tFirstKeyFrameInfo.vScale = scale;
            m_tFirstKeyFrameInfo.vRotation = rotation;
            m_tFirstKeyFrameInfo.vPosition = position;
        }

        m_vecKeyFrames.push_back(keyframe);
    }

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void Engine::Channel::Update_TransformationMatrix(const vector<Bone*>& _bones, const _float _currentTrackPosition, _uint* _frameIndex, _bool _isLoop, _float _duration)
{
	if (_currentTrackPosition == 0.f)
		*_frameIndex = 0;

	/* fCurrentTrackPosition시간에 맞는 현재 뼈의 상태를 만든다.*/
	KEYFRAME lastKeyframe = m_vecKeyFrames.back();

	_vector scale = {};
	_vector rotation = {};
	_vector position = {};

	// 현재 트랙의 위치가 마지막 프레임의 키 트랙보다 높으면(애니메이션이 끝난 상태)
	if (_currentTrackPosition >= lastKeyframe.fTrackPosition)
	{
        if (_isLoop && m_vecKeyFrames.size() > 1)
        {
            KEYFRAME& firstKeyframe = m_vecKeyFrames[0];

            // 마지막 키프레임 ~ Duration 사이의 비율
            _float remainingTime = _duration - lastKeyframe.fTrackPosition;
            _float ratio = 0.f;
            if (remainingTime > 0.0001f)
                ratio = (_currentTrackPosition - lastKeyframe.fTrackPosition) / remainingTime;

            ratio = min(1.f, max(0.f, ratio));

            scale = XMVectorLerp(XMLoadFloat3(&lastKeyframe.vScale), 
                XMLoadFloat3(&firstKeyframe.vScale), ratio);
            rotation = XMQuaternionSlerp(XMLoadFloat4(&lastKeyframe.vRotation), 
                XMLoadFloat4(&firstKeyframe.vRotation), ratio);
            position = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f),
                XMVectorSetW(XMLoadFloat3(&firstKeyframe.vPosition), 1.f), ratio);
        }
        else
        {
            scale = XMLoadFloat3(&lastKeyframe.vScale);
            rotation = XMLoadFloat4(&lastKeyframe.vRotation);
            position = XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f);
        }
	}

	// 프레임마다 왼쪽과 오른쪽 트랙의 위치에 맞춰서 크 자 이 보간
	else
	{
		_vector leftScale{}, rightScale{};
		_vector leftRotation{}, rightRotation{};
		_vector leftPosition{}, rightPosition{};

		// 현재 트랙이 인덱스 번호의 오른쪽 트랙보다 클경우(마지막 트랙 인덱스는 위의 if문에서 걸러짐)
        if (_currentTrackPosition >= m_vecKeyFrames[(*_frameIndex + 1)].fTrackPosition)
            ++(*_frameIndex);

		leftScale = XMLoadFloat3(&m_vecKeyFrames[*_frameIndex].vScale);
		rightScale = XMLoadFloat3(&m_vecKeyFrames[*_frameIndex + 1].vScale);

		leftRotation = XMLoadFloat4(&m_vecKeyFrames[*_frameIndex].vRotation);
		rightRotation = XMLoadFloat4(&m_vecKeyFrames[*_frameIndex + 1].vRotation);

		leftPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[*_frameIndex].vPosition), 1.f);
    		rightPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[*_frameIndex + 1].vPosition), 1.f);

		_float ration = (_currentTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition)
			/ (m_vecKeyFrames[*_frameIndex + 1].fTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition); // 비율

		scale = XMVectorLerp(leftScale, rightScale, ration);
		rotation = XMQuaternionSlerp(leftRotation, rightRotation, ration);
		position = XMVectorLerp(leftPosition, rightPosition, ration);

        XMStoreFloat3(&m_tCurrentKeyFrameInfo.vScale, scale);
        XMStoreFloat4(&m_tCurrentKeyFrameInfo.vRotation, rotation);
        XMStoreFloat3(&m_tCurrentKeyFrameInfo.vPosition, position);
	}

	XMStoreFloat4x4(&m_matCurrentKeyFrameMatrix,
		XMMatrixAffineTransformation(scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rotation, position));

	_bones[m_iChannelIndex]->Set_TransformationMatrix(m_matCurrentKeyFrameMatrix);	
}

/******************************************************* 업데이트 함수 *******************************************************/
void Engine::Channel::Update_TransformationMatrix_Animation_Tool(const vector<class Bone*>& _bones, const _float _currentTrackPosition, _uint* _frameIndex, _bool _isLoop, _float _duration)
{
    if (_currentTrackPosition == 0.f)
        *_frameIndex = 0;

    /* fCurrentTrackPosition시간에 맞는 현재 뼈의 상태를 만든다.*/
    KEYFRAME lastKeyframe = m_vecKeyFrames.back();

    _vector scale = {};
    _vector rotation = {};
    _vector position = {};

    // 현재 트랙의 위치가 마지막 프레임의 키 트랙보다 높으면(애니메이션이 끝난 상태)
    if (_currentTrackPosition >= lastKeyframe.fTrackPosition)
    {
        if (_isLoop && m_vecKeyFrames.size() > 1)
        {
            KEYFRAME& firstKeyframe = m_vecKeyFrames[0];

            // 마지막 키프레임 ~ Duration 사이의 비율
            _float remainingTime = _duration - lastKeyframe.fTrackPosition;
            _float ratio = 0.f;
            if (remainingTime > 0.0001f)
                ratio = (_currentTrackPosition - lastKeyframe.fTrackPosition) / remainingTime;

            ratio = min(1.f, max(0.f, ratio));

            scale = XMVectorLerp(XMLoadFloat3(&lastKeyframe.vScale),
                XMLoadFloat3(&firstKeyframe.vScale), ratio);
            rotation = XMQuaternionSlerp(XMLoadFloat4(&lastKeyframe.vRotation),
                XMLoadFloat4(&firstKeyframe.vRotation), ratio);
            position = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f),
                XMVectorSetW(XMLoadFloat3(&firstKeyframe.vPosition), 1.f), ratio);
        }
        else
        {
            scale = XMLoadFloat3(&lastKeyframe.vScale);
            rotation = XMLoadFloat4(&lastKeyframe.vRotation);
            position = XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f);
        }
    }

    // 프레임마다 왼쪽과 오른쪽 트랙의 위치에 맞춰서 크 자 이 보간
    else
    {
        _vector leftScale{}, rightScale{};
        _vector leftRotation{}, rightRotation{};
        _vector leftPosition{}, rightPosition{};

        //이코드를 이제 현재 _currentTrackPostion에 맞는 프레임인덱스로 맞춰줘야한다는거잖아

        //여기서 이제 _frameIndex를 증가시키는게 아니라 , 다시 처음부터 순회하면서 이 위치를 기준으로 보간을 시작해줘야함.

        _uint iLastKeyFrames = _uint(m_vecKeyFrames.size() - 1);

        _uint iIndex = 0;
        while (iIndex +1  < iLastKeyFrames && _currentTrackPosition >= m_vecKeyFrames[iIndex+1].fTrackPosition)
            ++iIndex;

        if (iIndex >= iLastKeyFrames) //현재 인덱스가 라스트보다 -1작아야 왼쪽과 보관가능하므로
        {
            iIndex = iLastKeyFrames - 1; 
        }

        *_frameIndex = iIndex;


        leftScale = XMLoadFloat3(&m_vecKeyFrames[*_frameIndex].vScale);
        rightScale = XMLoadFloat3(&m_vecKeyFrames[*_frameIndex + 1].vScale);

        leftRotation = XMLoadFloat4(&m_vecKeyFrames[*_frameIndex].vRotation);
        rightRotation = XMLoadFloat4(&m_vecKeyFrames[*_frameIndex + 1].vRotation);

        leftPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[*_frameIndex].vPosition), 1.f);
        rightPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[*_frameIndex + 1].vPosition), 1.f);

        _float ration = (_currentTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition)
            / (m_vecKeyFrames[*_frameIndex + 1].fTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition); // 비율

        scale = XMVectorLerp(leftScale, rightScale, ration);
        rotation = XMQuaternionSlerp(leftRotation, rightRotation, ration);
        position = XMVectorLerp(leftPosition, rightPosition, ration);

        XMStoreFloat3(&m_tCurrentKeyFrameInfo.vScale, scale);
        XMStoreFloat4(&m_tCurrentKeyFrameInfo.vRotation, rotation);
        XMStoreFloat3(&m_tCurrentKeyFrameInfo.vPosition, position);
    }

    XMStoreFloat4x4(&m_matCurrentKeyFrameMatrix,
        XMMatrixAffineTransformation(scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rotation, position));

    _bones[m_iChannelIndex]->Set_TransformationMatrix(m_matCurrentKeyFrameMatrix);
}

void Engine::Channel::Interpolate_SRT(_float fTrackPosition, _bool _isLoop, _float fDuration, SRT_DESC& SRTOut, _vector& _outScale, _vector& _outRotation, _vector& _outPosition)
{
    KEYFRAME lastKeyframe = m_vecKeyFrames.back();

    _vector scale = {};
    _vector rotation = {};
    _vector position = {};

    // 현재 트랙의 위치가 마지막 프레임의 키 트랙보다 높으면(애니메이션이 끝난 상태)
    if (fTrackPosition >= lastKeyframe.fTrackPosition)
    {
        if (_isLoop && m_vecKeyFrames.size() > 1)
        {
            KEYFRAME& firstKeyframe = m_vecKeyFrames[0];

            // 마지막 키프레임 ~ Duration 사이의 비율
            _float remainingTime = fDuration - lastKeyframe.fTrackPosition;
            _float ratio = 0.f;
            if (remainingTime > 0.0001f)
                ratio = (fTrackPosition - lastKeyframe.fTrackPosition) / remainingTime;

            ratio = min(1.f, max(0.f, ratio));

            scale = XMVectorLerp(XMLoadFloat3(&lastKeyframe.vScale),
                XMLoadFloat3(&firstKeyframe.vScale), ratio);
            rotation = XMQuaternionSlerp(XMLoadFloat4(&lastKeyframe.vRotation),
                XMLoadFloat4(&firstKeyframe.vRotation), ratio);
            position = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f),
                XMVectorSetW(XMLoadFloat3(&firstKeyframe.vPosition), 1.f), ratio);
        }
        else
        {
            scale = XMLoadFloat3(&lastKeyframe.vScale);
            rotation = XMLoadFloat4(&lastKeyframe.vRotation);
            position = XMVectorSetW(XMLoadFloat3(&lastKeyframe.vPosition), 1.f);
        }
    }

    // 프레임마다 왼쪽과 오른쪽 트랙의 위치에 맞춰서 크 자 이 보간
    else
    {
        _vector leftScale{}, rightScale{};
        _vector leftRotation{}, rightRotation{};
        _vector leftPosition{}, rightPosition{};

        // 현재 트랙이 인덱스 번호의 오른쪽 트랙보다 클경우(마지막 트랙 인덱스는 위의 if문에서 걸러짐)
        _uint iLastKeyFrames = _uint(m_vecKeyFrames.size() - 1);

        _uint iIndex = 0;
        while (iIndex + 1 < iLastKeyFrames && fTrackPosition >= m_vecKeyFrames[iIndex + 1].fTrackPosition)
            ++iIndex;

        if (iIndex >= iLastKeyFrames) //현재 인덱스가 라스트보다 -1작아야 왼쪽과 보관가능하므로
        {
            iIndex = iLastKeyFrames - 1;
        }

        leftScale = XMLoadFloat3(&m_vecKeyFrames[iIndex].vScale);
        rightScale = XMLoadFloat3(&m_vecKeyFrames[iIndex + 1].vScale);

        leftRotation = XMLoadFloat4(&m_vecKeyFrames[iIndex].vRotation);
        rightRotation = XMLoadFloat4(&m_vecKeyFrames[iIndex + 1].vRotation);

        leftPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[iIndex].vPosition), 1.f);
        rightPosition = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[iIndex + 1].vPosition), 1.f);

        _float ration = (fTrackPosition - m_vecKeyFrames[iIndex].fTrackPosition)
            / (m_vecKeyFrames[iIndex + 1].fTrackPosition - m_vecKeyFrames[iIndex].fTrackPosition); // 비율

        scale = XMVectorLerp(leftScale, rightScale, ration);
        rotation = XMQuaternionSlerp(leftRotation, rightRotation, ration);
        position = XMVectorLerp(leftPosition, rightPosition, ration);

    }

    // 어차피 구조체 전달 한 번만 하는것 같아서 바꿈
    //XMStoreFloat3(&SRTOut.vScale, scale);
    //XMStoreFloat4(&SRTOut.vRotation, rotation);
    //XMStoreFloat3(&SRTOut.vPosition, position);

    // 직접 전달
    _outScale = scale;
    _outRotation = rotation;
    _outPosition = position;
}


//////////////////////////////////////////////////////// 데이터 저장 함수 ////////////////////////////////////////////////////////
MYCHANNEL Engine::Channel::Get_ChannelData()
{
    MYCHANNEL channel;

    channel.mChannelIndex = m_iChannelIndex;
    channel.mNumKeyFrames = m_iNumKeyFrames;
    channel.mNumScalingKeys  = m_iNumScalingKeys;
    channel.mNumRotationKeys = m_iNumRotationKeys;
    channel.mNumPositionKeys = m_iNumPositionKeys;

    channel.mScalingKeys.reserve(m_iNumKeyFrames);
    channel.mRotationKeys.reserve(m_iNumKeyFrames);
    channel.mPositionKeys.reserve(m_iNumKeyFrames);

    MYKEYS myscalekey;
    MYROTKEYS myrotkey;
    MYKEYS myposkey;

    for (_uint i = 0; i < m_iNumKeyFrames; ++i)
    {
        myscalekey.mValue = m_vecKeyFrames[i].vScale;
        myrotkey.mValue = m_vecKeyFrames[i].vRotation;
        myposkey.mValue = m_vecKeyFrames[i].vPosition;
        myscalekey.mTime = myrotkey.mTime = myposkey.mTime = m_vecKeyFrames[i].fTrackPosition;

        channel.mScalingKeys.push_back(myscalekey);
        channel.mRotationKeys.push_back(myrotkey);
        channel.mPositionKeys.push_back(myposkey);
    }

    return channel;
}
/******************************************************* 데이터 저장 함수 *******************************************************/


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Channel* Engine::Channel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiNodeAnim* _aiNodeAnim, Model* _model, myChannel* _myChannel)
{
    if (_model->Get_BoneIndex(_aiNodeAnim->mNodeName.data) == -1)
        return nullptr;

	Channel* pInstance = new Channel(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_FBX(_aiNodeAnim, _model, _myChannel), L"Channel 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Channel* Engine::Channel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myChannel* _myChannel)
{
    Channel* pInstance = new Channel(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Binary(_myChannel), L"Channel 생성 실패", L"Caution!!!", nullptr);
    
    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Channel::Free()
{
	__super::Free();

	m_vecKeyFrames.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

