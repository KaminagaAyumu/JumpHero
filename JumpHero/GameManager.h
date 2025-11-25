#pragma once


// プロトタイプ宣言
class Player;

/// <summary>
/// ゲーム内の状態を管理するクラス
/// </summary>
class GameManager
{
public:
	GameManager(Player* player);
	virtual ~GameManager();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコアの値</param>
	void AddScore(int score);

	bool IsClear()const { return m_isClear; }

private:

	int m_score; // 表示用のスコア
	int m_currentScore; // ゲーム内のスコア
	int m_life; // 残機数
	int m_medalNum; // 強化メダルの所持数
	int m_balloonNum; // 風船をどれだけ取ったか
	bool m_isClear; // クリア状態かどうか

	// プレイヤーの参照用
	Player* m_pPlayer;

};

