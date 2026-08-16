#pragma once

#include "Component.h"

NS_BEGIN(Engine)

struct AnimFileInfo;
class StructuredBuffer;

class ENGINE_DLL Model final : public Component
{
public:
    struct MorphPreset
    {
        _string presetName{};
        unordered_map<_string, _float> morphs;
    };

private:
	explicit Model();
	explicit Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Model(const Model& original);
	virtual ~Model();

public:
	HRESULT Initialize_Prototype(MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix, MODELROLE _RoleType = MODELROLE::STANDALONE,_bool bNoneAnimFlag = false);
	HRESULT Initialize(void* arg);
	HRESULT Render(const _float fTimeDelta, const _uint _meshIndex);
	
private:
#pragma region Pre_Load(FBX)
    HRESULT Initialize_FBX(const _wstring& _filepath);
	HRESULT Ready_Bones_FBX(const aiNode* _aiNode, _int iParentIndex, myNode* _mynode);
	HRESULT Ready_Meshes_FBX(myModel& _mymodel);
	HRESULT Ready_Materials_FBX(const _wstring& _filepath, myModel& _mymodel);
	HRESULT Ready_Animations_FBX(myModel& _mymodel);
    HRESULT Add_Animations_And_Export_Binary(const _wstring& _animFolderPath);
    void Collect_AnimFiles_Recursive(const _wstring& _folderPath, vector<AnimFileInfo>& _outFiles);
#pragma endregion Pre_Load(FBX)
        

#pragma region Fast_Load(Binary)
    HRESULT Initialize_Binary(const _wstring& _filepath);
    HRESULT Ready_Bones_Binary(myNode* _mynode);
    HRESULT Ready_Meshes_Binary(myModel* _mymodel);
    HRESULT Ready_Materials_Binary(myModel* _mymodel, const _wstring& _modelFilePath);
    HRESULT Ready_Animations_Binary(myModel* _mymodel);
#pragma endregion Fast_Load(Binary)



public:
#pragma region 쉐이더 바인딩
    [[deprecated("이 함수는 레거시 함수입니다 Bind_Material_FullSlot 함수를 사용해주십시오.")]]
    HRESULT Bind_Material(class Shader* _shader, const _string& _constantname, _uint _meshindex, aiTextureType _texturetype, _uint _textureindex = 0, _uint* bitFlag = nullptr);
    HRESULT Bind_Material_FullSlot(class Shader* _shader, _uint _slotNum, _uint _meshIndex, aiTextureType _textureType, _uint _textureIndex = 0, _uint* bitFlag = nullptr);
    HRESULT Bind_Bones(class Shader* _shader, const _string& _constantname, _uint _meshIndex);
    HRESULT Bind_BonesMasterRig(class Shader* _shader, const _string& _constantname, _uint _meshIndex,const vector<class Bone*>& _MasterRigBones, const vector<_uint>& _RemapBone);
    HRESULT Bind_Morphs(Shader* _shader, _uint _meshIndex);
    HRESULT Update_MorphWeight(const _string& _morphName, _float _weight, _uint _meshIndex);
    HRESULT Bind_CombiendAndOffsetSRV(Shader* _shader);
    HRESULT Bind_ReampSRV(Shader* pShader, _uint _meshIndex);

#pragma endregion 쉐이더 바인딩


#pragma region Get_Infos
    inline _uint Get_NumMeshes() const { return m_iNumMeshes; }
    _int Get_BoneIndex(const _char* _name);
    vector<class Mesh*>& Get_Meshes() { return m_vecMeshes; }
    vector<class Bone*>& Get_Bones() { return m_vecBones; }
    
    HRESULT Update_SkinnedMeshes();
    const _float4x4* Get_BoneMatrixPtr(const _string& _boneName);
    class Bone* Get_Bone_ByName(const _string& _boneName);
    class Bone* Get_Bone_ByIndex(_int iBoneIndex);

    inline const _wstring& Get_FilePath() const { return m_wstrFilePath; }
    BoundingBox Get_LocalAABB();

