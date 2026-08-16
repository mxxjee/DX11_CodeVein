#include "Engine_Define.h"
#include "ResourceManager.h"

#include "GameInstance.h"
#include "Buffer.h"
#include "Shader.h"
#include "GameObject.h"

// HDR파일(이미지 읽기용) 헤더
#include <DirectXTex/DirectXTex.h>
#include <filesystem>

static vector<pair<_string, Engine::SHADERHANDLE*>>& Get_HandleRegistry()
{
	static vector<pair<_string, Engine::SHADERHANDLE*>> registry;
	return registry;
}

#pragma region Binding Variable(상수 버퍼 바인딩 편하게 하는 하드코딩)
/* Buffer_Camera (b0) */
DEFINE_SHADER_HANDLE(g_ViewMatrix)
DEFINE_SHADER_HANDLE(g_ProjMatrix)
DEFINE_SHADER_HANDLE(g_InverseViewMatrix)
DEFINE_SHADER_HANDLE(g_InverseProjMatrix)
DEFINE_SHADER_HANDLE(g_vCamPosition)

/* Buffer_Object (b1) */
DEFINE_SHADER_HANDLE(g_WorldMatrix)
DEFINE_SHADER_HANDLE(g_PrevWorldMatrix)
DEFINE_SHADER_HANDLE(g_vColor)
DEFINE_SHADER_HANDLE(g_fProgress)
DEFINE_SHADER_HANDLE(g_ObjectID)
DEFINE_SHADER_HANDLE(g_bitFlag)
DEFINE_SHADER_HANDLE(g_fTime)
DEFINE_SHADER_HANDLE(g_fRimOn)
DEFINE_SHADER_HANDLE(g_vEmissionUVPos)

/* Buffer_Lights (b2) */
DEFINE_SHADER_HANDLE(g_fLightRange)
DEFINE_SHADER_HANDLE(g_vLightDirection)
DEFINE_SHADER_HANDLE(g_vLightPosition)
DEFINE_SHADER_HANDLE(g_vLightDiffuse)
DEFINE_SHADER_HANDLE(g_vLightAmbient)
DEFINE_SHADER_HANDLE(g_vLightSpecular)
DEFINE_SHADER_HANDLE(g_LightViewMatrix)
DEFINE_SHADER_HANDLE(g_LightProjMatrix)
DEFINE_SHADER_HANDLE(g_fSpotInnerCone)
DEFINE_SHADER_HANDLE(g_fSpotOuterCone)
DEFINE_SHADER_HANDLE(lights_vDiffuse)
DEFINE_SHADER_HANDLE(lights_vAmbient)
DEFINE_SHADER_HANDLE(lights_vSpecular)
DEFINE_SHADER_HANDLE(lights_vPosition)
DEFINE_SHADER_HANDLE(lights_vRangeAndType)
DEFINE_SHADER_HANDLE(lightCount)

