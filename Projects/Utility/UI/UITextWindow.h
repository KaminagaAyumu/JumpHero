#pragma once
#include <string>
#include "UIBase.h"
#include "../Geometry.h"
#include <vector>

/// <summary>
/// テキストとウィンドウを表示するUIクラス
/// </summary>
class UITextWindow : public UIBase
{
public:
	UITextWindow();
	virtual ~UITextWindow();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="text">表示するテキストの内容</param>
	/// <param name="size">ウィンドウのサイズ</param>
	/// <param name="target">表示する中心座標</param>
	void Init(std::string text, Size size, const Position2& target);

	/// <summary>
	/// 複数ページのテキストデータをセットする
	/// </summary>
	/// <param name="id">テキストのID</param>
	/// <param name="pages">テキストの内容</param>
	void SetPages(const std::string& id, const std::vector<TextData>& pages);

	void Update()override;
	void Draw()const override;

	/// <summary>
	/// ページを進める
	/// </summary>
	/// <returns>true : まだ進められる false : もう進められない</returns>
	bool AdvancePages();

	/// <summary>
	/// ウィンドウを閉じる
	/// </summary>
	/// <param name="disappearDuration">ウィンドウが閉じるまでの時間</param>
	void CloseWindow(float disappearDuration);

	bool IsAlive()const override;

	/// <summary>
	/// ページ付きのウィンドウを表示する(AppearFromCenterで表示します)
	/// </summary>
	/// <param name="duration">表示が終わるまでの時間</param>
	void ShowPaging(float duration);

	/// <summary>
	/// ウィンドウを右から表示する(スライドして動いてくる)
	/// </summary>
	/// <param name="duration">表示が終わるまでの時間</param>
	void ShowFromRight(float duration);

	/// <summary>
	/// ウィンドウを中央から表示する(拡大して現れる)
	/// </summary>
	/// <param name="duration">表示が終わるまでの時間</param>
	void AppearFromCenter(float duration);

	enum class TextWindowState
	{
		Hidden, // 非表示
		Appearing, // 表示中
		Visible, // 表示完了
		Disappearing // 非表示中
	};

private:
	/// <summary>
	/// テキストデータをページで管理する構造体
	/// </summary>
	struct TextPager
	{
		std::string id; // 識別ID
		std::vector<TextData> pages; // ページごとのテキストデータ
		int index = 0; // ページ数
		bool isActive = false; // ページ送りが進行中か
	};

	Position2 m_pos; // 現在の表示座標(中心座標)
	Position2 m_startPos; // アニメーション開始位置の座標
	Position2 m_targetPos; // 指定位置に移動するための座標
	std::string m_text; // 表示するテキスト
	Size m_size; // ウィンドウのサイズ
	Size m_targetSize; // アニメーション使用時のサイズ
	TextWindowState m_state; // ウィンドウの状態
	float m_appearRate; // 表示率(0.0~1.0)
	float m_appearDuration; // 表示アニメーションの時間
	bool m_isChangePos; // 位置変更アニメーション中かどうか
	bool m_isChangeSize; // サイズ変更アニメーション中かどうか
	int m_aliveFrame; // ウィンドウが表示されてからのフレーム数

	TextPager m_textPager; // ページ付きのテキストデータを管理するために使う

	/// <summary>
	/// 現在のページのテキストを適用する
	/// </summary>
	void ApplyCurrentPageText();
};

