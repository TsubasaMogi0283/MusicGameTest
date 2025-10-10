#include "AnimationManager.h"

#include <cassert>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VectorCalculation.h"
#include "ModelManager.h"
#include "Calculation/QuaternionCalculation.h"

Elysia::AnimationManager* Elysia::AnimationManager::GetInstance(){
    static AnimationManager instance;
    return &instance;
}

Vector3 Elysia::AnimationManager::CalculationValue(const std::vector<KeyFrameVector3>& keyFrames, const float_t& time) {
    //特殊なケースを除外
    //キーが無いものは✕
    assert(!keyFrames.empty());
    //キーが1つか、時刻がキーフレーム前なら最初の値とする
    if (keyFrames.size() == 1u || time <= keyFrames[0].time) {
        return keyFrames[0].value;
    }

    for (size_t index = 0u; index < keyFrames.size() - 1u; ++index) {
        size_t nextIndex = index + 1u;
        //indexとnextIndexの2つのkeyFrameを取得して範囲内に時刻があるかを判定
        if (keyFrames[index].time <= time && time <= keyFrames[nextIndex].time) {
            //範囲内を補間する
            float_t t = (time - keyFrames[index].time) / (keyFrames[nextIndex].time - keyFrames[index].time);
            //Vector3 だと線形補間
            return VectorCalculation::Lerp(keyFrames[index].value, keyFrames[nextIndex].value, t);
        }
    }

    //ここまで来た場合は一番後ろの時刻よりも後ろなので最後の値を返すことにする
    return (*keyFrames.rbegin()).value;

}

Quaternion Elysia::AnimationManager::CalculationValue(const std::vector<KeyFrameQuaternion>& keyFrames, const float& time) {
    //特殊なケースを除外
    //キーが無いものは✕
    assert(!keyFrames.empty());
    //キーが1つか、時刻がキーフレーム前なら最初の値とする
    if (keyFrames.size() == 1u || time <= keyFrames[0].time) {
        return keyFrames[0].value;
    }

    for (size_t index = 0u; index < keyFrames.size() - 1u; ++index) {
        size_t nextIndex = index + 1u;
        //indexとnextIndexの2つのkeyFrameを取得して範囲内に時刻があるかを判定
        if (keyFrames[index].time <= time && time <= keyFrames[nextIndex].time) {
            //範囲内を補間する
            float_t t = (time - keyFrames[index].time) / (keyFrames[nextIndex].time - keyFrames[index].time);
            //QuaternionだとSlerp
            return QuaternionCalculation::QuaternionSlerp(keyFrames[index].value, keyFrames[nextIndex].value, t);
        }
    }
    //ここまで来た場合は一番後ろの時刻よりも後ろなので最後の値を返すことにする
    return (*keyFrames.rbegin()).value;
}



Animation Elysia::AnimationManager::LoadAnimationFile(const std::string& directoryPath, const std::string& fileName) {
    Animation animation = {};
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + fileName;
    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0u);

    //アニメーションが無い場合止める
    assert(scene->mNumAnimations != 0u);
    //最初のアニメーションだけを採用
    aiAnimation* animationAssimp = scene->mAnimations[0];

    //mTicksPerSecond...周波数
    //mDuration...mTicksPerSecondで指定された周波数における長さ
    animation.duration = static_cast<float_t>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);



    //NodeAnimationを解析する
    //assimpでは個々のNodeAnimationをChannelと呼んでいるのでChannelを回してNodeAnimationの情報をとってくる
    for (uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
        aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
        NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
        //Translate
        for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
            KeyFrameVector3 keyFrame = {};
            //秒変換
            keyFrame.time = static_cast<float_t>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            //右手->左手
            keyFrame.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
            nodeAnimation.translate.keyFrames.push_back(keyFrame);
        }

        //RotateはmNunRotateionKeys/mRotateKeys
        //RotateはQuaternionで、右手->左手に変換するために、YとZを反転させる必要がある。
        //Rotate
        for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
            aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
            KeyFrameQuaternion keyFrame = {};
            //秒変換
            keyFrame.time = static_cast<float_t>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            //右手->左手
            keyFrame.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
            nodeAnimation.rotate.keyFrames.push_back(keyFrame);
        }

        //ScaleはmNumScalingKeys/mScalingKeysで取得出来るので同時に行う
        //Scaleはそのままで良い

        //Scale
        for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
            KeyFrameVector3 keyFrame = {};
            //秒変換
            keyFrame.time = static_cast<float_t>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            keyFrame.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
            nodeAnimation.scale.keyFrames.push_back(keyFrame);
        }

    }


    return animation;
}

uint32_t Elysia::AnimationManager::LoadFile(const std::string& directoryPath, const std::string& fileName){

    for (uint32_t i = 0u; i < ANIMATION_MAX_AMOUNT_; ++i) {
        if (AnimationManager::GetInstance()->animationInfromtion_[i].directoryPath == directoryPath &&
            AnimationManager::GetInstance()->animationInfromtion_[i].fileName == fileName) {
            return AnimationManager::GetInstance()->animationInfromtion_[i].handle;
        }
    }
    

    ///読み込み
    Animation animation = AnimationManager::GetInstance()->LoadAnimationFile(directoryPath, fileName);


    //番号をインクリメント
    ++AnimationManager::GetInstance()->index_;

    //情報を登録
    AnimationManager::GetInstance()->animationInfromtion_[AnimationManager::GetInstance()->index_].animationData = animation;
    AnimationManager::GetInstance()->animationInfromtion_[AnimationManager::GetInstance()->index_].directoryPath = directoryPath;
    AnimationManager::GetInstance()->animationInfromtion_[AnimationManager::GetInstance()->index_].fileName = fileName;
    AnimationManager::GetInstance()->animationInfromtion_[AnimationManager::GetInstance()->index_].handle = AnimationManager::GetInstance()->index_;


    return AnimationManager::GetInstance()->index_;


}

void Elysia::AnimationManager::ApplyAnimation(Skeleton& skeleton,const uint32_t& animationHandle, float_t animationTime){


    Animation animationData = AnimationManager::GetInstance()->animationInfromtion_[animationHandle].animationData;
    for (Joint& joint : skeleton.joints) {
        //対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった

        if (auto it = animationData.nodeAnimations.find(joint.name); it != animationData.nodeAnimations.end()) {
            animationTime = std::fmodf(animationTime, animationData.duration);
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate = CalculationValue(rootNodeAnimation.translate.keyFrames, animationTime);
            joint.transform.rotate = CalculationValue(rootNodeAnimation.rotate.keyFrames, animationTime);
            joint.transform.scale = CalculationValue(rootNodeAnimation.scale.keyFrames, animationTime);
        }

    }
}


