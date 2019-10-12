// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 6
#ifndef __DXDEVICE_H__
#define __DXDEVICE_H__
// --------------- IncludeFile ---------------
#include "DirectXinclude.h"
#include "Global.h"
#include <dxgidebug.h>
#include "InputDevice.h"
#include "SceneManager.h"
// --- DebugInclude ---
#include <cstdlib>
#include <new>
#include <memory>
#include <crtdbg.h>
#include <thread>
#include <mutex>
// --- Debug�ׂ̈̃}�N�� ---
// ���������[�N�̏ꏊ�����m�ɕ\�������
#define _CRTDBG_MAP_ALLOC
#define new  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
// ----------------- �萔�錾 ----------------
static constexpr int FrameCount = 2;	// �o�b�N�o�b�t�@�[�̐�
// ---------------- �N���X�錾 ---------------
class CDXDevice
{
private:
	// ----- �`��� -----
	HWND									m_hWnd;								// �E�B���h�E�n���h��
	SIZE									m_WindowSize;						// �N�����̃E�B���h�E�T�C�Y
	ComPtr<ID3D12Device>					m_pd3dDevice;						// D3D12�f�o�C�X
	ComPtr<IDXGISwapChain3>					m_pSwapChain;						// �X���b�v�`�F�[��
	ComPtr<ID3D12GraphicsCommandList>		m_pCommandList;						// �R�}���h���X�g
	ComPtr<ID3D12CommandQueue>				m_pCommandQueue;					// �R�}���h�L���[
	ComPtr<ID3D12CommandAllocator>			m_pCommandAllocator[FrameCount];	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12Fence1>					m_pFence[FrameCount];				// �t�F���X
	UINT									m_FenceIndex;						// �t�F���X�C���f�b�N�X
	UINT64									m_FenceValue;						// �t�F���X�o�����[
	ComPtr<ID3D12Resource>					m_pRenderTargetView[FrameCount];	// �����_�[�^�[�Q�b�g�r���[
	ComPtr<ID3D12DescriptorHeap>			m_prtvDescriptorHeap;				// �����_�[�^�[�Q�b�g�p�f�X�N���v�^�q�[�v
	UINT									m_rtvbDescriptorSize;				// �����_�[�^�[�Q�b�g�p�f�X�N���v�^�o�C�g��
	ComPtr<ID3D12DescriptorHeap>			m_pdsvDescriptorHeap;				// �f�v�X�X�e���V���r���[�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource>					m_pDepthBuffer;						// �f�v�X�X�e���V���o�b�t�@�[
	DXGI_MODE_DESC1							m_DisplayMode;						// �f�B�X�v���C���[�h
	CD3DX12_VIEWPORT						m_ViewPort;
	D3D12_RECT								m_ScissorRect;
	// ----- �`��֘A -----
	CSceneManager*							m_pSceneManager;					// �V�[���}�l�[�W���[

private:
	HRESULT CreateCommandQueue();						// �R�}���h�L���[�̐���
	HRESULT CreateRenderTargetView();					// �����_�[�^�[�Q�b�g�r���[�̐���
	HRESULT CreateDepthStencilView();					// �f�v�X�X�e���V���r���[�̐���
	HRESULT CreateCommandAllocators();					// �o�b�t�@�̐������A���P�[�^�[�̐���
	HRESULT CreateFences();								// �`��t���[�������p�t�F���X�̐���
	HRESULT CreateCommandList();						// �R�}���h���X�g�̐���
	void WaitPreviousFrame();							// CPU�AGPU�̓�������
	void ChangeWindowModeOptimize();					// �\�����[�h�̍œK��
public:
	CDXDevice();
	~CDXDevice();
	HRESULT Initialize(HWND, SIZE, BOOL);		// �f�o�C�X�̏�����
	HRESULT GetDisplayMode();					// �f�B�X�v���C���[�h�̎擾
	BOOL CreateResource();						// ���\�[�X�̐���
	void CleanupResource();						// ���\�[�X�̉��
	void Render();								// �����_�����O����
	// --- �Q�b�^�[�A�Z�b�^�[ ---
	ID3D12Device* GetDevice()		{ return m_pd3dDevice.Get(); }
	IDXGISwapChain3* GetSwapChain() { return m_pSwapChain.Get(); }
};
/*
class CDXDevice
{
private:
	HWND					m_hWnd;					// �E�B���h�E�n���h��
	SIZE					m_WindowSize;			// �N�����̃E�B���h�E�T�C�Y
	ID3D11Device*			m_pd3dDevice;			// �f�o�C�X
	IDXGISwapChain*			m_pSwapChain;			// �X���b�v�`�F�[��
	ID3D11DeviceContext*	m_pDeviceContext;		// �f�o�C�X�R���e�L�X�g
	ID3D11RenderTargetView* m_pRenderTargetView;	// �����_�[�^�[�Q�b�g�r���[
	ID3D11Texture2D*		m_pDepthStencil;		// �f�v�X�X�e���V��
	ID3D11DepthStencilView* m_pDepthStencilView;	// �f�v�X�X�e���V���r���[
	DXGI_MODE_DESC			m_DisplayMode;			// �f�B�X�v���C���[�h
	CGrid*					m_pGrid;				// �O���b�g�N���X
private:
	HRESULT CreateRenderTargetView();				// �����_�[�^�[�Q�b�g�r���[�̐���
	HRESULT CreateDepthStencilView();				// �f�v�X�X�e���V���r���[�̐���
	void ChangeWindowModeOptimize();				// �\�����[�h�̍œK��
public:
	CDXDevice();
	~CDXDevice();
	HRESULT Initialize(HWND, SIZE, BOOL);			// �f�o�C�X�̏�����
	HRESULT GetDisplayMode();						// �f�B�X�v���C���[�h�̎擾
	void ChangeWindowMode();						// �\�����[�h�̐؂�ւ�
	BOOL CreateResource();							// ���\�[�X�̐���
	void CleanupResource();							// ���\�[�X�̉��
	void Render();									// �����_�����O����
	ID3D11Device* GetDevice()				{ return m_pd3dDevice;		}
	ID3D11DeviceContext* GetDeviceContext()	{ return m_pDeviceContext;	}
	IDXGISwapChain* GetSwapChain()			{ return m_pSwapChain;		}
};
*/
#endif	/* __DXDEVICE_H__ */