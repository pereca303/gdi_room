
// FirstMFCAppView.h : interface of the CFirstMFCAppView class
//

#pragma once


class CFirstMFCAppView : public CView {
protected: // create from serialization only
	CFirstMFCAppView() noexcept;
	DECLARE_DYNCREATE(CFirstMFCAppView)

	// Attributes
public:
	CFirstMFCAppDoc* GetDocument() const;

	// Operations
public:

	// Overrides
private:

	double pi = 3.14159265358979323846;

	CPoint center;
	CPoint** windowFrame;
	int windowPoints;
	CPoint** cabinetFront;
	int cabinetPoints;

	bool cabinetDoorOpened = false;
	CRgn* openedCabinetDoorRgn = NULL;
	CRgn* closedCabinetDoorRgn = NULL;

	HENHMETAFILE key;
	CRect topKeyPlace;
	CRect cabinetKeyPlace;
	bool keyFound = false;

	bool roomDoorOpened = false;
	CRgn* closedRoomDoorRgn = NULL;
	CRgn* openedRoomDoorRgn = NULL;

	HENHMETAFILE bigHand;
	HENHMETAFILE smallHand;
	CRect* bigHandRect = NULL;
	CRect* smallHandRect = NULL;

	//XFORM transform;

	//XFORM smallHandRot;
	double lastSHAngle = this->pi / 2;
	//XFORM bigHandRot;
	double lastBHAngle = 0;
	//XFORM pendulumRot;
	double lastPAngle = 0;
	double pMaxAngle;
	double pHStep;
	double pMStep;
	int sign = 1;

	// y = KxPar + nPar;
	float kPar;
	float nPar;

	virtual void drawWals(CDC* pDC, CRect canvas);
	virtual void drawClosedRoomDoor(CDC* pDC, CRect canvas);
	virtual void drawCabinetWithoutDoor(CDC* pDC, CRect canvas);
	virtual void drawOpenCabinet(CDC* pDC, CRect canvas);
	virtual void drawClosedCabinet(CDC* pDC, CRect canvas);
	virtual void drawWindow(CDC* pDC, CRect canvas);
	virtual void drawCurtain(CDC* pDC, CRect canvas);
	virtual void drawKeyBox(CDC* pDC, CRect canvas);
	virtual void drawOpenRoomDoor(CDC* pDC, CRect canvas);

	virtual void drawClock(CDC* pDC, CRect canvas);

	virtual void testTransform(CDC* pDC, CRect canvas);

	void setAndDrawLineFromCenter(int x, int y, CDC* pdc, CRect canvas);
	void setLineFromCenter(int x, int y);
	int calcYForX(int x);
	int calcXForY(int y);

	//afx_msg void OnLButtonDown(UINT nFlags, CPoint clickPoint);


public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view




	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Implementation
public:
	virtual ~CFirstMFCAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in FirstMFCAppView.cpp
inline CFirstMFCAppDoc* CFirstMFCAppView::GetDocument() const {
	return reinterpret_cast<CFirstMFCAppDoc*>(m_pDocument);
}
#endif

