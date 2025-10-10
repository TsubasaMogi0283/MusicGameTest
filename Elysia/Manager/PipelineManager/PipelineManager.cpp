#include "PipelineManager.h"

#include <vector>

#include "WindowsSetup.h"
#include "BlendMode.h"
#include "Convert.h"



Elysia::PipelineManager* Elysia::PipelineManager::GetInstance() {
	static PipelineManager instance;
	return &instance;
}

void Elysia::PipelineManager::GenaratePSO(PSOInformation& psoInformation,const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc,const D3D12_BLEND_DESC& blendDesc,const D3D12_RASTERIZER_DESC& rasterizerDesc){

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoInformation.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { psoInformation.vertexShaderBlob_->GetBufferPointer(),psoInformation.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { psoInformation.pixelShaderBlob_->GetBufferPointer(),psoInformation.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	HRESULT hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoInformation.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));





}

void Elysia::PipelineManager::Initialize(){
	
	//ブレンドモード
	const uint32_t BLEND_MODE = BlemdMode::BlendModeNormal;

	//モデル
	SetModelBlendMode(BLEND_MODE);
	GenerateModelPSO();

	//スプライト
	SetSpriteBlendMode(BLEND_MODE);
	GenerateSpritePSO();
	
	// スプライト用のPSOを生成
	GenerateSpritePSO();

	// モデル用のPSOを生成
	GenerateModelPSO();

	// SkinningModel用のPSOを生成
	GenerateAnimationModelPSO();

	// 3Dパーティクル用のPSOを生成
	GenerateParticle3DPSO();

	// CopyImage用のPSOを生成
	GenarateFullScreenPSO();

	// GrayScale用のPSOを生成
	GenarateGrayScalePSO();

	// SepiaScale用のPSOを生成
	GenarateSepiaScalePSO();

	// Vignette用のPSOを生成
	GenarateVignettePSO();

	// BoxFilter用のPSOを生成
	GenarateBoxFilterPSO();

	// GaussianFilter用のPSOを生成
	GenarateGaussianFilterPSO();

	//OutLine用のPSOを生成
	GenarateLuminanceBasedOutlinePSO();

	//DepthBasedOutline用
	GenarateDepthBasedOutlinePSO();

	//RadialBlur用のPSOを生成
	GenerateRadialBlurPSO();

	//Dissolve用のPSOを生成
	GenarateDissolvePSO();

	//RandomEffect用のPSOを生成
	GenarateRandomEffectPSO();

	//SkyBoxのPSOの生成
	GenarateSkyBoxPSO();
}

void Elysia::PipelineManager::GenaratedLinePSO() {
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//CBVを使う
	//座標用
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;


	//rootParameterは今後必要あるたびに追加していく

	//CBVを使う
	//カメラ用
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//レジスタ番号1を使う
	rootParameters[1].Descriptor.ShaderRegister = 1;


	//CBVを使う
	//マテリアル用
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[2].Descriptor.ShaderRegister = 0;




	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->linePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->linePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->linePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->linePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));








	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);








	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//透明度これはどのブレンドモードでも同じ
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//通常ブレンド
	//線をブレンドする必要性が今のところないので通常だけにする
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;







	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;



	//ShaderをCompileする
	PipelineManager::GetInstance()->linePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Line3D/Line3D.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->linePSO_.vertexShaderBlob_ != nullptr);


	//PS
	PipelineManager::GetInstance()->linePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Line3D/Line3D.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->linePSO_.pixelShaderBlob_ != nullptr);





	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->linePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->linePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->linePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->linePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->linePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;


	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプライン
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->linePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));


}

