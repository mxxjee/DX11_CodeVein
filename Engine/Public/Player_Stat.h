#pragma once
#include "Stat.h"

NS_BEGIN(Engine)
class ENGINE_DLL Player_Stat : public Stat
{
public:
	typedef struct tagPlayerStatDesc : public Stat::STAT_DESC
	{
		_uint iLevel = 1;//현재레벨

		_float fCurrentStamina = {};
		_float fMaxStamina = {};
		_float fLowStamina = {}; //경고표시가 켜질때를 판단하기 위한 스태미너값

		BLOODCODE eBloodCode = {}; //이거 추후에 직업으로 빼야하나 ? 

		_float iCurrentMyeonghyeol = {}; //명혈
		_float iMaxMyeonghyeol = {}; //최대 명혈

		_int iSTR = {}; //완력
		_int iDEX = {}; //재주
		_int iMND = {}; //정신
		_int iWIL = {}; //의지
		_int iVIT = {}; //활력
		_int iFOR = {}; //인내

		

	}PLAYERSTAT_DESC;

private:
	explicit Player_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Stat(const Player_Stat& original);
	virtual ~Player_Stat() = default;

public:
	_float Get_Stamina() const { return m_PlayerStat.fCurrentStamina; }
	_float Get_MaxStamina() const { return m_PlayerStat.fMaxStamina; }

	//UI 넘겨주기 위한 포인터반환
	_float* Get_StaminaPtr() { return &m_PlayerStat.fCurrentStamina; }
	_float* Get_MaxStaminaPtr() { return &m_PlayerStat.fMaxStamina; }

	_float Get_LowStamina() const { return m_PlayerStat.fLowStamina; }
	BLOODCODE Get_BloodCode() const { return m_PlayerStat.eBloodCode; }
	_float Get_Myeonghyeol() const { return m_PlayerStat.iCurrentMyeonghyeol; }
	_float Get_MaxMyeonghyeol() const { return m_PlayerStat.iMaxMyeonghyeol; }
	_int Get_STR() const { return m_PlayerStat.iSTR; }
	_int Get_DEX() const { return m_PlayerStat.iDEX; }
	_int Get_MND() const { return m_PlayerStat.iMND; }
	_int Get_WIL() const { return m_PlayerStat.iWIL; }
	_int Get_VIT() const { return m_PlayerStat.iVIT; }
	_int Get_FOR() const { return m_PlayerStat.iFOR; }

	_uint Get_Level() const { return m_PlayerStat.iLevel; }
	void	Set_Level(_uint iLevel) { m_PlayerStat.iLevel = iLevel; }

	void	Decrease_Stamina(_float fStamina); //스태미나 감소용
	void	ContinuousDecrease_Stamina(_float fDecrasePerSecond, _float fTimeDelta); //이건 Dash 와 같은 지속감소용
	void	Regen_Stamina(_float fRegenPerSecond, _float fTimeDelta); //스태미나 리젠용
	void	FullRestore_Stamina(); //스태미나 완전 회복 
	

	void	Apply_Myeonghyeol(_float iMyeonghyeol);
	void	Update_UIEvent();

	void	Set_DamageMultiplier(_float fMultiplier) { m_fDamageMultiplier = fMultiplier; }
	_float	Get_DamageMultiplier() const { return m_fDamageMultiplier; }

	void    Set_RoyalHeart(_bool bActive, _float fChance) { m_bRoyalHeartActive = bActive, m_fRoyalHeartChance = fChance; }
	_bool	Check_RoyalHeartSuperArmor();

	void	Set_Enchant(_bool bActive, _float2 vAddDamage) { m_bEnchantActive = bActive, m_vAdditionalDamage = vAddDamage; }
	_bool	Get_EnchnatActive() const { return m_bEnchantActive; }

	_float 	Apply_EnchantDamage();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;

	void		LevelUp(_uint iLevel, _uint iMaxHp, _uint iMaxST, _uint iAttack);
private:
	PLAYERSTAT_DESC m_PlayerStat = {};
	//스트라이크 라이저 데미지 배율
	_float			m_fDamageMultiplier = {1.0f};
	//로얄하트 활성
	_bool			m_bRoyalHeartActive = { false }; 
	_float			m_fRoyalHeartChance = {}; //로얄하트 일때 슈퍼아머 판정 확률 
	//인첸트 스킬
	_float2			m_vAdditionalDamage = {}; //추가 데미지 랜덤 value 사용예정
	_bool			m_bEnchantActive = { false };

public:
	static Player_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END
