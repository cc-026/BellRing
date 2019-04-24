
// BellRingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BellRing.h"
#include "BellRingDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define clamp_val(val, lo, hi) min(max(val, lo), hi)

SThreadParam* m_oaThreadParam;

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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBellRingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBellRingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOWTASK, OnTray)
	ON_EN_CHANGE(IDC_EDIT1, &CBellRingDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CBellRingDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CBellRingDlg::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &CBellRingDlg::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &CBellRingDlg::OnEnChangeEdit5)
	ON_BN_CLICKED(IDC_BUTTON1, &CBellRingDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CBellRingDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

unsigned __stdcall RingThread(void* pParam)
{
	PlaySound(TEXT("ring.wav"), NULL, SND_FILENAME);

	return 0;
}

unsigned __stdcall TimeShowThread(void* pParam)
{
	if (!pParam)
	{
		std::cout << "No param\n";
		return 0;
	}

	SThreadParam* pThreadParam = (SThreadParam*)pParam;


	while (true)
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		pThreadParam->currentTime.Format(_T("%d%d:%d%d"), st.wHour / 10, st.wHour % 10, st.wMinute / 10, st.wMinute % 10);
		CString timeShow;
		timeShow.Format(_T("%s:%d%d"), pThreadParam->currentTime, st.wSecond / 10, st.wSecond % 10);
		pThreadParam->timeText->SetWindowText((LPCTSTR)timeShow);
		if (pThreadParam->ringWait < pThreadParam->timeList.size() && pThreadParam->timeList[pThreadParam->ringWait] == pThreadParam->currentTime)
		{
			pThreadParam->timeListBox->DeleteString(0);

			unsigned int	iThreadID;
			UINT_PTR		hThreadHandle;
			hThreadHandle = _beginthreadex(0, 0, RingThread, NULL, CREATE_SUSPENDED, &iThreadID);
			ResumeThread((HANDLE)hThreadHandle);
			//PlaySound(TEXT("ring.wav"), NULL, SND_FILENAME);
			pThreadParam->ringWait++;
		}
		Sleep(1000);
	}
}

// CBellRingDlg message handlers

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

	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);	//得到窗口类型
	//style &= ~(WS_CAPTION);	//修改窗口类型，显示标题栏
	::SetWindowLong(m_hWnd, GWL_STYLE, style);	//设置窗口类型

	m_oaThreadParam = new SThreadParam();
	m_oaThreadParam->timeText = this->GetDlgItem(IDC_TIMETEXT_STATIC);
	m_oaThreadParam->timeListBox = ((CListBox*)(GetDlgItem(IDC_LIST1)));
	m_oaThreadParam->hThreadHandle = _beginthreadex(0, 0, TimeShowThread, m_oaThreadParam, CREATE_SUSPENDED, &(m_oaThreadParam->iThreadID));
	ResumeThread((HANDLE)m_oaThreadParam->hThreadHandle);
	
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT1))); 
	editHelp->SetWindowText(L"08:30");
	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT2)));
	editHelp->SetWindowText(L"13:00");
	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT3)));
	editHelp->SetWindowText(L"45");
	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT4)));
	editHelp->SetWindowText(L"10");
	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT5)));
	editHelp->SetWindowText(L"5");
	SetTime();
	ReSetRingWait();

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
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// 如果消息是键盘按下事件，且是Entert键，执行以下代码（什么都不做，你可以自己添加想要的代码）
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CBellRingDlg::TextInputFormatTime(CEdit* editHelp, bool bIsAM)
{
	CString strTemp = _T("");
	editHelp->GetWindowText(strTemp);
	int iPreTextLen = m_strPreString[bIsAM].GetLength();
	if (strTemp == m_strPreString[bIsAM])
		return;
	m_strPreString[bIsAM] = strTemp;

	int iPos = strTemp.Find(_T(":")) < 2 && strTemp.Find(_T(":")) > 0 ? strTemp.Find(_T(":")) : 2;
	if(strTemp.GetLength() > iPos + 3)
		strTemp = strTemp.Left(iPos + 3);
	if (strTemp.GetLength() == iPos && strTemp.GetLength() > iPreTextLen)
		strTemp.AppendChar(L':');
	if (strTemp.GetLength() == iPos+1 && strTemp.GetLength() < iPreTextLen)
		strTemp = strTemp.Left(iPos);

	CString strHour;
	AfxExtractSubString(strHour, strTemp, 0, _T(':'));
	int iHour = bIsAM ? clamp_val(_ttoi(strHour), 0, 11) : clamp_val(_ttoi(strHour), 12, 23);
	int iHl = strHour.GetLength();
	if (iHl > 1)
		strHour.Format(_T("%02d"), iHour);
	else
		strHour.Format(_T("%d"), iHour);

	CString strMin;
	AfxExtractSubString(strMin, strTemp, 1, _T(':'));
	int iMin = clamp_val(_ttoi(strMin), 0, 59);
	int iMl = strMin.GetLength();
	if (iMl > 1)
		strMin.Format(_T("%02d"), iMin);
	else
		strMin.Format(_T("%d"), iMin);

	char buf[1024];
	sprintf_s(buf, "editHelplen:%d,%d\n", iPreTextLen, strTemp.GetLength());
	OutputDebugStringA(buf);

	for (int i = 0; i < strTemp.GetLength(); i++)
	{
		if (i < iPos) {
			strTemp.SetAt(i, strHour.GetAt(i));
		}

		if (i > iPos) {
			strTemp.SetAt(i, strMin.GetAt(i - iPos - 1));
		}

		if (i == iPos)
		{
			strTemp.SetAt(i, L':');
		}
		else if (i < iPos + 3)
		{
			if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
			{
				strTemp = strTemp.Left(i);
			}
		}
	}

	editHelp->SetWindowText(strTemp);
	editHelp->SetSel(strTemp.GetLength(), strTemp.GetLength(), TRUE);
}

