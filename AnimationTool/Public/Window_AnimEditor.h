#pragma once
#include "AnimationTool_Define.h"
#include "Monster_Weapon.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
class Animation;
class Model;
NS_END

NS_BEGIN(AnimationTool)
class Window_AnimEditor final : public ImguiWindow
{
public:
	struct BonePickerName
	{
		Model* pTargetModel;
		vector<_string> vecBoneNames;
	};

private:
	Window_AnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Window_AnimEditor() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;
	virtual _uint       Update_Contents(_float fTimeDelta) override;

private:
	void Draw_PlaybackControl(); //애니메이션 재생 컨트롤
	void Draw_CreateNotify(Model* pModel, Animation* pAnimation);
	void Draw_EditNotify(Model* pModel, Animation* pAnimation);

	_bool Update_AnimToolData(); //이거 다른 윈도우들 접근해서 가져오는 툴용 데이터 갱신용
	void Edit_ModelParts(); //이건 파츠 모델들 편집용
	HRESULT Save_Animation();

	void BoneCache(Model* pModel); //모델마다 뼈 이름 갱신용
	_bool Draw_BonePicker(Model* pModel, _string& InOutSocketName); //뼈 선택 및 검색

	void Event_Edit(ANIMNOTIFY_DESC& AnimDesc);

	//헬퍼함수
	_bool DrawString(const _char* Label, _string& str);
	_bool DrawInt(const _char* Label, _int& fValue, _float fStep = 0.1f);
	_bool DrawFloat(const _char* Label, _float& fValue, _float fStep = 0.1f);
	_bool DrawBool(const _char* Label, _bool& bValue);

private:
	BonePickerName				m_PickModelBoneName; //선택한 모델의 뼈
	ANIMNOTIFY_DESC				m_NotifyDescForAdd = {}; //추가하기 위한 Desc 구조체
	_bool						m_bIntializeNotify = { false }; //생성할때 값 초기화용

	_uint						m_PrevSelectedId = { g_INVALID };
	_char						m_szSocketFillter[64] = ""; //소켓 필터 검색용
	_char						m_szSocketEditBuf[128] = ""; //입력 저장할 버퍼
	_bool						m_bSocketBufSync = { false }; //소켓이름 동기화용
	class Player_Weapon*		m_pPlayerWeapon = { nullptr };
	Monster_Weapon*				m_pMonsterWeapon = { nullptr };
	class Yakumo_Weapon*		m_pYakumoWeapon = { nullptr };
private:
	GameInstance*				m_pGameInstance = {};
	AnimToolData*				m_pAnimToolData = {};
	_bool						m_bActiveWeaponBone = { false };

public:
	static Window_AnimEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;

};

NS_END