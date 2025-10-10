#include "SunsetBackTexture.h"

void SunsetBackTexture::Initialize(){
	//生成
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	//色の設定
	const Vector4 SUNUSET_BACK_TEXTURE_COLOUR = { .x = 1.0f,.y = 0.22f,.z = 0.0f,.w = 1.0f };
	backTexture_->SetClearColour(SUNUSET_BACK_TEXTURE_COLOUR);
	//初期化
	backTexture_->Initialize();

}
