#pragma once
#include "Base.h"



//UIAnimTrack->UiAnimClip

//한 장면을 구성, 트랙들의 모임

NS_BEGIN(Engine)
class UIObject;
class ENGINE_DLL UIAnimClip :
    public Base
{
	struct UIAnimClipDesc
	{
		string ClipName = "";
		bool	bLoop = false;//루프설정

	};
private:
	explicit UIAnimClip();
	explicit UIAnimClip(const UIAnimClip& rhs);
	virtual ~UIAnimClip();


public:
	HRESULT Initialize(void* _arg);
	_int Update(_float CurrentTime,const _float fTimeDelta);
	_int Update_Late(_float CurrentTime, const _float fTimeDelta);

	void			Rename(string name) { ClipName = name; }
public:
	HRESULT			Add_AnimTrack(UIAnimTrack* AnimTrack);
	HRESULT			Remove_AnimTrack(_uint iIdx);
public:
	UIAnimClip* Clone();
	void		Compute_TotalDuration(_float* fOut);
	void		Update_TotalDuration();
	vector<UIAnimTrack*>* Get_Tracks() { return &m_AnimTracks; }
public:
					//이 클립에 몇개의 트랙이있는지
	_uint			Get_NumAnimTracks() { return _UINT(m_AnimTracks.size()); }

					//이 클립의 총 길이
	_float			Get_TotalDuration() { return m_fTotalDuration; }


	string			Get_ClipName() { return ClipName; }

	bool			Is_Loop()		{ return m_bLoop; }
	bool			Is_Pingpong() { return m_bPingpoing; }

public:
	HRESULT			Save_AnimClip(string	LocalFolder);
	HRESULT			Load_AnimClip(string	LocalFolder);

public:
	void			Set_Loop(bool b) { m_bLoop = b; }
	void			Set_Owner(UIObject* pOwner);
	void			Set_ClipName(string str) { ClipName = str; }
	void			Set_TotalDuration(_float fTotalDuration) { m_fTotalDuration = fTotalDuration; }
	void			Set_PingPoing(bool b) { m_bPingpoing = b; }
	void			Finish_Clip();	//완전히 끝. 
	void			End_Clip();		//한바퀴돌았을때 무조건실행(loop여도)
	void			Bind_EndFunction(function<void()> Func);
	void			Reset_EndFunction() { m_EndFunction = nullptr; }

public:
					//플레이 시작함수( 값 세팅등..)
	void			Play_Init();
	void			End_Anim();

#pragma region parsing
public:
	ordered_json	Save_Data();
	void			Load_Data(ordered_json& json);

#pragma endregion

public:
	
			//값복사를 통해 원본으로부터 값을 복사받는다.(안에 트랙까지 모두)
	void		Copy_From(UIAnimClip* pSource);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static UIAnimClip* Create(void *arg);
public:
	void Free() override final;



private:
	string					ClipName = "";
	vector<UIAnimTrack*>		m_AnimTracks;
	_float					m_fTotalDuration = 0.f;		//트랙 중 가장 긴 애로 설정(이 클립의 총길이)

	function<void()>			m_EndFunction=nullptr;	//이 애니메이션이 끝나고 수행할 function
	
	bool						m_bLoop = false;
	bool						m_bPingpoing = false;		//end에 도달하면 start로 다시복귀
	
	UIObject* m_pOwner = nullptr;

	static int Number;

#ifdef _DEBUG
public:
	UIAnimClip* Get_OriginalClip() { return pOriginalClip; }
private:
	UIAnimClip* pOriginalClip = nullptr;
#endif // _DEBUG



};
NS_END

