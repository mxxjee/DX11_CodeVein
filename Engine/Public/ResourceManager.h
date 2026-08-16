#pragma once

#include "Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class Buffer;

class ENGINE_DLL ResourceManager final : public Base
{
private:
	explicit ResourceManager();
	explicit ResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~ResourceManager();

public:
	HRESULT Initialize();

#pragma region Shader
	HRESULT Add_Shader(const _wstring& _ProtoName, class Shader* _shader);
	HRESULT Add_Shader(const _wstring& _ProtoName, const _wstring& _shaderPath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, SHADERENTRIES _entries);
	class Shader* Get_Shader_Prototype(const _wstring& _shaderName);
#pragma endregion Shader

#pragma region Buffer
	/* 버퍼 등록(이미 있으면 기존 버퍼 반환) */
	Buffer* Register_Buffer(_uint _slot, Buffer* _pBuffer);
	/* 슬롯 번호로 버퍼 찾기 */
	Buffer* Find_Buffer(_uint _slot);

	// CS 로컬 버퍼 관리
	Buffer* Register_LocalBuffer(const _string& _cbName, _uint _slot, Buffer* _pBuffer);
	Buffer* Find_LocalBuffer(const _string& _cbName);

	// 핸들 자동 등록 시스템
	void Register_ShaderHandle(const _string& _varName, SHADERHANDLE* _pHandle);
	void Update_Handles_From_Buffer(Buffer* _pBuffer, _uint _slot, _bool _bOverwrite = true);

#pragma endregion Buffer

#pragma region Texture
	ID3D11ShaderResourceView* Load_Texture(const _wstring& _filePath, _bool _bLinear);
	void Release_Texture(const _wstring& _filePath);
	_bool Release_Texture_Safe(const _wstring& _filePath);
#pragma endregion Texture

#pragma region Player
	void Add_Player(GameObject* _player);


	GameObject* Get_Player() { 
		if (m_pPlayer == nullptr) 
		{
			COUT("플레이어 없음");
			return nullptr;
		}
		else 
			return m_pPlayer;
	}

	void Release_Player();

	_vector Get_PlayerPos_Vector();
	_float3 Get_PlayerPos_Float3();
	_float4 Get_PlayerPos_Float4();
	const _float4x4* Get_Player_Matrix_Ptr();
#pragma endregion Player


#pragma region Companion
	GameObject* Get_Companion() {
		if (m_pCompanion == nullptr)
		{
			COUT("동행자 없음");
			return nullptr;
		}
		else
			return m_pCompanion;
	}

	void Add_Companion(GameObject* _companion);
	void Release_Companion();
	_vector Get_CompanionPos_Vector();
	_float3 Get_CompanionPos_Float3();
	_float4 Get_CompanionPos_Float4();
	const _float4x4* Get_Companion_Matrix_Ptr();
#pragma endregion Companion



private:
	UMAP<_wstring, class Shader*>				m_umapShader;	// Shader 공유자산
	vector<Buffer*>								m_vecBuffers;	// Buffer 공유자산
	UMAP<_wstring, ID3D11ShaderResourceView*>	m_mapSRVs;		// Material이 공유하는 Texture(SRV)

	// CS 로컬 버퍼 (cbuffer 이름 기준 관리)
	UMAP<_string, Buffer*>						m_umapLocalBuffers;

	// 핸들 레지스트리 (변수명 -> static 핸들 포인터)
	UMAP<_string, SHADERHANDLE*>				m_umapHandleRegistry;

	// 플레이어
	GameObject* m_pPlayer = { nullptr };
	// 동행자
	GameObject* m_pCompanion = { nullptr };

public:
	/* Buffer_Camera (b0) */
	DECLARE_SHADER_HANDLE(g_ViewMatrix)
	DECLARE_SHADER_HANDLE(g_ProjMatrix)
	DECLARE_SHADER_HANDLE(g_InverseViewMatrix)
	DECLARE_SHADER_HANDLE(g_InverseProjMatrix)
	DECLARE_SHADER_HANDLE(g_vCamPosition)

