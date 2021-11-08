// ============================================================================
//
// メイン (このファイルがはじめに読み込まれる。今回はこのファイルしかない。)
//
// ----------------------------------------------------------------------------
//
// □ 2010年06月30日 (PSGC)
//		・ハイスコアを消去できるようにした
//
// □ 2010年06月28日 (PSGC)
//		・フルスクリーンかウィンドウか選べるようにした
//		・敵を海より前面に見えるようにした
//		・敵の数を減らして難易度を落とした
//		・敵が速くなる頻度を上げたが速くなる間隔をあけた
//		・エンドレスになったｗ
//		・セーブファイルをテキストファイルからバイナリファイルにした
//
// □ 2010年06月25日 (PSGC)
//		・怒涛のコーディング(笑)
//
// □ 2010年06月21日 (PSGC)
//		・手続きを分けるために処理を関数に分けた
//
// □ 2010年06月20日 (PSGC)
//		・新規作成 / とりあえずメイン関数と初期化関連のみ
//
// ============================================================================

// ----------------------------------------------------------------------------
// ライブラリのインクルード
// ----------------------------------------------------------------------------

#include "DxLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ----------------------------------------------------------------------------
// 定数の宣言
// ----------------------------------------------------------------------------

#define WINDOW_X 0			// 画面の上端
#define WINDOW_Y 0			// 画面の左端
#define WINDOW_MAX_X 640	// 画面の右端
#define WINDOW_MAX_Y 480	// 画面の下端
#define COLOR_BIT 32		// 画面の色深度

#define ENEMY_MAX 600		// 敵の最大数
// ゲームモード
enum {
	GAMETITLE, GAMEMAIN, GAMEOVER
};

// ----------------------------------------------------------------------------
// グローバル変数の宣言
// ----------------------------------------------------------------------------

// グラフィックハンドル
int titleScreen, missScreen;

// BGMハンドル
int stageMusic;

// SEハンドル
int floatingSE, thunderSE, missSE;

// 移動物体の構造体
struct MOVER {
	int x, y;				// 座標
	double speedx, speedy;	// 速度
	int image;				// 画像ハンドル
};

typedef MOVER PLAYER;		// 自機の構造体
typedef MOVER ENEMY;		// 敵の構造体
typedef MOVER THUNDER;		// 雷の構造体
typedef MOVER SEA;			// 海の構造体

PLAYER player;
ENEMY enemy[ENEMY_MAX];
THUNDER thunder;
SEA sea;

int gameState;				// ゲームモードを格納する変数
int stageCount;				// 経過時間を格納する変数
int score;					// 今回のスコア
int hiScore;				// 最高記録

int countUpperSide;			// 画面上部にいた時間
int countLowerSide;			// 画面下部にいた時間

// ----------------------------------------------------------------------------
// 関数のプロトタイプ宣言
// ----------------------------------------------------------------------------

void InitDxLibBefore();
void InitDxLibAfter();
void LoadFiles();
void Initialize();
void Update();
void Draw();
void DoMainLoop();
void Write();
int Read();

// ----------------------------------------------------------------------------
// メイン関数
// ----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	// 乱数の種
	srand((unsigned)time(NULL));

	// DXライブラリ初期化前の処理
	InitDxLibBefore();

	// DXライブラリ初期化処理
	if (DxLib_Init() == -1) return -1;

	// DXライブラリ初期化後の処理
	InitDxLibAfter();

	// 各種ファイルの読み込み
	LoadFiles();

	// 変数などの初期化
	Initialize();

	// メインループ
	while (1) {
		// キャンバスをまっさらにする
		ClearDrawScreen();

		// 描画処理
		DoMainLoop();

		// 裏画面の内容を反映
		ScreenFlip();

		// メッセージ処理をやってもらう
		if (ProcessMessage() < 0) break;

		// ESCキーで終了
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
	}

	// DXライブラリ終了処理
	DxLib_End();

	// アプリケーションの終了処理
	return 0;
}