void Elysia::PipelineManager::GenerateSpritePSO() {

	//PSO
	////RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;


	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;
	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//rootParameterは今後必要あるたびに追加していく

	//DescriptorRangle
	//複数枚のTexture(SRV)を扱う場合1つづつ設定すると効率低下に繋がる
	//利用する範囲を指定して一括で設定を行う機能のこと
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	//数は1つ
	descriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->spritePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->spritePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->spritePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->spritePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));








	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;



	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);








	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//透明度これはどのブレンドモードでも同じ
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//ブレンドモードの選択
	//switchでやった方が楽でしょう
	switch (PipelineManager::GetInstance()->selectSpriteBlendMode_) {

	case BlendModeNone:
		//ブレンド無し
		blendDesc.RenderTarget[0].BlendEnable = false;

		break;

	case BlendModeNormal:
		//通常ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;





		break;


	case BlendModeAdd:
		//加算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;






		break;


	case BlendModeSubtract:
		//減算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;


		break;

	case BlendModeMultiply:
		//乗算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

		break;

	case BlendModeScreen:
		//スクリーンブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;






		break;


	}







	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;



	//ShaderをCompileする
	PipelineManager::GetInstance()->spritePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Object2D/Object2d.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->spritePSO_.vertexShaderBlob_ != nullptr);



	PipelineManager::GetInstance()->spritePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Object2D/Object2d.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->spritePSO_.pixelShaderBlob_ != nullptr);





	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->spritePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->spritePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->spritePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->spritePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->spritePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;


	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	//ID3D12PipelineState* graphicsPipelineState_ = nullptr;
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->spritePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));




}

void Elysia::PipelineManager::GenerateModelPSO() {

	//PSO
	////RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[9] = {};
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;


	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;

	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//rootParameterは今後必要あるたびに追加していく

	//DescriptorRangle
	//複数枚のTexture(SRV)を扱う場合1つづつ設定すると効率低下に繋がる
	//利用する範囲を指定して一括で設定を行う機能のこと
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	//数は1つ
	descriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//CBVを使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[3].Descriptor.ShaderRegister = 1;


	//CBVを使う
	//カメラ用
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[4].Descriptor.ShaderRegister = 1;



	//PixelShaderに送る方のカメラ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[5].Descriptor.ShaderRegister = 2;


	//PointLight
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[6].Descriptor.ShaderRegister = 3;


	//SpotLight
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[7].Descriptor.ShaderRegister = 4;


	D3D12_DESCRIPTOR_RANGE enviromentDescriptorRange[1] = {};
	//0から始まる
	enviromentDescriptorRange[0].BaseShaderRegister = 1;
	//数は1つ
	enviromentDescriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	enviromentDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	enviromentDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[8].DescriptorTable.pDescriptorRanges = enviromentDescriptorRange;
	//Tableで利用する数
	rootParameters[8].DescriptorTable.NumDescriptorRanges = _countof(enviromentDescriptorRange);



	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob=nullptr;
	HRESULT hrResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->modelPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hrResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hrResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->modelPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->modelPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->modelPSO_.rootSignature_));
	assert(SUCCEEDED(hrResult));


	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	std::vector <D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	inputElementDescs.push_back({});
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs.push_back({});
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs.push_back({});
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc={
		.pInputElementDescs= inputElementDescs.data(),
		.NumElements= static_cast<UINT>(inputElementDescs.size()),
	};

	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//α
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;


	//ブレンドモードの選択
	//switchでやった方が楽でしょう
	switch (PipelineManager::GetInstance()->selectModelBlendMode_) {

	case BlendModeNone:
		//ブレンド無し
		blendDesc.RenderTarget[0].BlendEnable = false;

		break;

	case BlendModeNormal:
		//通常ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		break;


	case BlendModeAdd:
		//加算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;


	case BlendModeSubtract:
		//減算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;

	case BlendModeMultiply:
		//乗算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

		break;

	case BlendModeScreen:
		//スクリーンブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;
	}


	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//ShaderをCompileする
	PipelineManager::GetInstance()->modelPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Object3D/Object3d.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->modelPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->modelPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Object3D/Object3d.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->modelPSO_.pixelShaderBlob_ != nullptr);

	//PSOの生成
	GenaratePSO(PipelineManager::GetInstance()->modelPSO_, inputLayoutDesc, blendDesc, rasterizerDesc);

}

