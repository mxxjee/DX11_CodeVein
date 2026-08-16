// 입력 버퍼 (읽기 전용)
StructuredBuffer<float> g_Input : register(t0);

// 출력 버퍼 (읽기/쓰기)
RWStructuredBuffer<float> g_Output : register(u0);

// 입출력 버퍼
RWStructuredBuffer<float> g_Data : register(u1);



// 입출력 버퍼
// 사용하는 쉐이더가 다르면(CS의 경우 함수가 다름 == 다른 쉐이더임) 레지스터 번호가 같아도 됨
// 근데 어지간하면 같은 hlsl 안에서는 다르게 선언합시다 헷갈리니까
RWStructuredBuffer<float4x4> g_Bones : register(u2);

// 본행렬 예시용 상수버퍼
cbuffer CB_Info : register(b10)
{
    uint g_NumBones;
    float3 padding;
};



// 한 그룹당 64개 스레드
[numthreads(256, 1, 1)]
void CS_MAIN(uint3 dispatchID : SV_DispatchThreadID)
{
    // 각 스레드가 자기 인덱스의 값을 2배로
    g_Output[dispatchID.x] = g_Input[dispatchID.x] * 2.0f;
}

[numthreads(64, 1, 1)]
void CS_ACC(uint3 dispatchID : SV_DispatchThreadID)
{
    // 각 스레드가 자기 인덱스의 값을 증가
    g_Data[dispatchID.x] = g_Data[dispatchID.x] + 1.0f;
}

[numthreads(256, 1, 1)]
void CS_BONES(uint3 dispatchID : SV_DispatchThreadID)
{
    // 실제 본 행렬 업데이트는 이것보다 훨씬 복잡함
    // 그냥 이만큼 해도 프레임 드랍이 훨씬 덜 하다는거 확인용
    if (dispatchID.x >= g_NumBones)
        return;
    
    // 테스트용: 행렬에 간단한 연산
    g_Bones[dispatchID.x] = mul(g_Bones[dispatchID.x], g_Bones[dispatchID.x]);

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