/* Buffer_Player (b3) */
DEFINE_SHADER_HANDLE(g_vPlayerInnerColor)
DEFINE_SHADER_HANDLE(g_fPlayerColorStrength)
DEFINE_SHADER_HANDLE(g_vPlayerHairColor);
DEFINE_SHADER_HANDLE(g_fPlayerGeneralShift)
DEFINE_SHADER_HANDLE(g_fPlayerHighlight1Shift)
DEFINE_SHADER_HANDLE(g_fPlayerHighlight1Strength);
DEFINE_SHADER_HANDLE(g_fPlayerHighlight2Shift)
DEFINE_SHADER_HANDLE(g_fPlayerHighlight2Strength)
DEFINE_SHADER_HANDLE(g_fPlayerHighlightPower);
DEFINE_SHADER_HANDLE(g_fPlayerContrast)
DEFINE_SHADER_HANDLE(g_fPlayerMetallic)
DEFINE_SHADER_HANDLE(g_fPlayerRoughnessMin);
DEFINE_SHADER_HANDLE(g_vPlayerEyeBaseColor);
DEFINE_SHADER_HANDLE(g_vPlayerEyeAppendColor);
DEFINE_SHADER_HANDLE(g_fPlayerEyeScale);
DEFINE_SHADER_HANDLE(g_fPlayerEyeIndividualScaleOffset);
DEFINE_SHADER_HANDLE(g_fPlayerEyeOffsetU);
DEFINE_SHADER_HANDLE(g_fPlayerEyeOffsetV);
DEFINE_SHADER_HANDLE(g_fPlayerEyeCastsEye);
DEFINE_SHADER_HANDLE(g_fPlayerEyeCentering);
DEFINE_SHADER_HANDLE(g_fPlayerEyeIrisStrength);
DEFINE_SHADER_HANDLE(g_fPlayerEyeHighlightStrength);
DEFINE_SHADER_HANDLE(g_fPlayerEyeIrisScaleX);
DEFINE_SHADER_HANDLE(g_fPlayerEyeIrisScaleY);
DEFINE_SHADER_HANDLE(g_vPlayerBrowColor);
DEFINE_SHADER_HANDLE(g_vPlayerEyeLineColor);
DEFINE_SHADER_HANDLE(g_vPlayerBrowUVOffset);
DEFINE_SHADER_HANDLE(g_fPlayerBrowUVScale);
DEFINE_SHADER_HANDLE(g_iPlayerUseSecondUV);
DEFINE_SHADER_HANDLE(g_fPlayerBrowUVRotation);
DEFINE_SHADER_HANDLE(g_fPlayerBrowXGap);

/* Buffer_Bones (b4) */
DEFINE_SHADER_HANDLE(g_BonesMatrices)

/* Buffer_Morph (b5) */
DEFINE_SHADER_HANDLE(g_MorphWeightsPacked)
DEFINE_SHADER_HANDLE(g_MorphIndicesPacked)
DEFINE_SHADER_HANDLE(g_NumActiveMorphs)
DEFINE_SHADER_HANDLE(g_NumVertices)

/* Buffer_LocalAnim (b6) */
DEFINE_SHADER_HANDLE(g_LiAnimIndex)
DEFINE_SHADER_HANDLE(g_LiNumBones)
DEFINE_SHADER_HANDLE(g_LiRootBoneIndex)
DEFINE_SHADER_HANDLE(g_LbAnimLoop)
DEFINE_SHADER_HANDLE(g_LfCurrentFrame)
DEFINE_SHADER_HANDLE(g_LfLerpRatio)
DEFINE_SHADER_HANDLE(g_LbLerping)
DEFINE_SHADER_HANDLE(g_LbRemoveRootTranslation)
DEFINE_SHADER_HANDLE(g_LiUpperAnimIndex)
DEFINE_SHADER_HANDLE(g_LbUpperAnimLoop)
DEFINE_SHADER_HANDLE(g_LfUpperCurrentFrame)
DEFINE_SHADER_HANDLE(g_LbUpperRemoveRootTranslation)
DEFINE_SHADER_HANDLE(g_LbUpperBodyBlendEnalbe)
DEFINE_SHADER_HANDLE(g_LfUpperLayerWeight)
DEFINE_SHADER_HANDLE(g_LfUpperLerpRatio)
DEFINE_SHADER_HANDLE(g_LbUpperLerping)

/* Buffer_Bone (b7) */
DEFINE_SHADER_HANDLE(g_NumBones)
DEFINE_SHADER_HANDLE(g_CurrentDepth)
DEFINE_SHADER_HANDLE(g_PreTransformMatrix)

/* Buffer_Decal (b8) */
DEFINE_SHADER_HANDLE(g_InvDecalWorldMatrix)
DEFINE_SHADER_HANDLE(g_fDecalAlpha)
DEFINE_SHADER_HANDLE(g_iMaskIndex)

