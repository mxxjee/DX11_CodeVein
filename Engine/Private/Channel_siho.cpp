#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, CModel* pModel)
{
    m_iBoneIndex = pModel->Get_BoneIndex(pAIChannel->mNodeName.data);
    if (m_iBoneIndex == -1)
        return E_FAIL;

    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3     vScale{};
    _float4     vRotation{};
    _float3     vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME    KeyFrame{};

        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;
        
        m_vecKeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

HRESULT CChannel::Initialize_Binary(ifstream& file)
{
    if (FAILED(Read_From_Binary(file)))
        return E_FAIL;

    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
    if (fCurrentTrackPosition == 0.f)
        (*pCurrentKeyFrameIndex) = 0;

    // fCurrentTrackPosition에 맞는 현재 뼈의 상태를 만든다.
    _float4x4       TransformationMatrix = {};

    // 마지막 키프레임 정보를 가져온다.
    KEYFRAME    LastKeyFrame = m_vecKeyFrames.back();

    _vector vScale{}, vRotation{}, vTranslation{};

    // 현재 재생위치가 마지막 키프레임의 위치를 지나면 마지막 키프레임 기준으로 SRT 정보를 유지한다.
    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else // 재생 위치가 마지막 키프레임을 지나지 않았다면 이전 키프레임과 다음 키프레임의 값을 보간하여 갱신해준다.
    {
        _vector     vLeftScale{}, vRightScale{};
        _vector     vLeftRotation{}, vRightRotation{};
        _vector     vLeftTranslation{}, vRightTranslation{};

        while (fCurrentTrackPosition >= m_vecKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        vLeftScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrameIndex)].vScale);
        vRightScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);

        vLeftRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
        vRightRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);

        vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
        vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);

        _float fRatio = (fCurrentTrackPosition - m_vecKeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
            (m_vecKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
        vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
    }

    DirectX::XMStoreFloat4x4(&TransformationMatrix,
        XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));

    vecBones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Write_To_Binary(ofstream& file)
{
    file.write(CHARCAST(&m_iBoneIndex), sizeof(_uint));
    file.write(CHARCAST(&m_iNumKeyFrames), sizeof(_uint));
    file.write(CHARCAST(m_vecKeyFrames.data()), sizeof(KEYFRAME) * m_iNumKeyFrames);

    return S_OK;
}

HRESULT CChannel::Read_From_Binary(ifstream& file)
{
    file.read(CHARCAST(&m_iBoneIndex), sizeof(_uint));
    file.read(CHARCAST(&m_iNumKeyFrames), sizeof(_uint));

    m_vecKeyFrames.resize(m_iNumKeyFrames);
    file.read(CHARCAST(m_vecKeyFrames.data()), sizeof(KEYFRAME) * m_iNumKeyFrames);

    return S_OK;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create_Binary(ifstream& file)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize_Binary(file)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();

    m_vecKeyFrames.clear();
}