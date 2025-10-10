#include "QuaternionCalculation.h"

#include <cmath>
#include <numbers>
#include "Vector3.h"
#include "VectorCalculation.h"


Quaternion QuaternionCalculation::QuaternionMultiply(const Quaternion& lhs, const Quaternion& rhs) {
    
    //wはそのまま
    //xはi部分
    //yはj部分
    //zはk部分
    Quaternion result = {
        .x = lhs.y * rhs.z - lhs.z * rhs.y + rhs.w * lhs.x + lhs.w * rhs.x,
        .y = lhs.z * rhs.x - lhs.x * rhs.z + rhs.w * lhs.y + lhs.w * rhs.y,
        .z = lhs.x * rhs.y - lhs.y * rhs.x + rhs.w * lhs.z + lhs.w * rhs.z,
        .w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
    };
    
    return result;
}

Quaternion QuaternionCalculation::IdentityQuaternion() {

    //e=1+0i+0j+0k
    //qe=eq=q

    Quaternion result = {
        .x = 0.0f,
        .y = 0.0f,
        .z = 0.0f,
        .w = 1.0f,
    };
    return result;
}

Quaternion QuaternionCalculation::Conjugate(const Quaternion& quaternion) {

    //q*=w-xi-yj-zk=(qw,-qv)
    //虚数部分があるところはマイナスになるよ
    Quaternion result = {
        .x = -quaternion.x,
        .y = -quaternion.y,
        .z = -quaternion.z,
        .w = quaternion.w,
    };
    
    return result;
}

float QuaternionCalculation::Norm(const Quaternion& quaternion) {

    //||q||=sqrtf(qq*)
    float result = 0.0f;
    result = sqrtf(
        quaternion.w * quaternion.w +
        quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z);

    return result;
}

Quaternion QuaternionCalculation::Normalize(const Quaternion& quaternion) {
    
    //長さを求める
    float length = sqrtf(
        quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w
    );

    //修正前
    Quaternion preResult = {
        preResult.x = quaternion.x,
        preResult.y = quaternion.y,
        preResult.z = quaternion.z,
        preResult.w = quaternion.w,
    };
    

    //長さが0の時以外割る
    if (length != 0.0f) {
        preResult.x = quaternion.x / length;
        preResult.y = quaternion.y / length;
        preResult.z = quaternion.z / length;
        preResult.w = quaternion.w / length;

    }

    //結果
    Quaternion result = {
        .x = preResult.x,
        .y = preResult.y,
        .z = preResult.z,
        .w = preResult.w,
    };
    
    return result;
}

Quaternion QuaternionCalculation::Inverse(const Quaternion& quaternion) {
    //q^-1=q*
    //   ||q||^2

    //ノルムを求める
    float norm = Norm(quaternion);
    Quaternion conjugate = Conjugate(quaternion);

    float t = norm * norm;

    //結果
    Quaternion result = {
        .x = conjugate.x / t,
        .y = conjugate.y / t,
        .z = conjugate.z / t,
        .w = conjugate.w / t,
    };
    

    return result;
}


//任意軸回転を表すQuaternionの生成
Quaternion QuaternionCalculation::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {

    //q=(cosθ/2,n*sinθ/2)
    Vector3 n = VectorCalculation::Normalize(axis);

    //結果
    Quaternion result = {
        .x = n.x * std::sinf(angle / 2.0f),
        .y = n.y * std::sinf(angle / 2.0f),
        .z = n.z * std::sinf(angle / 2.0f),
        .w = std::cosf(angle / 2.0f),
    };
    

    return result;
}

//ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 QuaternionCalculation::RotateVector(const Vector3& vector, const Quaternion& quaternion) {
    //wはいらない
    Quaternion convertQuaternion = {
        .x = vector.x,
        .y = vector.y,
        .z = vector.z,
        .w = 0.0f,
    };
    


    Quaternion conjugate = Conjugate(quaternion);

    //乗算
    Quaternion preResult = QuaternionMultiply(quaternion, QuaternionMultiply(convertQuaternion, conjugate));

    //結果
    //wはいらない
    Vector3 result = {
        .x= preResult.x,
        .y= preResult.y,
        .z= preResult.z 
    };
    return result;
}

//Quaternion版の回転行列を求める
Matrix4x4 QuaternionCalculation::MakeRotateMatrix(const Quaternion& quaternion) {
    Matrix4x4 result = {};
    float w = quaternion.w;
    float x = quaternion.x;
    float y = quaternion.y;
    float z = quaternion.z;

    result.m[0][0] = (w * w) + (x * x) - (y * y) - (z * z);
    result.m[0][1] = 2.0f * (x * y + w * z);
    result.m[0][2] = 2.0f * (x * z - w * y);
    result.m[0][3] = 0.0f;

    result.m[1][0] = 2.0f * (x * y - w * z);
    result.m[1][1] = (w * w) - (x * x) + (y * y) - (z * z);
    result.m[1][2] = 2.0f * (y * z + w * x);
    result.m[1][3] = 0.0f;

    result.m[2][0] = 2.0f * (x * z + w * y);
    result.m[2][1] = 2.0f * (y * z - w * x);
    result.m[2][2] = (w * w) - (x * x) - (y * y) + (z * z);
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;


    return result;
}

//回転の補間
Quaternion QuaternionCalculation::QuaternionSlerp(const Quaternion& q0, const Quaternion& q1, const float& t) {

    //q0・q1=||q0||・||q1||*cosθ
    //今は単位Quaternionを求めたいのでノルムは1なので
    //q0・q1 = cosθでOK!!
    float dot =
        q0.x * q1.x +
        q0.y * q1.y +
        q0.z * q1.z +
        q0.w * q1.w;

    //修正用
    Quaternion fixQuaternion0 = q0;


    //2通りあることから
    //詳しくは資料の14ページで
    if (dot < 0.0f) {
        //もう片方の回転を利用する
        fixQuaternion0.x = -q0.x;
        fixQuaternion0.y = -q0.y;
        fixQuaternion0.z = -q0.z;
        fixQuaternion0.w = -q0.w;

        //内積も反転
        dot = -dot;
    }

    //dotが限りなく1に近い場合
    const float EPSILON = 0.0005f;
    if (dot > 1.0f - EPSILON) {
        // 直線補間を行う
        Quaternion result = {};
        result.x = (1.0f - t) * fixQuaternion0.x + t * q1.x;
        result.y = (1.0f - t) * fixQuaternion0.y + t * q1.y;
        result.z = (1.0f - t) * fixQuaternion0.z + t * q1.z;
        result.w = (1.0f - t) * fixQuaternion0.w + t * q1.w;
        return result;
    }

    //角度を求める
    float theta = std::acosf(dot);

    //Quaternionの前にある係数
    //scale...係数
    float scale0 = float(std::sin((1 - t) * theta) / std::sin(theta));
    float scale1 = float(std::sin(t * theta) / std::sin(theta));

    //結果
    Quaternion result = {
        .x = scale0 * fixQuaternion0.x + scale1 * q1.x,
        .y = scale0 * fixQuaternion0.y + scale1 * q1.y,
        .z = scale0 * fixQuaternion0.z + scale1 * q1.z,
        .w = scale0 * fixQuaternion0.w + scale1 * q1.w,
    };
    

    return result;
}
