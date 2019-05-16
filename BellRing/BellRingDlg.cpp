
// BellRingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BellRing.h"
#include "BellRingDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <fstream>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define clamp_val(val, lo, hi) min(max(val, lo), hi)

const UINT    WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBellRingDlg dialog



CBellRingDlg::CBellRingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BELLRING_DIALOG, pParent)
	, m_iClassMin(0)
	, m_iBigGap(0)
	, m_iSmallGap(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBellRingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLASSTIME, m_iClassMin);
	DDV_MinMaxInt(pDX, m_iClassMin, 0, 60);
	DDX_Text(pDX, IDC_EDIT_GAPBIG, m_iBigGap);
	DDV_MinMaxInt(pDX, m_iBigGap, 0, 15);
	DDX_Text(pDX, IDC_EDIT_GAPSMALL, m_iSmallGap);
	DDV_MinMaxInt(pDX, m_iSmallGap, 0, 15);
}

BEGIN_MESSAGE_MAP(CBellRingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOWTASK, OnTray)
	ON_EN_CHANGE(IDC_EDIT_AM, &CBellRingDlg::OnEnChangeEditAM)
	ON_EN_CHANGE(IDC_EDIT_PM, &CBellRingDlg::OnEnChangeEditPM)
	ON_EN_CHANGE(IDC_EDIT_CLASSTIME, &CBellRingDlg::OnEnChangeEditClassTime)
	ON_EN_CHANGE(IDC_EDIT_GAPBIG, &CBellRingDlg::OnEnChangeEditGapBig)
	ON_EN_CHANGE(IDC_EDIT_GAPSMALL, &CBellRingDlg::OnEnChangeEditGapSmall)
	ON_EN_KILLFOCUS(IDC_EDIT_AM, &CBellRingDlg::OnKillFocusEditAM)
	ON_EN_KILLFOCUS(IDC_EDIT_PM, &CBellRingDlg::OnKillFocusEditPM)
	ON_BN_CLICKED(IDC_BUTTON_SETTIME, &CBellRingDlg::OnBnClickedButtonSettime)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, &CBellRingDlg::OnBnClickedButtonHide)
	ON_BN_CLICKED(IDC_CHECK1, &CBellRingDlg::OnBnClickedCheck1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CBellRingDlg::OnNMCustomdrawSlider1)
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, &CBellRingDlg::OnTaskBarCreated)
	ON_WM_DESTROY()
	ON_WM_POWERBROADCAST()
END_MESSAGE_MAP()

unsigned __stdcall VolumeThread(void* pParam)
{
	SWaveInfo* pSWaveInfo = (SWaveInfo*)pParam;

	if (!pSWaveInfo || !pSWaveInfo->pOriginalFileBytes || !pSWaveInfo->pScaledFileBytes)
		return 0;

	__int8 * pO = (__int8 *)(pSWaveInfo->pOriginalFileBytes);
	__int8 * pS = (__int8 *)(pSWaveInfo->pScaledFileBytes);

	for (int i = 80 / sizeof(*pS); i < pSWaveInfo->dwFileSize / sizeof(*pS); i++) {
		// COMMENT FOLLOWING LINE
		pS[i] = (__int8)((float)pO[i] * pSWaveInfo->fVolume);

	}

	return 0;
}

unsigned __stdcall RingThread(void* pParam)
{
	//PlaySound(TEXT("ring.wav"), NULL, SND_FILENAME);

	SWaveInfo* pSWaveInfo = (SWaveInfo*)pParam;

	if (!pSWaveInfo || !pSWaveInfo->pScaledFileBytes)
		return 0;

	PlaySound((LPCWSTR)pSWaveInfo->pScaledFileBytes, NULL, SND_MEMORY);

	return 0;
}

