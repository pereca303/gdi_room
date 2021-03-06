
// FirstMFCAppView.cpp : implementation of the CFirstMFCAppView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FirstMFCApp.h"
#endif

#include "FirstMFCAppDoc.h"
#include "FirstMFCAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFirstMFCAppView

IMPLEMENT_DYNCREATE(CFirstMFCAppView, CView)

BEGIN_MESSAGE_MAP(CFirstMFCAppView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CFirstMFCAppView construction/destruction

CFirstMFCAppView::CFirstMFCAppView() noexcept {
	// TODO: add construction code here
	CString keyPath = CString("C:\\emfs\\Key.emf");
	this->key = GetEnhMetaFile(keyPath);

	CString bigHandPath = CString("C:\\emfs\\bigHand.emf");
	this->bigHand = GetEnhMetaFile(bigHandPath);

	CString smallHandPath = CString("C:\\emfs\\smallHand.emf");
	this->smallHand = GetEnhMetaFile(smallHandPath);

}

CFirstMFCAppView::~CFirstMFCAppView() {
	for (int i = 0; i < this->windowPoints; i++)
		delete this->windowFrame[i];
	delete windowFrame;
	for (int i = 0; i < this->cabinetPoints; i++)
		delete this->cabinetFront[i];
	delete this->cabinetFront;

	delete this->openedCabinetDoorRgn;
	delete this->closedCabinetDoorRgn;

	delete this->closedRoomDoorRgn;
	delete this->openedRoomDoorRgn;

}

BOOL CFirstMFCAppView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFirstMFCAppView drawing

void CFirstMFCAppView::OnDraw(CDC* pDC) {
	CFirstMFCAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect canvas;
	GetClientRect(&canvas);

	this->center.x = canvas.right / 2;
	this->center.y = canvas.bottom / 2;

	CPen* newPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(newPen);

	this->drawWals(pDC, canvas);
	this->drawKeyBox(pDC, canvas);
	if (!this->roomDoorOpened)
		this->drawClosedRoomDoor(pDC, canvas);
	this->drawCabinetWithoutDoor(pDC, canvas);

	if (this->cabinetDoorOpened)
		this->drawOpenCabinet(pDC, canvas);
	else
		this->drawClosedCabinet(pDC, canvas);
	this->drawWindow(pDC, canvas);
	this->drawCurtain(pDC, canvas);

	//this->testTransform(pDC, canvas);

	this->drawClock(pDC, canvas);
	if (this->roomDoorOpened)
		this->drawOpenRoomDoor(pDC, canvas);

	delete pDC->SelectObject(oldPen);
	// TODO: add draw code for native data here
}

void CFirstMFCAppView::drawWals(CDC* pdc, CRect canvas) {
	CBrush* frontWallBrush = new CBrush(RGB(255, 191, 127));
	CBrush* oldBrush = pdc->SelectObject(frontWallBrush);

	pdc->Rectangle(canvas.right*0.1, canvas.bottom*0.1, canvas.right*0.9, canvas.bottom*0.9);

	CBrush* sideWallsBrush = new CBrush(RGB(192, 147, 98));
	delete pdc->SelectObject(sideWallsBrush); // delete frontWallBrush and select sideWallBrush

	CPoint leftWall[4];
	leftWall[0] = CPoint(0, 0);
	leftWall[1] = CPoint(canvas.right*0.1, canvas.bottom*0.1);
	leftWall[2] = CPoint(canvas.right*0.1, canvas.bottom*0.9);
	leftWall[3] = CPoint(0, canvas.bottom);
	pdc->Polygon(leftWall, 4);

	CPoint rightWall[4];
	rightWall[3] = CPoint(canvas.right*0.9, canvas.bottom*0.1); // top left
	rightWall[0] = CPoint(canvas.right, 0); // top right
	rightWall[1] = CPoint(canvas.right, canvas.bottom); // bottom right
	rightWall[2] = CPoint(canvas.right*0.9, canvas.bottom*0.9); // bottom left
	pdc->Polygon(rightWall, 4);

	CBrush* ceilingBrush = new CBrush(RGB(0, 0, 0));
	// delete pdc->SelectStockObject(WHITE_BRUSH);
	delete pdc->SelectObject(ceilingBrush); // delete sideWallBrush and select ceilinBrush

	CPoint ceiling[4];
	ceiling[3] = CPoint(0, 0); // top left
	ceiling[0] = CPoint(canvas.right, 0); // top right
	ceiling[1] = CPoint(canvas.right*0.9, canvas.bottom*0.1); // bottom right
	ceiling[2] = CPoint(canvas.right*0.1, canvas.bottom*0.1); // bottom left
	pdc->Polygon(ceiling, 4);

	delete pdc->SelectStockObject(LTGRAY_BRUSH); // delete ceilingBrush and select floorBrush
	CPoint floor[4];
	floor[3] = CPoint(canvas.right*0.1, canvas.bottom*0.9); // top left
	floor[0] = CPoint(canvas.right*0.9, canvas.bottom*0.9); // top right
	floor[1] = CPoint(canvas.right, canvas.bottom); // bottom right
	floor[2] = CPoint(0, canvas.bottom); // bottom left
	pdc->Polygon(floor, 4);

	pdc->SelectObject(oldBrush); // select oldBrush

}

void CFirstMFCAppView::drawClosedRoomDoor(CDC* pdc, CRect canvas) {

	CBrush* doorBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(doorBrush);

	this->setAndDrawLineFromCenter(canvas.right*0.9, canvas.bottom*0.2, pdc, canvas);

	CPoint door[4];

	door[0] = CPoint(canvas.right*0.91,
					 this->calcYForX(canvas.right*0.91)); // top left
	door[1] = CPoint(canvas.right*0.98,
					 calcYForX(canvas.right*0.98)); // top right

	this->setAndDrawLineFromCenter(canvas.right*0.9, canvas.bottom*0.9, pdc, canvas);

	door[2] = CPoint(canvas.right*0.98,
					 this->calcYForX(canvas.right*0.98)); // bottom right
	door[3] = CPoint(canvas.right*0.91,
					 calcYForX(canvas.right*0.91)); // bottom left
	pdc->Polygon(door, 4);

	if (this->closedRoomDoorRgn != NULL)
		delete this->closedRoomDoorRgn;
	// delete old (if exists) and create new (same as old one)
	this->closedRoomDoorRgn = new CRgn();
	this->closedRoomDoorRgn->CreatePolygonRgn(door, 4, ALTERNATE);


	CBrush* doorHandleBrush = new CBrush(RGB(235, 154, 0));
	delete pdc->SelectObject(doorHandleBrush);

	//left part (bigger) of the door handle
	CPoint dhTopLeft;
	dhTopLeft.x = door[1].x - ((door[1].x - door[0].x)*0.3);
	dhTopLeft.y = (door[1].y + (door[2].y - door[1].y) / 2);

	CPoint dhBottomRight;
	dhBottomRight.x = dhTopLeft.x + ((door[1].x - door[0].x)*0.2);
	dhBottomRight.y = dhTopLeft.y + ((door[2].y - door[1].y)*0.03);

	// right part (smaller) of the door handle
	CPoint smallDhTopLeft;
	smallDhTopLeft.x = dhTopLeft.x + (dhBottomRight.x - dhTopLeft.x)*0.8;
	smallDhTopLeft.y = dhTopLeft.y + (dhBottomRight.y - dhTopLeft.y)*0.2;

	CPoint smallDhBottomRight;
	smallDhBottomRight.x = dhBottomRight.x + (dhBottomRight.x - smallDhTopLeft.x);
	smallDhBottomRight.y = dhTopLeft.y + (dhBottomRight.y - dhTopLeft.y)*0.8;

	// first draw smaller part
	pdc->Ellipse(smallDhTopLeft.x, smallDhTopLeft.y, smallDhBottomRight.x, smallDhBottomRight.y);
	pdc->Ellipse(dhTopLeft.x, dhTopLeft.y, dhBottomRight.x, dhBottomRight.y);

	CBrush* keyHoleBrush = new CBrush(RGB(0, 0, 0));
	delete pdc->SelectObject(keyHoleBrush); // delete doorHandleBrush and select keyHoleBrush

	CPoint keyHoleTopLeft;
	keyHoleTopLeft.x = dhBottomRight.x;
	keyHoleTopLeft.y = dhBottomRight.y + (smallDhBottomRight.y - smallDhTopLeft.y);

	CPoint keyHoleBottomRight;
	keyHoleBottomRight.x = smallDhBottomRight.x;
	keyHoleBottomRight.y = keyHoleTopLeft.y + (smallDhBottomRight.y - smallDhTopLeft.y);

	pdc->Ellipse(keyHoleTopLeft.x, keyHoleTopLeft.y, keyHoleBottomRight.x, keyHoleBottomRight.y);

	pdc->SelectObject(oldBrush);
}

void CFirstMFCAppView::drawCabinetWithoutDoor(CDC * pdc, CRect canvas) {
	CBrush* cabinetBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(cabinetBrush);

	this->cabinetPoints = 4;
	this->cabinetFront = new CPoint*[4];

	cabinetFront[0] = new CPoint(canvas.right*0.05, canvas.bottom*0.20); // top left
	cabinetFront[1] = new CPoint(cabinetFront[0]->x + (canvas.right*0.15), cabinetFront[0]->y); // top right
	cabinetFront[2] = new  CPoint(cabinetFront[1]->x, canvas.bottom*0.95); // bottom right
	cabinetFront[3] = new CPoint(cabinetFront[0]->x, canvas.bottom*0.95); // bottom left

	pdc->Rectangle(this->cabinetFront[0]->x, this->cabinetFront[0]->y, this->cabinetFront[2]->x, this->cabinetFront[2]->y);

	this->setAndDrawLineFromCenter(this->cabinetFront[2]->x, this->cabinetFront[2]->y, pdc, canvas);

	CPoint cabinetSide[4];
	cabinetSide[0] = CPoint(this->cabinetFront[1]->x, this->cabinetFront[0]->y); // top left
	cabinetSide[3] = CPoint(this->cabinetFront[2]->x, this->cabinetFront[2]->y); // bottom left
	cabinetSide[2] = CPoint(this->calcXForY(canvas.bottom*0.9), canvas.bottom*0.9); // bottom right
	this->setAndDrawLineFromCenter(this->cabinetFront[1]->x, this->cabinetFront[1]->y, pdc, canvas); // set line for top side
	cabinetSide[1] = CPoint(cabinetSide[2].x, this->calcYForX(cabinetSide[2].x)); // top right
	pdc->Polygon(cabinetSide, 4);

	int cabinetHeight = this->cabinetFront[3]->y - this->cabinetFront[0]->y;
	int cabinetWidth = this->cabinetFront[1]->x - this->cabinetFront[0]->x;

	int verticalDoorSpace = cabinetHeight * 0.05;
	int horizontalDoorSpace = cabinetWidth * 0.05;

	int smallDrawerHeight = (cabinetHeight - 12 - 7 * verticalDoorSpace) / 6;
	int bigDrawerHeight = 4 * smallDrawerHeight + 4 * verticalDoorSpace - 12;

	int drawerWidth = cabinetWidth - 2 * horizontalDoorSpace;

	// DRAWERS

	CPoint cabinetDoor2[2];
	cabinetDoor2[0] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace,
							 this->cabinetFront[0]->y + bigDrawerHeight + 2 * verticalDoorSpace);
	cabinetDoor2[1] = CPoint(cabinetDoor2[0].x + drawerWidth,
							 cabinetDoor2[0].y + smallDrawerHeight);

	pdc->Rectangle(cabinetDoor2[0].x, cabinetDoor2[0].y, cabinetDoor2[1].x, cabinetDoor2[1].y);

	CPoint cabinetDoor3[2];
	cabinetDoor3[0] = CPoint(cabinetDoor2[0].x, cabinetDoor2[1].y + verticalDoorSpace);
	cabinetDoor3[1] = CPoint(cabinetDoor2[1].x, cabinetDoor3[0].y + smallDrawerHeight);

	pdc->Rectangle(cabinetDoor3[0].x, cabinetDoor3[0].y, cabinetDoor3[1].x, cabinetDoor3[1].y);

	CBrush* drawerHandleBrush = new CBrush(RGB(0, 0, 0));
	delete pdc->SelectObject(drawerHandleBrush);

	int cabinetHorizontalCenter = this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2;

	CPoint drawerHandle3[2];
	drawerHandle3[0] = CPoint(cabinetHorizontalCenter - (drawerWidth*0.05),
							  cabinetDoor2[0].y + smallDrawerHeight / 2 - smallDrawerHeight * 0.1);
	drawerHandle3[1] = CPoint(cabinetHorizontalCenter + (drawerWidth*0.05),
							  cabinetDoor2[0].y + smallDrawerHeight / 2 + smallDrawerHeight * 0.1);

	pdc->Ellipse(drawerHandle3[0].x, drawerHandle3[0].y, drawerHandle3[1].x, drawerHandle3[1].y);

	CPoint drawerHandle4[2];
	drawerHandle4[0] = CPoint(cabinetHorizontalCenter - (drawerWidth*0.05),
							  cabinetDoor3[0].y + smallDrawerHeight / 2 - smallDrawerHeight * 0.1);
	drawerHandle4[1] = CPoint(cabinetHorizontalCenter + (drawerWidth*0.05),
							  cabinetDoor3[0].y + smallDrawerHeight / 2 + smallDrawerHeight * 0.1);

	pdc->Ellipse(drawerHandle4[0].x, drawerHandle4[0].y, drawerHandle4[1].x, drawerHandle4[1].y);

}

