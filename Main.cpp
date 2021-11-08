// ============================================================================
//
// ���C�� (���̃t�@�C�����͂��߂ɓǂݍ��܂��B����͂��̃t�@�C�������Ȃ��B)
//
// ----------------------------------------------------------------------------
//
// �� 2010�N06��30�� (PSGC)
//		�E�n�C�X�R�A�������ł���悤�ɂ���
//
// �� 2010�N06��28�� (PSGC)
//		�E�t���X�N���[�����E�B���h�E���I�ׂ�悤�ɂ���
//		�E�G���C���O�ʂɌ�����悤�ɂ���
//		�E�G�̐������炵�ē�Փx�𗎂Ƃ���
//		�E�G�������Ȃ�p�x���グ���������Ȃ�Ԋu��������
//		�E�G���h���X�ɂȂ�����
//		�E�Z�[�u�t�@�C�����e�L�X�g�t�@�C������o�C�i���t�@�C���ɂ���
//
// �� 2010�N06��25�� (PSGC)
//		�E�{���̃R�[�f�B���O(��)
//
// �� 2010�N06��21�� (PSGC)
//		�E�葱���𕪂��邽�߂ɏ������֐��ɕ�����
//
// �� 2010�N06��20�� (PSGC)
//		�E�V�K�쐬 / �Ƃ肠�������C���֐��Ə������֘A�̂�
//
// ============================================================================

// ----------------------------------------------------------------------------
// ���C�u�����̃C���N���[�h
// ----------------------------------------------------------------------------

#include "DxLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ----------------------------------------------------------------------------
// �萔�̐錾
// ----------------------------------------------------------------------------

#define WINDOW_X 0			// ��ʂ̏�[
#define WINDOW_Y 0			// ��ʂ̍��[
#define WINDOW_MAX_X 640	// ��ʂ̉E�[
#define WINDOW_MAX_Y 480	// ��ʂ̉��[
#define COLOR_BIT 32		// ��ʂ̐F�[�x

#define ENEMY_MAX 600		// �G�̍ő吔
// �Q�[�����[�h
enum {
	GAMETITLE, GAMEMAIN, GAMEOVER
};

// ----------------------------------------------------------------------------
// �O���[�o���ϐ��̐錾
// ----------------------------------------------------------------------------

// �O���t�B�b�N�n���h��
int titleScreen, missScreen;

// BGM�n���h��
int stageMusic;

// SE�n���h��
int floatingSE, thunderSE, missSE;

// �ړ����̂̍\����
struct MOVER {
	int x, y;				// ���W
	double speedx, speedy;	// ���x
	int image;				// �摜�n���h��
};

typedef MOVER PLAYER;		// ���@�̍\����
typedef MOVER ENEMY;		// �G�̍\����
typedef MOVER THUNDER;		// ���̍\����
typedef MOVER SEA;			// �C�̍\����

PLAYER player;
ENEMY enemy[ENEMY_MAX];
THUNDER thunder;
SEA sea;

int gameState;				// �Q�[�����[�h���i�[����ϐ�
int stageCount;				// �o�ߎ��Ԃ��i�[����ϐ�
int score;					// ����̃X�R�A
int hiScore;				// �ō��L�^

int countUpperSide;			// ��ʏ㕔�ɂ�������
int countLowerSide;			// ��ʉ����ɂ�������

// ----------------------------------------------------------------------------
// �֐��̃v���g�^�C�v�錾
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
// ���C���֐�
// ----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	// �����̎�
	srand((unsigned)time(NULL));

	// DX���C�u�����������O�̏���
	InitDxLibBefore();

	// DX���C�u��������������
	if (DxLib_Init() == -1) return -1;

	// DX���C�u������������̏���
	InitDxLibAfter();

	// �e��t�@�C���̓ǂݍ���
	LoadFiles();

	// �ϐ��Ȃǂ̏�����
	Initialize();

	// ���C�����[�v
	while (1) {
		// �L�����o�X���܂�����ɂ���
		ClearDrawScreen();

		// �`�揈��
		DoMainLoop();

		// ����ʂ̓��e�𔽉f
		ScreenFlip();

		// ���b�Z�[�W����������Ă��炤
		if (ProcessMessage() < 0) break;

		// ESC�L�[�ŏI��
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
	}

	// DX���C�u�����I������
	DxLib_End();

	// �A�v���P�[�V�����̏I������
	return 0;
}

