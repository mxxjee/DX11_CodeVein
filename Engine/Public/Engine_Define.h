#ifndef Engine_Define_h__
#define Engine_Define_h__

#ifndef NOMINMAX
#define NOMINMAX
#endif


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/cooking/PxCooking.h>

namespace physx {}
using namespace physx;

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXTK/SimpleMath.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

using namespace std;

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <assimp/Exporter.hpp>

#include <d3dcompiler.h>

using namespace DirectX;
using namespace SimpleMath;
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include <array>
#include <typeindex>
#include <locale>
#include <variant>
#include <queue>
#include <deque>
#include <random> //SSAO용으로 추가

#include <unordered_map>
#include <ctime>
#include <Json/json.hpp>
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using SoundID = size_t;

//For LoadFiles
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;


#include <json.hpp>
#include <stack>

#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_TextMacro.h"
#include "Model_Struct.h"
#include "Engine_SimpleMath_Wrapping.h"
#include "Event_Define.h"
#include "FileParsing_Data.h"


namespace Engine
{
	ENGINE_DLL extern bool g_bPrintRefCnt;
	inline static const unsigned int g_iMaxNumBones = 512;
	inline static const unsigned int g_INVALID = UINT_MAX; //유효하지않은값들 초기화해서 예외처리하려고
	inline static const unsigned int g_iReadbackBone = 8;
	ENGINE_DLL extern bool g_bDrawRTV;
	ENGINE_DLL extern bool g_bAppActive;
	ENGINE_DLL extern bool g_bDrawDebugCollider;
	ENGINE_DLL extern bool g_bOmniPVDRecording;
	ENGINE_DLL extern bool g_bPhysXDebug;
	ENGINE_DLL extern HWND eg_hWnd;
	ENGINE_DLL extern TOOLTYPE g_toolType;
	ENGINE_DLL extern _float3 g_vHDRColor;
	ENGINE_DLL extern _bool g_bClient;
	
	extern _float Engine_WINCX;
	extern _float Engine_WINCY;

	// 쉐이더 편하게 쓰자!
	/* Buffer_Camera (b0) */
	EXTERN_SHADER_HANDLE(g_ViewMatrix);
	EXTERN_SHADER_HANDLE(g_ProjMatrix);
	EXTERN_SHADER_HANDLE(g_InverseViewMatrix);
	EXTERN_SHADER_HANDLE(g_InverseProjMatrix);
	EXTERN_SHADER_HANDLE(g_vCamPosition);

	/* Buffer_Object (b1); */
	EXTERN_SHADER_HANDLE(g_WorldMatrix);
	EXTERN_SHADER_HANDLE(g_PrevWorldMatrix);
	EXTERN_SHADER_HANDLE(g_vColor);
	EXTERN_SHADER_HANDLE(g_fProgress);
	EXTERN_SHADER_HANDLE(g_ObjectID);
	EXTERN_SHADER_HANDLE(g_bitFlag);
	EXTERN_SHADER_HANDLE(g_fTime);
	EXTERN_SHADER_HANDLE(g_fRimOn);
	EXTERN_SHADER_HANDLE(g_vEmissionUVPos);

	/* Buffer_Lights (b2); */
	EXTERN_SHADER_HANDLE(g_fLightRange);
	EXTERN_SHADER_HANDLE(g_vLightDirection);
	EXTERN_SHADER_HANDLE(g_vLightPosition);
	EXTERN_SHADER_HANDLE(g_vLightDiffuse);
	EXTERN_SHADER_HANDLE(g_vLightAmbient);
	EXTERN_SHADER_HANDLE(g_vLightSpecular);
	EXTERN_SHADER_HANDLE(g_LightViewMatrix);
	EXTERN_SHADER_HANDLE(g_LightProjMatrix);
	EXTERN_SHADER_HANDLE(g_fSpotInnerCone);
	EXTERN_SHADER_HANDLE(g_fSpotOuterCone);
	EXTERN_SHADER_HANDLE(lights_vDiffuse);
	EXTERN_SHADER_HANDLE(lights_vAmbient);
	EXTERN_SHADER_HANDLE(lights_vSpecular);
	EXTERN_SHADER_HANDLE(lights_vPosition);
	EXTERN_SHADER_HANDLE(lights_vRangeAndType);
	EXTERN_SHADER_HANDLE(lightCount);


