#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc {
		unsigned int iNumLevels;
		HWND hWnd;
		HINSTANCE hInstance;

	}ENGINE_DESC;

    typedef struct Shader_Desc {
        _int iBufferNum = { -1 };
        _int iVariableNum = { -1 };
    }SHADERHANDLE;

    // 쉐이더 진입점 이름
    typedef struct tagShaderEntry
    {
        _string vsEntry;
        _string psEntry;
        _string gsEntry;
        _string hsEntry;
        _string dsEntry;
    }SHADERENTRY;

    // 쉐이더 진입점 배열
    typedef struct tagShaderEntryArray
    {
        SHADERENTRY* pEntries = nullptr;
        _uint iNumpass = {};
    }SHADERENTRIES;

    // Pass가 사용하는 리소스 정보
    typedef struct tagResourceBindingInfo {
        _string name = {};
        _uint   slot = {};                      // 해당 쉐이더 스테이지에서의 슬롯 번호
        D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};   // 리플렉션에서 가져온 전체 바인딩 정보
        _uint   stageMask = {};                 // VS=1, PS=2, HS=4 등 (stageMask)
        _uint structureStride = {};             // StructuredBuffer의 요소 크기 (e.g., sizeof(struct))
        const void* pInitialData = { nullptr }; // 초기 데이터 포인터 (로딩 시 업로드용)
    }RESOURCEINFO;

    // 쉐이더가 사용하는 Pass
    typedef struct Pass
    {
        ID3D11VertexShader*     pVS = { nullptr };
        ID3D11PixelShader*      pPS = { nullptr };
        ID3D11HullShader*       pHS = { nullptr };
        ID3D11DomainShader*     pDS = { nullptr };
        ID3D11GeometryShader*   pGS = { nullptr };

        ID3D11InputLayout* pInputLayout = { nullptr };

        // 각 타입별 바인딩 정보
        vector<RESOURCEINFO> vecCBs;        // Constant Buffers (정적 버퍼)
        vector<RESOURCEINFO> vecSRVs;       // Shader Resource Views (쉐이더 리소스 뷰)
        vector<RESOURCEINFO> vecSamplers;   // Samplers (샘플러)
        vector<RESOURCEINFO> vecStructuredBuffers; // Structured Buffers (구조체 버퍼)
    }PASS;

    typedef struct tagVertexPosition {
        XMFLOAT3 vPosition = {};

        static const unsigned int iNumElements = { 1 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
    }VTXPOS;

    typedef struct tagVertexColorPosition {
        XMFLOAT3    vPosition = {};
        XMFLOAT4     vColor = { 0,0,0,0 };

        static const unsigned int iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}

        };
    }VTXPOSCOR;

	typedef struct tagVertexPositionTextureCoordinate {
		XMFLOAT3 vPosition = {};
		XMFLOAT2 vTexCoord = {};

		static const unsigned int iNumElements = { 2 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXPOSTEX;

    typedef struct tagFontTextureCoordinate {
        XMFLOAT3 vPosition = {};
        XMFLOAT4 vColor= {};
        XMFLOAT2 vTexCoord = {};

        static const unsigned int iNumElements = { 3 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 유지
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}    // 유지
         };
    }FONTTEX;

	typedef struct tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXNORTEX;

    typedef struct tagVertexCube
    {
        XMFLOAT3		vPosition;
        XMFLOAT3		vTexcoord;

        static const unsigned int		iNumElements = { 2 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
    }VTXCUBE;

	typedef struct tagVertexMesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT3 vTangent{};
		XMFLOAT3 vBinormal{};
		XMFLOAT2 vTexcoord{};
		
		static const unsigned int		iNumElements = { 5 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXMESH;

	typedef struct tagVertexEffectMesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		
		static const unsigned int		iNumElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXEFFECTMESH;

	typedef struct tagVertexAnimationMesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT3 vTangent{};
		XMFLOAT3 vBinormal{};   
        XMFLOAT2 vTexcoord{};
        XMUINT4	 vBlendIndex{};
		XMFLOAT4 vBlendWeight{};
        XMFLOAT2 vTexcoord1{};

		static const unsigned int		iNumElements = { 8 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXANIMMESH;

    typedef struct tagVertexPlayerAnimMesh
    {
        XMFLOAT3 vPosition{};
        XMFLOAT3 vNormal{};
        XMFLOAT3 vTangent{};
        XMFLOAT3 vBinormal{};
        XMFLOAT2 vTexcoord{};
        XMUINT4	 vBlendIndex{};
        XMUINT4	 vBlendIndex2{};
        XMFLOAT4 vBlendWeight{};
        XMFLOAT4 vBlendWeight2{};
        XMFLOAT2 vTexcoord1{};

        static const unsigned int		iNumElements = { 10 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDINDICES", 1, DXGI_FORMAT_R32G32B32A32_UINT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 104, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 120, D3D11_INPUT_PER_VERTEX_DATA, 0}

        };

    }VTXPLAYERANIMMESH;

    typedef struct tagVertexMorphAnimMesh
    {
        XMFLOAT3 vPosition{};
        XMFLOAT3 vNormal{};
        XMFLOAT3 vTangent{};
        XMFLOAT3 vBinormal{};
        XMFLOAT2 vTexcoord{};
        XMUINT4	 vBlendIndex{};
        XMFLOAT4 vBlendWeight{};

        static const unsigned int		iNumElements = { 7 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
    }VTXMORPHMESH;

    typedef struct tagBoneHierarchyGpu
    {
        _int iParent = -1; //부모 인덱스(-1이면 루트)
        _int iDepth{}; // 계층 깊이 Depth별 Dispatch 연산하기 위해 (부모 완료 후 자식 순)
        _int iPadding0{}; //padding은 16바이트 맞추려고
        _int iPadding1{};
    }BONEINFO_GPU;

    typedef struct tagBoneUpdateCB //업데이트용 상수버퍼
    {
        _uint g_NumBones = {};
        _uint g_CurrentDepth = {};
        _uint iPadding0 = {};
        _uint iPadding1 = {};
        _float4x4 g_PreTransformMatrix = {};

    }BONECOMPUTE_DESC;

    typedef struct tagKeyFrameGPU
    {
        _float3 vScale = {};
        _float fTrackPosition = {}; //16

        _float4 vRotation = {};  //16

        _float3 vPosition = {}; 
        _float fPadding = {}; //16

    }KEYFRAME_GPU;

    typedef struct tagChannelGPU
    {
        _uint iKeyStart = {}; //키 시작 인덱스
        _uint iKeyCount = {}; //키 개수(iNumKeyFrames)
        _uint iPadding0 = {};
        _uint iPadding1 = {};

    }CHANNELINFO_GPU;

    typedef struct tagAnimGPU
    {
        _uint iChannelStart = {};//전체 채널버퍼에서 이 애니메이션의 첫 번째 본 정보 위치
        _uint iNumBones = {}; //이 애니메이션에 포함된 본의 개수
        _float fDuration = {}; //전체 재생 시간
        _float fTickPerSecond = {}; //초당 틱수

    }ANIMINFO_GPU;

    typedef struct tagAnimLocalMatrixCB
    {
        _uint   g_LiAnimIndex = {}; //현재 재생중인 애니메이션 인덱스
        _uint   g_LiNumBones = {}; //뼈 개수
        _uint   g_LiRootBoneIndex = {}; //루트본 인덱스
        _uint   g_LbAnimLoop; //루프 여부

        _float  g_LfCurrentFrame = {}; //현재 재생 시간(Tick)
        _float  g_LfLerpRatio = {}; //보간 비율
        _uint   g_LbLerping = {}; //보간 할 것인지 
        _uint   g_LbRemoveRootTranslation = {}; //루트본 이동 제거할것인지 

        //상하체 블렌딩 나누기
        _uint   g_LiUpperAnimIndex = {}; //상체 애니메이션 인덱스
        _uint   g_LbUpperAnimLoop = {}; //상체 루프 여부
        _float  g_LfUpperCurrentFrame = {}; //상체 현재 재생 시간(Tick)
        _uint   g_LbUpperRemoveRootTranslation = {}; //상체 루트본 이동 제거할 것인지

        _float  g_LbUpperBodyBlendEnalbe = {}; //상하체 블렌드 활성화 여부
        _float  g_LfUpperLayerWeight = {}; //상체 전체 블렌드 가중치 값
        _float  g_LfUpperLerpRatio = {}; //상체 보간 비율
        _uint   g_LbUpperLerping = {}; //상체 보간 할 것인지 

    }ANIMLOCALCOMPUTE_DESC;

    typedef struct tagPOSESRT_GPU //SRT 저장용도
    {
        _float3 vScale = {};
        _float fPadding0 = {};
        _float4 vRotation = {};
        _float3 vPosition = {};
        _float fPadding1 = {};

    }POSESRT_GPU;

    typedef struct tagSRT //루트모션만 CPU에서 돌리기 위해서 순수 SRT만 담는 구조체 만듬.
    {
        _float3 vScale = {};
        _float4 vRotation = {};
        _float3 vPosition = {};

    }SRT_DESC;

    typedef struct tagReadback_CB
    {
        _uint iReadbackIndices[8];

        _uint iReadbackCount;
        _uint iPadding[3];

    }READBACK_DESC;

    typedef struct tagSocketBone
    {
        _string strBoneName;            // 소켓이 붙을 타겟 뼈의 이름
        _int    iTargetBoneIndex;       // 전체 뼈 배열 내 타겟 뼈의 위치 인덱스
        vector<_int> vecHierarchy;      // Root부터 타겟 뼈까지의 계층별 뼈 인덱스 (순회 연산용)
        vector<_uint> vecCacheIndices;  // vecHierarchy와 1:1 대응하는 캐시 인덱스
        _float4x4 CombinedMatrix;       // 이번 프레임 연산이 완료된 소켓의 최종 변환 행렬

    }SOCKET_DESC;

    typedef struct tagResetModelBindPose
    {
        _wstring            wstrFilePath;
        vector<_string>    vecTargetBoneNames;

    }RESETPOSE_DESC;

    typedef struct tagDrapeBone
    {
        _int iLeftArm_BoneIndex = {};
        _int iRightArm_BoneIndex = {};
        _int LeftOuterOpne_BoneIndex = {};
        _int LeftOuterOpne1_BoneIndex = {};
        _int TempLeftDrape_BoneIndex = {};
        _int LeftOuterUp_BoneIndex = {};
        _int RightOuterOpen_BoneIndex = {};
        _int RightOuterOpne1_BoneIndex = {};
        _int TempRightDrape_BoneIndex = {};
        _int RightOuterUp_BoneIndex = {};

    }DRAPEBONE_DESC;

    typedef struct tagDrapeRule
    {
        _int iSourceBone; //LeftArm or RightArm 뼈
        _int iTargetBone; //해당 소스에 영향 받는 본들
        _uint iSourceAxis; //Source뼈에서 무슨 축을 가져올것인지 / 언리얼 기준 Z , Y가 영향울 주고 
        _uint iTargetAxis; //해동 Source뼈의 축을 어디 축에 영향을 줄것인지 / X Y Z 가 영향을 받고
        _uint iCurveId; // LeftOpenZ , RightOpenZ, LeftUp,RightUp  = 각도에 따라 Open(x축으로 벌림) Up(y축으로 위아래 들림) Id  
        _uint iAlphaSlot; //Open 과 Up을 결정하는 용도
        _float fMultipy; //값 반전용도 축 바꾸거나 

    }DRAPERULE_DESC;

    typedef struct tagDrapeDelta_GUP
    {
        _uint iTargetBone;
        _uint iPadding0, iPadding1, iPadding2;
        _float4 QuaternionDelta; //회전 변화랑

    }DRAPEDELTA_DESC;

    typedef struct tagCurveKey
    {
        _float fTime; //입력값(소스뼈의 회전값)
        _float fValue; //해당 Time을 이용한 타겟뼈의 실제 회전값

    }CURVEKEY_DESC;


	typedef struct tagPixelPicking
	{
		XMFLOAT4 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
        XMFLOAT4 vBlendWeights{};
        XMFLOAT4 vBlendIndices{};

        static const unsigned int		iNumElements = { 5 };

		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	}PSPICK;

    typedef struct tagVertexInstance
    {
        XMFLOAT4		vRight;
        XMFLOAT4		vUp;
        XMFLOAT4		vLook;
        XMFLOAT4		vTranslation;
        XMFLOAT2		vLifeTime;
    }VTXPARTICLE;

    typedef struct tagParticleGPU
    {
        XMFLOAT4		vRight;
        XMFLOAT4		vUp;
        XMFLOAT4		vLook;
        XMFLOAT4		vTranslation;
        XMFLOAT2		vLifeTime;
        XMFLOAT2        vRotation;
    }VTXPARTICLEGPU;

    typedef struct tagParticleInitialData
    {
        XMFLOAT4        vInitTranslation;
        float           fSpeed;
        float           fMaxLifeTime;
        float           fGravity;
        float           fStartDelay;
    }PARTICLE_INIT;


    //단축키등록
    struct HotKey
    {
        _ubyte				eKeyCode;
        bool				Ctrl;
        bool				Shift;
        bool				alt;
        HOTKEY_MODE			eMode = HOTKEY_MODE::DOWN; // 입력 모드
        HotKeyCallback		m_CallBack = nullptr;
    };

    //타이머에대한 어떠한 동작수행
    typedef struct tagAlarm
    {
        float Elapsed = 0.f;; //흐른시간
        float Limit = 0.f;
        bool        IsRunning = false;

        function<void()>        m_AlarmFunc = nullptr;
        void    On() { IsRunning = true; }
        void    Stop() { IsRunning = false; }
        void    Off() {IsRunning = false; Elapsed= 0.f;}
        void Update(const float fTimeDelta)
        {
            if (IsRunning)
            {
                Elapsed += fTimeDelta;
                if (Elapsed >= Limit)
                {
                    if (m_AlarmFunc)
                        m_AlarmFunc();
                }
            }
          
        }
    }Alarm;


    typedef struct tagVertexPosTexParticle
    {
        static const unsigned int		iNumElements = { 8 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},						
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

            { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "TEXCOORD", 6, DXGI_FORMAT_R32G32_FLOAT, 1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
    }VTXPOSTEX_PARTICLE;

	typedef struct tagVertexTrail
	{
		XMFLOAT3    vPosition{};        // 무기의 뿌리 / 무기의 끝
		XMFLOAT2    vTexcoord{};        // vTexcoord.x => 0 일 때 무기의 뿌리, 1 일 때 무기의 끝,    vTexcoord.y => 0 일 때 최신 점, 1일 때 오래된 점
		float       fAlpha{};

        static const unsigned int       iNumElements = { 3 };
        static constexpr D3D11_INPUT_ELEMENT_DESC        Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
    }VTXTRAIL;

    typedef struct tagVertexPosParticle
    {
        static const unsigned int		iNumElements = { 6 };

        static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},		

            { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},

            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
    }VTXPOS_PARTICLE;

    typedef struct tagVertexInstanceMesh
    {
        static const unsigned int iNumElements = { 9 };

        static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},

            { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
        };
    } VTXINSTANCEMESH;


	typedef struct tagLightDesc
	{
		LIGHT eType;
		XMFLOAT4 vDiffuse  = {};
		XMFLOAT4 vAmbient  = {};
		XMFLOAT4 vSpecular = {};

        //directional
		XMFLOAT4 vDirection = {};

        //Point
		XMFLOAT4 vPosition  = {};
		float fRange = {};

        //Spot은 추가로
        float fInnerCone;
        float fOuterCone;

        //거리비교용
        float fPlayerDistance;
        _bool bIsVisible = { true };
	}LIGHT_DESC;

    typedef struct tagShadowDesc
    {
        _float4		vPosition;
        _float4		vAt;

        _float     vOrthSize;

        _float		fFovy;
        _float		fNearZ;
        _float		fFarZ;
        _float      fDepth;
        _float4     vAddPos;
    }SHADOW_DESC;

    typedef struct tagKeyFrame {
        XMFLOAT3 vScale = {};
        XMFLOAT4 vRotation = {};
        XMFLOAT3 vPosition = {};
        float	 fTrackPosition = {};
    }KEYFRAME;

    typedef struct tagLerpContainer {
        XMFLOAT3 vScale = {};
        XMFLOAT4 vRotation = {};
        XMFLOAT3 vPosition = {};
    }LERPCONTAIN;

#pragma region 쉐이더쪽 던질것들
    //칼라그레이딩 + 뎁스포그 + PBR조명
    typedef struct tagShaderDesc
    {
        //칼라그레이딩
        XMFLOAT3 vShadowTintColor; //색
        float fShadowTintWeight; //가중치

        XMFLOAT3 vMidtoneTintColor; //색
        float fMidtoneTintWeight; //가중치

        XMFLOAT3 vHighlightTintColor; //색
        float fHighlightTintWeight; //가중치
        float fShadowRange; //범위(0~Range)
        float fHighLightRange; //범위(high~1)
        float g_fEnableColorGrading; //On/OFF
        float PaddingColorGrading;

        //뎁스포그
        XMFLOAT3 vFogColor; //색깔
        float fFogEnable; //ON/OFF

        float fFogStartDist; //포그 시작거리
        float fFogEndDist; //포그 적용거리
        float fFogDensity; //포그 빽빽함
        float fogType; //포그타입 0.5로 나뉨

        float fFogBaseHeight; //포그 가장 짙은 높이
        float fFogFadeHeight; //포그 사라질 위치
        float fHeightFogIntensity; //높이 포그 강도
        float fFogIntensity; //전체 포그 강도
        
        //PBR
        //조명
        float fLightMultiplier; //전체 조명 세기 
        float fAmbientStrength;  //앰비언트 강도
        XMFLOAT2 vToonShadowRange; //툰그림자 경계범위

        XMFLOAT2 vToonBrightnessRange;  //그림자 밝은면 최소/최대 밝기
        XMFLOAT2 vAmbientFloorMin; //최소 앰비언트 보장

        // SSS / Skin
        XMFLOAT3 vSSSColor; //SSS색상
        float fBackFillStrength; //뒷면 보정강도(이건조명용)

        XMFLOAT3 vSkinTint; //피부톤
        float fSSSPower;  //SSS투과정도

        float fSSSIntensity;  //강도
        float fSkinRoughnessMin; //피부최소 Rough

        // Rim
        float fRimPower; //림폭(낮으면 넓어짐)
        float fRimIntensity; //림 강도
        XMFLOAT2 vRimMaskRange; //림 마스크범위

        // Specular
        XMFLOAT2 vSpecBoostRange; //강한건 강하게 약한건 약하게
        float fSpecularIntensity;  //전체 스펙 세기
        float fMinDiffuse;  //디퓨즈 보장

        XMFLOAT2 vSpecBosstMapRange;
        float fSpecularMapIntensity; //맵 스펙 세기
        float fUnderMapZeroDirSpecular; //지하 directional스페큘러 없애기

        //PBR보정
        float fKValue; //함수 엄격도?
        float fMtrlRoughnessMin; //물체 최소 Rough

        XMFLOAT2 vBackFaceShaowRange; //그림자 뒷면 범위
        float fSpecSoftClamp; //옷에 반짝거리는거 제거위해서
        float fShadowBright; //그림자 최소 밝기

        XMFLOAT3 Padding123;
        float fShadowBackBright; //그림자 뒷면 최소밝기

        //진짜GodRay 조정용
        float fGodRayDensity;    // 빛줄기 길이 (높을수록 길어짐)
        float fGodRayDecay;   // 거리 감쇠 (1에 가까울수록 멀리까지)
        float fGodRayWeight;    // 샘플당 기여도
        float fGodRayExposure;    // 최종 밝기
        XMFLOAT4 vGodRayColor; // 갓레이 색
        XMFLOAT4 vGodRayPosition; //갓레이 위치

        //GodRay메쉬조정용
        float fGodRayMeshScrollSpeed; // UV 스크롤 속도
        float fGodRayMeshSwayFreq; // 좌우 흔들림 빈도
        float fGodRayMeshSwayPhase; // 높이별 위상 차이
        float fGodRayMeshSwayAmp; // 좌우 흔들림 폭

        XMFLOAT3 vGodRayMeshColor; // 빛 색상
        float fGodRayMeshContrast; // 노이즈 대비

        float fGodRayMeshIntensity; // 전체 밝기
        float fGodRayMeshEdgeFade; // 가장자리 페이드 폭
        float fSoftParticleRange; //소프트파티클(물체경계부드럽게)
        float fGodRayMeshAngleFadePower; //옆에보면 사라질거

        float fGodRayMeshFadeNear; //메쉬 가까이
        float fGodRayMeshFadeFar; //메쉬 멀리

        float fBambooEmission; //죽순 밝기(지상, 지하)
        float fUnderBambooEmission;
        float fCaveMapWet;

        float fCamMotionBlurIntensity;
        float fCamMotionBlurMaxVelocity; 
        float fCamMotionBlurSamples;
        //몬스터 스페큘러
        float fMonsterSpecularIntensity;
        XMFLOAT2 vSpecMonsterBoostRange;
        float padding;

    }ShaderDesc;

    typedef struct tagZoneDesc {
        float fExposure = {};
        float fLightMultiplier = {};
        float fAmbientStrength = {};
        XMFLOAT3 vFogColor = {};
        float vFogType = {};
        float fFogStartDist = {};
        XMFLOAT3 vShadowTint = {};
        XMFLOAT3 vMidtoneTint = {};
        XMFLOAT3 vHighlightTint = {};
        float vfUnderMapZeroDirSpecular;
        XMFLOAT2 vSpecBoostRange;
        XMFLOAT2 vSpecMapBoostRange;
        XMFLOAT2 vSpecMonsterBoostRange;
        float fBackFillStr;
        float fBambooEmissionIntensity;
        float fCaveWet;
        float fMinMtrlRough;
    }ZoneDesc;
#pragma endregion

#pragma region 피킹 구조체
    struct VERTEX_QUAD {
        _float3 pos;
        _float2 uv;
    };

    struct MyRay {
        _float3 vOrigin = {};
        _float3 vDirection = {};
    };

    struct HitResult {
        _bool bHit = false;
        _float fDistance = FLT_MAX;
        _float3 vHitPos = {};
        class GameObject* pObject = { nullptr };
    };

    typedef struct tagMousePickingDesc {
        _float fDistance{};
        _uint iMeshIndex{};
        _float3 vWorldPos{};
        class GameObject* pGameObject;
    }PICKING_DESC;
#pragma endregion

    
    typedef struct tagAnimMeshMorphTargetDesc {
        _string strName{};
        vector<_float3> vDeltaVertices;
        vector<_float3> vDeltaNormals;
        vector<_float3> vDeltaTangents;
    }MORPH;


    typedef struct font_description {
        TEXTDRAW_SETTING eSetting = TEXTDRAW_SETTING::NORMAL;
        _float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
        _float4 vOutlineColor = _float4(0.f, 0.f, 0.f, 1.f);
        _float2 vPosition = _float2();
        _float2 vSize = _float2(1.f, 1.f);
        _wstring wstrDrawText = {};
        _wstring wstrFontName = L"Font_Default";

    }FONT_DESC, TEXT_DESC, FONT_DRAW;

    struct UITransform
    {
        _float m_fX = {};
        _float m_fY = {};
        _float m_fZ = 0.f;  //월드 ui일시에 사용

        _float m_fSizeX = {};
        _float m_fSizeY = {};

        _float m_fRotationY = { };

        _float m_fRotationZ = {};

        _float m_fAlpha = { 1.f };
        _float2 m_UVOffset = { 0.f,0.f };


        ordered_json    To_Json() const 
        {
            ordered_json j;

            j["Pos"] = { m_fX,m_fY,m_fZ };
            j["Size"] = { m_fSizeX,m_fSizeY };
            j["RotationY"] = m_fRotationY;
            j["RotationZ"] = m_fRotationZ;
            j["Alpha"] = m_fAlpha;
            j["UVOffset"] = { m_UVOffset.x,m_UVOffset.y };

            return j;
        }


        void    From_Json(const ordered_json& j)
        {
            //위치로드
            if (j.contains("Pos"))
            {
                m_fX = j["Pos"][0];
                m_fY = j["Pos"][1];
                if (j["Pos"].size() >= 3)
                    m_fZ = j["Pos"][2];
                else
                    m_fZ = 0.f;

            }
           
            //스케일 로드
            if (j.contains("Size"))
            {
                m_fSizeX = j["Size"][0];
                m_fSizeY = j["Size"][1];
            }

            //단일값 로드(없으면 기본값넣도록)
            m_fRotationY = j.value("RotationY", 0.f);
            m_fRotationZ = j.value("RotationZ", 0.f);
            m_fAlpha = j.value("Alpha", 1.f);

            //UV로드
            if (j.contains("UVOffset"))
            {
                m_UVOffset.x = j["UVOffset"][0];
                m_UVOffset.y = j["UVOffset"][1];
            }

        }
    };


    /////////////UI_Anim관련////////////////
    //어떤 변환을 수행할것인가(ex) 영화의 한장면의 배우정보)
    struct UIAnimTrack
    {
        UIANIMTYPE      m_eAnimType = UIANIMTYPE::END;
        float           m_fStartTime = 0.f;     //시작시간(딜레이)
        float           m_fDuration = 1.f;      //끝나는시간
        _float3         m_vStartValue=_float3(0.f,0.f,0.f);  //시작값
        _float3         m_vEndValue=_float3(0.f,0.f,0.f);    //끝날때의 값
        EASETYPE        m_eEase=EASETYPE::LINEAR;        // 애니메이션 보간설정
        
        UIAnimTrack() {}
        UIAnimTrack(const UIAnimTrack& rhs)
            :m_eAnimType(rhs.m_eAnimType),m_fStartTime(rhs.m_fStartTime),
            m_fDuration(rhs.m_fDuration),m_vStartValue(rhs.m_vStartValue),
            m_vEndValue(rhs.m_vEndValue),m_eEase(rhs.m_eEase)
        {

        }

        ordered_json To_Json() const
        {
            ordered_json j;
            string AnimType = "";
            string EaseType = "";

            switch (m_eAnimType)
            {
            case Engine::UIANIMTYPE::SCALE:
                AnimType = "SCALE";
                break;
            case Engine::UIANIMTYPE::ROTATION:
                AnimType = "ROTATION";
                break;
            case Engine::UIANIMTYPE::POSITION:
                AnimType = "POSITION";
                break;
            case Engine::UIANIMTYPE::ALPHA:
                AnimType = "ALPHA";
                break;
            case Engine::UIANIMTYPE::UVSCROLL:
                AnimType = "UVSCROLL";
                break;

            case Engine::UIANIMTYPE::COLOR:
                AnimType = "COLOR";
                break;

            case Engine::UIANIMTYPE::PROGRESS:
                AnimType = "PROGRESS";
                break;
            default:
                break;
            }

            switch (m_eEase)
            {
            case Engine::EASETYPE::LINEAR:
                EaseType = "LINEAR";
                break;
            case Engine::EASETYPE::SMOOTH:
                EaseType = "SMOOTH";
                break;
            case Engine::EASETYPE::IMME:
                EaseType = "IMME";
                break;

            case Engine::EASETYPE::SHAKE:
                EaseType = "SHAKE";
                break;

                    //현재값에서 부터 endvalue까지 
            case Engine::EASETYPE::CURRENT:
                EaseType = "CURRENT";
                break;

            case Engine::EASETYPE::BOUNCE:
                EaseType = "BOUNCE";
                break;
            default:
                break;
            }
          

            j["AnimType"] = AnimType;
            j["StartTime"] = m_fStartTime;
            j["Duration"] = m_fDuration;
            j["StartValue"] = { m_vStartValue.x,m_vStartValue.y,m_vStartValue.z };
            j["EndValue"] = { m_vEndValue.x,m_vEndValue.y,m_vEndValue.z };
            j["EaseType"] = EaseType;
            
            return j;

        }

        void    Load_From(ordered_json& json)
        {
            string AnimType = json.value("AnimType","POSITION");
            string EaseType = json.value("EaseType","LINEAR");

            //SetAnimType
            {
                if (AnimType == "POSITION")
                    m_eAnimType = UIANIMTYPE::POSITION;

                else if (AnimType == "SCALE")
                    m_eAnimType = UIANIMTYPE::SCALE;

                else if (AnimType == "ROTATION")
                    m_eAnimType = UIANIMTYPE::ROTATION;

                else if (AnimType == "ALPHA")
                    m_eAnimType = UIANIMTYPE::ALPHA;

                else if (AnimType == "UVSCROLL")
                    m_eAnimType = UIANIMTYPE::UVSCROLL;

                else if (AnimType == "COLOR")
                    m_eAnimType = UIANIMTYPE::COLOR;

                else if (AnimType == "PROGRESS")
                    m_eAnimType = UIANIMTYPE::PROGRESS;
            }
           
            //setEaseType
            {
                if (EaseType == "LINEAR")
                    m_eEase = EASETYPE::LINEAR;


                if (EaseType == "SMOOTH")
                    m_eEase = EASETYPE::SMOOTH;

                if (EaseType == "IMME")
                    m_eEase = EASETYPE::IMME;

                if (EaseType == "SHAKE")
                    m_eEase = EASETYPE::SHAKE;


                if (EaseType == "CURRENT")
                    m_eEase = EASETYPE::CURRENT;


                if (EaseType == "BOUNCE")
                    m_eEase = EASETYPE::BOUNCE;

            }

            m_fStartTime = json.value("StartTime", 0.f);
            m_fDuration = json.value("Duration", 1.f);
            if (json.contains("StartValue") && json["StartValue"].is_array())
            {
                m_vStartValue = _float3(json["StartValue"][0], json["StartValue"][1], json["StartValue"][2]);
            }

            if (json.contains("EndValue") && json["EndValue"].is_array())
            {
                m_vEndValue = _float3(json["EndValue"][0], json["EndValue"][1], json["EndValue"][2]);
            }

    


        }

    };

 
    /////////////UI EventStruct ///////////////
    struct UI_MasterEvent
    {
        _uint           m_iObjID = 0;
        string           m_ActionName;  //전달할(실행시킬 이벤트이름)
        UI_EVENT_TARGET     m_EventTarget= UI_EVENT_TARGET::COMPONENT;
        _float          m_fValue=0.f;
        _bool           m_bFlag;
        wstring         m_Text;
        void*           m_pData;    //무엇이든지 담을수있는 데이터
        _bool           m_bPersistent = false;


        //계산을 통해서 사용해야하는함수
        size_t      m_ActionStrHash=0;


        ordered_json To_Json() const
        {
            ordered_json j;
            j["Action"] = m_ActionName;

            string strEventTarget = "";
            switch (m_EventTarget)
            {
            case Engine::UI_EVENT_TARGET::COMPONENT:
                strEventTarget = "COMPONENT";
                break;

            case Engine::UI_EVENT_TARGET::UIOBJECT:
                strEventTarget = "UIOBJECT";
                break;

            case Engine::UI_EVENT_TARGET::SYSTEM:
                strEventTarget = "SYSTEM";
                break;

            }

            j["EventTarget"] = strEventTarget;
            j["Value"] = m_fValue;
            j["Flag"] = m_bFlag;
            j["Text"] = wstringToString(m_Text);

            return j;
        }

        void    From_Json(const ordered_json& j)
        {
            m_ActionName = j.value("Action", "");
            if (j.contains("EventTarget"))
            {
                if (j["EventTarget"] == "COMPONENT")
                    m_EventTarget = UI_EVENT_TARGET::COMPONENT;
                
                else if (j["EventTarget"] == "UIOBJECT")
                    m_EventTarget = UI_EVENT_TARGET::UIOBJECT;

                else if (j["EventTarget"] == "SYSTEM")
                    m_EventTarget = UI_EVENT_TARGET::SYSTEM;

            }
            
            m_fValue = j.value("Value", 0.f);
            m_bFlag = j.value("Flag", false);
            m_Text = stringToWstring(j.value("Text", ""));


            m_ActionStrHash = hash<string>{}(m_ActionName);
        }

    };


    /////////////////UI_ActiveEvent(파싱때문에 여기로이사합니다.)////////
    struct UI_ActiveEvent       //활성화/비활성화되었을때의 간단한이벤트설정
    {
        string  strAnimClip = "";
        string  strSoundFile = "";

        ordered_json    To_Json() const
        {
            ordered_json j;
            j["AnimName"] = strAnimClip;
            j["SoundName"] = strSoundFile;

            return j;
        }


        void    From_Json(const ordered_json& j)
        {
            //위치로드
            if (j.contains("AnimName"))
                strAnimClip = j["AnimName"];

            if(j.contains("SoundName"))
                strSoundFile = j["SoundName"];

        }
    };

    struct ComponentData
    {
        string  strComType = "";//원본 컴포넌트 타입(복사시 필요함)
        ordered_json ComJson;      //각 컴포넌트가 저장할 json {"TexKey": ".."}

        ordered_json To_Json() const
        {
            ordered_json j;
            j["ComType"] = strComType;
            j["ComData"] = ComJson;
            return j;
        
        }

        void    From_Json(const ordered_json& j)
        {
            strComType = j.value("ComType", "");
            ComJson = j["ComData"];
        }
    };
    
    //Animation Event
    typedef struct Animation_Notify_Event
    {
        _float fFrame = {}; //어느 Tick (CurrentFrame == TrackPostion) 시점에 실행시킬건지,  

        _float fStartFrame = {}; //시작지점
        _float fEndFrame = {};  //끝 지점

        _bool bActive = { false };

        ANIM_NOTIFY_TYPE eNotify_Type = { ANIM_NOTIFY_TYPE::END }; //단발인지, 지속인지 ex)사운드/이펙트 
        ANIM_EVENT_TYPE eNotify_Event = { ANIM_EVENT_TYPE::END }; //무슨 이벤트 발생시킬지 = 기간 설정 ex) 공격구간 / 지속 이펙트

        UMAP<_string, VariantValue> UmapEvent; //여기에 이제 이벤트 데이터들 저장할 예정

        //뼈에 붙일 소켓이름 추가
        _string SocketName; //어디에 붙일건지 
        _bool bAttached = { false }; //뼈에 붙여서 따라가게 할건지 
        
        _uint iNotifyId = {}; //툴에서 사용할 노티파이 구분용

        class CACHED_EVENT_BASE* pCachedEvent = {};
    }ANIMNOTIFY_DESC;

    struct AnimToolData //툴용 공유해야하는 데이터들
    {
        class Model* pModelCom = { nullptr };
        class GameObject* pSelectedGameObject = { nullptr };
        class Animation* pSelectedAnimation = { nullptr };

        _uint	m_iAnimationIndex = {};

        _bool	bIsAnimPlaying = { false };
        _bool	bIsLoop = { false };
        _float	fLerpDuration = { 0.2f };
        _float	fAnimationSpeed = { 1.0f };

        _bool   bScrubbing = { false };

        _uint   m_iHoveredNotifyId = UINT_MAX;
        _uint   m_iSelectedNotifyId = UINT_MAX;

    };

    /////Bitmapfont
    struct BM_CHARACTER {
        int x, y, width, height;
        int xoffset, yoffset, xadvance;
    };

#pragma region 최적화
    // 옥트리 인스턴싱 컬링용 구조체
// 정적 오브젝트에만 적용(동적 오브젝트는 적용 X)
    struct STATIC_OBJ_INFO
    {
        class GameObject* pObject = nullptr;
        class Model* pModel = nullptr;       // 캐싱된 모델 포인터
        _uint               iModelTypeID = 0;       // 모델 ID
        RENDER_GROUP        eRenderGroup = RENDER_GROUP::END;
        BoundingBox         tAABB;                  // 절두체 컬링용 AABB
        _bool               bIsOccluder = { false }; // Occluder 캐싱
        _float              fMaxFaceArea = {};
    };

    struct FrustumPlanes
    {
        _float4 planes[6] = {};
    };

    // GPU Frustum Culling용 오브젝트 데이터 (96 bytes, 16-byte aligned)
    struct GPU_OBJECT_DATA
    {
        _float3     vCenter = {};       // AABB 중심
        _float      fMaxExtent = {};    // max(extents.x, y, z)
        _float3     vExtents = {};      // AABB 반크기
        _uint       iGroupIndex = {};   // 모델 그룹 인덱스
        _float4x4   matWorld = {};      // 월드 행렬 (_14에 ObjectID 포함)
    };

    // 모델 그룹 디스크립터 (16 bytes)
    struct GPU_GROUP_DESC
    {
        _uint iFirstObject = {};        // ObjectData 배열 내 시작 인덱스
        _uint iObjectCount = {};        // 그룹 내 오브젝트 수
        _uint iFirstArgsEntry = {};     // IndirectArgs 배열 내 시작 인덱스
        _uint iMeshCount = {};          // 모델 메시 수
    };

    // GPU Cull cbuffer (128 bytes, 16-byte aligned)
    struct alignas(16) CB_GPU_CULL
    {
        _float4 vPlanes[6] = {};       // 96 bytes
        _float4 vCamPosition = {};     // 16 bytes
        _uint   iNumObjects = {};      // 4
        _uint   iNumGroups = {};       // 4
        _float  fCullThresholdSq = {}; // 4
        _uint   iPad = {};             // 4  = 16 bytes
    };

    // DrawIndexedInstancedIndirect 인자 (20 bytes)
    struct DRAW_INDEXED_INDIRECT_ARGS
    {
        _uint IndexCountPerInstance = {};
        _uint InstanceCount = {};               // GPU가 채움
        _uint StartIndexLocation = {};
        _int  BaseVertexLocation = {};
        _uint StartInstanceLocation = {};       // GPU가 채움
    };

    // GPU 컬링 그룹 렌더링 정보 (CPU측 보관)
    struct GPU_CULL_RENDER_GROUP
    {
        class Model* pModel = { nullptr };
        _uint           iFirstArgsEntry = {};   // IndirectArgs 배열 내 시작
        _uint           iMeshCount = {};
    };
#pragma endregion


        typedef struct DebugCapsule
        {
            _float3 vCenter = {};
            _float fRadius = {};
            _float fHalfHeight = {};       // 중심에서 반구 중심까지의 거리
            _float4 qOrientation = { 0.f, 0.f, 0.f, 1.f };
            _float3 vColor = { 0.2f, 1.f, 0.2f };
        }CAPSULE_DESC, DEBUG_CAPSULE_DESC;

        struct DebugFan {
            _float3 vCenter = {};
            _float3 vLook = {};       // 정면 방향 (정규화)
            _float  fRadius = {};
            _float  fHalfAngle = {};  // 반각 (도)
            _float3 vColor = {};
        };


#pragma region PhysX
    typedef struct PhysX_Actor {
        _float4x4           matWorld = {};                          // 베이킹 할 모델의 worldmatrix(어디에 어떤 충돌체가 있을지 위치를 알아야 하니까)
        class Model*        pModel = { nullptr };                   // 베이킹 할 모델
        _float              fStaticFriction = 0.5f;                 // 정적 마찰 계수
        _float              fDynamicFriction = 0.5;                 // 동적 마찰 계수
        _float              fRestitution = 0.1f;                    // 반발 계수(탄성)
        PX_ACTOR_TYPE		eActorType = { PX_ACTOR_TYPE::END };    // 액터 타입
        _uint				iObjectID = { 0 };			            // GameObject 고유 ID
        class GameObject*   pOwner = { nullptr };		            // 소유 GameObject 포인터 (필요시 직접 접근)
    }PHYSX_ACTOR_DESC;

    typedef struct PhysX_Controller_Desc
    {
        PxExtendedVec3      vPosition = PxExtendedVec3(0.0, 0.0, 0.0);
        _float              fRadius = 0.3f;             // 캡슐 반지름
        _float              fHeight = 1.0f;             // 캡슐 높이 (반구 제외한 원통 부분)
        _float              fStepOffset = 0.3f;         // 올라갈 수 있는 계단 높이
        _float              fSlopeLimit = 50.f;       // 올라갈 수 있는 경사 (Degree값으로 넣기)
        _float              fContactOffset = 0.1f;      // 충돌 감지 여유 거리
        _float3             vMaterial = { 0.5f, 0.5f, 0.f };
        PX_ACTOR_TYPE       eActorType = { PX_ACTOR_TYPE::END };    // 액터 타입
        _uint			    iObjectID = { 0 };			            // GameObject 고유 ID
        class GameObject*   pOwner = { nullptr };		            // 소유 GameObject 포인터 (필요시 직접 접근)
    } PHYSX_CONTROLLER_DESC;

    // 액터 하나의 정보 구조체
    struct PHYSX_USER_DATA
    {
        PX_ACTOR_TYPE		eActorType = { PX_ACTOR_TYPE::END };    // 액터 타입
        _uint				iObjectID = { 0 };			            // GameObject 고유 ID
        class GameObject*   pOwner = { nullptr };		            // 소유 GameObject 포인터 (필요시 직접 접근)
    };

    typedef struct PhysX_RaycastResult
    {
        _bool           bHit = { false };           // 충돌 여부
        _float3         vPosition = {};             // 맞은 위치
        _float3         vNormal = {};               // 맞은 면의 법선
        _float          fDistance = {};             // 레이 시작점에서 맞은 지점까지 거리
        PxRigidActor*   pActor = { nullptr };       // 충돌한 액터
        PHYSX_USER_DATA* pUserData = { nullptr };   // 액터 식별정보
    } PHYSX_RAYCAST_RESULT;

    typedef struct PhysX_SweepResult
    {
        _bool               bHit = { false };           // 충돌 여부
        _float3             vPosition = {};             // 맞은 위치
        _float3             vNormal = {};               // 맞은 면의 법선
        _float              fDistance = {};             // 스윕 시작점에서 충돌 지점까지 거리
        PxRigidActor*       pActor = { nullptr };       // 충돌한 액터
        PHYSX_USER_DATA*    pUserData = { nullptr };    // 액터 식별정보
    } PHYSX_SWEEP_RESULT;

    typedef struct PhysXMultiSweepResult // 다중 스윕 히트 결과
    {
        _bool                       bHit = { false };   // 충돌 여부
        vector<PHYSX_USER_DATA*>    vecUserData = {};   // 충돌한 액터 데이터
        vector<_float3>             vecPositions = {};  // 맞은 위치
        vector<_float>              vecDistances = {};  // 스윕 시작점에서 충돌 지점까지 거리
        vector<_float3>             vecNormals = {};    // 충돌의 법선 방향
    } PHYSX_MULTI_SWEEP_RESULT;

    typedef struct PhysX_OverlapResult
    {
        _bool                       bHit = { false };   // 겹침 여부
        vector<PxRigidActor*>       vecActors;          // 겹친 액터 목록
        vector<PHYSX_USER_DATA*>    vecUserData;        // 각 액터의 식별 정보
        vector<_float3>             vecHitData;
    } PHYSX_OVERLAP_RESULT;


    typedef struct PHYSX_BONE_CHAIN_DESC
    {
        _uint iChainID = {};
        vector<_string> vecBoneNames = {}; // 체인에 포함될 본 이름 목록 (루트부터 끝까지 순서대로)
        _float fBoneRadius = { 0.05f };    // 각 본 actor의 충돌 반지름
        _float fSwingLimit = { 40.f };     // swing 제한 각도 (도)
        _float fTwistLimit = { 15.f };     // twist 제한 각도 (도)
        _float fDamping = { 0.3f };        // 감쇠 계수
        _float fStiffness = { 0.1f };      // 원래 포즈로 복원하는 힘
        _float fDensity = { 1.0f };        // 액터 밀도
        vector<_float4x4>	vecInitialPoses = {}; // 초기 Position 저장(RestPose)
        vector<_int> vecParentIndices = {}; // 각 본의 체인 내 부모 인덱스 (-1이면 루트)

    }BONE_CHAIN_DESC;

    typedef struct PHYSX_BONE_CHAIN_ACTOR
    {
        _string strBoneName = {};       // 본 이름
        _int iBoneIndex = {};           // MasterRig에서의 본 인덱스
        PxRigidDynamic* pActor = {};    // PhysX actor정보
        _bool bKinematic = {};          // 루트 본 여부
    }BONE_CHAIN_ACTOR;

    struct PHYSX_BONE_CHAIN
    {
        _uint iChainID = {};                        // 본 체인의 첫 번째 본 ID
        vector<BONE_CHAIN_ACTOR> vecActors = {};    // 본 체인마다의 액터
        vector<PxD6Joint*> vecJoints = {};          // 쪼인트
        PxMaterial* pMaterial = {};                 // 액터의 마테리얼
        PxTransform prevKinematicPose = PxTransform(PxIdentity);    // 이 전 프레임의 위치
    };

    // 체인 정보
    struct CHAIN_BONE_INFO
    {
        _string		strBoneName = {};
        _int		iMasterBoneIndex = {};
        _int		iChainParentIndex = {};		// 체인 내 부모 인덱스 (-1이면 체인 루트의 부모 Socket 사용)
        _float4x4	matRestPoseLocal = {};
    };

    struct CHAIN_BONE_DESC
    {
        _string					strParentBoneName = {};		// 체인 루트의 부모 본 이름
        //const _float4x4* pParentSocketMatrix = {};	// 부모 본의 Socket Combined 포인터
        _int iParentSocketNum = { -1 };
        vector<CHAIN_BONE_INFO>	vecBones = {};				// 체인 본 정보 (순서대로)
    };

    struct PHYSX_CHAIN_INFO
    {
        _uint		iChainID = {};				// PhysX BoneChain ID
        _string		strKinematicBoneName = {};	// Kinematic 루트 본 이름 (Socket으로 추종)
        //const _float4x4* pKinematicSocketMatrix = {};	// 해당 본의 Socket Combined 포인터
        _int iKinematicSocketNum = { -1 };
    };

#pragma endregion PhysX
    typedef struct tagPlayerAnimRequest
    {
        WEAPON_ANIM_ACTION eWeaponAction; //무기별 애니메이션
        _int iCombo; //무기 콤보 어택용 
        INPUT_DIR   eDir;
        //추후 다른 구조체 추가해서 요청해서 무기별 애니메이션 정하기

    }ANIMREQUEST_DESC;

#pragma region ObjectPooling
    /* 풀 버킷: 하나의 프로토타입에 대한 오브젝트 풀 */
    struct PoolBucket
    {
        /* Clone된 오브젝트 배열 */
        vector<class GameObject*>  vecSlots;

        /* 사용 가능한 슬롯 인덱스 (FIFO: 골고루 재사용) */
        queue<_uint>         queueFree;

        /* 활성화된 순서 추적 (RECYCLE_OLDEST용) */
        deque<_uint>         dequeActiveOrder;

        /* Pre-warm 시 필요한 정보 (추가 Clone용) */
        _uint                iPrototypeLevel = {};  // 프로토타입이 생성된 레벨
        wstring              strPrototypeTag;       // 프로토타입 태그 (Clone 시 사용)
        _uint                iLevelID = {};         // 오브젝트가 배치될 레벨 ID
        wstring              strLayerName;          // 오브젝트가 들어갈 레이어 이름

        /* DESC 복사본 (추가 Clone 시 사용) */
        void* pArgTemplate = { nullptr };           // DESC 복사본 (Grow 시 Clone에 넘길 초기화 데이터)
        _uint                iArgSize = {};         // DESC 크기 (memcpy용)

        /* 포화 정책 */
        POOL_POLICY          ePolicy = { POOL_POLICY::RECYCLE_OLDEST };

        /* 배치 확장 크기 */
        _uint                iGrowBatchSize = { 5 };

        /* 등록 여부 */
        _bool                bRegistered = { false };
        POOL_ID              ePoolID = { POOL_ID::END };

        /* --- 통계 --- */
        _uint                iActiveCount = {};     // 현재 활성 오브젝트 수
        _uint                iHighWatermark = {};   // 최대 동시 활성 수 (Pre-warm 수량 튜닝 기준)

        /* ParticleSystem용 */
        class ParticleSystem* pSystem = { nullptr };
    };

#pragma endregion ObjectPooling



#pragma warning(push)
#pragma warning(disable: 4251)
    ///UI-constantBuffer구조체
    struct ENGINE_DLL alignas(16) UI_BUFFER_PACKET
    {
        _float4 g_UIColor = { 1.f,1.f,1.f,1.f };

        _float2 g_UVScale = { 1.f,1.f };
        _float2 g_UVOffSet = { 0.f,0.f };

        _float2 g_UVScroll = { 0.f,0.f };
        float g_AlphaOffSet = 0.f;
        float g_Reserved = 0.f;

        _float2 g_TextureSize = _float2(1.f, 1.f);
        float g_OutLineThickness = 0.f;
        float g_UIProgress = 1.f;

        _float4 g_OutLineColor = _float4(1.f, 1.f, 1.f, 1.f);

        float g_ScrollTime = 0.f;
        float g_fClipX=10000.f;
        _float2 g_UIPadding;

        float g_fUIValue; //기냥 던질값. 아무거나사용가능
        _float2 g_fClip;
        float g_fUIIntensity = 1.f;


        
    };
}
#pragma warning(pop)









#endif // Engine_Struct_h__
