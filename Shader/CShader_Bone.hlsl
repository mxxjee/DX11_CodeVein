#include "Shader_Buffer.hlsli"

struct BoneHierarchy
{
    int iParent;
    int iDepth;
    int iPadding0;
    int iPadding1;
};

struct KEYFRAME
{
    float3 vScale;
    float fTrackPosition;
    float4 vRotation;
    float3 vPosition;
    float fPadding;
};

struct CHANNEL
{
    uint iKeyStart;
    uint iKeyCount;
    uint iPadding0;
    uint iPadding1;    
};

struct ANIMINFO
{
    uint iChannelStart;
    uint iNumBones;
    float fDuration;
    float fTickPersecond;
};

struct POSE_SRT
{
    float3 vScale;
    float fPadding0;
    float4 vRotation;
    float3 vPosition;
    float fPadding1;
};

struct BoneMatrix
{
    row_major float4x4 mat;
};

struct DRIVENDELTA
{
    uint iTargetBone;
    uint iPadding0;
    uint iPadding1;
    uint iPadding2;
    float4 vQuaternionDelta;
};

cbuffer AnimLocalBuffer : register(b6)
{
    uint g_LiAnimIndex;
    uint g_LiNumBones;
    uint g_LiRootBoneIndex;
    uint g_LbAnimLoop;
    
    float g_LfCurrentFrame;
    float g_LfLerpRatio;
    uint g_LbLerping;
    uint g_LbRemoveRootTranslation;
    
    uint g_LiUpperAnimIndex;
    uint g_LbUpperAnimLoop;
    float g_LfUpperCurrentFrame;
    uint g_LbUpperRemoveRootTranslation;

    float g_LbUpperBodyBlendEnalbe;
    float g_LfUpperLayerWeight;
    float g_LfUpperLerpRatio;
    uint g_LbUpperLerping;
}

cbuffer ReadbackBuffer : register(b12)
{
    uint4 g_iReadbackIndices[2];
    uint g_iReadbackCount;
    uint3 g_iPadding;
}

cbuffer DrapeBoneBuffer : register(b11)
{
    uint g_iDrapeDeltaCount;
    uint3 g_iPadding0;
}

float4 QuatSlerp(float4 PrevQuat , float4 NextQuat , float fTime) //쿼터니언 선형 보간
{
    if (dot(PrevQuat, NextQuat) < 0.0f) // 내적이 음수일 경우 반대 방향으로 회전하는 것을 방지하기 위해
        NextQuat = -NextQuat;
    
    float fCosTheta = saturate(dot(PrevQuat, NextQuat));
    if(fCosTheta >0.9995f)
        return normalize(lerp(PrevQuat, NextQuat, fTime));
    
    // 표준 Slerp 공식 적용
    float fTheta = acos(fCosTheta);
    float fSinTheta = sin(fTheta);
    
    // 호의 길이에 따른 보간 가중치(Weight) 계산
    float fWeight0 = sin((1.0f - fTime) * fTheta) / fSinTheta;
    float fWeight1 = sin(fTime * fTheta) / fSinTheta;
    
    return PrevQuat * fWeight0 + NextQuat * fWeight1;
}

float3x3 QuationToMatrix3x3(float4 Quat) //쿼터니언을 3x3회전 행렬로 변환 / x,y,z축 방향으로 펼쳐놓고 마지막에 스케일이랑 이동을 더해서 최종4x4행렬로 조립
{
    Quat = normalize(Quat);
    float x = Quat.x, y = Quat.y, z = Quat.z, w = Quat.w;
    
    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;
    
    // 쿼터니언 성분을 이용한 3x3 회전 행렬 조립 공식
    return float3x3(
    1 - 2 * (yy + zz), 2 * (xy + wz), 2 * (xz - wy),
    2 * (xy - wz), 1 - 2 * (xx + zz), 2 * (yz + wx),
    2 * (xz + wy), 2 * (yz - wx), 1 - 2 * (xx + yy)
    );
    
}

