#include "stdafx.h"
#include "ZzzLodTerrain.h"
#include "PathFinding.h"

#define FIND_PATH(x,y,add) \
if (this->IsValid(x,y)!=0) \
{ \
	if (this->IsDest(x,y,dst)!=0) \
	{ \
		this->m_CellInfo[TERRAIN_INDEX(x,y)].parent_i = i; \
		this->m_CellInfo[TERRAIN_INDEX(x,y)].parent_j = j; \
		this->Trace(dst); \
		found = 1; \
		break; \
	} \
	else if (this->m_CloseList[TERRAIN_INDEX(x,y)]==0&&this->IsUnblocked(x,y)!=0) \
	{ \
		g = this->m_CellInfo[TERRAIN_INDEX(i,j)].g+add; \
		h = this->CalcValue(x,y,dst); \
		f = g+h; \
		if (this->m_CellInfo[TERRAIN_INDEX(x,y)].f==FLT_MAX||this->m_CellInfo[TERRAIN_INDEX(x,y)].f>f) \
		{ \
			list.insert(std::make_pair(f,std::make_pair(x,y))); \
			this->m_CellInfo[TERRAIN_INDEX(x,y)].f = f; \
			this->m_CellInfo[TERRAIN_INDEX(x,y)].g = g; \
			this->m_CellInfo[TERRAIN_INDEX(x,y)].h = h; \
			this->m_CellInfo[TERRAIN_INDEX(x,y)].parent_i = i; \
			this->m_CellInfo[TERRAIN_INDEX(x,y)].parent_j = j; \
		} \
	} \
}

CPathFinding::CPathFinding()
{
	this->m_CellInfo = new PATH_CELL_INFO[TERRAIN_SIZE * TERRAIN_SIZE];
	this->m_CloseList = new bool[TERRAIN_SIZE * TERRAIN_SIZE];
}

CPathFinding::~CPathFinding()
{
	if (this->m_CellInfo)
	{
		delete[]m_CellInfo;
		this->m_CellInfo = 0;
	}

	if (this->m_CloseList)
	{
		delete[]m_CloseList;
		this->m_CloseList = 0;
	}
}

bool CPathFinding::IsValid(int x, int y)
{
	return x >= 0 && x < TERRAIN_SIZE&& y >= 0 && y < TERRAIN_SIZE;
}

bool CPathFinding::IsUnblocked(int x, int y)
{
	WORD att = TerrainWall[TERRAIN_INDEX(x, y)];

	return ((att & 4) == 0) && ((att & 8) == 0);
}

bool CPathFinding::IsDest(int x, int y, PAIR dst)
{
	return x == dst.first && y == dst.second;
}

double CPathFinding::CalcValue(int x, int y, PAIR dst)
{
	return sqrt(pow((double)(x - dst.first), 2) + pow((double)(y - dst.second), 2));
}

void CPathFinding::Trace(PAIR dst)
{
	int x = dst.first;
	int y = dst.second;

	int count = 0;

	std::stack<PAIR> path;
	while (!(this->m_CellInfo[TERRAIN_INDEX(x, y)].parent_i == x
		&& this->m_CellInfo[TERRAIN_INDEX(x, y)].parent_j == y))
	{
		path.push(std::make_pair(x, y));
		int tx = this->m_CellInfo[TERRAIN_INDEX(x, y)].parent_i;
		int ty = this->m_CellInfo[TERRAIN_INDEX(x, y)].parent_j;
		x = tx;
		y = ty;
	}

	path.push(std::make_pair(x, y));
	while (!path.empty())
	{
		std::pair<int, int> p = path.top();
		path.pop();
		if (((count++) % 7) == 0 || path.empty())
		{
			this->m_WayPoint.push_back(p);
		}
	}
}

BOOL CPathFinding::FindPath(int sx, int sy, int dx, int dy)
{
	PAIR src = std::make_pair(sx, sy);
	PAIR dst = std::make_pair(dx, dy);

	if (this->IsValid(src.first, src.second) == 0 || this->IsValid(dst.first, dst.second) == 0)
	{
		printf("path IsValid\n");
		return 0;
	}

	if (this->IsUnblocked(src.first, src.second) == 0 || this->IsUnblocked(dst.first, dst.second) == 0)
	{
		printf("path IsUnblocked\n");
		return 0;
	}

	if (this->IsDest(src.first, src.second, dst) != 0)
	{
		printf("path IsDest\n");
		return 0;
	}

	memset(this->m_CloseList, 0, TERRAIN_SIZE * TERRAIN_SIZE);

	this->m_WayPoint.clear();

	int i, j;
	for (i = 0; i < TERRAIN_SIZE; i++)
	{
		for (j = 0; j < TERRAIN_SIZE; j++)
		{
			this->m_CellInfo[TERRAIN_INDEX(i, j)].f = FLT_MAX;
			this->m_CellInfo[TERRAIN_INDEX(i, j)].g = FLT_MAX;
			this->m_CellInfo[TERRAIN_INDEX(i, j)].h = FLT_MAX;
			this->m_CellInfo[TERRAIN_INDEX(i, j)].parent_i = -1;
			this->m_CellInfo[TERRAIN_INDEX(i, j)].parent_j = -1;
		}
	}

	i = src.first;
	j = src.second;

	this->m_CellInfo[TERRAIN_INDEX(i, j)].f = 0.0;
	this->m_CellInfo[TERRAIN_INDEX(i, j)].g = 0.0;
	this->m_CellInfo[TERRAIN_INDEX(i, j)].h = 0.0;
	this->m_CellInfo[TERRAIN_INDEX(i, j)].parent_i = i;
	this->m_CellInfo[TERRAIN_INDEX(i, j)].parent_j = j;

	std::set<PAIRPAIR> list;
	list.insert(std::make_pair(0.0, std::make_pair(i, j)));

	bool found = false;

	while (!list.empty())
	{
		std::set<PAIRPAIR>::iterator it = list.begin();

		i = it->second.first;
		j = it->second.second;

		list.erase(list.begin());

		this->m_CloseList[TERRAIN_INDEX(i, j)] = 1;

		double f, g, h;

		FIND_PATH(i - 1, j, 1.0);
		FIND_PATH(i + 1, j, 1.0);
		FIND_PATH(i, j + 1, 1.0);
		FIND_PATH(i, j - 1, 1.0);
		FIND_PATH(i - 1, j + 1, 1.414);
		FIND_PATH(i - 1, j - 1, 1.414);
		FIND_PATH(i + 1, j + 1, 1.414);
		FIND_PATH(i + 1, j - 1, 1.414);
	}

	return (found == 0) ? 0 : 1;
}