void CFirstMFCAppView::drawOpenCabinet(CDC* pdc, CRect canvas) {

	// draw front side of the cabinet

	CBrush* cabinetBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(cabinetBrush);

	int cabinetHeight = this->cabinetFront[3]->y - this->cabinetFront[0]->y;
	int cabinetWidth = this->cabinetFront[1]->x - this->cabinetFront[0]->x;

	int verticalDoorSpace = cabinetHeight * 0.05;
	int horizontalDoorSpace = cabinetWidth * 0.05;

	int smallDrawerHeight = (cabinetHeight - 12 - 7 * verticalDoorSpace) / 6;
	int bigDrawerHeight = 4 * smallDrawerHeight + 4 * verticalDoorSpace - 12;

	int drawerWidth = cabinetWidth - 2 * horizontalDoorSpace;

	CBrush* blackBrush = new CBrush(RGB(0, 0, 0));
	pdc->SelectObject(blackBrush);

	CPoint leftDoorSpace[2];
	leftDoorSpace[0] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace,
							  this->cabinetFront[0]->y + verticalDoorSpace);
	leftDoorSpace[1] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2,
							  this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight);

	pdc->Rectangle(leftDoorSpace[0].x, leftDoorSpace[0].y, leftDoorSpace[1].x, leftDoorSpace[1].y);

	pdc->SelectObject(cabinetBrush);

	CPoint rightDoor[2];
	rightDoor[0] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2,
						  this->cabinetFront[0]->y + verticalDoorSpace);
	rightDoor[1] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth,
						  this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight);

	pdc->Rectangle(rightDoor[0].x, rightDoor[0].y, rightDoor[1].x, rightDoor[1].y);

	int cabinetHorizontalCenter = this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2;

	pdc->SelectObject(blackBrush);

	CPoint rightDrawerHandle[2];
	rightDrawerHandle[0] = CPoint(cabinetHorizontalCenter + (drawerWidth*0.1),
								  this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight / 2 - (smallDrawerHeight*0.1));
	rightDrawerHandle[1] = CPoint(rightDrawerHandle[0].x + (drawerWidth*0.1),
								  this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight / 2 + (smallDrawerHeight*0.1));

	pdc->Ellipse(rightDrawerHandle[0].x, rightDrawerHandle[0].y, rightDrawerHandle[1].x, rightDrawerHandle[1].y);

	pdc->SelectObject(cabinetBrush);

	CPoint openedDoor[4];

	this->setAndDrawLineFromCenter(leftDoorSpace[0].x, leftDoorSpace[0].y, pdc, canvas);

	openedDoor[0] = CPoint(leftDoorSpace[0].x - (drawerWidth / 2)*0.75,
						   this->calcYForX(leftDoorSpace[0].x - (drawerWidth / 2)*0.9)); // top left
	openedDoor[1] = leftDoorSpace[0];

	this->setAndDrawLineFromCenter(leftDoorSpace[0].x, leftDoorSpace[1].y, pdc, canvas);

	openedDoor[2] = CPoint(leftDoorSpace[0].x, leftDoorSpace[1].y);
	openedDoor[3] = CPoint(leftDoorSpace[0].x - (drawerWidth / 2)*0.75,
						   this->calcYForX(leftDoorSpace[0].x - (drawerWidth / 2)*0.9)); // bottom left

	if (this->openedCabinetDoorRgn != NULL)
		delete this->openedCabinetDoorRgn;
	// delete old one (if exists) and create new one (same as the old)
	this->openedCabinetDoorRgn = new CRgn();
	this->openedCabinetDoorRgn->CreatePolygonRgn(openedDoor, 4, ALTERNATE);

	this->cabinetKeyPlace.top = leftDoorSpace[1].y - (bigDrawerHeight*0.1);
	this->cabinetKeyPlace.right = leftDoorSpace[0].x + (bigDrawerHeight*0.1);
	this->cabinetKeyPlace.bottom = leftDoorSpace[1].y;
	this->cabinetKeyPlace.left = leftDoorSpace[0].x;

	pdc->Polygon(openedDoor, 4);
	if (!this->keyFound)
		PlayEnhMetaFile(pdc->m_hDC, this->key, this->cabinetKeyPlace);

	delete pdc->SelectObject(oldBrush);
}

