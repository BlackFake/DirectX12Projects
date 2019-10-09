// --------------- IncludeFile ---------------
#include "DXDevice.h"

// �R���X�g���N�^
CDXDevice::CDXDevice()
{
	m_hWnd = nullptr;
	m_WindowSize = SIZE();
	m_pd3dDevice = nullptr;
	m_pCommandQueue = nullptr;
	m_pSwapChain = nullptr;
	m_prtvDescriptorHeap = nullptr;
	m_rtvbDescriptorSize = 0;
	m_pdsvDescriptorHeap = nullptr;
	m_pDepthBuffer = nullptr;
	m_FenceIndex = 0;
	m_pCommandList = nullptr;

	m_pSceneManager = nullptr;
	ZeroMemory(&m_DisplayMode, sizeof(m_DisplayMode));
}

// �f�X�g���N�^
CDXDevice::~CDXDevice()
{
	SAFE_RELEASE(m_pCommandList);

	// �t�F���X���
	for (UINT i = 0; i < FrameCount; i++)
		SAFE_RELEASE(m_pFence[i]);

	// �R�}���h�A���P�[�^�[���
	for (UINT i = 0; i < FrameCount; i++)
		SAFE_RELEASE(m_pCommandAllocator[i]);

	SAFE_RELEASE(m_pDepthBuffer);
	SAFE_RELEASE(m_pdsvDescriptorHeap);

	// �����_�[�^�[�Q�b�g�r���[���
	for (UINT i = 0; i < FrameCount; i++)
		SAFE_RELEASE(m_pRenderTargetView[i]);

	SAFE_RELEASE(m_prtvDescriptorHeap);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pCommandQueue);
	SAFE_RELEASE(m_pd3dDevice);
}

