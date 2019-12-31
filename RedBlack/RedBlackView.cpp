
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
END_MESSAGE_MAP()


#define RECT_COORDINATE(rect)	rect.left, rect.top, rect.Width(), rect.Height()

// CRedBlackView 생성/소멸

int CRedBlackView::getTreeMaxDepth(struct rbnode* pNode)
{
	if (pNode == rb_get_nil())
		return -1;
	int lft = getTreeMaxDepth(pNode->lft);
	int rgt = getTreeMaxDepth(pNode->rgt);
	return lft > rgt ? lft + 1 : rgt + 1;
}

void CRedBlackView::drawTree(DRAWTOOLS& tools, rbnode* pNode, int& depth, int& posY)
{
	if (pNode->lft != rb_get_nil())
		drawTree(tools, pNode->lft, ++depth, posY);

	RBData* pData = rb_entry(pNode, RBData, rbt);
	pData->pos.Y = tools.canvasRect.X + depth * NODE_SIZE;
	pData->pos.X = posY;
	pData->pos.Width = NODE_SIZE;
	pData->pos.Height = NODE_SIZE;

	RectF realRect;
	realRect.X = pData->pos.X + 5;
	realRect.Y = pData->pos.Y + 5;
	realRect.Height = pData->pos.Height - 5;
	realRect.Width = pData->pos.Width - 5;

	tools.canvas.FillEllipse(rb_get_color(pNode) == RED ? &tools.redBrush : &tools.blackBrush, realRect);
	CString number;
	number.Format(L"%d", pData->key);
	tools.canvas.DrawString(number, number.GetLength(), &tools.font, pData->pos, &tools.strFormat, rb_get_color(pNode) == RED ? &tools.blackBrush : &tools.whiteBrush);

	posY += NODE_SIZE;

	if (pNode->rgt != rb_get_nil())
	{
		drawTree(tools, pNode->rgt, ++depth, posY);
	}
	--depth;
}

void CRedBlackView::drawLine(DRAWTOOLS& tools, rbnode* pNode)
{

	if (pNode->lft != rb_get_nil())
		drawLine(tools, pNode->lft);

	RBData* pData = rb_entry(pNode, RBData, rbt);
	tools.points[0].X = pData->pos.X + pData->pos.Width / 2;
	tools.points[0].Y = pData->pos.Y + 5;
	pData = rb_entry(rb_get_parent(pNode), RBData, rbt);
	tools.points[2].X = pData->pos.X + pData->pos.Width / 2;
	tools.points[2].Y = pData->pos.Y + pData->pos.Height;
	tools.points[1].X = (tools.points[0].X + tools.points[2].X) / 2;
	tools.points[1].Y = (tools.points[0].Y + tools.points[2].Y) / 2;
	if(rb_get_parent(pNode) != rb_get_nil())
		tools.canvas.DrawCurve(&tools.pen, tools.points, 3, 1.0f);

	if (pNode->rgt != rb_get_nil())
		drawLine(tools, pNode->rgt);
}

CRedBlackView::CRedBlackView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
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

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	pDC->SetROP2(R2_XORPEN);

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
	
	canvasRect.X += NODE_SIZE / 2;
	canvasRect.Y += NODE_SIZE / 2;
	canvasRect.Width -= NODE_SIZE / 2;
	canvasRect.Height -= NODE_SIZE / 2;

	struct rbtree* pTree = pDoc->GetRBTree();
	if (pTree->pRoot == rb_get_nil())
		return;

	int depth, posY;
	depth = 0;
	posY = canvasRect.Y;
	drawTree(tools, pTree->pRoot, depth, posY);
	drawLine(tools, pTree->pRoot);
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
