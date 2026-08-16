#include "Engine_Define.h"
#include "UIObject.h"
#include "GameInstance.h"
#include "Component.h"
#include "UI_Render.h"
#include "Layer.h"
#include "Mouse.h"

#ifdef _DEBUG

#include "InputManager.h"
#include "DebugDraw.h"
#include "ImguiManager.h"

#endif // _DEBUG




//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::UIObject::UIObject()
{
}

Engine::UIObject::UIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{

    m_pMouse = Mouse::GetInstance();


}

Engine::UIObject::UIObject(const UIObject& original)
	: GameObject(original)
{
    m_pMouse = original.m_pMouse;

#ifdef _DEBUG
    m_eEngineMode = original.m_eEngineMode;
#endif // _DEBUG

 
 


}

Engine::UIObject::~UIObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIObject::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
#ifdef _DEBUG
    m_eEngineMode = m_pGameInstance->Get_EngineMode();
#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::UIObject::Initialize(void* arg)
{
    static _uint namenum = 0;

    m_eRenderGroup = RENDER_GROUP::UI;
    m_DeActiveAlarm.Off();

	if (arg != nullptr)
	{
		UIOBJECT_DESC* desc = CAST(UIOBJECT_DESC*)(arg);

		/* UI오브젝트 초기화 */
		m_Local.m_fX = desc->fX;
		m_Local.m_fY = desc->fY;
		m_Local.m_fSizeX = desc->fCX;
		m_Local.m_fSizeY = desc->fCY;
        m_Local.m_fAlpha = desc->fAlpha;
        
		m_fWindowX = desc->fWindowX;
		m_fWindowY = desc->fWindowY;
        m_iZOrder = desc->iZOrder;
        
        m_iShaderSlotNum = desc->iShaderNumber;


        m_BaseType = desc->UIBaseType;
        m_UIType = desc->UIType;

        //fOriginSizeX = m_Local.m_fSizeX;
        //fOriginSizeY = m_Local.m_fSizeY;


        m_iObjectNumber = namenum++;

        if(desc->wstrName==L"")
            CAST(UIOBJECT_DESC*)(arg)->wstrName = L"UI_Obj_" + to_wstring(m_iObjectNumber);

        //else
        //{
        //    /**/
        //    if(!desc->pParent)
        //        CAST(UIOBJECT_DESC*)(arg)->wstrName = desc->wstrName + to_wstring(m_iObjectNumber);

        //}

        m_wstrName = CAST(UIOBJECT_DESC*)(arg)->wstrName;
        Set_HashName();

        //기본값사용하도록 채워주기
        if (desc->wstrShaderName == L"")
        {
            desc->wstrShaderName = L"Prototype_Component_Shader_VTXPosTex";
     
        }
        
        desc->vecPasses.push_back(m_iShaderSlotNum);
        fOriginSizeX = 100.f;
        fOriginSizeY = 100.f;
	}

	D3D11_VIEWPORT viewportdesc = {};
	if (m_fWindowX == 0.f ||m_fWindowY == 0.f)
	{
		UINT inumviewport = { 1 };
		m_pContext->RSGetViewports(&inumviewport, &viewportdesc);
		
        m_fWindowX = viewportdesc.Width;
        m_fWindowY = viewportdesc.Height;

       
	}

 
    MSG_FAIL(GameObject::Initialize(arg), L"게임오브젝트 초기화 실패", L"Caution!!!", E_FAIL);

    /*처음 생성한 스크린좌표를 월드좌표로 배치*/
	m_pTransformCom->Set_Scale(m_Local.m_fSizeX, m_Local.m_fSizeY, 1.f);
	m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(m_Local.m_fX - m_fWindowX * 0.5f, -m_Local.m_fY + m_fWindowY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_matViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProjectionMatrix, XMMatrixOrthographicLH(m_fWindowX, m_fWindowY, 0.0f, 1.f));

    

    CHECK_FAILED(Ready_Components(arg), E_FAIL);

    if (arg)
    {
        UIOBJECT_DESC* desc = CAST(UIOBJECT_DESC*)(arg);
        if (desc->pParent)
            desc->pParent->Add_Child_OnLoad(this);

        else
        {
            if(desc->bAddUIManager)
                m_pGameInstance->Add_UIObject(this);
        }
           

        ////처음 비활성화/활성화 초기값
        //if (desc->bInitActive == false)
        //    Set_Active(false);
    }
   

    
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIObject::Ready_Components(void* pArg)
{
    /*Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
     Add_Component(m_iLevel, _textureName, Com_Texture, RCAST(Component**)(&m_pTextureCom));*/
    
    if (pArg != nullptr)
    {
        UIOBJECT_DESC* desc = CAST(UIOBJECT_DESC*)(pArg);
		Add_Component(0, desc->wstrShaderName, Com_Shader, RCAST(Component**)(&m_pShaderCom));
		//uirender컴포넌트 추가


    }

    return S_OK;
}

/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIObject::Update_Priority(const _float fTimeDelta)
{
    
    //부모가있을경우, 부모가 자식객체를 모두 관리하므로 return 때린다.
    if (m_pParent)
        return 0;

    if (!m_bIsActive && !m_bClosing)
        return 0;

    for (auto& pChild : m_vecChildren)
        pChild->Update_Priority(fTimeDelta);


	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/

 

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIObject::Update(const _float fTimeDelta)
{
    

#ifdef _DEBUG
    if (m_eEngineMode== ENGINEMODE::EDITOR)
    {
        Move(10.f, fTimeDelta);
        Transform_With_Mouse(fTimeDelta);
        if (m_bDragEnter && m_pGameInstance->MouseUp(MOUSEKEYSTATE::LB))
            OnDraggingExit();
    }

#endif // _DEBUG

    if (!m_bIsActive && !m_bClosing)
        return 0;
    
    //for (auto& pCom : m_umapComponents)
    //{
    //    if (pCom.second)//
    //        pCom.second->Update(fTimeDelta);
    //}

    //부모가있을경우, 부모가 자식객체를 모두 관리하므로 return 때린다.
    if (m_pParent)
        return 0;

  
    //static _float Timer = {};
    //Timer += fTimeDelta;

    //

    //m_Local.m_fSizeX = m_Local.m_fSizeY = SinWave(Timer, 1.f, 100.f, 200.f);
    //m_bIsVisible = true;


    m_DeActiveAlarm.Update(fTimeDelta);
    //가장 root가 재귀적으로 자식꺼 호출( root인경우 m_fX,m_fY, 1x1 이 기준임)
                                        //m_bDirty=값이 변했을때 true,
    if (!Is_WorldUI())
        Update_Recursive(fTimeDelta, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, _float2(0.f, 0.f), m_bIsDirty);

    else
        Update_World_Recursive(fTimeDelta, XMMatrixIdentity(),1.f);



	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIObject::Update_Late(const _float fTimeDelta)
{
    if (!m_bIsActive && !m_bClosing)
        return 0;

    /*만약 late_update에서 처리할연산이있으면 여기서적어. 대신 부모가 있는 자식들은 renderer에 추가되면안돼.*/
    for (auto& pComp : m_umapComponents)
    {
        pComp.second->Update_Late(fTimeDelta);
    }

    //부모가있을경우, 부모가 자식객체를 모두 관리하므로 return 때린다.
    if(m_bIsVisible && !m_pParent)
        m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

    //자식들의 update_late호출(add_renderobject가아니라 그냥 추가연산)
    //필요하면 주석풀기
    for (auto& pChild : m_vecChildren)
        pChild->Update_Late(fTimeDelta);

	return 0;
}
HRESULT Engine::UIObject::Render(const _float fTimeDelta)
{
    if (!m_bIsActive && !m_bClosing || !m_bIsVisible)
        return 0;

     Render_UI(fTimeDelta);


    return S_OK;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIObject::Render_UI(const _float fTimeDelta)
{

    //UI 상수버퍼 바인딩
    UI_BUFFER_PACKET Packet;
    MSG_FAIL(Bind_ShaderResources(&Packet, fTimeDelta), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


    //렌더의 책임을 render component 에게넘긴다.
    //부모가 먼저 render된후,
    CHECK_NULLPTR(m_pUIRenderer);
 

     m_pUIRenderer->Render_UI(m_iShaderSlotNum,&Packet);


    //vector의 순서가 z-순서 (0->1->2순으로 그려짐)
    for (auto& pChild : m_vecChildren)
    {
        if(Is_WorldUI())
            m_pGameInstance->Set_DepthStencilState(DSSTATE::SKY);
        if (pChild->m_bIsVisible)
            pChild->Render(fTimeDelta);
 
    }

    return S_OK;
}

HRESULT Engine::UIObject::Bind_ShaderResources(UI_BUFFER_PACKET* pPacket, const _float fTimeDelta)
{
    CHECK_NULLPTR(pPacket);

    m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &fTimeDelta, sizeof(_float));

    pPacket->g_AlphaOffSet = m_Combined.m_fAlpha;
    pPacket->g_UVScroll = m_Combined.m_UVOffset;
    pPacket->g_fUIValue = m_fValue;



    //(추가):마스킹
    if (m_pMaskProgress && m_bMaskProgress)
    {
        pPacket->g_UIProgress = m_pMaskProgress->Get_RenderRatio();
        pPacket->g_fClipX = m_pMaskProgress->Get_GetClipX();
    }

    else


        for (auto& pComp : m_UICompBindList)
            pComp->Bind_Resource(m_pShaderCom, pPacket);


    return S_OK;
   

 
}





HRESULT Engine::UIObject::Add_NewRenderComponent(_uint ProtoTypeLevel, wstring PrototypeName, wstring componentName, void* pArg)
{
    CHECK_NULLPTR(pArg);

    HRESULT Result=Add_Component(ProtoTypeLevel, PrototypeName, componentName, RCAST(Component**)(&m_pUIRenderer), pArg);

#ifdef _DEBUG
    m_RendererName = componentName;
#endif // _DEBUG



    return Result;
}

HRESULT Engine::UIObject::Add_NewRenderComponent(wstring componentName, UIComponent* pComponent)
{
    if (m_umapComponents.contains(componentName))
    {
        _wstring message = componentName + L"컴포넌트가 이미 존재합니다.";
        _wstring caption = Get_Name() + L"오브젝트 컴포넌트 추가 실패";
        MSG_ON(message.c_str(), caption.c_str());
        BREAK;
        return E_FAIL;
    }

#ifdef _DEBUG
    if (m_pUIRenderer)
    {
        UI_Render* pNewRenderer = dynamic_cast<UI_Render*>(pComponent);
        if (pNewRenderer)
        {
            return Upgrade_RenderComponent(componentName, pNewRenderer);

        }
    }

#endif // _DEBUG

   
    m_umapComponents.emplace(componentName, pComponent);
    m_pUIRenderer = dynamic_cast<UI_Render*>(pComponent);

#ifdef _DEBUG
    m_RendererName = componentName;
#endif // _DEBUG
    return S_OK;
}

HRESULT Engine::UIObject::Add_NewUIComponent(UITYPE eType, _uint ProtoTypeLevel, wstring PrototypeName, wstring componentName, void* pArg)
{
    HRESULT Result=S_OK;
    if (pArg)
    {
        UIComponent::UICOMPDESC* pDesc = static_cast<UIComponent::UICOMPDESC*>(pArg);
        pDesc->pOwner = this;
    }

    if (eType == UITYPE::BUTTON)
    {
        Result = Add_Component(ProtoTypeLevel, PrototypeName, componentName, RCAST(Component**)(&m_pUIButton), pArg);

    }

 
    else
    {
        UIComponent* pUIComp = nullptr;
        Result = Add_Component(ProtoTypeLevel, PrototypeName, componentName, RCAST(Component**)(&pUIComp), pArg);

        if (pUIComp)
        {
            if(pUIComp->Is_BindAble())
                m_UICompBindList.push_back(pUIComp);
        }

        if (eType == UITYPE::PROGRESSBAR)
        {
            if (!m_pParent)
            {//자식에게전파
                Set_MaskProgress();
            }
        }
    }

    return Result;
}

HRESULT Engine::UIObject::Add_NewUIComponent(UITYPE eType, wstring componentName, UIComponent* pComponent)
{
    if (m_umapComponents.contains(componentName))
    {
        _wstring message = componentName + L"컴포넌트가 이미 존재합니다.";
        _wstring caption = Get_Name() + L"오브젝트 컴포넌트 추가 실패";
        MSG_ON(message.c_str(), caption.c_str());
        BREAK;
        return E_FAIL;
    }

    m_umapComponents.emplace(componentName, pComponent);
    if (eType == UITYPE::BUTTON)
        m_pUIButton = dynamic_cast<UI_Button*>(pComponent);

    if (pComponent->Is_BindAble())
        m_UICompBindList.push_back(pComponent);


    return S_OK;
}



//******************************************************* 렌더 함수 *******************************************************/


HRESULT Engine::UIObject::Bind_OrthoMatrices(Shader* shader) const
{
    MSG_FAIL(shader->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ViewMatrix", m_matViewMatrix), L"뷰 행렬 바인딩에 실패했습니다.", L"쉐이더 바인딩 실패!", E_FAIL);

    MSG_FAIL(shader->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ProjMatrix", m_matProjectionMatrix), L"투영 행렬 바인딩에 실패했습니다.", L"쉐이더 바인딩 실패!", E_FAIL);

    return S_OK;
}

HRESULT Engine::UIObject::Bind_OrthoMatrices(Shader* shader, const _string& viewmatrixname, const _string& projmatrixname) const
{
    MSG_FAIL(shader->Bind_Matrix_FullSlot(0, viewmatrixname, m_matViewMatrix), L"뷰 행렬 바인딩에 실패했습니다.", L"쉐이더 바인딩 실패!", E_FAIL);

    MSG_FAIL(shader->Bind_Matrix_FullSlot(0, projmatrixname, m_matProjectionMatrix), L"투영 행렬 바인딩에 실패했습니다.", L"쉐이더 바인딩 실패!", E_FAIL);

    return S_OK;
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



#pragma region 로컬위치변경함수
//////////////////////////////////////////////////////// 로컬 위치 변경 함수 ////////////////////////////////////////////////////////

void Engine::UIObject::Set_Position()
{
    //m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(m_fX - m_fWindowX * 0.5f, -m_fY + m_fWindowY * 0.5f, 0.f, 1.f));
}

void Engine::UIObject::Set_Position(_float _fX, _float _fY)
{
   
    m_Local.m_fX = _fX, m_Local.m_fY = _fY;
    m_bIsDirty = true;

    // m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(m_fX - m_fWindowX * 0.5f, -m_fY + m_fWindowY * 0.5f, 0.f, 1.f));
}

void Engine::UIObject::Set_Position_By_World(_float3 vWorldPos)
{
  
    _float4x4 fMatView = m_pGameInstance->Get_PipeLineMatrix(D3DTS_VIEW);
    _float4x4 fMatProj = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);


    _matrix matView = XMLoadFloat4x4(&fMatView);
    _matrix matProj = XMLoadFloat4x4(&fMatProj);

    _float fWinCX, fWinCY;
    m_pGameInstance->Get_Winsize(&fWinCX, &fWinCY);

 
    _vector vPos = XMLoadFloat3(&vWorldPos);
    vPos = XMVectorSetW(vPos, 1.f);

    _vector vViewPos = XMVector3TransformCoord(vPos, matView);
    _vector vProjPos = XMVector3TransformCoord(vViewPos, matProj);

    if (XMVectorGetZ(vViewPos) < 0.f)
        Set_Visible(false);

    else
        Set_Visible(true);

    _float w = XMVectorGetW(vProjPos); // 실제로는 TransformCoord 사용 시 1로 정규화됨
    _float x = XMVectorGetX(vProjPos);
    _float y = XMVectorGetY(vProjPos);

    _float fScreenX = (x + 1.f) * 0.5f * fWinCX;
    _float fScreenY = (1.f - y) * 0.5f * fWinCY; // Y는 위아래가 뒤집혀 있으니 (1 - y)

    Set_Position(fScreenX, fScreenY);

}

void Engine::UIObject::Set_Size()
{
   // m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
}

void Engine::UIObject::Set_Size(_float _fSizeX, _float _fSizeY)
{
   
    m_Local.m_fSizeX = _fSizeX;
    m_Local.m_fSizeY = _fSizeY;

    m_bIsDirty = true;

    //m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
}

void Engine::UIObject::Set_Color(_float4 vColor)
{
    if (m_pUIRenderer)
    {
        UI_Image* pImage = dynamic_cast<UI_Image*>(m_pUIRenderer);
        if (pImage)
        {
            pImage->Set_Color(vColor);
            return;
        }

        UI_Text* pText = dynamic_cast<UI_Text*>(m_pUIRenderer);
        if (pText)
        {
            pText->Set_Color(vColor);
            return;
        }
       
    }
}
void Engine::UIObject::Set_Visible(_bool _isVisible)
{
    __super::Set_Visible(_isVisible);
    if (!m_vecChildren.empty())
    {
        for (auto& pObj : m_vecChildren)
            pObj->Set_Visible(_isVisible);
    }
}

void Engine::UIObject::Set_Active(_bool _isActive)
{
    //CHECK_TRUE(m_pParent!=nullptr);

    //활성화는 애니메이션 재생과상관없이 바로 true만들기

    if (_isActive)
    {

        if (m_bIsVisible)
            m_bIsActive = true;
        /*Active애니메이션 재생*/
        m_bIsDirty = true;

		if (m_ActiveEvents[!_isActive])
		{
			Component* pAnim = Get_Component_FromName(L"Prototype_Component_UI_Animation");
			if (pAnim)
			{
				m_bIsVisible = true;
				UI_Animation* AnimationComp = dynamic_cast<UI_Animation*>(pAnim);
				if (AnimationComp)
					AnimationComp->Play_AnimClip(m_ActiveEvents[!_isActive]->strAnimClip);

				//사운드재생..
				//m_pSoundManager->PlaySound();
			}
		}
        

    }

    else
    {
        /*부모가 닫히느중이라면, 나도닫히게설정(전파)*/
        if (m_pParent)
        {
            if (m_pParent->m_bClosing)
                m_bClosing = true;
        }
      

        if (m_ActiveEvents[!_isActive])
        {
            m_bIsDirty = true;

            //애니메이션이있으면 애니메이션끝나고 처리하기
            Component* pAnim = Get_Component_FromName(L"Prototype_Component_UI_Animation");
            if (pAnim)
            {
                UI_Animation* AnimationComp = dynamic_cast<UI_Animation*>(pAnim);
                if (AnimationComp)
                {
                    AnimationComp->Play_AnimClip(m_ActiveEvents[!_isActive]->strAnimClip);
                    m_bIsVisible = true;
                    m_bClosing = true;


                    //콜백설정
                    UIAnimClip* pClip = AnimationComp->Get_AnimationClip(m_ActiveEvents[!_isActive]->strAnimClip);
                    if (pClip)
                        pClip->Bind_EndFunction([this]()
                            {
                                m_bClosing = false;
                                m_bIsActive = false;

                                if (m_pParent)
                                    m_pParent->m_bClosing = false;

                                if (m_DeActiveEvent)
                                    m_DeActiveEvent();
                            });
                }

            }


        }
        else
        {
            
            if (!m_bClosing)
            {
                m_bIsActive = false;
                if(m_DeActiveEvent)
                    m_DeActiveEvent();
            }


        }
           

    }


    if (!m_vecChildren.empty())
    {
        for (auto& pObj : m_vecChildren)
            pObj->Set_Active(_isActive);
    }

  /*  if (!_isActive && !m_ActiveEvents[!_isActive])
    {
        if (m_pParent == nullptr)
        {
            if(!m_bClosing)
                m_bIsActive = false;
        }
           

        else
        {
            if (m_pParent->m_bClosing == false)
                m_bIsActive = false;

        }

    }*/
}

//부모애니메이션만 적용시킬떄
void Engine::UIObject::Set_Active(_bool _isActive,_bool bUseAnim)
{

    //CHECK_TRUE(m_pParent!=nullptr);

    //활성화는 애니메이션 재생과상관없이 바로 true만들기
    if (_isActive)
    {

        if (m_bIsVisible)
            m_bIsActive = true;

        m_bIsDirty = true;
        /*Active애니메이션 재생*/
        if (bUseAnim)
        {
            if (m_ActiveEvents[!_isActive])
            {
                Component* pAnim = Get_Component_FromName(L"Prototype_Component_UI_Animation");
                if (pAnim)
                {
                    m_bIsVisible = true;
                    UI_Animation* AnimationComp = dynamic_cast<UI_Animation*>(pAnim);
                    if (AnimationComp)
                        AnimationComp->Play_AnimClip(m_ActiveEvents[!_isActive]->strAnimClip);

                    //사운드재생..
                    //m_pSoundManager->PlaySound();
                }
            }
        }
       
    }

    else
    {
        m_bIsDirty = true;
        if (m_ActiveEvents[!_isActive])
        {
            if (bUseAnim)
            {
              

                //애니메이션이있으면 애니메이션끝나고 처리하기
                Component* pAnim = Get_Component_FromName(L"Prototype_Component_UI_Animation");
                if (pAnim)
                {
                    UI_Animation* AnimationComp = dynamic_cast<UI_Animation*>(pAnim);
                    if (AnimationComp)
                    {
                        AnimationComp->Play_AnimClip(m_ActiveEvents[!_isActive]->strAnimClip);
                        m_bIsVisible = true;
                        m_bClosing = true;
                        if (m_pParent)
                            m_pParent->m_bClosing = true;

                        //콜백설정
                        UIAnimClip* pClip = AnimationComp->Get_AnimationClip(m_ActiveEvents[!_isActive]->strAnimClip);
                        if (pClip)
                            pClip->Bind_EndFunction([this]()
                                {
                                    m_bClosing = false;
                                    m_bIsActive = false;

                                    if (m_pParent)
                                        m_pParent->m_bClosing = false;
                                });
                    }

                }
            }
            else
                m_bIsActive = false;
           
        }
     

    }
  

    if (!m_vecChildren.empty())
    {
        for (auto& pObj : m_vecChildren)
            pObj->Set_Active(_isActive, false);
    }

    if (!_isActive && !m_ActiveEvents[!_isActive])
    {
        m_bIsActive = false;
      
    }

}

void Engine::UIObject::Set_Active_Force(_bool isActive)
{
    m_bIsActive = isActive;
    if (m_bClosing)
        m_bClosing = false;

}

void Engine::UIObject::Set_Active_Delay(_float fSecond)
{
    Set_Active(true);


    m_DeActiveAlarm.Limit = fSecond;
    m_DeActiveAlarm.m_AlarmFunc = [this]()
        {
            Set_Active(false);
            m_DeActiveAlarm.Off();
        };

    m_DeActiveAlarm.On();

}


void Engine::UIObject::Set_Dead(_bool _isalive)
{
    
    __super::Set_Dead(_isalive);
    /*자식이있을 경우 자식 모두삭제.*/


	if (!m_vecChildren.empty())
	{
		for (auto& pObj : m_vecChildren)
		{

			pObj->Set_Dead(_isalive);
			Safe_Release(pObj);
		}
	}
	m_vecChildren.clear();




#ifdef _DEBUG
    if (m_pGameInstance->Get_UISelectObject() == this)
    {
        m_pGameInstance->Set_SelectObject(nullptr);
        ImguiManager::GetInstance()->Reset_All_Window();
    }
#endif // _DEBUG


    if (m_pGameInstance->Get_ClickedUI() == this)
        m_pGameInstance->Clear_ClickedUI();

    if (m_pGameInstance->Get_HoveredUI() == this)
        m_pGameInstance->Clear_HoveredUI ();

}

/******************************************************* 위치 변경 함수 *******************************************************/
#pragma endregion



#pragma region 부모-자식함수
////////////////////////////////////////////////////////부모-자식함수////////////////////////////////////////////////////////
HRESULT Engine::UIObject::Add_Child(UIObject* pObj)
{
    CHECK_NULLPTR(pObj);
    CHECK_TRUE_RESULT(this == pObj, E_FAIL);
    //순환참조 체크
    if (!pObj->Get_Children()->empty())
    {
        for (auto iter = pObj->Get_Children()->begin(); iter != pObj->Get_Children()->end(); ++iter)
        {
            if ((*iter) == this)
                return E_FAIL;
        }

    }

    //이미존재하는 자식일경우 추가 X (이거도 재귀처리로 검사해야함)
    HRESULT hr = Check_Duplicate_Child(pObj);
    CHECK_TRUE_RESULT(hr == E_FAIL, E_FAIL);

    _float fParentAccScaleX = m_Combined.m_fSizeX / fOriginSizeX;
    _float fParentAccScaleY = m_Combined.m_fSizeY / fOriginSizeY;

    if (fParentAccScaleX <= 0.f) fParentAccScaleX = 1.f;
    if (fParentAccScaleY <= 0.f) fParentAccScaleY = 1.f;

    //1.나 자신의 월드 상태
    _float fWorldX = (m_Combined.m_fX != 0.f) ? m_Combined.m_fX : m_Local.m_fX;
    _float fWorldY = (m_Combined.m_fY != 0.f) ? m_Combined.m_fY : m_Local.m_fY;


  
    //내 누적 배율 계산 
    _float fParentScaleX = (m_Local.m_fSizeX != 0.f) ? (m_Combined.m_fSizeX / m_Local.m_fSizeX) : 1.f;
    _float fParentScaleY = (m_Local.m_fSizeY != 0.f) ? (m_Combined.m_fSizeY / m_Local.m_fSizeY) : 1.f;

    pObj->m_Local.m_fSizeX /= fParentAccScaleX;
    pObj->m_Local.m_fSizeY /= fParentAccScaleY;

    pObj->Set_Rotation(m_Local.m_fRotationZ + pObj->m_Local.m_fRotationZ);

    //자식의 월드상태,
  //자식이 이미 그룹화가 되어있으면 combined를, 아니라면 fx를 사용
    _float fChildWorldX = (pObj->m_Combined.m_fX != 0.f) ? pObj->m_Combined.m_fX : pObj->m_Local.m_fX;
    _float fChildWorldY = (pObj->m_Combined.m_fY != 0.f) ? pObj->m_Combined.m_fY : pObj->m_Local.m_fY;

    if (fParentScaleX <= 0.f)
        fParentScaleX = 1.f;

    if (fParentScaleY <= 0.f)
        fParentScaleY = 1.f;

    _float fParentAlpha= m_Local.m_fAlpha;
    _float2 fParentUVScroll = m_Local.m_UVOffset;

    // 자식의 새로운 상대 좌표 세팅
    pObj->Set_Position((fChildWorldX - m_Combined.m_fX) / fParentAccScaleX,
        (fChildWorldY - m_Combined.m_fY) / fParentAccScaleY);

  
    //상대 알파,UV스크롤 갱신(부모꺼누적)
    pObj->m_Combined.m_fAlpha = fParentAlpha * pObj->m_Local.m_fAlpha;
    pObj->m_Combined.m_UVOffset = fParentUVScroll + pObj->m_Local.m_UVOffset;



    m_vecChildren.push_back(pObj);
    Add_Child_OnMap(pObj);
    pObj->Set_Parent(this);


#ifdef _DEBUG
    UI_Text* pText = dynamic_cast<UI_Text*>(pObj->Get_Component_FromName(L"Prototype_Component_UI_Text"));
    if (pText)
        pText->Set_ParentMask();
#endif // _DEBUG


    //자식의 z-order는 같은부모내에서 누가 더 위에있느냐인지만 판단. 픽킹,렌더의 z-order는 부모zorder가 먼저기준임
    sort(m_vecChildren.begin(), m_vecChildren.end(), [](UIObject* pLeft, UIObject* pRight)
        {
            return pLeft->Get_ZOrder() < pRight->Get_ZOrder();
        });


    return S_OK;
}

HRESULT Engine::UIObject::Add_Child(GameObject* pObj)
{
    UIObject* pUIObj = dynamic_cast<UIObject*>(pObj);
    CHECK_NULL_RESULT(pUIObj, E_FAIL);

    Add_Child_OnLoad(pUIObj);

    return S_OK;
}

HRESULT Engine::UIObject::Add_Child_OnLoad(UIObject* pObj)
{
    CHECK_NULL_RESULT(pObj, E_FAIL);
    //중복 체크
    if (!pObj->Get_Children()->empty())
    {
        for (auto iter = pObj->Get_Children()->begin(); iter != pObj->Get_Children()->end(); ++iter)
        {
            if ((*iter) == pObj)
                return E_FAIL;
        }

    }
    m_vecChildren.push_back(pObj);
    Add_Child_OnMap(pObj);

 //   Safe_AddRef(pObj);
    pObj->Set_Parent(this);

    _float fParentAccScaleX = m_Combined.m_fSizeX / fOriginSizeX;
    _float fParentAccScaleY = m_Combined.m_fSizeY / fOriginSizeY;

    if (fParentAccScaleX <= 0.f) fParentAccScaleX = 1.f;
    if (fParentAccScaleY <= 0.f) fParentAccScaleY = 1.f;

    // Combined 업데이트 (부모 배율 전파)
    pObj->m_Combined.m_fSizeX = pObj->m_Local.m_fSizeX * fParentAccScaleX;
    pObj->m_Combined.m_fSizeY = pObj->m_Local.m_fSizeY * fParentAccScaleY;
    pObj->m_Combined.m_fX = m_Combined.m_fX + (pObj->m_Local.m_fX * fParentAccScaleX);
    pObj->m_Combined.m_fY = m_Combined.m_fY + (pObj->m_Local.m_fY * fParentAccScaleY);

    // Transform 반영
    pObj->m_pTransformCom->Set_Scale(pObj->m_Combined.m_fSizeX, pObj->m_Combined.m_fSizeY, 1.f);
    pObj->m_pTransformCom->Set_State(DIRECTION::POSITION,
        XMVectorSet(pObj->m_Combined.m_fX - m_fWindowX * 0.5f,
            -pObj->m_Combined.m_fY + m_fWindowY * 0.5f, 0.f, 1.f));

    //자식의 z-order는 같은부모내에서 누가 더 위에있느냐인지만 판단. 픽킹,렌더의 z-order는 부모zorder가 먼저기준임
    sort(m_vecChildren.begin(), m_vecChildren.end(), [](UIObject* pLeft, UIObject* pRight)
        {
            return pLeft->Get_ZOrder() < pRight->Get_ZOrder();
        });

    return S_OK;
}

HRESULT Engine::UIObject::Add_Child_OnLoad(GameObject* pObj)
{
    UIObject* pUIObj = dynamic_cast<UIObject*>(pObj);
    CHECK_NULL_RESULT(pUIObj, E_FAIL);

    Add_Child_OnLoad(pUIObj);

    return S_OK;
}

HRESULT Engine::UIObject::Add_Child_OnMap(UIObject* pObj)
{

    size_t hKey = pObj->m_NameHash;
    
    //중복체크
    auto iter = m_mapChildren.find(hKey);
    if (iter != m_mapChildren.end())
    {
        if (iter->second->Get_Name() != pObj->Get_Name()) {
            MSG_BOX("해시 충돌 발생! 이름을 바꾸기 ㄱㄱ.");
            return E_FAIL;
        }
    }

    m_mapChildren[hKey] = pObj;
    return S_OK;
}

HRESULT Engine::UIObject::Remove_Child_OnMap(UIObject* pObj)
{
    size_t hKey = pObj->m_NameHash;

    //중복체크
    auto iter = m_mapChildren.find(hKey);
    if (iter != m_mapChildren.end())
    {
        m_mapChildren.erase(iter);
    }

    return S_OK;
}

void Engine::UIObject::Set_Parent(UIObject* pParent)
{
    if (!pParent)
        return;

    m_pParent = pParent;

    //부모가있는경우에는 ui_manager에서 관리 대상 제외 / parenet가 모두관리
    m_pGameInstance->Remove_UIObject(this);
    
    // 만약 부모가 worldui라면 자식도 worldui
    if (m_pParent->Is_WorldUI())
        m_eRenderGroup = RENDER_GROUP::WORLD_UI;


    //게임오브젝트 레이어에서도 추가 제거
    if(m_pLayer)
        m_pLayer->Remove_GameObject(this);


    //부모의 Interactable 값을 따라가자.
    if (m_pUIRenderer)
    {
        UI_Render* pRenderer = m_pParent->Get_UIRenderComponent();
        if (pRenderer)
            m_pUIRenderer->Set_Interactable(pRenderer->Is_Interactable());

    }

    //부모의 progress가 있다면, 이를 캐싱하자.
    m_pMaskProgress = dynamic_cast<UI_Progress*>(m_pParent->Get_Component_FromName(L"Prototype_Component_UI_Progress"));


}
void Engine::UIObject::Detach_Child()
{
    for (auto iter = m_vecChildren.begin(); iter != m_vecChildren.end();)
    {

        Remove_Child_OnMap((*iter));

        if ((*iter)->m_pMaskProgress != nullptr)
            (*iter)->m_pMaskProgress = nullptr;

        (*iter)->m_pParent = nullptr;
        (*iter)->Set_Size((*iter)->m_Combined.m_fSizeX, (*iter)->m_Combined.m_fSizeY);
        (*iter)->Set_Position((*iter)->m_Combined.m_fX, (*iter)->m_Combined.m_fY);

        m_pGameInstance->Add_UIObject((*iter));



        Safe_Release((*iter));
        iter = m_vecChildren.erase(iter);
        

    }
}
void Engine::UIObject::Detach_Me_From_Parent()
{
    CHECK_JUST_NULL(m_pParent);
    vector<UIObject*>* Children = m_pParent->Get_Children();
    if (Children->empty())
        return;

    for (auto iter = Children->begin(); iter != Children->end(); ++iter)
    {
        if ((*iter) == this)
        {
           m_pMaskProgress = nullptr;
           Safe_Release((*iter));
           Children->erase(iter);

            return;
        }
    }
   
}
HRESULT Engine::UIObject::Check_Duplicate_Child(UIObject* pObj)
{
    HRESULT hr;
    for (auto iter = m_vecChildren.begin(); iter != m_vecChildren.end(); ++iter)
    {
        if (*iter == pObj)
            return E_FAIL;

        if (!(*iter)->Get_Children()->empty())
        {
            hr=(*iter)->Check_Duplicate_Child(pObj);
            CHECK_TRUE_RESULT(hr == E_FAIL, E_FAIL);
        }
     

    }
    return S_OK;
}
UIObject* Engine::UIObject::Get_Child(wstring wstrName)
{
    CHECK_TRUE_RESULT(m_vecChildren.empty(), nullptr);

    size_t HashKey = hash<wstring>{}(wstrName);

    UIObject* pResult = nullptr;
    auto iter = m_mapChildren.find(HashKey);

    if (iter == m_mapChildren.end())
    {
        //자식에서 찾기
        for (auto& pChild : m_vecChildren)
        {
            UIObject* ppChild= pChild->Get_Child(wstrName);
            if (ppChild)
                return ppChild;
        }
           
    }

    else
        pResult = (iter->second);

    return pResult;
}
UIObjectInfo Engine::UIObject::Save_To_Json()
{


    //현재 내 상태를 담을 UIInfo
    //root노드가 호출해서 재귀적으로 자식까지 호출

    UIObjectInfo UIInfo;


    //1.기본정보
    string strName = wstringToString(m_wstrName);
    size_t pos = strName.find(to_string(m_iObjectNumber));

    if (pos != string::npos)
    {
        strName=strName.substr(0,pos);
    }

    UIInfo.strObjectKey = strName;

    UIInfo.UIBaseType = m_BaseType;
    UIInfo.UIType = m_UIType;
    UIInfo.m_bMaskProgress = m_bMaskProgress;

    UIInfo.UIDataStr = m_UIDataStr;

    if (m_pParent)
        UIInfo.strParentName = wstringToString(m_pParent->Get_Name());
    UIInfo.iZOrder = m_iZOrder;



    UIInfo.ShaderComName = wstringToString(m_wstrShaderName);
    UIInfo.bIsBlurUI = m_bBlurUI;
    UIInfo.m_eRenderGroup = m_eRenderGroup;


    //2.상태값
    UIInfo.bVisible = m_bIsVisible;
    UIInfo.bActive = m_bIsActive;

    UIInfo.bIsInteratable = Is_Interactable();
    UIInfo.iShaderPass = m_iShaderSlotNum;

    //3. transform
    UIInfo.m_Local = m_Local;

    

    //Hitbox
    UIInfo.m_HitBoxPadding.x= m_LocalPadding.x;
    UIInfo.m_HitBoxPadding.y=m_LocalPadding.y ;

    UIInfo.m_HitBoxSize = m_HitBoxSize;


    //다른값
    UIInfo.m_fValue = m_fValue;

    //4.이벤트정보
    if (m_ActiveEvents[0])
        UIInfo.m_ActiveEvent[0] = *m_ActiveEvents[0];


    if (m_ActiveEvents[1])
        UIInfo.m_ActiveEvent[1] = *m_ActiveEvents[1];

    //5.컴포넌트(정렬 후 저장)
    vector<Component*> SortVector;
    for (auto& pair : m_umapComponents) {
        if (pair.second) SortVector.push_back(pair.second);
    }

    // 우선순위 기준 오름차순 정렬
    sort(SortVector.begin(), SortVector.end(), [](Component* a, Component* b) {
        return a->Get_SavePriority() < b->Get_SavePriority();
        });

    for (auto& pComp : SortVector)
    {
        if (pComp)
        {
            UIComponent* pUIComp = dynamic_cast<UIComponent*>(pComp);
            if (!pUIComp)
                continue;

            ComponentData Data;
            
            wstring ProtoName = pComp->Get_PrototypeName();
            if (ProtoName == L"" )
                continue;

            Data.strComType = wstringToString(ProtoName);
            pComp->Save_Data(Data.ComJson);

            if(Data.ComJson!=NULL)
                UIInfo.vecComponents.push_back(Data);
        }

    }

    //6.자식저장
    if (!m_vecChildren.empty())
    {
        sort(m_vecChildren.begin(), m_vecChildren.end(), [](UIObject* pLeft, UIObject* pRight)
            {
                return pLeft->Get_ZOrder() < pRight->Get_ZOrder();
            });
    }

    for (auto& pChild : m_vecChildren)
    {
        UIObjectInfo ChildInfo=pChild->Save_To_Json();
        UIInfo.m_Children.push_back(ChildInfo);

    }

    return UIInfo;
}
void Engine::UIObject::Apply_Data_From_Info(UIObjectInfo& Info)
{
   // m_wstrName = stringToWstring(Info.strObjectKey);
    m_iZOrder = Info.iZOrder;
    m_bIsVisible = Info.bVisible;
    m_bIsActive = Info.bActive;

    
    m_bBlurUI = Info.bIsBlurUI;

    m_wstrShaderName = stringToWstring(Info.ShaderComName);

    m_BaseType = Info.UIBaseType;
    m_iBaseType = hash<string>{}(m_BaseType);

    m_UIType = Info.UIType;
    m_iUIType = hash<string>{}(m_UIType);
    m_eRenderGroup = Info.m_eRenderGroup;

    
    m_fValue = Info.m_fValue;
    m_UIDataStr = Info.UIDataStr;


    m_Local = Info.m_Local;
    m_LocalPadding.x = Info.m_HitBoxPadding.x;
    m_LocalPadding.y = Info.m_HitBoxPadding.y;

    m_HitBoxSize = Info.m_HitBoxSize;

    m_fInitSize.x = Info.m_Local.m_fSizeX;
    m_fInitSize.y = Info.m_Local.m_fSizeY;

    m_bMaskProgress = Info.m_bMaskProgress;

    //이벤트추가
    if (Info.m_ActiveEvent[0].strAnimClip != "")
    {
        m_ActiveEvents[0] = new UI_ActiveEvent();
        m_ActiveEvents[0]->strAnimClip = Info.m_ActiveEvent[0].strAnimClip;
        m_ActiveEvents[0]->strSoundFile = Info.m_ActiveEvent[0].strSoundFile;
    }

    if (Info.m_ActiveEvent[1].strAnimClip != "")
    {
        m_ActiveEvents[1] = new UI_ActiveEvent();
        m_ActiveEvents[1]->strAnimClip = Info.m_ActiveEvent[1].strAnimClip;
        m_ActiveEvents[1]->strSoundFile = Info.m_ActiveEvent[1].strSoundFile;
    }

    //컴포넌트복구
    for (auto& pCompData : Info.vecComponents)
    {
        UIComponent::UICOMPDESC Desc;
        Desc.pOwner = this;

        wstring     ProtoCompName = stringToWstring(pCompData.strComType);
        Base* pComp = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, ProtoCompName, nullptr);

        if (pComp)
        {
            UIComponent* pUIComp = dynamic_cast<UIComponent*>(pComp);
            if (pUIComp)
            {
                UITYPE eType;
                pUIComp->Load_Data(this, pCompData,&eType);
                Add_Component_After_Load(eType, ProtoCompName, pUIComp);

            }
          
            
        }

        
    }

    Set_Interatable(Info.bIsInteratable);
 //   After_ApplyData();
 
}
void Engine::UIObject::Add_Component_After_Load(UITYPE eType, wstring componentName, UIComponent* pComp)
{
    //렌더러설정
    if (eType == UITYPE::ROOT || eType == UITYPE::IMAGE || eType == UITYPE::SPRITE || eType == UITYPE::TEXT||eType==UITYPE::BITMAPTEXT||eType==UITYPE::SLOTGRID)
        Add_NewRenderComponent(componentName, pComp);


    //설정
    else
        Add_NewUIComponent(eType, componentName, pComp);


}
void Engine::UIObject::Add_ActiveEvent(bool bActive, UI_ActiveEvent& pEvent)
{
}
void Engine::UIObject::Update_Recursive(const _float fTimeDelta, _float fParentfX, _float fParentfY, _float fParentScalefX, _float fParentScalefY, _float fRotationZ,_float fAlpha, _float2 fUVOffSet, bool bParentDirty)
{
    for (auto& pCom : m_umapComponents)
    {
        if (pCom.second)//
            pCom.second->Update(fTimeDelta);
    }

    //제일 최상위인 경우 m_fCombined=m_fX , m_fCombinedSizeX=m_fsizeX;
    //[위치계산]

     /*UI 컴포넌트 매프레임마다 돌아갈것들 (ex,버튼호버처리 / 스프라이트애니메이션)*/
    //부모꺼 먼저 처리한후,
  
    _bool bNeedUpdate = m_bIsDirty || bParentDirty;
    _float fMyCurrentScaleX = m_Combined.m_fSizeX / fOriginSizeX;
    _float fMyCurrentScaleY = m_Combined.m_fSizeY / fOriginSizeY;

    if (bNeedUpdate)
    {
        m_Combined.m_fX = fParentfX + (m_Local.m_fX * fParentScalefX);
        m_Combined.m_fY = fParentfY + (m_Local.m_fY * fParentScalefY);

        //[크기계산]
        m_Combined.m_fSizeX = fParentScalefX * m_Local.m_fSizeX;
        m_Combined.m_fSizeY = fParentScalefY * m_Local.m_fSizeY;


        fMyCurrentScaleX = m_Combined.m_fSizeX / fOriginSizeX;
        fMyCurrentScaleY = m_Combined.m_fSizeY / fOriginSizeY;

        _float CombinedAlpha = fAlpha * m_Local.m_fAlpha;
        _float2 CombinedUV = _float2(m_Combined.m_UVOffset.x + fUVOffSet.x, m_Combined.m_UVOffset.y + fUVOffSet.y);
        m_Combined.m_fRotationZ = fRotationZ + m_Local.m_fRotationZ;

        //Transform반영
        if (m_eRenderGroup == RENDER_GROUP::UI)
        {
            m_pTransformCom->Set_Scale(m_Combined.m_fSizeX, m_Combined.m_fSizeY, 1.f);
            m_pTransformCom->Set_State(DIRECTION::POSITION,
                XMVectorSet(m_Combined.m_fX - m_fWindowX * 0.5f, -m_Combined.m_fY + m_fWindowY * 0.5f, 0.f, 1.f));

            m_pTransformCom->Rotation(0.f, 0.f, XMConvertToRadians(m_Combined.m_fRotationZ));

        }

        else
        {
            if (m_pParent)
            {
                m_Combined.m_fZ = m_pParent->Get_Combined().m_fZ + (m_Local.m_fZ + m_iZOrder * -0.01f);
                m_Combined.m_fRotationY = m_pParent->Get_Combined().m_fRotationY + m_Local.m_fRotationY;
            }
            else
            {
                m_Combined.m_fRotationY = m_Local.m_fRotationY;
                m_Combined.m_fZ = m_Local.m_fZ;
            }
               
            m_pTransformCom->Set_Scale(m_Combined.m_fSizeX, m_Combined.m_fSizeY, 1.f);
            m_pTransformCom->Set_State(DIRECTION::POSITION,
                XMVectorSet(m_Combined.m_fX , m_Combined.m_fY ,m_Combined.m_fZ, 1.f));

            m_pTransformCom->Rotation(0.f, XMConvertToRadians(m_Combined.m_fRotationY), XMConvertToRadians(m_Combined.m_fRotationZ));

        }
        /*애니메이션을 위한 알파, uv offset 설정*/
        m_Combined.m_fAlpha = CombinedAlpha;
        m_Combined.m_UVOffset = CombinedUV;

        m_bIsDirty = false;
  
    }
    


    //자식 update돌리기
    for (auto& pChild : m_vecChildren)
    {
        //스케일->비율을넘겨줘야한다.
        pChild->Update(fTimeDelta);
        pChild->Update_Recursive(fTimeDelta, m_Combined.m_fX, m_Combined.m_fY, fMyCurrentScaleX, fMyCurrentScaleY, m_Combined.m_fRotationZ,m_Combined.m_fAlpha, m_Combined.m_UVOffset, bNeedUpdate);
       
    }
        

}

void Engine::UIObject::Update_World_Recursive(const _float fTimeDelta, _matrix matParentWorld, _float fAlpha)
{
    for (auto& pCom : m_umapComponents)
    {
        if (pCom.second)//
            pCom.second->Update(fTimeDelta);
    }

    if (m_bIsDirty)
    {
      
        _float fBaseScale = 1.f;

       
        if (m_pParent != nullptr) {
            fBaseScale = 0.01f; 
        }

        m_pTransformCom->Set_Scale(m_Local.m_fSizeX * fBaseScale,
            m_Local.m_fSizeY * fBaseScale, 1.f);

 
        m_pTransformCom->Rotation(XMConvertToRadians(0.f),
            XMConvertToRadians(m_Local.m_fRotationY),
            XMConvertToRadians(m_Local.m_fRotationZ));

       
        _float fFinalLocalX = m_Local.m_fX * fBaseScale;
        _float fFinalLocalY = m_Local.m_fY * fBaseScale;
        _float fFinalLocalZ = (m_pParent == nullptr) ? m_Local.m_fZ : (m_iZOrder * -0.001f);

        m_pTransformCom->Set_State(DIRECTION::POSITION,
            XMVectorSet(fFinalLocalX, fFinalLocalY, fFinalLocalZ, 1.f));

       
        _matrix matMyWorld = m_pTransformCom->Get_WorldMatrix() * matParentWorld;
        m_pTransformCom->Set_Matrix(matMyWorld);

        m_Combined.m_fAlpha = m_Local.m_fAlpha*fAlpha;
        // 자식들에게 전파
        for (auto& pChild : m_vecChildren) {
            pChild->m_bIsDirty = true;
            pChild->Update_World_Recursive(fTimeDelta, matMyWorld, m_Combined.m_fAlpha);
        }
    }

}


void Engine::UIObject::Set_AnimValue(UIANIMTYPE eType, _float3 vResult)
{
    switch (eType)
    {
    case Engine::UIANIMTYPE::PROGRESS:
    {
        if(m_pProgress==nullptr)
            m_pProgress = static_cast<UI_Progress*>(Get_Component_FromName(L"Prototype_Component_UI_Progress"));
        
        if (m_pProgress) {

            m_pProgress->Set_Ratio(vResult.x);
            m_pProgress->Set_TargetRatio(vResult.x);
            m_pProgress->Set_RenderRato(vResult.x);

        }
    }
        break;
    case Engine::UIANIMTYPE::END:
        break;
    default:
        break;
    }
}

void Engine::UIObject::Execute_By_Event(const string& strActionName, void* pArg)
{

}

void Engine::UIObject::Reset_AnimationComp()
{
    UI_Animation* pAnim = dynamic_cast<UI_Animation*>( Get_Component_FromName(L"Prototype_Component_UI_Animation"));
    if (pAnim)
    {
        pAnim->Reset_Animation();
    }

}

void Engine::UIObject::Bind_SetActiveFalse(function<void()> Func)
{
    m_DeActiveEvent = Func;
}

void Engine::UIObject::Play_Animation(string AnimClipName)
{
    CHECK_TRUE(AnimClipName == "");
    Component* pComp = Get_Component_FromName(L"Prototype_Component_UI_Animation");
    if (pComp)
    {

        UI_Animation* pAnimComp = dynamic_cast<UI_Animation*>(pComp);
        if (pAnimComp)
        {
            m_bIsVisible = true;
            pAnimComp->Play_AnimClip(AnimClipName);

        }
    }


    for (auto& pChild : m_vecChildren)
        pChild->Play_Animation(AnimClipName);
}

void Engine::UIObject::Stop_Animation()
{
    Component* pComp = Get_Component_FromName(L"Prototype_Component_UI_Animation");
    if (pComp)
    {

        UI_Animation* pAnimComp = dynamic_cast<UI_Animation*>(pComp);
        if (pAnimComp)
            pAnimComp->Stop_Animation();
    }


    for (auto& pChild : m_vecChildren)
        pChild->Stop_Animation();
}



/******************************************************* 부모-자식함수 *******************************************************/
#pragma endregion


#ifdef  _DEBUG
////////////////////////////////////////////////////////Debug 함수//////////////////////////////////////////////////////////
void Engine::UIObject::Move(_float _speed, const _float fTimeDelta)
{
    UIObject* pObject = m_pGameInstance->Get_UISelectObject();

    if (pObject && pObject != this)
        return;



    if (m_pGameInstance->KeyPress(DIK_UP))
    {
        Move_Up(_speed, fTimeDelta);
        COUT("Pos : \n" << "X : " << m_Local.m_fX << "\tY : " << m_Local.m_fY);
    }
    if (m_pGameInstance->KeyPress(DIK_DOWN))
    {
        Move_Down(_speed, fTimeDelta);
        COUT("Pos : \n" << "X : " << m_Local.m_fX << "\tY : " << m_Local.m_fY);
    }
    if (m_pGameInstance->KeyPress(DIK_LEFT))
    {
        Move_Left(_speed, fTimeDelta);
        COUT("Pos : \n" << "X : " << m_Local.m_fX << "\tY : " << m_Local.m_fY);
    }
    if (m_pGameInstance->KeyPress(DIK_RIGHT))
    {
        Move_Right(_speed, fTimeDelta);
        COUT("Pos : \n" << "X : " << m_Local.m_fX << "\tY : " << m_Local.m_fY);
    }
    if (m_pGameInstance->KeyPress(DIK_PGUP))
    {
        m_Local.m_fSizeX += _speed * fTimeDelta;
        m_pTransformCom->Set_Scale(m_Local.m_fSizeX, m_Local.m_fSizeY, 1.f);
        COUT("Scale : \n" << "CX : " << m_Local.m_fSizeX << "\tCY : " << m_Local.m_fSizeY);
    }
    if (m_pGameInstance->KeyPress(DIK_PGDN))
    {
        m_Local.m_fSizeX -= _speed * fTimeDelta;
        m_pTransformCom->Set_Scale(m_Local.m_fSizeX, m_Local.m_fSizeY, 1.f);
        COUT("Scale : \n" << "CX : " << m_Local.m_fSizeX << "\tCY : " << m_Local.m_fSizeY);
    }
    if (m_pGameInstance->KeyPress(DIK_NUMPADPLUS))
    {
        m_Local.m_fSizeY += _speed * fTimeDelta;
        m_pTransformCom->Set_Scale(m_Local.m_fSizeX, m_Local.m_fSizeY, 1.f);
        COUT("Scale : \n" << "CX : " << m_Local.m_fSizeX << "\tCY : " << m_Local.m_fSizeY);
    }
    if (m_pGameInstance->KeyPress(DIK_NUMPADMINUS))
    {
        m_Local.m_fSizeY -= _speed * fTimeDelta;
        m_pTransformCom->Set_Scale(m_Local.m_fSizeX, m_Local.m_fSizeY, 1.f);
        COUT("Scale : \n" << "CX : " << m_Local.m_fSizeX << "\tCY : " << m_Local.m_fSizeY);
    }
}

void Engine::UIObject::Move_Down(_float _speed, const _float fTimeDelta)
{
    m_Local.m_fY += _speed * fTimeDelta;
    Set_Position(m_Local.m_fX, m_Local.m_fY);
}

void Engine::UIObject::Move_Up(_float _speed, const _float fTimeDelta)
{
    m_Local.m_fY -= _speed * fTimeDelta;
    Set_Position(m_Local.m_fX, m_Local.m_fY);
}

void Engine::UIObject::Move_Left(_float _speed, const _float fTimeDelta)
{
    m_Local.m_fX -= _speed * fTimeDelta;
    Set_Position(m_Local.m_fX, m_Local.m_fY);
}

void Engine::UIObject::Move_Right(_float _speed, const _float fTimeDelta)
{
    m_Local.m_fX += _speed * fTimeDelta;
    Set_Position(m_Local.m_fX, m_Local.m_fY);
}

void Engine::UIObject::Append_ActiveEvent(bool bActive)
{
    int iTargetIdx = bActive ? 0 : 1;

	if (!m_ActiveEvents[iTargetIdx])
		m_ActiveEvents[iTargetIdx] = new UI_ActiveEvent();


}

void Engine::UIObject::Set_PassNumber_To_Chlid(_uint i)
{
    
    Set_PassNum(i);

    for (auto& pChild : m_vecChildren)
        pChild->Set_PassNumber_To_Chlid(i);
}

HRESULT Engine::UIObject::Upgrade_RenderComponent(wstring ComponentName,UI_Render* pNewRenderer)
{
    if (typeid(*m_pUIRenderer) == typeid(*pNewRenderer))
    {
        Safe_Release(pNewRenderer);
        return E_FAIL;
    }

    wstring oldName = m_RendererName;
    m_umapComponents.erase(oldName);

    Safe_Release(m_pUIRenderer);
    m_pUIRenderer = pNewRenderer;
    m_umapComponents.emplace(ComponentName,m_pUIRenderer);

    return S_OK;
}

void Engine::UIObject::Set_Lock(bool b)
{
    m_bLock = b;
    for (auto& pChild : m_vecChildren)
        pChild->Set_Lock(b);

}

void Engine::UIObject::Render_IMGUI()
{

    Render_UIInfo();
    /*ui transform은 따로 호출 .(transform으로 하기에는 좀 처리가 애매함)*/
    Render_UITransform();
    Render_ActiveEvent();

    //긱 컴포넌트 디버그 띄우기
    for (auto& pComp : m_umapComponents)
    {
        if (pComp.second)
            pComp.second->Render_Imgui();
    }
}
void Engine::UIObject::Render_UIInfo()
{
    char buffer[128] = "";
    char oldName[128] = "";
    strcpy_s(buffer, wstringToString(m_wstrName).c_str());
    strcpy_s(oldName, wstringToString(m_wstrName).c_str());

    /*이름변경 =엔터 눌러야함*/
    bool bEnterPressed = ImGui::InputText("-Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue);
    bool bLostFocus = ImGui::IsItemDeactivatedAfterEdit();
    if (bEnterPressed || bLostFocus)
    {
        if (strcmp(buffer, oldName)!=0)
        {
            if (m_pParent)
                Remove_Child_OnMap(this);

            m_wstrName = stringToWstring(buffer);
            Set_HashName();


            if (m_pParent == nullptr)
            {
                if (stringToWstring(oldName) != m_wstrName)
                    m_pGameInstance->Rename_Object(stringToWstring(oldName), this);

            }

            else
            {
                //새로추가
                Add_Child_OnMap(this);
            }
        }
        
    }

    ImGui::DragInt("-Zorder", (int*)&m_iZOrder);

    ImGui::DragFloat("Value", (float*)&m_fValue);
   

    if (ImGui::Checkbox("-Lock", &m_bLock))
    {
        Set_Lock(m_bLock);
    }

    if (ImGui::Checkbox("-Visible", &m_bIsVisible))
    {
         Set_Visible(m_bIsVisible);
    }

    bool bActive = m_bIsActive;
    if (ImGui::Checkbox("-Active", &bActive))
    {
        Set_Active(bActive);
    }

    if (ImGui::Checkbox("-MaskProgress", &m_bMaskProgress))
    {
        for (auto& pChild : m_vecChildren)
            pChild->m_bMaskProgress = m_bMaskProgress;
        
        if (m_bMaskProgress)
            Set_MaskProgress();
    }


    if (m_pUIRenderer)
    {
        bool bInteractable = m_pUIRenderer->Is_Interactable();
        if (ImGui::Checkbox("-Interactable", &bInteractable))
        {
            Set_Interatable(bInteractable);
        }

    }
    const SHADERENTRIES Entrys = m_pShaderCom->Get_ShaderEntries();
    //임시 const char배열 할당
    int     iShaderSlotNum = (int)m_iShaderSlotNum;

    vector<const char*>     m_ShaderPassNames;
    m_ShaderPassNames.reserve(Entrys.iNumpass);
    for (_uint i = 0; i < Entrys.iNumpass; ++i)
    {
        m_ShaderPassNames.push_back(Entrys.pEntries[i].psEntry.c_str());
    }

    if (ImGui::Combo("PassList", &iShaderSlotNum, m_ShaderPassNames.data(), (int)m_ShaderPassNames.size()))
    {
        m_iShaderSlotNum = iShaderSlotNum;
    }

    if (ImGui::Button("Set PassNumber To Child"))
    {
        Set_PassNumber_To_Chlid(m_iShaderSlotNum);
    }

    char Buff[1024];
    strcpy_s(Buff, sizeof(Buff), wstringToString(m_UIDataStr).c_str());
    if (ImGui::InputTextMultiline("UIDataStr:", Buff, sizeof(Buff)))
    {
        m_UIDataStr = stringToWstring(Buff);
    }

}
void Engine::UIObject::Render_UITransform()
{
    
    ImGui::Separator();

    /*로컬좌표기준*/
    _float2 vPos = { m_Local.m_fX,m_Local.m_fY };
    

    if (ImGui::DragFloat2("Position", &vPos.x, 0.1f))
    {
       

        Set_Position(vPos.x, vPos.y);
    }


    _float2 vScale = { m_Local.m_fSizeX,m_Local.m_fSizeY };

    if (ImGui::DragFloat2("Scale", &vScale.x, 0.1f))
    {
       
        Set_Size(vScale.x, vScale.y);

    }

    ImGui::DragFloat("RotationY", &m_Local.m_fRotationY, 0.1f);
    ImGui::DragFloat("RotationZ", &m_Local.m_fRotationZ, 0.1f);

    ImGui::DragFloat("Alpha", &m_Local.m_fAlpha,0.1f,0.f,1.f);
    
    if (ImGui::DragFloat2("HitPadding", &m_LocalPadding.x, 0.1f))
    {
    }

    if (ImGui::DragFloat2("HitBoxSize", &m_HitBoxSize.x, 0.1f))
    {
    }
}
void Engine::UIObject::Render_ActiveEvent()
{
    if (ImGui::CollapsingHeader("Life-Cycle Events (Active/Disable)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const char* labels[] = { "On Active", "On Disable" };

        for (int i = 0; i < 2; ++i)
        {
            ImGui::PushID(i);

            if (m_ActiveEvents[i] == nullptr)
            {
                // 할당되지 않았을 때는 생성 버튼
                ImGui::Text("%s : No Event", labels[i]);
                ImGui::SameLine();
                string str;

                if (i == 0)
                    str = "Add ActiveEvent";

                else
                    str = "Add Disactive Event";

                if (ImGui::Button(str.c_str()))
                {
                    bool b = (bool)i;
                    Append_ActiveEvent(!b);
                }
                   
            }
            else
            {
                //할당되었을 때 (설정 UI 노출)
                bool bOpen = ImGui::TreeNodeEx(labels[i], ImGuiTreeNodeFlags_DefaultOpen);

                // 오른쪽에 삭제 버튼 배치
                ImGui::SameLine(ImGui::GetWindowWidth() - 70);
                if (ImGui::Button("Remove", ImVec2(60, 0)))
                {
                    Safe_Delete(m_ActiveEvents[i]); // 메모리 해제 및 nullptr 세팅
                    if (bOpen) ImGui::TreePop();
                    ImGui::PopID();
                    continue;
                }

                if (bOpen)
                {
                    //상세 설정
                    char szAnim[128];
                    strcpy_s(szAnim, m_ActiveEvents[i]->strAnimClip.c_str());
                    if (ImGui::InputText("Anim Clip", szAnim, 128))
                        m_ActiveEvents[i]->strAnimClip = szAnim;

                    char szSound[128];
                    strcpy_s(szSound, m_ActiveEvents[i]->strSoundFile.c_str());
                    if (ImGui::InputText("Sound Key", szSound, 128))
                        m_ActiveEvents[i]->strSoundFile = szSound;

                    ImGui::TreePop();
                }
            }

            ImGui::PopID();
            if (i == 0) ImGui::Separator();
        }
    }
}
void Engine::UIObject::Transform_With_Mouse(const _float fTImeDelta)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse) {
        // 마우스가 ImGui 패널 위에 있으므로 게임의 Picking 로직을 무시하고 리턴
        return;
    }
    CHECK_JUST_NULL(m_pMouse);
    CHECK_FALSE(m_pGameInstance->Get_UISelectObject() == this);
    CHECK_TRUE(m_pGameInstance->Get_EngineMode() == ENGINEMODE::CLIENT);

    POINT pt=m_pMouse->Get_MousePos();
    RECT Engine{};
    GetClientRect(eg_hWnd, &Engine);
    CHECK_FALSE(PtInRect(&Engine, pt));

    // 드래그 시작 시점 (딱 한 번 실행)
    if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(eg_hWnd, &pt);

        m_vDragStartMousePos = { (_float)pt.x, (_float)pt.y };
        m_vDragStartObjPos = { m_Local.m_fX, m_Local.m_fY };
        m_vDragStartObjSize = { m_Local.m_fSizeX, m_Local.m_fSizeY };
        m_eResizeDir = Get_ResizeDirection();
        m_bDragEnter = true;
    }

    if (m_pGameInstance->MousePress(MOUSEKEYSTATE::LB) && m_bDragEnter)
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(eg_hWnd, &pt);
        _float fDiffX = (_float)pt.x - m_vDragStartMousePos.x;
        _float fDiffY = (_float)pt.y - m_vDragStartMousePos.y;

        if (m_eResizeDir == RESIZE_DIRECTION::DIR_NONE)
        {
            //이동
            m_Local.m_fX = m_vDragStartObjPos.x + fDiffX;
            m_Local.m_fY = m_vDragStartObjPos.y + fDiffY;
            Set_Position(m_Local.m_fX, m_Local.m_fY);
        }
        else
        {
            Resize_ByDiff(m_eResizeDir, fDiffX, fDiffY);
        }
    }

    if (m_pGameInstance->MouseUp(MOUSEKEYSTATE::LB))
    {
        m_bDragEnter = false;
        m_eResizeDir = RESIZE_DIRECTION::DIR_NONE;
    }
}

void Engine::UIObject::Resize_ByDiff(RESIZE_DIRECTION eDir, _float fDiffX, _float fDiffY)
{
    switch (eDir)
    {
    case RESIZE_DIRECTION::DIR_R:
       
        Set_Size(m_vDragStartObjSize.x + fDiffX, m_vDragStartObjSize.y);
        break;
    case RESIZE_DIRECTION::DIR_L:
        
        Set_Size(m_vDragStartObjSize.x - fDiffX, m_vDragStartObjSize.y);
        break;
    
    case RESIZE_DIRECTION::DIR_B:
        Set_Size(m_vDragStartObjSize.x, m_vDragStartObjSize.y-fDiffY);
        break;


    case RESIZE_DIRECTION::DIR_T:
        Set_Size(m_vDragStartObjSize.x, m_vDragStartObjSize.y+fDiffY);
        break;


    case RESIZE_DIRECTION::DIR_LT:
        Set_Size(m_vDragStartObjSize.x - fDiffX, m_vDragStartObjSize.y +fDiffY);
        break;
    case RESIZE_DIRECTION::DIR_RT:
        Set_Size(m_vDragStartObjSize.x + fDiffX, m_vDragStartObjSize.y+fDiffY);
        break;

    case RESIZE_DIRECTION::DIR_LB:
        Set_Size(m_vDragStartObjSize.x - fDiffX, m_vDragStartObjSize.y - fDiffY);
        break;
    case RESIZE_DIRECTION::DIR_RB:
        Set_Size(m_vDragStartObjSize.x + fDiffX, m_vDragStartObjSize.y - fDiffY);
        break;

    }



}

UIObject::RESIZE_DIRECTION Engine::UIObject::Get_ResizeDirection()
{
    CHECK_TRUE_RESULT(m_eEditMode == EDITMODE::MOVE, UIObject::RESIZE_DIRECTION::DIR_NONE);
    CHECK_NULL_RESULT(m_pMouse, UIObject::RESIZE_DIRECTION::DIR_NONE);
    POINT   pt;
    GetCursorPos(&pt);
    ScreenToClient(eg_hWnd, &pt);


    //이 영역의 left,right,bottom,top을구한다
    m_LocalHitBoxArea.L = (m_Combined.m_fX - m_Combined.m_fSizeX * 0.5f) - m_LocalPadding.x;
    m_LocalHitBoxArea.R = (m_Combined.m_fX + m_Combined.m_fSizeX * 0.5f) + m_LocalPadding.x;
    m_LocalHitBoxArea.T = (m_Combined.m_fY - m_Combined.m_fSizeY * 0.5f) - m_LocalPadding.y;
    m_LocalHitBoxArea.B = (m_Combined.m_fY + m_Combined.m_fSizeY * 0.5f) + m_LocalPadding.y;


    m_HitBoxSize.x = m_Combined.m_fSizeX + (m_LocalPadding.x*2.f);
    m_HitBoxSize.y = m_Combined.m_fSizeY + (m_LocalPadding.y*2.f);

    //감지범위 
    const float margin = 5.f;
    
    bool inInLeft = (pt.x >= m_LocalHitBoxArea.L - margin && pt.x <= m_LocalHitBoxArea.L + margin) ? true : false;
    bool inInRight = (pt.x >= m_LocalHitBoxArea.R - margin && pt.x <= m_LocalHitBoxArea.R + margin) ? true : false;
    bool inInBotton = (pt.y >= m_LocalHitBoxArea.T - margin && pt.y <= m_LocalHitBoxArea.T + margin) ? true : false;
    bool inInTop = (pt.y >= m_LocalHitBoxArea.B - margin && pt.y <= m_LocalHitBoxArea.B + margin) ? true : false;

    if (inInLeft)
    {
        m_eEditMode = EDITMODE::SCALE;
        if (inInTop)
            return RESIZE_DIRECTION::DIR_LT;

        if (inInBotton)
            return RESIZE_DIRECTION::DIR_LB;
       
        return RESIZE_DIRECTION::DIR_L;

    }

    if (inInRight)
    {
        m_eEditMode = EDITMODE::SCALE;
        if (inInTop)
            return RESIZE_DIRECTION::DIR_RT;

        if (inInBotton)
            return RESIZE_DIRECTION::DIR_RB;
       
        return RESIZE_DIRECTION::DIR_R;

    }

    m_eEditMode = EDITMODE::NONE;
    return RESIZE_DIRECTION::DIR_NONE;
}
void Engine::UIObject::Resize_ByMouse(RESIZE_DIRECTION eDir, long DeltaX, long DeltaY)
{
    /*한쪽방향으로만 크기조절하기위해선 움직임까지 고려해야함*/
    switch (eDir)
    {
    case Engine::UIObject::RESIZE_DIRECTION::DIR_L:
        Set_Size(m_Local.m_fSizeX - DeltaX, m_Local.m_fSizeY);
        break;

    case Engine::UIObject::RESIZE_DIRECTION::DIR_R:
        Set_Size(m_Local.m_fSizeX +DeltaY, m_Local.m_fSizeY);
        break;
    case Engine::UIObject::RESIZE_DIRECTION::DIR_T:
        Set_Size(m_Local.m_fSizeX , m_Local.m_fSizeY+DeltaY);
        break;
    case Engine::UIObject::RESIZE_DIRECTION::DIR_B:
        Set_Size(m_Local.m_fSizeX, m_Local.m_fSizeY - DeltaY);
        break;
    case Engine::UIObject::RESIZE_DIRECTION::DIR_LT:
        Set_Size(m_Local.m_fSizeX - DeltaX, m_Local.m_fSizeY + DeltaY);

        break;
    case Engine::UIObject::RESIZE_DIRECTION::DIR_RT:
        Set_Size(m_Local.m_fSizeX + DeltaX, m_Local.m_fSizeY + DeltaY);
        break;

    case Engine::UIObject::RESIZE_DIRECTION::DIR_LB:
        Set_Size(m_Local.m_fSizeX - DeltaX, m_Local.m_fSizeY - DeltaY);
        break;

    case Engine::UIObject::RESIZE_DIRECTION::DIR_RB:
        Set_Size(m_Local.m_fSizeX + DeltaX, m_Local.m_fSizeY - DeltaY);
        break;

    default:
        break;
    }
}
HRESULT Engine::UIObject::Copy(UIObject** pOut)
{
   
    return S_OK;

}
/******************************************************* Debug함수 *******************************************************/

#endif //  _DEBUG


//////////////////////////////////////////////////////// 피킹 함수 ////////////////////////////////////////////////////////
_bool Engine::UIObject::IsMouseOver(const POINT& _mousePos)
{
    // 스크린 좌표 기준 AABB 검사
    m_LocalHitBoxArea.L = (m_Combined.m_fX - m_Combined.m_fSizeX * 0.5f) - m_LocalPadding.x;
    m_LocalHitBoxArea.R = (m_Combined.m_fX + m_Combined.m_fSizeX * 0.5f) + m_LocalPadding.x;
    m_LocalHitBoxArea.T = (m_Combined.m_fY - m_Combined.m_fSizeY * 0.5f) - m_LocalPadding.y;
    m_LocalHitBoxArea.B = (m_Combined.m_fY + m_Combined.m_fSizeY * 0.5f) + m_LocalPadding.y;

    m_HitBoxSize.x = m_Combined.m_fSizeX + (m_LocalPadding.x*2.f);
    m_HitBoxSize.y = m_Combined.m_fSizeY + (m_LocalPadding.y*2.f);

    return (_mousePos.x >= m_LocalHitBoxArea.L && _mousePos.x <= m_LocalHitBoxArea.R &&
        _mousePos.y >= m_LocalHitBoxArea.T && _mousePos.y <= m_LocalHitBoxArea.B);
}

UIObject* Engine::UIObject::Pick_Recursive(ENGINEMODE eMode, const POINT& _mousePos,_vector vRayPos, _vector vRayDir)
{
    // 1. 기본 상태 확인 (비활성화되어 있거나 안 보이면 즉시 리턴)
    if (!m_bIsActive || !m_bIsVisible)
        return nullptr;

    // 자식 요소들부터 역순(그려지는 순서의 역순)으로 검사
    for (auto iter = m_vecChildren.rbegin(); iter != m_vecChildren.rend(); ++iter)
    {
      
        UIObject* pPicked = (*iter)->Pick_Recursive(eMode, _mousePos, vRayPos, vRayDir);

        if (pPicked)
            return pPicked; // 깊은 곳에서 찾았다면 즉시 반환
    }
    //월드 ui인경우 추가
    if (Is_WorldUI()&& Is_Interactable())
    {
     /*   _vector vRaypos, vRayDir;
        m_pMouse->Get_MouseRay(vRaypos, vRayDir);
    */

        //레이를 ui로컬로변경
        _float4x4 m_World = Get_Transform()->Get_WorldFloat4x4();
        _matrix InvWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4( & m_World));

        // 레이를 UI 로컬 공간으로 변환
        _vector vLocalRayPos = XMVector3TransformCoord(vRayPos, InvWorld);
        _vector vLocalRayDir = XMVector3TransformNormal(vRayDir, InvWorld);
        
        
        float fDirZ = XMVectorGetZ(vLocalRayDir);

        if (fabsf(fDirZ) > 0.0001f)
        {
            float t = -XMVectorGetZ(vLocalRayPos) / fDirZ;

            if (t >= 0.f) // 카메라 앞쪽일 때
            {
                _vector vIntersectPos = vLocalRayPos + vLocalRayDir * t;
                float x = XMVectorGetX(vIntersectPos);
                float y = XMVectorGetY(vIntersectPos);
                if (x >= -0.5f && x <= 0.5f && y >= -0.5f && y <= 0.5f)
                {
                    WCOUT(L"월드UI충돌:" << m_wstrName);
                    return this;
                }
            }
        }
        return nullptr; // 월드 UI인데 안 맞았으면 끝
    }





    //자식들 중에 마우스에 걸리는 게 없다면, 이제 '나 자신'을 검사
    if (eMode == ENGINEMODE::CLIENT)
    {
        // 상호작용 가능하고 영역 안에 마우스가 있을 때
        if (Is_Interactable() && IsMouseOver(_mousePos))
            return this;
    }

#ifdef _DEBUG
    //엔진에서는 Lock과 mouseover을 검사
    else
    {
        if (!Is_Lock() && IsMouseOver(_mousePos))
            return this;
    }
#endif // _DEBUG

    
    return nullptr;
}

void Engine::UIObject::OnClick()
{
#ifdef _DEBUG
    m_pGameInstance->Set_SelectObject(this);
#endif // _DEBUG

    ENGINEMODE eMode = m_pGameInstance->Get_EngineMode();

    if(eMode!=ENGINEMODE::CLIENT)
        m_eSelectMode = SELECTSTATE::SELECT;

    if (eMode == CLIENT)
    {
        string AnimName = "";

        if (m_pUIButton)
        {
            m_pUIButton->Change_ButtonState(UI_Button::PRESSED);

            AnimName = m_pUIButton->Get_HoverEventAnimClipName();
        }
		//자식이 있다면 자식에게도 전파.
		for (auto& pChild : m_vecChildren)
		{
			if (!pChild->m_pUIButton)
				pChild->Play_Animation(AnimName);

			else
				pChild->OnClick();
		}
    }

    if (m_OnClickEvent)
        m_OnClickEvent();

  
}


void Engine::UIObject::OnClickCancle()
{
#ifdef _DEBUG
    m_pGameInstance->Set_SelectObject(nullptr);
    m_eSelectMode = SELECTSTATE::NONESELECT;
    m_eEditMode = EDITMODE::NONE;
#endif // _DEBUG

}

void Engine::UIObject::OnHover()
{
   
   
}

void Engine::UIObject::OnHoverEnter()
{
    if (m_pGameInstance->Get_EngineMode() == CLIENT)
    {
        string AnimName = "";
        if (m_pUIButton)
        {
            m_pUIButton->Change_ButtonState(UI_Button::HOVER);
            AnimName = m_pUIButton->Get_HoverEventAnimClipName();
        }
        //자식이 있다면 자식에게도 전파.
        for (auto& pChild : m_vecChildren)
        {
            if (!pChild->m_pUIButton)
                pChild->Play_Animation(AnimName);

            else
                pChild->OnHoverEnter();
        }

    }
   
}

void Engine::UIObject::OnHoverExit()
{
    if (m_pGameInstance->Get_EngineMode() == CLIENT)
    {
        string AnimName = "";
        if (m_pUIButton)
            m_pUIButton->OnHoverExit();

        //자식이 있다면 자식에게도 전파.
        for (auto& pChild : m_vecChildren)
        {
            if (!pChild->m_pUIButton)
                pChild->Play_Animation(AnimName);

            else
                pChild->OnHoverExit();
        }
    }
   

}

void Engine::UIObject::OnDraggingExit()
{
#ifdef _DEBUG
    m_eEditMode = EDITMODE::NONE;
    m_bDragEnter = false;
#endif //_DEBUG 


}



/******************************************************* 피킹 함수 *******************************************************/


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
UIObject* Engine::UIObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
    UIObject* pInstance = new UIObject(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObject 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Engine::UIObject::Clone(void* arg)
{
    UIObject* pInstance = new UIObject(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"UIObject 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::UIObject::Free()
{

	__super::Free();


    if (!m_vecChildren.empty())
    {
        for (auto& pChild : m_vecChildren)
            Safe_Release(pChild);
    }
    m_vecChildren.clear();


    for (auto& pActive : m_ActiveEvents)
        Safe_Delete(pActive);

    
}
void Engine::UIObject::Set_MaskProgress()
{
    CHECK_FALSE(m_bMaskProgress);
    if (m_pParent)
    {
        UI_Progress* pParentComp = dynamic_cast<UI_Progress*>(m_pParent->Get_Component_FromName(L"Prototype_Component_UI_Progress"));

        if (pParentComp)
        {
             m_pMaskProgress = pParentComp;
        }
        else
        {
            m_pMaskProgress = m_pParent->m_pMaskProgress;
        }
    }


    for (auto& pChild : m_vecChildren)
        pChild->Set_MaskProgress();

}
/******************************************************* 객체 반환 함수 *******************************************************/

///////////////////////////////////////////////////////나머지 함수//////////////////////////////////////////////////////////////
bool Engine::UIObject::Is_Interactable()
{
    if (m_pUIRenderer)
        return m_pUIRenderer->Is_Interactable();
    return false;
}

void Engine::UIObject::Set_Interatable(bool b)
{
    if (m_pUIRenderer)
        m_pUIRenderer->Set_Interactable(b);

 /*   for (auto& pChild : m_vecChildren)
        pChild->Set_Interatable(b);*/

}

void Engine::UIObject::Set_Alpha(_float _fAlpha)
{
    m_bIsDirty = true;
    m_Local.m_fAlpha = clamp(_fAlpha, 0.f, 1.f);
}

void Engine::UIObject::Set_UVOffSet(_float2 OffSet)
{
    m_bIsDirty = true;

    m_Combined.m_UVOffset = OffSet;

}

void Engine::UIObject::Set_Rotation(_float fRotation)
{
    //Z축회전
    m_bIsDirty = true;

    m_Local.m_fRotationY = fRotation;
}

void Engine::UIObject::Set_Combined_Poistion(_float fX, _float fY,_float fZ)
{
    m_bIsDirty = true;
    m_Combined.m_fX = fX;
    m_Combined.m_fY = fY;
    m_Combined.m_fZ = fZ;

    if (m_pParent == nullptr)
    {
        m_Local.m_fX = m_Combined.m_fX;
        m_Local.m_fY = m_Combined.m_fY;
        m_Local.m_fZ = m_Combined.m_fZ;

    }

}


_float4 Engine::UIObject::Get_Color()
{
    if (m_pUIRenderer)
    {
        UI_Image* pImage = dynamic_cast<UI_Image*>(m_pUIRenderer);
        if (pImage)
            return pImage->Get_CurrentColor();

        else
            return _float4();
    }

    else
        return _float4();

    
}

_float4 Engine::UIObject::Get_OriginColor()
{
    if (m_pUIRenderer)
    {
        UI_Image* pImage = dynamic_cast<UI_Image*>(m_pUIRenderer);
        if (pImage)
            return pImage->Get_OriginColor();


        UI_Text* pText = dynamic_cast<UI_Text*>(m_pUIRenderer);
        if (pText)
            return pText->Get_OriginColor();


        else
            return _float4();
    }

    else
        return _float4();
}


void        Engine::UIObject::After_ApplyData()
{
    Set_MaskProgress();
}

void Engine::UIObject::Release_Resources()
{
    
	// 구독목록 전부 해제
	for (auto& handle : m_vecSubscribeNumbers)
	{
		m_pGameInstance->UnsubScribe(handle);
	}
	m_vecSubscribeNumbers.clear();
}
/******************************************************* 나머지 함수 *******************************************************/