    _vector Get_RootMotionDelta()const { return m_vRootMotionDelta; }
    _vector Get_RootMotionRotationDelta() { return m_qRootMotionRotationDelta; }

    _float4x4 Get_PreTransformMatrix() { return m_PreTransformMatrix; }
#pragma endregion Get_Infos


#pragma region Animations
    _bool Is_AnimFinished() const { return m_bIsAnimationFinished; }
    _bool Get_UpperBlendEnable() const { return m_bUpperBlendEnable; }
    _bool Is_UpperAnimFinished() const { return m_bUpperIsAnimationFinished; }
    HRESULT Play_Animation(const _float fTimeDelta, const _bool _forceLoop = false); //승우가 사용하던 Play_Animation
    void Update_CombinedMatrices_Weapon(_matrix _socketMatrix); // 무기 CombinedMatrix만들기용

    void Play_Animation_CS(const _float fTimeDelta);
    void Play_Animation_New(const _float fTimeDelta); 
    void Play_Animation_AnimationTool(const _float fTimeDelta); //툴용

    void LerpAnimation(const _float fTimeDelta);

    void Set_Animation(_uint _animationIndex, _bool _isLoop);
    void Set_Animation(_uint _animationIndex, _float _lerpDuration = 0.2f);
    void Set_Animation_Siho(_uint _animationIndex, _bool _isLoop = false, _float fLerpDuration = 0.2f , _float fAnimationSpeed = 1.f);
    void Set_Animation_CS(_uint _animationIndex, _bool _isLoop = false, _float fLerpDuration = 0.2f, _float fAnimationSpeed = 1.f);
    void Set_AnimationUpper_CS(_uint iUpperAnimationIndex, _bool bUpperBlendEnable, _float fUpperLayerWeight =1.f, _bool _isLoop = false, _float fLerpDuration = 0.2f, _float fAnimationSpeed=1.f); // 상체 애니메이션 블렌드용
    void Set_NextAnimation(_uint _animationIndex, _float _nextLerpDuration);
    void Set_Animation_Speed(const _float _speed); //모든 애니메이션 스피드 제어 ?
    void Set_SingleAnimation_Speed(_float fSpeed); //모델이 가지고 있는 애니메이션 스피드 제어

    void Set_OwnerId(_uint iObjectId) { m_iOwnerId = iObjectId; }
    void Set_RootBoneIndex(_int iRootBoneIndex) { m_iRootBoneIndex = iRootBoneIndex; }
    _int Get_RootBoneIndex() const { return m_iRootBoneIndex; }