void Elysia::PipelineManager::GenerateAnimationModelPSO() {

	//PSO
	////RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[10] = {};
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;


	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;

	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//rootParameterは今後必要あるたびに追加していく

	//DescriptorRangle
	//複数枚のTexture(SRV)を扱う場合1つづつ設定すると効率低下に繋がる
	//利用する範囲を指定して一括で設定を行う機能のこと
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	//数は1つ
	descriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//CBVを使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[3].Descriptor.ShaderRegister = 1;


	//CBVを使う
	//カメラ用
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[4].Descriptor.ShaderRegister = 1;



	//PixelShaderに送る方のカメラ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[5].Descriptor.ShaderRegister = 2;


	//PointLight
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[6].Descriptor.ShaderRegister = 3;


	//SpotLight
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[7].Descriptor.ShaderRegister = 4;





	D3D12_DESCRIPTOR_RANGE descriptorRangeForWell[1] = {};
	//0から始まる
	descriptorRangeForWell[0].BaseShaderRegister = 0;
	//数は一つ
	descriptorRangeForWell[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRangeForWell[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForWell[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//Well用
	//今回はDescriptorTableを使う
	//Instancingを参考にして
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//VertwxShaderで使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報(t0)
	rootParameters[8].Descriptor.ShaderRegister = 0;
	//Tableの中身の配列を指定
	rootParameters[8].DescriptorTable.pDescriptorRanges = descriptorRangeForWell;
	//Tableで利用する数
	rootParameters[8].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForWell);




	D3D12_DESCRIPTOR_RANGE enviromentDescriptorRange[1] = {};
	//0から始まる
	enviromentDescriptorRange[0].BaseShaderRegister = 1;
	//数は1つ
	enviromentDescriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	enviromentDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	enviromentDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[9].DescriptorTable.pDescriptorRanges = enviromentDescriptorRange;
	//Tableで利用する数
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(enviromentDescriptorRange);



	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;



	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->animationModelPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hr)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hr = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->animationModelPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->animationModelPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->animationModelPSO_.rootSignature_));
	assert(SUCCEEDED(hr));

	//InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト



	//余分に読み込んでしまうけどこれしか方法が無かった
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	//float32_t4
	inputElementDescs[3].InputSlot = 1;								//1番目のslotのVBVのことだと伝える
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;	//int32_t4
	inputElementDescs[4].InputSlot = 1;								//1番目のslotのVBVのことだと伝える
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;




	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);






	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//α
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;


	//ブレンドモードの選択
	//switchでやった方が楽でしょう
	switch (PipelineManager::GetInstance()->selectAnimiationModelBlendMode_) {

	case BlendModeNone:
		//ブレンド無し
		blendDesc.RenderTarget[0].BlendEnable = false;

		break;

	case BlendModeNormal:
		//通常ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;




		break;


	case BlendModeAdd:
		//加算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;






		break;


	case BlendModeSubtract:
		//減算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;

	case BlendModeMultiply:
		//乗算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;




		break;

	case BlendModeScreen:
		//スクリーンブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;





		break;


	}







	//RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;



	//ShaderをCompileする
	PipelineManager::GetInstance()->animationModelPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/AnimationObject3D/AnimationObject3D.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->animationModelPSO_.vertexShaderBlob_ != nullptr);



	PipelineManager::GetInstance()->animationModelPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/AnimationObject3D/AnimationObject3D.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->animationModelPSO_.pixelShaderBlob_ != nullptr);





	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->animationModelPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->animationModelPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->animationModelPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->animationModelPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->animationModelPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	hr = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->animationModelPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hr));

}

void Elysia::PipelineManager::GenerateParticle3DPSO() {

	//PSO
	////RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[8] = {};
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	//0から始まる
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	//数は一つ
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	//今回はDescriptorTableを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//VertwxShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;
	//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	//Tableで利用する数
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//rootParameterは今後必要あるたびに追加していく

	//DescriptorRangle
	//複数枚のTexture(SRV)を扱う場合1つづつ設定すると効率低下に繋がる
	//利用する範囲を指定して一括で設定を行う機能のこと
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	//数は1つ
	descriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//DescriptorTableを使う
	//Texture用
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//カメラ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//レジスタ番号1を使う
	rootParameters[3].Descriptor.ShaderRegister = 0;


	//DirectionalLight
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[4].Descriptor.ShaderRegister = 1;

	//PS用のカメラ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[5].Descriptor.ShaderRegister = 2;

	//PointLight
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[6].Descriptor.ShaderRegister = 3;


	//SpotLight
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号2を使う
	rootParameters[7].Descriptor.ShaderRegister = 4;



	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->particle3DPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hr)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hr = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->particle3DPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->particle3DPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->particle3DPSO_.rootSignature_));
	assert(SUCCEEDED(hr));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//float4
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//float2
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//float3
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//加算ブレンド
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;


	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//今回はカリングをオフにする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//ShaderをCompileする
	PipelineManager::GetInstance()->particle3DPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Particle3D/Particle3d.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->particle3DPSO_.vertexShaderBlob_ != nullptr);
	PipelineManager::GetInstance()->particle3DPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Particle3D/Particle3d.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->particle3DPSO_.pixelShaderBlob_ != nullptr);

	//PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->particle3DPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->particle3DPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->particle3DPSO_.vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->particle3DPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->particle3DPSO_.pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	hr = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->particle3DPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hr));


}