// ------------------------------------------------------------------------
//�@�֐���  :Initialize                 �f�o�C�X�̏�����
//  ����	:hWnd                       �E�B���h�E�n���h��
//          :WindowSize                 �E�B���h�E�T�C�Y
//          :Windowed                   �\�����[�h
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::Initialize(HWND hWnd, SIZE WindowSize, BOOL Windowed)
{
	m_hWnd = hWnd;
	m_WindowSize = WindowSize;

	HRESULT hr = S_OK;

	// �f�o�b�O���C���[�̗L��
	ID3D12Debug* pDebug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug))))
		pDebug->EnableDebugLayer();

	// GBV�̗L����
	ID3D12Debug1* pGBV = nullptr;
	if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug))))
		return E_FAIL;

	if (SUCCEEDED(pDebug->QueryInterface(IID_PPV_ARGS(&pGBV))))
	{
		pGBV->SetEnableGPUBasedValidation(TRUE);
	}

	SAFE_RELEASE(pGBV);
	SAFE_RELEASE(pDebug);

	// DXGI�t�@�N�g���[�̐���
	IDXGIFactory3* pDXGIFactory = nullptr;
	UINT Flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hr = CreateDXGIFactory2(Flags, IID_PPV_ARGS(&pDXGIFactory));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDXGIFactory2()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �n�[�h�E�F�A�A�_�v�^�[�̌���
	IDXGIAdapter1* pDXGIAdapter = nullptr;
	UINT AdapterIndex = { 0, };
	while (DXGI_ERROR_NOT_FOUND !=
		pDXGIFactory->EnumAdapters1(AdapterIndex, &pDXGIAdapter))
	{
		DXGI_ADAPTER_DESC1 desc = { 0, };
		pDXGIAdapter->GetDesc1(&desc);
		++AdapterIndex;
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		// D3D12�f�o�C�X���g�p�\��
		hr = D3D12CreateDevice(pDXGIAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
			break;

		SAFE_RELEASE(pDXGIAdapter);
	}

	// D3D12�f�o�C�X�̐���
	hr = D3D12CreateDevice(pDXGIAdapter,
		D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pd3dDevice));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("D3D12CreateDevice()�Ɏ��s�B\n");

		return E_FAIL;
	}

	SAFE_RELEASE(pDXGIAdapter);

	// �R�}���h�L���[�̐���
	if (FAILED(CreateCommandQueue()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandQueue()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �X���b�v�`�F�C���̐���
	RECT rect = {0,};
	::GetClientRect(hWnd, &rect);
	// �T�C�Y�����߂�
	SIZE Size;
	Size.cx = rect.right - rect.left;
	Size.cy = rect.bottom - rect.top;

	DXGI_SWAP_CHAIN_DESC1 SCDesc;
	ZeroMemory(&SCDesc, sizeof(SCDesc));
	SCDesc.BufferCount = FrameCount;
	if (Windowed)
	{
		SCDesc.Width = Size.cx;		// �o�b�t�@�̕�
		SCDesc.Height = Size.cy;	// �o�b�t�@�̍���
	}
	else
	{
		SCDesc.Width = WindowSize.cx;		// �o�b�t�@�̕�
		SCDesc.Height = WindowSize.cy;	// �o�b�t�@�̍���
	}
	
	SCDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SCDesc.SampleDesc.Count = 1;
	
	IDXGISwapChain1* pSwapChain = nullptr;
	hr = pDXGIFactory->CreateSwapChainForHwnd(m_pCommandQueue, hWnd, &SCDesc, nullptr, nullptr, &pSwapChain);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	m_pSwapChain = reinterpret_cast<IDXGISwapChain3*>(pSwapChain);

	SAFE_RELEASE(pDXGIFactory);

	// �f�B�X�v���C���[�h�̎擾
	if (FAILED(GetDisplayMode()))
	{
		if (g_DebugFlag)
			OutputDebugString("GetDisplayMode()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �����_�[�^�[�Q�b�g�̐���
	if (FAILED(CreateRenderTargetView()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateRenderTargetView()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �f�v�X�X�e���V���r���[�̐���
	if (FAILED(CreateDepthStencilView()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDepthStencilView()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �R�}���h�A���P�[�^�[�̐���
	if (FAILED(CreateCommandAllocators()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandAllocators()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �t���[���t�F���X�̐���
	if (FAILED(CreateFrameFences()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateFrameFences()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �R�}���h���X�g�̐���
	if (FAILED(CreateCommandList()))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandList()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �r���[�|�[�g�̎w��
	CD3DX12_VIEWPORT Vp;
	D3D12_RECT rc;
	ZeroMemory(&Vp, sizeof(Vp));
	ZeroMemory(&rc, sizeof(rc));

	if (!Windowed)
	{
		Vp.Width = static_cast<FLOAT>(m_DisplayMode.Width);
		Vp.Height = static_cast<FLOAT>(m_DisplayMode.Height);
		rc.right = static_cast<LONG>(m_DisplayMode.Width);
		rc.bottom = static_cast<LONG>(m_DisplayMode.Height);
	}
	else
	{
		Vp.Width = static_cast<FLOAT>(Size.cx);
		Vp.Height = static_cast<FLOAT>(Size.cy);
		rc.right = static_cast<LONG>(m_DisplayMode.Width);
		rc.bottom = static_cast<LONG>(m_DisplayMode.Height);
	}
	Vp.MinDepth = 0.0f;
	Vp.MaxDepth = 1.0f;
	Vp.TopLeftX = 0;
	Vp.TopLeftY = 0;
	//m_pCommandList->RSSetViewports(1, &Vp);
	//m_pCommandList->RSSetScissorRects(1, &rc);
	m_ViewPort = Vp;
	m_ScissorRect = rc;

	// ���\�[�X�̎���
	if (!CreateResource())
	{
		CleanupResource();
		return E_FAIL;
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateCommandQueue			�R�}���h�L���[�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateCommandQueue()
{
	HRESULT hr = S_OK;

	D3D12_COMMAND_QUEUE_DESC desc = {
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		0,
		D3D12_COMMAND_QUEUE_FLAG_NONE,
		0
	};

	hr = m_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandQueue()�Ɏ��s�B\n");

		return E_FAIL;
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateRenderTargetView     �����_�[�^�[�Q�b�g�r���[�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateRenderTargetView()
{
	HRESULT hr = S_OK;

	// �����_�[�^�[�Q�b�g�p�̃f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		FrameCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		0
	};

	hr = m_pd3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_prtvDescriptorHeap));
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	// �����_�[�^�[�Q�b�g�p�̃f�X�N���v�^�q�[�v�o�C�g�����擾
	m_rtvbDescriptorSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// �����_�[�^�[�Q�b�g�r���[�̐���
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_prtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < FrameCount; ++i)
	{
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargetView[i]));
		m_pd3dDevice->CreateRenderTargetView(m_pRenderTargetView[i], nullptr, rtvHandle);
		// �Q�Ƃ���f�X�N���v�^��ύX
		rtvHandle.Offset(1, m_rtvbDescriptorSize);
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateDepthStencilView     �f�v�X�X�e���V���r���[�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC1 desc;
	m_pSwapChain->GetDesc1(&desc);
	BOOL FullScreen = FALSE;
	m_pSwapChain->GetFullscreenState(&FullScreen, nullptr);

	// �E�B���h�E�̋�`���擾
	RECT rect;
	::GetWindowRect(m_hWnd, &rect);

	SIZE Size;
	Size.cx = rect.right - rect.left;
	Size.cy = rect.bottom - rect.top;

	// �f�v�X�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	::ZeroMemory(&dsvHeapDesc, sizeof(dsvHeapDesc));
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = m_pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pdsvDescriptorHeap));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDescriptorHeap()�Ɏ��s�B\n");

		return E_FAIL;
	}

	SIZE DepthSize;
	if (FullScreen)
	{
		DepthSize.cx = Size.cx;		// �o�b�t�@�̕�
		DepthSize.cy = Size.cy;	// �o�b�t�@�̍���
	}
	else
	{
		DepthSize.cx = m_WindowSize.cx;		// �o�b�t�@�̕�
		DepthSize.cy = m_WindowSize.cy;		// �o�b�t�@�̍���
	}

	// �f�v�X�o�b�t�@�̐���
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,	// �t�H�[�}�b�g
		DepthSize.cx,			// �����_�[�^�[�Q�b�g�Ɠ����T�C�Y
		DepthSize.cy,			// �����_�[�^�[�Q�b�g�Ɠ����T�C�Y
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	D3D12_CLEAR_VALUE DepthClearValue = {};
	DepthClearValue.Format = resDesc.Format;
	DepthClearValue.DepthStencil.Depth = 1.0f;
	DepthClearValue.DepthStencil.Stencil = 0;

	hr = m_pd3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �f�v�X��������
		&DepthClearValue,
		IID_PPV_ARGS(&m_pDepthBuffer)
	);

	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommittedResource()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �f�v�X�X�e���V���r���[����
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	::ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pdsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	m_pd3dDevice->CreateDepthStencilView(m_pDepthBuffer, &dsvDesc, dsvHandle);

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateCommandAllocators    �o�b�t�@�̐������A���P�[�^�[�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateCommandAllocators()
{
	HRESULT hr = S_OK;

	for (int i = 0; i < FrameCount; i++)
	{
		hr = m_pd3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_pCommandAllocator[i])
		);

		if (FAILED(hr))
		{
			if (g_DebugFlag)
				OutputDebugString("CreateCommandAllocator()�Ɏ��s�B\n");

			return E_FAIL;
		}
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateFrameFences			�`��t���[�������p�t�F���X�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateFrameFences()
{
	HRESULT hr = S_OK;

	for (int i = 0; i < FrameCount; i++)
	{
		hr = m_pd3dDevice->CreateFence(
			0,	// �����l
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_pFence[i])
		);

		if (FAILED(hr))
		{
			if (g_DebugFlag)
				OutputDebugString("CreateFence()�Ɏ��s�B\n");

			return E_FAIL;
		}
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateCommandList			�R�}���h���X�g�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateCommandList()
{
	HRESULT hr = S_OK;

	// �R�}���h���X�g�̐���
	hr = m_pd3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator[0],
		nullptr,
		IID_PPV_ARGS(&m_pCommandList)
	);

	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandList()�Ɏ��s�B\n");

		return E_FAIL;
	}

	hr = m_pCommandList->Close();

	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CommandList��Close()�Ɏ��s�B\n");

		return E_FAIL;
	}

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :GetDisplayMode				�f�B�X�v���C���[�h�̎擾
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::GetDisplayMode()
{
	HRESULT hr = S_OK;

	IDXGIFactory3* pDXGIFactory = nullptr;
	IDXGIAdapter1* pDXGIAdapter = nullptr;

	hr = m_pSwapChain->GetParent(IID_PPV_ARGS(&pDXGIFactory));

	// �A�_�v�^���(��������ꍇ�̏����͊���)
	pDXGIFactory->EnumAdapters1(0, &pDXGIAdapter);
	SAFE_RELEASE(pDXGIFactory);

	IDXGIOutput* pOutput = nullptr;
	hr = pDXGIAdapter->EnumOutputs(0, &pOutput);
	if (FAILED(hr))
		return E_FAIL;


	DXGI_SWAP_CHAIN_DESC desc;
	m_pSwapChain->GetDesc(&desc);

	// �f�B�X�v���C���[�h�̎擾
	UINT NumDisplayMode = 0;
	hr = pOutput->GetDisplayModeList(desc.BufferDesc.Format, 0, &NumDisplayMode, 0);
	if (FAILED(hr))
		return E_FAIL;

	DXGI_MODE_DESC* pDisplayMode = new DXGI_MODE_DESC[NumDisplayMode];
	hr = pOutput->GetDisplayModeList(desc.BufferDesc.Format, 0, &NumDisplayMode, pDisplayMode);

	int Level = 1000000;
	for (DWORD i = 0; i < NumDisplayMode; i++)
	{
		if (pDisplayMode[i].Format != DXGI_FORMAT_R8G8B8A8_UNORM)
			continue;

		int l = abs((int)(pDisplayMode[i].Width - m_WindowSize.cx)) + abs((int)(pDisplayMode[i].Height - m_WindowSize.cy));
		if (l < Level)
		{
			memcpy(&m_DisplayMode, &pDisplayMode[i], sizeof(DXGI_MODE_DESC));
			Level = l;
		}
	}
	SAFE_DELETE_ARRAY(pDisplayMode);
	if (FAILED(hr))
		return E_FAIL;

	SAFE_RELEASE(pDXGIAdapter);
	SAFE_RELEASE(pDXGIFactory);
	SAFE_RELEASE(pOutput);

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateResource				���\�[�X�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDXDevice::CreateResource()
{
	// �V�[���}�l�[�W���[�̐���
	m_pSceneManager = new CSceneManager(m_hWnd, m_pd3dDevice, m_pCommandList);
	if (nullptr == m_pSceneManager)
		return FALSE;

	if (FAILED(m_pSceneManager->Initialize()))
		return FALSE;

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���  :CleanupResource			���\�[�X�̉��
//  ����	:							�Ȃ�
//  �߂�l  :                           �Ȃ�
// ------------------------------------------------------------------------
void CDXDevice::CleanupResource()
{
	SAFE_DELETE(m_pSceneManager);
}
// ------------------------------------------------------------------------
//�@�֐���  :Render				        �����_�����O����
//  ����	:							�Ȃ�
//  �߂�l  :                           �Ȃ�
// ------------------------------------------------------------------------
void CDXDevice::Render()
{
	ID3D12PipelineState* pPipelineState = m_pSceneManager->GetPipelineState();

	// �X���b�v�`�F�C���C���[�W���C���f�b�N�X�擾
	m_FenceIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	m_pCommandAllocator[m_FenceIndex]->Reset();
	m_pCommandList->Reset(
		m_pCommandAllocator[m_FenceIndex],
		pPipelineState
	);

	m_pCommandList->RSSetViewports(1, &m_ViewPort);
	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);


	// �X���b�v�`�F�C���\���\���烌���_�[�^�[�Q�b�g�`��\��
	auto BarrierToRT = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pRenderTargetView[m_FenceIndex],
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	m_pCommandList->ResourceBarrier(1, &BarrierToRT);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
		m_prtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_FenceIndex,
		m_rtvbDescriptorSize
	);

	// �f�v�X�o�b�t�@(�f�v�X�X�e���V���r���[)�̃N���A
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(
		m_pdsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);

	m_pCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �`�����Z�b�g
	m_pCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	// �J���[�o�b�t�@(�����_�[�^�[�Q�b�g�r���[)�̃N���A
	const FLOAT ClearColor[] = { 0.1f, 0.25f, 0.5f,1.0f };	 // red, green, blue, alpha
	m_pCommandList->ClearRenderTargetView(rtv, ClearColor, 0, nullptr);

	// �`��R�}���h();
	m_pSceneManager->Render();

	auto BarrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pRenderTargetView[m_FenceIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	m_pCommandList->ResourceBarrier(1, &BarrierToPresent);

	// �`��R�}���h�̐ςݏ����I��
	m_pCommandList->Close();

	ID3D12CommandList* pList[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, pList);

	// ��ʂւ̕\��
	m_pSwapChain->Present(1, 0);

	// �ȑO�̃R�}���h�̎��s��҂�
	WaitPreviousFrame(m_pFence[m_FenceIndex], m_FenceIndex);
}
// ------------------------------------------------------------------------
//�@�֐���  :WaitPreviousFrame		    CPU�AGPU�̓�������
//  ����	:pFence						�t�F���X�̃A�h���X
//			:Value						�C���f�b�N�X
//  �߂�l  :                           �Ȃ�
// ------------------------------------------------------------------------
void CDXDevice::WaitPreviousFrame(ID3D12Fence* pFence, UINT64 Value)
{
	const UINT64 value = m_FenceIndex;
	m_pCommandQueue->Signal(pFence, value);
	m_FenceIndex++;

	if (pFence->GetCompletedValue() < value)
	{
		HANDLE hHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (hHandle != nullptr)
		{
			pFence->SetEventOnCompletion(value, hHandle);
			WaitForSingleObject(hHandle, INFINITE);
			CloseHandle(hHandle);
		}
	}
	m_FenceIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}