    vector<class Animation*>* Get_AnimationComs() {
        if (!m_vecAnimations.empty()) return &m_vecAnimations;
        else return nullptr;
    }
    class Animation* Get_CurrentAnimationCom() {
        if (!m_vecAnimations.empty()) return m_vecAnimations[m_iCurrentAnimationIndex];
        else return nullptr;
    }
    _uint Get_CurrentAnimationIndex() { return m_iCurrentAnimationIndex; }
    _uint Get_CurrentUpperAnimationIndex() { return m_iUpperCurrentAnimationIndex; }
    _uint Get_NumAnimations() const { return m_iNumAnimations; }
    _float Get_Animation_Speed();
    _float Get_SingleAnimation_Speed() const { return m_fAnimationSpeed; }
    _bool Is_Loop_Anim() { return m_bIsLoopAnimation; }
#pragma endregion


#pragma region MorphAnim
    HRESULT Register_StructuredBuffer(class Shader* _shader);
    HRESULT Initialize_UnifiedMorphs();
    // 모프 하나에만 적용하기
    HRESULT Update_UnifiedMorphWeight(const _string& _morphName, _float _weight);
    const vector<_string>& Get_UnifiedMorphNames() const { return m_vecUnifiedMorphNames; }
    unordered_map<_string, MorphPreset>& Get_MorphPreset() { return m_umapMorphPresets; }
    // 모프 프리셋에 적용하기
    HRESULT Apply_MorphPreset(const _string& _presetName, _float _intensity = 1.f);
    HRESULT Blend_MorphPreset(const _string& _presetName1, const _string& _presetName2, _float _blend);
    // 다중 프리셋 동시 적용
    HRESULT Apply_MorphPresets_Additive(const vector<pair<_string, _float>>& _presets);
    HRESULT Start_MorphLerp(const _string& _targetPresetName, _float _duration);
    void Update_MorphLerp(_float _timeDelta);
    void Clear_MorphPreset();
#pragma endregion


#pragma region ShaderPass
    HRESULT ShaderPass_Auto_Selection(vector<_int>* PassVec);
#pragma endregion

#pragma region Instancing
    HRESULT Render_Instancing(class Shader* pShader, const vector<_float4x4>& MatrixData);
    _uint Get_ModelID() { return m_iModelID; }
#pragma endregion

#pragma region Animation ComputeSahder
    HRESULT Initialize_UmapMasterBone();
    HRESULT Initialize_BoneIndexByName();
    HRESULT RegisterPartModelCom(Model* pPartModelCom); //파츠 모델 등록 
    HRESULT UnRegisterPartModelCom(Model* pPartModelCom);//파츠 모델 해제 / 장비 교체 대비
    void    Update_MasterOffset(); //파츠교체 대비 매 프레임 확인해야하는 오프셋 업데이트용
    HRESULT Build_MastrOffset(); //파츠교체(Update) 또는 초기화시점에 실행해야하는 오프셋 행렬만들기 
    HRESULT Build_StandAloneOffset(); //통모델 오프셋 

    HRESULT Ready_BoneCS_Buffers(); //버퍼 생성 
    HRESULT Build_BoneHierarchy(); //Parent/Depth 채우는 함수

    HRESULT Update_LocalMatricesToGPU(); //LocalMatricesBuffer 갱신
    HRESULT Update_DispatchCombinedByDepth(); //depth별 dispatch 로 Combined 완성
    HRESULT Update_OffsetToGPU(); //파츠 교체시 플래그 true되면 GPU로 업데이트
    HRESULT Ready_ReampSRV(Model* pMasterModel, vector<_uint>& vecRemapBone);
    HRESULT Ready_StandAloneRemapSRV();

    //CS + 보간작업
    HRESULT Ready_BindPose_Buffer();
    HRESULT Ready_UpperBodyMaskWeight(); //상하체 블렌드용 가중치
    HRESULT Ready_AnimCS_Buffers(); 
    HRESULT Build_AnimGPU();
    HRESULT Update_DispatchLocalPose(); //기존 LocalMatrix만들어주는 작업 컴퓨트 셰이더로
    HRESULT Copy_CurrentPoseToFrozenPos(); //애니메이션 보간 필요할때 포즈 저장 
    void    Reset_CurrentKeyIndex();
    void    Reset_UpperCurrentKeyIndex(); //상하체 블렌드용 키인덱스 리셋

    void    Set_UseComputeSKinning(_bool bUse) { m_bUseComputeSKinning = bUse; }

    //루트애니메이션 함수 분리
    void    Ready_NextRootMotion();
    void    Update_RootMotion_CS(_float fPrevFrame, _float fCurrentFrame);

    _vector Get_RootParentRotation();

    //GPU버퍼 읽어오기 위한 버퍼준비
    HRESULT Ready_Readback_Buffer();
    HRESULT Register_ReadbackBoneName(const _string& BoneName , _uint iIndexNumber); // 플레이어 마스터모델 0번 사용중, 무기(바요네트) 0번도 사용중
    HRESULT Unregister_ReadbackBoneName(_uint _indexNumber);
    void    Update_DispatchReadback();
    const _float4x4* Get_ReadbackBoneMatrixPtr(const _string& _boneName);
    _float4x4* Get_ReadbackBoneMatrixPtr_Nonconst(const _string& _boneName);

