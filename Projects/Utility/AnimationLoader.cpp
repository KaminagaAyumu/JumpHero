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
	bool isHeader = true; // 最初の1行目かどうかのフラグ
	while (std::getline(ifs, line))
	{
		// 最初の1行は読み込まない
		if (isHeader)
		{
			isHeader = false; // 1行目を読み込んだらフラグをfalseにする
			continue; // 1行目はヘッダーなので読み飛ばす
		}

		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> row;
		while (getline(stream, field, ','))
		{
			row.push_back(field);
		}

		// 各データを取得する
		const std::string name = row[0];
		const int width = std::stoi(row[1]);
		const int height = std::stoi(row[2]);
		const int animNum = std::stoi(row[3]);
		const int animFrame = std::stoi(row[4]);
		const int animIndex = std::stoi(row[5]);
		const bool isLoop = row[6] == "1" ? true : false; // "1"ならtrue、それ以外ならfalseにする

		Animation animation;
		if (animIndex >= 0)
		{
			animation.SetAnimation(graphHandle, { width, height }, animIndex, animNum, animFrame, isLoop);
		}
		else
		{
			animation.SetAnimation(graphHandle, { width, height }, animNum, animFrame, isLoop);
		}

		// アニメーションを格納する
		outAnimations.emplace(name, animation);
	}
	// ここまで来たらロードが正常に終わったのでtrueを返す
	return true;
}
