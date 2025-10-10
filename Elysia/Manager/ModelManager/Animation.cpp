#include "Animation.h"
#include <cassert>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <VectorCalculation.h>
#include <Calculation/QuaternionCalculation.h>

Animation LoadAnimationFile(const std::string& directoryPath, const std::string& fileName){
    Animation animation = {};
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + fileName;
    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
   
    //アニメーションが無い場合止める
    assert(scene->mNumAnimations != 0);
    //最初のアニメーションだけを採用
    aiAnimation* animationAssimp = scene->mAnimations[0];

    //mTicksPerSecond...周波数
    //mDuration...mTicksPerSecondで指定された周波数における長さ
    animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
    


    //NodeAnimationを解析する
    //assimpでは個々のNodeAnimationをChannelと呼んでいるのでChannelを回してNodeAnimationの情報をとってくる
    for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
        aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
        NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
        //Translate
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
            KeyFrameVector3 keyFrame = {};
            //秒変換
            keyFrame.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            //右手->左手
            keyFrame.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
            nodeAnimation.translate.keyFrames.push_back(keyFrame);
        }

        //RotateはmNunRotateionKeys/mRotateKeys
        //RotateはQuaternionで、右手->左手に変換するために、YとZを反転させる必要がある。
        //Rotate
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
            aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
            KeyFrameQuaternion keyFrame = {};
            //秒変換
            keyFrame.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            //右手->左手
            //KeyFrame.value={rotate.x,-rotate.y,-rotate.z,rotate.w};
            keyFrame.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
            nodeAnimation.rotate.keyFrames.push_back(keyFrame);
        }

        //ScaleはmNumScalingKeys/mScalingKeysで取得出来るので同時に行う
        //Scaleはそのままで良い

        //Scale
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
            KeyFrameVector3 keyFrame = {};
            //秒変換
            keyFrame.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            keyFrame.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z};
            nodeAnimation.scale.keyFrames.push_back(keyFrame);
        }

    }


    return animation;
}

Vector3 CalculationValue(const std::vector<KeyFrameVector3>& keyFrames, float time){
    //特殊なケースを除外
    //キーが無いものは✕
    assert(!keyFrames.empty());
    //キーが1つか、時刻がキーフレーム前なら最初の値とする
    if (keyFrames.size() == 1 || time<=keyFrames[0].time) {
        return keyFrames[0].value;
    }

    for (size_t index = 0; index < keyFrames.size() - 1; ++index) {
        size_t nextIndex = index + 1;
        //indexとnextIndexの2つのkeyFrameを取得して範囲内に時刻があるかを判定
        if (keyFrames[index].time <= time && time <= keyFrames[nextIndex].time) {
            //範囲内を補間する
            float t = (time - keyFrames[index].time) / (keyFrames[nextIndex].time - keyFrames[index].time);
            //Vector3 だと線形補間
            return VectorCalculation::Lerp(keyFrames[index].value, keyFrames[nextIndex].value, t);
        }
    }

    //ここまで来た場合は一番後ろの時刻よりも後ろなので最後の値を返すことにする
    return (*keyFrames.rbegin()).value;

}

Quaternion CalculationValue(const std::vector<KeyFrameQuaternion>& keyFrames, float time) {
    //特殊なケースを除外
    //キーが無いものは✕
    assert(!keyFrames.empty());
    //キーが1つか、時刻がキーフレーム前なら最初の値とする
    if (keyFrames.size() == 1 || time<=keyFrames[0].time) {
        return keyFrames[0].value;
    }

    for (size_t index = 0; index < keyFrames.size() - 1; ++index) {
        size_t nextIndex = index + 1;
        //indexとnextIndexの2つのkeyFrameを取得して範囲内に時刻があるかを判定
        if (keyFrames[index].time <= time && time <= keyFrames[nextIndex].time) {
            //範囲内を補間する
            float t = (time - keyFrames[index].time) / (keyFrames[nextIndex].time - keyFrames[index].time);
            //QuaternionだとSlerp
            return QuaternionCalculation::QuaternionSlerp(keyFrames[index].value, keyFrames[nextIndex].value, t);
        }
    }
    //ここまで来た場合は一番後ろの時刻よりも後ろなので最後の値を返すことにする
    return (*keyFrames.rbegin()).value;
}

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime){

    for (Joint& joint : skeleton.joints) {
        //対象のJointのAnimation
        //対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった
        
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            //NodeAnimation& rootNodeAnimation=std::fmodf(animation.nodeAnimations[])
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate = CalculationValue(rootNodeAnimation.translate.keyFrames, animationTime);
            joint.transform.rotate = CalculationValue(rootNodeAnimation.rotate.keyFrames, animationTime);
            joint.transform.scale = CalculationValue(rootNodeAnimation.scale.keyFrames, animationTime);
        }

    }
}

