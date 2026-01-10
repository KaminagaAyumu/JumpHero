#include "Animation.h"
#include <unordered_map>
#include <string>
#include "AnimationLoader.h"
#include <fstream>
#include <sstream>

bool AnimationLoader::LoadAnimationData(const std::wstring& path, int graphHandle, std::unordered_map<std::string, Animation>& outAnimations)
{
	// ファイルを開く
	std::ifstream ifs(path);

	// ファイルが開けなかった場合はfalseを返す
	if (!ifs.is_open()) return false;

	std::string line;
	while (std::getline(ifs, line))
	{

	}
    return false;
}