float4 QuatMul(float4 a, float4 b)
{
    return float4(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

row_major float4x4 ComposeLocal(float3 Scale, float4 Quat, float3 Translation) //로컬 행렬 조립 (Scale * Rotation * Translation)
{
    float3x3 Rotation = QuationToMatrix3x3(Quat);
    
    row_major float4x4 Mat;
    // 각 축(Row)에 스케일을 적용하여 회전과 크기를 동시 결합
    Mat[0] = float4(Rotation[0] * Scale.x, 0.0f);
    Mat[1] = float4(Rotation[1] * Scale.y, 0.0f);
    Mat[2] = float4(Rotation[2] * Scale.z, 0.0f);
    Mat[3] = float4(Translation, 1.0f); // 마지막 행에 이동(Translation) 값 적용
    return Mat;

}

POSE_SRT BlendPoseSRT(POSE_SRT BasePose, POSE_SRT UpperPose, float fWeight)
{
    fWeight = saturate(fWeight);

    POSE_SRT OutPose;
    OutPose.vScale = lerp(BasePose.vScale, UpperPose.vScale, fWeight);
    OutPose.vRotation = QuatSlerp(BasePose.vRotation, UpperPose.vRotation, fWeight);
    OutPose.vPosition = lerp(BasePose.vPosition, UpperPose.vPosition, fWeight);
    OutPose.fPadding0 = 0.0f;
    OutPose.fPadding1 = 0.0f;
    return OutPose;
}

//SRV 그룹들
StructuredBuffer<BoneMatrix> g_LocalMatrix : register(t0); // 기존 Channel에서 하던 LocalMatrix 만들어주는 과정
StructuredBuffer<BoneHierarchy> g_BoneHierarchy : register(t1);

StructuredBuffer<KEYFRAME>  g_KeyFrame : register(t2);
StructuredBuffer<CHANNEL>   g_Channel : register(t3);
StructuredBuffer<ANIMINFO>  g_AnimInfo : register(t4);
StructuredBuffer<POSE_SRT>  g_BindPoseSRT : register(t5); //애니메이션 키프레임이 0초일때 기본값으로 줄 SRT (Bone에서 바로 가져오는 용도)
StructuredBuffer<POSE_SRT>  g_FrozenPoseSRT : register(t6); 
//StructuredBuffer<POSE_SRT>  g_LocalPoseSRT : register(t7); //CS_COPYFROZEN에서 사용할 저장용 ->CopyReosurce로 바꿈
StructuredBuffer<BoneMatrix> g_CombinedMatrixSRV : register(t7);
StructuredBuffer<float> g_UpperBodyMaskWeightSRV : register(t8); //상체 블렌드 가중치
StructuredBuffer<DRIVENDELTA> g_DrivenDeltaSRV : register(t9); 

//UAV 그룹들
RWStructuredBuffer<BoneMatrix> g_CombinedMatrix : register(u0); //UAV 하나로 읽고/쓰기 같은 버퍼에서 parent 읽고 self로 쓰기

RWStructuredBuffer<BoneMatrix> g_LocalMatrixRW : register(u1); // 변수 이름 읽기용이랑 겹쳐서 RW로 읽기,쓰기용  CS_진입점함수 EvaluateLocal이 여기 씀
RWStructuredBuffer<uint>       g_CurrentKeyIndex : register(u2); // 읽기/쓰기 둘 다
//RWStructuredBuffer<POSE_SRT>   g_FrozenPoseSRTRW : register(u3); // CS_진입점함수CopyFrozen이 여기 씀 / 애니메이션 보간 시작할때 행렬 저장
RWStructuredBuffer<POSE_SRT>   g_LocalPoseSRTRW : register(u4);
RWStructuredBuffer<BoneMatrix> g_ReadbackMatrix : register(u5);
RWStructuredBuffer<uint>       g_UpperCurrentKeyIndex : register(u6); //상체 현재 키 인덱스

// 입출력 버퍼
// 사용하는 쉐이더가 다르면(CS의 경우 함수가 다름 == 다른 쉐이더임) 레지스터 번호가 같아도 됨
// 근데 어지간하면 같은 hlsl 안에서는 다르게 선언합시다 헷갈리니까
// 한 그룹당 256개 스레드

POSE_SRT EvaluateBasePose(uint iBoneIndex)
{
    ANIMINFO AnimInfo = g_AnimInfo[g_LiAnimIndex]; //현재 재생중인 애니메이션 정보 
    uint iChannelIndex = AnimInfo.iChannelStart + iBoneIndex; //CPU에서 채널의 뼈 이름으로 맵핑되어있는 인덱스를 기준으로 채웠기 때문에 +i로 바로 접근가능
    CHANNEL Channel = g_Channel[iChannelIndex]; //실제 채널 접근
    
    POSE_SRT TargetPos;
    if (Channel.iKeyCount == 0) //채널없는본
    {
        TargetPos = g_BindPoseSRT[iBoneIndex];
    }
    else
    {
        float fCurrentFrame = g_LfCurrentFrame;
    
        if (g_LbAnimLoop != 0 && AnimInfo.fDuration > 0.0001f)
            fCurrentFrame = fmod(fCurrentFrame, AnimInfo.fDuration); //시간 보정
    
        uint iCurrentKeyIndex = g_CurrentKeyIndex[iBoneIndex];
        if (iCurrentKeyIndex >= Channel.iKeyCount) //현재 채널이 최대 범위와 같거나 벗어나면
            iCurrentKeyIndex = Channel.iKeyCount - 1; //마지막 키프레임으로 고정시킴으로써 안전장치 + 최대 인덱스는 -1
    
    //기존 채널에서 사용하던 프레임인덱스 증가 Channel에서 하던작업 while (iIndex +1  < iLastKeyFrames && _currentTrackPosition >= m_vecKeyFrames[iIndex+1].fTrackPosition)     ++iIndex;
        while (iCurrentKeyIndex + 1 < Channel.iKeyCount //다음 칸이 실제 존재하는지 
        && g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex + 1].fTrackPosition <= fCurrentFrame) //다음 칸의 시간보다 현재 시간이 더 뒤인지 체크
            iCurrentKeyIndex++; //현재 시간보다 작거나 같은 키프레임 중 가장 마지막것을 가리킴
    
    //애니메이션 루프되어 처음으로 돌아가거나 시간이 뒤로 튀었을 때 인덱스를 거꾸로 거슬러 올라가서 현재 시간에 딱 맞는 위치를 찾아주는 역할
        while (iCurrentKeyIndex > 0 && //현재 키프레임인덱스가 0번보다 크고
        g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex].fTrackPosition > fCurrentFrame)//현재 칸의 시간이 현재 재생중인 시간보다 작다면 뒤로 가야하므로 
            iCurrentKeyIndex--;
    
        uint iNextKeyIndex = min(iCurrentKeyIndex + 1, Channel.iKeyCount - 1); //다음 키프레임인덱스 
    
    //기존 왼쪽 키프레임과 오른쪽 키프레임
        KEYFRAME LeftKeyFrame = g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex];
        KEYFRAME RightKeyFrame = g_KeyFrame[Channel.iKeyStart + iNextKeyIndex];
    
    //기존C++_float ration = (_currentTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition)/ (m_vecKeyFrames[ * _frameIndex + 1].fTrackPosition - m_vecKeyFrames[ * _frameIndex].fTrackPosition); // 비율

        float fTotalLength = RightKeyFrame.fTrackPosition - LeftKeyFrame.fTrackPosition; //전체 재생 길이
        float fRatio = (fTotalLength > 0.00001f) ? saturate((fCurrentFrame - LeftKeyFrame.fTrackPosition) / fTotalLength) : 0.0f; //비율
    
        TargetPos.vScale = lerp(LeftKeyFrame.vScale, RightKeyFrame.vScale, fRatio);
        TargetPos.vRotation = QuatSlerp(LeftKeyFrame.vRotation, RightKeyFrame.vRotation, fRatio);
        TargetPos.vPosition = lerp(LeftKeyFrame.vPosition, RightKeyFrame.vPosition, fRatio);
        TargetPos.fPadding0 = 0;
        TargetPos.fPadding1 = 0;
        
        g_CurrentKeyIndex[iBoneIndex] = iCurrentKeyIndex; //현재 키인덱스를 캐싱해서 인덱스를 매번 처음부터 돌지않도록
    }
    
    //루트 이동 제거 
    if (g_LbRemoveRootTranslation != 0 && iBoneIndex == g_LiRootBoneIndex)
    {
        TargetPos.vPosition = float3(0.f, 0.f, 0.f);
        TargetPos.vRotation = float4(0.f, 0.f, 0.f, 1.f);
    }
        

    if (g_LbLerping != 0)
    {
        POSE_SRT FrozenSRT = g_FrozenPoseSRT[iBoneIndex];

        TargetPos.vScale = lerp(FrozenSRT.vScale, TargetPos.vScale, g_LfLerpRatio);
        TargetPos.vRotation = QuatSlerp(FrozenSRT.vRotation, TargetPos.vRotation, g_LfLerpRatio);
        TargetPos.vPosition = lerp(FrozenSRT.vPosition, TargetPos.vPosition, g_LfLerpRatio);

        if (g_LbRemoveRootTranslation != 0 && iBoneIndex == g_LiRootBoneIndex)
        {
            TargetPos.vPosition = float3(0.f, 0.f, 0.f);
            TargetPos.vRotation = float4(0.f, 0.f, 0.f, 1.f);
        }
    }

    return TargetPos;
}

