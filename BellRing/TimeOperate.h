#pragma once
class TimeOperate
{
private:
	CString m_strTime;

public:
	TimeOperate(int x1);
	TimeOperate(CString s1);
	TimeOperate(TimeOperate& p);
	~TimeOperate();

public:
	CString GetValue();

private:
	int TimeToInt(CString time);
	CString TimeToCString(int time);

public:
	const TimeOperate operator+(const TimeOperate& p);
	const TimeOperate operator-(const TimeOperate& p);
	const TimeOperate operator+=(const TimeOperate& p);
	const TimeOperate operator-=(const TimeOperate& p);
	const bool operator>(const TimeOperate& p);
	const bool operator<(const TimeOperate& p);
	const bool operator>=(const TimeOperate& p);
	const bool operator<=(const TimeOperate& p);
	const bool operator==(const TimeOperate& p);
};

