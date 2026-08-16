#pragma once

/* 프로토타입을 레벨별로 보관하는 기능 */
/* 레벨에 있는 프로토타입을 복사해서 반환하는 기능 */

#include "Base.h"

#pragma region Components
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Terrain_Flat.h"
//#include "VIBuffer_Instancing.h"
#include "VIBuffer_Skydome.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Trail.h"
#include "LightComponent.h"
#include "ComputeShader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "StateMachine.h"
#include "BlobShadow.h"
#include "NewTexture.h"
#include "Navigation.h"
#include "Transform.h"
#include "Collider.h"
#include "Texture.h"
#include "Shader.h"
#include "Video.h"
#include "Model.h"
#include "Stat.h"

#pragma endregion

#pragma region UI_Component
#include "UI_WorldComponent.h"
#include "UI_EventReactor.h"
#include "UI_BitmapText.h"
#include "UI_Animation.h"
#include "UI_Progress.h"
#include "UI_SlotGrid.h"
#include "UI_Sprite.h"
#include "UI_Button.h"
#include "UI_Render.h"
#include "UI_Image.h"
#include "UI_Text.h"





#pragma endregion

NS_BEGIN(Engine)

class PrototypeManager final : public Base
{
private:
	explicit PrototypeManager();
	explicit PrototypeManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~PrototypeManager();

	HRESULT Initialize(const _uint _levelMax);

public:
	HRESULT Add_Prototype(const _uint _levelID, const _wstring& prototypename, class Base* pPrototype);
	class Base* Clone_Prototype(const PROTOTYPE _prototypeID, const _uint _levelID, const _wstring& prototypename, void* pArg = nullptr);
	HRESULT Clear_Level(_uint _levelID);
	Base* Find_Prototype(const _wstring& _prototypeName, _uint _levelID) const;

public:
	//프로토타입(원본) 찾으면 base자체로 넘겨준다
    unordered_map<_wstring, class Base*>* Get_Prototypes() { return m_umapPrototypes; }

private:
	_uint m_iLevelMax = {};
	_uint m_iLevelID = {};
	unordered_map<_wstring, class Base*>* m_umapPrototypes = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static PrototypeManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint _levelMax);

public:
	void Free() override final;
};

NS_END
