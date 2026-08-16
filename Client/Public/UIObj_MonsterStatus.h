#pragma once
#include "UIObject.h"

namespace Engine
{
    class Camera;

}

NS_BEGIN(Client)
class UIObj_ProgressBar;

//몬스터 월드위치를 따라다니는 HP바.
class Monster;

class UIObj_MonsterStatus :
    public UIObject
{
public:
    enum class MONSTERSTATUS_UI_EVENT {SET_OWNERID,
        SET_VISIBLE,SET_INVISIBLE,
        OWNER_DEAD,END};
    enum class MONTSTERSTATUS_UI_TYPE {HPBAR,GUARDBAR,END};
     struct MonsterStatusEvent
    {
        MONSTERSTATUS_UI_EVENT eType;
        _uint   iOwnerID;       //set -owner id인 경우 설정할 오너아이디 / owner_Dead인 경우 죽었을때 끄기 위한 오너아이디

    };
protected:
    explicit UIObj_MonsterStatus();
    explicit UIObj_MonsterStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_MonsterStatus(const UIObj_MonsterStatus& original);
    virtual ~UIObj_MonsterStatus();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    virtual void        After_ApplyData();
    
            //오너바뀔때, 첫세팅 시 호출 (풀에서 처음꺼내고 세팅할대?)
    void                Set_TargetObjectID(Monster* pOwner, _float3 OffSet = _float3(0.f,1.f,0.f));

                //오너없어졌을때. 오너주것을때?(풀로 다시들어갈때?)
    void                Clear_TargetObjectID();
    
                    //오너가죽어서.,.다 초기화 후 비활성화
    void            Owner_Dead();
private:
    /*이벤트 처리 변수들*/
    
            //매프레임몬스터가 플레이어와의 거리계산중이므로, 이벤트를 보내서 ui를제어함
            //발신자: 몬스터 
public:
    static UIObj_MonsterStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;


private:
    void        Set_Scale_By_Distance();
    virtual     void Set_Dead(_bool isalive);
    virtual void	OnDestroyInLayer();
private:
    _uint                               m_iTargetObjectID=UINT_MAX;
    Transform*                         m_pTargetTransform = nullptr;        //직접적인 연결은안하지만,, 위치쫓아가기용

    vector< UIObject*>         m_ProgressBars;
    _float3                          m_fOffSet=_float3(0.f,1.f,0.f);


    //타겟카메라캐싱(플레이어카메라)
    Camera*             m_pTargetCam = nullptr;



};
NS_END

