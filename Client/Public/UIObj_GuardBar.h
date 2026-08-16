#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_ProgressBar;

class UIObj_GuardBar :
    public UIObject
{
    enum class ArrowType{LEFTARROW,RIGHTARROW,END};
 
    struct ArrowInfo
    {
        UIObject* m_pArrow = nullptr;
        _float     m_InitLocalPosX;
        _float     m_CurrentLocalPosX;
        _float     m_MaxLocalPosX;

    };
    enum MoveType{INCREASE,DECREASE};
protected:
    explicit UIObj_GuardBar();
    explicit UIObj_GuardBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_GuardBar(const UIObj_GuardBar& original);
    virtual ~UIObj_GuardBar();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    //Event함수
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);

public:
    virtual void        After_ApplyData();

                    //연결한값들다 초기화
    void            On_Dead();

public:
    static UIObj_GuardBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;


private:
    void        Move_Arrow(_float fValue, MoveType eType);
    void        Blink_HitRate();
    void        Active_GuageFullEffect();

    bool        Check_AnimationEnd();
    void        End_Animation();

private:
    /*이벤트 함수들*/
    void        OnGuardEvent(GuardBarUIEvent e);

public:
    void        Set_OwnerObjectID(_uint iD) { m_iOwnerObjectID = iD; }
    void        Clear_OwnerObjectID() { m_iOwnerObjectID = UINT_MAX; }

public:
    void        Set_TargetValue(float* pTarget);
    void        Set_MaxValue(float* pMax);
    /*연출을 위해 제어할 포인터들*/
private:
    vector<ArrowInfo>   m_Arrows;
    vector<UIObj_ProgressBar*>      m_ProgressBars;
    UIObject* m_HitRate = nullptr;
    UIObject* m_GuageFullEffect = nullptr;
    
                    //겹치지않기위해  size_t사용
    unordered_map<string, UIOwnerEventInfo> m_Actions;

private:
    
    //progress에게 전달하기위해 여기에 캐싱
    float*       m_fCurrnetPoint = nullptr;
    float*      m_fMaxPoint = nullptr;

    float       m_fMaxGuardDistance = 28.f;
    float       m_fCurrentMoveRatio = 0.f;

                //다 차서 가드 full 이펙트진입
    bool        m_bPlayFullEffect = false;


private:
    _uint           m_iOwnerObjectID = UINT_MAX;
};

NS_END
