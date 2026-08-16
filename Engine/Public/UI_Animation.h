#pragma once
#include "UIComponent.h"
#include "UIAnimClip.h"


NS_BEGIN(Engine)

class ENGINE_DLL UI_Animation:
    public UIComponent
{

protected:
	explicit UI_Animation();
	explicit UI_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Animation(const UI_Animation& original);
	virtual ~UI_Animation();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);
	void		Reset_Animation();


public:
				//기존에 존재하는 클립을 변수로 받은 애로 교체
	void	Change_Clip(UIAnimClip* pClip, _uint iIdx);
	void	Reset_PlayTime() { m_fPlayTime = 0.f; }
	int		Get_CurrentClipIdx() { return m_CurrentClipIdx; }

public:
	bool		Is_Playing() { return m_bIsPlaying; }

public:
	static UI_Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);
public:
#ifdef _DEBUG
	virtual void		Render_Imgui();

	//Imgui에서 사용할 edit한거 잠시 프리뷰로 보는 용도로 쓰는함수
	void		Set_PreviewClip(UIAnimClip* pPreviewClip);


	//Imgui용
	void	Reset_CurrentClipIdx() { m_CurrentClipIdx = -1; }
private:
	void		Render_CurrentAnimationList();

				//브라우저에서 클립선택해서 로드
	void		On_LoadAnimationFile();
	void		On_SaveAnimationFile();

	void		Load_AnimClip_From_FilePath(string path);
	void		Save_AnimClip_From_FilePath(string path);

				//툴에서 클립생성
	void		On_CreateNewClip();
	void		Update_ClipName(_uint idx, string NewName);

public:
	void		Update_All_ClipName();


private:
	vector<string>      m_ClipNames;
	vector<const char*>		m_ComboItems; // ImGui 전용 캐시
	ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui

#endif // _DEBUG

public:
	void		Add_AnimClips(UIAnimClip* Clip);
	
			//특정 애니메이션 클립재생
	HRESULT		 Play_AnimClip(string ClipName);
	HRESULT	     Play_AnimClip(_uint iIdx);
	HRESULT	     Play_AnimClip();

	HRESULT		Stop_Animation();
	UIAnimClip* Get_CurrentClip() { return m_CurrentClip; }
	UIAnimClip* Get_AnimationClip(const string& Name);
#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion
public:
	void Free() override;


private:
	UIAnimClip*					m_CurrentClip=nullptr;//현재 재생할 클립
	int							m_CurrentClipIdx = -1;

	vector<UIAnimClip*>					m_AnimClips;	//실제클립들
	unordered_map<string, UIAnimClip*>	m_MapAnimClips;		//검색용 클립


	bool					m_bIsPlaying = false;

	float					m_fPlayTime = 0.f;		//현재 진행시간


	_float4					vOriginColor = _float4(1.f, 1.f, 1.f, 1.f);


};

NS_END

