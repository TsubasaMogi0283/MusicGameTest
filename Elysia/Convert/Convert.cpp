#include "Convert.h"

Vector4 Convert::Color::Adapter(const uint32_t& color) {
    Vector4 result = {
        //赤
       .x = ((color >> 24) & 0xFF) / 255.0f,
       //緑
       .y = ((color >> 16) & 0xFF) / 255.0f,
       //青
       .z = ((color >> 8) & 0xFF) / 255.0f,
       //透明度
       .w = ((color) & 0xFF) / 255.0f

    };

    return result;

}

std::wstring Convert::Text::ToWString(const std::string& text){
    //何もなかったら空を返す
	if (text.empty()){
		return std::wstring();
	}

	//マルチバイトに変換
	int32_t sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&text[0]), static_cast<int>(text.size()), NULL, 0);
	if (sizeNeeded == 0){
		return std::wstring();
	}

	//W文字列に変換
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&text[0]), static_cast<int>(text.size()), &result[0], sizeNeeded);
	return result;
}

std::string Convert::Text::ToString(const std::wstring& text){
	//何もなかったら空を返す
	if (text.empty()) {
		return std::string();
	}

	//マルチバイトに変換
	int32_t sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}

	//通常の文字列に変換
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
