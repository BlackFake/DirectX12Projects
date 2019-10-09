// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#pragma once
// -------------------- IncludeFile ------------------
#include "Global.h"
#include "DXDevice.h"
#include "TimerThread.h"
#include "InputDevice.h"
// -------------------- �N���X�錾 -------------------
class CApplication  
{
private:
	static CApplication		m_Instance;				// �C���X�^���X
	BOOL					m_bWindowActive;		// �ŏ����̗L��
	POINT					m_WindowShowPosition;	// �\�����W
	SIZE					m_WindowSize;			// �T�C�Y
	CDXDevice*				m_pDXDevice;			// �Q�[���f�o�C�X
private:
	CApplication();
	~CApplication();
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	HWND InitWindow(HINSTANCE, LPCWSTR, SIZE, BOOL);
	SIZE GetWindowSize(HWND);
	void WindowMoveToCenter(HWND);
	void SetClientSize(HWND, SIZE);
public:
	HWND Initialize(HINSTANCE, LPCWSTR, SIZE, BOOL);
	int	Run(HWND);
	SIZE GetClientSize(HWND);
	static CApplication* GetInstance();
};
#endif	/* __APPLICATION_H__ */