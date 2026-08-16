#include "AnimationTool_Define.h"
#include "Window_AnimTimeline.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Animation.h"
#include "ImguiManager.h"
#include "Window_AnimList.h"

Window_AnimTimeline::Window_AnimTimeline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ImguiWindow(pDevice, pContext)
{
}

HRESULT Window_AnimTimeline::Initialize(void* pArg)
{
	m_pGameInstance = GameInstance::GetInstance();

	__super::Initialize(pArg);

	return S_OK;
}

_uint Window_AnimTimeline::Update_Contents(_float fTimeDelta)
{
    ImGui::Begin("Animation_Timeline");

    Draw_Timeline(fTimeDelta);

    ImGui::End();

    return 0;
}

void Window_AnimTimeline::Draw_Timeline(_float fTimeDelta)
{
    ImguiWindow* pWindow = ImguiManager::GetInstance()->Find_Window("Animation_List");
    if (pWindow == nullptr)
        return;

    Window_AnimList* pAnimList = dynamic_cast<Window_AnimList*>(pWindow);
    if (pAnimList == nullptr)
        return;

    AnimToolData* pData = pAnimList->Get_AnimToolData();
    if (pData == nullptr || pData->pModelCom == nullptr)
        return;

    m_pAnimToolData = pData;

    if (m_pAnimToolData->pModelCom == nullptr)
        return;


    //애니메이션 타임라인 영역

    if (m_pAnimToolData->pSelectedAnimation == nullptr)
    {
        ImGui::TextDisabled("Select an animation.");
        return;
    }

    _float fDurtaion = m_pAnimToolData->pSelectedAnimation->Get_Duration(); //현재 재생중인 애니메이션의 전체 재생 길이 ( Tick 단위임 )
    _float fCurrentFrame = m_pAnimToolData->pSelectedAnimation->Get_CurrentFrame(); //이게 이제 현재 재생중인 애니메이션의 키프레임 , 처음 시작은 0일것이고
    _float fTicksPerSecond = m_pAnimToolData->pSelectedAnimation->Get_Animation_Speed(); //이름이 헷갈리긴하는데 이게 이제 애니메이션 재생 속도(초당 Tick 변화량),이거 디버그용도로 띄우주자

    //fTickPerSecond가 1이면 60인듯

    //ex) m_fDuration =300.f ,m_fTickPersecond=30.f 이라면 이 애니메이션은 300Tick동안 진행되며 1초=30Tick이니까 총 재생시간은 10초다.(필기 해놨던거)
    //이제 초단위로 내가 보여주려면 fDuration과 fCurrenFrame을 이 애니메이션의 fTickPerSeCond로 나누면 이게 이제 

    _float fCurrentTime = fCurrentFrame / fTicksPerSecond; //초 단위 현재 애니메이션 재생 시간
    _float fTotalTime = fDurtaion / fTicksPerSecond;  //초 단위 현재 애니메이션 전체 재생 시간

    ImGui::SeparatorText("Timeline");
    ImGui::Text("Time: %.3f / %.3f (Tick : %.3f / %.3f , TickPerSecond: %.3f)", //위에 변수들 값 보여주기
        fCurrentTime, fTotalTime, fCurrentFrame, fDurtaion, fTicksPerSecond);

    _float fCanvasHeight = 240.f; //캔버스 높이
    ImGui::BeginChild("##TimelineCanvas", ImVec2(0, fCanvasHeight), true, ImGuiWindowFlags_NoScrollbar);

    ImDrawList* DrawList = ImGui::GetWindowDrawList(); //현재 창의 그림을 그릴수 있게 해주는 도구(붓/캔버스)
    ImVec2 ScreenPos = ImGui::GetCursorScreenPos(); //값의 기준은 모니터의 0,0이지만 실제 그 값들이 가르키는 장소는 지금 내 창의 시작점
    ImVec2 Avail = ImGui::GetContentRegionAvail(); //현재 윈도우의 우측 하단 한계점에서 현재 커서 위치를 뺀값 , 즉 공간이 얼마나 남았는지,창 이동 및 크기 조절 대비

    if (Avail.y < 10.f) Avail.y = 10.f; //최소 확보 

    _float fRulerHeight = 28.f; //타임라인 상단의 자(시간 표시줄)가 차지할 고정 높이
    _float fTrackTop = ScreenPos.y + fRulerHeight; //트랙이 시작되는 Y축 절대 좌표로 , 나중에 AddLine이나 AddRect로 트랙을 그릴때, 이 지점부터 그리기 시작,시간 표시줄은 더한값
    _float fTrackHegiht = Avail.y - fRulerHeight; //자를 제외하고 트랙들이 실제로 그려질 순수 높이 / 창 크기 조절해도 자의 높이는 28로 고정, 트랙 영역만 유동적으로 움직이게

    //투명 버튼용으로 , 타임라인 어디를 클릭하든 이 투명 버튼이 이벤트를 가로채서 "어디를 클릭했는지"정보를 알려주는 함수
    ImGui::InvisibleButton("##TIMELINE_BUTTON",Avail, 
        ImGuiButtonFlags_MouseButtonLeft |ImGuiButtonFlags_MouseButtonRight |ImGuiButtonFlags_MouseButtonMiddle); //마우스 좌,우,휠 모두 감지

    _bool bHoverd = ImGui::IsItemHovered(); //마우스 올렸을때 호버 상태인지
    _bool bAtctive = ImGui::IsItemActive(); //마우스로 클릭하고 있는 상태인지

    _float fWidthPixel = (Avail.x > 1.f) ? Avail.x : 1.f; //타임라인에 그려질 실제 픽셀 너비, Avail.x로 사용가능한 가로 공간을 가져옴

    _float fVisibleTime = (fTotalTime > 0.f) ? (fTotalTime / m_ViewData.fZoom) : 1.f; //보이는 시간 범위로 줌으로 확대 및 축소 가능
    fVisibleTime = (fVisibleTime < 0.1f) ? 0.1f : fVisibleTime;
    _float fTimePerPixel = fVisibleTime / fWidthPixel; //픽셀당 초 , 현실 시간과 컴퓨터의 픽셀 사이를 연결해줌, ex) 5초 지점이 화면 어디에 그려져야하는지, 마우스 300px 지점이 몇초인지 

    _float fMaxPan = (fTotalTime > fVisibleTime) ? (fTotalTime - fVisibleTime) : 0.f; //타임라인의 맨 끝까지 가기 위해 오른쪽으로 밀 수 있는 최대 시간 값
    m_ViewData.fPanTime = clamp(m_ViewData.fPanTime, 0.f, fMaxPan);

    if (bHoverd)
    {
        _float fWheel = ImGui::GetIO().MouseWheel;

        if (fWheel != 0.f && fTotalTime > 0.f) //휠이 0이 아니면 휠로 움직였다는 뜻, fTotalTime의 조건은 0나누기 방지 및 방어코드
        {
            _float fMouseX = ImGui::GetIO().MousePos.x;
            _float fMouseTime = m_ViewData.fPanTime + 
                (fMouseX - ScreenPos.x) * fTimePerPixel; //상대적 픽셀 위치 * 픽셀을 시간으로 변환 

            _float fZoomMul = (fWheel > 0.f) ? 1.1f : (1.f / 1.1f); //휠을 10% 단위로 확대,축소를 제어하기 위해
            m_ViewData.fZoom = clamp(m_ViewData.fZoom * fZoomMul, 1.0f, 50.0f);

            //줌 갱신후 fTimePerPixel(픽셀당 초) 재계산
            fVisibleTime = (fTotalTime > 0.f) ? (fTotalTime / m_ViewData.fZoom) : 1.f;
            fVisibleTime = (fVisibleTime < 0.1f) ? 0.1f : fVisibleTime;
            fTimePerPixel = fVisibleTime / fWidthPixel;

            //줌을 하기 전과 후에 마우스 커서가 가리키는 타임라인의 시간 지점이 화면의 똑같은 위치에 고정 되게 만들기 위해서
            m_ViewData.fPanTime = fMouseTime - (fMouseX - ScreenPos.x) * fTimePerPixel;
            m_ViewData.fPanTime = clamp(m_ViewData.fPanTime, 0.f, (fTotalTime > fVisibleTime) ? (fTotalTime - fVisibleTime) : 0.f);

        }

    }

    if (bAtctive && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) //마우스 클릭 + 오른쪽 마우스 버튼 
    {
        _float fDeltaX = ImGui::GetIO().MouseDelta.x; //지난 프레임과 이번 프레임 사이에 마우스가 가로로 몇 픽셀이나 움직였는가
        m_ViewData.fPanTime -= fDeltaX * fTimePerPixel; //오른쪽 드래그하면 시간은 왼쪽으로 이동하는 느낌
        m_ViewData.fPanTime = clamp(m_ViewData.fPanTime, 0.f, fMaxPan);
    }

    if (bAtctive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) //좌클릭 드래그 
    {
        //마우스가 이동한 만큼의 시간을 더해서 현재 가리키는 실제 시간을 구함.
        _float fMouseX = ImGui::GetIO().MousePos.x;
        _float fTime = m_ViewData.fPanTime + (fMouseX - ScreenPos.x) * fTimePerPixel;
        fTime = clamp(fTime, 0.f, fTotalTime);

        _float fNewCurrentTime = fTime * fTicksPerSecond;
        m_pAnimToolData->pSelectedAnimation->Set_CurrentFrame(fNewCurrentTime);

        m_pAnimToolData->bScrubbing = true;
        m_pAnimToolData->pModelCom->Play_Animation_AnimationTool(0.f);

    }
    else
    {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) //마우스를 놓았을때
        {
            m_pAnimToolData->bScrubbing = false;
        }
    }

    //배경 , 구분선 그리기
    DrawList->AddRect(ScreenPos, ImVec2(ScreenPos.x + Avail.x, ScreenPos.y + Avail.y), IM_COL32(80, 80, 80, 255));
    DrawList->AddLine(ImVec2(ScreenPos.x, ScreenPos.y + fRulerHeight), ImVec2(ScreenPos.x + Avail.x, ScreenPos.y + fRulerHeight), IM_COL32(80, 80, 80, 255));


    //눈금
    _float fTimeStep = ChooseTiemStepSecond(fTimePerPixel);

    _float fStartTimeSecond = floorf(m_ViewData.fPanTime / fTimeStep) * fTimeStep;
    _float fEndSecond = m_ViewData.fPanTime + fVisibleTime;

    for (_float fTime = fStartTimeSecond; fTime <= fEndSecond + 0.0001f; fTime += fTimeStep)
    {
        //시간을 화면 좌표로 변환
        _float fX = ScreenPos.x + (fTime - m_ViewData.fPanTime) / fTimePerPixel;
        if (fX < ScreenPos.x || fX > ScreenPos.x + Avail.x) continue;

        //큰 눈금과 숫자 그리기
        DrawList->AddLine(ImVec2(fX, ScreenPos.y), ImVec2(fX, ScreenPos.y + fRulerHeight), IM_COL32(120, 120, 120, 255));

        _char Text[32];
        snprintf(Text, 32, "%.2f", fTime);
        DrawList->AddText(ImVec2(fX + 2.f, ScreenPos.y + 2.f),
            IM_COL32(200, 200, 200, 255), Text);

        //작은 눈금 그리기
        _float fMinorStep = fTimeStep / 5.f;
        _float kMinLabelSpacingPx = 28.f; 
        _float lastMinorLabelX = -1e9f; //초기화용

        for (_int k = 1; k < 5; ++k)
        {

            _float fMinorTime = fTime + fMinorStep * k;
            if (fMinorTime > fEndSecond) break;

            _float fMinorX = ScreenPos.x + (fMinorTime - m_ViewData.fPanTime) / fTimePerPixel;
            if (fMinorX < ScreenPos.x || fMinorX > ScreenPos.x + Avail.x) continue;

            // 작은 눈금 선
            DrawList->AddLine(
                ImVec2(fMinorX, ScreenPos.y + fRulerHeight * 0.55f),
                ImVec2(fMinorX, ScreenPos.y + fRulerHeight),
                IM_COL32(90, 90, 90, 255)
            );

            // 너무 촘촘하면 스킵 (겹침 방지)
            if (fMinorX - lastMinorLabelX < kMinLabelSpacingPx)
                continue;

            // 소수점 2자리 표시
            char minorText[32];
            snprintf(minorText, 32, "%.2f", fMinorTime);

            // 텍스트 위치: 살짝 위로/오른쪽으로
            DrawList->AddText(
                ImVec2(fMinorX + 2.f, ScreenPos.y + 2.f),
                IM_COL32(160, 160, 160, 255),
                minorText
            );

            lastMinorLabelX = fMinorX;

        }

    }

    _float fTrackSpacing = 22.f;

    for (_int i = 0; i < (_int)Engine::ANIM_EVENT_TYPE::END; ++i)
    {
        _float fY = fTrackTop + (i * fTrackSpacing); // 30.f를 fTrackSpacing으로 변경

        // 트랙 구분 가로선 (높이를 간격에 맞춰 조절)
        DrawList->AddLine(
            ImVec2(ScreenPos.x, fY + (fTrackSpacing - 2.f)),
            ImVec2(ScreenPos.x + Avail.x, fY + (fTrackSpacing - 2.f)),
            IM_COL32(60, 60, 60, 255)
        );

        // 트랙 이름 배경 및 텍스트 (높이를 fTrackSpacing에 맞춤)
        DrawList->AddRectFilled(
            ImVec2(ScreenPos.x, fY),
            ImVec2(ScreenPos.x + 120.f, fY + (fTrackSpacing - 2.f)),
            IM_COL32(40, 40, 40, 180)
        );

        // 2. 트랙 이름 표시 (화면 왼쪽 고정)
        const _char* pTrackName = Get_NotifyEventName((Engine::ANIM_EVENT_TYPE)i);

        DrawList->AddText(
            ImVec2(ScreenPos.x + 5.f, fY + 1.f),
            IM_COL32(150, 150, 150, 255),
            pTrackName
        );
    }

    { //타임라인에서 현재 어디가 재생되고 있는지
        _float fX = ScreenPos.x + (fCurrentTime - m_ViewData.fPanTime) / fTimePerPixel;
        fX = clamp(fX, ScreenPos.x, ScreenPos.x + Avail.x);
        DrawList->AddLine(ImVec2(fX, ScreenPos.y), ImVec2(fX, ScreenPos.y + Avail.y), IM_COL32(255, 220, 120, 255));

        //위에 작은 표시
        DrawList->AddRectFilled(ImVec2(fX - 4.f, ScreenPos.y), ImVec2(fX + 4.f, ScreenPos.y + 6.f), IM_COL32(255, 220, 120, 255));
    }

    vector<ANIMNOTIFY_DESC> NotifyDescs = m_pAnimToolData->pSelectedAnimation->Get_Notifies();

    _float fTrackPadding = 8.f; //상하 여백
    _float fTrackYTop = fTrackTop + fTrackPadding; //트랙 실제 범위 위 / fTrackTop은 전체 Y길이
    _float fTrackYUnder = ScreenPos.y + Avail.y - fTrackPadding;//트랙 실제 범위 아래
    _float fTrackMid = (fTrackYTop + fTrackYUnder) * 0.5f; //트랙 중심점

    //DrawList->AddLine(ImVec2(ScreenPos.x, fTrackMid), ImVec2(ScreenPos.x + Avail.x, fTrackMid), IM_COL32(70, 70, 70, 255)); //트랙 중간 라인

    _float fViewStartTime = m_ViewData.fPanTime; //화면 왼쪽 끝에 보이는 시간
    _float fViewEndTime = m_ViewData.fPanTime + fVisibleTime; //화면 오른쪽 끝에 보이는 시간 , 줌값을 더해줌으로써 줌할때 보정

    
    auto IsMouseInRect = [](const ImVec2& a, const ImVec2& b) //마커/바를 마우스와 충돌했는지 체크
        {
            ImVec2 MPos = ImGui::GetIO().MousePos;
            return (MPos.x >= a.x && MPos.x <= b.x && MPos.y >= a.y && MPos.y <= b.y);
        };

    m_pAnimToolData->m_iHoveredNotifyId = g_INVALID; //호버는 매프레임 초기화 , 선택은 이전게 남아있어야하고

    for (auto& NotifyDesc : NotifyDescs)
    {
        _int iTrackIndex = (_int)NotifyDesc.eNotify_Event;
        _float fOffsetTrackY = fTrackTop + (iTrackIndex * fTrackSpacing); // 트랙당 20px 간격

        if (NotifyDesc.eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY) //단발
        {
            _float fFrame = (fTicksPerSecond > 0.f) ? (NotifyDesc.fFrame / fTicksPerSecond) : 0.f; //노티파이에 등록된 프레임의 실제 시간
            if (fFrame < fViewStartTime - 000.1f || fFrame > fViewEndTime + 0.001f) //줌을 하거나 함으로써 안보이는 시간대는 그리지 않도록 하기 위해서
                continue;

            _float fX = ScreenPos.x + (fFrame - m_ViewData.fPanTime) / fTimePerPixel; //현재 내 눈에 보이는 시작점으로부터 어디 위치에 그려야하는지
            fX = clamp(fX, ScreenPos.x, ScreenPos.x + Avail.x);

            //트랙을 세로로 꽉 채우는 바 형태로 마우스로 잡기 편해짐
            _float fHalfWidth = 2.f;
            ImVec2 LeftTop(fX - fHalfWidth, fOffsetTrackY + 1.f);
            ImVec2 RightBottom(fX + fHalfWidth, fOffsetTrackY+(fTrackSpacing-3.f));

            _bool bHovered = IsMouseInRect(LeftTop, RightBottom);
            if (bHovered) m_pAnimToolData->m_iHoveredNotifyId = NotifyDesc.iNotifyId;

            _bool bSelected = (m_pAnimToolData->m_iSelectedNotifyId == NotifyDesc.iNotifyId);

            ImU32 Color = bSelected ? IM_COL32(255, 220, 120, 255) : IM_COL32(140, 200, 255, 255); //선택(노란색),기본(하늘색)
            if (bHovered && !bSelected) Color = IM_COL32(180, 230, 255, 255);// 호버(밝은 하늘색)

            DrawList->AddRectFilled(LeftTop, RightBottom, Color, 2.f);

            const _char* pEventName = Get_NotifyEventName(NotifyDesc.eNotify_Event);
            DrawList->AddText(ImVec2(fX + 5.f, fOffsetTrackY + 1.f), IM_COL32(220, 220, 220, 255), pEventName);


        }
        else // Notify_State 지속
        {
            _float fStartFrame = (fTicksPerSecond > 0.f) ? (NotifyDesc.fStartFrame / fTicksPerSecond) : 0.f;
            _float fEndFrame = (fTicksPerSecond > 0.f) ? (NotifyDesc.fEndFrame / fTicksPerSecond) : 0.f;


            auto DrawRangeBar = [&](_float fStartFrame, _float fEndFrame)
            {
                    //노티파이 마지막 프레임이 화면 시작보다 전이거나 ,노티파이 시작 프레임이 화면 끝보다 뒤면 그리지 않도록 
                    if (fEndFrame < fViewStartTime || fStartFrame > fViewEndTime) return;

                    //화면 경계에 맞춰 시간 클램핑 (화면 밖으로 막대가 삐져나가지 않게 절단)
                    float fClampedStart = clamp(fStartFrame, fViewStartTime, fViewEndTime);
                    float fClampedEnd = clamp(fEndFrame, fViewStartTime, fViewEndTime);

                    //시간을 픽셀 좌표(X)로 변환
                    float fLeftX = ScreenPos.x + (fClampedStart - m_ViewData.fPanTime) / fTimePerPixel;
                    float fRightX = ScreenPos.x + (fClampedEnd - m_ViewData.fPanTime) / fTimePerPixel;

                    //혹시라도 시작/끝 시간이 뒤바뀌어 들어왔을 경우를 대비해 정렬
                    if (fRightX < fLeftX) std::swap(fLeftX, fRightX);

                    //최소 폭 보정: 너무 짧은 구간도 마우스로 클릭할 수 있게 최소 6픽셀 확보
                    if (fRightX - fLeftX < 6.f) fRightX = fLeftX + 6.f;

                    //최종 사각형 영역 설정 (위아래 10px씩 총 20px 높이)
                    ImVec2 RectMin(fLeftX, fOffsetTrackY+2.f);
                    ImVec2 RectMax(fRightX, fOffsetTrackY + (fTrackSpacing-2.f));

                    //상호작용 체크 (호버 및 선택)
                    _bool isHovered = IsMouseInRect(RectMin, RectMax);
                    if (isHovered) m_pAnimToolData->m_iHoveredNotifyId = NotifyDesc.iNotifyId;

                    _bool isSelected = (m_pAnimToolData->m_iSelectedNotifyId == NotifyDesc.iNotifyId);

                    //상태에 따른 색상 결정 선택일때는 주황색, 기본은 연녹색
                    ImU32 barColor = isSelected ? IM_COL32(255, 180, 120, 200) : IM_COL32(120, 255, 160, 160);
                    if (isHovered && !isSelected) barColor = IM_COL32(160, 255, 200, 190);

                    //실제 그리기 (채우기 + 테두리)
                    DrawList->AddRectFilled(RectMin, RectMax, barColor, 4.f); // 몸체
                    DrawList->AddRect(RectMin, RectMax, IM_COL32(30, 30, 30, 255), 4.f); // 외곽선

                    const _char* pEventName = Get_NotifyEventName(NotifyDesc.eNotify_Event);
                    ImVec2 TextPos(fLeftX + 4.f, fOffsetTrackY + 1.f);
                    DrawList->AddText(TextPos, IM_COL32(255, 255, 255, 255), pEventName);
            };

            if (fStartFrame <= fEndFrame) //기존 처음부터 끝까지 재생 
                DrawRangeBar(fStartFrame, fEndFrame);
            else //Start 프레임이 End 이후에 설정한 값일때
            {
                DrawRangeBar(fStartFrame, fTotalTime);
                DrawRangeBar(0.f, fEndFrame);
            }

        }

    }

    if (bHoverd)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (m_pAnimToolData->m_iHoveredNotifyId != g_INVALID)
                m_pAnimToolData->m_iSelectedNotifyId = m_pAnimToolData->m_iHoveredNotifyId;
            else
                m_pAnimToolData->m_iSelectedNotifyId = g_INVALID;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_pAnimToolData->m_iHoveredNotifyId != g_INVALID) //우클릭 했고 호버된게 유효한 아이디일때
        {
            m_pAnimToolData->m_iSelectedNotifyId = m_pAnimToolData->m_iHoveredNotifyId; //우클릭했을때도 선택된Id로 등록해주고
            ImGui::OpenPopup("##NotifyContext"); //노티파이 메뉴창을 열라는 신호넣어주고
        }

    }

    if (ImGui::BeginPopup("##NotifyContext")) //신호가 왔을때 해당 내용들을 렌더시작
    {
        ImGui::Text("NotifyId: %u", m_pAnimToolData->m_iSelectedNotifyId);
        ImGui::Separator();

        if (ImGui::MenuItem("Delete"))
        {
            //현재 선택된 Id의 Notify를 지우기
            m_pAnimToolData->pSelectedAnimation->Remove_Notify(m_pAnimToolData->m_iSelectedNotifyId);
            m_pAnimToolData->m_iSelectedNotifyId = g_INVALID;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::EndChild();

    if (!m_pAnimToolData->bScrubbing)
    {
        if (m_pAnimToolData->bIsAnimPlaying)
            m_pAnimToolData->pModelCom->Play_Animation_CS(fTimeDelta);
    }

    if (m_pAnimToolData->pModelCom->Is_AnimFinished() == true && m_pAnimToolData->bIsLoop == false) //루프 애니메이션 아니고 , 현재 재생중인 애니메이션이 끝났으면
    {
        if (fCurrentFrame >= fDurtaion) //루프 아닌애들도 현재 재생위치를 최대 시간으로 되돌리게 , 툴에서만 사용할예정 / 안하면 계속 CurrentFrame이 증가해서 보기 불편
            m_pAnimToolData->pSelectedAnimation->Set_CurrentFrame(fDurtaion);
    }

}

_float Window_AnimTimeline::ChooseTiemStepSecond(_float fTimePerPixel)
{
    _float fTarget = fTimePerPixel * 80.0f; //눈금 간격 최소 60px 정도 되게 

    //화면을 아무리 확대/축소해도 눈금이 겹치지 않고 보기 편한 단위(1, 2, 5 계열)로 자동 조절해주는

    _float fBase = 1.0f;
    while (fBase * 10.0f < fTarget) fBase *= 10.0f;

    _float fCandidates[6] = { fBase , fBase * 2.f ,fBase * 5.f, fBase * 10.f, fBase * 20.f, fBase * 50.f };
    _float fBest = fCandidates[0];

    //Candidate 는 후보자 정하기
    for (_float fCandidate : fCandidates)
    {
        if (fCandidate >= fTarget) { fBest = fCandidate; break; }
        fBest = fCandidate;
    }

    return fBest;
}

const _char* Window_AnimTimeline::Get_NotifyEventName(ANIM_EVENT_TYPE eEventType)
{
    switch (eEventType)
    {
    case Engine::ANIM_EVENT_TYPE::PLAY_SOUND:
    {
        return "PLAY_SOUND";
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_PARTICLE:
    {
        return "SPAWN_PARTICLE";
    }
    case Engine::ANIM_EVENT_TYPE::ACTIVE_COLLIDER:
    {
        return "ACTIVE_COLLIDER";
    }
    case Engine::ANIM_EVENT_TYPE::CAMERA:
    {
        return "CAMERA";
    }
    case Engine::ANIM_EVENT_TYPE::SPAWN_TRAIL:
    {
        return "SPAWN_TRAIL";
    }
    case Engine::ANIM_EVENT_TYPE::PLAYER_ANIM:
    {
        return "PLAYER_ANIM";
    }
    case Engine::ANIM_EVENT_TYPE::MONSTER_ANIM:
    {
        return "MONSTER_ANIM";
    }
    case Engine::ANIM_EVENT_TYPE::DISSOLVE_FLAG:
    {
        return "DISSOLVE_FLAG";
    }
    default:
    {
        return "등록된 이벤트 타입 이름이 없습니다";
    }
    };
}


Window_AnimTimeline* Window_AnimTimeline::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    Window_AnimTimeline* pInstance = new Window_AnimTimeline(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : Window_AnimTimeline");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Window_AnimTimeline::Free()
{
    __super::Free();

}
