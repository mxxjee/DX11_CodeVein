#include "Engine_Define.h"
#include "SoundManager.h"
#include "GameObject.h"

#include <fstream>
#include <algorithm> // std::transform

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "stb_vorbis.c"
#include "GameInstance.h"


#include <filesystem>
namespace fs = std::filesystem;


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT SoundManager::Initialize()
{
    if (FAILED(XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
        return E_FAIL;

    if (FAILED(m_xAudio2->CreateMasteringVoice(&m_masterVoice)))
    {
        m_bHasSoundDevice = false;
        COUT("사운드 장치가 없습니다.");
    }

    GameInstance* pGameInstance = GameInstance::GetInstance();
    if (pGameInstance)
    {
        m_iSoundEventHandle = pGameInstance->Subscribe<SOUND_EVENT>(
            [this](const SOUND_EVENT& _event)
            {
                SoundEvent_Function(_event);
            });
    }

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 (완료된 Voice 정리) ////////////////////////////////////////////////////////
void SoundManager::Update(_float fTimeDelta)
{
    if (!m_bHasSoundDevice)
        return;

    for (_int i = CAST(_int)(m_vecFadeTasks.size()) - 1; i >= 0; --i)
    {
        m_vecFadeTasks[i].fCurrentVolume -= m_vecFadeTasks[i].fFadeSpeed * fTimeDelta;

        if (m_vecFadeTasks[i].fCurrentVolume <= 0.f)
        {
            SoundID targetID = m_vecFadeTasks[i].soundID;

            m_vecFadeTasks[i] = m_vecFadeTasks.back();
            m_vecFadeTasks.pop_back();

            StopSound(targetID);
        }
        else
        {
            m_vecFadeTasks[i].pVoice->SetVolume(m_vecFadeTasks[i].fCurrentVolume);
        }
    }
    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        // 콜백 플래그 확인
        if (m_vecActiveVoices[i].callback->bFinished)
        {
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback; // 콜백 해제

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }
}
/******************************************************* 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너에 사운드 저장 ////////////////////////////////////////////////////////
HRESULT SoundManager::LoadSound(const string& _name, const std::wstring& _filePath, SoundID* _outID)
{
    if (!m_bHasSoundDevice)
        return S_OK;

    SoundID nameID = HashSoundName(_name); // 진입 시 해싱

    if (m_soundMap.find(nameID) != m_soundMap.end()) // SoundID로 검색
    {
        MessageBox(nullptr, (L"추가하려는 사운드 \n[" + stringToWstring(_name) + L"]\n이 이미 존재합니다").c_str(), L"사운드 추가 실패", MB_OK);
        BREAK;
        return S_OK;
    }

    SoundData sound;

    _wstring extension = _filePath.substr(_filePath.find_last_of(L".") + 1);

    std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

    HRESULT loadSuccess = E_FAIL;

    if (extension == L"wav")
        loadSuccess = LoadWAVFile(_filePath, sound);
    else if (extension == L"mp3")
        loadSuccess = LoadMP3File(_filePath, sound);
    else if (extension == L"ogg")
        loadSuccess = LoadOGGFile(_filePath, sound);

    if (FAILED(loadSuccess))
    {
        MessageBox(NULL, _filePath.c_str(), L"없음", 0);
        MessageBox(NULL, L"해당 경로의 사운드 파일이 없거나 지원하지 않는 형식입니다", L"없음", 0);
        return E_FAIL;
    }

    if (_outID)
        *_outID = nameID;

    m_soundMap[nameID] = sound; // SoundID로 저장
    return S_OK;
}

HRESULT SoundManager::LoadSound_AddGroup(const string& _name, const string& _groupName, const std::wstring& _filePath, SoundID* _outID)
{
    SoundID nameID = {};
    CHECK_FAILED(LoadSound(_name, _filePath, &nameID), E_FAIL);

    // 로드 성공하면 그룹에 등록
    SoundID groupNameID = HashSoundName(_groupName);

    auto& group = m_soundGroups[groupNameID];
    for (const auto& id : group)
    {
        if (id == nameID)
            goto SKIP_ADD; // 이미 있으면 추가 안 함
    }
    group.push_back(nameID);

SKIP_ADD:
    if (_outID)
        *_outID = nameID;

    return S_OK;
}

HRESULT Engine::SoundManager::LoadSoundFolder(const _wstring& _folderPath)
{
    if (!m_bHasSoundDevice)
        return S_OK;

    for(const auto& entry : fs::directory_iterator(_folderPath))
    {
        if (!entry.is_regular_file())
            continue;

        // 확장자 확인
        _wstring extension = entry.path().extension().wstring();
        // 대문자를 소문자화
        std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

        if (extension != L".wav" && extension != L".ogg" && extension != L".mp3")
            continue;

        // 그룹명 = 폴더 이름
        _wstring folderNameW = entry.path().parent_path().filename().wstring();
        _wstring fileNameW = entry.path().stem().wstring();

        // 이름 뽑아서 string으로 바꿔주기
        _string groupName = wstringToString(folderNameW);
        _string fileName = wstringToString(fileNameW);

        // 그냥 _folderPath넘기면 폴더명만 있게 되니까 지금 읽은 파일을 제대로 넘겨주기
        _wstring filePath = entry.path().wstring();

        // 사운드파일 등록
        if (FAILED(LoadSound_AddGroup(fileName, groupName, filePath)))
        {
            _wstring msg = L"사운드 로드 실패: " + filePath;
            WCOUT(msg.c_str());
            continue; // 하나 실패해도 나머지 계속 로드
        }
    }

    return S_OK;
}

HRESULT Engine::SoundManager::LoadSoundFolder_Recursive(const _wstring& _folderPath)
{
    if (!m_bHasSoundDevice)
        return S_OK;

    for (const auto& entry : fs::recursive_directory_iterator(_folderPath))
    {
        if (!entry.is_regular_file())
            continue;

        // 확장자 확인
        _wstring extension = entry.path().extension().wstring();
        // 대문자를 소문자화
        std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

        if (extension != L".wav" && extension != L".mp3" && extension != L".ogg")
            continue;

        // 그룹명 = 파일이 속한 폴더 이름
        _wstring groupNameW = entry.path().parent_path().filename().wstring();

        // 사운드명 = 그룹명_파일명 (확장자 제외)
        _wstring fileNameW = entry.path().stem().wstring();
        _wstring soundNameW = groupNameW + L"_" + fileNameW;

        // wstring -> string 변환
        string soundName = wstringToString(soundNameW);
        string groupName = wstringToString(groupNameW);

        // 전체 파일 경로
        _wstring filePath = entry.path().wstring();

        // 로드 + 그룹 등록
        if (FAILED(LoadSound_AddGroup(soundName, groupName, filePath)))
        {
            _wstring msg = L"사운드 로드 실패: " + filePath;
            WCOUT(msg.c_str());
            continue; // 하나 실패해도 나머지 계속 로드
        }
    }

    return S_OK;
}
/******************************************************* 컨테이너에 사운드 저장 *******************************************************/



//////////////////////////////////////////////////////// 사운드 재생 공통 ////////////////////////////////////////////////////////
void Engine::SoundManager::CreateAndPlayVoice(SoundID _soundID, SoundID _groupID, const SoundData& _sound, _float _volume, _bool _loop)
{
    VoiceCallback* callback = new VoiceCallback();

    IXAudio2SourceVoice* sourceVoice = nullptr;
    if (FAILED(m_xAudio2->CreateSourceVoice(&sourceVoice, &_sound.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback)))
    {
        delete callback;
        return;
    }

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = _sound.dataSize;
    buffer.pAudioData = _sound.audioData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    if (_loop)
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    if (FAILED(sourceVoice->SubmitSourceBuffer(&buffer)))
    {
        sourceVoice->DestroyVoice();
        delete callback;
        return;
    }

    sourceVoice->SetVolume(_volume);

    if (FAILED(sourceVoice->Start()))
    {
        sourceVoice->DestroyVoice();
        delete callback;
        return;
    }

    VoiceInstance instance;
    instance.sourceVoice = sourceVoice;
    instance.sound = &_sound;
    instance.callback = callback;
    instance.soundID = _soundID;
    instance.groupID = _groupID;

    m_vecActiveVoices.push_back(instance);
}
/******************************************************* 사운드 재생 공통 *******************************************************/



//////////////////////////////////////////////////////// 사운드 재생 ////////////////////////////////////////////////////////
void SoundManager::Play_Sound(const string& _name, _float _volume, _bool _loop)
{
    static const string emptyGroup = "";
    Play_Sound_InGroup(_name, emptyGroup, _volume, _loop);
}

void SoundManager::Play_Sound(SoundID _id, _float _volume, _bool _loop)
{
    Play_Sound_InGroup(_id, SoundID{}, _volume, _loop);
}
/******************************************************* 사운드 재생 *******************************************************/



//////////////////////////////////////////////////////// 그룹 지정 재생 ////////////////////////////////////////////////////////
void SoundManager::Play_Sound_InGroup(const string& _name, const string& _groupName, _float _volume, _bool _loop)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID nameID = HashSoundName(_name);              // 해싱
    SoundID groupNameID = HashSoundName(_groupName);    // 해싱

    auto it = m_soundMap.find(nameID);                  // SoundID로 검색
    if (it == m_soundMap.end())
        return;

    CreateAndPlayVoice(nameID, groupNameID, it->second, _volume, _loop);
}

void Engine::SoundManager::Play_Sound_InGroup(SoundID _id, SoundID _groupID, _float _volume, _bool _loop)
{
    if (!m_bHasSoundDevice)
        return;

    auto it = m_soundMap.find(_id);
    if (it == m_soundMap.end())
        return;

    CreateAndPlayVoice(_id, _groupID, it->second, _volume, _loop);
}
/******************************************************* 그룹 지정 재생 *******************************************************/



//////////////////////////////////////////////////////// 같은 사운드 하나만 재생 ////////////////////////////////////////////////////////
void SoundManager::Play_Sound_Unique(const string& _name, _float _volume, _bool _loop)
{
    if (!m_bHasSoundDevice)
        return;

    StopSound(_name);
    Play_Sound(_name, _volume, _loop);
}
/******************************************************* 같은 사운드 하나만 재생 *******************************************************/



//////////////////////////////////////////////////////// 해당 사운드 OFF ////////////////////////////////////////////////////////
void SoundManager::StopSound(const string& _name)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID targetID = HashSoundName(_name);

    // 뒤에서부터 순회하면서 swap + pop_back
    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        if (m_vecActiveVoices[i].soundID == targetID)
        {
            m_vecActiveVoices[i].sourceVoice->Stop();
            m_vecActiveVoices[i].sourceVoice->FlushSourceBuffers();
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback;

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }
}

void SoundManager::StopSound(SoundID _id)
{
    if (!m_bHasSoundDevice)
        return;

    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        if (m_vecActiveVoices[i].soundID == _id)
        {
            m_vecActiveVoices[i].sourceVoice->Stop();
            m_vecActiveVoices[i].sourceVoice->FlushSourceBuffers();
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback;

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }
}

void SoundManager::FadeOutSound(const string& _name, _float _fadeDuration)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID targetID = HashSoundName(_name);

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.soundID == targetID)
        {
            FadeTask task;
            task.pVoice = voice.sourceVoice;
            task.soundID = voice.soundID;

            task.pVoice->GetVolume(&task.fCurrentVolume);

            if (_fadeDuration > 0.f)
                task.fFadeSpeed = task.fCurrentVolume / _fadeDuration;
            else
                task.fFadeSpeed = task.fCurrentVolume;

            m_vecFadeTasks.push_back(task);
        }
    }
}
/******************************************************* 해당 사운드 OFF *******************************************************/



