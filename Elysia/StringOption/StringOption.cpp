#include "StringOption.h"

#include <fstream>
#include <filesystem>

std::string StringOption::FindExtension(const std::string& directory, const std::string& baseFileName){
	//拡張子を探す
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_regular_file()) {
			std::string fileName = entry.path().stem().string();
			// 見つかったら返す
			if (fileName == baseFileName) {
				return entry.path().extension().string();
			}
		}
	}
	// 見つからなければ空文字を返す
	return {};
}