/* Buffer_Particle (b8 / Compute_Shader) */
DEFINE_SHADER_HANDLE(g_vPivot)
DEFINE_SHADER_HANDLE(g_fTimeDelta)
DEFINE_SHADER_HANDLE(g_iNumParticles)
DEFINE_SHADER_HANDLE(g_bIsLoop)
DEFINE_SHADER_HANDLE(g_fAccumulatedTime)
DEFINE_SHADER_HANDLE(g_fShakeStrength)
DEFINE_SHADER_HANDLE(g_matParticleWorld)
DEFINE_SHADER_HANDLE(g_matPrevParticleWorld)
DEFINE_SHADER_HANDLE(g_bFollowParent)
DEFINE_SHADER_HANDLE(g_fConvergeMaxDist)

/* Buffer_Effect (b9) */
DEFINE_SHADER_HANDLE(g_EffectWorldMatrix)

DEFINE_SHADER_HANDLE(g_vScaleUV)
DEFINE_SHADER_HANDLE(g_fTransparency)
DEFINE_SHADER_HANDLE(g_fAlpha)

DEFINE_SHADER_HANDLE(g_fDistortionStrength)
DEFINE_SHADER_HANDLE(g_bIsBillboard)
DEFINE_SHADER_HANDLE(g_iFrameCount)
DEFINE_SHADER_HANDLE(g_iCountX)

DEFINE_SHADER_HANDLE(g_iCountY)
DEFINE_SHADER_HANDLE(g_iTransparentIndex)
DEFINE_SHADER_HANDLE(g_bUseMask)
DEFINE_SHADER_HANDLE(g_bRadialRotation)

DEFINE_SHADER_HANDLE(g_vEffectPivot)
DEFINE_SHADER_HANDLE(g_iMaskStartFrame)

DEFINE_SHADER_HANDLE(g_iMaskFrameCount)
DEFINE_SHADER_HANDLE(g_iMaskCountX)
DEFINE_SHADER_HANDLE(g_iMaskCountY)
DEFINE_SHADER_HANDLE(g_fParticleScale)

DEFINE_SHADER_HANDLE(g_vMaskUVScroll)
DEFINE_SHADER_HANDLE(g_bUseNoise)
DEFINE_SHADER_HANDLE(g_bVerticalFade)

DEFINE_SHADER_HANDLE(g_bUseTexture)
DEFINE_SHADER_HANDLE(g_bUseAlphaMask)
DEFINE_SHADER_HANDLE(g_bUseEdgeGlow)
DEFINE_SHADER_HANDLE(g_fGlowStrength)
DEFINE_SHADER_HANDLE(g_fLifeTime)
DEFINE_SHADER_HANDLE(g_fTimeElapsed)
DEFINE_SHADER_HANDLE(g_vDiffuseUVScroll)
DEFINE_SHADER_HANDLE(g_vMaskUVScale)
DEFINE_SHADER_HANDLE(g_vFadeInOut)
DEFINE_SHADER_HANDLE(g_fMaskIntensity)
DEFINE_SHADER_HANDLE(g_vMaskUVStartOffset)
DEFINE_SHADER_HANDLE(g_iMaskSampler)
DEFINE_SHADER_HANDLE(g_bDirectionalDissolve)
DEFINE_SHADER_HANDLE(g_bDissolveFlipY)
DEFINE_SHADER_HANDLE(g_fNoiseWeight)
DEFINE_SHADER_HANDLE(g_fDissolveEdgeWidth)
DEFINE_SHADER_HANDLE(g_vDissolveEdgeColor)
DEFINE_SHADER_HANDLE(g_bVerticalShrink)
DEFINE_SHADER_HANDLE(g_fDisplaceStrength)
DEFINE_SHADER_HANDLE(g_vHotColor)
DEFINE_SHADER_HANDLE(g_fHotColor)
DEFINE_SHADER_HANDLE(g_iDirectionalScale)
DEFINE_SHADER_HANDLE(g_bEmissive)
DEFINE_SHADER_HANDLE(g_vEffectOffset)