//////////////////////////////////////////////////////// 그룹 정지 ////////////////////////////////////////////////////////
void SoundManager::StopGroup(const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID targetGroupID = HashSoundName(_groupName);

    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        if (m_vecActiveVoices[i].groupID == targetGroupID)
        {
            m_vecActiveVoices[i].sourceVoice->Stop();
            m_vecActiveVoices[i].sourceVoice->FlushSourceBuffers();
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback;

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }
}

void SoundManager::StopGroup(SoundID _groupID)
{
    if (!m_bHasSoundDevice)
        return;

    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        if (m_vecActiveVoices[i].groupID == _groupID)
        {
            m_vecActiveVoices[i].sourceVoice->Stop();
            m_vecActiveVoices[i].sourceVoice->FlushSourceBuffers();
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback;

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }
}
/******************************************************* 그룹 정지 *******************************************************/



//////////////////////////////////////////////////////// 사운드 전부 끄기 ////////////////////////////////////////////////////////
void SoundManager::StopAll()
{
    if (!m_bHasSoundDevice)
        return;

    for (auto& voice : m_vecActiveVoices)
    {
        voice.sourceVoice->Stop();
        voice.sourceVoice->FlushSourceBuffers();
        voice.sourceVoice->DestroyVoice();
        delete voice.callback;
    }
    m_vecActiveVoices.clear();
}
/******************************************************* 사운드 전부 끄기 *******************************************************/



