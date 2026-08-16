import json
from collections import Counter

# ===== 설정 =====
INPUT_JSON = "C:\Users\swkk9\source\repos\02. 팀 프로젝트\01. 최종팀폴\DX11_Final_TeamProject\DataFiles\Level_Main\MapData.json"      # 입력 JSON 파일 경로
OUTPUT_TXT = "object_list.txt"  # 출력 txt 파일 경로

# ===== JSON 파일 읽기 =====
with open(INPUT_JSON, 'r', encoding='utf-8') as f:
    data = json.load(f)

# ===== ObjectKey 수집 =====
# data가 리스트인 경우와 딕셔너리 안에 리스트가 있는 경우 처리
if isinstance(data, list):
    items = data
else:
    # 딕셔너리라면 첫 번째 리스트 값을 찾음
    items = next((v for v in data.values() if isinstance(v, list)), [])

# ObjectKey만 추출
object_keys = [item.get("ObjectKey", "") for item in items if "ObjectKey" in item]

# ===== 종류별 카운트 =====
key_counts = Counter(object_keys)

# ===== txt 파일로 출력 =====
with open(OUTPUT_TXT, 'w', encoding='utf-8') as f:
    f.write(f"=== ObjectKey 목록 (총 {len(key_counts)}종, 전체 {len(object_keys)}개) ===\n\n")
    
    # 개수 많은 순으로 정렬
    for key, count in sorted(key_counts.items(), key=lambda x: -x[1]):
        f.write(f"[{key}] - {count}개\n")

print(f"완료! {len(key_counts)}종의 ObjectKey를 '{OUTPUT_TXT}'에 저장했습니다.")