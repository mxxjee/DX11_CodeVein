# ============================================
# JSON ObjectKey 해시 제거 스크립트
# ============================================

import os
import re
import json
import sys

def main():
    if len(sys.argv) >= 2:
        targetDir = sys.argv[1]
    else:
        targetDir = os.path.dirname(os.path.abspath(__file__))

    print(f"대상 폴더: {targetDir}\n")

    hashPattern = re.compile(r'_[0-9a-fA-F]{6,10}$')

    # json 파일 스캔
    jsonFiles = [f for f in os.listdir(targetDir) if f.endswith('.json')]

    if not jsonFiles:
        print("JSON 파일이 없습니다.")
        input("\nEnter를 누르면 종료합니다...")
        return

    for jsonFileName in jsonFiles:
        jsonPath = os.path.join(targetDir, jsonFileName)
        modified = False

        with open(jsonPath, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # 최상위 구조에 따라 오브젝트 배열 추출
        if isinstance(data, list):
            items = data
        elif isinstance(data, dict) and 'data' in data:
            items = data['data']
        elif isinstance(data, dict):
            items = [data]
        else:
            items = []

        for item in items:
            if 'ObjectKey' not in item:
                continue

            objectKey = item['ObjectKey']
            # 경로의 마지막 부분에서 해시 제거
            parts = objectKey.rsplit('/', 1)
            if len(parts) == 2:
                prefix = parts[0]
                name = parts[1]
            else:
                prefix = None
                name = parts[0]

            # 추가됨 : Gaia 포함된 이름은 스킵
            if 'Gaia' in name:
                continue

            if hashPattern.search(name):
                cleanName = hashPattern.sub('', name)
                newKey = f"{prefix}/{cleanName}" if prefix else cleanName
                item['ObjectKey'] = newKey
                print(f"  수정: {objectKey} -> {newKey}")
                modified = True

        if modified:
            with open(jsonPath, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4, ensure_ascii=False)
            print(f"\n[{jsonFileName}] 저장 완료")
        else:
            print(f"[{jsonFileName}] 변경 없음")

    print(f"\n완료!")
    input("\nEnter를 누르면 종료합니다...")


if __name__ == "__main__":
    main()