#include "Client_Define.h"
#include "UIObj_CustomizeGrid.h"
#include "UIObj_Text.h"
#include "UIObj_CustomizeSelector.h"
#include "UIObj_ValueSelector.h"



Client::UIObj_CustomizeGrid::UIObj_CustomizeGrid()
{
}

Client::UIObj_CustomizeGrid::UIObj_CustomizeGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_CustomizeGrid::UIObj_CustomizeGrid(const UIObj_CustomizeGrid& original)
    :UIObject(original)
{
}

Client::UIObj_CustomizeGrid::~UIObj_CustomizeGrid()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_CustomizeGrid::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_CustomizeGrid::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
   

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< CustomizeGridUIEvent>([this](const CustomizeGridUIEvent& e)
        {
            //만약, 클릭한 카테고리가바뀐상황이라면 
            if (e.eventType == CustomizeGridEventType::UPDATE_FOCUSCATEGORY)
            {
                //판단하여 자기를 킬지안킬지 정함.
                if (e.eTargetCategory == m_eCustomType)
                {
                    Set_Visible(true);
                    Set_Active(true);
                }

                else
                {
                    Set_Visible(false);
                    Set_Active(false);
                }
            }

            //타입이맞는 메뉴에서의 호버만 바군다.
            else if (e.eventType == CustomizeGridEventType::UPDATE_HOVERTEXT)
            {
                CHECK_TRUE(e.eTargetCategory != m_eCustomType);
                CHECK_JUST_NULL(m_pHoverText);
                wstring* wstr = static_cast<wstring*>(e.pArg);
                if (wstr)
                {
                    m_pHoverText->Set_Text(*wstr);
                    m_pHoverText->Set_Active(true);
                }

               
               

            }
            

            //메뉴 진입 시 첫번째 슬롯이 자동으로 호버된 상태로만들기위함
            else if (e.eventType == CustomizeGridEventType::OPEN_MENU)
            {
                CHECK_TRUE(e.eTargetCategory != m_eCustomType);
                CHECK_TRUE(m_Clickable.empty());

               
                for (auto& pObj : m_Clickable)
                {
                    pObj->OnHoverExit();
                    pObj->Set_Alpha(1.f);
                }

                m_Clickable[0]->OnHoverEnter();

            

            }
            //편집기창이열렸을때 다른 버튼들은 어둡게처리
            else if (e.eventType == CustomizeGridEventType::OPEN_SELECTOR)
            {
                CHECK_TRUE(e.eTargetCategory != m_eCustomType);
                CHECK_TRUE(m_Clickable.empty());


				int pIdx = e.idx;
				if (pIdx!=-1)
				{
					for (auto& pObj : m_Clickable)
					{
						UIObj_ValueSelector* pSelector = dynamic_cast<UIObj_ValueSelector*>(pObj);
						if (pSelector)
						{
							if (*pSelector->Get_Idx() == pIdx)
								continue;
						}

						pObj->Set_Alpha(0.5f);
					}
				}


            }

            else if (e.eventType == CustomizeGridEventType::EXIT_SELECTOR)
            {
                CHECK_TRUE(e.eTargetCategory != m_eCustomType);
                CHECK_TRUE(m_Clickable.empty());
                CHECK_JUST_NULL(m_pHoverText);

              //  m_pHoverText->Set_Active(false);
                
                for (auto& pObj : m_Clickable)
                {
                   
                    pObj->Set_Alpha(1.f);
                }

 


            }
            else if (e.eventType == CustomizeGridEventType::EXIT_MENU)
            {
                CHECK_TRUE(e.eTargetCategory != m_eCustomType);
                CHECK_TRUE(m_Clickable.empty());

                for (auto& pObj : m_Clickable)
                {
                    pObj->Set_Alpha(1.f);
                    pObj->OnHoverExit();
                   
                }

                if (m_pHoverText)
                    m_pHoverText->Set_Active(false);



            }
        }));



    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


void Client::UIObj_CustomizeGrid::Free()
{
    __super::Free();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_CustomizeGrid::After_ApplyData()
{
    __super::After_ApplyData();

    //1.타입에 따라서  customtype지정
    m_eCustomType = Get_CustomType(m_UIType);



    //2. 각 슬롯 호버시 보일 텍스트 캐싱
    m_pHoverText = dynamic_cast<UIObj_Text*>(Get_Child(L"Hover_Text"));


    for (auto& pChild : m_vecChildren)
    {
        if (pChild->Get_Name() == L"Hover_Text")
            continue;

        m_Clickable.push_back(pChild);


    }

    /*내림차순정렬*/
    sort(m_Clickable.begin(), m_Clickable.end(), [&](GameObject* a, GameObject* b)
        {
            if (a && b)
            {
                return a->Get_ZOrder() > b->Get_ZOrder();
            }

            return false;
        });
    
}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_CustomizeGrid::Set_Active(_bool _isActive)
{
    if (_isActive)
    {
        if (m_pHoverText)
        {
            m_pHoverText->Set_Text(L"");
            
        }


    }
    __super::Set_Active(_isActive);
}

    
    


///////////////////////////////////////////////////////////
UIObj_CustomizeGrid* Client::UIObj_CustomizeGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_CustomizeGrid* pInstance = new UIObj_CustomizeGrid(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_CustomizeGrid 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}



GameObject* Client::UIObj_CustomizeGrid::Clone(void* pArg)
{
    UIObj_CustomizeGrid* pInstance = new UIObj_CustomizeGrid(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_CustomizeGrid 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