// ----------------------------------------------------------------------------
// DX���C�u�����������O�̏���
// ----------------------------------------------------------------------------
void InitDxLibBefore()
{
	// �E�B���h�E�̃^�C�g����ݒ�
#ifdef _DEBUG
	SetWindowText("DANGEROUS TRIP - DEBUG MODE");
#else
	SetWindowText("DANGEROUS TRIP");
#endif

	// ���O�o�̗͂L��(TRUE: ����, FALSE: ���Ȃ�)
	SetOutApplicationLogValidFlag(FALSE);

	// �A�[�J�C�u�@�\���g��
	SetUseDXArchiveFlag(TRUE);

	// �A�[�J�C�u�̊g���q
	SetDXArchiveExtension("arc");

	// �t���X�N���[�� or �E�B���h�E
	int isFullScreen = MessageBox(NULL, TEXT("�t���X�N���[���ŋN�����܂���"),
		TEXT("Dangerous Trip"), MB_YESNO | MB_ICONQUESTION);

	if (isFullScreen == IDYES) {
		ChangeWindowMode(FALSE);
	}
	else {
		ChangeWindowMode(TRUE);
	}

	// ��ʃT�C�Y�Ȃǂ�ݒ�
	SetGraphMode(WINDOW_MAX_X, WINDOW_MAX_Y, COLOR_BIT);
}

// ----------------------------------------------------------------------------
// DX���C�u������������̏���
// ----------------------------------------------------------------------------
void InitDxLibAfter()
{
	// ���ߐF�̕ύX
	SetTransColor(255, 0, 255);

	// ����ʏ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �N������̓^�C�g����ʂ�
	gameState = GAMETITLE;
}

// ----------------------------------------------------------------------------
// �e��t�@�C���̓ǂݍ���
// ----------------------------------------------------------------------------
void LoadFiles()
{
	// BGM�̃��[�h
	stageMusic = LoadSoundMem(".\\sound\\stage.ogg");

	// SE�̃��[�h
	floatingSE = LoadSoundMem(".\\sound\\floating.wav");
	thunderSE = LoadSoundMem(".\\sound\\thunder.wav");
	missSE = LoadSoundMem(".\\sound\\miss.wav");

	// �O���t�B�b�N�̃��[�h
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
// �ϐ��Ȃǂ̏�����
// ----------------------------------------------------------------------------
void Initialize()
{
	// ���@�̏������W
	player.x = (int)(WINDOW_MAX_X * 0.8);
	player.y = (int)(WINDOW_MAX_Y * 0.4);

	// ���@�̏������x
	player.speedx = 0;
	player.speedy = 0;

	// �G�̏������W
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].x = (rand() % 10000) - 10000;
		enemy[i].y = (rand() % 1000);
	}

	// �G�̏������x
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].speedx = 1;
	}

	// �C�̏������W
	sea.x = WINDOW_MAX_X - 16;
	sea.y = WINDOW_MAX_Y - 32;

	// �J�n����̃X�R�A��0�_
	score = 0;

	// �n�C�X�R�A��ǂݍ���
	hiScore = Read();

	// �J�n����̌o�ߎ��Ԃ�0
	stageCount = 0;

	// �J�n����͏�ɂ����ɂ����Ȃ�
	countUpperSide = 0;
	countLowerSide = 0;
}

