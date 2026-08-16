#pragma once
#include "Client_Define.h"

/// <summary>
/// 수치 테스트용 struct
/// </summary>
struct PlayerStatus
{
    //////HP관련
    float fMaxHp = 100.f;
    float fCurrentHp = fMaxHp;


    ///////////Stamina관련
    float fMaxStamina = 50.f;
    float fCurrentStamina = fMaxStamina;
    float fLowStamina = 20.f;		//경고표시가 켜질때를 판단하기 위한 스태미너값
    float fStaminaGenTime = 0.f;
    bool	m_bUseStamina = false;
    //기준치보다 낮으면 들어오는상태, 이벤트발생을 위해(빨간색깜빡거림)
    bool	m_bEnterLessStaminaState = false;


    ///////////////가드(집중상태-파란색게이지)관련
    float fMaxGuard = 50.f;
    float fCurrentGuard = 0.f;
    //집중상태를 의미하는변수 (이거활성화되면 가드 풀충전되서 그 깜빡깜빡쓰)
    bool	m_bFocusState = false;


   
 


    //각스테미나 처리방법
    void Update(const _float fTimeDelta);


    void Walk(const _float fTimeDelta);	//스테미나 깎이는거 Test

};