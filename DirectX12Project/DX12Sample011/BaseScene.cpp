// --------------- IncludeFile ---------------
#include "BaseScene.h"

// �R���X�g���N�^
CBaseScene::CBaseScene(CSceneChanger* pSceneChanger)
{
	m_pSceneChanger = pSceneChanger;
}

// �f�X�g���N�^
CBaseScene::~CBaseScene()
{

}

// ������
HRESULT CBaseScene::Initialize()
{
	return S_OK;
}

// ����֐�
void CBaseScene::Manage()
{

}

// �`��֐�
void CBaseScene::Render()
{

}