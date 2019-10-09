// --------------- IncludeFile ---------------
#include "XAudio.h"

// �R���X�g���N�^
CXAudio::CXAudio() : m_pXAudio(nullptr), m_pMasteringVoice(nullptr)
, m_pSourceVoice(nullptr), m_pWaveFormat(nullptr), m_pReader(nullptr)
{
	m_Primary = std::vector<BYTE>();
	m_Secondary = std::vector<BYTE>();
	m_NextFirstSample = 0;
	m_SubmitCount = 0;
	m_Buffer = XAUDIO2_BUFFER();
	m_Playing = FALSE;
	m_LoopFlag = FALSE;
}

// �f�X�g���N�^
CXAudio::~CXAudio()
{
	Release();
}

//	�֐���:Create					����
//	������:
//	�߂�l:							�Ȃ�
HRESULT CXAudio::Create()
{
	TCHAR Buffer[MAX_PATH];

	UINT32 flags = 0;
	if (FAILED(XAudio2Create(&m_pXAudio, flags)))
	{
		if (g_DebugFlag)
		{
			OutputDebugString(_T("XAudio2Create()�Ɏ��s���܂����B\n"));
		}
		return E_FAIL;
	}

	IXAudio2MasteringVoice* pMasteringVoice = nullptr;
	if (FAILED(m_pXAudio->CreateMasteringVoice(
		&pMasteringVoice, 2)))
	{
		if (g_DebugFlag)
		{
			OutputDebugString(_T("CreateMasteringVoice()�Ɏ��s���܂����B\n"));
		}
		return E_FAIL;
	}
	m_pMasteringVoice = pMasteringVoice;

	if (g_DebugFlag)
	{
		sprintf_s(Buffer, _T("m_pMasteringVoice (0x%p)\n"), m_pMasteringVoice);
		OutputDebugString(Buffer);
		XAUDIO2_VOICE_DETAILS details;
		m_pMasteringVoice->GetVoiceDetails(&details);
		sprintf_s(Buffer, _T("�`�����l����: %u\n"), details.InputChannels);
		OutputDebugString(Buffer);
		sprintf_s(Buffer, _T("�T���v�����O���[�g: %uHz\n"), details.InputSampleRate);
		OutputDebugString(Buffer);
	}

	m_pReader = new WaveFileReader();

	return S_OK;
}

//	�֐���:Release					�������
//	������:
//	�߂�l:							�Ȃ�
void CXAudio::Release()
{
	SAFE_DELETE(m_pReader);

	if (m_pSourceVoice)
	{
		m_pSourceVoice->DestroyVoice();
		m_pSourceVoice = nullptr;
	}

	if (m_pMasteringVoice)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	SafeRelease(&m_pXAudio);
}

//	�֐���:LoadWave					����֐�
//	������:pInputDevice				���̓f�o�C�X�̃A�h���X
//		  :pSoundManager			�T�E���h�}�l�[�W���[�̃A�h���X
//	�߂�l:							TRUE: �ǂݍ��ݐ����AFALSE: �ǂݍ��ݎ��s
BOOL CXAudio::LoadWave(const TCHAR* pFileName)
{
	TCHAR Buffer[MAX_PATH];

	if (!pFileName || !m_pReader->Open(pFileName))
	{
		if (g_DebugFlag)
		{
			OutputDebugString(_T("WAVE�t�@�C����Open()�Ɏ��s���܂����B\n"));
		}
		return FALSE;
	}

	const WAVEFORMATEX* pWaveFormat = m_pReader->GetWaveFormat();
	if (!pWaveFormat)
	{
		if (g_DebugFlag)
		{
			OutputDebugString(_T("GetWaveFormat()�Ɏ��s���܂����B\n"));
		}
		return FALSE;
	}

	m_pWaveFormat = const_cast<WAVEFORMATEX*>(pWaveFormat);

	if (FAILED(m_pXAudio->CreateSourceVoice(&m_pSourceVoice, m_pWaveFormat)))
	{
		if (g_DebugFlag)
		{
			OutputDebugString(_T("CreateSourceVoice()�Ɏ��s���܂����B\n"));
		}
		return FALSE;
	}

	if (g_DebugFlag)
	{
		sprintf_s(Buffer, _T("m_pSourceVoice (0x%p)\n"), m_pSourceVoice);
		OutputDebugString(Buffer);
		sprintf_s(Buffer, _T("�`�����l���� %u\n"), m_pWaveFormat->nChannels);
		OutputDebugString(Buffer);
		sprintf_s(Buffer, _T("�T���v�����O���[�g %uHz\n"), m_pWaveFormat->nSamplesPerSec);
		OutputDebugString(Buffer);
		sprintf_s(Buffer, _T("�ʎq���r�b�g�� %u\n"), m_pWaveFormat->wBitsPerSample);
		OutputDebugString(Buffer);
	}

	std::size_t NextFirstSample = 0;
	std::size_t SubmitCount = 0;

	// �v���C�}���o�b�t�@
	m_Primary.resize(m_pWaveFormat->nAvgBytesPerSec * 3);
	if (NextFirstSample < m_pReader->GetSamples())
	{
		std::size_t readSamples = m_pReader->ReadRaw(NextFirstSample, m_pWaveFormat->nSamplesPerSec * 3, &(m_Primary[0]));
		if (readSamples > 0)
		{
			XAUDIO2_BUFFER xb = { 0 };
			xb.Flags = NextFirstSample + readSamples >= m_pReader->GetSamples() ? XAUDIO2_END_OF_STREAM : 0;
			xb.AudioBytes = readSamples * m_pWaveFormat->nBlockAlign;
			xb.pAudioData = &(m_Primary[0]);

			m_pSourceVoice->SubmitSourceBuffer(&xb);
			m_Buffer = xb;

			if (g_DebugFlag)
			{
				sprintf_s(Buffer, _T("Read: 0���%u-----%u���%u\n"), 
					NextFirstSample, 
					(NextFirstSample + readSamples - 1), 
					(m_pReader->GetSamples() - 1));
				OutputDebugString(Buffer);
			}
			NextFirstSample += readSamples;
			++SubmitCount;
		}
	}

	// �Z�J���_���o�b�t�@
	m_Secondary.resize(m_pWaveFormat->nAvgBytesPerSec * 3);

	m_NextFirstSample = NextFirstSample;
	m_SubmitCount = SubmitCount;

	return TRUE;
}

