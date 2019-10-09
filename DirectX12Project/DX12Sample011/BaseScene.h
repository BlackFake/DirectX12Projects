// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __BASESCENE_H__
#define __BASESCENE_H__
// --------------- IncludeFile ---------------
#include "Global.h"
#include "SceneModule.h"
#include "SceneChanger.h"
#include "InputDevice.h"
// ---------------- �N���X�錾 ---------------
class CBaseScene : public CSceneModule
{
protected:
	CSceneChanger*	m_pSceneChanger;		// �V�[���`�F���W���[�̃|�C���^
											//(�V�[���`�F���W���[���p�������}�l�[�W���[�̃|�C���^)
public:
	CBaseScene(CSceneChanger*);
	virtual ~CBaseScene();
	virtual HRESULT Initialize() override;	// �������֐�
	virtual void Manage();					// ����֐�
	virtual void Render() override;			// �`��֐�
};
#endif /* __BASESCENE_H__ */