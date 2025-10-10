#pragma once

/**
 * @file Convert.h
 * @brief 変換関数を集めた
 * @author 茂木翼
 */

#include <cstdint>
#include <string>
#include <format>
#include <dxgidebug.h>
#include <dxcapi.h>

#include "Vector4.h"

/// <summary>
/// 変換
/// </summary>
namespace Convert {
	
	/// <summary>
	/// テキスト
	/// </summary>
	namespace Text {

		/// <summary>
		/// string型からwstring型に変換
		/// </summary>
		/// <param name="text">テキスト</param>
		/// <returns></returns>
		std::wstring ToWString(const std::string& text);

		/// <summary>
		/// wstring型からstring型に変換
		/// </summary>
		/// <param name="text">テキスト</param>
		/// <returns></returns>
		std::string ToString(const std::wstring& text);

	}

	/// <summary>
	/// 色
	/// </summary>
	namespace Color {

		/// <summary>
		/// 変換(uint32_t→Vector4) 
		/// </summary>
		/// <param name="color">色</param>
		/// <returns>色(Vector4)</returns>
		Vector4 Adapter(const uint32_t& color);

	}
	

}