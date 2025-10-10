#include "Skeleton.h"
#include <Matrix4x4Calculation.h>
#include <Calculation/QuaternionCalculation.h>

void Skeleton::Create(const Node& rootNode){
    root = CreateJoint(rootNode, {}, joints);

    //名前とIndexのマッピングを行いアクセスしやすくする。
    for (const Joint& joint : joints) {
        //emplace...push_backみたいなもの。新しく挿入する。
        jointMap.emplace(joint.name, joint.index);
    }

}

void Skeleton::Update(){
    //全てのJointを更新。親が若いので通常ループで処理可能になっている。
    for (Joint& joint : joints) {

        Matrix4x4 scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(joint.transform.scale);
        Matrix4x4 rotateMatrix = QuaternionCalculation::MakeRotateMatrix(joint.transform.rotate);
        Matrix4x4 translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(joint.transform.translate);
        

        joint.localMatrix = Matrix4x4Calculation::Multiply(scaleMatrix,Matrix4x4Calculation::Multiply(rotateMatrix, translateMatrix));

        //親がいれば親の行列を掛ける
        if (joint.parent) {
            joint.skeletonSpaceMatrix = Matrix4x4Calculation::Multiply(joint.localMatrix, joints[*joint.parent].skeletonSpaceMatrix);
        }
        //親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
        else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }

    }


}
