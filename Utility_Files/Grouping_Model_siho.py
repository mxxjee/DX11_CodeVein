# ============================================
# 마지막 _해시 제거 + 원본 moved 폴더 이동 스크립트
# ============================================

import os
import re
import shutil
import sys

def main():
    # 수정됨 : 더블클릭 시 py파일이 위치한 폴더를 대상으로 실행
    if len(sys.argv) >= 2:
        targetDir = sys.argv[1]
    else:
        targetDir = os.path.dirname(os.path.abspath(__file__))

    print(f"대상 폴더: {targetDir}\n")

    if not os.path.isdir(targetDir):
        print(f"폴더를 찾을 수 없습니다: {targetDir}")
        input("\nEnter를 누르면 종료합니다...")
        return

    # 해시 패턴: 마지막 _뒤에 6~10자리 hex
    hashPattern = re.compile(r'_[0-9a-fA-F]{6,10}$')

    # 파일 스캔 + 그룹핑
    grouped = {}
    for fileName in os.listdir(targetDir):
        filePath = os.path.join(targetDir, fileName)
        if not os.path.isfile(filePath):
            continue

        # 수정됨 : 자기 자신(.py) 스킵
        if filePath == os.path.abspath(__file__):
            continue

        baseName, ext = os.path.splitext(fileName)

        # 수정됨 : .siho 파일만 처리
        if ext.lower() != '.siho':
            continue

        if hashPattern.search(baseName):
            cleanName = hashPattern.sub('', baseName)

            if cleanName not in grouped:
                grouped[cleanName] = []
            grouped[cleanName].append(filePath)

    if not grouped:
        print("해시 패턴이 붙은 .siho 파일이 없습니다.")
        input("\nEnter를 누르면 종료합니다...")
        return

    # moved 폴더 생성
    movedDir = os.path.join(targetDir, "moved")
    os.makedirs(movedDir, exist_ok=True)

    for cleanName, files in grouped.items():
        print(f"\n[{cleanName}] 그룹 - {len(files)}개 파일")

        # 첫 번째 파일 기준으로 클린 이름 파일 생성
        source = files[0]
        ext = os.path.splitext(source)[1]
        cleanFileName = cleanName + ext
        cleanPath = os.path.join(targetDir, cleanFileName)

        if not os.path.exists(cleanPath):
            shutil.copy2(source, cleanPath)
            print(f"  생성: {cleanFileName}")
        else:
            print(f"  이미 존재: {cleanFileName} (스킵)")

        # 원본 파일들 moved로 이동
        for filePath in files:
            fileName = os.path.basename(filePath)
            destPath = os.path.join(movedDir, fileName)
            shutil.move(filePath, destPath)
            print(f"  이동: {fileName} -> moved/")

    print(f"\n완료! 처리된 그룹: {len(grouped)}개")
    input("\nEnter를 누르면 종료합니다...")


if __name__ == "__main__":
    main()