void Elysia::PipelineManager::GenarateFullScreenPSO() {

	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	//Effect
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;



	//CBVを使う
	//Vignette
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 1;



	//応急処置でTextureを無理矢理2番目になるようにする
	//そろわなくなってしまう
	//いつか直す

	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);


	//CBVを使う
	//GaussianFilter
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[3].Descriptor.ShaderRegister = 2;


	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->fullScreenPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->fullScreenPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->fullScreenPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->fullScreenPSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->fullScreenPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/FullScreen/FullScreen.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->fullScreenPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->fullScreenPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/FullScreen/FullScreen.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->fullScreenPSO_.pixelShaderBlob_ != nullptr);


	//PSOの生成
	GenaratePSO(PipelineManager::GetInstance()->fullScreenPSO_, inputLayoutDesc, blendDesc, rasterizerDesc);

}

void Elysia::PipelineManager::GenarateGrayScalePSO(){
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};


	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr_ = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->grayScalePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hr_)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr_ = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->grayScalePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->grayScalePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->grayScalePSO_.rootSignature_));
	assert(SUCCEEDED(hr_));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->grayScalePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/GrayScale/GrayScale.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->grayScalePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->grayScalePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/GrayScale/GrayScale.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->grayScalePSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->grayScalePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->grayScalePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->grayScalePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->grayScalePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->grayScalePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hr_ = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->grayScalePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hr_));


}

void Elysia::PipelineManager::GenarateSepiaScalePSO(){
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};


	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->sepiaScalePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->sepiaScalePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->sepiaScalePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->sepiaScalePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->sepiaScalePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/SepiaScale/SepiaScale.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->sepiaScalePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->sepiaScalePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/SepiaScale/SepiaScale.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->sepiaScalePSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->sepiaScalePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->sepiaScalePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->sepiaScalePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->sepiaScalePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->sepiaScalePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->sepiaScalePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));

}

void Elysia::PipelineManager::GenarateVignettePSO(){
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};


	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);



	//CBVを使う
	//Scale,Pow
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->vignettePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->vignettePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->vignettePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->vignettePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->vignettePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Vignette/Vignette.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->vignettePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->vignettePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Vignette/Vignette.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->vignettePSO_.pixelShaderBlob_ != nullptr);


	GenaratePSO(PipelineManager::GetInstance()->vignettePSO_, inputLayoutDesc, blendDesc, rasterizerDesc);

}

void Elysia::PipelineManager::GenarateBoxFilterPSO(){
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};


	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);



	//CBVを使う
	//Scale,Pow
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->boxFilterPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->boxFilterPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->boxFilterPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->boxFilterPSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->boxFilterPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/BoxFilter/BoxFilter.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->boxFilterPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->boxFilterPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/BoxFilter/BoxFilter.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->boxFilterPSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->boxFilterPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->boxFilterPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->boxFilterPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->boxFilterPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->boxFilterPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->boxFilterPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));


	


}

void Elysia::PipelineManager::GenarateGaussianFilterPSO(){
	//RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};


	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);



	//CBVを使う
	//Scale,Pow
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult  = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->gaussianFilterPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->gaussianFilterPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->gaussianFilterPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->gaussianFilterPSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->gaussianFilterPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/GaussianFilter/GaussianFilter.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->gaussianFilterPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->gaussianFilterPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/GaussianFilter/GaussianFilter.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->gaussianFilterPSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->gaussianFilterPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->gaussianFilterPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->gaussianFilterPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->gaussianFilterPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->gaussianFilterPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->gaussianFilterPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));
}

