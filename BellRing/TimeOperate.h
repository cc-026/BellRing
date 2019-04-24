#pragma once
class TimeOperate
{
private:
	CString m_strTime;

public:
	TimeOperate(int iMin);
	TimeOperate(CString strTime);
	TimeOperate(const TimeOperate& p);
	~TimeOperate();

public:
	CString GetValue();
	bool bIsAM();

private:
	int TimeToInt(CString strTime);
	CString TimeToCString(int iTime);

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