void CFirstMFCAppView::drawClosedCabinet(CDC * pdc, CRect canvas) {
	CBrush* cabinetBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(cabinetBrush);

	int cabinetHeight = this->cabinetFront[3]->y - this->cabinetFront[0]->y;
	int cabinetWidth = this->cabinetFront[1]->x - this->cabinetFront[0]->x;

	int verticalDoorSpace = cabinetHeight * 0.05;
	int horizontalDoorSpace = cabinetWidth * 0.05;

	int smallDrawerHeight = (cabinetHeight - 12 - 7 * verticalDoorSpace) / 6;
	int bigDrawerHeight = 4 * smallDrawerHeight + 4 * verticalDoorSpace - 12;

	int drawerWidth = cabinetWidth - 2 * horizontalDoorSpace;

	CPoint leftDoor[2];
	leftDoor[0] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace,
						 this->cabinetFront[0]->y + verticalDoorSpace);
	leftDoor[1] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2,
						 this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight);

	if (this->closedCabinetDoorRgn != NULL)
		delete this->closedCabinetDoorRgn;

	this->closedCabinetDoorRgn = new CRgn();
	this->closedCabinetDoorRgn->CreateRectRgn(leftDoor[0].x, leftDoor[0].y, leftDoor[1].x, leftDoor[1].y);

	pdc->Rectangle(leftDoor[0].x, leftDoor[0].y, leftDoor[1].x, leftDoor[1].y);

	CPoint rightDoor[2];
	rightDoor[0] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2,
						  this->cabinetFront[0]->y + verticalDoorSpace);
	rightDoor[1] = CPoint(this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth,
						  this->cabinetFront[0]->y + verticalDoorSpace + bigDrawerHeight);

	pdc->Rectangle(rightDoor[0].x, rightDoor[0].y, rightDoor[1].x, rightDoor[1].y);

	int cabinetHorizontalCenter = this->cabinetFront[0]->x + horizontalDoorSpace + drawerWidth / 2;

	CBrush* doorHandleBrush = new CBrush(RGB(0, 0, 0));
	delete pdc->SelectObject(doorHandleBrush);

	CPoint drawerHandle1[2];
	drawerHandle1[0] = CPoint(cabinetHorizontalCenter - (drawerWidth*0.2),
							  leftDoor[0].y + bigDrawerHeight / 2 - (smallDrawerHeight*0.1));
	drawerHandle1[1] = CPoint(drawerHandle1[0].x + (drawerWidth*0.1),
							  drawerHandle1[0].y + smallDrawerHeight * 0.2);

	pdc->Ellipse(drawerHandle1[0].x, drawerHandle1[0].y, drawerHandle1[1].x, drawerHandle1[1].y);

	CPoint drawerHandle2[2];
	drawerHandle2[0] = CPoint(cabinetHorizontalCenter + (drawerWidth*0.2) / 2, drawerHandle1[0].y);
	drawerHandle2[1] = CPoint(drawerHandle2[0].x + (drawerWidth*0.1), drawerHandle1[1].y);

	pdc->Ellipse(drawerHandle2[0].x, drawerHandle2[0].y, drawerHandle2[1].x, drawerHandle2[1].y);


	delete pdc->SelectObject(oldBrush);

}

