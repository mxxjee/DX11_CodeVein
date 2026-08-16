#pragma once

#include "Base.h"

/* 이벤트 매니저 */
/* 객체간의 상호작용을 매니저를 통해서 전달하기 위해 설계 */
/* 객체들의 결합도가 낮아짐 */
/* 디자인 패턴중 Observer패턴과 Publish-Subscribe (Pub/Sub) 패턴의 느낌 */

NS_BEGIN(Engine)

class GameInstance;

class EventManager final : public Base
{
private:
	explicit EventManager();
	explicit EventManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~EventManager();

public:
	HRESULT Initialize();
	_int	Update_Priority(const _float fTimeDelta);
	_int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	/* 구독 함수(템플릿 함수라서 헤더파일에) */
	/* 콜백을 원하는 객체가 이벤트 매니저에 원하는 항목을 구독 */
	template<typename Event>
	EventHandle Subscribe(function<void(const Event&)> _callback)
	{
		// 통관번호 발행
		EventHandle handle = m_iNextHandle++;

		// 사용자의 콜백을 void* 형태로 변환해서 타입을 소거(하나의 컨테이너에 저장하기 위해)
		// function<반환타입(매개변수)> 변수명;
		// 람다 함수
		function<void(const void*)> wrappedCallback = [_callback](const void* pEventData)
			{
				// void*를 원래의 타입으로 캐스팅해서 사용자 콜백 호출
				const Event* event = CAST(const Event*)(pEventData);
				_callback(*event);
			};

		// 리스너 정보 생성
		Listner_Info info;
		info.iListnerNum = handle; // 리스너의 번호
		info.callback = wrappedCallback; // 리스너가 구독한 항목(콜백 받을 항목, 항목들)

		// 이벤트 타입을 Key로 만들어서 저장
		type_index typeindex(typeid(Event));
		m_umapListner[typeindex].push_back(info);

		m_umapHandleToType.emplace(handle, typeindex);

		// 통관번호 리턴해서 객체가 저장하게
		return handle;
	}

	/* 이벤트 발행 함수 */
	/* 이 이벤트를 구독한 모든 리스너들에게 전달 */
	template<typename Event>
	void Publish(const Event& _eventData)
	{
		// 이벤트 타입으로 key 생성
		type_index typeindex(typeid(Event));

		auto iter = m_umapListner.find(typeindex);
		if (iter == m_umapListner.end())
			return;

		// Pending이 있을 때만 검사
		bool hasPending = !m_setPendingRemove.empty();

		for (const auto& listner : iter->second)
		{
			// Pending 있고, Pending 이벤트가 있는 경우에만 continue
			if (hasPending && m_setPendingRemove.count(listner.iListnerNum))
				continue;

			listner.callback(&_eventData);
		}
	}

	/* 구독취소 함수 */
	/* 구독한 이벤트를 해제하여 댕글링 포인터 방지 */
	void UnsubScribe(EventHandle _handlenum)
	{
		auto findIter = m_umapHandleToType.find(_handlenum);
		if (findIter == m_umapHandleToType.end())
			return;

		m_setPendingRemove.insert(_handlenum);
	}

	/* 보내려는 이벤트 데이터를 저장해뒀다가 Update_Priority 이 전 시점에 Publish하기 위한 함수 */
	/* 이벤트를 한 프레임 저장함 */
	template<typename Event>
	void Publish_Stack(Event&& _eventData)
	{
		m_vecStackEventData.push_back(
			[this, captured = std::forward<Event>(_eventData)]()
			{
				Publish<std::decay_t<Event>>(captured);
			});
	}

	HRESULT Publish_StackedEvent() {
		for (auto& event : m_vecStackEventData)
		{
			event();
		}
		m_vecStackEventData.clear();

		return S_OK;
	}

	/* 프레임 끝에 호출하여 Pending 삭제 리스너를 일괄 정리 */
	void Flush_PendingRemove()
	{
		for (EventHandle handle : m_setPendingRemove)
		{
			auto typeIter = m_umapHandleToType.find(handle);
			if (typeIter == m_umapHandleToType.end())
				continue;

			auto mapIter = m_umapListner.find(typeIter->second);
			if (mapIter != m_umapListner.end())
			{
				auto& listners = mapIter->second;
				std::erase_if(listners, [handle](const Listner_Info& info)
					{
						return info.iListnerNum == handle;
					});
			}
			m_umapHandleToType.erase(typeIter);
		}
		m_setPendingRemove.clear();
	}


private:
	/* 이벤트 매니저 구독자와 이벤트 정보를 담고있는 컨테이너 */
	/* Key: 이벤트 타입 (예: typeid(DamageEvent)) */
	/* Value: 해당 타입을 구독한 리스너들 */
	/* type_index : typeid를 하나의 변수로 쓸 수 있게 만든것 */
	/* typeid : 클래스나 구조체같은거 타입 그러니까 이 클래스의 typeid는 EventManager, Base를 상속받고 있지만 typeid는 EventManager로 나옴 */
	UMAP<type_index, vector<Listner_Info>> m_umapListner;

	/* handle -> type_index 역방향 맵 (빠른 타입 조회) */
	UMAP<EventHandle, type_index> m_umapHandleToType;
	/* Lazy 삭제 대기 목록 */
	unordered_set<EventHandle> m_setPendingRemove;


	/* 다음에 부여할 번호 */
	EventHandle m_iNextHandle = {};

	
	vector<function<void()>> m_vecStackEventData;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };

public:
	static EventManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
