
// BellRingDlg.h : header file
//
#define WM_SHOWTASK (WM_USER +1)

#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#include "afxdialogex.h"
#include "TimeOperate.h"
#include "traypos.h"
#pragma comment(lib, "winmm.lib")
#pragma once

// CBellRingDlg dialog
struct SWaveInfo {
	DWORD dwFileSize;
	float fVolume;
	BYTE* pOriginalFileBytes;
	BYTE* pScaledFileBytes;
};

struct SThreadParam
{
	unsigned int iThreadID;
	UINT_PTR hThreadHandle;

	bool bIsMute;
	size_t ringTimeFlag;
	CString strCurrentTime;
	std::vector<TimeOperate>timeList;

	SWaveInfo* pRingInfo;
	CWnd* pParentCBellRingDlg;
	CWnd* pWndTimeText;
	CListBox* pTimeListBox;

	bool PushtimeList(TimeOperate &time, bool bIsAM, int iClassTime); 
};

class CBellRingDlg : public CDialogEx
{
// Construction
public:
	CBellRingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BELLRING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	NOTIFYICONDATA nid;

private:
	void TextInputFormatTime(CEdit* editHelp, bool bIsAM, bool bIsKillFocus);
	//void TextInputFormatMinute(CEdit* editHelp);
	void SetTime();
	void CreateIconOnTray();
	void ShowTimeList();
	void ReadRingFile();
	void ReadSettingFile();
	void SaveSettingToFile();

public:
	void ReSetRingFlag();
	void SetTimeListSize();

private:
	SThreadParam* m_pThreadParam;
	CString m_strPreTimeString[2] = { _T("") };
	bool m_bIsTimeListBoxSized = false;
	CMsgTrayPos	m_traypos;

// Implementation
protected:
	HICON m_hIcon;
	BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
#ifdef _WIN64
	afx_msg LRESULT OnTray(WPARAM wParam, LPARAM lParam);
#else
	afx_msg LRESULT OnTray(UINT nID, LPARAM lParam);
#endif
	//afx_msg LRESULT OnTray(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditAM();
	afx_msg void OnEnChangeEditPM();
	afx_msg void OnEnChangeEditClassTime();
	afx_msg void OnEnChangeEditGapBig();
	afx_msg void OnEnChangeEditGapSmall();
	afx_msg void OnKillFocusEditAM();
	afx_msg void OnKillFocusEditPM();
	afx_msg void OnBnClickedButtonSettime();
	afx_msg void OnBnClickedButtonHide();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData);
	afx_msg LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);
public:
	int m_iClassMin;
	int m_iBigGap;
	int m_iSmallGap;

};



