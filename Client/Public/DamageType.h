#pragma once

enum DAMAGEBLOWTYPE
{
	BLOW, //날아가기 
	STRIKE, // 제자리 넘어지기
	LOOP, //Blow , Loop 이후에 누워있는 상태
};

enum DOWN_POSTURE
{
	PRONE, //엎드린 상태  (배를 바닥에)
	SUPINE //하늘을 보고 누운 상태 (등을 바닥에)
};

enum DEATHTYPE
{
    DEATHIDLE, //기본 죽을때 상태
    DOWNDEATH_S, //누운 상태에서 죽었을때 + 방향
    DOWNDEATH_P,//누운 상태에서 죽었을때 + 방향
};