/* Buffer_UI (b10) */
DEFINE_SHADER_HANDLE(g_UIColor)
DEFINE_SHADER_HANDLE(g_UVScale)
DEFINE_SHADER_HANDLE(g_UVOffSet)
DEFINE_SHADER_HANDLE(g_UVScroll)
DEFINE_SHADER_HANDLE(g_AlphaOffSet)
DEFINE_SHADER_HANDLE(g_Reserved)
DEFINE_SHADER_HANDLE(g_TextureSize)
DEFINE_SHADER_HANDLE(g_OutLineThickness)
DEFINE_SHADER_HANDLE(g_UIPadding)
DEFINE_SHADER_HANDLE(g_OutLineColor)
DEFINE_SHADER_HANDLE(g_UIProgress)
DEFINE_SHADER_HANDLE(g_ScrollTime)

/* Buffer_Readback (b12) */
DEFINE_SHADER_HANDLE(g_iReadbackIndices)
DEFINE_SHADER_HANDLE(g_iReadbackCount)
//Buffer_DarpeBone(CS)
DEFINE_SHADER_HANDLE(g_iDrapeDeltaCount);
/* Buffer_Final (b13) */
DEFINE_SHADER_HANDLE(g_vPlayerUV);
DEFINE_SHADER_HANDLE(g_vCenterUV);
DEFINE_SHADER_HANDLE(g_vTargetUV);
DEFINE_SHADER_HANDLE(g_isClipping);
DEFINE_SHADER_HANDLE(g_fRotation);
DEFINE_SHADER_HANDLE(g_fZoom);
DEFINE_SHADER_HANDLE(g_fIconScale);
DEFINE_SHADER_HANDLE(g_fCameraAngle);

DEFINE_SHADER_HANDLE(g_fLightMultiplier);
DEFINE_SHADER_HANDLE(g_fAmbientStrength);
DEFINE_SHADER_HANDLE(g_vToonShadowRange);
DEFINE_SHADER_HANDLE(g_vToonBrightnessRange);
DEFINE_SHADER_HANDLE(g_vAmbientFloorMin);
DEFINE_SHADER_HANDLE(g_vSSSColor);
DEFINE_SHADER_HANDLE(g_fBackFillStrength);
DEFINE_SHADER_HANDLE(g_vSkinTint);
DEFINE_SHADER_HANDLE(g_fSSSPower);
DEFINE_SHADER_HANDLE(g_fSSSIntensity);
DEFINE_SHADER_HANDLE(g_fSkinRoughnessMin);
DEFINE_SHADER_HANDLE(g_fRimPower);
DEFINE_SHADER_HANDLE(g_fRimIntensity);
DEFINE_SHADER_HANDLE(g_vRimMaskRange);
DEFINE_SHADER_HANDLE(g_vSpecBoostRange);
DEFINE_SHADER_HANDLE(g_fSpecularIntensity);
DEFINE_SHADER_HANDLE(g_fMinDiffuse);
DEFINE_SHADER_HANDLE(g_fKValue);
DEFINE_SHADER_HANDLE(g_fMtrlRoughnessMin);
DEFINE_SHADER_HANDLE(g_ShadowSplit);
DEFINE_SHADER_HANDLE(g_ShadowDir);
DEFINE_SHADER_HANDLE(g_ShadowCascade_MixMaxtrix);
DEFINE_SHADER_HANDLE(g_Shadow_ViewProjMatrix);
DEFINE_SHADER_HANDLE(g_SSAOOption);


