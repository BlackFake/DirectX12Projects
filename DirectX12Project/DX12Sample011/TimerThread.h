// Programing by N.Sonoda
#ifndef __TIMERTHREAD_H__
#define __TIMERTHREAD_H__
// -------------------- IncludeFile ------------------
#include <process.h>
#include "Global.h"
#include "DXDevice.h"
#include "InputDevice.h"
// -------------------- �N���X�錾 -------------------
class CTimerThread  
{
private:
	static CTimerThread*	m_pInstance;			// �V���O���g��
	HWND					m_hWnd;					// �E�B���h�E�n���h��
	HANDLE					m_hTimerThread;			// �n���h��
	BOOL					m_LoopFlag;				// ���[�v�̗L��
	BOOL					m_TimerThreadSleep;		// �^�C�}�[�X���b�h�ҋ@�̗L��
	CDXDevice*				m_pDXDevice;			// �f�o�C�X�N���X
private:
	static unsigned __stdcall Run(CTimerThread*);
public:
	CTimerThread(HWND);
	virtual ~CTimerThread();
	static CTimerThread* GetInstance()	{ return m_pInstance;	}
	static void Create(HWND);
	static void Destroy();
	void Start(CDXDevice*);
	void SetTimerThreadSleep(BOOL);
	BOOL GetTimerThreadSleep();
};
#endif	/* __TIMERTHREAD_H__ */