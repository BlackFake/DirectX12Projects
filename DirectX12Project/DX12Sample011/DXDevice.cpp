// --------------- IncludeFile ---------------
#include "DXDevice.h"

// �R���X�g���N�^
CDXDevice::CDXDevice()
{
	m_hWnd = nullptr;
	ZeroMemory(&m_WindowSize, sizeof(m_WindowSize));
	m_FenceIndex = 0;
	m_FenceValue = 0;
	m_rtvbDescriptorSize = 0;
	ZeroMemory(&m_DisplayMode, sizeof(m_DisplayMode));
	ZeroMemory(&m_ViewPort, sizeof(m_ViewPort));
	ZeroMemory(&m_ScissorRect, sizeof(m_ScissorRect));

	m_pSceneManager = nullptr;
}

// �f�X�g���N�^
CDXDevice::~CDXDevice()
{
	
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
	ComPtr<ID3D12Debug> pDebug;
	ComPtr<ID3D12Debug3> pGPUBasedValidation;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug))))
		pDebug->EnableDebugLayer();

	// GPU�x�[�X�̃o���f�[�V�����L����
	pDebug.As(&pGPUBasedValidation);
	pGPUBasedValidation->SetEnableGPUBasedValidation(TRUE);

	// DXGI�t�@�N�g���[�̐���
	ComPtr<IDXGIFactory3> pDXGIFactory;
	UINT Flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	
	if (FAILED(CreateDXGIFactory2(Flags, IID_PPV_ARGS(&pDXGIFactory))))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDXGIFactory2()�Ɏ��s�B\n");

		return E_FAIL;
	}

	// �n�[�h�E�F�A�A�_�v�^�[�̌���
	ComPtr<IDXGIAdapter1> pDXGIAdapter;
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
		hr = D3D12CreateDevice(pDXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
			break;
	}

	// D3D12�f�o�C�X�̐���
	ComPtr<ID3D12Device> pd3dDevice;
	hr = D3D12CreateDevice(pDXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pd3dDevice));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("D3D12CreateDevice()�Ɏ��s�B\n");

		return E_FAIL;
	}
	pd3dDevice.As(&m_pd3dDevice);

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
		SCDesc.Width = WindowSize.cx;	// �o�b�t�@�̕�
		SCDesc.Height = WindowSize.cy;	// �o�b�t�@�̍���
	}
	
	SCDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SCDesc.SampleDesc.Count = 1;
	
	ComPtr<IDXGISwapChain1> pSwapChain;
	hr = pDXGIFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), hWnd, &SCDesc, nullptr, nullptr, &pSwapChain);
	if (FAILED(hr))
		return E_FAIL;

	pSwapChain.As(&m_pSwapChain);

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

	// �t�F���X�̐���
	if (FAILED(CreateFences()))
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

	ComPtr<ID3D12CommandQueue> pCommandQueue;
	hr = m_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&pCommandQueue));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandQueue()�Ɏ��s�B\n");

		return E_FAIL;
	}
	pCommandQueue.As(&m_pCommandQueue);

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

	ComPtr<ID3D12DescriptorHeap> prtvDescriptorHeap;
	hr = m_pd3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&prtvDescriptorHeap));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDescriptorHeap()�Ɏ��s�B\n");

		return E_FAIL;
	}
	prtvDescriptorHeap.As(&m_prtvDescriptorHeap);

	// �����_�[�^�[�Q�b�g�p�̃f�X�N���v�^�q�[�v�o�C�g�����擾
	m_rtvbDescriptorSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// �����_�[�^�[�Q�b�g�r���[�̐���
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_prtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < FrameCount; ++i)
	{
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargetView[i]));
		m_pd3dDevice->CreateRenderTargetView(m_pRenderTargetView[i].Get(), nullptr, rtvHandle);
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

	ComPtr<ID3D12DescriptorHeap> pdscDescriptorHeap;
	hr = m_pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pdscDescriptorHeap));
	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateDescriptorHeap()�Ɏ��s�B\n");

		return E_FAIL;
	}
	pdscDescriptorHeap.As(&m_pdsvDescriptorHeap);

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

	ComPtr<ID3D12Resource> pDepthBuffer;
	hr = m_pd3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �f�v�X��������
		&DepthClearValue,
		IID_PPV_ARGS(&pDepthBuffer)
	);
	pDepthBuffer.As(&m_pDepthBuffer);

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
	m_pd3dDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), &dsvDesc, dsvHandle);

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
	ComPtr<ID3D12CommandAllocator> pCommandAllocator[FrameCount];

	for (int i = 0; i < FrameCount; i++)
	{
		hr = m_pd3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&pCommandAllocator[i])
		);

		if (FAILED(hr))
		{
			if (g_DebugFlag)
				OutputDebugString("CreateCommandAllocator()�Ɏ��s�B\n");

			return E_FAIL;
		}
		pCommandAllocator[i].As(&m_pCommandAllocator[i]);
	}
	
	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���  :CreateFences				�`��t���[�������p�t�F���X�̐���
//  ����	:							�Ȃ�
//  �߂�l  :                           S_OK:�����AE_FAIL:��������̖�蔭��
// ------------------------------------------------------------------------
HRESULT CDXDevice::CreateFences()
{
	HRESULT hr = S_OK;
	ComPtr<ID3D12Fence1> pFence[FrameCount];

	for (int i = 0; i < FrameCount; i++)
	{
		hr = m_pd3dDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&pFence[i])
		);

		if (FAILED(hr))
		{
			if (g_DebugFlag)
				OutputDebugString("CreateFence()�Ɏ��s�B\n");

			return E_FAIL;
		}
		pFence[i].As(&m_pFence[i]);
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
	ComPtr<ID3D12GraphicsCommandList> pCommandList;

	// �R�}���h���X�g�̐���
	hr = m_pd3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator[0].Get(),
		nullptr,
		IID_PPV_ARGS(&pCommandList)
	);
	pCommandList.As(&m_pCommandList);

	if (FAILED(hr))
	{
		if (g_DebugFlag)
			OutputDebugString("CreateCommandList()�Ɏ��s�B\n");

		return E_FAIL;
	}

	if (FAILED(m_pCommandList->Close()))
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
	m_pSceneManager = new CSceneManager(m_hWnd, m_pd3dDevice.Get(), m_pCommandList.Get());
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
		m_pCommandAllocator[m_FenceIndex].Get(),
		pPipelineState
	);

	m_pCommandList->RSSetViewports(1, &m_ViewPort);
	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);


	// �X���b�v�`�F�C���\���\���烌���_�[�^�[�Q�b�g�`��\��
	auto BarrierToRT = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pRenderTargetView[m_FenceIndex].Get(),
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
		m_pRenderTargetView[m_FenceIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	m_pCommandList->ResourceBarrier(1, &BarrierToPresent);

	// �`��R�}���h�̐ςݏ����I��
	m_pCommandList->Close();

	ID3D12CommandList* pList[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(1, pList);

	// ��ʂւ̕\��
	m_pSwapChain->Present(1, 0);

	// �ȑO�̃R�}���h�̎��s��҂�
	WaitPreviousFrame();
}
// ------------------------------------------------------------------------
//�@�֐���  :WaitPreviousFrame		    CPU�AGPU�̓�������
//  ����	:pFence						�t�F���X�̃A�h���X
//			:Value						�C���f�b�N�X
//  �߂�l  :                           �Ȃ�
// ------------------------------------------------------------------------
void CDXDevice::WaitPreviousFrame()
{
	const UINT64 value = m_FenceValue;
	ID3D12Fence1* pFence = m_pFence[m_FenceIndex].Get();
	m_pCommandQueue->Signal(pFence, value);
	m_FenceValue++;

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