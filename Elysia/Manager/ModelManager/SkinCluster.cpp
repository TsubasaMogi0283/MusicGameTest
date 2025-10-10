#include "SkinCluster.h"
#include <SrvManager.h>
#include <Matrix4x4Calculation.h>
#include <algorithm>
#include <PipelineManager.h>
#include "ModelManager.h"
#include <cassert>

void  SkinCluster::Create(const Skeleton& newSkeleton, const ModelData& modelData){
    skeleton = newSkeleton;

    //DirectXクラスを取得
    Elysia::DirectXSetup* directXSetup = Elysia::DirectXSetup::GetInstance();
    //SRV管理クラスを取得
    Elysia::SrvManager* srvManager = Elysia::SrvManager::GetInstance();


    //palette用のリソースを生成
    paletteResource = directXSetup->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
    WellForGPU* wellForGPU = nullptr;
    paletteResource->Map(0u, nullptr, reinterpret_cast<void**>(&wellForGPU));
    //spanを使ってアクセスするようにする
    mappedPalette = { wellForGPU,skeleton.joints.size() };
    srvIndex = srvManager->Allocate();
    paletteSrvHandle.first = srvManager->GetCPUDescriptorHandle(srvIndex);
    paletteSrvHandle.second = srvManager->GetGPUDescriptorHandle(srvIndex);
    paletteResource->Unmap(0u, nullptr);

    //Palette用のSRVの生成
    srvManager->CreateSRVForPalette(static_cast<UINT>(skeleton.joints.size()), sizeof(WellForGPU), paletteResource.Get(), srvIndex);



    //influence用のリソースを生成
    //頂点ごとにinfluence情報
    influenceResource = directXSetup->CreateBufferResource(sizeof(VertexInfluence) * modelData.vertices.size());
    VertexInfluence* vertexInfluence = nullptr;
    influenceResource->Map(0u, nullptr, reinterpret_cast<void**>(&vertexInfluence));
    //0埋め。Weightを0にしておく
    std::memset(vertexInfluence, 0u, sizeof(VertexInfluence) * modelData.vertices.size());
    mappedInfluence = { vertexInfluence,modelData.vertices.size() };

    //Influence用のVertexBufferViewを作成
    influenceBufferView.BufferLocation = influenceResource->GetGPUVirtualAddress();
    influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
    influenceBufferView.StrideInBytes = sizeof(VertexInfluence);
    influenceResource->Unmap(0u, nullptr);

    //InfluenceBindPoseMatrixの保存領域を作成
    inverseBindPoseMatrices.resize(skeleton.joints.size());
    //最後の所は「関数ポインタ」
    //()を付けないでね。ここ重要。
    std::generate(inverseBindPoseMatrices.begin(), inverseBindPoseMatrices.end(), Matrix4x4Calculation::MakeIdentity4x4);


    ////std::generate...初期化するときに便利！
    ////for文と似ているのでそっちでやっても◎
    ////実際はこんな感じ
    //for (int i = 0; i < skeleton.joints.size(); ++i) {
    //    skinCluster.inverseBindPoseMatrices[i] = MakeIdentity4x4();
    //}


    //ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める
    for (const auto& jointWeight : modelData.skinClusterData) {
        //JointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
        auto it = skeleton.jointMap.find(jointWeight.first);
        if (it == skeleton.jointMap.end()) {
            //その名前のJointは存在しないので次に回す
            continue;
        }
        //(*it).secondにはjointのindexが入っているので、街灯のindexのinverseBindPoseMatrixを代入
        inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
            //該当のvertexIndexのinfluence情報を参照しておく
            auto& currentInfluence = mappedInfluence[vertexWeight.vertexIndex];
            
            //空いている所に入れる
            for (uint32_t index = 0u; index < NUM_MAX_INFLUENCE; ++index) {
                //weight==0が空いている状態なので、その場所にweightとjointのindexを代入
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index] = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = (*it).second;
                    break;
                }
            }
        }
    }
}

void SkinCluster::Update(const Skeleton& newSkeleton){
    for (size_t jointIndex = 0; jointIndex < newSkeleton.joints.size(); ++jointIndex) {
        assert(jointIndex < inverseBindPoseMatrices.size());
        //それぞれの行列を計算
        mappedPalette[jointIndex].skeletonSpaceMatrix =
            Matrix4x4Calculation::Multiply(inverseBindPoseMatrices[jointIndex], newSkeleton.joints[jointIndex].skeletonSpaceMatrix);
        mappedPalette[jointIndex].skeletonSpaceIncerseTransposeMatrix = 
            Matrix4x4Calculation::MakeTransposeMatrix(Matrix4x4Calculation::Inverse(mappedPalette[jointIndex].skeletonSpaceMatrix));
    }


}