void CFirstMFCAppView::drawWindow(CDC* pdc, CRect canvas) {

	CBrush* windowBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(windowBrush);

	this->windowPoints = 2;
	this->windowFrame = new CPoint*[2];

	windowFrame[0] = new CPoint(this->center.x - (canvas.right*0.1), canvas.bottom*0.20);
	this->windowFrame[1] = new CPoint(this->center.x + (canvas.right*0.1), this->windowFrame[0]->y + canvas.bottom * 0.50);

	pdc->Rectangle(this->windowFrame[0]->x, this->windowFrame[0]->y, this->windowFrame[1]->x, this->windowFrame[1]->y);

	CBrush* blueBrush = new CBrush(RGB(100, 148, 224));
	pdc->SelectObject(blueBrush);

	int windowWidth = this->windowFrame[1]->x - this->windowFrame[0]->x;
	int windowHeight = this->windowFrame[1]->y - this->windowFrame[0]->y;

	CPoint downLeftFrame1[2];
	downLeftFrame1[0] = CPoint(this->windowFrame[0]->x + windowWidth * 0.05, this->center.y);
	downLeftFrame1[1] = CPoint(this->windowFrame[0]->x + (windowWidth / 2 - (windowWidth*0.025)),
							   this->windowFrame[1]->y - windowHeight * 0.05);

	pdc->Rectangle(downLeftFrame1[0].x, downLeftFrame1[0].y, downLeftFrame1[1].x, downLeftFrame1[1].y);

	CPoint downLeftFrame2[2];
	downLeftFrame2[0] = CPoint(this->center.x + windowWidth * 0.025, this->center.y);
	downLeftFrame2[1] = CPoint(this->windowFrame[1]->x - windowWidth * 0.05, downLeftFrame1[1].y);

	pdc->Rectangle(downLeftFrame2[0].x, downLeftFrame2[0].y, downLeftFrame2[1].x, downLeftFrame2[1].y);

	int windowTopHeight = canvas.bottom * 0.05;
	int offWindowFrame = windowWidth * 0.1;
	int windowTopWidht = windowWidth + 2 * offWindowFrame;

	delete pdc->SelectObject(windowBrush);

	CPoint windowTop[2];
	windowTop[0] = CPoint(this->windowFrame[0]->x - offWindowFrame, this->windowFrame[0]->y - windowTopHeight);
	windowTop[1] = CPoint(this->windowFrame[1]->x + offWindowFrame, this->windowFrame[0]->y);

	int cornerX = windowTopHeight;
	int cornerY = windowTopHeight;

	pdc->RoundRect(windowTop[0].x, windowTop[0].y, windowTop[1].x, windowTop[1].y, cornerX, cornerY);

	delete pdc->SelectObject(oldBrush);

}