// ----------------------------------------------------------------------------
// DXライブラリ初期化前の処理
// ----------------------------------------------------------------------------
void InitDxLibBefore()
{
	// ウィンドウのタイトルを設定
#ifdef _DEBUG
	SetWindowText("DANGEROUS TRIP - DEBUG MODE");
#else
	SetWindowText("DANGEROUS TRIP");
#endif

	// ログ出力の有無(TRUE: する, FALSE: しない)
	SetOutApplicationLogValidFlag(FALSE);

	// アーカイブ機能を使う
	SetUseDXArchiveFlag(TRUE);

	// アーカイブの拡張子
	SetDXArchiveExtension("arc");

	// フルスクリーン or ウィンドウ
	int isFullScreen = MessageBox(NULL, TEXT("フルスクリーンで起動しますか"),
		TEXT("Dangerous Trip"), MB_YESNO | MB_ICONQUESTION);

	if (isFullScreen == IDYES) {
		ChangeWindowMode(FALSE);
	}
	else {
		ChangeWindowMode(TRUE);
	}

	// 画面サイズなどを設定
	SetGraphMode(WINDOW_MAX_X, WINDOW_MAX_Y, COLOR_BIT);
}

// ----------------------------------------------------------------------------
// DXライブラリ初期化後の処理
// ----------------------------------------------------------------------------
void InitDxLibAfter()
{
	// 透過色の変更
	SetTransColor(255, 0, 255);

	// 裏画面処理
	SetDrawScreen(DX_SCREEN_BACK);

	// 起動直後はタイトル画面へ
	gameState = GAMETITLE;
}

// ----------------------------------------------------------------------------
// 各種ファイルの読み込み
// ----------------------------------------------------------------------------
void LoadFiles()
{
	// BGMのロード
	stageMusic = LoadSoundMem(".\\sound\\stage.ogg");

	// SEのロード
	floatingSE = LoadSoundMem(".\\sound\\floating.wav");
	thunderSE = LoadSoundMem(".\\sound\\thunder.wav");
	missSE = LoadSoundMem(".\\sound\\miss.wav");

	// グラフィックのロード
	player.image = LoadGraph(".\\image\\player.png");
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].image = LoadGraph(".\\image\\enemy.png");
	};
	thunder.image = LoadGraph(".\\image\\thunder.png");
	sea.image = LoadGraph(".\\image\\sea.png");

	titleScreen = LoadGraph(".\\image\\title.png");
	missScreen = LoadGraph(".\\image\\miss.png");
}

// ----------------------------------------------------------------------------
// 変数などの初期化
// ----------------------------------------------------------------------------
void Initialize()
{
	// 自機の初期座標
	player.x = (int)(WINDOW_MAX_X * 0.8);
	player.y = (int)(WINDOW_MAX_Y * 0.4);

	// 自機の初期速度
	player.speedx = 0;
	player.speedy = 0;

	// 敵の初期座標
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].x = (rand() % 10000) - 10000;
		enemy[i].y = (rand() % 1000);
	}

	// 敵の初期速度
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].speedx = 1;
	}

	// 海の初期座標
	sea.x = WINDOW_MAX_X - 16;
	sea.y = WINDOW_MAX_Y - 32;

	// 開始直後のスコアは0点
	score = 0;

	// ハイスコアを読み込む
	hiScore = Read();

	// 開始直後の経過時間は0
	stageCount = 0;

	// 開始直後は上にも下にもいない
	countUpperSide = 0;
	countLowerSide = 0;
}

