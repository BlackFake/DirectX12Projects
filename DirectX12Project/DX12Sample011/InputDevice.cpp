// --------------- IncludeFile ---------------
#include "InputDevice.h"
CInputDevice* CInputDevice::m_pInstance = nullptr;

// �R���X�g���N�^
CInputDevice::CInputDevice()
{
	ZeroMemory(m_Keyboard, sizeof(m_Keyboard));
	ZeroMemory(m_XIJoystick, sizeof(m_XIJoystick));
}

// �f�X�g���N�^
CInputDevice::~CInputDevice()
{

}

// ����
void CInputDevice::Create()
{
	if (nullptr == m_pInstance)
		m_pInstance = new CInputDevice();
}

// �j��
void CInputDevice::Destroy()
{
	SAFE_DELETE( m_pInstance );
}

// ���̓f�o�C�X�̍X�V
void CInputDevice::DeviceUpDate()
{
	if (!GetKeyboardState(m_Keyboard))
		return;

	for (DWORD i = XINPUT_MAX_CONTROLLERS; i-- > 0;)
		GetXInputJoystate(&m_XIJoystick[i], i);
}

// �L�[�{�[�h���̎擾
LPBYTE CInputDevice::GetKeyboard()
{
	return m_Keyboard;
}

// XInputJoystick�̎擾
XINPUTJOYSTATE* CInputDevice::GetXIJoystick()
{
	return m_XIJoystick;
}

CInputDevice* CInputDevice::GetInstance()
{
	return m_pInstance;
}