//////////////////////////////////////////////////////// 재생 상태 확인 ////////////////////////////////////////////////////////
_bool SoundManager::IsPlaying(const string& _name)
{
    if (!m_bHasSoundDevice)
        return false;

    SoundID targetID = HashSoundName(_name); // 해싱

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.soundID == targetID) // 정수 비교
        {
            XAUDIO2_VOICE_STATE state;
            voice.sourceVoice->GetState(&state);

            if (state.BuffersQueued > 0)
                return true;
        }
    }

    return false;
}

_bool SoundManager::IsPlaying(SoundID _id)
{
    if (!m_bHasSoundDevice)
        return false;

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.soundID == _id && !voice.callback->bFinished)
            return true;
    }

    return false;
}

_bool SoundManager::IsGroupPlaying(const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return false;

    SoundID targetGroupID = HashSoundName(_groupName); // 해싱

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == targetGroupID) // 정수 비교
        {
            XAUDIO2_VOICE_STATE state;
            voice.sourceVoice->GetState(&state);

            if (state.BuffersQueued > 0)
                return true;
        }
    }

    return false;
}

_bool SoundManager::IsGroupPlaying(SoundID _groupID)
{
    if (!m_bHasSoundDevice)
        return false;

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == _groupID && !voice.callback->bFinished)
            return true;
    }

    return false;
}

