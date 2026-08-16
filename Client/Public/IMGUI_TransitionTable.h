#pragma once
#include "Client_Define.h"
#include "ImguiWindow.h"
#include "MState_TransitionTypes.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

NS_BEGIN(Client)

class Monster;
class MState_TransitionTable;

class IMGUI_TransitionTable final : public ImguiWindow
{
private:
    explicit IMGUI_TransitionTable();
    explicit IMGUI_TransitionTable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~IMGUI_TransitionTable();

public:
    HRESULT Initialize(void* arg);
    _uint   Update_Contents(_float fTimeDelta) override final;
    _int    Render(const _float fTimeDelta);

private:
    void    Draw_StateSelector();
    void    Draw_RuleList();
    void    Draw_RuleEditor();
    void    Draw_ConditionEditor(TRANSITION_COND& _cond, _uint _index);
    void    Draw_Buttons();

private:
    // 상태/조건 이름 변환용
    const char* Get_StateName(_uint _iState) const;
    const char* Get_ConditionName(TRANSITION_CONDITION _eCond) const;

private:
    GameInstance* m_pGameInstance = { nullptr };
    Monster* m_pTargetMonster = { nullptr };
    MState_TransitionTable* m_pTable = { nullptr };

    // 편집 상태
    _uint                   m_iSelectedFrom = {};       // 현재 선택된 from 상태
    _int                    m_iSelectedRule = { -1 };   // 현재 선택된 룰 인덱스
    _wstring                m_wstrSavePath = {};

public:
    static IMGUI_TransitionTable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);

public:
    void Free() override final;
};

NS_END