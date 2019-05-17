#include "stdafx.h"
#include <commctrl.h>
#include <process.h>

#include "traypos.h"
#include "resource.h"

CTrayPos::CTrayPos()
{
	UINT	uThreadId;

	m_bTrackMouse = FALSE;
	m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, CTrayPos::TrackMousePt, this, 0, &uThreadId);
	InitializeCriticalSection(&m_cs);
}

CTrayPos::~CTrayPos()
{
	if(m_hThread != NULL)
	{
		SetEvent(m_hExitEvent);
		if(WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hExitEvent != NULL)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

UINT CALLBACK CTrayPos::TrackMousePt(PVOID pvClass)
{
	POINT		ptMouse;
	CTrayPos	*pTrayPos = (CTrayPos *) pvClass;

	while(WaitForSingleObject(pTrayPos->m_hExitEvent, 2000) == WAIT_TIMEOUT)
	{

		if(pTrayPos->m_bTrackMouse == TRUE)
		{
			GetCursorPos(&ptMouse);
			
			if(ptMouse.x != pTrayPos->m_ptMouse.x || ptMouse.y != pTrayPos->m_ptMouse.y)
			{
				pTrayPos->m_bTrackMouse = FALSE;
				pTrayPos->OnMouseLeave();
			}
		}
	}

	return 0;
}

VOID CTrayPos::OnMouseMove()
{
	EnterCriticalSection(&m_cs);

	GetCursorPos(&m_ptMouse);
	if(m_bTrackMouse == FALSE)
	{
		OnMouseHover();
		m_bTrackMouse = TRUE;
	}

	LeaveCriticalSection(&m_cs);
}

BOOL CTrayPos::IsMouseHover()
{
	return m_bTrackMouse;
}

//////////////////////////////////////////////////////////////////////////

CMsgTrayPos::CMsgTrayPos(HWND hwnd, UINT uID, UINT uCallbackMsg)
	: CTrayPos()
{
	SetNotifyIconInfo(hwnd, uID, uCallbackMsg);
}

CMsgTrayPos::~CMsgTrayPos()
{
}

VOID CMsgTrayPos::SetNotifyIconInfo(HWND hwnd, UINT uID, UINT uCallbackMsg)
{
	m_hNotifyWnd = hwnd;
	m_uID = uID;
	m_uCallbackMsg = uCallbackMsg;
}

VOID CMsgTrayPos::OnMouseHover()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSEHOVER);
}

VOID CMsgTrayPos::OnMouseLeave()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSELEAVE);
}