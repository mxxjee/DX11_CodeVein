// SoundManager.h
#pragma once
#include <xaudio2.h>
#include <mutex>
#include "Base.h"


NS_BEGIN(Engine)

// 사운드 이름을 정수로 변환하는 함수
inline SoundID HashSoundName(const string& _name)
{
    // std::hash가 string을 받아서 size_t 정수를 리턴해줌
    // 같은 문자열은 항상 같은 숫자가 나옴
    static std::hash<string> hasher;
    return hasher(_name);
}

// 재생 완료 콜백
// XAudio2는 별도 오디오 스레드에서 콜백을 호출함
// 콜백 안에서 직접 DestroyVoice를 하면 안 되고,
// "끝났다"는 표시만 남기고 메인 스레드의 Update에서 정리함
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    // 이 voice가 끝났는지 표시하는 플래그
    _bool bFinished = false;

    // 스트림 끝에 도달했을 때 XAudio2가 호출
    void STDMETHODCALLTYPE OnStreamEnd() override { bFinished = true; }

    // 나머지는 사용하지 않지만 순수가상함수라 구현 필수
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
    void STDMETHODCALLTYPE OnBufferStart(void*) override {}
    void STDMETHODCALLTYPE OnBufferEnd(void*) override {}
    void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
    void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
};

class SoundManager : public Base
{
private:
    explicit SoundManager();
    virtual ~SoundManager();

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);
    void Clear_SoundManager();

    // 사운드 파일 로드. name은 식별자.
    HRESULT LoadSound(const string& _name, const std::wstring& _filePath, SoundID* _outID = nullptr);
    HRESULT LoadSound_AddGroup(const string& _name, const string& _groupName, const std::wstring& _filePath, SoundID* _outID = nullptr);
    // 폴더 하나 전부 등록(폴더명 = 그룹명)
    HRESULT LoadSoundFolder(const wstring& _folderPath);
    // 하위 폴더까지 전부 등록(최하위폴더명 = 그룹명)
    HRESULT LoadSoundFolder_Recursive(const wstring& _folderPath);

    // 사운드 재생. loop가 true면 무한 반복
    void Play_Sound(const string& _name, _float _volume = 1.f, _bool _loop = false);

    // 특정 그룹 재생
    void Play_Sound_InGroup(const string& _name, const string& _groupName, _float _volume = 1.f, _bool _loop = false);

    // 같은 이름의 사운드 하나만 재생
    void Play_Sound_Unique(const string& _name, _float _volume = 1.f, _bool _loop = false);

    // 특정 이름의 모든 재생 중인 사운드 중지
    void StopSound(const string& _name);

    // 특정 그룹 전부 정지
    void StopGroup(const string& _groupName);

    // 전체 사운드 정지
    void StopAll();

    // 사운드 재생중인지
    _bool IsPlaying(const string& _name);

    // 사운드 그룹 재생중인지
    _bool IsGroupPlaying(const string& _groupName);

    // 사운드가 끝났는지
    _bool IsGroupFinished(const string& _groupName);

    // 특정 이름의 사운드 볼륨 조절 (0.0f = 음소거, 1.0f = 원래 크기, 2.0f = 2배)
    void SetVolume(const string& _name, _float _volume);

    // 특정 그룹의 사운드 볼륨 조절
    void SetGroupVolume(const string& _groupName, _float _volume);

    // 마스터볼륨 조절
    void SetMasterVolume(_float _volume);
    _float GetMasterVolume() const;

    // 그룹에서 플레이중인 개수 확인
    _uint GetGroupPlayingCount(const string& _groupName);

    // 그룹에 사운드 등록 (미리 정의)
    void AddSoundToGroup(const string& _soundName, const string& _groupName);

    // 그룹에서 사운드 제거
    void RemoveSoundFromGroup(const string& _soundName, const string& _groupName);

    // 그룹 전체 재생
    void PlayGroup(const string& _groupName, _float _volume = 1.f, _bool _loop = false);

    // 그룹 클리어
    void ClearGroup(const string& _groupName);
    
    // 그룹 내 랜덤 1개 재생
    void PlayRandomInGroup(const string& _groupName, _float _volume = 1.f, _bool _loop = false);

    // 사운드 매니저에서 제거
    void Delete_Sound(const _string& _soundName);
    void Delete_Sound(SoundID _ID);


    // SoundID를 직접 받는 오버로드 (외부에서 해시를 미리 계산해둔 경우 사용)
    void Play_Sound(SoundID _id, _float _volume = 1.f, _bool _loop = false);
    void Play_Sound_InGroup(SoundID _id, SoundID _groupID, _float _volume = 1.f, _bool _loop = false);
    void StopSound(SoundID _id);
    void StopGroup(SoundID _groupID);
    void FadeOutSound(const string& _name, _float _fadeDuration);
    _bool IsPlaying(SoundID _id);
    _bool IsGroupPlaying(SoundID _groupID);
    _bool IsGroupFinished(SoundID _groupID);
    void SetVolume(SoundID _id, _float _volume);
    void SetGroupVolume(SoundID _groupID, _float _volume);


    //거리기반 사운드재생하기위한 함수객체
public:
    typedef std::function<_float(_uint iOwnerId, _float fOriginalVolume)> VolumeModifierFunc;
    void Set_VolumeModifier(VolumeModifierFunc fn) { m_fnVolumeModifier = fn; }
private:
    VolumeModifierFunc m_fnVolumeModifier = { nullptr };

private:
    struct SoundData
    {
        WAVEFORMATEX waveFormat = {};      // 사운드 형식 정보
        BYTE* audioData = nullptr;         // 사운드 데이터
        DWORD dataSize = {};
    };

    struct VoiceInstance
    {
        IXAudio2SourceVoice* sourceVoice = nullptr;
        const SoundData* sound = nullptr;
        VoiceCallback* callback = nullptr; // 콜백 포인터
        SoundID soundID = {};
        SoundID groupID = {};
    };

    // voice 공통 생성 로직(PlaySound류 공통 로직 여기 모아놓기)
    void CreateAndPlayVoice(SoundID _soundID, SoundID _groupID, const SoundData& _sound, _float _volume, _bool _loop);

    HRESULT LoadWAVFile(const std::wstring& _filePath, SoundData& _outSound);
    HRESULT LoadMP3File(const std::wstring& _filePath, SoundData& _outSound);
    HRESULT LoadOGGFile(const std::wstring& _filePath, SoundData& _outSound);

    void SoundEvent_Function(const SoundEvent& _event);

private:
    IXAudio2* m_xAudio2 = nullptr;
    IXAudio2MasteringVoice* m_masterVoice = nullptr;

    unordered_map<SoundID, SoundData> m_soundMap;
    vector<VoiceInstance> m_vecActiveVoices;
    unordered_map<SoundID, vector<SoundID>> m_soundGroups;

    _bool m_bHasSoundDevice = true;
    EventHandle m_iSoundEventHandle = {};

    std::mt19937 m_randomEngine = std::mt19937{ std::random_device{}() }; // 랜덤 엔진

private:// 페이드 아웃
    struct FadeTask
    {
        IXAudio2SourceVoice* pVoice;
        SoundID soundID;
        _float fCurrentVolume;
        _float fFadeSpeed;
    };

    std::vector<FadeTask> m_vecFadeTasks;

public:
    static SoundManager* Create();

public:
    void Free() override final;
};

NS_END