void CFirstMFCAppView::drawCurtain(CDC* pdc, CRect canvas) {

	int windowWidth = this->windowFrame[1]->x - this->windowFrame[0]->x;
	int windowHeight = this->windowFrame[1]->y - this->windowFrame[0]->y;

	CPoint curtain[4];
	curtain[0] = CPoint(this->windowFrame[0]->x - (windowWidth*0.025),
						this->windowFrame[1]->y - (windowHeight*0.1));
	curtain[1] = CPoint(this->windowFrame[0]->x - (windowWidth*0.025),
						this->windowFrame[0]->y);
	curtain[2] = CPoint(this->windowFrame[1]->x + (windowWidth*0.025),
						this->windowFrame[0]->y);
	curtain[3] = CPoint(this->windowFrame[1]->x + (windowWidth*0.025),
						this->windowFrame[1]->y - (windowHeight*0.1));

	pdc->BeginPath();
	pdc->Polyline(curtain, 4);

	int offStraightLine = windowHeight * 0.1;

	CPoint curtainBottom[4];
	curtainBottom[0] = curtain[0];
	curtainBottom[1] = CPoint(curtain[0].x + (windowWidth*0.25), curtain[0].y - offStraightLine);
	curtainBottom[2] = CPoint(curtain[3].x - (windowWidth*0.25), curtain[0].y + offStraightLine);
	curtainBottom[3] = curtain[3];

	pdc->PolyBezier(curtainBottom, 4);
	pdc->EndPath();

	CBrush* curtainBrush = new CBrush(RGB(0, 100, 0));
	CBrush* oldBrush = pdc->SelectObject(curtainBrush);

	//pdc->FloodFill(curtain[1].x + 2, curtain[1].y + 2, RGB(0, 0, 0));

	pdc->SetPolyFillMode(ALTERNATE);
	pdc->FillPath();

	delete pdc->SelectObject(oldBrush);


}

void CFirstMFCAppView::drawKeyBox(CDC * pdc, CRect canvas) {
	CBrush* curtainBrush = new CBrush(RGB(100, 20, 100));
	CBrush* oldBrush = pdc->SelectObject(curtainBrush);

	this->setAndDrawLineFromCenter(0, 0, pdc, canvas);

	CPoint keyBox[2];
	keyBox[0] = CPoint(canvas.right*0.01, this->calcYForX(canvas.right*0.01));
	keyBox[1] = CPoint(canvas.right*0.08, this->calcYForX(canvas.right*0.08));

	pdc->RoundRect(keyBox[0].x, keyBox[0].y, keyBox[1].x, keyBox[1].y, 10, 10);

	int keyBoxWidth = keyBox[1].x - keyBox[0].x;
	int keyBoxHeight = keyBox[1].y - keyBox[0].y;

	int margin = keyBoxHeight * 0.1;

	this->topKeyPlace.top = keyBox[0].y + margin;
	this->topKeyPlace.bottom = keyBox[1].y - margin;
	this->topKeyPlace.right = keyBox[1].x - margin;
	this->topKeyPlace.left = keyBox[0].x + margin;

	if (this->keyFound)
		PlayEnhMetaFile(pdc->m_hDC, this->key, this->topKeyPlace);

	delete pdc->SelectObject(oldBrush);
}

void CFirstMFCAppView::drawOpenRoomDoor(CDC * pdc, CRect canvas) {

	CBrush* openDoorBrush = new CBrush(RGB(0, 0, 0));
	CBrush* oldBrush = pdc->SelectObject(openDoorBrush);

	this->setAndDrawLineFromCenter(canvas.right*0.9, canvas.bottom*0.2, pdc, canvas);

	CPoint door[4];

	door[0] = CPoint(canvas.right*0.91,
					 this->calcYForX(canvas.right*0.91)); // top left
	door[1] = CPoint(canvas.right*0.98,
					 calcYForX(canvas.right*0.98)); // top right

	this->setAndDrawLineFromCenter(canvas.right*0.9, canvas.bottom*0.9, pdc, canvas);

	door[2] = CPoint(canvas.right*0.98,
					 this->calcYForX(canvas.right*0.98)); // bottom right
	door[3] = CPoint(canvas.right*0.91,
					 calcYForX(canvas.right*0.91)); // bottom left
	pdc->Polygon(door, 4);


	CBrush* doorBrush = new CBrush(RGB(132, 99, 66));
	delete pdc->SelectObject(doorBrush);

	int doorWidh = door[1].x - door[0].x;

	CPoint openDoor[2];
	openDoor[0] = CPoint(door[0].x - doorWidh - (doorWidh*0.65), door[0].y);
	openDoor[1] = door[3];

	pdc->Rectangle(openDoor[0].x, openDoor[0].y, openDoor[1].x, openDoor[1].y);

	if (this->openedRoomDoorRgn != NULL)
		delete this->openedRoomDoorRgn;

	this->openedRoomDoorRgn = new CRgn();
	this->openedRoomDoorRgn->CreateRectRgn(openDoor[0].x, openDoor[0].y, openDoor[1].x, openDoor[1].y);

	delete pdc->SelectObject(oldBrush);

}

