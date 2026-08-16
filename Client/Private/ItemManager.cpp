#include "Client_Define.h"
#include "ItemManager.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(ItemManager);

int      ItemManager::BloodcodeID = 0;//1000~
int      ItemManager::WeaponID=1000;//1000~
int      ItemManager::ArmorID=2000;//2000~
int      ItemManager::PassiveID=3000;//3000~
int      ItemManager::SkillID=4000;//4000~
int      ItemManager::ItemID=5000;//5000~
//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////

Client::ItemManager::ItemManager()
{
}

Client::ItemManager::~ItemManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::ItemManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = GameInstance::GetInstance();


	//직접 이렇게해서 등록하거나 json파일 수정해서 등록하기

	//Create_Skill_Prototype(L"블러드 샷", "KetsugiIcon/T_Ketsugi004", 1, 1, L"피의 힘을 거대한 탄환으로 바꿔 날린다.", L"양", L"액티브", 2, 2, L"관통");
	//Create_Skill_Prototype(L"기프트힐", "KetsugiIcon/T_Ketsugi001", 1, 1, L"자신의 HP를 소비하는 대신 동행자의 HP를 회복시킨다.", L"양", L"액티브", 2, 3, L"-");
	//Create_Skill_Prototype(L"블레이즈 로어", "KetsugiIcon/T_Ketsugi007", 1, 1, L"타오르는 화염을 생성해 표적에게 날린다.", L"음", L"액티브", 2, 3, L"파쇄");


	//Create_Weapon_Prototype(L"타락한 바요네트", "WeaponIcon/T_WeaponIcon_WeaponBayonet_E", L"타귀가 된 흡혈귀가 손에 든 총검", L"관통", 220);
	//Create_Weapon_Prototype(L"헤비 액스", "WeaponIcon/T_WeaponIcon_WeaponHammer_B", L"임시총독부에서 양산된 자루가 긴 배틀 액스", L"절단", 220);

	//Create_Passive_Prototype(L"체력 증가", "PassiveIcon/T_Passive_001", 1,1,L"최대 HP가 증가한다", L"양",L"패시브");

	//Create_Item_Prototype(L"명혈 농축약", "ItemIcon/T_ItemIcon_065", 1, 5, L"명혈로 가득찬 작은 병\n사용하면 소량의 명혈을 얻는다");

	//Create_Armor_Prototype(L"여왕 토벌대 아장/가시형", "BloodGearIcon/T_CategoryIcon_LongCoat_E_Col3", L"퀸 토벌군에게 배치되었던 흡혈 아장\n종류를 가리지 않고 연혈의 효과를 보조한다",L"관통",190.f);
	//Create_BloodCode_Prototpye(L"캐스터", "Textures/T_BloodCode_Caster", L"윤택한 명혈로 강력한 연혈 공격이 가능한 타입\n위력이 강한 원거리 연혈을 갖추었다");
	////Save_ItemData();



	Load_ItemData("../../DataFiles/Item_Data/ItemData.json");

	return S_OK;

}
HRESULT Client::ItemManager::Create_Skill_Prototype(wstring ItemName, string TexKey, int itemCount, int maxSize, wstring Description, wstring Lineage_Type, wstring SkillType, float fInchor_cost, float cooldown, wstring AttackType)
{
	//스킬아이템 만들기
	//계통,연혈타임, 소비명혈, 재사용시간, 공격 속성
	ItemInfo pNewSkillItemInfo;
	pNewSkillItemInfo.ItemID = SkillID++;
	pNewSkillItemInfo.ItemName = ItemName;
	pNewSkillItemInfo.ItemTexKey = TexKey;
	pNewSkillItemInfo.m_eCategory = ITEM_CATEGORY::SKILL;
	pNewSkillItemInfo.itemCount = itemCount;
	pNewSkillItemInfo.maxItemSize = maxSize;
	pNewSkillItemInfo.ItemDescription = Description;


	pNewSkillItemInfo.m_wstrLineage_Type = Lineage_Type;		//음/양
	pNewSkillItemInfo.m_SkillType = SkillType;		
	pNewSkillItemInfo.m_AttackType = AttackType;
	pNewSkillItemInfo.m_Values[_UINT(ITEM_VALUE::ICHOR_COST)] = fInchor_cost;	//소비명혈
	pNewSkillItemInfo.m_Values[_UINT(ITEM_VALUE::COOLDOWN)] = cooldown;


	size_t hashKey = hash<wstring>{}(pNewSkillItemInfo.ItemName);
	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewSkillItemInfo);
	return S_OK;
}
HRESULT Client::ItemManager::Create_Item_Prototype(wstring ItemName, string TexKey, int itemCount, int maxSize, wstring Description)
{
	//아이템 만들기
	//현재아이템/최대아이템개슈
	ItemInfo pNewItemInfo;
	pNewItemInfo.ItemID = ItemID++;
	pNewItemInfo.ItemName = ItemName;
	pNewItemInfo.ItemTexKey = TexKey;
	pNewItemInfo.m_eCategory = ITEM_CATEGORY::ITEM;
	pNewItemInfo.itemCount = itemCount;
	pNewItemInfo.maxItemSize = maxSize;
	pNewItemInfo.ItemDescription = Description;


	size_t hashKey = hash<wstring>{}(pNewItemInfo.ItemName);

	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewItemInfo);
	return S_OK;
}

