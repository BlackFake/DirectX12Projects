// -------------------- IncludeFile -----------------
#include "Camera.h"
CCamera* CCamera::m_pInstance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^
CCamera::CCamera()
{

}
// �f�X�g���N�^
CCamera::~CCamera()
{

}
// ������
void CCamera::Initialize()
{
	m_vViewPosition = VECTOR4(0.0f, 0.0f, 0.0f, 0.0f);		// �J�����̈ʒu
	m_vViewAtPosition = VECTOR4(0.0f, 0.0f, 0.0f, 0.0f);	// �J�����̏œ_
	m_vViewAngle = VECTOR4(0.175f * XM_PI, 0.0f, 0.0f, 0.0f);// �J�����̃A���O��
	m_Distance = 3.0f;										// �{��(�J�������猴�_�܂ł̋���) 
	m_vLightPosition = VECTOR4(1.0f, 2.0f, -1.0f, 0.0f);	// ���C�g���W
	m_vDirectionalLight = VECTOR4(-1.0f, -2.0f, 1.0f, 0.0f);// ���C�g����(�P�ʉ�)
	XMVECTOR vDirectionalLight = m_vDirectionalLight.GetXMVector();
	vDirectionalLight = XMVector4Normalize(vDirectionalLight);
	m_vDirectionalLight = vDirectionalLight;
}
// �J�����̔z�u
void CCamera::SetCamera()
{
	XMMATRIX matView;					// ���_�s��
	XMMATRIX matProjection;				// ���e�ϊ��s��
	XMMATRIX matRotation;				// �I�u�W�F�N�g�s��̎���(��])
	XMVECTOR vBaseViewPosition = XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);	// �J�����̊�ʒu
	XMVECTOR vViewUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vViewAtPosition = XMVectorSet(m_vViewAtPosition.x, m_vViewAtPosition.y, m_vViewAtPosition.z, m_vViewAtPosition.w);

	// 3D�x�N�g���̃X�P�[�����O
	vBaseViewPosition = XMVectorScale(
		vBaseViewPosition,	// �����̊�ɂȂ�XMVector�\���̂��w��
		m_Distance);		// �X�P�[�����O�l���w��

	// ���[�A�s�b�`�A���[�����w�肵�čs����쐬
	matRotation = XMMatrixRotationRollPitchYaw(
		m_vViewAngle.x,	// x���̃s�b�`(���W�A���P��)���w��
		m_vViewAngle.y,	// y���̃��[(���W�A���P��)���w��
		m_vViewAngle.z);// z���̃��[��(���W�A���P��)���w��

	// 3D�x�N�g�����s��ŕϊ�
	vBaseViewPosition = XMVector4Transform(
		vBaseViewPosition,	// �ϊ��O�̃x�N�g�����w��
		matRotation);		// �����̊�ɂȂ�s����w��

	// 3D�x�N�g�����s��ŕϊ�
	vViewUp = XMVector4Transform(
		vViewUp,			// �ϊ��O�̃x�N�g�����w��
		matRotation);		// �����̊�ɂȂ�s����w��

	vBaseViewPosition += vViewAtPosition;

	// ������W�n�r���[�s����쐬
	matView = XMMatrixLookAtLH(
		vBaseViewPosition,	// ���_���`����XMVECTOR�\���̂��w��
		vViewAtPosition,	// �œ_���`����XMVECTOR�\���̂��w��
		vViewUp);			// �J�����g���[���h�̏���A��ʂɂ�[0, 1, 0]���`����XMVECTOR�\���̂��w��
	m_vViewPosition = vBaseViewPosition;
	XMStoreFloat4x4(&m_matView, matView);

	D3D11_VIEWPORT Vp;
	UINT NumViewports;
	m_pDeviceContext->RSGetViewports(&NumViewports, nullptr);
	m_pDeviceContext->RSGetViewports(&NumViewports, &Vp);
	// ����(FOV)�Ɋ�Â��āA������W�n�p�[�X�y�N�e�B�u�ˉe�s����쐬
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	// ����p���w��(���W�A���P��)(45�x���w��)
		Vp.Width / Vp.Height,		// �A�X�y�N�g����w��
		1.0f,						// �߂��̃r���[�v���[����z�l���w��
		1000.0f);					// �����̃r���[�v���[����z�l���w��
	XMStoreFloat4x4(&m_matProjection, matProjection);
}
// �쐬
void CCamera::Create(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_pInstance)
		m_pInstance = new CCamera();

	m_pInstance->GetInstance()->m_pDeviceContext = pDeviceContext;
	m_pInstance->GetInstance()->Initialize();
}
// �j��
void CCamera::Destroy()
{
	SAFE_DELETE(m_pInstance);
}