# ============================================
# FBX 마지막 _해시 제거 + 원본 original 폴더 보관 스크립트
# ============================================

import os
import re
import shutil
import sys

def main():
    if len(sys.argv) >= 2:
        targetDir = sys.argv[1]
    else:
        targetDir = os.path.dirname(os.path.abspath(__file__))

    print(f"대상 폴더: {targetDir}\n")

    if not os.path.isdir(targetDir):
        print(f"폴더를 찾을 수 없습니다: {targetDir}")
        return

    # 해시 패턴: 마지막 _뒤에 6~10자리 hex
    hashPattern = re.compile(r'_[0-9a-fA-F]{6,10}$')

    # fbx 파일 스캔
    targets = []
    for fileName in os.listdir(targetDir):
        filePath = os.path.join(targetDir, fileName)
        if not os.path.isfile(filePath):
            continue

        baseName, ext = os.path.splitext(fileName)

        # fbx 파일만 처리
        if ext.lower() != '.fbx':
            continue

        # Gaia 포함된 파일 제외
        if 'Gaia' in baseName:
            continue

        if hashPattern.search(baseName):
            cleanName = hashPattern.sub('', baseName)
            targets.append((filePath, fileName, cleanName, ext))

    if not targets:
        print("해시 패턴이 붙은 .fbx 파일이 없습니다.")
        return

    # original 폴더 생성
    originalDir = os.path.join(targetDir, "original")
    os.makedirs(originalDir, exist_ok=True)

    for filePath, fileName, cleanName, ext in targets:
        cleanFileName = cleanName + ext
        cleanPath = os.path.join(targetDir, cleanFileName)

        # 원본을 original 폴더에 복사
        destPath = os.path.join(originalDir, fileName)
        shutil.copy2(filePath, destPath)
        print(f"  보관: {fileName} -> original/")

        # 해시 제거한 이름으로 리네임
        if not os.path.exists(cleanPath):
            os.rename(filePath, cleanPath)
            print(f"  리네임: {fileName} -> {cleanFileName}")
        else:
            # 이미 클린 이름 파일 존재하면 원본 삭제
            os.remove(filePath)
            print(f"  이미 존재: {cleanFileName} (원본 삭제)")

    print(f"\n완료! 처리된 파일: {len(targets)}개")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n에러 발생: {e}")
    finally:
        input("\nEnter를 누르면 종료합니다...")