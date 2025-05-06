#!/bin/bash
set -e

export ANDROID_ABI=arm64-v8a # arm64-v8a armeabi-v7a x86 x86_64
export NDK_PATH=/opt/android-sdk/ndk/29.0.13113456
export NDK_BIN_PATH=$NDK_PATH/toolchains/llvm/prebuilt/linux-x86_64/bin
export QT_PATH=/opt/Qt/6.9.0
export QT_HOST_PATH=$QT_PATH/gcc_64
export SDK_PATH=/opt/android-sdk
export JDK_PATH=/opt/android-studio/jbr
export ANDROID_PLATFORM=android-35
export BUILD_FOLDER=build.android/$ANDROID_ABI
export BUILD_FOLDER_QT_APP=$BUILD_FOLDER/Qt
export ANDROID_STL=c++_static

if [ $ANDROID_ABI = "armeabi-v7a" ]; then
    export QT_PREFIX_PATH=$QT_PATH/android_armv7
elif [ $ANDROID_ABI = "arm64-v8a" ]; then
    export QT_PREFIX_PATH=$QT_PATH/android_arm64_v8a
elif [ $ANDROID_ABI = "x86" ]; then
    export QT_PREFIX_PATH=$QT_PATH/android_x86
elif [ $ANDROID_ABI = "x86_64" ]; then
    export QT_PREFIX_PATH=$QT_PATH/android_x86_64
fi

echo "Building Glubschauge for platform ${ANDROID_PLATFORM}, ${ANDROID_ABI}"

mkdir -p $BUILD_FOLDER
#rm -rf $BUILD_FOLDER/*

cmake \
    -B$BUILD_FOLDER \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_NDK=$NDK_PATH \
    -DANDROID_SDK_ROOT=$SDK_PATH \
    -DCMAKE_C_COMPILER=$NDK_BIN_PATH/clang \
    -DCMAKE_CXX_COMPILER=$NDK_BIN_PATH/clang++ \
    -DANDROID_ABI=$ANDROID_ABI \
    -DCMAKE_PREFIX_PATH=$QT_PREFIX_PATH \
    -DCMAKE_FIND_ROOT_PATH=$QT_PREFIX_PATH \
    -DQT_HOST_PATH:PATH=$QT_HOST_PATH \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_STL=$ANDROID_STL \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
    -DCMAKE_COLOR_DIAGNOSTICS=ON

cmake --build $BUILD_FOLDER --target all -j

$QT_PATH/gcc_64/bin/androiddeployqt \
    --input $BUILD_FOLDER_QT_APP/android-SudokuScanAndSolve-deployment-settings.json \
    --output $BUILD_FOLDER_QT_APP/android-build \
    --android-platform $ANDROID_PLATFORM \
    --jdk $JDK_PATH \
    --gradle \
    --release
