// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once
// -------------------- Define�錾 -------------------
// -------------------- IncludeFile ------------------
#include "Global.h"
// -------------------- �N���X�錾 -------------------
class CCamera
{
private:
	static CCamera*				m_pInstance;			// �V���O���g��
	ID3D11DeviceContext*		m_pDeviceContext;		// �f�o�C�X�R���e�L�X�g
	VECTOR4						m_vViewPosition;		// �J�����̈ʒu
	VECTOR4						m_vViewAtPosition;		// �J�����̏œ_
	VECTOR4						m_vViewAngle;			// �J�����̃A���O��
	FLOAT						m_Distance;				// �{��(�J�������猴�_�܂ł̋���) 
	XMFLOAT4X4					m_matView;				// �r���[�s��
	XMFLOAT4X4					m_matProjection;		// �ˉe�s��
	VECTOR4						m_vDirectionalLight;	// ������
	VECTOR4						m_vLightPosition;		// �������ʒu
public:
	CCamera();											// �R���X�g���N�^
	~CCamera();											// �f�X�g���N�^
	void Initialize();									// ������
	void SetCamera();									// �J�����̔z�u
	static void Create(ID3D11DeviceContext*);
	static void Destroy();

	static CCamera* GetInstance()	{ return m_pInstance;		}
	FLOAT GetDistance()				{ return m_Distance;		}
	void SetDistance(FLOAT Value)	{ m_Distance = Value;		}
	XMVECTOR GetViewAngle()	{
		return m_vViewAngle.GetXMVector();
	}
	void SetViewAngle(XMVECTOR Value) {
		m_vViewAngle = Value;
	}
	void SetViewAtPosition(VECTOR4 Value) {
		m_vViewAtPosition = Value;	
	}
	XMMATRIX GetViewMatrix()		{ return XMLoadFloat4x4(&m_matView); }
	XMMATRIX GetProjectionMatrix(){ return XMLoadFloat4x4(&m_matProjection); }
	XMVECTOR GetDirectionalLight() {
		XMVECTOR r = m_vDirectionalLight.GetXMVector();
		return XMVector4Normalize(r);
	}
	XMVECTOR GetViewPosition()		{ return m_vViewPosition.GetXMVector();	}
	XMVECTOR GetLightPosition()		{ return m_vLightPosition.GetXMVector();	}
};
#endif	/* __CAMERA_H__ */