DEFINE_SHADER_HANDLE(g_vShadowTintColor);
DEFINE_SHADER_HANDLE(g_fShadowTintWeight);
DEFINE_SHADER_HANDLE(g_vMidtoneTintColor);
DEFINE_SHADER_HANDLE(g_fMidtoneTintWeight);
DEFINE_SHADER_HANDLE(g_vHighlightTintColor);
DEFINE_SHADER_HANDLE(g_fHighlightTintWeight);
DEFINE_SHADER_HANDLE(g_fShadowRange);
DEFINE_SHADER_HANDLE(g_fHighLightRange);
DEFINE_SHADER_HANDLE(g_fEnableColorGrading);
DEFINE_SHADER_HANDLE(g_FogColor);
DEFINE_SHADER_HANDLE(g_bFogEnable);
DEFINE_SHADER_HANDLE(g_fFogStartDist);
DEFINE_SHADER_HANDLE(g_fFogEndDist);
DEFINE_SHADER_HANDLE(g_fFogDensity);
DEFINE_SHADER_HANDLE(g_fogType);
DEFINE_SHADER_HANDLE(g_fFogBaseHeight);
DEFINE_SHADER_HANDLE(g_fFogFadeHeight);
DEFINE_SHADER_HANDLE(g_fHeightFogIntensity);
DEFINE_SHADER_HANDLE(g_fFogIntensity);
DEFINE_SHADER_HANDLE(g_HDROption);
DEFINE_SHADER_HANDLE(g_fSpecSoftClamp);
DEFINE_SHADER_HANDLE(g_vBackFaceShaowRange);
DEFINE_SHADER_HANDLE(g_fShadowBright);
DEFINE_SHADER_HANDLE(g_fShadowBackBright);
DEFINE_SHADER_HANDLE(g_ShadowCascadeNum);
DEFINE_SHADER_HANDLE(g_ShadowCascade_ViewProjMatrix);


#pragma region Binding Variable(상수 버퍼 바인딩 편하게 하는 하드코딩)


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ResourceManager::ResourceManager()
{
}

