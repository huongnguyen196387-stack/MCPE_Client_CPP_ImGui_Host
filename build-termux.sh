#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
export ANDROID_HOME="${ANDROID_HOME:-$HOME/android-sdk}"
export ANDROID_SDK_ROOT="$ANDROID_HOME"
export ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-$ANDROID_HOME/ndk/27.2.12479018}"
export JAVA_HOME="${JAVA_HOME:-$PREFIX/lib/jvm/java-21-openjdk}"
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"

printf '%s\n' '== MCPE Client Android build ==' 
printf 'ANDROID_HOME=%s\n' "$ANDROID_HOME"
printf 'ANDROID_NDK_HOME=%s\n' "$ANDROID_NDK_HOME"
printf 'JAVA_HOME=%s\n' "$JAVA_HOME"

if ! command -v gradle >/dev/null 2>&1 && [ ! -x ./gradlew ]; then
  pkg install -y gradle
fi

[ -f local.properties ] || printf 'sdk.dir=%s\n' "$ANDROID_HOME" > local.properties

if [ -x ./gradlew ]; then
  ./gradlew clean --no-daemon
  ./gradlew assembleDebug --no-daemon --stacktrace
else
  gradle clean --no-daemon
  gradle assembleDebug --no-daemon --stacktrace
fi

APK=$(find app/build/outputs/apk -type f -name '*debug*.apk' | head -1 || true)
if [ -n "$APK" ]; then
  mkdir -p "$HOME/storage/downloads" 2>/dev/null || true
  cp "$APK" "$HOME/storage/downloads/MCPE_Client_CPP_ImGui_Host-debug.apk" 2>/dev/null || true
  echo "APK: $APK"
  ls -lh "$APK"
  [ -e "$HOME/storage/downloads/MCPE_Client_CPP_ImGui_Host-debug.apk" ] && \
    echo "Copied: $HOME/storage/downloads/MCPE_Client_CPP_ImGui_Host-debug.apk"
else
  echo 'ERROR: APK not found.'
  exit 2
fi
