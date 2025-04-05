#!/bin/bash
set -e

export ANDROID_ABI=arm64-v8a # arm64-v8a armeabi-v7a x86 x86_64
export NDK_PATH=/opt/android-sdk/ndk/29.0.13113456
export NDK_BIN_PATH="${NDK_PATH}/toolchains/llvm/prebuilt/linux-x86_64/bin"
export QT_PATH=/opt/Qt/5.15.2
export QT_PREFIX_PATH=$QT_PATH/android
export QT_HOST_PATH=$QT_PATH/gcc_64
export SDK_PATH=/opt/android-sdk
export JDK_PATH=/opt/android-studio/jbr
export ANDROID_API=35
export ANDROID_PLATFORM=android-${ANDROID_API}
export BUILD_FOLDER=build.android/$ANDROID_ABI
export ANDROID_STL=c++_static

mkdir -p $BUILD_FOLDER
#rm -rf $BUILD_FOLDER/*

cmake \
    -B$BUILD_FOLDER \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_NDK=$NDK_PATH \
    -DANDROID_SDK_ROOT=$SDK_PATH \
    -DCMAKE_C_COMPILER=${NDK_BIN_PATH}/clang \
    -DCMAKE_CXX_COMPILER=${NDK_BIN_PATH}/clang++ \
    -DCMAKE_PREFIX_PATH=${QT_PREFIX_PATH} \
    -DCMAKE_FIND_ROOT_PATH=$QT_PREFIX_PATH \
    -DQT_HOST_PATH:PATH=$QT_HOST_PATH \
    -DANDROID_STL=$ANDROID_STL \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_NATIVE_API_LEVEL=${ANDROID_API} \
    -DANDROID_MIN_SDK_VERSION=28 \
    -DANDROID_TARGET_SDK_VERSION=${ANDROID_API} \
    -DANDROID_ABI=$ANDROID_ABI \
    -DCMAKE_COLOR_DIAGNOSTICS=ON

cmake --build $BUILD_FOLDER --target all -j

${QT_PATH}/android/bin/androiddeployqt \
    --input $BUILD_FOLDER/android_deployment_settings.json \
    --output $BUILD_FOLDER/android-build \
    --android-platform ${ANDROID_PLATFORM} \
    --jdk ${JDK_PATH} \
    --gradle \
    --release
