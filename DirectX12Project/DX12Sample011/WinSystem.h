// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __WINSYSTEM_H__
#define __WINSYSTEM_H__
// --------------- IncludeFile ---------------
#include "Application.h"
// ---------------- Define�錾 ---------------

// -------------- �O���[�o���ϐ� -------------
bool g_DebugFlag = false;			// �f�o�b�O���[�h�̗L��
SIZE g_WindowSize = { 800, 600 };	// �E�B���h�E�T�C�Y
LPCWSTR g_WindowTitle = L"";		// �A�v���P�[�V������
const std::wstring GetConfigString(const wchar_t* pFilename, const wchar_t* pSectionname, const wchar_t* pKeyname);
#endif /* __WINSYSTEM_H__ */