POSE_SRT EvaluateUpperPose(uint iBoneIndex)
{
    POSE_SRT TargetPos;
    
    if (g_LiUpperAnimIndex == 0xFFFFFFFF) //Upper(상체)애니메이션 없는경우
    {
        TargetPos = g_BindPoseSRT[iBoneIndex];
        return TargetPos;
    }
    
    ANIMINFO AnimInfo = g_AnimInfo[g_LiUpperAnimIndex]; //현재 재생중인 애니메이션 정보 
    uint iChannelIndex = AnimInfo.iChannelStart + iBoneIndex; //CPU에서 채널의 뼈 이름으로 맵핑되어있는 인덱스를 기준으로 채웠기 때문에 +i로 바로 접근가능
    CHANNEL Channel = g_Channel[iChannelIndex]; //실제 채널 접근
    
    if (Channel.iKeyCount == 0) //채널없는본
    {
        TargetPos = g_BindPoseSRT[iBoneIndex];
    }
    else
    {
        float fCurrentFrame = g_LfUpperCurrentFrame;
    
        if (g_LbUpperAnimLoop != 0 && AnimInfo.fDuration > 0.0001f)
            fCurrentFrame = fmod(fCurrentFrame, AnimInfo.fDuration); //시간 보정
    
        uint iCurrentKeyIndex = g_UpperCurrentKeyIndex[iBoneIndex];
        if (iCurrentKeyIndex >= Channel.iKeyCount) //현재 채널이 최대 범위와 같거나 벗어나면
            iCurrentKeyIndex = Channel.iKeyCount - 1; //마지막 키프레임으로 고정시킴으로써 안전장치 + 최대 인덱스는 -1
    
    //기존 채널에서 사용하던 프레임인덱스 증가 Channel에서 하던작업 while (iIndex +1  < iLastKeyFrames && _currentTrackPosition >= m_vecKeyFrames[iIndex+1].fTrackPosition)     ++iIndex;
        while (iCurrentKeyIndex + 1 < Channel.iKeyCount //다음 칸이 실제 존재하는지 
        && g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex + 1].fTrackPosition <= fCurrentFrame) //다음 칸의 시간보다 현재 시간이 더 뒤인지 체크
            iCurrentKeyIndex++; //현재 시간보다 작거나 같은 키프레임 중 가장 마지막것을 가리킴
    
    //애니메이션 루프되어 처음으로 돌아가거나 시간이 뒤로 튀었을 때 인덱스를 거꾸로 거슬러 올라가서 현재 시간에 딱 맞는 위치를 찾아주는 역할
        while (iCurrentKeyIndex > 0 && //현재 키프레임인덱스가 0번보다 크고
        g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex].fTrackPosition > fCurrentFrame)//현재 칸의 시간이 현재 재생중인 시간보다 작다면 뒤로 가야하므로 
            iCurrentKeyIndex--;
    
        uint iNextKeyIndex = min(iCurrentKeyIndex + 1, Channel.iKeyCount - 1); //다음 키프레임인덱스 
    
    //기존 왼쪽 키프레임과 오른쪽 키프레임
        KEYFRAME LeftKeyFrame = g_KeyFrame[Channel.iKeyStart + iCurrentKeyIndex];
        KEYFRAME RightKeyFrame = g_KeyFrame[Channel.iKeyStart + iNextKeyIndex];
    
    //기존C++_float ration = (_currentTrackPosition - m_vecKeyFrames[*_frameIndex].fTrackPosition)/ (m_vecKeyFrames[ * _frameIndex + 1].fTrackPosition - m_vecKeyFrames[ * _frameIndex].fTrackPosition); // 비율

        float fTotalLength = RightKeyFrame.fTrackPosition - LeftKeyFrame.fTrackPosition; //전체 재생 길이
        float fRatio = (fTotalLength > 0.00001f) ? saturate((fCurrentFrame - LeftKeyFrame.fTrackPosition) / fTotalLength) : 0.0f; //비율
    
        TargetPos.vScale = lerp(LeftKeyFrame.vScale, RightKeyFrame.vScale, fRatio);
        TargetPos.vRotation = QuatSlerp(LeftKeyFrame.vRotation, RightKeyFrame.vRotation, fRatio);
        TargetPos.vPosition = lerp(LeftKeyFrame.vPosition, RightKeyFrame.vPosition, fRatio);
        TargetPos.fPadding0 = 0;
        TargetPos.fPadding1 = 0;
        
        g_UpperCurrentKeyIndex[iBoneIndex] = iCurrentKeyIndex; //현재 키인덱스를 캐싱해서 인덱스를 매번 처음부터 돌지않도록
    }
   
    //루트 이동 제거 
    if (g_LbUpperRemoveRootTranslation != 0 && iBoneIndex == g_LiRootBoneIndex)
        TargetPos.vPosition = float3(0.f, 0.f, 0.f);

    if (g_LbUpperLerping != 0)
    {
        POSE_SRT FrozenSRT = g_FrozenPoseSRT[iBoneIndex];

        TargetPos.vScale = lerp(FrozenSRT.vScale, TargetPos.vScale, g_LfUpperLerpRatio);
        TargetPos.vRotation = QuatSlerp(FrozenSRT.vRotation, TargetPos.vRotation, g_LfUpperLerpRatio);
        TargetPos.vPosition = lerp(FrozenSRT.vPosition, TargetPos.vPosition, g_LfUpperLerpRatio);

        if (g_LbRemoveRootTranslation != 0 && iBoneIndex == g_LiRootBoneIndex)
            TargetPos.vPosition = float3(0.f, 0.f, 0.f);
    }
    
    return TargetPos;
}

