#include "UITool_Define.h"
#include "UIFactory.h"
#include "GameInstance.h"
#include "UITool_Macro_Component.h"


HRESULT UIFactory::CreateHpBar(_wstring _layer, _uint iLevel, _float2 SizeValue, UIObject** pOut)
{
	//{
	//	static int		m_iHpBarCnt = 0;
	//	//Empty Root(HpBar)
	//			//Hp_Back__Balck
	//				//HP_Bar_Yellow
	//				//HP_Bar_RedFill
	//				//HP_Bar_Frame
	//	GameObject* pRoot = nullptr;
	//	UIObject::UIOBJECT_DESC RootDesc;
	//	RootDesc.fCX = (_float)694.f * SizeValue.x;
	//	RootDesc.fCY = (_float)122.0 * SizeValue.y;
	//	RootDesc.fX = 785.f;
	//	RootDesc.fY = 405.f;
	//	RootDesc.wstrName = L"HpBar";

	//	if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject", iLevel, _layer, &pRoot, &RootDesc)))
	//		return E_FAIL;
	//	UIObject* pUIRoot = dynamic_cast<UIObject*>(pRoot);
	//	UI_Render::UICOMPDESC Desc;
	//	Desc.pOwner = pUIRoot;

	//	pUIRoot->Add_NewRenderComponent(0, Proto_UIRender, Proto_UIRender, &Desc);

	//	//자식들생성
	//	/////BlackFill
	//	GameObject* pBlackBack = nullptr;
	//	UIObject::UIOBJECT_DESC BlackDesc;
	//	BlackDesc.fCX = 90.49f;
	//	BlackDesc.fCY = 19.672;
	//	BlackDesc.fX = 0.f;
	//	BlackDesc.fY = 1.f;
	//	BlackDesc.iZOrder = 1;
	//	BlackDesc.pParent = pUIRoot;
	//	BlackDesc.wstrName = L"HpBar_Black";
	//	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject",iLevel,_layer,&pBlackBack, &BlackDesc)))
	//		return E_FAIL;

	//	
	//	//Component붙이기
	//	UIObject* pUIBlackBack = dynamic_cast<UIObject*>(pBlackBack);
	//	{
	//		UI_Image::UIIMAGEDESC ImageDesc;
	//		ImageDesc.pOwner = pUIBlackBack;
	//		ImageDesc.TextureKey = "UI4_Main/Main_state_2";
	//		pUIBlackBack->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &ImageDesc);

	//	}
	//	
	//	/////YellowFill
	//	GameObject* pYellowFill = nullptr;
	//	UIObject::UIOBJECT_DESC YelloFillDesc;
	//	YelloFillDesc.fCX = 100.f;
	//	YelloFillDesc.fCY = 102.5f;
	//	YelloFillDesc.fX = 0.091f;
	//	YelloFillDesc.fY = -2.226f;
	//	YelloFillDesc.iZOrder = 0;
	//	YelloFillDesc.wstrName = L"HpBar_YellowFill";
	//	YelloFillDesc.pParent = dynamic_cast<UIObject*>(pBlackBack);
	//	YelloFillDesc.iShaderNumber = 5;
	//	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject", iLevel, _layer, &pYellowFill, &YelloFillDesc)))
	//		return E_FAIL;

	//	//Component붙이기
	//	{
	//		UIObject* pYellowFillUI = dynamic_cast<UIObject*>(pYellowFill);
	//		CHECK_NULL_RESULT(pYellowFillUI, E_FAIL);

	//		UI_Image::UIIMAGEDESC ImageDesc;
	//		ImageDesc.pOwner = pYellowFillUI;
	//		pYellowFillUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &ImageDesc);

	//		UI_Progress::UIProgressDesc ProgressDesc;
	//		ProgressDesc.pOwner = pYellowFillUI;
	//		//ProgressDesc.fCurrent = 1.f;
	//		ProgressDesc.TextureKey = "UI4_Main/Main_state_1_2";
	//		pYellowFillUI->Add_NewUIComponent(UITYPE::PROGRESSBAR, 0, Proto_UIProgress, Proto_UIProgress, &ProgressDesc);


	//
	//	}
	//	

	//	/////RedFill
	//	GameObject* pRedFill = nullptr;
	//	UIObject::UIOBJECT_DESC RedFillDesc;
	//	RedFillDesc.fCX = 99.602f;
	//	RedFillDesc.fCY = 101.233f;
	//	RedFillDesc.fX = 0.159f;
	//	RedFillDesc.fY = -4.167f;
	//	RedFillDesc.iZOrder = 2;
	//	RedFillDesc.wstrName = L"HpBar_RedFill";
	//	RedFillDesc.pParent = dynamic_cast<UIObject*>(pBlackBack);
	//	RedFillDesc.iShaderNumber = 5;
	//	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject", iLevel, _layer, &pRedFill, &RedFillDesc)))
	//		return E_FAIL;

	//	//Component붙이기
	//	{
	//		UIObject* pRedFillUI = dynamic_cast<UIObject*>(pRedFill);
	//		CHECK_NULL_RESULT(pRedFillUI, E_FAIL);

	//		UI_Image::UIIMAGEDESC ImageDesc;
	//		ImageDesc.pOwner = pRedFillUI;
	//		pRedFillUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &ImageDesc);

	//		UI_Progress::UIProgressDesc ProgressDesc;
	//		ProgressDesc.pOwner = pRedFillUI;
	//		//ProgressDesc.fCurrent = 1.f;
	//		ProgressDesc.TextureKey = "UI4_Main/Main_state_1";
	//		pRedFillUI->Add_NewUIComponent(UITYPE::PROGRESSBAR,0, Proto_UIProgress, Proto_UIProgress,&ProgressDesc);

	//		
	//	}


	//	/////BarFrame
	//	GameObject* pBarFrame = nullptr;
	//	UIObject::UIOBJECT_DESC BarFrameDesc;
	//	BarFrameDesc.fCX = 99.841f;
	//	BarFrameDesc.fCY = 163.683f;
	//	BarFrameDesc.fX = 0.178f;
	//	BarFrameDesc.fY = -5.579f;
	//	BarFrameDesc.iZOrder = 3;
	//	BarFrameDesc.wstrName = L"HpBar_BarFrame";
	//	BarFrameDesc.pParent = dynamic_cast<UIObject*>(pBlackBack);

	//	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), L"Prototype_UIObject", iLevel, _layer, &pBarFrame, &BarFrameDesc)))
	//		return E_FAIL;

	//	//Component붙이기
	//	{
	//		UIObject* pFrameUI = dynamic_cast<UIObject*>(pBarFrame);
	//		CHECK_NULL_RESULT(pFrameUI, E_FAIL);

	//		UI_Image::UIIMAGEDESC ImageDesc;
	//		ImageDesc.pOwner = pFrameUI;
	//		ImageDesc.TextureKey = "UI4_Main/Main_state_8";
	//		pFrameUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &ImageDesc);

	//	}


	//	if (pOut != nullptr)
	//		*pOut = pUIRoot;

	//	return S_OK;
	//}
	return S_OK;
}
