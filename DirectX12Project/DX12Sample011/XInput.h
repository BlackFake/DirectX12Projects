// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __XINPUT_H__
#define __XINPUT_H__
// -------------------- IncludeFile ------------------
#include "Global.h"
#include <XInput.h>
// -------------------- LibraryFile ------------------
#pragma comment (lib, "xinput.lib")
// -------------------- Define�錾 -------------------
#define BUFFER_SIZE				256			// �o�b�t�@�T�C�Y
#define XINPUT_MAX_CONTROLLERS  4			//XInput���F���ł���̂�4�܂�
#define XINPUT_THRESHOLD		6000		//�������l
#define XINPUT_MAX_SYNCBUTTON	3			// �����ɓ��͂ł���{�^���̍ő吔
// -------------------- �񋓑̐錾 -------------------
enum eXIJoystickType
{
	eXIJoystick_First,	// ��Ԗڂ̃R���g���[���[
	eXIJoystick_Second,	// ��Ԗڂ̃R���g���[���[
	eXIJoystick_Third,	// �O�Ԗڂ̃R���g���[���[
	eXIJoystick_Fourth	// �l�Ԗڂ̃R���g���[���[
};
// -------------------- �N���X�錾 -------------------
class XINPUTJOYSTATE
{
public:
	XINPUT_STATE State;
	bool ConnectFlag;
	WORD SyncButtons[XINPUT_MAX_SYNCBUTTON];
public:
	XINPUTJOYSTATE()
	{
		State = XINPUT_STATE();
		ConnectFlag = false;
		memset(SyncButtons, 0x00, sizeof(SyncButtons));
	}
};
// --------------------- �֐��錾 --------------------
void GetXInputJoystate(XINPUTJOYSTATE*, DWORD);	// XInputJoystick�̍X�V
bool XInputConnectCheck(XINPUTJOYSTATE*, DWORD);// XInputJoystick���ڑ�����Ă��邩
#endif /* __XINPUT_H__ */