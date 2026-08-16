#include "Shader_Buffer.hlsli"

// mip chain 생성용 Compute Shader

// Mip 0 패스: Target_Depth(float4)에서 .r 추출
// Mip 1~N 패스: 이전 mip(float)에서 2x2 max downsample
Texture2D g_PrevMip : register(t0); // float4 또는 float, 패스에 따라 다름
RWTexture2D<float> g_NextMip : register(u0);

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    // Mip 0: Target_Depth(.r = NDC z)에서 단순 복사
    if (g_IsCopyPass)
    {
        // Target_Depth는 R32G32B32A32_FLOAT
        // .r = ProjPos.z / ProjPos.w (NDC z, 0 ~ 1)
        // w나누기 된 NDC값(0 ~ 1)
        uint2 coord = DTid.xy;
        if (coord.x >= (uint) g_MipTexSize.x || coord.y >= (uint) g_MipTexSize.y)
            return;

        g_NextMip[coord] = g_PrevMip[coord].r;
        return;
    }

    // Mip 1~N: 2x2 블록에서 max depth 추출
    uint2 srcCoord = DTid.xy * 2;

    // 홀수 해상도 대응: 텍스처 경계 clamping
    uint2 maxCoord = uint2((uint) g_MipTexSize.x - 1, (uint) g_MipTexSize.y - 1);

    float d0 = g_PrevMip[min(srcCoord + uint2(0, 0), maxCoord)].r;
    float d1 = g_PrevMip[min(srcCoord + uint2(1, 0), maxCoord)].r;
    float d2 = g_PrevMip[min(srcCoord + uint2(0, 1), maxCoord)].r;
    float d3 = g_PrevMip[min(srcCoord + uint2(1, 1), maxCoord)].r;

    g_NextMip[DTid.xy] = max(max(d0, d1), max(d2, d3));
}