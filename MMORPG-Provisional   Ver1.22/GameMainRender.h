#ifndef GAMEMAINRENDER_H
#define GAMEMAINRENDER_H

// �Q�[�����C���̕`�揈��������������
//     �߂�l : ����������ɏI���������ǂ���(true:����ɏI������  false:�G���[����������)
extern bool GameMainRender_Initialize(void);

// �Q�[�����C���̕`�揈���̌�n��������
extern void GameMainRender_Terminate(void);

// �X�e�[�W�J�n���̕`�揀�����s��
extern void GameMainRender_StageSetup(void);

// �Q�[�����C���̕`�揈�����s��
extern void GameMainRender_Render(void);

#endif