unsigned __stdcall TimeShowThread(void* pParam)
{
	SThreadParam* pThreadParam = (SThreadParam*)pParam;

	if (!pThreadParam)
		return 0;

	while (true)
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		pThreadParam->strCurrentTime.Format(_T("%02d:%02d"), st.wHour, st.wMinute);
		CString timeShow;
		timeShow.Format(_T("%s:%02d"), pThreadParam->strCurrentTime, st.wSecond);
		pThreadParam->pWndTimeText->SetWindowText((LPCTSTR)timeShow);

		if (timeShow == L"00:00:00") {
			char buf[64];
			sprintf_s(buf, "Reset\n");
			OutputDebugStringA(buf);
			((CBellRingDlg*)pThreadParam->pParentCBellRingDlg)->ReSetRingFlag();
		}

		if (pThreadParam->ringTimeFlag < pThreadParam->timeList.size() && pThreadParam->timeList[pThreadParam->ringTimeFlag] == pThreadParam->strCurrentTime)
		{
			int iPos;
			iPos = pThreadParam->pTimeListBox->FindString(0, L"下课");
			if (iPos == 3) 
				pThreadParam->pTimeListBox->DeleteString(0);
			pThreadParam->pTimeListBox->DeleteString(0);

			((CBellRingDlg*)pThreadParam->pParentCBellRingDlg)->SetTimeListSize();

			if (!pThreadParam->bIsMute) {
				unsigned int	iThreadID;
				UINT_PTR		hThreadHandle;
				hThreadHandle = _beginthreadex(0, 0, RingThread, pThreadParam->pRingInfo, CREATE_SUSPENDED, &iThreadID);
				ResumeThread((HANDLE)hThreadHandle);
				//PlaySound(TEXT("ring.wav"), NULL, SND_FILENAME);
			}

			pThreadParam->ringTimeFlag++;
		}
		Sleep(1000);
	}
}

// CBellRingDlg message handlers

WNDPROC prevProc[2];
LRESULT CALLBACK TimeEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	TCHAR ch[16];
	::GetWindowText(hWnd, ch, 6);
	CString str = ch;
	//char buf[16];
	//sprintf_s(buf, ",pos:%d, len:%d\n", str.Find(L":"), str.GetLength());
	//OutputDebugStringA(buf);
	bool bHaveColon = (str.Find(L":") >= 0);
	bool bInputColon = (!bHaveColon && wParam == ':');

	if (uMsg == WM_CHAR && wParam != VK_BACK) {

		CEdit *pEdit = (CEdit*)CWnd::FromHandle(::GetDlgItem(::GetParent(hWnd), ::GetDlgCtrlID(hWnd)));
		int nStartChar = 0, nEndChar = 0;
		if (pEdit)
			pEdit->GetSel(nStartChar, nEndChar);

		if (bHaveColon && str.GetLength() >= str.Find(L":") + 3 && nStartChar == nEndChar) {
			::SetFocus(::GetNextDlgTabItem(::GetParent(hWnd), hWnd, false));
			::SendDlgItemMessage(::GetParent(GetFocus()), ::GetDlgCtrlID(GetFocus()), EM_SETSEL, 0, -1);
			return 1;
		}

		if ((wParam < '0' || wParam > '9') && !bInputColon)
			return 1;
	}

	return prevProc[::GetDlgCtrlID(hWnd) - IDC_EDIT_AM](hWnd, uMsg, wParam, lParam);
}