_bool SoundManager::IsGroupFinished(const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return false;

    SoundID targetGroupID = HashSoundName(_groupName); // 해싱

    // 그룹에 속한 사운드가 하나도 재생 중이 아니면 완료
    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == targetGroupID) // 정수 비교
        {
            XAUDIO2_VOICE_STATE state;
            voice.sourceVoice->GetState(&state);

            // 아직 재생 중인 게 있으면 false
            if (state.BuffersQueued > 0)
                return false;
        }
    }

    return true;
}

_bool SoundManager::IsGroupFinished(SoundID _groupID)
{
    if (!m_bHasSoundDevice)
        return false;

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == _groupID && !voice.callback->bFinished)
            return false;
    }

    return true;
}

_uint SoundManager::GetGroupPlayingCount(const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return 0;

    SoundID targetGroupID = HashSoundName(_groupName); // 해싱

    _uint count = 0; // iCount -> count

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == targetGroupID) // 정수 비교
        {
            XAUDIO2_VOICE_STATE state;
            voice.sourceVoice->GetState(&state);

            if (state.BuffersQueued > 0)
                ++count;
        }
    }

    return count;
}
/******************************************************* 재생 상태 확인 *******************************************************/



//////////////////////////////////////////////////////// 특정 사운드의 소리 크기 조절 ////////////////////////////////////////////////////////
void SoundManager::SetVolume(const string& _name, _float _volume)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID targetID = HashSoundName(_name); // 해싱

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.soundID == targetID) // 정수 비교
        {
            voice.sourceVoice->SetVolume(_volume);
        }
    }
}

void SoundManager::SetVolume(SoundID _id, _float _volume)
{
    if (!m_bHasSoundDevice)
        return;

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.soundID == _id)
            voice.sourceVoice->SetVolume(_volume);
    }
}
/******************************************************* 특정 사운드의 소리 크기 조절 *******************************************************/



//////////////////////////////////////////////////////// 그룹 볼륨 ////////////////////////////////////////////////////////
void SoundManager::SetGroupVolume(const string& _groupName, _float _volume)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID targetGroupID = HashSoundName(_groupName); // 해싱

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == targetGroupID) // 정수 비교
        {
            voice.sourceVoice->SetVolume(_volume);
        }
    }
}

void SoundManager::SetGroupVolume(SoundID _groupID, _float _volume)
{
    if (!m_bHasSoundDevice)
        return;

    for (auto& voice : m_vecActiveVoices)
    {
        if (voice.groupID == _groupID)
            voice.sourceVoice->SetVolume(_volume);
    }
}
/******************************************************* 그룹 볼륨 *******************************************************/



