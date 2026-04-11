#pragma once
#include <stack>
#include <set>

class CPathFinding
{
public:
	typedef std::pair<int, int> PAIR;
	typedef std::pair<double, PAIR> PAIRPAIR;

	struct PATH_CELL_INFO
	{
		int parent_i;
		int parent_j;
		double f, g, h;
	};

public:
	CPathFinding();
	virtual ~CPathFinding();
	BOOL FindPath(int sx, int sy, int dx, int dy);

private:
	bool IsValid(int x, int y);
	bool IsUnblocked(int x, int y);
	bool IsDest(int x, int y, PAIR dst);
	double CalcValue(int x, int y, PAIR dst);
	void Trace(PAIR dst);

private:
	PATH_CELL_INFO* m_CellInfo;
	bool* m_CloseList;

protected:
	std::deque<PAIR> m_WayPoint;
};

