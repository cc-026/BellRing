#include "stdafx.h"
#include "TimeOperate.h"
#include "time.h"


TimeOperate::TimeOperate(int iMin)
{
	this->m_strTime = TimeToCString(iMin);
}

TimeOperate::TimeOperate(CString strTime)
{
	this->m_strTime = strTime;
}

TimeOperate::TimeOperate(TimeOperate& p)
{
	this->m_strTime = p.m_strTime;
}

TimeOperate::~TimeOperate()
{
}

CString TimeOperate::GetValue()
{
	return this->m_strTime;
}

int TimeOperate::TimeToInt(CString strTime)
{
	int i = strTime.Find(L":");
	if (i==-1)
	{
		return i;
	}

	int iHour = _wtoi(strTime.Left(i));
	int iLength = strTime.Delete(0, i+1);
	strTime = strTime.Right(iLength);
	int iMinute = _wtoi(strTime);

	if (iHour <= 24 && iMinute <= 60)
	{
		return iHour * 60 + iMinute;
	}
	else
	{
		return -1;
	}
}

CString TimeOperate::TimeToCString(int time)
{
	int iHour = time / 60;
	int iMinute = time - (iHour * 60);

	CString strTime;

	if (iHour >= 0 && iHour < 24 && iMinute >= 0 && iMinute < 60)
	{
		strTime.Format(_T("%02d:%02d"), iHour, iMinute);
	}
	else
	{
		strTime = L"Error!";
	}
	return strTime;
}

const TimeOperate TimeOperate::operator+(const TimeOperate& p)
{

	if (TimeToInt(m_strTime) == -1 || TimeToInt(p.m_strTime) == -1)
	{
		return TimeOperate(L"Error!");
	}
	else
	{
		int iReturnValue = TimeToInt(m_strTime) + TimeToInt(p.m_strTime);
		int iHour = iReturnValue / 60;
		while (!(iHour >= 0 && iHour < 24) )
		{
			if (iHour <0)
			{
				iReturnValue += (24 * 60);
			} 
			if (iHour >= 24)
			{
				iReturnValue -= (24 * 60);
			}
			iHour = iReturnValue / 60;
		}
		return TimeOperate(iReturnValue);
	}

}

const TimeOperate TimeOperate::operator-(const TimeOperate& p)
{

	if (TimeToInt(m_strTime) == -1 || TimeToInt(p.m_strTime) == -1)
	{
		return TimeOperate(L"Error!");
	}
	else
	{
		int iReturnValue = TimeToInt(m_strTime) - TimeToInt(p.m_strTime);
		int iHour = iReturnValue / 60;
		while (!(iHour >= 0 && iHour < 24))
		{
			if (iHour < 0)
			{
				iReturnValue += (24 * 60);
			}
			if (iHour >= 24)
			{
				iReturnValue -= (24 * 60);
			}
			iHour = iReturnValue / 60;
		}
		return TimeOperate(iReturnValue);
	}

}

const TimeOperate TimeOperate::operator+=(const TimeOperate& p)
{

	if (TimeToInt(m_strTime) == -1 || TimeToInt(p.m_strTime) == -1)
	{
		m_strTime = L"Error!";
	}
	else
	{
		int iReturnValue = TimeToInt(m_strTime) + TimeToInt(p.m_strTime);
		int iHour = iReturnValue / 60;
		while (!(iHour >= 0 && iHour < 24))
		{
			if (iHour < 0)
			{
				iReturnValue += (24 * 60);
			}
			if(iHour >= 24)
			{
				iReturnValue -= (24 * 60);
			}
			iHour = iReturnValue / 60;
		}

		m_strTime = TimeToCString(iReturnValue);
	}

	return *this;
}

const TimeOperate TimeOperate::operator-=(const TimeOperate& p)
{

	if (TimeToInt(m_strTime) == -1 || TimeToInt(p.m_strTime) == -1)
	{
		m_strTime = L"Error!";
	}
	else
	{
		int returnValue = TimeToInt(m_strTime) - TimeToInt(p.m_strTime);
		int hour = returnValue / 60;
		while (!(hour >= 0 && hour < 24))
		{
			if (hour < 0)
			{
				returnValue += (24 * 60);
			}
			if (hour >= 24)
			{
				returnValue -= (24 * 60);
			}
			hour = returnValue / 60;
		}

		m_strTime = TimeToCString(returnValue);
	}

	return *this;
}

const bool TimeOperate::operator<(const TimeOperate& p)
{
	return TimeToInt(m_strTime) < TimeToInt(p.m_strTime);
}

const bool TimeOperate::operator>(const TimeOperate& p)
{
	return TimeToInt(m_strTime) > TimeToInt(p.m_strTime);
}

const bool TimeOperate::operator<=(const TimeOperate& p)
{
	return TimeToInt(m_strTime) <= TimeToInt(p.m_strTime);
}

const bool TimeOperate::operator>=(const TimeOperate& p)
{
	return TimeToInt(m_strTime)  >= TimeToInt(p.m_strTime);
}

const bool TimeOperate::operator==(const TimeOperate& p)
{
	return TimeToInt(m_strTime) == TimeToInt(p.m_strTime);
}