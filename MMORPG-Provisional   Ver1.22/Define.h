#pragma once

//------------------------------------------------------------------------------------------------�Q�[�����[�h-----------------------------------------------------------------------------------------------

//#define SET_DEBAG_FLAG 0
#define SET_TIMER_FLAG 0
#define SET_STAGE_COM_FLAG 1


//-------------------------------------------------------------------------------------------------�J�����֌W------------------------------------------------------------------------------------------------

//�J�����̎�O�N���b�v����
#define CAMERA_NEAR_CLIP 50.0f
//�J�����̉��N���b�v����
#define CAMERA_FAR_CLIP 5000.0f
//�}�E�X�z�C�[���𓮂������Ƃ��̃J�����ړ���
#define WHEEL_SPEED 60.0f
//�}�E�X�z�C�[�������l
#define WHEEL_INIT_NUM -1.0f
//�J�����Y�[���J�E���g�����l
#define WHEEL_CNT_INIT_NUM -0.5f
//�z�C�[���̏����J�E���g�l
#define WHEEL_MOVE_CNT 0.05f
//�Y�[���ő�l
#define WHEEL_MAX -0.5f
//�Y�[���ŏ��l
#define WHEEL_MIN -3.0f
//�J�����A���O���̈ړ���
#define CAMERA_ANGLE_SPEED 0.005f
//�����_�܂ł̍ő勗��
#define DISTANCE_MAX 1000.0f
//�����_�܂ł̍ŏ�����
#define DISTANCE_MIN 170.0f
//�}�E�X�ړ��������W ����X���W		//
#define MOUSE_AREA_X1 50			//�S�ė]�T��������
//�}�E�X�ړ��������W �E��X���W		//
#define MOUSE_AREA_X2 1870			//�t���X�N���[�����
//�}�E�X�ړ��������W ����Y���W		//
#define MOUSE_AREA_Y1 50			//+-50�̒l�ɂ��Ă���B
//�}�E�X�ړ��������W �E��Y���W		//
#define MOUSE_AREA_Y2 1030			//
//-----------------------------------------------------------------------------------------------���W�֌W-------------------------------------------------------------------------------------------------

//�`�揉�����W
//�������W ����X���W
#define INIT_AREA_X1 0
//�������W �E��X���W
#define INIT_AREA_X2 1920
//�������W ����Y���W
#define INIT_AREA_Y1 0
//�������W �E��Y���W
#define INIT_AREA_Y2 1080

//�^�C�g�����
//�^�C�g����ʔw�i�\������X���W
#define TITLE_BACKIMAGE_POS_X -40
//�^�C�g����ʔw�i�\������Y���W
#define TITLE_BACKIMAGE_POS_Y -120
//�^�C�g���\������X���W
#define TITLE_POS_X 360
//�^�C�g���\������Y���W
#define TITLE_POS_Y 200
//��]����}�[�N�\������X���W
#define MARK_POS_X 960
//��]����}�[�N�\������Y���W
#define MARK_POS_Y 500
//LOGIN�\������X���W
#define LOGIN_POS_X 760
//LOGIN�\������Y���W
#define LOGIN_POS_Y 640
//KEYCONFIG�\������X���W
#define KEYCONFIG_POS_X 810
//KEYCONFIG�\������Y���W
#define	KEYCONFIG_POS_Y 800
//EXIT�\������X���W
#define EXIT_POS_X 910
//EXIT�\������Y���W
#define	EXIT_POS_Y 900
//LOGIN�I���J�[�\���\��X���W
#define CORSOL_POINT_X1 LOGIN_POS_X-75		//�J�[�\���̏ꏊ�ݒ�
//LOGIN�I���J�[�\���\��Y���W				//
#define CORSOL_POINT_Y1 LOGIN_POS_Y+25		//-75�𕶎����W��������ċ��߂�
//KEYCONFIG�I���J�[�\���\��X���W			//
#define CORSOL_POINT_X2 KEYCONFIG_POS_X-75	//
//KEYCONFIG�I���J�[�\���\��Y���W			//
#define CORSOL_POINT_Y2 KEYCONFIG_POS_Y		//+25����������LOGIN�̕����̒��S��
//EXIT�I���J�[�\���\��X���W					//
#define CORSOL_POINT_X3 EXIT_POS_X-75		//Y���W���������炷
//EXIT�I���J�[�\���\��Y���W					//
#define CORSOL_POINT_Y3 EXIT_POS_Y			//


////�Q�[�����C�����
//���j���[UI�n�܂��X���W
#define B_MENU_AXIS_X 1300
//���j���[UI�n�܂��Y���W
#define B_MENU_AXIS_Y 1050



//-----------------------------------------------------------------------------------------------�Q�[�����C�� ----------------------------------------------------------------------------------------------

#define KEY_REFRESH_COUNT 2//�ړ��L�[�����������ꍇ�̓����X�s�[�h


//-----------------------------------------------------------------------------------------------�X�e�[�W�֌W-------------------------------------------------------------------------------------------------

//�I�u�W�F�N�g�̐�
#define OBJECT_NUM 20

//�V���h�E�}�b�v�`��ʒu
#define SHADOWMAP_INIT1 VGet(-1000.0f,-1.0f,-1000.0f)
#define SHADOWMAP_INIT2 VGet(1000.0f,1000.0f,1000.0f)

//�X�e�[�W�`��ʒu
#define STAGE_INIT VGet(600.0f,255.0f,-400.0f)

//---------------------------------------------------------------------------------------------�}�b�v�`�b�v�֌W---------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------���̑�--------------------------------------------------------------------------------------------------