// ----------------------------------------------------------------------------
// 移動処理・変数の更新処理
// ----------------------------------------------------------------------------
void Update()
{
	int key = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// ------------------------------------------------------------------------
	// タイトル画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMETITLE) {
		// 他画面から遷移してきたときのためにもう一度初期化
		Initialize();

		// Cキーが押されたらハイスコアを消去
		if (key & PAD_INPUT_C) {
			hiScore = 0;
			Write();
		}

		// Zキーが押されたらゲームメイン画面へ
		if (key & PAD_INPUT_A) {
			gameState = GAMEMAIN;
		}
	}

	// ------------------------------------------------------------------------
	// ゲームメイン画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMEMAIN) {
		// 音楽が鳴っていなかったら鳴らす
		if (!(CheckSoundMem(stageMusic))) {
			PlaySoundMem(stageMusic, DX_PLAYTYPE_LOOP);
		}

		// カーソルキーで左右に移動
		if (key & PAD_INPUT_LEFT) player.speedx -= 0.2;
		if (key & PAD_INPUT_RIGHT) player.speedx += 0.2;

		// 加速しすぎ防止
		if (player.speedx < -6) player.speedx = -6;
		if (player.speedx > 6) player.speedx = 6;

		// 最後に速度を足して移動完了
		player.x += (int)player.speedx;

		// Zキーで上昇
		if (key & PAD_INPUT_A) {
			player.speedy -= 0.2;

			// 音が鳴っていなかったら音を鳴らす
			if (!(CheckSoundMem(floatingSE))) {
				PlaySoundMem(floatingSE, DX_PLAYTYPE_BACK);
			}
		}
		else {
			player.speedy += 0.2;
		}

		// 縦方向に加速しすぎ防止
		if (player.speedy < -6) player.speedy = -6;
		if (player.speedy > 6) player.speedy = 6;

		// 最後に速度を足して移動完了
		player.y += (int)player.speedy;

		// 画面外に出ないように
		if (player.x < WINDOW_X) player.x = WINDOW_X;
		if (player.x > WINDOW_MAX_X - 64) player.x = WINDOW_MAX_X - 64;
		if (player.y < WINDOW_Y) player.y = WINDOW_Y;

		// 敵を動かす
		for (int i = 0; i < ENEMY_MAX; i++) {
			enemy[i].x += (int)enemy[i].speedx;
			// 敵が過ぎ去ったら位置を元に戻す。お手軽エンドレス
			if (enemy[i].x > WINDOW_MAX_X) {
				enemy[i].x = (WINDOW_X - 9000);
			}
		}

		// 2000カウントを超えると敵の動きが速くなる
		if (stageCount == 2000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 4000カウントを超えると敵の動きがさらに速くなる
		if (stageCount == 4000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 6000カウントを超えるとさらにさらに速くなる
		if (stageCount == 6000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 8000カウントを超えると（ｒｙ
		if (stageCount == 8000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}

		// 10000カウントを（ｒｙ
		if (stageCount == 10000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}

		// 海の画像を少しずつ右へ
		if (stageCount % 5 == 0) sea.x++;

		// 敵に当たったらゲームオーバー
		for (int i = 0; i < ENEMY_MAX; i++) {
			int ax1 = player.x + 12;
			int ay1 = player.y + 12;
			int ax2 = player.x + 64 - 12;
			int ay2 = player.y + 64 - 12;
			int bx1 = enemy[i].x + 8;
			int by1 = enemy[i].y + 8;
			int bx2 = enemy[i].x + 16 - 8;
			int by2 = enemy[i].y + 16 - 8;

			if ((ax1 < bx2) && (bx1 < ax2) && (ay1 < by2) && (by1 < ay2)) {
				// 音を鳴らす
				if (!(CheckSoundMem(missSE))) {
					PlaySoundMem(missSE, DX_PLAYTYPE_BACK);
				}
				gameState = GAMEOVER;
			}
		}

		// 下に落ちてもゲームオーバー
		if (player.y > WINDOW_MAX_Y) {
			// 音を鳴らす
			if (!(CheckSoundMem(missSE))) {
				PlaySoundMem(missSE, DX_PLAYTYPE_BACK);
			}
			gameState = GAMEOVER;
		}

		// ズドーン
		if (countUpperSide > 230 || countLowerSide > 230) {
			// 音を鳴らす
			if (!(CheckSoundMem(thunderSE))) {
				PlaySoundMem(thunderSE, DX_PLAYTYPE_BACK);
			}
		}
		// 雷に打たれたのでゲームオーバー
		if (countUpperSide > 250 || countLowerSide > 250) {
			gameState = GAMEOVER;
		}

		// 時を進める
		stageCount++;

		// 上のほうにいたらカウントする
		if (player.y < 200 - 64) countUpperSide++;
		else countUpperSide = 0;

		// 下のほうにいたらカウントする
		if (player.y > 340) countLowerSide++;
		else countLowerSide = 0;

		// スコアを更新
		score = stageCount / 20 * 10;

		// ハイスコアを更新したときに限り、ハイスコアの値も更新
		if (score > hiScore) hiScore = score;
	}

	// ------------------------------------------------------------------------
	// ゲームオーバー画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMEOVER) {
		// 流れている音楽を止める
		if (CheckSoundMem(stageMusic)) {
			StopSoundMem(stageMusic);
		}

		// ハイスコアをセーブする
		Write();

		// Xキーが押されたらタイトル画面へ
		if (key & PAD_INPUT_B) {
			gameState = GAMETITLE;
		}
	}
}

