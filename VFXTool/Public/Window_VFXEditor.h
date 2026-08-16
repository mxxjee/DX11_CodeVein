#pragma once
#include "ImguiWindow.h"
#include "VIBuffer_Particle.h"
#include "VIBuffer_Trail.h"
#include "SubEmitter.h"
#include "MeshEffect.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class GameInstance;
class ParticleSystem;
class ParticleEffect;
class Model;
NS_END

NS_BEGIN(VFXTool)

/* Selection Type for VFX Editor */
enum class SELECTION_TYPE
{
	NONE,
	PARTICLE_SYSTEM,
	PARTICLE_EFFECT,
	TRAIL_EFFECT,
	MESH_EFFECT,
	SUBEMITTER_ENTRY
};

class Window_VFXEditor final : public ImguiWindow
{
private:
	Window_VFXEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Window_VFXEditor() = default;

public: 
	void Set_Parser(class VFX_Parsing* pParser);

private:
	_string ConvertToRelativePath(const _wstring& wstrFilePath);

private:
	// Particle Properties
	// Texture
	_wstring	m_strTexturePath = {};				// 전체 경로
	_wstring	m_strTextureName = {};				// 텍스처 파일명
	_wstring	m_strAlphaMaskTexturePath = {};		// 알파마스크 텍스처
	_wstring	m_strAlphaMaskTextureName = {};		// 알파마스크 텍스처
	_wstring	m_strMaskTexturePath = {};			// 마스크 텍스처
	_wstring	m_strMaskTextureName = {};			// 마스크 텍스처
	_wstring	m_strNoiseTexturePath = {};			// 노이즈 텍스처
	_wstring	m_strNoiseTextureName = {};			// 노이즈 텍스처

	// Shader
	_wstring				m_strShaderTag = {};			// 일반 쉐이더 태그
	vector<_string>			m_vecShaderPrototypes;			// 일반 쉐이더 목록
	_wstring				m_strComputeShaderTag = {};		// 컴퓨트 쉐이더 태그
	vector<_string>			m_vecComputeShaderPrototypes;	// 컴퓨트 쉐이더 목록
	_int					m_iPassIndex = {};

	GameInstance*								m_pGameInstance = { nullptr };
	VIBuffer_Particle::PARTICLE_DESC			m_tParticleDesc{};

	class BasicParticle*						m_pEffect = { nullptr };
	class VFX_Parsing*							m_pParser = { nullptr };

	/* ============================== */
	/*     VFX Editor New Members     */
	/* ============================== */

	// Selection 상태
	SELECTION_TYPE		m_eSelectionType = SELECTION_TYPE::NONE;
	_int				m_iSelectedSystemIndex = -1;
	_int				m_iSelectedEffectIndex = -1;
	_int				m_iSelectedSubEmitterIndex = -1;

	// 선택된 포인터 (캐시용)
	ParticleSystem*		m_pSelectedSystem = nullptr;
	ParticleEffect*		m_pSelectedEffect = nullptr;

	// SubEmitter 편집용 캐시
	SubEmitter::SUBEMITTER_DESC		m_tSubEmitterDesc{};

	// UI Popup 상태
	_bool				m_bShowAddSystemPopup = false;
	_bool				m_bShowAddEffectPopup = false;
	_bool				m_bShowAddSubEmitterPopup = false;
	_bool				m_bShowAddTrailEffectPopup = false;
	_bool				m_bShowAddMeshEffectPopup = false;
	_char				m_szNewName[128] = {};

	// ParticleSystem 편집용 캐시
	_bool				m_bCachedAutoPlay = false;
	_bool				m_bCachedLoop = false;
	_float				m_fCachedTotalDuration = 5.f;
	_float				m_fCachedDelayedTime = {};
	_float3				m_vSystemRotation = {};

	// ParticleEffect 편집용 캐시
	_bool				m_bUseEdgeGlow = {};
	_bool				m_bCachedEffectLoop = {};
	_bool				m_bCachedBackCulling = {};
	_bool				m_bCachedEmissive = {};
	_uint				m_iMaskSampler = {};
	_float				m_fCachedStartDelay = {};
	_float				m_fCachedMaskIntensity = {};
	_float3				m_vCachedRotation = {};
	_float2				m_vCachedDiffuseScroll = {};
	_float2				m_vCachedFadeInOut= {};
	_float2				m_vCachedEffectOffset = {};

	// TrailEffect 편집용 캐시
	VIBuffer_Trail::TRAIL_DESC		m_tTrailDesc{};

	// MeshEffect 편집용 캐시
	MeshEffect::MESHEFFECT_DESC		m_tMeshDesc{};
	_float3							m_vMeshPosition = {};
	_float3							m_vMeshScale = {};
	_float3							m_vMeshRotation = {};
	_float							m_fMeshLifeTime = 1.f;
	_bool							m_bMeshUseEdgeGlow = {};
	_wstring						m_wstrModelTag = {};
	_float3							m_vHotColor = { 1.f, 1.f, 0.3f };
	_float							m_fHotPower = { 2.f };

	// ParticleSystem 목록 (에디터에서 관리)
	vector<ParticleSystem*>		m_vecParticleSystems;

	/* ============================== */
	/*       UI Render Functions      */
	/* ============================== */
private:
	void Render_TopToolbar();
	void Render_TreeView();
	void Render_DetailPanel();

	void Render_Detail_ParticleSystem();
	void Render_Detail_ParticleEffect();
	void Render_Detail_SubEmitter();
	void Render_Detail_TrailEffect();
	void Render_Detail_MeshEffect();

	void Render_AddSystemPopup();
	void Render_AddEffectPopup();
	void Render_AddSubEmitterPopup();
	void Render_AddTrailEffectPopup();
	void Render_AddMeshEffectPopup();

public:
	virtual _uint Update_Contents(_float fTimeDelta) override;
	static Window_VFXEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

NS_END