//////////////////////////////////////////////////////////////////////////
//  GOBoid.h
//////////////////////////////////////////////////////////////////////////
#ifndef __GOBOID_H__
#define __GOBOID_H__

#include "zzzObject.h"

#define BOID_FLY		0
#define BOID_DOWN		1
#define BOID_GROUND		2
#define BOID_UP			3

typedef struct
{
	int nType;
	int RenderIndex;
	int MovementType;
	float RenderScale;
}PET_GOBOID;





void CreateGoboId(int ItemIndex, vec3_t Position, OBJECT* Owner, int SubType = 0, int LinkBone = 0);
void CreateSubGoboId(int ItemIndex, vec3_t Position, OBJECT* Owner, OBJECT* o, int SubType = 0, int LinkBone = 0);

//void CreateBug(int Type, vec3_t Position, OBJECT* Owner, int SubType = 0, int LinkBone = 0);



bool IsBug(ITEM* pItem);
bool isGoBoidItem(int ItemIndex);


bool CreateBugSub(int Type, vec3_t Position, OBJECT* Owner, OBJECT* o, int SubType = 0, int LinkBone = 0);
bool MoveBug(OBJECT* o, bool bForceRender = false);
bool RenderBug(OBJECT* o, bool bForceRender = false);
void RenderDarkHorseSkill(OBJECT* o, BMD* b);
void RenderSkillEarthQuake(CHARACTER* c, OBJECT* o, BMD* b, int iMaxSkill = 30);



void MoveBoids(void);
void RenderBoids(bool bAfterCharacter = false);
void DeleteBoids(void);

void MoveFishs(void);
void RenderFishs(void);

void GoBoidREG(int nType, int Type, int Movement, float scale);



class CGoboidManager
{
public:
	CGoboidManager(){
		petList[0] = std::vector<OBJECT>(MAX_BUTTERFLES);
		petList[1] = std::vector<OBJECT>(MAX_BUTTERFLES);
		petList[2] = std::vector<OBJECT>(MAX_BUTTERFLES);

		for (int i = 0; i < MAX_BUTTERFLES; i++)
		{
			petList[0][i].Initialize();
			petList[1][i].Initialize();
			petList[2][i].Initialize();
		}
	};
	virtual~CGoboidManager() {
		petList[0].clear();
		petList[1].clear();
		petList[2].clear();
	};

	void CreateBug(int Type, vec3_t Position, OBJECT* Owner, int SubType = 0, int targetIndex = 0);
	void DeleteBug(OBJECT* Owner, int targetIndex = -1);

	void MoveBugs();
	void RenderBugs();

	static CGoboidManager* Instance();
private:
	std::vector<OBJECT> petList[3];
};

#define gGoboidManager			(CGoboidManager::Instance())
extern std::map<int, PET_GOBOID> goBoidList;
#endif// __GOBOID_H__