	/* Buffer_Object (b1) */
	DECLARE_SHADER_HANDLE(g_WorldMatrix)
	DECLARE_SHADER_HANDLE(g_PrevWorldMatrix)
	DECLARE_SHADER_HANDLE(g_vColor)
	DECLARE_SHADER_HANDLE(g_fProgress)
	DECLARE_SHADER_HANDLE(g_ObjectID)
	DECLARE_SHADER_HANDLE(g_bitFlag)
	DECLARE_SHADER_HANDLE(g_fTime)
	DECLARE_SHADER_HANDLE(g_fRimOn)
	DECLARE_SHADER_HANDLE(g_vEmissionUVPos)

	/* Buffer_Lights (b2) */
	DECLARE_SHADER_HANDLE(g_fLightRange)
	DECLARE_SHADER_HANDLE(g_vLightDirection)
	DECLARE_SHADER_HANDLE(g_vLightPosition)
	DECLARE_SHADER_HANDLE(g_vLightDiffuse)
	DECLARE_SHADER_HANDLE(g_vLightAmbient)
	DECLARE_SHADER_HANDLE(g_vLightSpecular)
	DECLARE_SHADER_HANDLE(g_LightViewMatrix)
	DECLARE_SHADER_HANDLE(g_LightProjMatrix)
	DECLARE_SHADER_HANDLE(g_fSpotInnerCone)
	DECLARE_SHADER_HANDLE(g_fSpotOuterCone)
	DECLARE_SHADER_HANDLE(lights_vDiffuse)
	DECLARE_SHADER_HANDLE(lights_vAmbient)
	DECLARE_SHADER_HANDLE(lights_vSpecular)
	DECLARE_SHADER_HANDLE(lights_vPosition)
	DECLARE_SHADER_HANDLE(lights_vRangeAndType)
	DECLARE_SHADER_HANDLE(lightCount)


