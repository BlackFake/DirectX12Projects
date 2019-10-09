// -------------------- IncludeFile -----------------
#include "TimerThread.h"
CTimerThread* CTimerThread::m_pInstance = nullptr;	// �C���X�^���X�̎���
// �R���X�g���N�^
CTimerThread::CTimerThread(HWND hWnd)
{
	m_hWnd = hWnd;
	m_hTimerThread = nullptr;
	m_LoopFlag = TRUE;
	m_TimerThreadSleep = FALSE;
	m_pDXDevice = nullptr;
}
// �f�X�g���N�^
CTimerThread::~CTimerThread()
{
	m_LoopFlag = FALSE;
	while (WaitForSingleObject(m_hTimerThread, 0) == WAIT_TIMEOUT)
		Sleep(1);
	CloseHandle( m_hTimerThread );
	m_hTimerThread = nullptr;
}
// ------------------------------------------------------------------------
//�@�֐���	:Start						�^�C�}�[�X���b�h�̍쐬
//	����	:pDXDevice					�f�o�C�X
//			:hWnd						�E�B���h�E�n���h��
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void CTimerThread::Start(CDXDevice* pDXDevice)
{
	m_pDXDevice = pDXDevice;

	// �X���b�h�J�n
	DWORD threadID;						// �X���b�h�pID
	m_hTimerThread = (HANDLE)_beginthreadex(
		NULL,							// �Z�L�����e�B����(�Ȃ�)
		0,								// ���b�h�����X�^�b�N�̃T�C�Y���A�o�C�g�P�ʂŎw��(�f�t�H���g)
		(unsigned(__stdcall*)(void*))&Run,// �X���b�h�̊J�n�A�h���X���w��
		(VOID *)this,					// �X���b�h�ւ̈����w��(�A�h���X)
		0,								// ����t���O���w��(�ݒ�Ȃ�)
		(unsigned int*)&threadID);		// �X���b�hID���i�[����Ă���|�C���^�w��
}
// ------------------------------------------------------------------------
//�@�֐���	:Run						�A�v���P�[�V�����̎��s
//	����	:pTimerThread				�^�C�}�[�X���b�h
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
unsigned __stdcall CTimerThread::Run(CTimerThread* pTimerThread)
{
	static DWORD OldTime, NowTime;		// ���ԕۑ��p
	DWORD Timer = 20;					// �Q�[���X�s�[�h��0.02�b�Ԋu�Ɏw��

	while (CTimerThread::GetInstance()->m_LoopFlag)
	{
		NowTime = timeGetTime();				// ���݂̎��Ԃ��擾
		if (NowTime - OldTime < Timer)			// ���݂̎��� - �ߋ��̎��� < 0.02�b�̏ꍇ
		{
			Sleep(Timer - (NowTime - OldTime));	// ���ԑ҂�
			NowTime = timeGetTime();			// ���݂̎��Ԃ��擾
		}
		OldTime = NowTime;						// ���݂̎��Ԃ��ߋ��̎��Ԃɒu��������

		LPBYTE pKeyboard = CInputDevice::GetInstance()->GetKeyboard();
		
		// �s���̍X�V
		if (!CTimerThread::GetInstance()->m_TimerThreadSleep)
		{
			// �\�����[�h�̐؂�ւ�
			if (((pKeyboard[VK_LALT] & 0x80) && (pKeyboard[VK_RETURN] & 0x80))
				|| ((pKeyboard[VK_RALT] & 0x80) && (pKeyboard[VK_RETURN] & 0x80)))
			{
				CTimerThread* p = CTimerThread::GetInstance();
				CTimerThread::GetInstance()->m_TimerThreadSleep = TRUE;
				PostMessage(p->m_hWnd, UM_CHANGEWINDOWMODE, 0L, 0L);
			}
			// ��ʂ̍X�V
			pTimerThread->m_pDXDevice->Render();	// �����_�����O����
		}
		Sleep(1);
	}

	_endthreadex( 0 );
	return 0;
}

// ------------------------------------------------------------------------
//�@�֐���	:SetTimerThreadSleep		�^�C�}�[�X���b�h�ҋ@�̎w��
//	����	:bTimerThreadSleep			�ҋ@�̗L��
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void CTimerThread::SetTimerThreadSleep(BOOL TimerThreadSleep)
{
	m_TimerThreadSleep = TimerThreadSleep;
}
// ------------------------------------------------------------------------
//�@�֐���	:GetTimerThreadSleep		�^�C�}�[�X���b�h�ҋ@�̎擾
//	����	:							�Ȃ�
//	�߂�l	:							�^�C�}�[�X���b�h�ҋ@�̗L��
// ------------------------------------------------------------------------
BOOL CTimerThread::GetTimerThreadSleep()
{
	return m_TimerThreadSleep;
}
// �쐬
void CTimerThread::Create(HWND hWnd)
{
	if (!m_pInstance)
		m_pInstance = new CTimerThread(hWnd);
}
// �j��
void CTimerThread::Destroy()
{
	SAFE_DELETE( m_pInstance );
}