	/* Buffer_Player (b3); */
	EXTERN_SHADER_HANDLE(g_vPlayerInnerColor);
	EXTERN_SHADER_HANDLE(g_fPlayerColorStrength);
	EXTERN_SHADER_HANDLE(g_vPlayerHairColor);
	EXTERN_SHADER_HANDLE(g_fPlayerGeneralShift);
	EXTERN_SHADER_HANDLE(g_fPlayerHighlight1Shift);
	EXTERN_SHADER_HANDLE(g_fPlayerHighlight1Strength);
	EXTERN_SHADER_HANDLE(g_fPlayerHighlight2Shift);
	EXTERN_SHADER_HANDLE(g_fPlayerHighlight2Strength);
	EXTERN_SHADER_HANDLE(g_fPlayerHighlightPower);
	EXTERN_SHADER_HANDLE(g_fPlayerContrast);
	EXTERN_SHADER_HANDLE(g_fPlayerMetallic);
	EXTERN_SHADER_HANDLE(g_fPlayerRoughnessMin);
	EXTERN_SHADER_HANDLE(g_vPlayerEyeBaseColor);
	EXTERN_SHADER_HANDLE(g_vPlayerEyeAppendColor);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeScale);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeIndividualScaleOffset);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeOffsetU);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeOffsetV);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeCastsEye);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeCentering);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeIrisStrength);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeHighlightStrength);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeIrisScaleX);
	EXTERN_SHADER_HANDLE(g_fPlayerEyeIrisScaleY);
	EXTERN_SHADER_HANDLE(g_vPlayerBrowColor);
	EXTERN_SHADER_HANDLE(g_vPlayerEyeLineColor);
	EXTERN_SHADER_HANDLE(g_vPlayerBrowUVOffset);
	EXTERN_SHADER_HANDLE(g_fPlayerBrowUVScale);
	EXTERN_SHADER_HANDLE(g_iPlayerUseSecondUV);
	EXTERN_SHADER_HANDLE(g_fPlayerBrowUVRotation);
	EXTERN_SHADER_HANDLE(g_fPlayerBrowXGap);

	/* Buffer_Bones (b4); */
	EXTERN_SHADER_HANDLE(g_BonesMatrices);

	/* Buffer_Morph (b5); */
	EXTERN_SHADER_HANDLE(g_MorphWeightsPacked);
	EXTERN_SHADER_HANDLE(g_MorphIndicesPacked);
	EXTERN_SHADER_HANDLE(g_NumActiveMorphs);
	EXTERN_SHADER_HANDLE(g_NumVertices);

	/* Buffer_LocalAnim (b6); */
	EXTERN_SHADER_HANDLE(g_LiAnimIndex);
	EXTERN_SHADER_HANDLE(g_LiNumBones);
	EXTERN_SHADER_HANDLE(g_LiRootBoneIndex);
	EXTERN_SHADER_HANDLE(g_LbAnimLoop);
	EXTERN_SHADER_HANDLE(g_LfCurrentFrame);
	EXTERN_SHADER_HANDLE(g_LfLerpRatio);
	EXTERN_SHADER_HANDLE(g_LbLerping);
	EXTERN_SHADER_HANDLE(g_LbRemoveRootTranslation);
	EXTERN_SHADER_HANDLE(g_LiUpperAnimIndex);
	EXTERN_SHADER_HANDLE(g_LbUpperAnimLoop);
	EXTERN_SHADER_HANDLE(g_LfUpperCurrentFrame);
	EXTERN_SHADER_HANDLE(g_LbUpperRemoveRootTranslation);
	EXTERN_SHADER_HANDLE(g_LbUpperBodyBlendEnalbe);
	EXTERN_SHADER_HANDLE(g_LfUpperLayerWeight);
	EXTERN_SHADER_HANDLE(g_LfUpperLerpRatio);
	EXTERN_SHADER_HANDLE(g_LbUpperLerping);

	/* Buffer_Bone (b7); */
	EXTERN_SHADER_HANDLE(g_NumBones);
	EXTERN_SHADER_HANDLE(g_CurrentDepth);
	EXTERN_SHADER_HANDLE(g_PreTransformMatrix);

	/* Buffer_Decal (b8); */
	EXTERN_SHADER_HANDLE(g_InvDecalWorldMatrix);
	EXTERN_SHADER_HANDLE(g_fDecalAlpha);
	EXTERN_SHADER_HANDLE(g_iMaskIndex);

	/* Buffer_Particle (b8 / Compute_Shader); */
	EXTERN_SHADER_HANDLE(g_vPivot);
	EXTERN_SHADER_HANDLE(g_fTimeDelta);
	EXTERN_SHADER_HANDLE(g_iNumParticles);
	EXTERN_SHADER_HANDLE(g_bIsLoop);
	EXTERN_SHADER_HANDLE(g_fAccumulatedTime);
	EXTERN_SHADER_HANDLE(g_fShakeStrength);
	EXTERN_SHADER_HANDLE(g_matParticleWorld);
	EXTERN_SHADER_HANDLE(g_matPrevParticleWorld);
	EXTERN_SHADER_HANDLE(g_bFollowParent);
	EXTERN_SHADER_HANDLE(g_fConvergeMaxDist);

	/* Buffer_Effect (b9); */
	EXTERN_SHADER_HANDLE(g_EffectWorldMatrix);
	EXTERN_SHADER_HANDLE(g_vScaleUV);
	EXTERN_SHADER_HANDLE(g_fTransparency);
	EXTERN_SHADER_HANDLE(g_fAlpha);
	EXTERN_SHADER_HANDLE(g_fDistortionStrength);
	EXTERN_SHADER_HANDLE(g_bIsBillboard);
	EXTERN_SHADER_HANDLE(g_iFrameCount);
	EXTERN_SHADER_HANDLE(g_iCountX);
	EXTERN_SHADER_HANDLE(g_iCountY);
	EXTERN_SHADER_HANDLE(g_iTransparentIndex);
	EXTERN_SHADER_HANDLE(g_bUseMask);
	EXTERN_SHADER_HANDLE(g_bRadialRotation);
	EXTERN_SHADER_HANDLE(g_vEffectPivot);
	EXTERN_SHADER_HANDLE(g_iMaskStartFrame);
	EXTERN_SHADER_HANDLE(g_iMaskFrameCount);
	EXTERN_SHADER_HANDLE(g_iMaskCountX);
	EXTERN_SHADER_HANDLE(g_iMaskCountY);
	EXTERN_SHADER_HANDLE(g_vMaskUVScroll);
	EXTERN_SHADER_HANDLE(g_fParticleScale);
	EXTERN_SHADER_HANDLE(g_bUseNoise);
	EXTERN_SHADER_HANDLE(g_bVerticalFade);
	EXTERN_SHADER_HANDLE(g_bUseTexture);
	EXTERN_SHADER_HANDLE(g_bUseAlphaMask);
	EXTERN_SHADER_HANDLE(g_bUseEdgeGlow);
	EXTERN_SHADER_HANDLE(g_fGlowStrength);
	EXTERN_SHADER_HANDLE(g_fLifeTime);
	EXTERN_SHADER_HANDLE(g_fTimeElapsed);
	EXTERN_SHADER_HANDLE(g_vDiffuseUVScroll);
	EXTERN_SHADER_HANDLE(g_vMaskUVScale);
	EXTERN_SHADER_HANDLE(g_vFadeInOut);
	EXTERN_SHADER_HANDLE(g_fMaskIntensity);
	EXTERN_SHADER_HANDLE(g_vMaskUVStartOffset);
	EXTERN_SHADER_HANDLE(g_iMaskSampler);
	EXTERN_SHADER_HANDLE(g_bDirectionalDissolve);
	EXTERN_SHADER_HANDLE(g_bDissolveFlipY);
	EXTERN_SHADER_HANDLE(g_fNoiseWeight);
	EXTERN_SHADER_HANDLE(g_fDissolveEdgeWidth);
	EXTERN_SHADER_HANDLE(g_vDissolveEdgeColor);
	EXTERN_SHADER_HANDLE(g_bVerticalShrink);
	EXTERN_SHADER_HANDLE(g_fDisplaceStrength);
	EXTERN_SHADER_HANDLE(g_vHotColor);
	EXTERN_SHADER_HANDLE(g_fHotColor);
	EXTERN_SHADER_HANDLE(g_iDirectionalScale);
	EXTERN_SHADER_HANDLE(g_bEmissive);
	EXTERN_SHADER_HANDLE(g_vEffectOffset);

	/* Buffer_UI (b10); */
	EXTERN_SHADER_HANDLE(g_UIColor);
	EXTERN_SHADER_HANDLE(g_UVScale);
	EXTERN_SHADER_HANDLE(g_UVOffSet);
	EXTERN_SHADER_HANDLE(g_UVScroll);
	EXTERN_SHADER_HANDLE(g_AlphaOffSet);
	EXTERN_SHADER_HANDLE(g_Reserved);
	EXTERN_SHADER_HANDLE(g_TextureSize);
	EXTERN_SHADER_HANDLE(g_OutLineThickness);
	EXTERN_SHADER_HANDLE(g_UIProgress);
	EXTERN_SHADER_HANDLE(g_UIPadding);
	EXTERN_SHADER_HANDLE(g_OutLineColor);
	EXTERN_SHADER_HANDLE(g_ScrollTime);

	/* Buffer_Readback (b12); */
	EXTERN_SHADER_HANDLE(g_iReadbackIndices);
	EXTERN_SHADER_HANDLE(g_iReadbackCount);

	//Buffer_DarpeBone(CS)
	EXTERN_SHADER_HANDLE(g_iDrapeDeltaCount);

	/* FinalBuffer(b13) */
	EXTERN_SHADER_HANDLE(g_vPlayerUV);
	EXTERN_SHADER_HANDLE(g_vCenterUV);
	EXTERN_SHADER_HANDLE(g_vTargetUV);
	EXTERN_SHADER_HANDLE(g_isClipping);
	EXTERN_SHADER_HANDLE(g_fRotation);
	EXTERN_SHADER_HANDLE(g_fZoom);
	EXTERN_SHADER_HANDLE(g_fIconScale);
	EXTERN_SHADER_HANDLE(g_fCameraAngle);


	EXTERN_SHADER_HANDLE(g_fLightMultiplier);
	EXTERN_SHADER_HANDLE(g_fAmbientStrength);
	EXTERN_SHADER_HANDLE(g_vToonShadowRange);
	EXTERN_SHADER_HANDLE(g_vToonBrightnessRange);
	EXTERN_SHADER_HANDLE(g_vAmbientFloorMin);
	EXTERN_SHADER_HANDLE(g_vSSSColor);
	EXTERN_SHADER_HANDLE(g_fBackFillStrength);
	EXTERN_SHADER_HANDLE(g_vSkinTint);
	EXTERN_SHADER_HANDLE(g_fSSSPower);
	EXTERN_SHADER_HANDLE(g_fSSSIntensity);
	EXTERN_SHADER_HANDLE(g_fSkinRoughnessMin);
	EXTERN_SHADER_HANDLE(g_fRimPower);
	EXTERN_SHADER_HANDLE(g_fRimIntensity);
	EXTERN_SHADER_HANDLE(g_vRimMaskRange);
	EXTERN_SHADER_HANDLE(g_vSpecBoostRange);
	EXTERN_SHADER_HANDLE(g_fSpecularIntensity);
	EXTERN_SHADER_HANDLE(g_fMinDiffuse);
	EXTERN_SHADER_HANDLE(g_fKValue);
	EXTERN_SHADER_HANDLE(g_fMtrlRoughnessMin);
	EXTERN_SHADER_HANDLE(g_ShadowSplit);
	EXTERN_SHADER_HANDLE(g_ShadowDir);
	EXTERN_SHADER_HANDLE(g_ShadowCascade_MixMaxtrix);
	EXTERN_SHADER_HANDLE(g_Shadow_ViewProjMatrix);
	EXTERN_SHADER_HANDLE(g_SSAOOption);


	EXTERN_SHADER_HANDLE(g_vShadowTintColor);
	EXTERN_SHADER_HANDLE(g_fShadowTintWeight);
	EXTERN_SHADER_HANDLE(g_vMidtoneTintColor);
	EXTERN_SHADER_HANDLE(g_fMidtoneTintWeight);
	EXTERN_SHADER_HANDLE(g_vHighlightTintColor);
	EXTERN_SHADER_HANDLE(g_fHighlightTintWeight);
	EXTERN_SHADER_HANDLE(g_fShadowRange);
	EXTERN_SHADER_HANDLE(g_fHighLightRange);
	EXTERN_SHADER_HANDLE(g_fEnableColorGrading);
	EXTERN_SHADER_HANDLE(g_FogColor);
	EXTERN_SHADER_HANDLE(g_bFogEnable);
	EXTERN_SHADER_HANDLE(g_fFogStartDist);
	EXTERN_SHADER_HANDLE(g_fFogEndDist);
	EXTERN_SHADER_HANDLE(g_fFogDensity);
	EXTERN_SHADER_HANDLE(g_fogType);
	EXTERN_SHADER_HANDLE(g_fFogBaseHeight);
	EXTERN_SHADER_HANDLE(g_fFogFadeHeight);
	EXTERN_SHADER_HANDLE(g_fHeightFogIntensity);
	EXTERN_SHADER_HANDLE(g_fFogIntensity);
	EXTERN_SHADER_HANDLE(g_HDROption);
	EXTERN_SHADER_HANDLE(g_fSpecSoftClamp);
	EXTERN_SHADER_HANDLE(g_vBackFaceShaowRange);
	EXTERN_SHADER_HANDLE(g_fShadowBright);
	EXTERN_SHADER_HANDLE(g_fShadowBackBright);
	EXTERN_SHADER_HANDLE(g_ShadowCascadeNum);
	EXTERN_SHADER_HANDLE(g_ShadowCascade_ViewProjMatrix);

	// Ease 이름으로 호출하는 함수
	inline _float ApplyEase(_float t, EASE_TYPE eType)
	{
		switch (eType)
		{
		case EASE_TYPE::LINEAR:          return t;
		case EASE_TYPE::SMOOTHSTEP:      return SmoothStep(t);
		case EASE_TYPE::SMOOTHERSTEP:    return SmootherStep(t);
		case EASE_TYPE::EASE_IN_QUAD:    return EaseInQuad(t);
		case EASE_TYPE::EASE_OUT_QUAD:   return EaseOutQuad(t);
		case EASE_TYPE::EASE_IN_OUT_QUAD: return EaseInOutQuad(t);
		case EASE_TYPE::EASE_IN_CUBIC:   return EaseInCubic(t);
		case EASE_TYPE::EASE_OUT_CUBIC:  return EaseOutCubic(t);
		case EASE_TYPE::EASE_IN_OUT_CUBIC: return EaseInOutCubic(t);
		case EASE_TYPE::EASE_OUT_ELASTIC: return EaseOutElastic(t);
		case EASE_TYPE::EASE_OUT_BACK:   return EaseOutBack(t);
		case EASE_TYPE::EASE_OUT_BOUNCE: return EaseOutBounce(t);
		default:                         return t;
		}
	}

	namespace CinematicPreset
	{
		inline vector<CINEMATIC_KEYFRAME> Load(const string& filePath)
		{
			vector<CINEMATIC_KEYFRAME> keyframes;
			ifstream file(filePath);
			if (!file.is_open())
				return keyframes;
			json root;
			file >> root;
			file.close();
			const auto& keyArray = root["keyframes"];
			for (const auto& k : keyArray)
			{
				CINEMATIC_KEYFRAME key = {};
				key.eMode = CAST(KEYFRAME_MODE)(k.value("mode", 0));
				key.eEaseType = CAST(EASE_TYPE)(k.value("ease", 0));
				key.fDuration = k.value("duration", 0.f);
				key.fFov = k.value("fov", 0.f);
				if (k.contains("posOffset"))
				{
					key.vPosOffset.x = k["posOffset"][0];
					key.vPosOffset.y = k["posOffset"][1];
					key.vPosOffset.z = k["posOffset"][2];
				}
				key.fPitchOffset = k.value("pitchOffset", 0.f);
				key.fYawOffset = k.value("yawOffset", 0.f);
				if (k.contains("absPosition"))
				{
					key.vAbsPosition.x = k["absPosition"][0];
					key.vAbsPosition.y = k["absPosition"][1];
					key.vAbsPosition.z = k["absPosition"][2];
				}
				if (k.contains("lookAtTarget"))
				{
					key.vlookAtTarget.x = k["lookAtTarget"][0];
					key.vlookAtTarget.y = k["lookAtTarget"][1];
					key.vlookAtTarget.z = k["lookAtTarget"][2];
				}

				// 수정됨 : eLookAt enum 로딩 + 구버전 호환
				if (k.contains("lookAt"))
				{
					key.eLookAt = CAST(CINEMATIC_LOOKAT)(k.value("lookAt", 0));
				}
				else if (k.contains("lookAtPlayer"))
				{
					// 추가됨 : 구버전 JSON 호환 (bool -> enum 변환)
					key.eLookAt = k.value("lookAtPlayer", false)
						? CINEMATIC_LOOKAT::PLAYER
						: CINEMATIC_LOOKAT::CUSTOM_TARGET;
				}

				if (k.contains("events"))
				{
					for (const auto& evJson : k["events"])
					{
						CINEMATIC_EVENT ev = {};
						ev.eType = CAST(CINEMATIC_EVENT_TYPE)(evJson.value("type", 0));
						ev.fTriggerTime = evJson.value("triggerTime", 0.f);
						ev.bFired = false;
						if (evJson.contains("shakePayload"))
						{
							CameraShake shake = {};
							const auto& s = evJson["shakePayload"];
							shake.fDuration = s.value("duration", 0.f);
							shake.fAmpX = s.value("ampX", 0.f);
							shake.fAmpY = s.value("ampY", 0.f);
							shake.fAmpZ = s.value("ampZ", 0.f);
							shake.fFreqX = s.value("freqX", 0.f);
							shake.fFreqY = s.value("freqY", 0.f);
							shake.fFreqZ = s.value("freqZ", 0.f);
							shake.fAmpPitch = s.value("ampPitch", 0.f);
							shake.fAmpYaw = s.value("ampYaw", 0.f);
							shake.fFreqPitch = s.value("freqPitch", 0.f);
							shake.fFreqYaw = s.value("freqYaw", 0.f);
							shake.fAmpFov = s.value("ampFov", 0.f);
							shake.fFreqFov = s.value("freqFov", 0.f);
							shake.fKickStrength = s.value("kickStrength", 0.f);
							shake.fKickDecay = s.value("kickDecay", 0.f);
							shake.fBlendOutTime = s.value("blendOut", 0.f);
							ev.tPayload = shake;
						}
						if (evJson.contains("controlPayload"))
						{
							CINEMATIC_CHARACTER_CONTROLL control = {};
							const auto& c = evJson["controlPayload"];
							control.iNumber = c.value("number", 0u);
							if (c.contains("position"))
							{
								control.vPosition.x = c["position"][0];
								control.vPosition.y = c["position"][1];
								control.vPosition.z = c["position"][2];
							}
							ev.tPayload = control;
						}
						key.vecEvents.push_back(ev);
					}
				}
				keyframes.push_back(key);
			}
			return keyframes;
		}
	}
}

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)
#pragma warning(disable : 26495)
#pragma warning(error: 4706)
#pragma warning(default : 4996)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif


using namespace Engine;

#endif // Engine_Define_h__