void CFirstMFCAppView::drawClock(CDC * pdc, CRect canvas) {

	CBrush* clockBrush = new CBrush(RGB(132, 99, 66));
	CBrush* oldBrush = pdc->SelectObject(clockBrush);

	// outside brown rectangle
	CPoint clockRect[2];
	clockRect[0] = CPoint(canvas.right*0.715, canvas.bottom*0.30);
	clockRect[1] = CPoint(clockRect[0].x + (canvas.right*0.1), clockRect[0].y + (canvas.bottom*0.35));

	int clockRectWidth = clockRect[1].x - clockRect[0].x;
	int clockRectHeight = clockRect[1].y - clockRect[0].y;
	CPoint clockCenter = CPoint(clockRect[0].x + clockRectWidth / 2, clockRect[0].y);
	int clockRadius = clockRectWidth / 2 + (clockRectWidth*0.15);

	int cornerRoundnes = clockRectWidth * 0.1;
	int rectMargine = clockRectWidth * 0.05;

	// black rectangle
	CPoint insideClockRect[2];
	insideClockRect[0] = CPoint(clockRect[0].x + rectMargine, clockRect[0].y + rectMargine);
	insideClockRect[1] = CPoint(clockRect[1].x - rectMargine, clockRect[1].y - rectMargine);

	// draw outside rectangle
	pdc->RoundRect(clockRect[0].x, clockRect[0].y, clockRect[1].x, clockRect[1].y, cornerRoundnes, cornerRoundnes);

	CBrush* clockRectInsideBrush = new CBrush(RGB(0, 0, 0));
	pdc->SelectObject(clockRectInsideBrush); // select black brush for rectangle inside

	// draw black rectangle inside
	pdc->RoundRect(insideClockRect[0].x, insideClockRect[0].y, insideClockRect[1].x, insideClockRect[1].y, cornerRoundnes, cornerRoundnes);

	CPoint pendulumStick[2];// relative from clockCenter
	pendulumStick[0] = CPoint(-clockRectWidth * 0.025, 0);
	pendulumStick[1] = CPoint(clockRectWidth*0.025, clockRectHeight*0.8);

	CBrush* goldBrush = new CBrush(RGB(255, 223, 0));
	delete pdc->SelectObject(goldBrush); // delete black brush for rect inside and select brown brush


	int pendulumCircleRadius = (pendulumStick[1].x - pendulumStick[0].x) * 3;
	int pendulumStickWidht = 2 * (clockRectWidth*0.025);

	CPoint pendulumEnd[2];
	pendulumEnd[0] = CPoint(pendulumStick[1].x - pendulumCircleRadius - pendulumStickWidht / 2,
							pendulumStick[1].y - pendulumCircleRadius);
	pendulumEnd[1] = CPoint(pendulumStick[1].x + pendulumCircleRadius - pendulumStickWidht / 2,
							pendulumStick[1].y + pendulumCircleRadius);

	int pendulumStickHeight = pendulumStick[1].y - pendulumStick[0].y;
	int pendulumEndRadius = pendulumEnd[1].x - pendulumEnd[0].x;

	XFORM rot;

	this->pMaxAngle = asin(((clockRectWidth - pendulumEndRadius - 2 * rectMargine) / 2) / (double)pendulumStickHeight);
	this->pHStep = this->pMaxAngle / 4;
	this->pMStep = this->pMaxAngle / 8;
	//this->lastPAngle = -this->pMaxAngle;

	//
	// here add pendulum transformation
	//

	pdc->SetViewportOrg(clockCenter);

	SetGraphicsMode(pdc->m_hDC, GM_ADVANCED);

	rot.eM11 = cos(this->lastPAngle);
	rot.eM12 = sin(this->lastPAngle);
	rot.eM21 = -sin(this->lastPAngle);
	rot.eM22 = cos(this->lastPAngle);
	rot.eDx = 0;
	rot.eDy = 0;

	pdc->SetWorldTransform(&rot);

	// draw pendulum stick
	pdc->Rectangle(pendulumStick[0].x, pendulumStick[0].y, pendulumStick[1].x, pendulumStick[1].y);
	pdc->Ellipse(pendulumEnd[0].x, pendulumEnd[0].y, pendulumEnd[1].x, pendulumEnd[1].y);

	pdc->ModifyWorldTransform(&rot, MWT_IDENTITY);

	pdc->SetViewportOrg(0, 0);

	//
	// remove pendulum transformation
	//

	pdc->SelectObject(clockBrush);

	CPoint clockHead[2];
	clockHead[0] = CPoint(clockCenter.x - clockRadius, clockCenter.y - clockRadius);
	clockHead[1] = CPoint(clockCenter.x + clockRadius, clockCenter.y + clockRadius);

	pdc->Ellipse(clockHead[0].x, clockHead[0].y, clockHead[1].x, clockHead[1].y); // cloclk head outside circle

	CPoint clockHeadInside[2];
	clockHeadInside[0] = CPoint(clockHead[0].x + clockRadius * 0.15, clockHead[0].y + clockRadius * 0.15);
	clockHeadInside[1] = CPoint(clockHead[1].x - clockRadius * 0.15, clockHead[1].y - clockRadius * 0.15);

	pdc->SelectObject(goldBrush);
	pdc->Ellipse(clockHeadInside[0].x, clockHeadInside[0].y, clockHeadInside[1].x, clockHeadInside[1].y);
	// draw golden clock head inside

	delete clockBrush;
	delete goldBrush;

	CBrush* whiteBrush = new CBrush(RGB(255, 255, 255));
	pdc->SelectObject(whiteBrush);

	CPoint clockWhiteCenter[2];
	clockWhiteCenter[0] = CPoint(clockHeadInside[0].x + clockRadius * 0.3, clockHeadInside[0].y + clockRadius * 0.3);
	clockWhiteCenter[1] = CPoint(clockHeadInside[1].x - clockRadius * 0.3, clockHeadInside[1].y - clockRadius * 0.3);

	CPoint clockWhiteCenter2[2];
	clockWhiteCenter2[0] = CPoint(clockHeadInside[0].x + clockRadius * 0.45, clockHeadInside[0].y + clockRadius * 0.45);
	clockWhiteCenter2[1] = CPoint(clockHeadInside[1].x - clockRadius * 0.45, clockHeadInside[1].y - clockRadius * 0.45);

	pdc->Ellipse(clockWhiteCenter[0].x, clockWhiteCenter[0].y, clockWhiteCenter[1].x, clockWhiteCenter[1].y);
	pdc->Ellipse(clockWhiteCenter2[0].x, clockWhiteCenter2[0].y, clockWhiteCenter2[1].x, clockWhiteCenter2[1].y);

	// should be uncommented
	// !!!!!!!
	// 
	delete whiteBrush;
	//
	//!!!!!
	//

	CString numbers[] = {
		CString("XII"),
			CString("I"),
			CString("II"),
			CString("III"),
			CString("IV"),
			CString("V"),
			CString("VI"),
			CString("VII"),
			CString("VIII"),
			CString("IX"),
			CString("X"),
			CString("XI")
	};

	CPen* boldPen = new CPen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen* normalPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	// pen for number circle boundary
	CPen* numBackPen = new CPen(PS_SOLID, 0, RGB(209, 157, 98));

	// just to get old pen
	CPen* oldPen = pdc->SelectObject(numBackPen);

	// first brush in this section
	// only needed
	CBrush* numBackBrush = new CBrush(RGB(209, 157, 98));
	pdc->SelectObject(numBackBrush);


	//
	// PAY ATTENTION
	//
	clockCenter.x -= 1;
	clockCenter.y -= 1;
	pdc->SetViewportOrg(clockCenter);
	//
	//
	//

	// circle around number
	int numberBackRadius = clockWhiteCenter2[0].y - clockWhiteCenter[0].y;
	CPoint numberBack[2];
	numberBack[0] = CPoint(clockCenter.x - numberBackRadius - clockCenter.x,
						   clockHeadInside[0].y + 1 - clockCenter.y + 1);
	numberBack[1] = CPoint(clockCenter.x + numberBackRadius - clockCenter.x - 1,
						   clockWhiteCenter[0].y - 1 - clockCenter.y - 1);
	// place to draw number
	CRect numRect = CRect(CPoint(numberBack[0].x, numberBack[0].y + numberBackRadius * 0.1),
						  CPoint(numberBack[1].x, numberBack[1].y + numberBackRadius * 0.1));

	// background color for text (numbers)
	pdc->SetBkColor(RGB(209, 157, 98));

	// rotation matrix
	//XFORM rot;
	rot.eM11 = 0;
	rot.eM12 = 0;
	rot.eM21 = 0;
	rot.eM22 = 0;
	rot.eDx = 0;
	rot.eDy = 0;

	for (int i = 0; i < 60; i++) {

		rot.eM11 = cos(i* this->pi / 30);
		rot.eM12 = sin(i* this->pi / 30);
		rot.eM21 = -sin(i* this->pi / 30);
		rot.eM22 = cos(i* this->pi / 30);

		// aply transformation
		pdc->SetWorldTransform(&rot);

		if (i % 5 == 0) {
			// every fifth time draw number
			pdc->SelectObject(numBackPen);

			pdc->Ellipse(numberBack[0].x, numberBack[0].y, numberBack[1].x, numberBack[1].y);
			pdc->DrawText(numbers[i / 5], numRect, DT_CENTER | DT_VCENTER);

			// every fifth is bold
			pdc->SelectObject(boldPen);
		}

		// draw line
		pdc->MoveTo(0, clockWhiteCenter2[0].y - clockCenter.y);
		pdc->LineTo(0, clockWhiteCenter[0].y - 1 - clockCenter.y);

		pdc->SelectObject(normalPen);

	}

	// rotation for big hand
	rot.eM11 = cos(this->lastBHAngle);
	rot.eM12 = sin(this->lastBHAngle);
	rot.eM21 = -sin(this->lastBHAngle);
	rot.eM22 = cos(this->lastBHAngle);
	rot.eDx = 0;
	rot.eDy = 0;

	// apply rotation
	pdc->SetWorldTransform(&rot);

	int bigHandWidth = clockRadius * 0.15;
	int bigHandHeight = clockRadius * 0.9;

	if (this->bigHandRect != NULL)
		delete this->bigHandRect;
	this->bigHandRect = new CRect(CPoint(-bigHandWidth / 2, -bigHandHeight * 0.66),
								  CPoint(bigHandWidth / 2, bigHandHeight * 0.33));
	// display big hand 
	pdc->PlayMetaFile(this->bigHand, this->bigHandRect);

	// rotation for small hand
	rot.eM11 = cos(this->lastSHAngle);
	rot.eM12 = sin(this->lastSHAngle);
	rot.eM21 = -sin(this->lastSHAngle);
	rot.eM22 = cos(this->lastSHAngle);
	rot.eDx = 0;
	rot.eDy = 0;

	// apply rotation
	pdc->SetWorldTransform(&rot);

	int smallHandWidth = clockRadius * 0.3;
	int smallHandHeight = clockRadius * 0.65;

	if (this->smallHandRect != NULL)
		delete this->smallHandRect;
	this->smallHandRect = new CRect(CPoint(-smallHandWidth / 2, -smallHandHeight * 0.66),
									CPoint(smallHandWidth / 2, smallHandHeight * 0.33));

	// display small hand
	pdc->PlayMetaFile(this->smallHand, this->smallHandRect);

	// reset environment
	pdc->SetViewportOrg(0, 0); // set coor. center in top left corner
	pdc->ModifyWorldTransform(NULL, MWT_IDENTITY);// set transf mat. to neutral

	delete boldPen;
	delete numBackPen;

	delete pdc->SelectObject(oldPen);
	delete pdc->SelectObject(oldBrush);
}