// ----------------------------------------------------------------------------
// �ړ������E�ϐ��̍X�V����
// ----------------------------------------------------------------------------
void Update()
{
	int key = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// ------------------------------------------------------------------------
	// �^�C�g����ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMETITLE) {
		// ����ʂ���J�ڂ��Ă����Ƃ��̂��߂ɂ�����x������
		Initialize();

		// C�L�[�������ꂽ��n�C�X�R�A������
		if (key & PAD_INPUT_C) {
			hiScore = 0;
			Write();
		}

		// Z�L�[�������ꂽ��Q�[�����C����ʂ�
		if (key & PAD_INPUT_A) {
			gameState = GAMEMAIN;
		}
	}

	// ------------------------------------------------------------------------
	// �Q�[�����C����ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMEMAIN) {
		// ���y�����Ă��Ȃ�������炷
		if (!(CheckSoundMem(stageMusic))) {
			PlaySoundMem(stageMusic, DX_PLAYTYPE_LOOP);
		}

		// �J�[�\���L�[�ō��E�Ɉړ�
		if (key & PAD_INPUT_LEFT) player.speedx -= 0.2;
		if (key & PAD_INPUT_RIGHT) player.speedx += 0.2;

		// �����������h�~
		if (player.speedx < -6) player.speedx = -6;
		if (player.speedx > 6) player.speedx = 6;

		// �Ō�ɑ��x�𑫂��Ĉړ�����
		player.x += (int)player.speedx;

		// Z�L�[�ŏ㏸
		if (key & PAD_INPUT_A) {
			player.speedy -= 0.2;

			// �������Ă��Ȃ������特��炷
			if (!(CheckSoundMem(floatingSE))) {
				PlaySoundMem(floatingSE, DX_PLAYTYPE_BACK);
			}
		}
		else {
			player.speedy += 0.2;
		}

		// �c�����ɉ����������h�~
		if (player.speedy < -6) player.speedy = -6;
		if (player.speedy > 6) player.speedy = 6;

		// �Ō�ɑ��x�𑫂��Ĉړ�����
		player.y += (int)player.speedy;

		// ��ʊO�ɏo�Ȃ��悤��
		if (player.x < WINDOW_X) player.x = WINDOW_X;
		if (player.x > WINDOW_MAX_X - 64) player.x = WINDOW_MAX_X - 64;
		if (player.y < WINDOW_Y) player.y = WINDOW_Y;

		// �G�𓮂���
		for (int i = 0; i < ENEMY_MAX; i++) {
			enemy[i].x += (int)enemy[i].speedx;
			// �G���߂���������ʒu�����ɖ߂��B����y�G���h���X
			if (enemy[i].x > WINDOW_MAX_X) {
				enemy[i].x = (WINDOW_X - 9000);
			}
		}

		// 2000�J�E���g�𒴂���ƓG�̓����������Ȃ�
		if (stageCount == 2000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 4000�J�E���g�𒴂���ƓG�̓���������ɑ����Ȃ�
		if (stageCount == 4000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 6000�J�E���g�𒴂���Ƃ���ɂ���ɑ����Ȃ�
		if (stageCount == 6000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}
		// 8000�J�E���g�𒴂���Ɓi����
		if (stageCount == 8000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}

		// 10000�J�E���g���i����
		if (stageCount == 10000) {
			for (int i = 0; i < ENEMY_MAX; i++) {
				enemy[i].speedx++;
			}
		}

		// �C�̉摜���������E��
		if (stageCount % 5 == 0) sea.x++;

		// �G�ɓ���������Q�[���I�[�o�[
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
				// ����炷
				if (!(CheckSoundMem(missSE))) {
					PlaySoundMem(missSE, DX_PLAYTYPE_BACK);
				}
				gameState = GAMEOVER;
			}
		}

		// ���ɗ����Ă��Q�[���I�[�o�[
		if (player.y > WINDOW_MAX_Y) {
			// ����炷
			if (!(CheckSoundMem(missSE))) {
				PlaySoundMem(missSE, DX_PLAYTYPE_BACK);
			}
			gameState = GAMEOVER;
		}

		// �Y�h�[��
		if (countUpperSide > 230 || countLowerSide > 230) {
			// ����炷
			if (!(CheckSoundMem(thunderSE))) {
				PlaySoundMem(thunderSE, DX_PLAYTYPE_BACK);
			}
		}
		// ���ɑł��ꂽ�̂ŃQ�[���I�[�o�[
		if (countUpperSide > 250 || countLowerSide > 250) {
			gameState = GAMEOVER;
		}

		// ����i�߂�
		stageCount++;

		// ��̂ق��ɂ�����J�E���g����
		if (player.y < 200 - 64) countUpperSide++;
		else countUpperSide = 0;

		// ���̂ق��ɂ�����J�E���g����
		if (player.y > 340) countLowerSide++;
		else countLowerSide = 0;

		// �X�R�A���X�V
		score = stageCount / 20 * 10;

		// �n�C�X�R�A���X�V�����Ƃ��Ɍ���A�n�C�X�R�A�̒l���X�V
		if (score > hiScore) hiScore = score;
	}

	// ------------------------------------------------------------------------
	// �Q�[���I�[�o�[��ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMEOVER) {
		// ����Ă��鉹�y���~�߂�
		if (CheckSoundMem(stageMusic)) {
			StopSoundMem(stageMusic);
		}

		// �n�C�X�R�A���Z�[�u����
		Write();

		// X�L�[�������ꂽ��^�C�g����ʂ�
		if (key & PAD_INPUT_B) {
			gameState = GAMETITLE;
		}
	}
}

