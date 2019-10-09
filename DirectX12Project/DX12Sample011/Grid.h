// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __GRID_H__
#define __GRID_H__
#pragma once
// -------------------- Define�錾 -------------------
#define GRIDSSZE	10.0f							// �O���b�h�T�C�Y
// -------------------- IncludeFile ------------------
#include "Global.h"
#include "Shader\\grid_vs.h"
#include "Shader\\grid_ps.h"
#include "Shader\\grid_gs.h"
#include "Camera.h"
// -------------------- LibraryFile ------------------
// -------------------- �N���X�錾 -------------------
class CGrid
{
private:
	// ----- �\���̐錾 -----
	typedef struct _GRIDVERTEX_
	{
		VECTOR4	vPosition;				// ���W
		VECTOR4	vColor;					// ���_�̐F
	}GRIDVERTEX;
	typedef struct _tagGeometory_Shader_Global_// �R���X�^���g�o�b�t�@
	{
		XMMATRIX	matWorldTranspose;		// ���[���h�s��̓]�u�s��
		XMMATRIX	matViewTranspose;		// �r���[�s��̓]�u�s��
		XMMATRIX	matProjectionTranspose;	// �v���W�F�N�V�����̓]�u�s��
	}GEOMETORY_SHADER_GLOBAL;
	// ----- �`��� -----
	ID3D11Device*				m_pd3dDevice;			// �f�o�C�X
	ID3D11DeviceContext*		m_pDeviceContext;		// �f�o�C�X�R���e�L�X�g
	ID3D11InputLayout*			m_pVertexLayout;		// ���_���C�A�E�g
	ID3D11VertexShader*			m_pVertexShader;		// ���_�V�F�[�_
	ID3D11GeometryShader*		m_pGeometryShader;		// �W�I���g���[�V�F�[�_
	ID3D11PixelShader*			m_pPixelShader;			// �s�N�Z���V�F�[�_
	ID3D11Buffer*				m_pConstantBufferGeometory;	// �R���X�^���g�o�b�t�@(�W�I���g��)
	// ----- �`��֘A -----
	ID3D11Buffer*				m_pVertexBuffer;		// ���_�o�b�t�@
	// --------------------
public:
	CGrid(ID3D11Device*, ID3D11DeviceContext*);			// �R���X�g���N�^
	~CGrid();											// �f�X�g���N�^
	HRESULT Initialize();								// ����������
	void Draw();										// �`��
};
#endif	/* __GRID_H__ */