//////////////////////////////////////////////////////// 전체 소리 크기 조절 ////////////////////////////////////////////////////////
void SoundManager::SetMasterVolume(_float _volume)
{
    if (!m_bHasSoundDevice)
        return;

    if (m_masterVoice)
    {
        m_masterVoice->SetVolume(_volume);
    }
}
/******************************************************* 전체 소리 크기 조절 *******************************************************/



//////////////////////////////////////////////////////// 마스터 볼륨 획득 ////////////////////////////////////////////////////////
_float SoundManager::GetMasterVolume() const
{
    if (!m_bHasSoundDevice)
        return 0.f;

    _float volume = 0.f;
    m_masterVoice->GetVolume(&volume);
    return volume;
}
/******************************************************* 마스터 볼륨 획득 *******************************************************/



//////////////////////////////////////////////////////// 그룹에 사운드 등록 ////////////////////////////////////////////////////////
void SoundManager::AddSoundToGroup(const string& _soundName, const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID soundNameID = HashSoundName(_soundName);  // 해싱
    SoundID groupNameID = HashSoundName(_groupName);  // 해싱

    // 사운드가 존재하는지 확인
    if (m_soundMap.find(soundNameID) == m_soundMap.end()) // SoundID로 검색
        return;

    // 이미 등록되어 있는지 확인
    auto& group = m_soundGroups[groupNameID]; // SoundID로 접근
    for (const auto& id : group)
    {
        if (id == soundNameID) // SoundID 비교
            return;  // 이미 있음
    }

    group.push_back(soundNameID); // SoundID로 저장
}
/******************************************************* 그룹에 사운드 등록 *******************************************************/



//////////////////////////////////////////////////////// 그룹에서 사운드 제거 ////////////////////////////////////////////////////////
void SoundManager::RemoveSoundFromGroup(const string& _soundName, const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID soundNameID = HashSoundName(_soundName);  // 해싱
    SoundID groupNameID = HashSoundName(_groupName);  // 해싱

    auto iter = m_soundGroups.find(groupNameID); // SoundID로 검색
    if (iter == m_soundGroups.end())
        return;

    auto& group = iter->second;
    for (auto it = group.begin(); it != group.end(); ++it)
    {
        if (*it == soundNameID) // SoundID 비교
        {
            group.erase(it);
            return;
        }
    }
}
/******************************************************* 그룹에서 사운드 제거 *******************************************************/



//////////////////////////////////////////////////////// 그룹 전체 재생 ////////////////////////////////////////////////////////
void SoundManager::PlayGroup(const string& _groupName, _float _volume, _bool _loop)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID groupNameID = HashSoundName(_groupName); // 해싱

    auto iter = m_soundGroups.find(groupNameID); // SoundID로 검색
    if (iter == m_soundGroups.end())
        return;

    // SoundID를 순회하므로 Play_Sound_InGroup에 string을 넘길 수 없음
    //          내부용 오버로드를 쓰거나 직접 생성
    for (const auto& soundNameID : iter->second)
    {
        // m_soundMap에서 직접 찾아서 재생
        auto soundIter = m_soundMap.find(soundNameID);
        if (soundIter == m_soundMap.end())
            continue;

        CreateAndPlayVoice(soundNameID, groupNameID, soundIter->second, _volume, _loop);
    }
}
/******************************************************* 그룹 전체 재생 *******************************************************/



//////////////////////////////////////////////////////// 그룹 내 랜덤 1개 재생 ////////////////////////////////////////////////////////
void SoundManager::PlayRandomInGroup(const string& _groupName, _float _volume, _bool _loop)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID groupNameID = HashSoundName(_groupName);

    auto iter = m_soundGroups.find(groupNameID);
    if (iter == m_soundGroups.end())
        return;

    const auto& group = iter->second;
    if (group.empty())
        return;

    // 랜덤 인덱스 선택
    std::uniform_int_distribution<size_t> dist(0, group.size() - 1);
    SoundID pickedID = group[dist(m_randomEngine)];

    auto soundIter = m_soundMap.find(pickedID);
    if (soundIter == m_soundMap.end())
        return;

    CreateAndPlayVoice(pickedID, groupNameID, soundIter->second, _volume, _loop);
}
/******************************************************* 그룹 내 랜덤 1개 재생 *******************************************************/