HRESULT Client::ItemManager::Create_Weapon_Prototype(wstring ItemName, string TexKey, wstring Description, wstring AttackType, float PhysicsDamage)
{
	//아이템 만들기
		//공격력/물리
	ItemInfo pNewItemInfo;
	pNewItemInfo.ItemID = WeaponID++;
	pNewItemInfo.ItemName = ItemName;
	pNewItemInfo.ItemTexKey = TexKey;
	pNewItemInfo.m_eCategory = ITEM_CATEGORY::EQUIP_WEAPON;
	pNewItemInfo.itemCount = 1;
	pNewItemInfo.maxItemSize = 1;
	pNewItemInfo.ItemDescription = Description;

	pNewItemInfo.m_AttackType = AttackType;
	pNewItemInfo.m_Values[_UINT(ITEM_VALUE::PHYSICS_DMG)] = PhysicsDamage;


	size_t hashKey = hash<wstring>{}(pNewItemInfo.ItemName);

	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewItemInfo);
	return S_OK;
}
HRESULT Client::ItemManager::Create_Passive_Prototype(wstring ItemName, string TexKey, int itemCount, int maxSize, wstring Description, wstring Lineage_Type, wstring SkillType)
{
	//아이템 만들기
		//공격력/물리
	ItemInfo pNewItemInfo;
	pNewItemInfo.ItemID = PassiveID++;
	pNewItemInfo.ItemName = ItemName;
	pNewItemInfo.ItemTexKey = TexKey;
	pNewItemInfo.m_eCategory = ITEM_CATEGORY::PASSIVE;
	pNewItemInfo.itemCount = itemCount;
	pNewItemInfo.maxItemSize = maxSize;
	pNewItemInfo.ItemDescription = Description;

	pNewItemInfo.m_wstrLineage_Type = Lineage_Type;
	pNewItemInfo.m_SkillType = SkillType;

	
	size_t hashKey = hash<wstring>{}(pNewItemInfo.ItemName);

	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewItemInfo);
	return S_OK;
}
HRESULT Client::ItemManager::Create_Armor_Prototype(wstring ItemName, string TexKey, wstring Description, wstring AttackType, float PhysicsDamage)
{
	//아이템 만들기
		//공격력/물리
	ItemInfo pNewItemInfo;
	pNewItemInfo.ItemID = ArmorID++;
	pNewItemInfo.ItemName = ItemName;
	pNewItemInfo.ItemTexKey = TexKey;
	pNewItemInfo.m_eCategory = ITEM_CATEGORY::EQUIP_ARMOR;
	pNewItemInfo.itemCount = 1;
	pNewItemInfo.maxItemSize = 1;
	pNewItemInfo.ItemDescription = Description;

	pNewItemInfo.m_AttackType = AttackType;
	pNewItemInfo.m_Values[_UINT(ITEM_VALUE::PHYSICS_DMG)] = PhysicsDamage;



	size_t hashKey = hash<wstring>{}(pNewItemInfo.ItemName);

	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewItemInfo);
	return S_OK;

}
HRESULT Client::ItemManager::Create_BloodCode_Prototpye(wstring ItemName, string TexKey, wstring Description)
{
	//아이템 만들기
		//공격력/물리
	ItemInfo pNewItemInfo;
	pNewItemInfo.ItemID = BloodcodeID++;
	pNewItemInfo.ItemName = ItemName;
	pNewItemInfo.ItemTexKey = TexKey;
	pNewItemInfo.m_eCategory = ITEM_CATEGORY::BLOODCODE;
	pNewItemInfo.itemCount = 1;
	pNewItemInfo.maxItemSize = 1;
	pNewItemInfo.ItemDescription = Description;

	size_t hashKey = hash<wstring>{}(pNewItemInfo.ItemName);

	//중복검사
	for (auto& pair : m_ItemPrototypes)
	{
		if (hashKey == pair.first)
		{
			MSG_BOX("중복한 키를 가진 원본이있습니다.");
			return E_FAIL;
		}
	}
	m_ItemPrototypes.emplace(hashKey, pNewItemInfo);
	return S_OK;
}
HRESULT Client::ItemManager::Save_ItemData()
{
	ordered_json	root;
	
	for (auto& pair : m_ItemPrototypes)
	{
		ItemInfo	Info = pair.second;

		ordered_json	item;
		item["ItemID"] = Info.ItemID;
		item["Description"] = wstringToString(Info.ItemDescription);

		item["Name"] = wstringToString(Info.ItemName);
		item["ItemTexKey"] = Info.ItemTexKey;
		
		item["ItemCount"] = Info.itemCount;				//0
		item["MaxItemCount"] = Info.maxItemSize;

		item["Category"] = ItemInfo::Get_Category_To_String(Info.m_eCategory);
		for (int i = 0; i < (int)ITEM_VALUE::END; ++i)
		{
			if (Info.m_Values[i] != -1.f)
			{
				string Key = ItemValue_To_String(ITEM_VALUE(i));
				item["Values"][Key] = Info.m_Values[i];
			}
		}
	
		if(Info.m_wstrLineage_Type!=L"")
			item["Values"]["Lineage_Type"] = wstringToString(Info.m_wstrLineage_Type);
		
		if (Info.m_SkillType != L"")
			item["Values"]["SkillType"] = wstringToString(Info.m_SkillType);

		if (Info.m_AttackType != L"")
			item["Values"]["AttackType"] = wstringToString(Info.m_AttackType);

		root["ItemPrototypes"].push_back(item);
	
	}
	
	std::ofstream file("../../DataFiles/Item_Data/ItemData.json");
	file << root.dump(4); 

	return S_OK;
}
HRESULT Client::ItemManager::Load_ItemData(string path)
{
	ifstream file(path);
	if (!file.is_open())
		return E_FAIL;


	ordered_json root = ordered_json::parse(file);
	for (auto& itemData : root["ItemPrototypes"])
	{
		ItemInfo	Info;

		Info.ItemID = itemData["ItemID"];
		Info.ItemDescription = stringToWstring(itemData["Description"]);
		Info.ItemName = stringToWstring(itemData["Name"]);
		Info.ItemTexKey = itemData["ItemTexKey"];
		Info.itemCount = itemData["ItemCount"];
		Info.maxItemSize = itemData["MaxItemCount"];

		Info.m_eCategory = ItemInfo::Get_Category_To_Enum(itemData["Category"]);
		Info.m_iHaze = itemData.value("Haze",0);

		for (auto& value : itemData["Values"].items())
		{
			ITEM_VALUE eType = String_To_ItemValue(value.key());

			if (eType != ITEM_VALUE::END)
			{
				Info.m_Values[(_uint)eType] = value.value();
			}
		}


		Info.m_wstrLineage_Type = stringToWstring(itemData["Values"].value("Lineage_Type", ""));
		Info.m_SkillType = stringToWstring(itemData["Values"].value("SkillType", ""));
		Info.m_AttackType = stringToWstring(itemData["Values"].value("AttackType", ""));

		size_t Hash = hash<wstring>{}(Info.ItemName);
		m_ItemPrototypes.emplace(Hash, Info);
	
	}
	return S_OK;
}

