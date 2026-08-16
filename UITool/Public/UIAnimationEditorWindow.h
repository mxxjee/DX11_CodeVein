#pragma once
#include "ImguiWindow.h"

namespace Engine
{
    class GameInstance;
    class UIObject;
    class UI_Animation;
    class UIAnimClip;
}

NS_BEGIN(UITool)
class UIAnimationEditorWindow :
    public ImguiWindow
{
public:
 
protected:
    UIAnimationEditorWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~UIAnimationEditorWindow() = default;

public:
    static UIAnimationEditorWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free();
public:
    //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta); //
    virtual     _int            Reset();
private:
    void        Render_AnimationClip();
    void        Render_EditArea();
                //트랙 삭제시 true반환
    bool        Render_TrackSettings(UIAnimTrack* pTrack,int iIndex);
    void        Render_TimelineBar();
    void        Render_AddTrack(UIAnimClip* pClip);

    UIAnimTrack* Create_NewTrack(UIANIMTYPE eType);
    _float3      Get_CurrentValue(UIANIMTYPE eType);

    void        Copy_To_Original(UIAnimClip* pCopy);
private:

private:
    string Get_AnimTypeName(UIAnimTrack*  pTrack);

private:
    void        Change_Currentclip();

private:
    GameInstance* pGameInstance = { nullptr };
    UIObject* m_pSelectObject = { nullptr };

    UI_Animation*   m_pSelectAnimationComponent = nullptr;
    UIAnimClip*     m_pCurrentClip = nullptr;

    //save버튼 을 눌러야 animclip이 저장하게하기위해
    UIAnimClip* m_pCloneClip = nullptr;

private:
    vector<UIAnimClip*>     m_Garbage;
};
NS_END
