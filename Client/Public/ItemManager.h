#pragma once
#include "Base.h"

namespace Engine
{
    class GameInstance;

}

/*아이템의 원본을 등록하는곳. json이나 실제 create로도 만들기가능*/
NS_BEGIN(Client)
class ItemManager :
    public Base
{
    DECLARE_SINGLETON(ItemManager);

private:
    explicit ItemManager();
    virtual ~ItemManager();

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
                //d아이템원본을 만들어서 map에추가
    HRESULT     Create_Skill_Prototype(wstring ItemName, string TexKey,int itemCount, int maxSize, wstring Description, wstring Lineage_Type, wstring SkillType, float fInchor_cost, float cooldown,wstring AttackType);
    HRESULT     Create_Item_Prototype(wstring ItemName, string TexKey,  int itemCount, int maxSize, wstring Description);
    HRESULT     Create_Weapon_Prototype(wstring ItemName, string TexKey, wstring Description, wstring AttackType, float PhysicsDamage);


    HRESULT     Create_Passive_Prototype(wstring ItemName, string TexKey, int itemCount, int maxSize, wstring Description, wstring Lineage_Type, wstring SkillType);


    HRESULT     Create_Armor_Prototype(wstring ItemName, string TexKey, wstring Description, wstring AttackType, float PhysicsDamage);
    HRESULT     Create_BloodCode_Prototpye(wstring ItemName, string TexKey, wstring Description);
private:
    HRESULT     Save_ItemData();   
    HRESULT    Load_ItemData(string path);

public:
                //워본을 통해 복사본 clone타입의 iteminfo를반환
    ItemInfo*   Get_ItemInfo(wstring wstrPrototypeName);
    ItemInfo*   Get_ItemInfo(_uint _itemID);

    //아이템 이름으로 아이템정보가져오기
    //아이템 아이디가져오기
    _uint       Get_ItemID(wstring ItemName);
    
public:
    void        Set_MaxCount(wstring ItemName, _uint MaxCount);

    
public:
    void Free() override final;

public:
    GameInstance* m_pGameInstance = nullptr;
    unordered_map<size_t, ItemInfo>      m_ItemPrototypes;


private:
    static int      BloodcodeID;
    static int      WeaponID;//1000~
    static int      ArmorID;//2000~
    static int      PassiveID;//3000~
    static int      SkillID;//4000~
    static int      ItemID;//5000~


};
NS_END