ItemInfo* Client::ItemManager::Get_ItemInfo(wstring wstrPrototypeName)
{
	size_t hashKey = hash<wstring>{}(wstrPrototypeName);
	auto iter = m_ItemPrototypes.find(hashKey);
	if (iter != m_ItemPrototypes.end())
	{
		ItemInfo* pClone = new ItemInfo((iter->second));
		return pClone;
	}
	return nullptr;
}

ItemInfo* Client::ItemManager::Get_ItemInfo(_uint _itemID)
{
	for (auto& pair : m_ItemPrototypes)
	{
		if (pair.second.ItemID == _itemID)
		{
			ItemInfo* pClone = new ItemInfo(pair.second);
			return pClone;
		}
	}
	return nullptr;
}

_uint Client::ItemManager::Get_ItemID(wstring ItemName)
{
	size_t Hash = hash<wstring>{}(ItemName);
	auto iter = m_ItemPrototypes.find(Hash);

	if (iter != m_ItemPrototypes.end())
		return iter->second.ItemID;

	return 0;
}

void Client::ItemManager::Set_MaxCount(wstring ItemName, _uint MaxCount)
{
	size_t Hash = hash<wstring>{}(ItemName);
	auto iter = m_ItemPrototypes.find(Hash);

	if (iter != m_ItemPrototypes.end())
	{
		iter->second.maxItemSize = MaxCount;
	}

	return;
}

void Client::ItemManager::Free()
{
	
	__super::Free();
}
/******************************************************* 객체 준비 함수 *******************************************************/
