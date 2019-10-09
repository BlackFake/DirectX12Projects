// -------------------- IncludeFile -----------------
#include "Application.h"
CApplication CApplication::m_Instance;				// �C���X�^���X�̎���
// �R���X�g���N�^
CApplication::CApplication() : m_pDXDevice(nullptr)
{
	m_bWindowActive = FALSE;
	m_WindowSize = SIZE();
	m_WindowShowPosition.x = -1;
}
// �f�X�g���N�^
CApplication::~CApplication()
{

}
// ------------------------------------------------------------------------
//�@�֐���	:Initialize					�A�v���P�[�V�����̏�����
//	����	:hInstance					�C���X�^���X�n���h��
//			:WindowTitle				�E�B���h�E�^�C�g��
//			:WindowSize					�E�B���h�E�T�C�Y
//			:Windowed					TRUE:�E�B���h�E���[�h�AFALSE:�t���X�N���[�����[�h
//	�߂�l	:							�E�B���h�E�n���h��
// ------------------------------------------------------------------------
HWND CApplication::Initialize(HINSTANCE hInstance, LPCWSTR WindowTitle,	SIZE WindowSize, BOOL Windowed)
{
	HRESULT hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return nullptr;

	// COM�̏�����
	::CoInitialize(0);

	// �E�B���h�E�̏�����
	HWND hWnd = InitWindow(hInstance, WindowTitle, WindowSize, Windowed);
	if (nullptr == hWnd)
		return nullptr;

	// �Q�[���f�o�C�X�̏�����
	m_pDXDevice = new CDXDevice();
	hr = m_pDXDevice->Initialize(hWnd, WindowSize, Windowed);
	if (FAILED( hr ))
	{
		SAFE_DELETE( m_pDXDevice );
		return nullptr;
	}

	// ���̓f�o�C�X�̐���
	CInputDevice::Create();

	// �^�C�}�[�̕���x��1ms�ɐݒ�
	timeBeginPeriod(1);

	// �E�B���h�E�̕\��
	::ShowWindow(hWnd, SW_SHOW);

	return hWnd;
}
// ------------------------------------------------------------------------
//�@�֐���	:Run						�A�v���P�[�V�����̎��s
//	����	:hWnd						�E�B���h�E�n���h��
//	�߂�l	:							�A�v���P�[�V�����I���l
// ------------------------------------------------------------------------
int CApplication::Run(HWND hWnd)
{
	// �Q�[���X�s�[�h�Ǘ��X���b�h�쐬/���s
	CTimerThread::Create(hWnd);
	CTimerThread::GetInstance()->Start(m_pDXDevice);

	// ���b�Z�[�W���[�v
	MSG msg;
	do {
		// ���b�Z�[�W����
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);			// �L�[�{�[�h���b�Z�[�W�𕶎����b�Z�[�W�ɕϊ����ă��b�Z�[�W�L���[�Ƀ|�X�g
			DispatchMessage(&msg);			// �E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}

		CInputDevice::GetInstance()->DeviceUpDate();

		Sleep(1);
	} while (msg.message != WM_QUIT);

	// �Q�[���X�s�[�h�Ǘ��X���b�h�I��
	CTimerThread::Destroy();

	// ���\�[�X�̉��
	m_pDXDevice->CleanupResource();

	// ���̓f�o�C�X�̉��
	CInputDevice::Destroy();

	// �f�o�C�X�̉��
	SAFE_DELETE( m_pDXDevice );

	// �^�C�}�[�̕���x��1ms�ɐݒ�
	timeEndPeriod(1);

	// COM�̉���
	::CoUninitialize();

	// �߂�l(���b�Z�[�W��wParam�p�����[�^)
	return (int)msg.wParam;
}
// ------------------------------------------------------------------------
//�@�֐���	:WndProc					�E�B���h�E�̃v���V�[�W��	
//	����	:hWnd						�E�B���h�E�̃n���h��
//			:uMessage					���b�Z�[�W�̎��
//			:wParam						���b�Z�[�W�̕⏕���
//			:lParam						���b�Z�[�W�̕⏕���
//	�߂�l	:							���b�Z�[�W��߂�		
// ------------------------------------------------------------------------
LRESULT CALLBACK CApplication::WndProc(HWND hWnd,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	// ���b�Z�[�W����
	switch(uMessage)
	{
		case WM_KEYDOWN:
		{
			switch(wParam)
			{
				case VK_ESCAPE:
				{
					SendMessage(hWnd, WM_CLOSE, 0L, 0L);
					break;
				}
			}
			break;
		}
		// �\�����[�h�̐؂�ւ�(ResizeBuffers�w�肪��낵���Ȃ��A�x�����o��)
		case UM_CHANGEWINDOWMODE:
		{
			if (CApplication::GetInstance()->m_pDXDevice && CTimerThread::GetInstance()->GetTimerThreadSleep())
			{
				//CApplication::GetInstance()->m_pDXDevice->ChangeWindowMode();

				DXGI_SWAP_CHAIN_DESC desc;
				//CApplication::GetInstance()->m_pDXDevice->GetSwapChain()->GetDesc( &desc );
				BOOL FullScreen = FALSE;
				//CApplication::GetInstance()->m_pDXDevice->GetSwapChain()->GetFullscreenState( &FullScreen, NULL );
				if (!FullScreen)
					::ShowWindow(hWnd, SW_SHOW);
				CTimerThread::GetInstance()->SetTimerThreadSleep(FALSE);
			}
			break;
		}
		// �E�B���h�E�̔j������	
		case WM_CLOSE:
		case WM_DESTROY:
		{
			PostQuitMessage(0);								// �E�B���h�E�I���ʒm
			break;
		}
		// Alt + Enter ��Beep�����~�߂�
		case WM_MENUCHAR:
		{
            return(MNC_CLOSE << 16);
            break;
		}
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}
// ------------------------------------------------------------------------
//�@�֐���	:InitWindow					�E�B���h�E�̏�����	
//	����	:hInstance					�C���X�^���X�n���h��
//			:WindowTitle				�^�C�g����
//			:WindowSize					�E�B���h�E�T�C�Y
//			:Windowed					TRUE:�E�B���h�E���[�h�AFALSE:�t���X�N���[�����[�h
//	�߂�l	:							�E�B���h�E�n���h��		
// ------------------------------------------------------------------------
HWND CApplication::InitWindow(	HINSTANCE hInstance,
								LPCWSTR WindowTitle,
								SIZE WindowSize,
								BOOL Windowed)
{
	// �E�B���h�E�N���X�̓o�^
	WNDCLASSEX wcex;	// �E�B���h�E�N���X�̏��i�[�p�\���̕ϐ�
	::ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize			= sizeof(WNDCLASSEX);					// �\���̂̃T�C�Y��ݒ�
	wcex.style			= CS_HREDRAW | CS_VREDRAW;				// �E�B���h�E�X�^�C��(��/�����ύX����)
	wcex.lpfnWndProc	= (WNDPROC)WndProc;						// �E�B���h�E�v���V�[�W���̎w��
	wcex.cbClsExtra		= 0;									// �ǉ��̈�͎g��Ȃ�
	wcex.cbWndExtra		= 0;									// �ǉ��̈�͎g��Ȃ�
	wcex.hInstance		= hInstance;							// ���̃C���X�^���X�̃n���h��
	wcex.hIcon			= LoadIcon(hInstance,IDI_APPLICATION);	// ���[�W�A�C�R��
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);		// �J�[�\���X�^�C��
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);	// �w�i�F ���F
	wcex.lpszMenuName	= nullptr;								// �\�����j���[�̃Z���N�g(���j���[�Ȃ�)
	wcex.lpszClassName	= "MainWindowClass";					// �E�B���h�E�N���X��
	wcex.hIconSm		= LoadIcon(wcex.hInstance,( LPCTSTR)IDI_APPLICATION);// �X���[���A�C�R��
	::RegisterClassEx(&wcex);									// �E�B���h�E�N���X�̓o�^

	// �E�B���h�E�̍쐬
	HWND hWnd = ::CreateWindowW(
		L"MainWindowClass",		// �N���X��
		WindowTitle,			// �^�C�g��
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,	// �X�^�C��
		CW_USEDEFAULT,			// �����W(Windows�ɔC����)
		CW_USEDEFAULT,			// �����W(Windows�ɔC����)�����ō���p�̍��W
		WindowSize.cx,			// ��
		WindowSize.cy,			// ����
		HWND_DESKTOP,			// ����
		(HMENU)nullptr,			// ���j���[����
		hInstance,				// ���̃v���O�����̃C���X�^���X�̃n���h��
		nullptr);				// �ǉ���������
	if (nullptr == hWnd)
		return nullptr;

	// �N���C�A���g�T�C�Y�̕ύX
	SetClientSize(hWnd, WindowSize);
	// �E�B���h�E�\���ʒu����ʂ̒��S�ύX
	WindowMoveToCenter(hWnd);

	return hWnd;
}
// ------------------------------------------------------------------------
//�@�֐���	:SetClientSize				���C�A���g�̈�̃T�C�Y���w��
//	����	:hWnd						�E�B���h�E�n���h��
//			:Size						�w�肷��T�C�Y
//	�߂�l	:							�Ȃ�	
// ------------------------------------------------------------------------
void CApplication::SetClientSize(HWND hWnd,SIZE size)
{
	// �N���C�A���g�̈�T�C�Y�̎擾
	SIZE ClientSize = GetClientSize(hWnd);
	// �N���C�A���g�̈�Ɛݒ肷��T�C�Y�̍����擾
	SIZE CompensationSize;
	CompensationSize.cx = size.cx - ClientSize.cx;
	CompensationSize.cy = size.cy - ClientSize.cy;
	// �E�B���h�E��`�̕��ƍ������擾
	SIZE WindowSize = GetWindowSize( hWnd );
	// �E�B���h�E�̕��ƍ������w��
	m_WindowSize;
	m_WindowSize.cx = WindowSize.cx + CompensationSize.cx;
	m_WindowSize.cy = WindowSize.cy + CompensationSize.cy;
	// �T�C�Y���w��
	::SetWindowPos(hWnd, nullptr, 0, 0, m_WindowSize.cx, m_WindowSize.cy, SWP_HIDEWINDOW);
}
// ------------------------------------------------------------------------
//�@�֐���	:WindowMoveToCenter			�E�B���h�E����ʒ��S�ֈړ�
//	����	:hWnd						�E�B���h�E�n���h��
//	�߂�l	:							�Ȃ�	
// ------------------------------------------------------------------------
void CApplication::WindowMoveToCenter(HWND hWnd)
{
	// �f�X�N�g�b�v�̋�`���擾
	RECT Desktop;
	::GetWindowRect(::GetDesktopWindow(), (LPRECT)&Desktop);
	RECT Window;
	::GetWindowRect( hWnd, (LPRECT)&Window );
	// �E�B���h�E�̒��S���W�����ߍŏI�I�Ɉʒu������
	m_WindowShowPosition.x = (Desktop.right - (Window.right - Window.left)) / 2;
	m_WindowShowPosition.y = (Desktop.bottom - (Window.bottom - Window.top)) / 2;
	// �|�W�V�������ړ�����
	::SetWindowPos(hWnd, HWND_TOP, m_WindowShowPosition.x, m_WindowShowPosition.y, (Window.right - Window.left), (Window.bottom - Window.top), SWP_HIDEWINDOW);
}
// ------------------------------------------------------------------------
//�@�֐���	:GetWindowSize				�E�B���h�E�T�C�Y���擾
//	����	:hWnd						�E�B���h�E�n���h��
//	�߂�l	:							�E�B���h�E�T�C�Y	
// ------------------------------------------------------------------------
SIZE CApplication::GetWindowSize(HWND hWnd)
{
	// �E�B���h�E�̋�`���擾
	RECT Window;
	::GetWindowRect( hWnd, &Window );

	// �T�C�Y�����߂�
	SIZE Result;
	Result.cx = Window.right - Window.left;
	Result.cy = Window.bottom - Window.top;
	
	return Result;
}
// ------------------------------------------------------------------------
//�@�֐���	:GetClientSize				�N���C�A���g�̈�T�C�Y���擾
//	����	:hWnd						�E�B���h�E�n���h��
//	�߂�l	:							�N���C�A���g�̈�T�C�Y	
// ------------------------------------------------------------------------
SIZE CApplication::GetClientSize(HWND hWnd)
{
	// �N���C�A���g�̈�̋�`���擾
	RECT Client;
	::GetClientRect(hWnd, &Client);

	// �T�C�Y�����߂�
	SIZE Result;
	Result.cx = Client.right - Client.left;
	Result.cy = Client.bottom - Client.top;

	return Result;
}
// ------------------------------------------------------------------------
//�@�֐���	:GetInstance				�C���X�^���X�̎擾
//	����	:							�Ȃ�
//	�߂�l	:							�C���X�^���X�n���h��	
// ------------------------------------------------------------------------
CApplication* CApplication::GetInstance()
{
	return &m_Instance;
}