[numthreads(256, 1, 1)]
void CS_EVALUATELOCAL(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;
    if(i >=g_LiNumBones)
        return;
    
    POSE_SRT BasePose = EvaluateBasePose(i); //기존 포즈
    POSE_SRT UpperPose = EvaluateUpperPose(i); //상체 포즈
    
    float fMaskWeight = g_UpperBodyMaskWeightSRV[i]; //CPU에서 초기화 시점에 준비한 상체본마다의 가중치값
    float fUpperBlendWeight = 0.0f; 
    
    if(g_LbUpperBodyBlendEnalbe !=0 && g_LiUpperAnimIndex>=0) //상체 블렌드 true 고 상체 애니메이션이 있다면
        fUpperBlendWeight = saturate(g_LfUpperLayerWeight * fMaskWeight); //상체블렌드 가중치는 0~1(상체 전체에대한 가중치 * 초기화 시점 준비된 상체별 가중치값)
    
    POSE_SRT FinalPose = BlendPoseSRT(BasePose, UpperPose, fUpperBlendWeight);
    
    g_LocalPoseSRTRW[i] = FinalPose;
    
    g_LocalMatrixRW[i].mat = ComposeLocal(FinalPose.vScale, FinalPose.vRotation, FinalPose.vPosition); //크자이 조합해서 행렬 완성 

}

