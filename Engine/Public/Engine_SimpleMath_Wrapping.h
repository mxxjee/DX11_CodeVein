#pragma once

namespace SimpleMath_Lapping {
	constexpr DirectX::SimpleMath::Vector2 v2Zero	= { 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector2 v2One	= { 1.f, 1.f };

	constexpr DirectX::SimpleMath::Vector3 v3Zero	= { 0.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector3 v3One	= { 1.f, 1.f, 1.f };
	constexpr DirectX::SimpleMath::Vector3 v3Right	= { 1.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector3 v3Left	= { -1.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector3 v3Up		= { 0.f, 1.f, 0.f };
	constexpr DirectX::SimpleMath::Vector3 v3Down	= { 0.f, -1.f, 0.f };
	constexpr DirectX::SimpleMath::Vector3 v3Look	= { 0.f, 0.f, 1.f };
	constexpr DirectX::SimpleMath::Vector3 v3Forward= { 0.f, 0.f, 1.f };
	constexpr DirectX::SimpleMath::Vector3 v3Back	= { 0.f, 0.f, -1.f };

	constexpr DirectX::SimpleMath::Vector4 v4Zero	= { 0.f, 0.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4One	= { 1.f, 1.f, 1.f, 1.f };
	constexpr DirectX::SimpleMath::Vector4 v4Right	= { 1.f, 0.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Left	= { -1.f, 0.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Up		= { 0.f, 1.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Down	= { 0.f, -1.f, 0.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Look	= { 0.f, 0.f, 1.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Forward= { 0.f, 0.f, 1.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Back	= { 0.f, 0.f, -1.f, 0.f };
	constexpr DirectX::SimpleMath::Vector4 v4Pos	= { 0.f, 0.f, 0.f, 1.f };

	constexpr DirectX::SimpleMath::Matrix Identity = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

}

using namespace SimpleMath_Lapping;


/* SimpleMath 쓸 때 주의사항 */
//
//	상황					| 권장 방식		| 이유
//	게임오브젝트 Transform	| SimpleMath	| 오브젝트당 1회(가독성)
//	카메라 계산				| SimpleMath	| 프레임당 1~2회(가독성)
//	파티클 시스템(1000 + )	| DirectXMath	| 대량 반복(성능)
//	스키닝 / 본 시스템		| DirectXMath	| 본 개수 x 버텍스만큼(성능)
//	버텍스 변환(CPU)		| DirectXMath	| 대량 데이터(성능)
//	물리 충돌 검사			| 상황 따라		| 충돌 쌍이 많으면 성능, 아니면 가독성
//	UI 위치 계산			| SimpleMath	| 소량(가독성)
//	셰이더 상수버퍼 세팅	| 어느 쪽이든	| 어차피 Store해서 GPU전송