BOOL CBellRingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);

	m_pThreadParam = new SThreadParam();
	m_pThreadParam->pWndTimeText = this->GetDlgItem(IDC_TIMETEXT_STATIC);
	m_pThreadParam->pTimeListBox = ((CListBox*)(GetDlgItem(IDC_LIST1)));
	m_pThreadParam->pParentCBellRingDlg = this;
	
	CSliderCtrl* pVolume = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1));
	pVolume->SetPos(pVolume->GetRangeMax());
	m_pThreadParam->pRingInfo = new SWaveInfo();
	m_pThreadParam->pRingInfo->fVolume = 1.0f;

	m_pThreadParam->hThreadHandle = _beginthreadex(0, 0, TimeShowThread, m_pThreadParam, CREATE_SUSPENDED, &(m_pThreadParam->iThreadID));
	ResumeThread((HANDLE)m_pThreadParam->hThreadHandle);

	ReadSettingFile();
	SetTime();
	ReadRingFile();
	prevProc[IDC_EDIT_AM - IDC_EDIT_AM] = (WNDPROC)SetWindowLongPtr(GetDlgItem(IDC_EDIT_AM)->m_hWnd, GWLP_WNDPROC, (LONG_PTR)TimeEditProc);
	prevProc[IDC_EDIT_PM - IDC_EDIT_AM] = (WNDPROC)SetWindowLongPtr(GetDlgItem(IDC_EDIT_PM)->m_hWnd, GWLP_WNDPROC, (LONG_PTR)TimeEditProc);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBellRingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBellRingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBellRingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CBellRingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_ESCAPE)
	{
		// 如果消息是键盘按下事件，且是Esc键，执行以下代码（什么都不做，你可以自己添加想要的代码）
		GetDlgItem(IDC_BUTTON_HIDE)->SendMessage(BM_CLICK);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// 如果消息是键盘按下事件，且是Entert键，执行以下代码（什么都不做，你可以自己添加想要的代码）
		//GetNextDlgTabItem(GetFocus())->SetFocus();
		//SendDlgItemMessage(GetFocus()->GetDlgCtrlID(), EM_SETSEL, 0, -1);
		GetDlgItem(IDC_BUTTON_SETTIME)-> SetFocus();
		GetDlgItem(IDC_BUTTON_SETTIME)-> SendMessage(BM_CLICK);
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CBellRingDlg::TextInputFormatTime(CEdit* editHelp, bool bIsAM, bool bIsKillFocus)
{
	CString strTemp;
	editHelp->GetWindowText(strTemp);
	int iPreTextLen = m_strPreTimeString[bIsAM].GetLength();
	if (strTemp == m_strPreTimeString[bIsAM] && !bIsKillFocus)
		return;

	int iPos = strTemp.Find(_T(":")) < 2 && strTemp.Find(_T(":")) >= 0 ? strTemp.Find(_T(":")) : 2;

	if (strTemp.GetLength() == iPos && strTemp.GetLength() > iPreTextLen)
		strTemp.AppendChar(L':');
	if (strTemp.GetLength() == iPos && strTemp.GetLength() < iPreTextLen)
		strTemp = strTemp.Left(iPos - 1);

	iPos = strTemp.Find(_T(":"));

	CString strHour;
	AfxExtractSubString(strHour, strTemp, 0, _T(':'));
	int iHour = bIsAM ? clamp_val(_ttoi(strHour), 0, 11) : clamp_val(_ttoi(strHour), 12, 23);
	if (strHour.GetLength() == 2 || bIsKillFocus)
		strHour.Format(_T("%02d"), iHour);
	else if (strHour.GetLength() == 1 && iPos >= 0)
		strHour.Format(_T("%d"), iHour);

	CString strMin;
	AfxExtractSubString(strMin, strTemp, 1, _T(':'));
	int iMin = clamp_val(_ttoi(strMin), 0, 59);
	if (strMin.GetLength() == 2 || bIsKillFocus)
		strMin.Format(_T("%02d"), iMin);
	else if (strMin.GetLength() == 1)
		strMin.Format(_T("%d"), iMin);

	if (bIsKillFocus) {
		strTemp = strHour + L":" + strMin;
		iPos = 2;
	}
	else if (iPos >= 0)
		strTemp = strHour + L":" + strMin;
	else 
		strTemp = strHour;

	m_strPreTimeString[bIsAM] = strTemp;
	editHelp->SetWindowText(strTemp);
	editHelp->SetSel(strTemp.GetLength(), strTemp.GetLength(), TRUE);
}

//void CBellRingDlg::TextInputFormatMinute(CEdit* editHelp)
//{
//	CString strTemp = _T("");
//	editHelp->GetWindowText(strTemp);
//	int len = strTemp.GetLength();
//
//	for (int i = 0; i < len; i++)
//	{
//		if (i<2)
//		{
//			if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
//			{
//				strTemp = strTemp.Left(i);
//				editHelp->SetWindowText(strTemp);
//				editHelp->SetSel(i, i, TRUE);
//
//				return;
//			}
//		} 
//		else
//		{
//			strTemp = strTemp.Left(i);
//			editHelp->SetWindowText(strTemp);
//			editHelp->SetSel(i, i, TRUE);
//		}
//	}
//}

void CBellRingDlg::SetTime()
{
	CString strTemp = _T("");
	CString strStartTime[2];
	GetDlgItemText(IDC_EDIT_AM, strTemp);
	strStartTime[0] = strTemp;

	GetDlgItemText(IDC_EDIT_PM, strTemp);
	strStartTime[1] = strTemp;

	m_pThreadParam->timeList.clear();

	for (int i = 0; i < 2; i++) {
		TimeOperate temp(strStartTime[i]);
		temp -= m_iBigGap;
		for (int j = 0; j < 4; j++) {
			int iClassGap = 0;
			if (j % 2 == 0)
				iClassGap = m_iBigGap;
			else
				iClassGap = m_iSmallGap;

			temp += iClassGap;
			m_pThreadParam->PushtimeList(temp, !i, m_iClassMin);
		}
	}

	ReSetRingFlag();
}

void CBellRingDlg::ReSetRingFlag() 
{
	m_pThreadParam->ringTimeFlag = 0;
	while (m_pThreadParam->ringTimeFlag < m_pThreadParam->timeList.size() && m_pThreadParam->timeList[m_pThreadParam->ringTimeFlag] < m_pThreadParam->strCurrentTime)
	{
		m_pThreadParam->ringTimeFlag++;
	}

	ShowTimeList();
}

void CBellRingDlg::SetTimeListSize()
{
	CListBox* ringList = m_pThreadParam->pTimeListBox;
	bool bVS = (ringList->GetStyle() & WS_VSCROLL);
	if (bVS == m_bIsTimeListBoxSized)
		return;
	
	int iSize;
	if (m_bIsTimeListBoxSized) {
		iSize = -20; m_bIsTimeListBoxSized = false;
	}
	else {
		iSize = 20; m_bIsTimeListBoxSized = true;
	}

	CRect rect;
	ringList->GetWindowRect(&rect);
	ringList->SetWindowPos(NULL, 0, 0, rect.Width() + iSize, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	GetWindowRect(&rect);
	SetWindowPos(NULL, 0, 0, rect.Width() + iSize, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
}

void CBellRingDlg::ShowTimeList()
{
	size_t i = m_pThreadParam->ringTimeFlag;
	CListBox* ringList = m_pThreadParam->pTimeListBox;
	ringList->ResetContent();
	while (i < m_pThreadParam->timeList.size())
	{
		CString title = i % 2 == 0 ? L"上课 - " : L"下课 - ";
		ringList->AddString(title + m_pThreadParam->timeList[i].GetValue());
		if (i % 2 != 0) ringList->AddString(L"");
		i++;
	}

	SetTimeListSize();
}

void CBellRingDlg::ReadRingFile()
{
	std::ifstream f("ring.wav", std::ios::binary);

	if (!f) {
		MessageBox(L"Missing \"ring.wav\"",L"Error!", MB_OK | MB_ICONERROR);
		return;
	}

	f.seekg(0, std::ios::end);
	int lim = f.tellg();
	m_pThreadParam->pRingInfo->dwFileSize = lim;

	m_pThreadParam->pRingInfo->pOriginalFileBytes = new BYTE[lim];
	m_pThreadParam->pRingInfo->pScaledFileBytes = new BYTE[lim];

	f.seekg(0, std::ios::beg);

	f.read((char *)m_pThreadParam->pRingInfo->pOriginalFileBytes, lim);
	
	f.close();

	memcpy(m_pThreadParam->pRingInfo->pScaledFileBytes, m_pThreadParam->pRingInfo->pOriginalFileBytes, lim);
}

void CBellRingDlg::ReadSettingFile()
{
	CFile cfg;
	CString strTimeAM = L"08:30";
	CString strTimePM = L"13:00";
	CString strClassTime = L"45";
	CString strBigGap = L"10";
	CString strSmallGap = L"5";
	bool bMute = false;
	int iPos = 100;
	if (cfg.Open(L"cfg", CFile::modeRead)) {
		CArchive ar(&cfg, CArchive::load);
		ar >> strTimeAM >> strTimePM >> strClassTime >> strBigGap >> strSmallGap >> bMute >> iPos;
		cfg.Close();
		//MessageBox(strTimeAM + strTimePM + strClassTime + strBigGap + strSmallGap);
	}

	GetDlgItem(IDC_EDIT_AM)->SetWindowText(strTimeAM);
	GetDlgItem(IDC_EDIT_PM)->SetWindowText(strTimePM);
	GetDlgItem(IDC_EDIT_CLASSTIME)->SetWindowText(strClassTime);
	GetDlgItem(IDC_EDIT_GAPBIG)->SetWindowText(strBigGap);
	GetDlgItem(IDC_EDIT_GAPSMALL)->SetWindowText(strSmallGap);

	CButton * pMuteButton = ((CButton *)GetDlgItem(IDC_CHECK1));
	pMuteButton->SetCheck(!bMute);
	pMuteButton->SendMessage(BM_CLICK);
	CSliderCtrl* pVolumeSlider = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1));
	pVolumeSlider->SetPos(iPos);
	pVolumeSlider->SendMessage(NM_CUSTOMDRAW);
	UpdateData();
}

void CBellRingDlg::SaveSettingToFile()
{
	CFile cfg(L"cfg", CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&cfg, CArchive::store);
	CString strTimeAM, strTimePM, strClassTime, strBigGap, strSmallGap;
	GetDlgItemText(IDC_EDIT_AM, strTimeAM);
	GetDlgItemText(IDC_EDIT_PM, strTimePM);
	GetDlgItemText(IDC_EDIT_CLASSTIME, strClassTime);
	GetDlgItemText(IDC_EDIT_GAPBIG, strBigGap);
	GetDlgItemText(IDC_EDIT_GAPSMALL, strSmallGap);
	bool bMute = m_pThreadParam->bIsMute;
	int iPos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->GetPos();
	ar << strTimeAM << strTimePM << strClassTime << strBigGap << strSmallGap << bMute << iPos;
}

void CBellRingDlg::OnEnChangeEditAM()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_AM)));
	TextInputFormatTime(editHelp, true, false);
}

