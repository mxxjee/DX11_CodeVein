import json
import re
import os

# 1. 파일 이름 설정 (사용자님이 보내주신 파일명)
input_file = 'transforms.json'
output_file = 'Cleaned_Transforms.json'

try:
    # 2. JSON 파일 읽기 (객체 구조로 파싱)
    with open(input_file, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print(f"🔍 {input_file} 분석 중...")
    change_count = 0

    # 3. 데이터 리스트를 돌면서 'MeshName'만 수정
    for item in data:
        if 'MeshName' in item:
            original_name = item['MeshName']
            # 문자열 끝에 붙은 _ + 8자리 해시만 정확히 제거
            cleaned_name = re.sub(r'_[a-zA-Z0-9]{8}$', '', original_name)
            
            if original_name != cleaned_name:
                item['MeshName'] = cleaned_name
                change_count += 1

    # 4. 수정된 데이터를 다시 JSON으로 저장 (들여쓰기 유지)
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

    print(f"✅ 작업 완료!")
    print(f" - 수정된 MeshName 개수: {change_count}개")
    print(f" - 결과 파일: {output_file}")

except FileNotFoundError:
    print(f"❌ 에러: 폴더에 '{input_file}' 파일이 없습니다.")
    print(f"현재 폴더 파일 목록: {os.listdir('.')}")
except Exception as e:
    print(f"❌ 예상치 못한 에러 발생: {e}")

input("\n창을 닫으려면 엔터를 누르세요...")