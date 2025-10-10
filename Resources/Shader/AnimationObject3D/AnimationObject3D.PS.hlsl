#include "AnimationObject3D.hlsli"

///PixelShader
//ここからの出力は色で、OutputManagerに送られて
//ブレンド処理も行い画面に色が打ち込まれる
 
 
//テクスチャを張り付けたり、ライティングを行ったり出来る。
//最も重要なShader

//ラスタライザー
//		↓VertexShadeOutput
//ピクセルシェーダー
//		↓PixelShadeOutput
//	 出力結合



//Material...色など三角形の表面の材質を決定するもの
struct Material{
    //色
    float4 color;
    //ライティングの設定
    int enableLighting;
    //UVトランスフォーム
    float4x4 uvTransform;
    //光沢度
    float shininess;
    //環境光の強さ
    float ambientIntensity;
    //環境マップ
    bool isEnviromentMap;
};


struct DirectionalLight{
	//ライトの色
    float4 color;
	//ライトの向き
    float3 direction;
	//ライトの輝度
    float intensity;
};

struct PointLight{
	//ライトの色
    float4 color;
	//ライトの位置
    float3 position;
	//ライトの輝度
    float intensity;
	
	//ライトに届く最大距離
    float radius;
	//減衰率
    float decay;

};


//カメラの位置を送る
struct Camera{
    float3 worldPosition;
};

//
struct SpotLight{
	//ライトの色
    float4 color;
	//ライトの位置
    float3 position;
	//輝度
    float intensity;

	//スポットライトの方向
    float3 direction;
	//ライトの届く最大距離
    float distance;
	//減衰率
    float decay;
	//Fallowoffを制御する
    float cosFallowoffStart;
	//スポットライトの余弦
    float cosAngle;
    
};

//
////ConstantBuffer<構造体>変数名:register(b0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnviromentTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);


//Textureは基本的にそのまま読まずSamplerを介して読む
//処理方法を記述している
//Samplerを介してを使ってTextureを読むことをSamplingという

//Textureの各PixelのことはTexelという
//Excelみたいだね()

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
	
    float3 camera = gCamera.worldPosition;
	
	//Materialを拡張する
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    if (textureColor.a <= 0.0f){
        discard;
    }
	
	//DirectionalLightingする場合
    if (gMaterial.enableLighting == 1){
		//Half Lambert
        float NdotL = dot(normalize(input.normal), -normalize(gDirectionalLight.direction));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		
		//Cameraへの方向を算出
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
		
		//入射光の反射ベクトルを求める
        float3 reflectLight = reflect(normalize(gDirectionalLight.direction), normalize(input.normal));
		
		
		//HalfVector
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
		
		//拡散反射
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		//鏡面反射
		//1.0f,1.0f,1.0fの所は反射色。
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
		
		
		//通常はこっち
        if (gMaterial.isEnviromentMap == false){
            output.color.rgb = diffuse + specular;
            output.color.a = gMaterial.color.a * textureColor.a;

        }
		//環境マップ
        if (gMaterial.isEnviromentMap == true){
            float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
            float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
            float4 enviromentColor = gEnviromentTexture.Sample(gSampler, reflectedVector);

            output.color.rgb = (enviromentColor.rgb) * (diffuse + specular);
            output.color.a = gMaterial.color.a * textureColor.a;
		
        }
    }
	//PointLight
    else if (gMaterial.enableLighting == 2){
		//光が当たらないところは「当たらない」のでもっと暗くなるわけではない。そこでsaturate関数を使う
		//saturate関数は値を[0,1]にclampするもの。エフェクターにもSaturationってあるよね。
	
		//点光源
        float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
		
		
		//Half Lambert
        float NdotL = dot(normalize(input.normal), -pointLightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		
		//Cameraへの方向を算出
        float3 toEye = normalize(gCamera.worldPosition - normalize(input.worldPosition));
		//入射光の反射ベクトルを求める
        float3 reflectLight = reflect(gPointLight.position, normalize(input.normal));
		
		//ポイントライトへの距離
        float distance = length(gPointLight.position - input.worldPosition);
		//指数によるコントロール
        float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
		
		//HalfVector
        float3 halfVector = normalize(-gPointLight.position + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
		
		
        float3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity;
        float3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
		
        
         //通常はこっち
        if (gMaterial.isEnviromentMap == false){
            output.color.rgb = (diffusePointLight + specularPointLight) * factor;
            output.color.a = gMaterial.color.a * textureColor.a;

        }
		//環境マップ
        if (gMaterial.isEnviromentMap == true){
            float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
            float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
            float4 enviromentColor = gEnviromentTexture.Sample(gSampler, reflectedVector);

            output.color.rgb = (enviromentColor.rgb) * ((diffusePointLight + specularPointLight) * factor);
            output.color.a = gMaterial.color.a * textureColor.a;
		
        }
        
        
    }
	//SpotLight
    else if (gMaterial.enableLighting == 3){
		//光が当たらないところは「当たらない」のでもっと暗くなるわけではない。そこでsaturate関数を使う
		//saturate関数は値を[0,1]にclampするもの。エフェクターにもSaturationってあるよね。
	
		//入射光の計算
        float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
		
		
		//Half Lambert
        float NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		
		//Cameraへの方向を算出
        float3 toEye = normalize(gCamera.worldPosition - normalize(input.worldPosition));
		//入射光の反射ベクトルを求める
        float3 reflectLight = reflect(gSpotLight.position, normalize(input.normal));
		
		//ポイントライトへの距離
        float distance = length(gSpotLight.position - input.worldPosition);
		//指数によるコントロール
        float attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay);
		
		//HalfVector
        float3 halfVector = normalize(-gSpotLight.position + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
		
		
		//角度が大きくなるほど光の強さは弱くなるよ
		//こういった指向性のある光源の、角度に応じた光の減衰はFalloffと呼ばれるよ
        float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
        float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFallowoffStart - gSpotLight.cosAngle));
		
		
        float3 diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity;
        float3 specularSpotLight = gSpotLight.color.rgb * gSpotLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
		
        
        //通常はこっち
        if (gMaterial.isEnviromentMap == false){
            output.color.rgb = (diffuseSpotLight + specularSpotLight) * attenuationFactor * falloffFactor;
            output.color.a = gMaterial.color.a * textureColor.a;

        }
		//環境マップ
        if (gMaterial.isEnviromentMap == true){
            float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
            float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
            float4 enviromentColor = gEnviromentTexture.Sample(gSampler, reflectedVector);

            output.color.rgb = (enviromentColor.rgb) * ((diffuseSpotLight + specularSpotLight) * attenuationFactor * falloffFactor);
            output.color.a = gMaterial.color.a * textureColor.a;
		
        }
        
    }
    else if (gMaterial.enableLighting == 4){
        float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 enviromentColor = gEnviromentTexture.Sample(gSampler, reflectedVector);

        output.color.rgb = enviromentColor.rgb;
        output.color.a = gMaterial.color.a;
		
    }
    else{
		//Lightingしない場合
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}