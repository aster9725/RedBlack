
// RedBlackDoc.cpp: CRedBlackDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RedBlack.h"
#endif

#include "RedBlackDoc.h"
#include "MainFrm.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRedBlackDoc

IMPLEMENT_DYNCREATE(CRedBlackDoc, CDocument)

BEGIN_MESSAGE_MAP(CRedBlackDoc, CDocument)
	ON_BN_CLICKED(IDC_BTN_ADEL, &CRedBlackDoc::OnBnClickedBtnAdel)
END_MESSAGE_MAP()


// CRedBlackDoc 생성/소멸

CRedBlackDoc::CRedBlackDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_RBTree.cnt = 0;
	m_RBTree.pRoot = { rb_get_nil(), };
}

CRedBlackDoc::~CRedBlackDoc()
{
}

BOOL CRedBlackDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CRedBlackDoc serialization

void CRedBlackDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CRedBlackDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CRedBlackDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CRedBlackDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRedBlackDoc 진단

#ifdef _DEBUG
void CRedBlackDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRedBlackDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// RBNODE 함수 정의

void rb_copyData(void* target, void* replacement)
{
	RBData* pTarget, * pReplacement;
	pTarget = rb_entry(target, RBData, rbt);
	pReplacement = rb_entry(replacement, RBData, rbt);
	pTarget->key = pReplacement->key;
	pTarget->pos = pReplacement->pos;
}

void rb_insert(struct rbtree* pTree, struct rbnode* pNewNode)
{
	struct rbnode** probe;
	RBData* pData = rb_entry(pNewNode, RBData, rbt);

	probe = &pTree->pRoot;

	while (*probe != rb_get_nil())
	{
		rb_set_parent(pNewNode, *probe);
		if (pData->key < rb_entry((*probe), RBData, rbt)->key)
			probe = &(*probe)->lft;
		else
			probe = &(*probe)->rgt;
	}
	*probe = pNewNode;
	pTree->cnt++;
	insert_fixup(pTree, pNewNode);
}

void* rb_search(struct rbtree* pTree, int data)
{
	struct rbnode* probe = pTree->pRoot;
	RBData* pData = nullptr;

	while (probe != rb_get_nil())
	{
		pData = rb_entry(probe, RBData, key);
		if (pData->key == data)
			break;
		else if (pData->key < data)
			probe = probe->rgt;
		else
			probe = probe->lft;
	}


	if (probe == rb_get_nil())
		return nullptr;
	else
		return pData;
}


// CRedBlackDoc 명령


void CRedBlackDoc::OnBnClickedBtnAdel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	int data;
	
	pFrame->GetDialogBarPtr()->GetDlgItemTextW(IDC_EDIT_NVALUE, str);
	data = _ttoi(str);
	str.Format(_T("errno : %d | data : %d"), errno, data);

	if (pFrame->IsModeInsert())
	{
		RBData* pNewNode = static_cast<RBData*>(rb_search(&m_RBTree, data));

		if (pNewNode != nullptr)
			MessageBox(pFrame->GetSafeHwnd(), L"중복된 데이터가 존재합니다",L"Insert Error", MB_OK);
		else
		{
			pNewNode = new RBData;
			pNewNode->key = data;
			pNewNode->rbt.lft = pNewNode->rbt.rgt = rb_get_nil();
			pNewNode->rbt.pParentColor = (unsigned long)rb_get_nil();
			rb_insert(&m_RBTree, &pNewNode->rbt);
			pFrame->GetDialogBarPtr()->SetDlgItemTextW(IDC_EDIT_NVALUE, L"");
		}
	}
	else
	{
		RBData* pNewNode;

		if (pNewNode = static_cast<RBData*>(rb_search(&m_RBTree, data)))
		{
			pNewNode = rb_entry(rb_delete(&m_RBTree, &pNewNode->rbt, rb_copyData), RBData, rbt);
			pFrame->GetDialogBarPtr()->SetDlgItemTextW(IDC_EDIT_NVALUE, L"");
		}
		else
			MessageBox(pFrame->GetSafeHwnd(), L"데이터가 존재하지 않습니다.", L"Delete Error", MB_OK);
	}
	
	UpdateAllViews(NULL);
	//SetModifiedFlag();
}
