#pragma once
#include "UIObject.h"

namespace Engine
{
	class GameInstance;
}

namespace UIFactory
{

	//Tool에서 prefab 생성용
	HRESULT CreateHpBar(_wstring _layer,_uint iLevel,_float2 SizeValue=_float2(1.f,1.f),UIObject** pOut=nullptr);




	inline static GameInstance* m_pGameInstance=GameInstance::GetInstance();
}