    //소켓
    HRESULT Register_SocketBoneName(const _string& BoneName);
    void    Update_Socket();
    void    Update_Socket_Simple();
    void    Evaluate_Pose(class Animation* pCurrentAnim, _float fCurrentFrame, _bool bCurrentLoop, class Animation* pPrevAnim, _float fPrevFrame, _bool bPrevLoop,
        _bool bIsLerping, _float fLerpRatio, _int iBoneIndex, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos);

    [[deprecated("이 함수는 최적화된 함수가 있습니다 Get_SocketBoneMatrixPtr_Index 함수를 사용해주세요.")]]
    const _float4x4* Get_SocketBoneMatrixPtr(const _string& _boneName);
    const _float4x4* Get_SocketBoneMatrixPtr_Index(const _string& _boneName);
    _int             Get_SocketBoneIndex(const _string& _boneName);
    const _float4x4* Get_SocketBoneMatrixPtr(_int _socketIndex);
    _uint   Is_RemoveRootTranslation() { return  m_tAnimLocalCompute.g_LbRemoveRootTranslation; }
    
    //상체블렌드 함수
    void Clear_AnimationUpper();
    void Request_ClearAnimationUpper(_float fFadeOutTime);
    void Update_UpperAnimFadeOut(_float fTimeDelta);    // 상체 애니메이션은 부드럽게 페이드 아웃
    void Sync_BaseToUpperAnim(); //상체 애니메이션 프레임을 베이스(전신) 프레임으로 동기화
    void Set_UpperAnimFinished(_bool bFinished) { m_bUpperIsAnimationFinished = bFinished; }

    //망토
    HRESULT    Ready_DrapeBoneIndex();//뼈 인덱스 저장하기
    HRESULT    Ready_DrapeRule(); //데이터 테이블화
    HRESULT    Ready_DrapeBuffer(); //버퍼 준비 
    void       Update_DrapeDelta(); //회전값 계산
    HRESULT    Update_DispatchDrivenBone();

    _float     EvaluateCurve(const vector<CURVEKEY_DESC>& CurveKeys, _float fDegree);//각도 통해서 값 선형으로 구하기
    _uint      UEAxisToDxAxis(_uint iUEAix); //언리얼 축 Dx로 바꾸기 
    _uint      UE_SourceAix_ToDX(_uint iSourceAxis); //소스 적용 축 변환 
    _float     Get_AxisDegree(_vector vDegreeXYZ, _uint iAxis); //축별 저장한 각도 가져오기 
#pragma endregion


#pragma region Socket
private:
    // 소켓 본 캐싱 (소켓에 사용되는 고유 본 수만큼만 할당)
    vector<_float4x4>  m_vecSocketCombinedCache;        // 매 프레임 캐싱된 소켓 컴바인드 매트릭스(소켓이 여러개일경우 얘 사용) 공통 부모 뼈의 중복 계산을 막기
    vector<_uint>      m_vecSocketCacheFrame;           // 글로벌 캐시에 저장된 각 뼈가 몇 번째 카운터에 계산되었는지 기록
    _uint              m_iSocketCacheCounter = {};      // 매 프레임 증가하며, 기존 캐시가 이번 프레임의 것인지 판별하는 식별자
    _bool              m_bSocketCacheDirty = true;      // 더티 플래그(업데이트 해야 하는 행렬인지) 소켓 구조 변경 시 인덱스 리빌드를 요청하는 플래그

    vector<pair<_int, _float4x4>>  m_vecSortScratch = {};    // Batched 업데이트 시 인덱스 정렬용 임시 버퍼 (매 프레임 재사용하여 힙 할당 방지)
    vector<_float4x4>              m_vecBatchScratch = {};    // 연속 구간의 행렬만 packed 복사하여 UpdateSubresource에 넘기는 임시 버퍼

