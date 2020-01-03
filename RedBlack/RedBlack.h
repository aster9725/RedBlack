
// RedBlack.h: RedBlack 애플리케이션의 기본 헤더 파일
//
#pragma once
#pragma comment(lib, "gdiplus")

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include <gdiplus.h>
using namespace Gdiplus;


// CRedBlackApp:
// 이 클래스의 구현에 대해서는 RedBlack.cpp을(를) 참조하세요.
//

class CRedBlackApp : public CWinApp
{
public:
	CRedBlackApp() noexcept;
	ULONG_PTR	m_gdiplusToken;

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CRedBlackApp theApp;