Engine::ResourceManager::ResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::ResourceManager::~ResourceManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ResourceManager::Initialize()
{
	// 상수 버퍼는 최대 개수가 14개라서 14개만 만들어줌
	m_vecBuffers.resize(14, nullptr);

	for (auto& pair : Get_HandleRegistry())
	{
		m_umapHandleRegistry.try_emplace(pair.first, pair.second);
	}


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// Shader ////////////////////////////////////////////////////////
HRESULT Engine::ResourceManager::Add_Shader(const _wstring& _ProtoName, Shader* _shader)
{
	if (m_umapShader.contains(_ProtoName))
	{
		MSG_ON((L"추가하려는 쉐이더 \n" + _ProtoName + L"\n이 이미 존재합니다.").c_str(), L"쉐이더 추가 실패!");
		BREAK;
		return E_FAIL;
	}

	m_umapShader[_ProtoName] = _shader;
	Safe_AddRef(_shader);

	m_pGameInstance->Add_Prototype(0, _ProtoName, _shader);

	return S_OK;
}

HRESULT Engine::ResourceManager::Add_Shader(const _wstring& _ProtoName, const _wstring& _shaderPath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, SHADERENTRIES _entries)
{
	if (m_umapShader.contains(_ProtoName))
	{
		MSG_ON((L"추가하려는 쉐이더 \n" + _ProtoName + L"\n이 이미 존재합니다.").c_str(), L"쉐이더 추가 실패!");
		BREAK;
		return E_FAIL;
	}

	Shader* shader = Shader::Create(m_pDevice, m_pContext, _shaderPath, _elementsDesc, _numElements, _entries);

	m_umapShader[_ProtoName] = shader;
	Safe_AddRef(shader);

	m_pGameInstance->Add_Prototype(0, _ProtoName, shader);

	return S_OK;
}

Shader* Engine::ResourceManager::Get_Shader_Prototype(const _wstring& _shaderName)
{
	if (!m_umapShader.contains(_shaderName))
	{
		MSG_ON((L"검색한 쉐이더 \n" + _shaderName + L"\n가 없습니다.").c_str(), L"쉐이더 검색 실패!");
		BREAK;
		return nullptr;
	}

	return m_umapShader[_shaderName];
}
/******************************************************* Shader *******************************************************/



//////////////////////////////////////////////////////// Buffer ////////////////////////////////////////////////////////
Buffer* Engine::ResourceManager::Register_Buffer(_uint _slot, Buffer* _pBuffer)
{
	// 있으면 기존 버퍼 반환
	if (m_vecBuffers[_slot] != nullptr)
	{
		Safe_AddRef(m_vecBuffers[_slot]);
		return m_vecBuffers[_slot];
	}

	m_vecBuffers[_slot] = _pBuffer;
	Safe_AddRef(m_vecBuffers[_slot]);

	// 버퍼 등록 시 핸들 자동 갱신
	Update_Handles_From_Buffer(_pBuffer, _slot, true);

	return nullptr;
}

Buffer* Engine::ResourceManager::Find_Buffer(_uint _slot)
{
	if (m_vecBuffers[_slot] == nullptr)
		return nullptr;

	return m_vecBuffers[_slot];
}

// CS 로컬 버퍼 등록 (이름 기준, 중복 시 기존 버퍼 공유)
Buffer* Engine::ResourceManager::Register_LocalBuffer(const _string& _cbName, _uint _slot, Buffer* _pBuffer)
{
	auto it = m_umapLocalBuffers.find(_cbName);
	if (it != m_umapLocalBuffers.end())
	{
		// 이미 같은 이름의 로컬 버퍼가 존재 -> 공유
		Safe_AddRef(it->second);
		return it->second;
	}

	// 새 로컬 버퍼 등록
	m_umapLocalBuffers.try_emplace(_cbName, _pBuffer);
	Safe_AddRef(_pBuffer);

	// 핸들 자동 갱신 (기존 전역 핸들 덮어쓰지 않음)
	Update_Handles_From_Buffer(_pBuffer, _slot, false);

	return nullptr;
}

Buffer* Engine::ResourceManager::Find_LocalBuffer(const _string& _cbName)
{
	auto it = m_umapLocalBuffers.find(_cbName);
	if (it == m_umapLocalBuffers.end())
		return nullptr;

	return it->second;
}

// 핸들 레지스트리 등록
void Engine::ResourceManager::Register_ShaderHandle(const _string& _varName, SHADERHANDLE* _pHandle)
{
	m_umapHandleRegistry.try_emplace(_varName, _pHandle);
}

// Buffer 변수로부터 핸들 자동 갱신
void Engine::ResourceManager::Update_Handles_From_Buffer(Buffer* _pBuffer, _uint _slot, _bool _bOverwrite)
{
	if (_pBuffer == nullptr) return;

	auto umapNames = _pBuffer->Get_UmapVariablenames();
	for (auto& pair : umapNames)
	{
		auto it = m_umapHandleRegistry.find(pair.first);
		if (it != m_umapHandleRegistry.end())
		{
			// 덮어쓰기 방지 : 이미 유효한 핸들이면 스킵
			if (!_bOverwrite && it->second->iBufferNum != UINT_MAX)
				continue;

			it->second->iBufferNum = _slot;
			it->second->iVariableNum = pair.second;
		}
	}
}
/******************************************************* Buffer *******************************************************/



//////////////////////////////////////////////////////// Material Texture ////////////////////////////////////////////////////////
ID3D11ShaderResourceView* Engine::ResourceManager::Load_Texture(const _wstring& _filePath, _bool _bLinear)
{
	// 중복 텍스쳐 확인
	auto iter = m_mapSRVs.find(_filePath);
	if (iter != m_mapSRVs.end())
	{
		Safe_AddRef(iter->second);
		return iter->second;
	}

	// 새로 로드
	ID3D11ShaderResourceView* pSRV = nullptr;
	HRESULT hr = E_FAIL;

	_tchar drive[MAX_PATH] = {};
	_tchar dir[MAX_PATH] = {};
	_tchar filename[MAX_PATH] = {};
	_tchar ext[MAX_PATH] = {};
	_wsplitpath_s(_filePath.c_str(), drive, MAX_PATH, dir, MAX_PATH, filename, MAX_PATH, ext, MAX_PATH);

	// 바이너리화 한 파일에 등록되어있는 마테리얼의 확장자가 PNG일때 
	// 같은 이름의 DDS를 찾아서 우선으로 등록하고 없으면 PNG를 탐색 
	if (!lstrcmpi(ext, L".png"))
	{
		_tchar ddsPath[MAX_PATH] = {};
		lstrcpy(ddsPath, drive);
		lstrcat(ddsPath, dir);
		lstrcat(ddsPath, filename);
		lstrcat(ddsPath, L".dds");

        hr = CreateDDSTextureFromFile(m_pDevice, ddsPath, nullptr, &pSRV);
        if (FAILED(hr))
        {
            if (_bLinear) //diffuse만 SRGB
            {
                // Linear 텍스처 - sRGB 무시

                hr = CreateWICTextureFromFileEx(m_pDevice, _filePath.c_str(), 0,
                    D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
                    WIC_LOADER_FORCE_SRGB, nullptr, &pSRV);
            }
            else //나머지는 linear
            {
                hr = CreateWICTextureFromFileEx(m_pDevice, _filePath.c_str(), 0,
                    D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
                    WIC_LOADER_IGNORE_SRGB, nullptr, &pSRV);
            }
            //hr = CreateWICTextureFromFile(m_pDevice, _filePath.c_str(), nullptr, &pSRV);
        }
    }
    else if (!lstrcmpi(ext, L".dds"))
    {
        hr = CreateDDSTextureFromFile(m_pDevice, _filePath.c_str(), nullptr, &pSRV);
    }
    else if (!lstrcmpi(ext, L".tga"))
    {
        // TGA는 DirectXTex로 로드
        ScratchImage image = {};
        hr = LoadFromTGAFile(_filePath.c_str(), nullptr, image);
        if (SUCCEEDED(hr))
        {
            hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &pSRV);
        }
    }
    else if (!lstrcmpi(ext, L".hdr"))
    {
        ScratchImage image = {};
        hr = LoadFromHDRFile(_filePath.c_str(), nullptr, image);
        if (SUCCEEDED(hr))
        {
            hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &pSRV);
        }
    }
    else
    {
        // 그 외 (jpg, bmp 등)
        hr = CreateWICTextureFromFile(m_pDevice, _filePath.c_str(), nullptr, &pSRV);
    }

	if (FAILED(hr) || pSRV == nullptr)
		return nullptr;

	// 캐시에 저장
	m_mapSRVs[_filePath] = pSRV;
	Safe_AddRef(pSRV);

	return pSRV;
}