void CBellRingDlg::TextInputFormatMinute(CEdit* editHelp)
{
	CString strTemp = _T("");
	editHelp->GetWindowText(strTemp);
	int len = strTemp.GetLength();

	for (int i = 0; i < len; i++)
	{
		if (i<2)
		{
			if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
			{
				strTemp = strTemp.Left(i);
				editHelp->SetWindowText(strTemp);
				editHelp->SetSel(i, i, TRUE);

				return;
			}
		} 
		else
		{
			strTemp = strTemp.Left(i);
			editHelp->SetWindowText(strTemp);
			editHelp->SetSel(i, i, TRUE);
		}
	}
}

void CBellRingDlg::SetTime()
{
	CEdit*editHelp;
	CString strTemp = _T("");
	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT1)));
	editHelp->GetWindowText(strTemp);
	m_oaThreadParam->firstTime = strTemp;

	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT2)));
	editHelp->GetWindowText(strTemp);
	m_oaThreadParam->secondTime = strTemp;

	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT3)));
	editHelp->GetWindowText(strTemp);
	m_oaThreadParam->classTime = _wtoi(strTemp);

	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT4)));
	editHelp->GetWindowText(strTemp);
	m_oaThreadParam->bigTime = _wtoi(strTemp);

	editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT5)));
	editHelp->GetWindowText(strTemp);
	m_oaThreadParam->smallTime = _wtoi(strTemp);

	m_oaThreadParam->timeList.clear();
	TimeOperate temp(-1);
	//AM
	temp = m_oaThreadParam->firstTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->smallTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->bigTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->smallTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	//PM
	temp = m_oaThreadParam->secondTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->smallTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->bigTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));

	temp += m_oaThreadParam->smallTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
	temp += m_oaThreadParam->classTime;
	m_oaThreadParam->timeList.push_back(TimeOperate(temp));
}

void CBellRingDlg::ReSetRingWait() 
{
	m_oaThreadParam->ringWait = 0;
	while (m_oaThreadParam->ringWait < m_oaThreadParam->timeList.size() && m_oaThreadParam->timeList[m_oaThreadParam->ringWait] < m_oaThreadParam->currentTime)
	{
		m_oaThreadParam->ringWait++;
	}

	ShowTimeList();
}

void CBellRingDlg::ShowTimeList()
{
	size_t i = m_oaThreadParam->ringWait;
	CListBox* ringList = ((CListBox*)(GetDlgItem(IDC_LIST1)));
	ringList->ResetContent();
	while (i < m_oaThreadParam->timeList.size())
	{
		ringList->AddString(m_oaThreadParam->timeList[i].GetValue());
		i++;
	}
}

void CBellRingDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT1)));
	TextInputFormatTime(editHelp, true);
}

void CBellRingDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT2)));
	TextInputFormatTime(editHelp, false);
}

void CBellRingDlg::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT3)));
	TextInputFormatMinute(editHelp);
}

void CBellRingDlg::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT4)));
	TextInputFormatMinute(editHelp);
}

void CBellRingDlg::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit* editHelp = ((CEdit*)(GetDlgItem(IDC_EDIT5)));
	TextInputFormatMinute(editHelp);
}

void CBellRingDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (true)
	{
		SetTime();
		ReSetRingWait();
		MessageBox(_T("Set time OK"));
	}
	else
	{
		MessageBox(_T("Error"));
	}
}

void CBellRingDlg::OnBnClickedButton2()
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
	//strcpy(nid.szTip, "程序名称"); //信息提示条
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
