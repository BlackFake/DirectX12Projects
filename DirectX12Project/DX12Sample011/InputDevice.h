// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __INPUTDEVICE_H__
#define __INPUTDEVICE_H__
// --------------- IncludeFile ---------------
#include "XInput.h"
// ---------------- Define�錾 ---------------
// �L�[�{�[�h�}�N��
#define VK_LALT VK_LMENU
#define VK_RALT VK_RMENU
// ---------------- �N���X�錾 ---------------
class CInputDevice 
{
private:
	static CInputDevice*	m_pInstance;							// �C���X�^���X
	BYTE					m_Keyboard[BUFFER_SIZE];				// �L�[�{�[�h���
	XINPUTJOYSTATE			m_XIJoystick[XINPUT_MAX_CONTROLLERS];	// XInput�W���C�X�e�B�b�N���
public:
	CInputDevice();
	~CInputDevice();
	static void Create();				// ����
	static void Destroy();				// �j��
	static CInputDevice* GetInstance();	// �C���X�^���X�̎擾
	void DeviceUpDate();				// ���̓f�o�C�X�̍X�V
	LPBYTE GetKeyboard();				// �L�[�{�[�h���̎擾
	XINPUTJOYSTATE* GetXIJoystick();	// XInput�W���C�X�e�B�b�N���̎擾
};
#endif	/* __INPUTDEVICE_H__ */