void Engine::ResourceManager::Release_Texture(const _wstring& _filePath)
{
	auto it = m_mapSRVs.find(_filePath);
	if (it == m_mapSRVs.end())
		return;

	Safe_Release(it->second);
	m_mapSRVs.erase(it);
}

_bool Engine::ResourceManager::Release_Texture_Safe(const _wstring& _filePath)
{
	auto it = m_mapSRVs.find(_filePath);
	if (it == m_mapSRVs.end())
		return false;

	it->second->AddRef();
	ULONG refCnt = it->second->Release();

	// 캐시만 들고 있는 경우에만 해제
	if (refCnt == 1)
	{
		Safe_Release(it->second);
		m_mapSRVs.erase(it);
		return true;
	}

	return false;
}
/******************************************************* Material Texture *******************************************************/



////////////////////////////////////////////////////////  Player  ////////////////////////////////////////////////////////
void Engine::ResourceManager::Add_Player(GameObject* _player)
{
	MSG_NULL(_player, L"없는거 넣지마 ㅡㅡ", L"플레이어 없으시대요", );

	if (m_pPlayer)
		COUT("이미 플레이어 있는디");

	m_pPlayer = _player;
	Safe_AddRef(m_pPlayer);
}

void Engine::ResourceManager::Release_Player()
{
	Safe_Release(m_pPlayer);
}

_vector Engine::ResourceManager::Get_PlayerPos_Vector()
{
	/*if (!m_pPlayer)
	{
		return XMVectorZero();
	}*/

	MSG_NULL(m_pPlayer, L"플레이어 없소", L"어허", XMVectorZero());

	return m_pPlayer->Get_Position();
}