	/* Buffer_Player (b3) */
	DECLARE_SHADER_HANDLE(g_vPlayerInnerColor)
	DECLARE_SHADER_HANDLE(g_fPlayerColorStrength)
	DECLARE_SHADER_HANDLE(g_vPlayerHairColor);
	DECLARE_SHADER_HANDLE(g_fPlayerGeneralShift)
	DECLARE_SHADER_HANDLE(g_fPlayerHighlight1Shift)
	DECLARE_SHADER_HANDLE(g_fPlayerHighlight1Strength);
	DECLARE_SHADER_HANDLE(g_fPlayerHighlight2Shift)
	DECLARE_SHADER_HANDLE(g_fPlayerHighlight2Strength)
	DECLARE_SHADER_HANDLE(g_fPlayerHighlightPower);
	DECLARE_SHADER_HANDLE(g_fPlayerContrast)
	DECLARE_SHADER_HANDLE(g_fPlayerMetallic)
	DECLARE_SHADER_HANDLE(g_fPlayerRoughnessMin);
	DECLARE_SHADER_HANDLE(g_vPlayerEyeBaseColor);
	DECLARE_SHADER_HANDLE(g_vPlayerEyeAppendColor);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeScale);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeIndividualScaleOffset);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeOffsetU);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeOffsetV);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeCastsEye);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeCentering);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeIrisStrength);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeHighlightStrength);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeIrisScaleX);
	DECLARE_SHADER_HANDLE(g_fPlayerEyeIrisScaleY);
	DECLARE_SHADER_HANDLE(g_vPlayerBrowColor);
	DECLARE_SHADER_HANDLE(g_vPlayerEyeLineColor);
	DECLARE_SHADER_HANDLE(g_vPlayerBrowUVOffset);
	DECLARE_SHADER_HANDLE(g_fPlayerBrowUVScale);
	DECLARE_SHADER_HANDLE(g_iPlayerUseSecondUV);
	DECLARE_SHADER_HANDLE(g_fPlayerBrowUVRotation);
	DECLARE_SHADER_HANDLE(g_fPlayerBrowXGap);

	/* Buffer_Bones (b4) */
	DECLARE_SHADER_HANDLE(g_BonesMatrices)

	/* Buffer_Morph (b5) */
	DECLARE_SHADER_HANDLE(g_MorphWeightsPacked)
	DECLARE_SHADER_HANDLE(g_MorphIndicesPacked)
	DECLARE_SHADER_HANDLE(g_NumActiveMorphs)
	DECLARE_SHADER_HANDLE(g_NumVertices)

	/* Buffer_LocalAnim (b6) */
	DECLARE_SHADER_HANDLE(g_LiAnimIndex)
	DECLARE_SHADER_HANDLE(g_LiNumBones)
	DECLARE_SHADER_HANDLE(g_LiRootBoneIndex)
	DECLARE_SHADER_HANDLE(g_LbAnimLoop)
	DECLARE_SHADER_HANDLE(g_LfCurrentFrame)
	DECLARE_SHADER_HANDLE(g_LfLerpRatio)
	DECLARE_SHADER_HANDLE(g_LbLerping)
	DECLARE_SHADER_HANDLE(g_LbRemoveRootTranslation)
	DECLARE_SHADER_HANDLE(g_LiUpperAnimIndex)
	DECLARE_SHADER_HANDLE(g_LbUpperAnimLoop)
	DECLARE_SHADER_HANDLE(g_LfUpperCurrentFrame)
	DECLARE_SHADER_HANDLE(g_LbUpperRemoveRootTranslation)
	DECLARE_SHADER_HANDLE(g_LbUpperBodyBlendEnalbe)
	DECLARE_SHADER_HANDLE(g_LfUpperLayerWeight)
	DECLARE_SHADER_HANDLE(g_LfUpperLerpRatio)
	DECLARE_SHADER_HANDLE(g_LbUpperLerping)

	/* Buffer_Bone (b7) */
	DECLARE_SHADER_HANDLE(g_NumBones)
	DECLARE_SHADER_HANDLE(g_CurrentDepth)
	DECLARE_SHADER_HANDLE(g_PreTransformMatrix)

	/* Buffer_Decal (b8) */
	DECLARE_SHADER_HANDLE(g_InvDecalWorldMatrix)
	DECLARE_SHADER_HANDLE(g_fDecalAlpha)
	DECLARE_SHADER_HANDLE(g_iMaskIndex)

	/* Buffer_Particle (b8 / Compute_Shader) */
	DECLARE_SHADER_HANDLE(g_vPivot)
	DECLARE_SHADER_HANDLE(g_fTimeDelta)
	DECLARE_SHADER_HANDLE(g_iNumParticles)
	DECLARE_SHADER_HANDLE(g_bIsLoop)
	DECLARE_SHADER_HANDLE(g_fAccumulatedTime)
	DECLARE_SHADER_HANDLE(g_fShakeStrength)
	DECLARE_SHADER_HANDLE(g_matParticleWorld)
	DECLARE_SHADER_HANDLE(g_matPrevParticleWorld)
	DECLARE_SHADER_HANDLE(g_bFollowParent)
	DECLARE_SHADER_HANDLE(g_fConvergeMaxDist)

	/* Buffer_Effect (b9) */
	DECLARE_SHADER_HANDLE(g_EffectWorldMatrix)
	DECLARE_SHADER_HANDLE(g_vScaleUV)
	DECLARE_SHADER_HANDLE(g_fTransparency)
	DECLARE_SHADER_HANDLE(g_fAlpha)
	DECLARE_SHADER_HANDLE(g_fDistortionStrength)
	DECLARE_SHADER_HANDLE(g_bIsBillboard)
	DECLARE_SHADER_HANDLE(g_iFrameCount)
	DECLARE_SHADER_HANDLE(g_iCountX)
	DECLARE_SHADER_HANDLE(g_iCountY)
	DECLARE_SHADER_HANDLE(g_iTransparentIndex)
	DECLARE_SHADER_HANDLE(g_bUseMask)
	DECLARE_SHADER_HANDLE(g_bRadialRotation)
	DECLARE_SHADER_HANDLE(g_vEffectPivot)
	DECLARE_SHADER_HANDLE(g_iMaskStartFrame)
	DECLARE_SHADER_HANDLE(g_iMaskFrameCount)
	DECLARE_SHADER_HANDLE(g_iMaskCountX)
	DECLARE_SHADER_HANDLE(g_iMaskCountY)
	DECLARE_SHADER_HANDLE(g_vMaskUVScroll)
	DECLARE_SHADER_HANDLE(g_fParticleScale)
	DECLARE_SHADER_HANDLE(g_bUseNoise)
	DECLARE_SHADER_HANDLE(g_bVerticalFade)
	DECLARE_SHADER_HANDLE(g_bUseTexture)
	DECLARE_SHADER_HANDLE(g_bUseAlphaMask)
	DECLARE_SHADER_HANDLE(g_bUseEdgeGlow)
	DECLARE_SHADER_HANDLE(g_fGlowStrength)
	DECLARE_SHADER_HANDLE(g_fLifeTime)
	DECLARE_SHADER_HANDLE(g_fTimeElapsed)
	DECLARE_SHADER_HANDLE(g_vDiffuseUVScroll)
	DECLARE_SHADER_HANDLE(g_vMaskUVScale)
	DECLARE_SHADER_HANDLE(g_vFadeInOut)
	DECLARE_SHADER_HANDLE(g_fMaskIntensity)
	DECLARE_SHADER_HANDLE(g_vMaskUVStartOffset)
	DECLARE_SHADER_HANDLE(g_iMaskSampler)
	DECLARE_SHADER_HANDLE(g_bDirectionalDissolve)
	DECLARE_SHADER_HANDLE(g_bDissolveFlipY)
	DECLARE_SHADER_HANDLE(g_fNoiseWeight)
	DECLARE_SHADER_HANDLE(g_fDissolveEdgeWidth)
	DECLARE_SHADER_HANDLE(g_vDissolveEdgeColor)
	DECLARE_SHADER_HANDLE(g_bVerticalShrink)
	DECLARE_SHADER_HANDLE(g_fDisplaceStrength)
	DECLARE_SHADER_HANDLE(g_vHotColor)
	DECLARE_SHADER_HANDLE(g_fHotColor)
	DECLARE_SHADER_HANDLE(g_iDirectionalScale)
	DECLARE_SHADER_HANDLE(g_bEmissive)
	DECLARE_SHADER_HANDLE(g_vEffectOffset)

	/* Buffer_UI (b10) */
	DECLARE_SHADER_HANDLE(g_UIColor)
	DECLARE_SHADER_HANDLE(g_UVScale)
	DECLARE_SHADER_HANDLE(g_UVOffSet)
	DECLARE_SHADER_HANDLE(g_UVScroll)
	DECLARE_SHADER_HANDLE(g_AlphaOffSet)
	DECLARE_SHADER_HANDLE(g_Reserved)
	DECLARE_SHADER_HANDLE(g_TextureSize)
	DECLARE_SHADER_HANDLE(g_OutLineThickness)
	DECLARE_SHADER_HANDLE(g_UIPadding)
	DECLARE_SHADER_HANDLE(g_OutLineColor)
	DECLARE_SHADER_HANDLE(g_UIProgress)
	DECLARE_SHADER_HANDLE(g_ScrollTime)
	DECLARE_SHADER_HANDLE(g_fClipX)
	DECLARE_SHADER_HANDLE(g_fUIValue)

	/* Buffer_Readback (b12) */
	DECLARE_SHADER_HANDLE(g_iReadbackIndices)
	DECLARE_SHADER_HANDLE(g_iReadbackCount)
	//Buffer_DarpeBone(CS)
	DECLARE_SHADER_HANDLE(g_iDrapeDeltaCount);

	/* Buffer_Final (b13) */
	DECLARE_SHADER_HANDLE(g_vPlayerUV);
	DECLARE_SHADER_HANDLE(g_vCenterUV);
	DECLARE_SHADER_HANDLE(g_vTargetUV);
	DECLARE_SHADER_HANDLE(g_isClipping);
	DECLARE_SHADER_HANDLE(g_fRotation);
	DECLARE_SHADER_HANDLE(g_fZoom);
	DECLARE_SHADER_HANDLE(g_fIconScale);
	DECLARE_SHADER_HANDLE(g_fCameraAngle);



	DECLARE_SHADER_HANDLE(g_fLightMultiplier);
	DECLARE_SHADER_HANDLE(g_fAmbientStrength);
	DECLARE_SHADER_HANDLE(g_vToonShadowRange);
	DECLARE_SHADER_HANDLE(g_vToonBrightnessRange);
	DECLARE_SHADER_HANDLE(g_vAmbientFloorMin);
	DECLARE_SHADER_HANDLE(g_vSSSColor);
	DECLARE_SHADER_HANDLE(g_fBackFillStrength);
	DECLARE_SHADER_HANDLE(g_vSkinTint);
	DECLARE_SHADER_HANDLE(g_fSSSPower);
	DECLARE_SHADER_HANDLE(g_fSSSIntensity);
	DECLARE_SHADER_HANDLE(g_fSkinRoughnessMin);
	DECLARE_SHADER_HANDLE(g_fRimPower);
	DECLARE_SHADER_HANDLE(g_fRimIntensity);
	DECLARE_SHADER_HANDLE(g_vRimMaskRange);
	DECLARE_SHADER_HANDLE(g_vSpecBoostRange);
	DECLARE_SHADER_HANDLE(g_fSpecularIntensity);
	DECLARE_SHADER_HANDLE(g_fMinDiffuse);
	DECLARE_SHADER_HANDLE(g_fKValue);
	DECLARE_SHADER_HANDLE(g_fMtrlRoughnessMin);
	DECLARE_SHADER_HANDLE(g_ShadowSplit);
	DECLARE_SHADER_HANDLE(g_ShadowDir);
	DECLARE_SHADER_HANDLE(g_ShadowCascade_MixMaxtrix);
	DECLARE_SHADER_HANDLE(g_Shadow_ViewProjMatrix);
	DECLARE_SHADER_HANDLE(g_SSAOOption);


	DECLARE_SHADER_HANDLE(g_vShadowTintColor);
	DECLARE_SHADER_HANDLE(g_fShadowTintWeight);
	DECLARE_SHADER_HANDLE(g_vMidtoneTintColor);
	DECLARE_SHADER_HANDLE(g_fMidtoneTintWeight);
	DECLARE_SHADER_HANDLE(g_vHighlightTintColor);
	DECLARE_SHADER_HANDLE(g_fHighlightTintWeight);
	DECLARE_SHADER_HANDLE(g_fShadowRange);
	DECLARE_SHADER_HANDLE(g_fHighLightRange);
	DECLARE_SHADER_HANDLE(g_fEnableColorGrading);
	DECLARE_SHADER_HANDLE(g_FogColor);
	DECLARE_SHADER_HANDLE(g_bFogEnable);
	DECLARE_SHADER_HANDLE(g_fFogStartDist);
	DECLARE_SHADER_HANDLE(g_fFogEndDist);
	DECLARE_SHADER_HANDLE(g_fFogDensity);
	DECLARE_SHADER_HANDLE(g_fogType);
	DECLARE_SHADER_HANDLE(g_fFogBaseHeight);
	DECLARE_SHADER_HANDLE(g_fFogFadeHeight);
	DECLARE_SHADER_HANDLE(g_fHeightFogIntensity);
	DECLARE_SHADER_HANDLE(g_fFogIntensity);
	DECLARE_SHADER_HANDLE(g_HDROption);
	DECLARE_SHADER_HANDLE(g_fSpecSoftClamp);
	DECLARE_SHADER_HANDLE(g_vBackFaceShaowRange);
	DECLARE_SHADER_HANDLE(g_fShadowBright);
	DECLARE_SHADER_HANDLE(g_fShadowBackBright);
	DECLARE_SHADER_HANDLE(g_ShadowCascadeNum);
	DECLARE_SHADER_HANDLE(g_ShadowCascade_ViewProjMatrix);

	
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static ResourceManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END