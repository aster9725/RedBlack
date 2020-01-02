
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

// RBTree 함수 정의	START

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

RBData* rb_search(struct rbtree* pTree, int data)
{
	struct rbnode* probe = pTree->pRoot;
	RBData* pData = nullptr;

	while (probe != rb_get_nil())
	{
		pData = rb_entry(probe, RBData, rbt);
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

// RBTree 함수 정의 END

// CRedBlackDoc 생성/소멸

CRedBlackDoc::CRedBlackDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_RBTree.cnt = 0;
	m_RBTree.pRoot = { rb_get_nil(), };
}

CRedBlackDoc::~CRedBlackDoc()
{
	ClearRBTree();
}

void CRedBlackDoc::ClearRBTree()
{
	while (m_RBTree.cnt > 0)
	{
		delete rb_entry(rb_delete(&m_RBTree, m_RBTree.pRoot, rb_copyData), RBData, rbt);
	}
	UpdateAllViews(NULL);
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


// CRedBlackDoc 명령


void CRedBlackDoc::OnBnClickedBtnAdel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str, message;
	int data = 0, index = 0;
	BOOL resultOnly;
	
	//pFrame->GetDialogBarPtr()->GetDlgItemTextW(IDC_EDIT_NVALUE, str);
	pFrame->GetDlgItem(IDD_DIALOGBAR)->GetDlgItemTextW(IDC_EDIT_NVALUE, str);
	resultOnly = pFrame->GetToolBarPtr()->IsDlgButtonChecked(ID_SHOW_RESULT_ONLY);

	
	//for (; index != -1; data = _ttoi(str.Tokenize(L",", index)))
	do
	{
		data = _ttoi(str.Tokenize(L",", index));
		RBData* pNewNode = rb_search(&m_RBTree, data);
		if (index == -1)
			break;
		if (pFrame->IsModeInsert())
		{


			if (pNewNode != NULL)
			{
				message.Format(L"%s (%d)", L"중복된 데이터가 존재합니다", data);
				MessageBox(pFrame->GetSafeHwnd(), message, L"Insert Error", MB_OK);
				pFrame->GetDialogBarPtr()->GetDlgItem(IDC_EDIT_NVALUE)->SetFocus();
			}
			else
			{
				pNewNode = new RBData;
				pNewNode->key = data;
				pNewNode->rbt.lft = pNewNode->rbt.rgt = rb_get_nil();
				pNewNode->rbt.pParentColor = (UINT64)rb_get_nil();
				pNewNode->pos.X = pNewNode->pos.Y = 0;
				pNewNode->pos.Width = pNewNode->pos.Height = 30;
				rb_insert(&m_RBTree, &pNewNode->rbt);
			}
		}
		else
		{
			if (pNewNode != NULL)
			{
				pNewNode = rb_entry(rb_delete(&m_RBTree, &pNewNode->rbt, rb_copyData), RBData, rbt);
				delete pNewNode;
			}
			else
			{
				message.Format(L"%s (%d)", L"데이터가 존재하지 않습니다", data);
				MessageBox(pFrame->GetSafeHwnd(), message, L"Delete Error", MB_OK);
				pFrame->GetDialogBarPtr()->GetDlgItem(IDC_EDIT_NVALUE)->SetFocus();
			}
		}
		str.Delete(0, index);
		pFrame->GetDialogBarPtr()->SetDlgItemTextW(IDC_EDIT_NVALUE, str);
		pFrame->lastStr = str;
		index = 0;
	} while (resultOnly);
	UpdateAllViews(NULL);
	//SetModifiedFlag();
}