void Elysia::PipelineManager::GenarateLuminanceBasedOutlinePSO() {

	///ootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};



	//応急処置でTextureを無理矢理2番目になるようにする
	//そろわなくなってしまう
	//いつか直す

	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/LuminanceBasedOutline/LuminanceBasedOutline.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/LuminanceBasedOutline/LuminanceBasedOutline.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->luminanceBasedOutlinePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));






}

void Elysia::PipelineManager::GenarateDepthBasedOutlinePSO() {
	///ootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};



	//応急処置でTextureを無理矢理2番目になるようにする
	//そろわなくなってしまう
	//いつか直す

	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//Camera
	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 0;



	//Depth
	D3D12_DESCRIPTOR_RANGE descriptorRangeForDepth[1] = {};
	descriptorRangeForDepth[0].BaseShaderRegister = 1;
	descriptorRangeForDepth[0].NumDescriptors = 1;
	descriptorRangeForDepth[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForDepth[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForDepth;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForDepth);



	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	//Depth
	//ポイントフィルタ
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	//0~1の範囲外をリピート
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[1].ShaderRegister = 1;
	//PixelShaderで使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);


	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->depthBasedOutlinePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->depthBasedOutlinePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->depthBasedOutlinePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->depthBasedOutlinePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->depthBasedOutlinePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/DepthBasedOutline/DepthBasedOutline.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->depthBasedOutlinePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->depthBasedOutlinePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/DepthBasedOutline/DepthBasedOutline.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->depthBasedOutlinePSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->depthBasedOutlinePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->depthBasedOutlinePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->depthBasedOutlinePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->depthBasedOutlinePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->depthBasedOutlinePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->depthBasedOutlinePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));



}

void Elysia::PipelineManager::GenerateRadialBlurPSO() {
	///ootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};



	//応急処置でTextureを無理矢理2番目になるようにする
	//そろわなくなってしまう
	//いつか直す

	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult= D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->radialBlurPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->radialBlurPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->radialBlurPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->radialBlurPSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->radialBlurPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/RadialBlur/RadialBlur.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->radialBlurPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->radialBlurPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/RadialBlur/RadialBlur.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->radialBlurPSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->radialBlurPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->radialBlurPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->radialBlurPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->radialBlurPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->radialBlurPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->radialBlurPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));



}

void Elysia::PipelineManager::GenarateDissolvePSO() {

	///ootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};



	//応急処置でTextureを無理矢理2番目になるようにする
	//そろわなくなってしまう
	//いつか直す

	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);



	//MaskTexture
	D3D12_DESCRIPTOR_RANGE maskTextureDescriptorRange[1] = {};
	maskTextureDescriptorRange[0].BaseShaderRegister = 1;
	maskTextureDescriptorRange[0].NumDescriptors = 1;
	maskTextureDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	maskTextureDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].DescriptorTable.pDescriptorRanges = maskTextureDescriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(maskTextureDescriptorRange);



	//CBVを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[2].Descriptor.ShaderRegister = 0;


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);







	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult= D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->dissolvePSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->dissolvePSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->dissolvePSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->dissolvePSO_.rootSignature_));
	assert(SUCCEEDED(hResult));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->dissolvePSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Dissolve/Dissolve.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->dissolvePSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->dissolvePSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/Dissolve/Dissolve.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->dissolvePSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->dissolvePSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->dissolvePSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->dissolvePSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->dissolvePSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->dissolvePSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->dissolvePSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));

}

void Elysia::PipelineManager::GenarateRandomEffectPSO() {
	///ootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように関連づけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};




	//Texture
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);




	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//PixelShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//レジスタ番号1を使う
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);



	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr_= D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->randomEffectPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hr_)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr_ = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->randomEffectPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->randomEffectPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->randomEffectPSO_.rootSignature_));
	assert(SUCCEEDED(hr_));



	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	////RasterizerState
	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする
	PipelineManager::GetInstance()->randomEffectPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/RandomEffect/RandomEffect.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->randomEffectPSO_.vertexShaderBlob_ != nullptr);

	PipelineManager::GetInstance()->randomEffectPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/RandomEffect/RandomEffect.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->randomEffectPSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->randomEffectPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->randomEffectPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->randomEffectPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->randomEffectPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->randomEffectPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	hr_ = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->randomEffectPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hr_));

}