void CBellRingDlg::OnEnChangeEditPM()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_PM)));
	TextInputFormatTime(editHelp, false, false);
}

void CBellRingDlg::OnEnChangeEditClassTime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_CLASSTIME)));
	//TextInputFormatMinute(editHelp);
	int iClassMin = m_iClassMin;
	if (!UpdateData()) {
		m_iClassMin = iClassMin;
		UpdateData(FALSE);
	}
}

void CBellRingDlg::OnEnChangeEditGapBig()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_GAPBIG)));
	//TextInputFormatMinute(editHelp);
	int iBigGap = m_iBigGap;
	if (!UpdateData()) {
		m_iBigGap = iBigGap;
		UpdateData(FALSE);
	}
}

void CBellRingDlg::OnEnChangeEditGapSmall()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_GAPSMALL)));
	//TextInputFormatMinute(editHelp);
	int iSmallGap = m_iSmallGap;
	if (!UpdateData()) {
		m_iSmallGap = iSmallGap;
		UpdateData(FALSE);
	}

}

void CBellRingDlg::OnKillFocusEditAM()
{
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_AM)));
	TextInputFormatTime(editHelp, true, true);
}

void CBellRingDlg::OnKillFocusEditPM()
{
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT_PM)));
	TextInputFormatTime(editHelp, false, true);
}

