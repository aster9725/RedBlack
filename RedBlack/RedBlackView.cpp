
// RedBlackView.cpp: CRedBlackView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RedBlack.h"
#endif

#include "RedBlackDoc.h"
#include "RedBlackView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRedBlackView

IMPLEMENT_DYNCREATE(CRedBlackView, CView)

BEGIN_MESSAGE_MAP(CRedBlackView, CView)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


#define RECT_COORDINATE(rect)	rect.left, rect.top, rect.Width(), rect.Height()

// CRedBlackView 생성/소멸

void CRedBlackView::drawTree(CDC* pDC)
{
	CRedBlackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	Graphics canvas(pDC->m_hDC);
	canvas.SetSmoothingMode(SmoothingModeHighQuality);
	CRect clientRect;
	GetClientRect(&clientRect);
	RectF canvasRect(clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height());

	Gdiplus::Font font(L"Times New Roman", 12, FontStyleBold, UnitPixel);
	StringFormat strFormat;
	strFormat.SetLineAlignment(StringAlignmentCenter);
	strFormat.SetAlignment(StringAlignmentCenter);

	Pen pen(Color(255, 0, 0, 0), 1);
	SolidBrush redBrush(Color(255, 255, 0, 0));
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	SolidBrush whiteBrush(Color(255, 255, 255, 255));

	Point points[3] = { Point(0,0), Point(0,0), Point(0,0) };

	DRAWTOOLS tools = { canvas, canvasRect, font, strFormat, pen, redBrush, blackBrush, whiteBrush, points };

	canvasRect.X += NODE_MARGIN / 2;
	canvasRect.Y += NODE_MARGIN / 2;
	canvasRect.Width -= NODE_MARGIN;
	canvasRect.Height -= NODE_MARGIN;

	struct rbtree* pTree = pDoc->GetRBTree();
	if (pTree->pRoot == rb_get_nil())
		return;

	int depth, posX;
	depth = 0;
	posX = canvasRect.Y;
	drawNode(tools, pTree->pRoot, depth, posX);
	drawLine(tools, pTree->pRoot);
}

void CRedBlackView::drawNode(DRAWTOOLS& tools, rbnode* pNode, int& depth, int& posX)
{
	if (pNode->lft != rb_get_nil())
		drawNode(tools, pNode->lft, ++depth, posX);

	// Store node position refered from (0,0)
	// Swap X,Y corrdinate to transform tree
	RBData* pData = rb_entry(pNode, RBData, rbt);
	pData->pos.X = posX;
	pData->pos.Y = tools.canvasRect.Y + depth * m_nNodeSize;

	// Calculate real position of node to draw
	RectF realRect;
	realRect.X = m_nWndOffset.X + pData->pos.X + NODE_MARGIN/2;
	realRect.Y = m_nWndOffset.Y + pData->pos.Y + NODE_MARGIN/2;
	realRect.Width = realRect.Height = m_nNodeSize - NODE_MARGIN;
	
	// Draw node circle
	tools.canvas.FillEllipse(rb_get_color(pNode) == RED ? &tools.redBrush : &tools.blackBrush, realRect);

	// Draw node data on the circle
	CString number;
	number.Format(L"%d", pData->key);
	tools.canvas.DrawString(number, number.GetLength(), &tools.font, realRect, &tools.strFormat, 
		rb_get_color(pNode) == RED ? &tools.blackBrush : &tools.whiteBrush);

	// Go to next Line
	posX += m_nNodeSize;

	if (pNode->rgt != rb_get_nil())
		drawNode(tools, pNode->rgt, ++depth, posX);
	--depth;
}

void CRedBlackView::drawLine(DRAWTOOLS& tools, rbnode* pNode)
{

	if (pNode->lft != rb_get_nil())
		drawLine(tools, pNode->lft);

	// child point
	RBData* pData = rb_entry(pNode, RBData, rbt);
	tools.points[0].X = m_nWndOffset.X + pData->pos.X + m_nNodeSize / 2;
	tools.points[0].Y = m_nWndOffset.Y + pData->pos.Y + NODE_MARGIN / 2;
	// parent point
	pData = rb_entry(rb_get_parent(pNode), RBData, rbt);
	tools.points[2].X = m_nWndOffset.X + pData->pos.X + m_nNodeSize / 2;
	tools.points[2].Y = m_nWndOffset.Y + pData->pos.Y + m_nNodeSize - NODE_MARGIN / 2;
	// middle point
	tools.points[1].X = (tools.points[0].X + tools.points[2].X) / 2;
	tools.points[1].Y = (tools.points[0].Y + tools.points[2].Y) / 2;
	
	// Draw line if pNode is not root node
	if(rb_get_parent(pNode) != rb_get_nil())
		tools.canvas.DrawCurve(&tools.pen, tools.points, 3, 0.3f);

	if (pNode->rgt != rb_get_nil())
		drawLine(tools, pNode->rgt);
}

CRedBlackView::CRedBlackView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_nNodeSize = DEF_NODE_SIZE;
	m_nWndOffset.X = m_nWndOffset.Y = 0;
	m_bMoveFlag = FALSE;
}

void CRedBlackView::ResetWndOffset()
{
	m_nWndOffset.X = m_nWndOffset.Y = 0;
}

CRedBlackView::~CRedBlackView()
{
}

BOOL CRedBlackView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	
	return CView::PreCreateWindow(cs);
}

// CRedBlackView 그리기


void CRedBlackView::OnDraw(CDC* pDC)
{
	CRedBlackDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

}


// CRedBlackView 진단

#ifdef _DEBUG
void CRedBlackView::AssertValid() const
{
	CView::AssertValid();
}

void CRedBlackView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRedBlackDoc* CRedBlackView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRedBlackDoc)));
	return (CRedBlackDoc*)m_pDocument;
}
#endif //_DEBUG


// CRedBlackView 메시지 처리기


BOOL CRedBlackView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags & MK_CONTROL)
	{
		if (zDelta > 0) // 확대
		{
			m_nNodeSize += 10;
			if (m_nNodeSize > NODE_SIZE_MAX)
				m_nNodeSize = NODE_SIZE_MAX;
		}
		else
		{
			m_nNodeSize -= 10;
			if (m_nNodeSize < NODE_SIZE_MIN)
				m_nNodeSize = NODE_SIZE_MIN;
		}
		Invalidate();
		return true;
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CRedBlackView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ((nFlags & MK_LBUTTON) && (nFlags & MK_CONTROL))
	{
		m_nWndOffset.X += point.x - m_nMousePoint.x;
		m_nWndOffset.Y += point.y - m_nMousePoint.y;
		m_nMousePoint = point;
		Invalidate();
	}
	CView::OnMouseMove(nFlags, point);
}


void CRedBlackView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCapture();
	m_nMousePoint = point;
	CView::OnLButtonDown(nFlags, point);
}


void CRedBlackView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}


void CRedBlackView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags & MK_CONTROL) 
	{
		m_bMoveFlag = TRUE;
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_HAND)));
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CRedBlackView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags & MK_CONTROL)
		m_bMoveFlag = FALSE;

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CRedBlackView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bMoveFlag)
		return TRUE;
	return CView::OnSetCursor(pWnd, nHitTest, message);
}


void CRedBlackView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.

	// Double Buffering

	CRect rect;
	GetClientRect(rect);

	CDC MemDC;
	CBitmap* pOldBitmap;
	CBitmap bmp;

	MemDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pOldBitmap = (CBitmap*)MemDC.SelectObject(&bmp);
	MemDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);

	drawTree(&MemDC);
	//drawTree(&dc);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();
}


BOOL CRedBlackView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

