#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL Stat : public Component
{
public: 
	typedef struct tagStatDesc //공통 스탯
	{
		_float	fMaxHp = {}; //최대 체력
		_float	fCurrentHp = {}; //현재 체력

		_int	iDefense = {}; //방어력
		_int	iAttack = {}; //공격력 

		_int	iHaze = {}; //몬스터 사망시 드랍 + 플레이어의 레벨업 + 장비구매 + 연혈 습득에 사용

		_float	fCurrentFocusGauge = {}; //집중 게이지 (민지가 사용하던 변수 이름 :fCurrentGuard)
			   
		_float	fMaxFocusGauge = {}; //집중 최댓값 정해놔서 iFocusGauge가 iMaxFocusGuage 보다 같거나 크면 특수 패턴(애니메이션) 발동하면 될듯 ?

		//충전 조건 : (적 공격 히트박스 직전에 회피 성공 시 가장 많이 증가 ),(피격 시 데이지 비례 증가) ,(일부 패시브 연혈이나 Blood Code효과로 추가 증가), 플레이어는가드까지
		//감소 조건 : 게이지가 절반 이상 차면 시간 경과로 천천히 감소,플레이어는 근접 공격이나 드레입 공격 시 추가 감소 
		//집중 상태 발동 효과 : 플레이어는 스태미나 즉시 FULL회복, 경직당하기 어려워짐, 특수 띄우기 공격(Lauch Attack 사용 가능) 
		//몬스터는 경직 당하기 어려워짐, 플레이어를 경직시키기 쉬워짐, 강력한 특수 공격 발동
		
		bool		m_bPlayer = false;		//플레이어판단을 위한 변수, 플레이어만 따로 true로설정
	}STAT_DESC;

protected:
	explicit Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Stat(const Stat& original);
	virtual ~Stat() = default;

	//공통으로 사용하는 값들은 부모에서 함수 구현해서 공통으로 사용하게 / 자식들 고유 스탯은 자식에서 함수 구현하면됨
	//공통으로 사용되는 함수들은 더하기 + 감소 구분해서 함수 구현함 알기 쉽게 / 자식들은 알아서 구현 ㄱㄱ 
public:
	//UI에 넘겨주기 위한 포인터 반환
	_float*	Get_HpPtr()  { return &m_Stat.fCurrentHp; }
	_float*	Get_MaxHpPtr()  { return &m_Stat.fMaxHp; }
	_float*	Get_FocusGaugePtr()  { return &m_Stat.fCurrentFocusGauge; }
	_float*	Get_MaxFocusGaugePtr()  { return &m_Stat.fMaxFocusGauge; }
		  
	_float	Get_Hp() const { return m_Stat.fCurrentHp; }
	_float	Get_MaxHp() const { return m_Stat.fMaxHp; }
	_int	Get_Defense() const { return m_Stat.iDefense; }
	_int	Get_Attack() const { return m_Stat.iAttack; }
	_int	Get_Haze() const { return m_Stat.iHaze; }
	_float	Get_FocusGauge() const { return m_Stat.fCurrentFocusGauge; }
	_float	Get_MaxFocusGauge() const { return m_Stat.fMaxFocusGauge; }
	_bool   Get_IsMaxFocusGauge() const { return m_bIsMaxFoucsGauge; }

	void	Set_IsMaxFocusGauge(_bool bFoucsGauge) { m_bIsMaxFoucsGauge = bFoucsGauge; } 	//증가 시킬때만 m_bIsMaxFocusGauge를 체크하는데 꽉차서 집중상태가 되었을때는 증가시키면 안되기 때문에 직접 false로 만들어주려고
	void	Set_FocusState(_bool bFocusState) { m_bFocusState = bFocusState; }

	void	Apply_Damage(_int iDamage); //데미지 적용
	_float  ApplyDamageAndGetLoseHpRatio(_int iDamage); //데미지 적용 + 잃은 체력 비율 가져오기

	void	Add_Haze(_int iHaze); //플레이어에게 헤이즈 주기 등
	void	Redeuce_Haze(_int iHaze); // 헤이즈 감소시키기
	void	Set_Haze(_int iHaze) {m_Stat.iHaze = iHaze;}//헤이즈 세팅
	void	Set_MaxHP(_float _maxHP) { m_Stat.fMaxHp = _maxHP; }

	void	Add_FocusGauge(_float iFocusGauge,_uint iObjectID); //포커스 게이지 획득
	void	Reduce_FocusGauge(_float iFocusGauge); //포커스 게이지 줄이기
	//포커스 게이지 지속감소용으로(Default 지속 + 근접 공격시 Reduce로 감소) or 현재 포커스 게이지가 전체의 반 이상이고 전투중이 아니라면 감소
	void	ContinuousDecrease_FocusGauge(_float fDecrasePerSecond, _float fTimeDelta); 

	void	Heal_Hp(_float iHealAmount); //체력 회복

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;

protected:
	STAT_DESC m_Stat = {};
	_bool	  m_bIsMaxFoucsGauge = { false };//이제 매프레임 체크안하기 위해서 증가시킬때만 게이지를 풀로 찼는지 판단하고 
	_bool	  m_bFocusState = { false }; 	//포커스 상태일때는 집중게이지를 증가시키면 증가 함수 호출될때 제어하기 위해서


public:
	virtual void Free() override;
};

NS_END