void CBellRingDlg::OnBnClickedButtonSettime()
{
	// TODO: Add your control notification handler code here
	if (true)
	{
		SetTime();
		SaveSettingToFile();
		MessageBox(_T("Set time OK"));
	}
	else
	{
		MessageBox(_T("Error"));
	}
}

void CBellRingDlg::OnBnClickedButtonHide()
{
	// TODO: Add your control notification handler code here
	CreateIconOnTray();
	this->ShowWindow(SW_HIDE);
}

void CBellRingDlg::CreateIconOnTray() 
{
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
	nid.hIcon = LoadIcon(GetModuleHandle(NULL)/*AfxGetInstanceHandle()*/, MAKEINTRESOURCE(IDR_MAINFRAME));
	//wcscpy(nid.szTip, L"BellRing"); //信息提示条
	swprintf_s(nid.szTip, L"BellRing"); //信息提示条
	Shell_NotifyIcon(NIM_ADD, &nid); //在托盘区添加图标
}


#ifdef _WIN64
LRESULT CBellRingDlg::OnTray(WPARAM wParam, LPARAM lParam)
#else
LRESULT CBellRingDlg::OnTray(UINT nID, LPARAM lParam)
#endif
{
	//if (wParam != IDR_MAINFRAME)
	//	return 1;

	switch (lParam)
	{
	case WM_RBUTTONUP:
	{}
	break;
	case WM_LBUTTONDBLCLK:
	{
		this->ShowWindow(SW_RESTORE);//简单的显示主窗口完事儿
		SetForegroundWindow();
		Shell_NotifyIcon(NIM_DELETE, &nid);//图标删除
	}
	break;
	}
	return 0;
}

void CBellRingDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	int iState = ((CButton *)GetDlgItem(IDC_CHECK1))->GetCheck();
	switch (iState) {
		//case 0:
		//	break;
	case 1:
		m_pThreadParam->bIsMute = true;
		break;
	default:
		m_pThreadParam->bIsMute = false;
		break;
	}
	SaveSettingToFile();
}

void CBellRingDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int iPos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->GetPos();

	if (m_pThreadParam->pRingInfo->fVolume != iPos / 100.0f) {
		m_pThreadParam->pRingInfo->fVolume = iPos / 100.0f;
		SaveSettingToFile();

		unsigned int	iThreadID;
		UINT_PTR		hThreadHandle;
		hThreadHandle = _beginthreadex(0, 0, VolumeThread, m_pThreadParam->pRingInfo, CREATE_SUSPENDED, &iThreadID);
		ResumeThread((HANDLE)hThreadHandle);
	}

	char buf[128];
	sprintf_s(buf, "fVolume:%f\n", m_pThreadParam->pRingInfo->fVolume);
	OutputDebugStringA(buf);

	*pResult = 0;
}

void CBellRingDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	delete m_pThreadParam->pRingInfo->pScaledFileBytes;
	m_pThreadParam->pRingInfo->pScaledFileBytes = NULL;
	delete m_pThreadParam->pRingInfo->pOriginalFileBytes;
	m_pThreadParam->pRingInfo->pOriginalFileBytes = NULL;
	delete m_pThreadParam->pRingInfo;
	m_pThreadParam->pRingInfo = NULL;
	delete m_pThreadParam;
	m_pThreadParam = NULL;
}

UINT CBellRingDlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData)
{
	if (nPowerEvent == PBT_APMRESUMEAUTOMATIC) ReSetRingFlag();
	return CDialogEx::OnPowerBroadcast(nPowerEvent, nEventData);
}

LRESULT CBellRingDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (!this->IsWindowVisible()) {
		CreateIconOnTray();
	}
	return 1;
}

bool SThreadParam::PushtimeList(TimeOperate & time, bool bIsAM, int iClassTime)
{
	TimeOperate after = (time + iClassTime);
	if (bIsAM == time.bIsAM() && bIsAM == after.bIsAM()) {
		timeList.push_back(time); timeList.push_back(time += iClassTime); 
		return true;
	}
	return false;
}