    void Rebuild_SocketCache();

public:
    // 키프레임 데이터 덮어씌우기
    HRESULT Override_KeyFrames_FromFemaleBindPose();
    HRESULT Override_KeyFrames_FromBindPose(const RESETPOSE_DESC& tResetPoseDesc);
    HRESULT Overwrite_CombinedMatrices(const vector<pair<_int, _float4x4>>& _vecBoneMatrices);  // Update에서 완료된 본 매트릭스에 덮어쓰기
    HRESULT Overwrite_CombinedMatrices_Batched(const vector<pair<_int, _float4x4>>& _vecBoneMatrices);  // Update에서 완료된 본 매트릭스에 덮어쓰기(한번에)
#pragma endregion Socket

private:
	Assimp::Importer	m_Importer = {};
	const aiScene*		m_pAIScene = { nullptr };

private:
	MODEL m_eModelType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};

	_uint m_iNumMeshes = {};
	vector<class Mesh*> m_vecMeshes = {};

	_uint m_iNumMaterials = {};
	vector<class Material*> m_vecMaterials = {};

	_uint m_iNumBones = {};
	vector<class Bone*> m_vecBones;

	_uint m_iNumAnimations = {};
	_uint m_iCurrentAnimationIndex = {};
    _uint m_iPrevAnimationIndex = {};
    _int m_iNextAnimationIndex = { -1 };
	vector<class Animation*> m_vecAnimations;
	_bool m_bIsLoopAnimation = {};
	_bool m_bIsAnimationFinished = {};
    _bool m_bIsLerping = {};
    _bool m_bStackNextAnim = {};
    _float m_fLerpDuration = { 0.2f };
    _float m_fLerpTimer = { 0.f };
    _float m_fNextLerpDuration = {};
    vector<LERPCONTAIN> m_vecFirstBoneInfo = {};
    vector<LERPCONTAIN> m_vecPrevBoneInfo = {};
    vector<LERPCONTAIN> m_vecNextBoneInfo = {};

    // 본 인덱스 저장용
    UMAP<_string, _uint> m_umapBoneIndexByName;

    // 모프용
    vector<_string> m_vecUnifiedMorphNames;
    vector<_float> m_vecUnifiedMorphWeights;
    unordered_map<_string, _uint> m_umapUnifiedMorphNameToIndex;
    unordered_map<_string, MorphPreset> m_umapMorphPresets;

    // 모프 보간용
    _bool                       m_bIsMorphLerping = { false };
    _float                      m_fMorphLerpTimer = {};
    _float                      m_fMorphLerpDuration = {};
    vector<_float>              m_vecPrevMorphWeights = {};
    vector<_float>              m_vecTargetMorphWeights = {};

    _wstring m_wstrprevName = {};

    //루트 애니메이션용 변수
    vector<_float4x4>				m_vecPrevBoneTransforms;            //이거 LERPCONTAIN 자료형 안맞아서 만듬 이전뼈행렬 저장용
    _float							m_fPrevTrackPosition = {};			// 이전 프레임의 애니메이션 재생 위치
    _vector							m_vPrevRootPosition = {};			// 이전 프레임의 루트 본 실제 위치
    _vector							m_vPrevAnimPosition = {};			// 이전 애니메이션의 마지막 프레임 위치
    _vector							m_vRootMotionDelta = {};			// 현재 프레임의 이동량
    _vector                         m_qRootMotionRotationDelta = XMQuaternionIdentity();    // 현재 프레임의 회전량
    _vector                         m_qPrevRootRotation = XMQuaternionIdentity();   // 이전 프레임의 회전량
    _vector							m_vAccumulatedMotionDelta = {};		// 애니메이션 반복 시 누적된 이동량
    _int							m_iRootBoneIndex = { -1 };			// 루트 본의 인덱스

    _vector                         m_vRootStartPos = {}; //루트 시작 위치 저장
    _vector                         m_vRootEndPos = {};  // 루트 마지막 위치 저장
    _vector                         m_vRootLastPosFromPrevAnim = {}; //이전 애니메이션의 루트 마지막 위치 저장 , 보간용
    _bool                           m_bSkipRootDelta = { false };
    // 파싱용 변수
    _wstring m_wstrFilePath = L"";

    //애니메이션 제어 + 이벤트 노티파이용
    _float      m_fAnimationSpeed = { 1.f }; //애니메이션 재생속도 디폴트(1.f)로 사용하면서, 인자로 넘기려고 만듬 
    _float      m_fPrevFrame = {0.f};
    _float      m_fCurrentFrame = {0.f};
    _uint       m_iOwnerId = {}; //이거 오브젝트 m_iObjectID를 넘겨줄 예정
    
    //상하체 블렌드 전용 상체 변수들 
    _uint       m_iUpperCurrentAnimationIndex = {}; //Upper용 현재재생중인 애니메이션 인덱스
    _bool       m_bUpperIsAnimationFinished = {}; // Upper용 현재 애내미이션 끝났는지
    _bool       m_bUpperIsLoopAnimation = {}; //Upper용 루프 할것인지
    _bool       m_bUpperBlendEnable = {}; //상체 블렌드 사용할것인지
    _float      m_fUpperLayerWeight = {}; //상체전체 블렌드 가중치 얼마나 섞을것인지
    _float      m_fUpperAnimationSpeed = { 1.f }; //애니메이션 재생속도 디폴트(1.f)로 사용하면서, 인자로 넘기려고 만듬 
    _float      m_fUpperPrevFrame = { 0.f };
    _float      m_fUpperCurrentFrame = { 0.f };
    _uint       m_iUpperPrevAnimationIndex = {};
    _bool       m_bUpperIsLerping = { false };
    _float      m_fUpperLerpDuration = { 0.f };
    _float      m_fUpperLerpTimer = { 0.f };
    _bool       m_bUpperFadeOut = { false }; //페이드 아웃 할 것인지 (가중치 내리는 용도)
    _float      m_fUpperFadeOutTime = { 0.f }; // 페이드 아웃 시간
    _float      m_fUpperFadeOutAccTime = { 0.f }; //누적 시간

    //애니메이션 컴퓨트 셰이더용
    MODELROLE               m_eModelRoleType = { MODELROLE::END };
    UMAP<_string, _uint>    m_umapMasterBoneIndexByName; //각 파츠별 offsetmatrix를 마스터본을 기준으로 통합하기 위해서 이름과 인덱스 저장
    vector<_float4x4>       m_vecBoneOffsetMatrix; //위에서 저장한 이름과 인덱스로 파츠모델들의 메쉬 순회하면서 offsetmatrix를 저장하기 위함 + 통모델들도
    vector<Model*>          m_vecPartModesl; //파츠모델들의 offsetmatrix를 순회하기 위해서 
    _bool                   m_bMasterOffsetDirty = { true }; //마스터 오프셋 변경됐는지 플래그용
    vector<_bool>           m_vecOffsetFilled; //offset이 같을때 첫번째 들어온걸 기준으로 사용하기 위해서 / 추후 문제가 생기면 파츠 부위에 따른 우선순위로 바꿔야할수도
    vector<BONEINFO_GPU>    m_vecBoneHierarchy; //로딩,초기화시 한번 CS에 넘길 본 계층 구조 데이터
    _uint                   m_iMaxBoneDepth = {}; //ComputeShader를 총 몇번 Dispatch 해야하는지 결정하는 개수
    vector<_float4x4>       m_vecInputLocalMatrices; //매프레임 채널 보간을 통해서 계산된 뼈의 로컬행렬을 저장
    vector<_float>          m_vecUpperMaskWeight; //상하체 블렌드용 가중치 적용
 
    //CS + 보간작업
    vector< KEYFRAME_GPU>       m_vecKeyFramesGPU;
    vector< CHANNELINFO_GPU>    m_vecChannelGPU;
    vector< ANIMINFO_GPU>       m_vecAnimGPU; //정확하게 CPU의 m_vecAnimations[i]처럼 애니메이션 객체로 GPU에서 접근이 안되므로 GPU가 이해 가능한 형태로 정의

    //망토 작업
    DRAPEBONE_DESC                  m_tDrapeBones; //망토뼈 인덱스저장용
    vector<DRAPERULE_DESC>          m_vecDrapeRules; //망토 초기화값
    vector<DRAPEDELTA_DESC>         m_vecDrapeDeltas; //망토 변화량
    _float4                         m_vLeftArmRotation = {}; //소켓에서 해당 뼈 회전 저장용 
    _float4                         m_vRightArmRotation = {};
    vector<vector<CURVEKEY_DESC>>   m_vecCurveKey; //키값 저장
    _float                          m_fDrapeOpenControl = {1.f}; //x축 컨트롤
    _float                          m_fDrapeUpControl = {1.f}; //y축 컨트롤 
    class ComputeShader*            m_pCShaderDrivenBone = { nullptr }; //컴셰
    _bool                           m_bUseDrapeDelta = { false };
    //ID3D11Buffer*               m_pLocalMatricesBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pLocalMatricesSRV = { nullptr };
    //ID3D11UnorderedAccessView*  m_pLocalMatricesUAV = { nullptr };

    //ID3D11Buffer*               m_pHierarchyBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pHierarchySRV = { nullptr };

    //ID3D11Buffer*               m_pCombinedMatrixBuffer = { nullptr };
    //ID3D11UnorderedAccessView*  m_pCombinedMatrixUAV = { nullptr };
    //ID3D11ShaderResourceView*   m_pCombiendMatrixSRV = { nullptr };

    ID3D11Buffer*               m_pOffsetBuffer = { nullptr };
    ID3D11ShaderResourceView*   m_pOffsetSRV = { nullptr };

    //ID3D11Buffer*               m_pKeyFrameBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pKeyFrameSRV = { nullptr };

    //ID3D11Buffer*               m_pChannelBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pChannelSRV = { nullptr };

    //ID3D11Buffer*               m_pAnimInfoBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pAnimInfoSRV = { nullptr };

    //ID3D11Buffer*               m_pCurrentKeyIndexBuffer = { nullptr }; //기존_frameIndex 를 대체하는용도
    //ID3D11ShaderResourceView*   m_pCurrentKeyIndexSRV = { nullptr };
    //ID3D11UnorderedAccessView*  m_pCurrentKeyIndexURV = { nullptr };

    //애니메이션 보간용으로 애니 전환 보간에서 CPU가 하던 이전 행렬보관 하던걸 이제 버퍼로 보관
    //전환 시작 순간에 한번만 캡쳐하고, 보간 끝날 때까지 읽기용으로 사용
    //ID3D11Buffer*               m_pFrozenSRTBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pFrozenSRTSRV = { nullptr };

    //로컬 SRT저장용 
    //ID3D11Buffer*               m_pLocalSRTBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pLocalSRTSRV = { nullptr };
    //ID3D11UnorderedAccessView*  m_pLocalSRTUAV = { nullptr };

    //뼈 없을때 원래 바인드 포즈의 SRT를 적용 
    //ID3D11Buffer*               m_pBindPoseSRTBuffer = { nullptr };
    //ID3D11ShaderResourceView*   m_pBindPoseSRTSRV = { nullptr };

    class ComputeShader*        m_pCShader = { nullptr }; 
    BONECOMPUTE_DESC            m_tBoneCompute = {};

    class ComputeShader*        m_pCShaderLocalMatrix = { nullptr };

    _bool                       m_bUseComputeSKinning = { true }; //GPU 스키닝 사용 여부 디버깅용
    _bool                       m_bComputeBufferReady = { false }; //연산용 GPU 리소스 모두 유효한지

    ANIMLOCALCOMPUTE_DESC       m_tAnimLocalCompute = {};
    _bool                       m_bFirstSetAnimation = { false };

    //GPU 버퍼 읽어와서 원하는 뼈의 Combined행렬 가져오기 위해 
    ID3D11Buffer*               m_pReadbackOutBuffer = { nullptr };
    ID3D11UnorderedAccessView*  m_pReadbackOutUAV = { nullptr };

    class ComputeShader*        m_pCShaderReadback = { nullptr };
    ID3D11Buffer*               m_pReadbackStagingBuffer = {nullptr };

    READBACK_DESC               m_tReadback;
    _float4x4                   m_ReadbackCombinedMatrix[g_iReadbackBone] = {}; //현재는 1개만 받아올 예정,g_iReadbackBone 는 Engine Define에 전역으로 선언해놓음
    _int                        m_iReadbackBoneIndex[g_iReadbackBone] = {-1};
    _uint                       m_iReadbackFrame = {};

    vector<SOCKET_DESC>         m_vecSockets; //소켓담을곳
    _float                      m_fPrevAnimFrameForSocket = {};
    _bool                       m_bPrevAnimLoopForSocket = { false };
    _bool                       m_bSocketLerpStart = { false };

    _float                      m_fUpperPrevAnimFrameForSocket = {};
    _bool                       m_bUpperPrevAnimLoopForSocket = { false };
    _bool                       m_bUpperSocketLerpStart = { false };