void CFirstMFCAppView::testTransform(CDC * pdc, CRect canvas) {

	int stackP = pdc->SaveDC();

	CPen* newPen = new CPen(PS_SOLID, 5, RGB(255, 255, 255));
	pdc->SelectObject(newPen);

	SetGraphicsMode(pdc->m_hDC, GM_ADVANCED);

	pdc->SetViewportOrg(500, 250);

	XFORM trans;
	trans.eM11 = cos(this->pi / 4);
	trans.eM12 = sin(this->pi / 4);
	trans.eM21 = -sin(this->pi / 4);
	trans.eM22 = cos(this->pi / 4);
	trans.eDx = 0;
	trans.eDy = 0;

	pdc->SetWorldTransform(&trans);

	CPoint startP(0, 0);
	CPoint endP(200, 0);

	pdc->MoveTo(startP);
	pdc->LineTo(endP);

	RestoreDC(pdc->m_hDC, stackP);


}

void CFirstMFCAppView::setAndDrawLineFromCenter(int x, int y, CDC* pdc, CRect canvas) {

	this->setLineFromCenter(x, y);

	CPen* linePen = new CPen(PS_SOLID, 1, RGB(0, 0, 200));
	CPen* oldPen = pdc->SelectObject(linePen);

	pdc->MoveTo(this->center);
	pdc->LineTo(*(new CPoint(x, this->calcYForX(x))));

	delete pdc->SelectObject(oldPen);
}

