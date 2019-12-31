
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once

class CMainToolBar : public CToolBar
{
public:
	CButton				m_wndCheckBox;
};

class CMainFrame : public CFrameWnd
{
private:
	bool m_bModeFlag;
	bool m_bSkipAnimation;
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	CString lastStr = L"";	// digit & , Only CString value
// 작업입니다.
public:
	CDialogBar* GetDialogBarPtr() { return &m_wndDialogBar; }
	bool IsModeInsert() { return m_bModeFlag; }
	bool IsSkipAnimation() { return m_bSkipAnimation; }
// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMainToolBar      m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CDialogBar		  m_wndDialogBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddNode();
	afx_msg void OnDeleteNode();
	afx_msg void OnDeleteTree();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnUpdateEditNvalue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