#ifdef _DEBUG
    vector<_uint>   m_vecDebugBoneIndices; // 디버그 표시할 본 인덱스들
    _uint           m_iReadbackBaseSlot = {}; // 기존 Readback 슬롯 끝 지점

public:
    void  Toggle_DebugBone(_uint _boneIndex);           // 디버그 본 선택(본 인덱스로)
    _int  Toggle_DebugBone(const _string& _boneName);   // 디버그 본 선택(본 이름으로)
    _bool Is_DebugBone(_uint _boneIndex) const;         // 선택한 본이 디버그용으로 켜져있는지
    void  Render_DebugBones(_matrix _worldMatrix);                          // 디버그 본 렌더
    _bool Has_DebugBone() { return !m_vecDebugBoneIndices.empty(); }
    void  Render_DebugBones_NoCS(_matrix _worldMatrix, _int _boneIndex);

private:
#endif // _DEBUG

    //망토용
    StructuredBuffer* m_pDrapeDeltaBuffer           = { nullptr };
    StructuredBuffer* m_pLocalMatricesBuffer        = { nullptr };
    StructuredBuffer* m_pHierarchyBuffer            = { nullptr };
    StructuredBuffer* m_pCombinedMatrixBuffer       = { nullptr };
    StructuredBuffer* m_pKeyFrameBuffer             = { nullptr };
    StructuredBuffer* m_pChannelBuffer              = { nullptr };
    StructuredBuffer* m_pAnimInfoBuffer             = { nullptr };
    StructuredBuffer* m_pCurrentKeyIndexBuffer      = { nullptr };
    StructuredBuffer* m_pFrozenSRTBuffer            = { nullptr };
    StructuredBuffer* m_pLocalSRTBuffer             = { nullptr };
    StructuredBuffer* m_pBindPoseSRTBuffer          = { nullptr };
    //상하체 블렌드용 추가
    StructuredBuffer* m_pUpperCurrentKeyIndexBuffer = { nullptr };
    StructuredBuffer* m_pUpperBodyMaskWeightBuffer  = { nullptr };

    //
    _bool m_bNoneAnimFlag = { false }; //이거 NoneAnim 타입일때, 어심프 Ai_PreTransformVertices 플래그 사용안하려고 / 무기 뼈가 다 걸러짐)
    // OcTree 적용용 BoundingBox
    BoundingBox m_tLocalAABB = {};
    BoundingBox m_tWorldAABB = {};
    _bool m_bIsAABBCalculated = { false };
    _uint m_iModelID = {};
    static _uint s_iGlobalModelID;

public:
#ifdef _DEBUG
    void Render_Debug(PrimitiveBatch<DirectX::VertexPositionColor>*) override;

#endif // _DEBUG
    
    void LocalAABB_To_WorldAABB(_matrix _worldmatrix);
    void LocalAABB_To_WorldAABB(BoundingBox _aabb);
public:
    static Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix, MODELROLE _RoleType = MODELROLE::STANDALONE,_bool bNoneAnimFlag = false);
	virtual Component* Clone(void* arg);

public:
	void Free() override final;



};

NS_END