_float3 Engine::ResourceManager::Get_PlayerPos_Float3()
{
	//if (!m_pPlayer)
	//	return _float3(0.f, 0.f, 0.f);

	MSG_NULL(m_pPlayer, L"플레이어 없소", L"어허", _float3(0.f, 0.f, 0.f));


	_float3 pos = {};
	XMStoreFloat3(&pos, m_pPlayer->Get_Position());
	return pos;
}

_float4 Engine::ResourceManager::Get_PlayerPos_Float4()
{
	MSG_NULL(m_pPlayer, L"플레이어 없소", L"어허", _float4(0.f, 0.f, 0.f, 0.f));

	_float4 pos = {};
	XMStoreFloat4(&pos, m_pPlayer->Get_Position());
	return pos;
}

const _float4x4* Engine::ResourceManager::Get_Player_Matrix_Ptr()
{
	MSG_NULL(m_pPlayer, L"플레이어 없소", L"어허", nullptr);

	return m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
}
/******************************************************* Player *******************************************************/



////////////////////////////////////////////////////////  Companion  ////////////////////////////////////////////////////////
void Engine::ResourceManager::Add_Companion(GameObject* _companion)
{
	MSG_NULL(_companion, L"없는거 넣지마 ㅡㅡ", L"플레이어 없으시대요", );

	if (m_pCompanion)
		COUT("이미 플레이어 있는디");

	m_pCompanion = _companion;
	Safe_AddRef(m_pCompanion);
}

void Engine::ResourceManager::Release_Companion()
{
	Safe_Release(m_pCompanion);
}

_vector Engine::ResourceManager::Get_CompanionPos_Vector()
{
	MSG_NULL(m_pCompanion, L"동행자 없소", L"어허", XMVectorZero());

	return m_pCompanion->Get_Position();
}

_float3 Engine::ResourceManager::Get_CompanionPos_Float3()
{
	MSG_NULL(m_pCompanion, L"동행자 없소", L"어허", _float3(0.f, 0.f, 0.f));


	_float3 pos = {};
	XMStoreFloat3(&pos, m_pCompanion->Get_Position());
	return pos;
}

_float4 Engine::ResourceManager::Get_CompanionPos_Float4()
{
	MSG_NULL(m_pCompanion, L"동행자 없소", L"어허", _float4(0.f, 0.f, 0.f, 0.f));

	_float4 pos = {};
	XMStoreFloat4(&pos, m_pCompanion->Get_Position());
	return pos;
}

const _float4x4* Engine::ResourceManager::Get_Companion_Matrix_Ptr()
{
	MSG_NULL(m_pCompanion, L"동행자 없소", L"어허", nullptr);

	return m_pCompanion->Get_Transform()->Get_WorldMatrixPtr();
}
/******************************************************* Companion *******************************************************/



////////////////////////////////////////////////////////    ////////////////////////////////////////////////////////
ResourceManager* Engine::ResourceManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ResourceManager* pInstance = new ResourceManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"ResourceManager  ", L"Caution!!!", nullptr);

	return pInstance;
}
/*******************************************************    *******************************************************/



////////////////////////////////////////////////////////    ////////////////////////////////////////////////////////
void Engine::ResourceManager::Free()
{
	__super::Free();

	// 공유 자산인 Shader 모두 제거 
	for (auto& shader : m_umapShader)
	{
		Safe_Release(shader.second);
	}
	m_umapShader.clear();

	// 공유 자산인 Buffer 모두 제거
	for (auto& buffer : m_vecBuffers)
	{
		Safe_Release(buffer);
	}
	m_vecBuffers.clear();

	// 공유 자산인 CS 로컬 버퍼 해제
	for (auto& pair : m_umapLocalBuffers)
	{
		Safe_Release(pair.second);
	}
	m_umapLocalBuffers.clear();

	// Material이 쓰는 공유중이던 Texture 제거
	for (auto& pair : m_mapSRVs)
	{
		Safe_Release(pair.second);
	}
	m_mapSRVs.clear();

	Release_Player();
	Release_Companion();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/*******************************************************    *******************************************************/