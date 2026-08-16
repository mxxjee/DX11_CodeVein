#pragma once
#include "GameInstance.h"
//자주쓰는 사운드 함수 (볼륨한꺼번에조정하려고 여기모아둠)

static _float UIVolume = 0.3f;

inline void PlayCancleSound() { GameInstance::GetInstance()->Play_Sound("Cancle", UIVolume); }
inline void PlayHoverSound() { GameInstance::GetInstance()->Play_Sound("Hover", UIVolume-0.1f); }

inline void PlayArrowSound() { GameInstance::GetInstance()->Play_Sound("ArrowButton", UIVolume); }
inline void PlayClickSound() { GameInstance::GetInstance()->Play_Sound("Enter1_01", UIVolume); }
inline void PlayOpenSound() { GameInstance::GetInstance()->Play_Sound("Window_Open", UIVolume); }

inline void PlaySelectSound() { GameInstance::GetInstance()->Play_Sound("Select", UIVolume); }

inline void PlayLevelUpSound() { GameInstance::GetInstance()->Play_Sound("LevelUp", UIVolume); }