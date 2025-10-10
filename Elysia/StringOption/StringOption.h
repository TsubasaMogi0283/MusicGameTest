#pragma once
#include <string>

/**
 * @file StringOption.h
 * @brief String関係の関数
 * @author 茂木翼
 */

/// <summary>
/// String関係の関数を集めたよ
/// </summary>
namespace StringOption{
	/// <summary>
	/// 拡張子を取得
	/// </summary>
	/// <param name="directory">ディレクトリー</param>
	/// <param name="baseFileName">元のファイル名</param>
	/// <returns>拡張子</returns>
	std::string FindExtension(const std::string& directory, const std::string& baseFileName);
};

