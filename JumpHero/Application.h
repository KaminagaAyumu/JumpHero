#pragma once
/// <summary>
/// アプリケーションを管理するシングルトンクラス
/// </summary>
class Application
{
public:
	virtual ~Application(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static Application& GetInstance();

	/// <summary>
	/// ゲームの初期化
	/// </summary>
	/// <returns>成功 : true 失敗 : false</returns>
	bool Init();
	
	/// <summary>
	/// ゲームループを行う
	/// </summary>
	void Run();
	
	/// <summary>
	/// ゲームの終了
	/// </summary>
	void Terminate();


private:
	Application(); // コンストラクタをprivateで宣言
	Application(const Application&) = delete; // コピーコンストラクタを作れないようにする
	void operator=(const Application&) = delete; // 代入演算子も使えないようにする


};