//	�֐���:Play						�ǂݍ���WAVE�t�@�C���̍Đ�
//	������:loop						���[�v
//	�߂�l:							�Ȃ�
void CXAudio::Play(BOOL loop)
{
	Stop();
	m_pSourceVoice->Start();
	m_Playing = TRUE;
	m_LoopFlag = loop;
}

//	�֐���:PlayNoReset				WAVE�t�@�C���̍Đ�(�I���܂ōĐ��ł��Ȃ�)
//	������:loop						���[�v
//	�߂�l:							�Ȃ�
void CXAudio::PlayNoReset(BOOL loop)
{
	if (m_pSourceVoice && !m_Playing)
	{
		Play(loop);
	}
}

//	�֐���:Pause					�ꎞ��~
//	������:
//	�߂�l:							�Ȃ�
void CXAudio::Pause()
{
	m_pSourceVoice->Stop();
	m_Playing = FALSE;
}

//	�֐���:Stop						�Đ����t�@�C���̒�~
//	������:
//	�߂�l:							�Ȃ�
void CXAudio::Stop()
{
	m_pSourceVoice->Stop();
	m_pSourceVoice->FlushSourceBuffers();
	XAUDIO2_BUFFER xb = m_Buffer;
	m_pSourceVoice->SubmitSourceBuffer(&xb);
	m_Playing = FALSE;
}

//	�֐���:UpDate					�Đ��X�V����
//	������:
//	�߂�l:							�Ȃ�
void CXAudio::UpDate()
{
	if (m_Playing)
	{
		XAUDIO2_VOICE_STATE state;
		m_pSourceVoice->GetState(&state);

		if (state.BuffersQueued == 0 && m_NextFirstSample >= m_pReader->GetSamples())
		{
			// ���ׂčĐ����I����Ă���
			Stop();
			if (m_LoopFlag)	// ���[�v��TRUE�ɂȂ��Ă���Ȃ�
			{
				Play(m_LoopFlag);	// ���[�v�Đ�
			}
		}
		else if (state.BuffersQueued < 2)
		{
			// �L���[�Ƀo�b�t�@��ǉ�
			std::vector< BYTE >& buffer = m_SubmitCount & 1 ? m_Secondary : m_Primary;

			std::size_t readSamples = m_pReader->ReadRaw(m_NextFirstSample, m_pWaveFormat->nSamplesPerSec * 3, &(buffer[0]));
			if (readSamples > 0)
			{
				XAUDIO2_BUFFER xb = m_Buffer;
				m_pSourceVoice->SubmitSourceBuffer(&xb);
				m_NextFirstSample += readSamples;
				++m_SubmitCount;
			}
		}
	}
}

//	�֐���:IsPlay					�Đ����̗L��
//	������:
//	�߂�l:							TRUE: �Đ����AFALSE: �Đ����Ă��Ȃ�
BOOL CXAudio::IsPlay()
{
	return m_Playing;
}