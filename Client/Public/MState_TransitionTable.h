#pragma once
#include "Client_Define.h"
#include "MState_TransitionTypes.h"
#include "Monster.h"
#include "Base.h"

NS_BEGIN(Client)

class MState_TransitionTable final : public Base
{
public:
    explicit MState_TransitionTable();
    ~MState_TransitionTable() = default;

public:
    HRESULT Initialize(const _wstring& _filePath);

public:
    // JSON 파일에서 전이 테이블 로드
    HRESULT Load(const _wstring& _filePath);

    // 특정 상태의 전이 룰 목록 조회 (없으면 nullptr)
    const vector<TRANSITION_RULE>* Find_Rules(_uint _iFromState) const;

    // 수동으로 룰 추가 (JSON 안 쓰고 코드로 세팅할 때)
    void Add_Rule(_uint _iFromState, const TRANSITION_RULE& _rule);

    // 전체 클리어
    void Clear();

    
    // 핫 리로드용(IMGUI)
    HRESULT Reload(const _wstring& _filePath);

    // IMGUI편집용 Get
    vector<Client::TRANSITION_RULE>* Get_Rules_Mutable(_uint _iFromState);

    // JSONC 저장
    HRESULT Save(const _wstring& _filePath) const;

    // 전이 테이블 삭제 
    void    Remove_Rule(_uint _iFromState, _uint _iRuleIndex);

    // 전이 테이블 정렬
    void    Sort_Rules_Public(_uint _iFromState) { Sort_Rules(_iFromState); }


private:
    // 우선순위순 정렬
    void Sort_Rules(_uint _iFromState);

private:
    // key: 상태 enum(_uint), value: 해당 상태에서의 전이 룰 목록
    vector<TRANSITION_RULE>     m_arrTable[Monster::MS_END];

public:
    static MState_TransitionTable* Create(const _wstring& _filePath);

public:
    void Free() override final;
};

NS_END