void CFirstMFCAppView::setLineFromCenter(int x, int y) {

	this->kPar = ((float)(this->center.y - y)) / (this->center.x - x);
	this->nPar = (float)this->center.y - (this->kPar*this->center.x);
}

int CFirstMFCAppView::calcYForX(int x) {
	return this->kPar*x + this->nPar;
}

int CFirstMFCAppView::calcXForY(int y) {
	return (y - this->nPar) / this->kPar;
}

// CFirstMFCAppView printing

BOOL CFirstMFCAppView::OnPreparePrinting(CPrintInfo* pInfo) {
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFirstMFCAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add extra initialization before printing
}

void CFirstMFCAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add cleanup after printing
}


// CFirstMFCAppView diagnostics

#ifdef _DEBUG
void CFirstMFCAppView::AssertValid() const {
	CView::AssertValid();
}

void CFirstMFCAppView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}

CFirstMFCAppDoc* CFirstMFCAppView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFirstMFCAppDoc)));
	return (CFirstMFCAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CFirstMFCAppView message handlers


void CFirstMFCAppView::OnLButtonDown(UINT nFlags, CPoint clickPoint) {

	if (this->roomDoorOpened) {
		if (this->openedRoomDoorRgn->PtInRegion(clickPoint)) {
			this->roomDoorOpened = false;
			Invalidate();
		}
	} else {
		if (this->closedRoomDoorRgn->PtInRegion(clickPoint)) {
			if (this->keyFound) {
				this->roomDoorOpened = true;
				Invalidate();
			}
		}
	}

	if (this->cabinetDoorOpened) {
		if (this->openedCabinetDoorRgn->PtInRegion(clickPoint)) {
			this->cabinetDoorOpened = false;
			Invalidate();
		} else if (this->cabinetKeyPlace.PtInRect(clickPoint)) {
			this->keyFound = true;
			Invalidate();
		}
	} else {
		if (this->closedCabinetDoorRgn->PtInRegion(clickPoint)) {
			this->cabinetDoorOpened = true;
			Invalidate();
		}
	}


	CView::OnLButtonDown(nFlags, clickPoint);
}


void CFirstMFCAppView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {

	// 77 is code for 'm'
	// 72 is code for 'h'
	double nextAngle;
	if (nChar == 77) { // 'm' is pressed
		// move big hand
		this->lastBHAngle += this->pi / 30;

		// move pendulum
		nextAngle = this->lastPAngle + this->pMStep*this->sign;

		if (fabs(nextAngle) > this->pMaxAngle) {
			this->lastPAngle = this->pMaxAngle*sign;
			this->sign *= -1;
		} else {
			this->lastPAngle = nextAngle;
		}

		Invalidate();
	} else if (nChar == 72) { // 'h' is pressed
		// move small hand
		this->lastSHAngle += this->pi / 6;

		// move pendulum
		nextAngle = this->lastPAngle + this->pHStep*sign;

		if (fabs(nextAngle) > this->pMaxAngle) {
			this->lastPAngle = this->pMaxAngle*sign;
			this->sign *= -1;
		} else {
			this->lastPAngle = nextAngle;
		}

		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
