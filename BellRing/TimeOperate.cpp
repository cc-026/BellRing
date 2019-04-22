#include "stdafx.h"
#include "TimeOperate.h"


TimeOperate::TimeOperate(int x1)
{
	x = TimeToCString(x1);
}

TimeOperate::TimeOperate(CString s1)
{
	x = s1;
}

TimeOperate::TimeOperate(TimeOperate& p)
{
	x = p.x;
}

TimeOperate::~TimeOperate()
{
}

CString TimeOperate::GetValue()
{
	return x;
}

int TimeOperate::TimeToInt(CString time)
{
	int i = time.Find(L":");
	if (i==-1)
	{
		return i;
	}

	int hour = _wtoi(time.Left(i));
	int length = time.Delete(0, i+1);
	time = time.Right(length);
	int minute = _wtoi(time);

	if (hour <= 24 && minute <= 60)
	{
		return hour * 60 + minute;
	}
	else
	{
		return -1;
	}
}

CString TimeOperate::TimeToCString(int time)
{
	int hour = time / 60;
	int minute = time - (hour * 60);

	CString timeCS;

	if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
	{
		timeCS.Format(_T("%d%d:%d%d"), hour / 10, hour % 10, minute / 10, minute % 10);
	}
	else
	{
		timeCS = L"Error!";
	}
	return timeCS;
}

const TimeOperate TimeOperate::operator+(const TimeOperate& p)
{

	if (TimeToInt(x) == -1 || TimeToInt(p.x) == -1)
	{
		return TimeOperate(L"Error!");
	}
	else
	{
		int returnValue = TimeToInt(x) + TimeToInt(p.x);
		int hour = returnValue / 60;
		while (!(hour >= 0 && hour < 24) )
		{
			if (hour <0)
			{
				returnValue += (24 * 60);
			} 
			if (hour >= 24)
			{
				returnValue -= (24 * 60);
			}
			hour = returnValue / 60;
		}
		return TimeOperate(returnValue);
	}

}

const TimeOperate TimeOperate::operator-(const TimeOperate& p)
{

	if (TimeToInt(x) == -1 || TimeToInt(p.x) == -1)
	{
		return TimeOperate(L"Error!");
	}
	else
	{
		int returnValue = TimeToInt(x) - TimeToInt(p.x);
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
		return TimeOperate(returnValue);
	}

}

const TimeOperate TimeOperate::operator+=(const TimeOperate& p)
{

	if (TimeToInt(x) == -1 || TimeToInt(p.x) == -1)
	{
		x = L"Error!";
	}
	else
	{
		int returnValue = TimeToInt(x) + TimeToInt(p.x);
		int hour = returnValue / 60;
		while (!(hour >= 0 && hour < 24))
		{
			if (hour < 0)
			{
				returnValue += (24 * 60);
			}
			if(hour>= 24)
			{
				returnValue -= (24 * 60);
			}
			hour = returnValue / 60;
		}

		x = TimeToCString(returnValue);
	}

	return *this;
}

const TimeOperate TimeOperate::operator-=(const TimeOperate& p)
{

	if (TimeToInt(x) == -1 || TimeToInt(p.x) == -1)
	{
		x = L"Error!";
	}
	else
	{
		int returnValue = TimeToInt(x) - TimeToInt(p.x);
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

		x = TimeToCString(returnValue);
	}

	return *this;
}

const bool TimeOperate::operator<(const TimeOperate& p)
{
	return TimeToInt(x) < TimeToInt(p.x);
}

const bool TimeOperate::operator>(const TimeOperate& p)
{
	return TimeToInt(x) > TimeToInt(p.x);
}

const bool TimeOperate::operator<=(const TimeOperate& p)
{
	return TimeToInt(x) <= TimeToInt(p.x);
}

const bool TimeOperate::operator>=(const TimeOperate& p)
{
	return TimeToInt(x)  >= TimeToInt(p.x);
}

const bool TimeOperate::operator==(const TimeOperate& p)
{
	return TimeToInt(x) == TimeToInt(p.x);
}