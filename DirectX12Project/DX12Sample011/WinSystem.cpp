// -------------------- IncludeFile -----------------
#include "WinSystem.h"
// ------------------------------------------------------------------------
//�@�֐���	:WinMain				�A�v���P�[�V�����G���g���[�|�C���g	
//	����	:hInstance				�C���X�^���X�n���h��(Windows���Ŏ��s����Ă���v���O��������ӂɎ��ʂ���)	
//			:hPrevInstance			Windows95�ȍ~��NULL
//			:pCmdLine				�v���O�����ɓn���ꂽ�R�}���h���C���������i�[���ꂽ������̃|�C���^
//			:CmdShow				�E�B���h�E�̕`����@
//	�߂�l	:						Windows�ւ̏I���R�[�h		
// ------------------------------------------------------------------------
int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int CmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CApplication* pApp = CApplication::GetInstance();

	// config.ini����K�v�ȃf�[�^���擾���A�O���[�o���ϐ��ɐݒ肷��
	const wchar_t* pFilename = L"resource/config.ini";
	auto bDebug = GetConfigString(pFilename, L"System", L"bDebug");

	if (wcscmp(bDebug.c_str(), L"true") == STRCMP_SUCCESS)
		g_DebugFlag = true;
	
	auto sWindowTitle = GetConfigString(pFilename, L"System", L"sWindowTitle");

	g_WindowTitle = sWindowTitle.c_str();


	// �A�v���P�[�V�����̏�����
	BOOL Windowed = TRUE;	// ���[�h�I��(TRUE:�E�B���h�E�AFALSE:�t���X�N���[��)
	HWND hWnd = pApp->Initialize(hInstance, g_WindowTitle, g_WindowSize, Windowed);

	if (nullptr == hWnd)
		return 0;

	// �A�v���P�[�V�������s
	return pApp->Run(hWnd);
}

// ------------------------------------------------------------------------
//�@�֐���	:GetConfigString		Config�f�[�^�̕�����ǂݎ��֐�	
//	����	:pFilename				�t�@�C���p�X
//			:pSectionname			�Z�N�V������
//			:pKeyname				�L�[��
//	�߂�l	:						�擾����������		
// ------------------------------------------------------------------------
const std::wstring GetConfigString(const wchar_t* pFilename, const wchar_t* pSectionname, const wchar_t* pKeyname)
{
	if (pFilename == nullptr)
		return L"";

	wchar_t buf[MAX_PATH] = {};
	GetPrivateProfileStringW(pSectionname, pKeyname, L"", buf, MAX_PATH, pFilename);

	return std::wstring(buf);
}