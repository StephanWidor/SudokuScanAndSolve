#!/bin/bash
set -e

if [ -d android_build ]; then
    echo "deleting old build folder"
    rm -rf android_build
fi
mkdir android_build
cd android_build

QT_PATH=/opt/Qt/5.15.2
SDK_PATH=/opt/android/android-sdk
NDK_PATH=/opt/android/android-sdk/ndk/22.0.7026061
NDK_BIN_PATH="${NDK_PATH}/toolchains/llvm/prebuilt/linux-x86_64/bin"
JDK_PATH=/usr/lib/jvm/default-java

cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=${NDK_BIN_PATH}/clang++ \
    -DCMAKE_C_COMPILER=${NDK_BIN_PATH}/clang \
    -DCMAKE_PREFIX_PATH=${QT_PATH}/android \
    -DANDROID_NDK=$NDK_PATH \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    -DCMAKE_FIND_ROOT_PATH=${QT_PATH}/android \
    -DANDROID_SDK=$SDK_PATH \
    -DANDROID_NATIVE_API_LEVEL=24 \
    -DANDROID_ABIS="armeabi-v7a arm64-v8a x86 x86_64" \
    -DANDROID_BUILD_ABI_armeabi-v7a=ON \
    -DANDROID_BUILD_ABI_arm64-v8a=ON \
    -DANDROID_BUILD_ABI_x86=ON \
    -DANDROID_BUILD_ABI_x86_64=ON \ #-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
    ..
cmake --build . --target all

${QT_PATH}/android/bin/androiddeployqt \
    --input android_deployment_settings.json \
    --output android-build \
    --android-platform android-30 \
    --jdk ${JDK_PATH} \
    --gradle \
    --release \
    --aab \
    --deployment bundled

cd ..
