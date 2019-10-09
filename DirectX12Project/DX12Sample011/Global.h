// Programing by Hyoga.Fukuno
// Date: 2019/ 10/ 3
#ifndef __GLOBAL_H__
#define __GLOBAL_H__
// ---------------- Define�錾 ---------------
#define UM_CHANGEWINDOWMODE	WM_USER + 101	// �\�����[�h�̐ݒ�
#define SAFE_DELETE(p)		{ if(p) { delete (p);		(p)=nullptr; } }	// �I�u�W�F�N�g����}�N��
#define SAFE_DELETE_ARRAY(p){ if(p) { delete [](p);		(p)=nullptr; } }	// �I�u�W�F�N�g����}�N��
#define SAFE_RELEASE(p)		{ if(p) { (p)->Release();	(p)=nullptr; } }	// �I�u�W�F�N�g����}�N��
#define STRCMP_SUCCESS 0	// strcmp�̐����}�N��
// ---------------- IncludeFile --------------
#include <windows.h>
#include <Mmsystem.h>
#include <commctrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#include <string>
#include <atlbase.h>            // ATL
#include <wincodec.h>           // WIC 
#include <locale.h>				// ���P�[��
// ---------------- LibraryFile --------------
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"windowscodecs.lib")
// ---------------- Template -----------------
template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}
// ---------------- namespace ----------------
using namespace DirectX;
// ---------------- �񋓑̐錾 ---------------
enum class eScenes
{
	eScene_None,
	eScene_Title,
	eScene_Game
};
// ----------------- �ϐ��錾 ----------------
extern  bool g_DebugFlag;			// �f�o�b�O���[�h�̗L��
extern SIZE g_WindowSize;			// �E�B���h�E�T�C�Y
extern LPCWSTR g_WindowTitle;		// �A�v���P�[�V������
// ---------------- �\���̐錾 ---------------
// "d3dx9math.inl"���Q�l
typedef struct VECTOR4 : public XMFLOAT4
{
public:
	VECTOR4() {};
	VECTOR4(CONST FLOAT *pf)
	{
		x = pf[0];
		y = pf[1];
		z = pf[2];
		w = pf[3];
	}
	VECTOR4(CONST VECTOR4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	VECTOR4(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	// �^�ϊ�
	operator FLOAT* ()
	{
		return (FLOAT *)&x;
	}
	operator CONST FLOAT* () const
	{
		return (CONST FLOAT *) &x;
	}

	// ������Z�q
	VECTOR4& operator += (CONST VECTOR4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	VECTOR4& operator -= (CONST VECTOR4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	VECTOR4& operator *= (FLOAT f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}
	VECTOR4& operator /= (FLOAT f)
	{
		FLOAT fInv = 1.0f / f;
		x *= fInv;
		y *= fInv;
		z *= fInv;
		w *= fInv;
		return *this;
	}

	// �P�����Z�q
	void operator = (CONST XMVECTOR& v)
	{
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
		w = v.m128_f32[3];
	}
	void operator = (CONST XMFLOAT4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	VECTOR4 operator + () const
	{
		return *this;
	}
	VECTOR4 operator - () const
	{
		return VECTOR4(-x, -y, -z, -w);
	}

	// 2 �����Z�q
	VECTOR4 operator + (CONST VECTOR4& v) const
	{
		return VECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	VECTOR4 operator - (CONST VECTOR4& v) const
	{
		return VECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	VECTOR4 operator * (FLOAT f) const
	{
		return VECTOR4(x * f, y * f, z * f, w * f);
	}
	VECTOR4 operator / (FLOAT f) const
	{
		FLOAT fInv = 1.0f / f;
		return VECTOR4(x * fInv, y * fInv, z * fInv, w * fInv);
	}

	friend VECTOR4 operator * (FLOAT f, CONST struct VECTOR4& v)
	{
		return VECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);
	}

	BOOL operator == (CONST VECTOR4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	BOOL operator != (CONST VECTOR4& v) const
	{
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	XMVECTOR GetXMVector()
	{
		XMFLOAT4 v;
		v.x = x;
		v.y = y;
		v.z = z;
		v.w = w;
		return XMLoadFloat4(&v);
	}
	void SetXMVector(CONST XMFLOAT4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

} VECTOR4;

typedef struct _PLANE_
{
public:
	FLOAT a, b, c, d;
}PLANE;

typedef struct _MESHVERTEX_
{
	VECTOR4	vPosition;				// ���W
	VECTOR4	vNormal;				// �@��
	XMFLOAT2 vTextureUV;			// �e�N�X�`���[UV�l
}MESHVERTEX;
#endif	/* __GLOBAL_H__ */