void Elysia::PipelineManager::GenarateSkyBoxPSO() {

	//PSO
	////RootSignatureを作成
	//RootSignature・・ShaderとResourceをどのように間レンズけるかを示したオブジェクトである
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//rootParameter生成。複数設定できるので配列。
	//今回は結果一つだけなので長さ１の配列

	//VSでもCBufferを利用することになったので設定を追加
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	//WorldTransform用
	//CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	////PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//レジスタ番号とバインド
	//register...Shader上のResource配置情報
	rootParameters[0].Descriptor.ShaderRegister = 0;


	//Camera用
	//CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//register...Shader上のResource配置情報
	rootParameters[1].Descriptor.ShaderRegister = 1;

	//ルートパラメータ配列へのポイント
	descriptionRootSignature_.pParameters = rootParameters;
	//配列の長さ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	//rootParameterは今後必要あるたびに追加していく

	//DescriptorRangle
	//複数枚のTexture(SRV)を扱う場合1つづつ設定すると効率低下に繋がる
	//利用する範囲を指定して一括で設定を行う機能のこと
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	//数は1つ
	descriptorRange[0].NumDescriptors = 1;
	//SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//Texture
	//DescriptorTableを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//PixelShaderを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);



	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//ありったけのMipmapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	//PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);




	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//VertwxShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//register...Shader上のResource配置情報
	rootParameters[3].Descriptor.ShaderRegister = 0;




	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hResult = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &PipelineManager::GetInstance()->skyBoxPSO_.signatureBlob_, &errorBlob);
	if (FAILED(hResult)) {
		Elysia::WindowsSetup::GetInstance()->OutPutStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateRootSignature(0, PipelineManager::GetInstance()->skyBoxPSO_.signatureBlob_->GetBufferPointer(),
		PipelineManager::GetInstance()->skyBoxPSO_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&PipelineManager::GetInstance()->skyBoxPSO_.rootSignature_));
	assert(SUCCEEDED(hResult));


	////InputLayout
	//InputLayout・・VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);






	////BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//α
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;


	//ブレンドモードの選択
	//switchでやった方が楽でしょう
	switch (PipelineManager::GetInstance()->selectAnimiationModelBlendMode_) {

	case BlendModeNone:
		//ブレンド無し
		blendDesc.RenderTarget[0].BlendEnable = false;

		break;

	case BlendModeNormal:
		//通常ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;




		break;


	case BlendModeAdd:
		//加算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;






		break;


	case BlendModeSubtract:
		//減算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;

	case BlendModeMultiply:
		//乗算ブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

		break;

	case BlendModeScreen:
		//スクリーンブレンド
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		break;
	}




	//RasterizerState・・・Rasterizerに対する設定
	//					  三角形の内部をピクセルに分解して、
	//					  PixelShaderを起動することでこの処理への設定を行う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;



	//ShaderをCompileする
	PipelineManager::GetInstance()->skyBoxPSO_.vertexShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/SkyBox/SkyBox.VS.hlsl", L"vs_6_0");
	assert(PipelineManager::GetInstance()->skyBoxPSO_.vertexShaderBlob_ != nullptr);
	PipelineManager::GetInstance()->skyBoxPSO_.pixelShaderBlob_ = DirectXSetup::GetInstance()->CompileShader(L"Resources/Shader/SkyBox/SkyBox.PS.hlsl", L"ps_6_0");
	assert(PipelineManager::GetInstance()->skyBoxPSO_.pixelShaderBlob_ != nullptr);


	////PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineManager::GetInstance()->skyBoxPSO_.rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { PipelineManager::GetInstance()->skyBoxPSO_.vertexShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->skyBoxPSO_.vertexShaderBlob_->GetBufferSize() };
	//vertexShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.PS = { PipelineManager::GetInstance()->skyBoxPSO_.pixelShaderBlob_->GetBufferPointer(),PipelineManager::GetInstance()->skyBoxPSO_.pixelShaderBlob_->GetBufferSize() };
	//pixelShaderBlob_->GetBufferSize();
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;



	//どのように画面に色を打ち込むのか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual 近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;





	//実際に生成
	hResult = DirectXSetup::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&PipelineManager::GetInstance()->skyBoxPSO_.graphicsPipelineState_));
	assert(SUCCEEDED(hResult));


}
