#pragma once
#include "UIObject.h"

#define DEFAULTCOLOR _float4(0.87f,0.87f,0.87f,1.f)
#define DIFFERCOLOR _float4(85.f/255.f,220.f/255.f,1.f,1.f)
#define LESSCOLOR _float4(0.5f,0.f,0.f,1.f)


struct LevelUpInfo_GetPlayer 
{                            
    int     CurrentHaze = 0;
    int         iHp = 0;
    int         iST = 0;

    int     Meyonghyul = 0;
    int     MaxMeyonghyul = 0;

    wstring                     BloodCodeName = L"";

};

struct LevelUpInfo_Player
{
    int                         NeedHaze = 0;
    wstring                     EvadeType = L"";

    wstring                     Type1 = L"";//완력
    wstring                     Type2 = L"";//재주
    wstring                     Type3 = L"";//정신
    wstring                     Type4 = L"";//의지
    wstring                     Type5 = L"";//활력
    wstring                     Type6 = L"";//인내

};
struct LevelUpInfo_Weapon
{
    int     Attack = 0;
    wstring Type = L"";// Rolling or HeavyRolling

    bool        m_bEquip = false;   //무기가 2개이므로 하나는 안끼고있을수도있어서

};
struct LevelUpInfo_Clothes
{
    int     Attack = 0;
    wstring Type = L"";// 관통?그거


};
struct LevelUpInfo_Shield
{
    vector<int> vecInts = { 34,42,38,43,42 };


};


enum class UIPARENT{LEVELINFO,PLAYERINFO,WEAPONINFO,TYPE,END};
enum class PlayerDataSequnce {CURRENTHAZE,NEEDHAZE,INPUT_BLOODCODE,HP,SP,MYUNGHYEOL,EVADE,TYPE1,TYPE2,TYPE3,TYPE4,TYPE5,TYPE6,END};//UIOBJECT순서
enum class WeaponDataSequnce {ATTACK1,ROLLING1,ATTACK2,ROLLING2,ATTACK3,ROLLING3,END};//UIOBJECT순서
enum class ShieldnDataSequnce { TYPE1,TYPE2,TYPE3,TYPE4,TYPE5,END };//UIOBJECT순서




struct LevelUpData
{
    int iLevel = 1;
    LevelUpInfo_Player PlayerData;
    LevelUpInfo_Shield ShieldData;

    int     MaxAttack;   //최대공격력,player set할시에 사용
    int     MaxHp;  // 최대 hp, 레벨업 이후 player set 할시에 사용
    int     MaxST;  //최대 스테미너, 레벨업 이후 player set할시에 사용
};


NS_BEGIN(Engine)
class Player_Stat;

NS_END

NS_BEGIN(Client)
class UIObj_Text;
class InventoryManager;
class Player;

class UIObj_Window_LevelUp :
    public UIObject
{
public:
    struct Window_LevelUpUIEvent
    {
        bool    m_bFocus = false;

    };
protected:
    explicit UIObj_Window_LevelUp();
    explicit UIObj_Window_LevelUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_LevelUp(const UIObj_Window_LevelUp& original);
    virtual ~UIObj_Window_LevelUp();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);


    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);


public:
    void Free() override;

private:
    HRESULT Load_LevelData();

public:
    virtual void        After_ApplyData();
    static UIObj_Window_LevelUp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    void Cache_LevelInfoText();
    void Cache_PlayerInfoText();
    void Cache_WeaponInfoText();
    void Cache_TypeText();//방어력- 제이슨에 임의이값


    void        Set_CurrentPlayerData();//현재 헤이즈,블러드 코드 등 얻어오기
    void        Set_LevelData(_uint iLevel);    //매개변수로 들어온 레벨의 데이터로 보여주자!
    void        Set_Different_Effect(LevelUpData& Data,_uint iLevel); //현재 래벨과 다르거나 같은경우 색깔/arrow처리
public:
    virtual void Set_Active(_bool _isActive);

private:
    UMAP<_uint, LevelUpData>        m_LevelUpDatas; //Json으로 읽어오는 데이터들
    LevelUpData*            m_pTargetData = nullptr;

private:
    LevelUpInfo_GetPlayer           m_PlayerData;       //실제 플레이어데이터들
    vector< LevelUpInfo_Weapon> WeaponDatas;//플레이어로부터 얻어오는데이터
    LevelUpInfo_Clothes ClothesData;    //플레이어로부터 얻어오는데이터


    _uint       m_iLevel = 1;   //플레이어 레벨
    _uint       m_iShowLevel = 1;

    int         iMaxLevel = 12;


private:
    UMAP< UIPARENT, vector<UIObj_Text*>> TextObjs;
    vector<UIObject*> m_Arrows;
    vector<UIObject*> Interactions;

private:
    InventoryManager* m_pInventoryManager = nullptr;
    Player*         m_pMainPlayer = nullptr;
    Player_Stat*    m_pMainPlayerStatComp = nullptr;

private:
    bool        m_bAbleKeyInput = false;    //헤이즈가부족할경우 E키 활성화X
    bool        m_bFocus = true;

private:
    bool            m_bGetItem_Level3=false;
    bool            m_bGetItem_Level7=false;

};
NS_END

