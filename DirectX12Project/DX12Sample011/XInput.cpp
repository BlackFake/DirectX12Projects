// -------------------- IncludeFile ------------------
#include "XInput.h"

//	�֐���:GetXInputJoystate		XInputJoystick�̍X�V
//	������:pXIJoystick				XINPUTJOYSTATE�̃|�C���^
//		  :UserIndex				�R���g���[���[�ԍ�(0�`3)
//	�߂�l:							�Ȃ�
void GetXInputJoystate(XINPUTJOYSTATE* pXIJoystick, DWORD UserIndex)
{
	DWORD dwResult;
	dwResult = XInputGetState(UserIndex, &pXIJoystick->State);

	if (dwResult == ERROR_SUCCESS)
		pXIJoystick->ConnectFlag = true;
	else
		pXIJoystick->ConnectFlag = false;
}

//	�֐���:XInputConnectCheck		XInputJoystick���ڑ�����Ă��邩
//	������:pXIJoystick				XINPUTJOYSTATE�̃|�C���^
//		  :UserIndex				�R���g���[���[�ԍ�(0�`3)
//	�߂�l:							TRUE: �ڑ�����Ă���AFALSE: �ڑ�����Ă��Ȃ�
bool XInputConnectCheck(XINPUTJOYSTATE* pXIJoystick, DWORD UserIndex)
{
	DWORD dwResult;
	dwResult = XInputGetState(UserIndex, &pXIJoystick->State);
	if (dwResult == ERROR_SUCCESS)
		return true;
	else
		return false;
}