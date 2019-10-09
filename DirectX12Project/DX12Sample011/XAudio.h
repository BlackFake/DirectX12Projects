// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __XAUDIO_H__
#define __XAUDIO_H__
// --------------- IncludeFile ---------------
#include "Global.h"
#include "WaveFileReader.h"
#include <xaudio2.h>
#include <mmsystem.h>
// --------------- LibraryFile ---------------
#pragma comment(lib, "xaudio2.lib")
// ---------------- �N���X�錾 ---------------
class CXAudio
{
private:
	IXAudio2*				m_pXAudio;
	IXAudio2MasteringVoice* m_pMasteringVoice;
	IXAudio2SourceVoice*	m_pSourceVoice;
	WAVEFORMATEX*			m_pWaveFormat;
	WaveFileReader*			m_pReader;
	std::vector< BYTE >		m_Primary;
	std::vector< BYTE >		m_Secondary;
	std::size_t				m_NextFirstSample;
	std::size_t				m_SubmitCount;
	XAUDIO2_BUFFER			m_Buffer;
	BOOL					m_Playing;
	BOOL					m_LoopFlag;
public:
	CXAudio();
	~CXAudio();
	HRESULT Create();				// ����
	void Release();					// �������
	BOOL LoadWave(const TCHAR*);	// WAVE�t�@�C���̓ǂݍ���
	void Play(BOOL loop = FALSE);	// �ǂݍ���WAVE�t�@�C���̍Đ�
	void PlayNoReset(BOOL loop = FALSE);// WAVE�t�@�C���̍Đ�(�I���܂ōĐ�����Ȃ�)
	void Pause();					// �ꎞ��~
	void Stop();					// �Đ����t�@�C���̒�~
	void UpDate();					// �Đ��X�V����
	BOOL IsPlay();					// �Đ����̗L��
	void SetPlaying(BOOL Flag) {
		m_Playing = Flag;
	}
};

typedef CXAudio WAVE;
#endif /* __XAUDIO_H__ */