//////////////////////////////////////////////////////// 그룹 클리어 ////////////////////////////////////////////////////////
void SoundManager::ClearGroup(const string& _groupName)
{
    if (!m_bHasSoundDevice)
        return;

    SoundID groupNameID = HashSoundName(_groupName); // 해싱

    auto iter = m_soundGroups.find(groupNameID); // SoundID로 검색
    if (iter != m_soundGroups.end())
    {
        iter->second.clear();
        m_soundGroups.erase(iter);
    }
}
/******************************************************* 그룹 클리어 *******************************************************/



//////////////////////////////////////////////////////// 사운드파일 불러오기 ////////////////////////////////////////////////////////
HRESULT SoundManager::LoadWAVFile(const wstring& _filePath, SoundData& _outSound)
{
    std::ifstream file(_filePath, std::ios::binary);
    if (!file)
        return E_FAIL;

    DWORD chunkType = 0, chunkSize = 0;
    DWORD format = 0;

    file.read(reinterpret_cast<char*>(&chunkType), 4); // "RIFF"
    file.seekg(4, std::ios::cur);                     // Chunk size
    file.read(reinterpret_cast<char*>(&format), 4);   // "WAVE"

    if (chunkType != 'FFIR' || format != 'EVAW')
        return E_FAIL;

    while (file.read(reinterpret_cast<char*>(&chunkType), 4))
    {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (chunkType == ' tmf') // "fmt "
        {
            file.read(reinterpret_cast<char*>(&_outSound.waveFormat), sizeof(WAVEFORMATEX));
            file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
        }
        else if (chunkType == 'atad') // "data"
        {
            _outSound.audioData = new BYTE[chunkSize];
            _outSound.dataSize = chunkSize;
            file.read(reinterpret_cast<char*>(_outSound.audioData), chunkSize);
            break;
        }
        else
        {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    return (_outSound.audioData != nullptr) ? S_OK : E_FAIL;
}

HRESULT SoundManager::LoadMP3File(const wstring& _filePath, SoundData& _outSound)
{
    drmp3 mp3;
    if (!drmp3_init_file_w(&mp3, _filePath.c_str(), nullptr))
        return E_FAIL;

    drmp3_uint64 frameCount = drmp3_get_pcm_frame_count(&mp3);
    size_t dataSize = (size_t)(frameCount * mp3.channels * sizeof(drmp3_int16));

    _outSound.audioData = new BYTE[dataSize];
    _outSound.dataSize = (DWORD)dataSize;

    drmp3_read_pcm_frames_s16(&mp3, frameCount, (drmp3_int16*)_outSound.audioData);

    _outSound.waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    _outSound.waveFormat.nChannels = (WORD)mp3.channels;
    _outSound.waveFormat.nSamplesPerSec = mp3.sampleRate;
    _outSound.waveFormat.wBitsPerSample = 16;
    _outSound.waveFormat.nBlockAlign = (WORD)(mp3.channels * 2);
    _outSound.waveFormat.nAvgBytesPerSec = mp3.sampleRate * _outSound.waveFormat.nBlockAlign;
    _outSound.waveFormat.cbSize = 0;

    drmp3_uninit(&mp3);
    return S_OK;
}

HRESULT SoundManager::LoadOGGFile(const std::wstring& _filePath, SoundData& _outSound)
{
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, _filePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string narrowPath(sizeNeeded - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, _filePath.c_str(), -1, &narrowPath[0], sizeNeeded, nullptr, nullptr);

    int channels = 0, sampleRate = 0;
    short* decoded = nullptr;

    int sampleCount = stb_vorbis_decode_filename(narrowPath.c_str(), &channels, &sampleRate, &decoded);
    if (sampleCount <= 0)
        return E_FAIL;

    size_t dataSize = (size_t)(sampleCount * channels * sizeof(short));

    _outSound.audioData = new BYTE[dataSize];
    _outSound.dataSize = (DWORD)dataSize;
    memcpy(_outSound.audioData, decoded, dataSize);

    _outSound.waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    _outSound.waveFormat.nChannels = (WORD)channels;
    _outSound.waveFormat.nSamplesPerSec = sampleRate;
    _outSound.waveFormat.wBitsPerSample = 16;
    _outSound.waveFormat.nBlockAlign = (WORD)(channels * 2);
    _outSound.waveFormat.nAvgBytesPerSec = sampleRate * _outSound.waveFormat.nBlockAlign;
    _outSound.waveFormat.cbSize = 0;

    free(decoded);
    return S_OK;
}
/******************************************************* 사운드파일 불러오기 *******************************************************/



//////////////////////////////////////////////////////// 사운드 이벤트 ////////////////////////////////////////////////////////
void Engine::SoundManager::SoundEvent_Function(const SoundEvent& _event)
{
    _float fFinalVolume = _event.fVolume;
    if (m_fnVolumeModifier != nullptr)
        fFinalVolume = m_fnVolumeModifier(_event.iOwnerId, _event.fVolume);

    switch (_event.ePhase)
    {
    case ANIM_FRAMEPHASE::START:
        if (_event.bRandomPlay == true)
        {
            PlayRandomInGroup(_event.strGroupName, fFinalVolume, _event.bLoop);
        }
        else if (_event.bInGroup == true)
        {
            Play_Sound_InGroup(_event.SoundName, _event.strGroupName, fFinalVolume, _event.bLoop);
        }
        else if (_event.bGroupPlay == true)
        {
            PlayGroup(_event.strGroupName, fFinalVolume, _event.bLoop);
        }
        else
        {
            Play_Sound(_event.SoundName, fFinalVolume, _event.bLoop);
        }
        
        break;


    case ANIM_FRAMEPHASE::UPDATE:
        break;


    case ANIM_FRAMEPHASE::END:
        if (_event.bLoop)
        {
            StopSound(_event.SoundName);
        }
        
        break;
    }
}
/******************************************************* 사운드 이벤트 *******************************************************/



//////////////////////////////////////////////////////// 사운드맵 클리어 ////////////////////////////////////////////////////////
void Engine::SoundManager::Clear_SoundManager()
{
    for (auto& pair : m_soundMap)
    {
        delete[] pair.second.audioData;
    }
    m_soundMap.clear();
}

void Engine::SoundManager::Delete_Sound(const _string& _soundName)
{
    SoundID hashID = HashSoundName(_soundName);

    Delete_Sound(hashID);
}

void Engine::SoundManager::Delete_Sound(SoundID _ID)
{
    auto iter = m_soundMap.find(_ID);
    if (iter == m_soundMap.end())
        return;

    // 재생 중인 voice 먼저 정리
    for (_int i = CAST(_int)(m_vecActiveVoices.size()) - 1; i >= 0; --i)
    {
        if (m_vecActiveVoices[i].soundID == _ID)
        {
            m_vecActiveVoices[i].sourceVoice->Stop();
            m_vecActiveVoices[i].sourceVoice->FlushSourceBuffers();
            m_vecActiveVoices[i].sourceVoice->DestroyVoice();
            delete m_vecActiveVoices[i].callback;

            m_vecActiveVoices[i] = m_vecActiveVoices.back();
            m_vecActiveVoices.pop_back();
        }
    }

    // 모든 그룹에서 해당 SoundID 제거
    for (auto& pair : m_soundGroups)
    {
        auto& group = pair.second;
        for (auto it = group.begin(); it != group.end(); )
        {
            if (*it == _ID)
                it = group.erase(it);
            else
                ++it;
        }
    }

    // 오디오 데이터 메모리 해제
    delete[] iter->second.audioData;

    m_soundMap.erase(iter);
}
/******************************************************* 사운드맵 클리어 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
SoundManager* Engine::SoundManager::Create()
{
    SoundManager* pInstance = new SoundManager();

    if (FAILED(pInstance->Initialize()))
    {
        MessageBox(nullptr, L"SoundManager 생성 실패", L"Caution!!!", MB_OK);
        BREAK;
        return nullptr;
    }

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void SoundManager::Free()
{
    GameInstance* pGameInstance = GameInstance::GetInstance();
    if (pGameInstance && m_iSoundEventHandle != 0)
    {
        pGameInstance->UnsubScribe(m_iSoundEventHandle);
    }

    StopAll();
    Clear_SoundManager();

    if (m_masterVoice)
        m_masterVoice->DestroyVoice();
    if (m_xAudio2)
        m_xAudio2->Release();

    m_vecActiveVoices.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/