// ----------------------------------------------------------------------------
// 描画処理
// ----------------------------------------------------------------------------
void Draw()
{
	// ------------------------------------------------------------------------
	// タイトル画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMETITLE) {
		// タイトル画面画像の描画
		DrawGraph(WINDOW_X, WINDOW_Y, titleScreen, TRUE);
	}

	// ------------------------------------------------------------------------
	// ゲームメイン画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMEMAIN) {
		// 自機の描画
		DrawGraph(player.x, player.y, player.image, TRUE);

#ifdef _DEBUG
		// 自機の当たり判定の描画
		DrawBox(player.x + 12, player.y + 12,
			player.x + 64 - 12, player.y + 64 - 12,
			GetColor(255, 255, 255), FALSE);
#endif

		// 海の描画
		for (int i = 0; i < 1000; i++) {
			DrawGraph(sea.x - i * 16, sea.y, sea.image, TRUE);
		}

		// 敵の描画
		for (int i = 0; i < ENEMY_MAX; i++) {
			// 画面内にいるものだけ描画
			if (enemy[i].x > WINDOW_X - 100 && enemy[i].x < WINDOW_MAX_X) {
				DrawGraph(enemy[i].x, enemy[i].y, enemy[i].image, TRUE);
			}
		}

		// 上部に長くいると警告
		if (countUpperSide > 150 && countUpperSide < 250) {
			DrawString(WINDOW_MAX_X / 2, WINDOW_Y + 30, "CAUTION...",
				GetColor(255, 255, 255));
		}
		// 下部に長くいると警告
		if (countLowerSide > 150 && countLowerSide < 250) {
			DrawString(WINDOW_MAX_X / 2, WINDOW_MAX_Y - 30, "CAUTION...",
				GetColor(255, 255, 255));
		}

		// 上部・下部に長くいると雷攻撃の前触れ
		if ((countUpperSide > 200 && countUpperSide < 210) ||
			(countLowerSide > 200 && countLowerSide < 210)) {
				DrawGraph(player.x + 12, WINDOW_Y, thunder.image, TRUE);
		}
		// ズドーン
		if (countUpperSide > 230 || countLowerSide > 230) {
			// 画像表示
			DrawGraph(player.x + 12, WINDOW_Y, thunder.image, TRUE);
		}

#ifdef _DEBUG
		// 経過時間の描画
		DrawFormatString(WINDOW_X, WINDOW_Y + 20, GetColor(255, 255, 255),
			"%d", stageCount);

		// 上部カウント
		DrawFormatString(WINDOW_X, WINDOW_Y + 40, GetColor(255, 255, 255),
			"Upper: %d", countUpperSide);

		// 下部カウント
		DrawFormatString(WINDOW_X, WINDOW_Y + 64, GetColor(255, 255, 255),
			"Lower: %d", countLowerSide);
#endif

		// スコアの描画
		DrawFormatString(WINDOW_X + 50, WINDOW_Y, GetColor(255, 255, 255),
			"SCORE: %d", score);

		// ハイスコアの描画
		DrawFormatString(WINDOW_X + 250, WINDOW_Y, GetColor(255, 255, 255),
			"HISCORE: %d", hiScore);
	}

	// ------------------------------------------------------------------------
	// ゲームオーバー画面の処理
	// ------------------------------------------------------------------------
	if (gameState == GAMEOVER) {
		// ゲームオーバー画面画像の描画
		DrawGraph(WINDOW_X, WINDOW_Y, missScreen, TRUE);
	}

	// 現在のゲームモードを表示する (デバッグ用)
#ifdef _DEBUG
	DrawFormatString(WINDOW_X, WINDOW_Y, GetColor(255, 255, 255),
		"%d", gameState);
#endif
}

// ----------------------------------------------------------------------------
// ループ内で行う処理
// ----------------------------------------------------------------------------
void DoMainLoop()
{
	// 移動処理・変数の更新処理
	Update();

	// 描画処理
	Draw();
}

// ----------------------------------------------------------------------------
// ファイル保存処理
// ----------------------------------------------------------------------------
void Write()
{
	FILE* fp;

	fp = fopen("score.dat", "wb");
	fwrite(&hiScore, sizeof(hiScore), 1, fp);
	fclose(fp);
}

// ----------------------------------------------------------------------------
// ファイル読み込み処理
// ----------------------------------------------------------------------------
int Read()
{
	FILE* fp;
	int input;

	fp = fopen("score.dat", "rb");
	fread(&input, sizeof(input), 1, fp);
	fclose(fp);

	return input;
}