// ----------------------------------------------------------------------------
// �`�揈��
// ----------------------------------------------------------------------------
void Draw()
{
	// ------------------------------------------------------------------------
	// �^�C�g����ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMETITLE) {
		// �^�C�g����ʉ摜�̕`��
		DrawGraph(WINDOW_X, WINDOW_Y, titleScreen, TRUE);
	}

	// ------------------------------------------------------------------------
	// �Q�[�����C����ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMEMAIN) {
		// ���@�̕`��
		DrawGraph(player.x, player.y, player.image, TRUE);

#ifdef _DEBUG
		// ���@�̓����蔻��̕`��
		DrawBox(player.x + 12, player.y + 12,
			player.x + 64 - 12, player.y + 64 - 12,
			GetColor(255, 255, 255), FALSE);
#endif

		// �C�̕`��
		for (int i = 0; i < 1000; i++) {
			DrawGraph(sea.x - i * 16, sea.y, sea.image, TRUE);
		}

		// �G�̕`��
		for (int i = 0; i < ENEMY_MAX; i++) {
			// ��ʓ��ɂ�����̂����`��
			if (enemy[i].x > WINDOW_X - 100 && enemy[i].x < WINDOW_MAX_X) {
				DrawGraph(enemy[i].x, enemy[i].y, enemy[i].image, TRUE);
			}
		}

		// �㕔�ɒ�������ƌx��
		if (countUpperSide > 150 && countUpperSide < 250) {
			DrawString(WINDOW_MAX_X / 2, WINDOW_Y + 30, "CAUTION...",
				GetColor(255, 255, 255));
		}
		// �����ɒ�������ƌx��
		if (countLowerSide > 150 && countLowerSide < 250) {
			DrawString(WINDOW_MAX_X / 2, WINDOW_MAX_Y - 30, "CAUTION...",
				GetColor(255, 255, 255));
		}

		// �㕔�E�����ɒ�������Ɨ��U���̑O�G��
		if ((countUpperSide > 200 && countUpperSide < 210) ||
			(countLowerSide > 200 && countLowerSide < 210)) {
				DrawGraph(player.x + 12, WINDOW_Y, thunder.image, TRUE);
		}
		// �Y�h�[��
		if (countUpperSide > 230 || countLowerSide > 230) {
			// �摜�\��
			DrawGraph(player.x + 12, WINDOW_Y, thunder.image, TRUE);
		}

#ifdef _DEBUG
		// �o�ߎ��Ԃ̕`��
		DrawFormatString(WINDOW_X, WINDOW_Y + 20, GetColor(255, 255, 255),
			"%d", stageCount);

		// �㕔�J�E���g
		DrawFormatString(WINDOW_X, WINDOW_Y + 40, GetColor(255, 255, 255),
			"Upper: %d", countUpperSide);

		// �����J�E���g
		DrawFormatString(WINDOW_X, WINDOW_Y + 64, GetColor(255, 255, 255),
			"Lower: %d", countLowerSide);
#endif

		// �X�R�A�̕`��
		DrawFormatString(WINDOW_X + 50, WINDOW_Y, GetColor(255, 255, 255),
			"SCORE: %d", score);

		// �n�C�X�R�A�̕`��
		DrawFormatString(WINDOW_X + 250, WINDOW_Y, GetColor(255, 255, 255),
			"HISCORE: %d", hiScore);
	}

	// ------------------------------------------------------------------------
	// �Q�[���I�[�o�[��ʂ̏���
	// ------------------------------------------------------------------------
	if (gameState == GAMEOVER) {
		// �Q�[���I�[�o�[��ʉ摜�̕`��
		DrawGraph(WINDOW_X, WINDOW_Y, missScreen, TRUE);
	}

	// ���݂̃Q�[�����[�h��\������ (�f�o�b�O�p)
#ifdef _DEBUG
	DrawFormatString(WINDOW_X, WINDOW_Y, GetColor(255, 255, 255),
		"%d", gameState);
#endif
}

// ----------------------------------------------------------------------------
// ���[�v���ōs������
// ----------------------------------------------------------------------------
void DoMainLoop()
{
	// �ړ������E�ϐ��̍X�V����
	Update();

	// �`�揈��
	Draw();
}

// ----------------------------------------------------------------------------
// �t�@�C���ۑ�����
// ----------------------------------------------------------------------------
void Write()
{
	FILE* fp;

	fp = fopen("score.dat", "wb");
	fwrite(&hiScore, sizeof(hiScore), 1, fp);
	fclose(fp);
}

// ----------------------------------------------------------------------------
// �t�@�C���ǂݍ��ݏ���
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