cbuffer CB_BONE : register(b7)
{
    uint g_NumBones;
    uint g_CurrentDepth;
    uint iPadding0;
    uint iPadding1;
    float4x4 g_PreTransformMatrix;
}

[numthreads(256, 1, 1)]
void CS_APPLYDRIVENBONES(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;
    if(i >= g_LiNumBones)
        return;
    
    POSE_SRT Pose = g_LocalPoseSRTRW[i]; //위에서 완성된 로컬포즈 가져와서
    
    for (uint k = 0; k< g_iDrapeDeltaCount;++k)
    {
        DRIVENDELTA DrivenDelta = g_DrivenDeltaSRV[k];
        if(DrivenDelta.iTargetBone == i)
        {
            Pose.vRotation = normalize(QuatMul(Pose.vRotation, DrivenDelta.vQuaternionDelta)); // normalize(QuatMul(Pose.vRotation, DrivenDelta.vQuaternionDelta))

        }
    }
        
    g_LocalPoseSRTRW[i] = Pose;
    g_LocalMatrixRW[i].mat = ComposeLocal(Pose.vScale, Pose.vRotation, Pose.vPosition); //크자이 조합해서 행렬 완성 
}

[numthreads(256, 1, 1)]
void CS_BONECOMBINED(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x; //id.x는 현재 실행 중인 스레드의 전역 변호로 이 번호를 뼈의 인덱스로 사용하여, 각 스레드가 어떤 뼈를 계산할지 결정하는 기준점으로 사용
    if (i >= g_NumBones)
        return;

    BoneHierarchy hierarchy = g_BoneHierarchy[i]; //이게 이제 스트럭처 버퍼 배열접근 가능한거고
    if (hierarchy.iDepth != (int) g_CurrentDepth) //저장된 계층과 매프레임 현재 계층을 보내주는 값이 다를경우 return
        return;

    //이게 이제 원래 C++에서 해주던 Combined행렬 계산
    if(hierarchy.iParent <0) //이건 이제 Root본일때
        g_CombinedMatrix[i].mat = mul(g_LocalMatrix[i].mat, g_PreTransformMatrix);
    else //나머지 자식들
    {
        // g_CombinedMatrix[hierarchy.iParent]: 이전 Dispatch에서 이미 계산된 '부모의 결과값'을 읽어옴 (Read)
        // g_LoaclMatrix[i]: 이번에 적용할 '나의 상대적 변환' 값
        // g_CombinedMatrix[i]: 계산된 최종 결과를 '나의 슬롯'에 다시 저장 (Write)
        g_CombinedMatrix[i].mat = mul(g_LocalMatrix[i].mat, g_CombinedMatrix[hierarchy.iParent].mat);
    }
}

[numthreads(64, 1, 1)]
void CS_READBACK(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;
    if (i >= g_iReadbackCount)
        return;
    
    uint iBoneIndex = g_iReadbackIndices[i/4][i%4];
    
    g_ReadbackMatrix[i].mat = g_CombinedMatrixSRV[iBoneIndex].mat;
}

/* 주의사항 */
/*
되도록이면 32 또는 64의 배수로 쓰레드를 배치할 것
이유
NVIDIA 그래픽카드는 32의 배수로 단위(Warp)를 설정해뒀고
AMD 그래픽카드는 64의 배수로 단위(Wave)를 설정해둠

즉 NVIDIA그래픽카드를 사용하는데 쓰레드를 33개 배치했으면 나머지 31개 쓰레드가 놀게 됨
AMD면 65개 배치했을때 63개가 놀게 됨

그래서 되도록이면 32 또는 64의 배수로 쓰레드 배치할 것
64가 제일 좋을듯

근데 그냥 64나 256 쓰셈 큰건 256 아닌건 64
그룹 수만 늘리면 되니까 고정합시당
왜냐하면 64랑 32 겹치는게 64, 256이라서

*/