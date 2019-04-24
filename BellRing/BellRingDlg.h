
// BellRingDlg.h : header file
//
#define WM_SHOWTASK (WM_USER +1)

#include "stdafx.h"
#include<iostream>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#include "afxdialogex.h"
#include "TimeOperate.h"
#pragma comment(lib, "winmm.lib")
#pragma once

struct SThreadParam
{
	unsigned int	iThreadID;
	UINT_PTR		hThreadHandle;

	CString currentTime;
	CString firstTime;
	CString secondTime;

	int classTime;
	int bigTime;
	int smallTime;

	std::vector<TimeOperate>timeList;
	size_t ringWait;

	CWnd* timeText;
	CListBox* timeListBox;
};

// CBellRingDlg dialog
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
	void TextInputFormatTime(CEdit* editHelp, bool bIsAM);
	void TextInputFormatMinute(CEdit* editHelp);
	void SetTime();
	void ReSetRingWait();
	void CreateIconOnTray();
	void ShowTimeList();

private:
	int m_iEditLen[2] = { 0 };
	CString m_strPreString[2] = {_T("")};

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
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
