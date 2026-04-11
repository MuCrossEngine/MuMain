#ifndef __ZZZINTERFACE_H__
#define __ZZZINTERFACE_H__

#define TRADELIMITLEVEL		( 6 )

typedef struct
{
	char      Text[256];
	int       LifeTime;
	BYTE      Color;
} NOTICE;

#define MAX_NOTICE 6

extern int    NoticeCount;
extern int    NoticeTime;
extern NOTICE Notice[MAX_NOTICE];

extern bool WhisperEnable;

extern bool LockInputStatus;

extern bool g_bAutoGetItem;

extern bool InputEnable;
extern bool TabInputEnable;
extern bool GuildInputEnable;
extern bool GoldInputEnable;
extern bool g_bScratchTicket;

extern int  InputGold;
extern int  InputResidentNumber;
extern int  InputNumber;
extern int  InputIndex;
extern int  InputTextWidth;
extern int  InputTextMax[12];
extern char InputText[12][256];
extern char InputTextIME[12][4];
extern int  InputLength[12];
extern char InputTextHide[12];
extern char MacroText[10][256];
extern int MacroTime;
extern int  FontHeight;
extern int  ItemHelp;
extern bool DontMove;
extern int  EditFlag;
extern int  SelectMonster;
extern int  SelectModel;
extern int  SelectMapping;
extern bool DebugEnable;
extern bool MouseOnWindow;
extern int  MouseUpdateTime;
extern int  MouseUpdateTimeMax;
extern int  TalkNPC;
extern int  SelectedItem;
extern int  SelectedNpc;
extern int  SelectedCharacter;
extern int  SelectedOperate;
extern int  SelectedHero;
extern int  Attacking;

extern int   g_iFollowCharacter;
extern int  HeroTile;
extern int  LoadingWorld;

extern int ItemKey;
extern bool g_bGMObservation;

void Action(CHARACTER* c, OBJECT* o, bool Now);

bool PressKey(int Key);
void MoveHero();
void EditObjects();

void ClearInput(BOOL bClearWhisperTarget = TRUE);
void CutText(const char* Text, char* Text1, char* Text2, int Length);
void ClearNotice(void);
void CreateNotice(char* Text, int Color);
void MoveNotices();
void RenderNotices();
void CreateWhisper(char* ID, const char* Text, int Type);
void MoveWhispers();
void RenderSwichState();
void CheckChatText(char* Text);

//  Whisper
bool CheckWhisperLevel(int lvl, char* text);
void RegistWhisperID(int lvl, char* text);
void ClearWhisperID(void);

void SaveIME_Status();
void SetIME_Status(bool halfShape);
bool CheckIME_Status(bool change, int mode);
void RenderIME_Status();

//  HotKey
int  FindHotKey(int Skill);

void RenderInputText(int x, int y, int Index, int Hide = 0);
void RenderDebugWindow();
void RenderTipText(int sx, int sy, const char* Text);

extern int g_iWidthEx;


void SelectObjects();
void RenderInterface(bool);
void MoveInterface();
void RenderCursor();
bool CheckArrow();
void ReloadArrow();
int SearchArrowCount();

extern void SetCreateMaxClass(BYTE MaxClass);
extern BYTE GetCreateMaxClass();
int SelectCharacter(BYTE Kind);
bool FindText(const char* Text, const char* Token, bool First = false);
bool FindTextABS(const char* Text, const char* Token, bool First = false);
bool CheckAbuseFilter(char* Text, bool bCheckSlash = true);
bool CheckAbuseNameFilter(char* Text);
bool CheckCommand(char* Text, bool bMacroText = false);
void SetActionClass(CHARACTER* c, OBJECT* o, int Action, int ActionType);
void RenderBar(float x, float y, float Width, float Height, float Bar, bool Disabled = false, bool clipping = true);
void RenderOutSides();
void RenderInventoryInterface();
int  RenderLimitTime(bool arrow);
void RenderTimes();
void RenderHPBar(int ScreenX, int ScreenY, float Width, int stepHP, bool scale);
void RenderPartyHP();
void SetPositionIME_Wnd(float x, float y);

void SendMove(CHARACTER* c, OBJECT* o);
bool CheckAttack();
bool CheckAttack_Fenrir(CHARACTER* c);
int	 getTargetCharacterKey(CHARACTER* c, int selected, int Key);

void MoveTournamentInterface();
void MoveBattleSoccerEffect(CHARACTER* c);
void RenderTournamentInterface();

void GetTime(DWORD time, std::string& timeText, bool isSecond = true);

extern int   TargetNpc;

// skill.
extern bool IsCanBCSkill(int nType);
extern bool CheckSkillUseCondition(OBJECT* o, int Type);
extern bool CheckWall(int sx1, int sy1, int sx2, int sy2, bool NoGround = false);
extern bool CheckTile(CHARACTER* c, int PositionX, int PositionY, float Range);
extern bool CheckTile(CHARACTER* c, float Range);
extern bool CheckTile(CHARACTER* c, OBJECT* o, float Range);
extern void LetHeroStop(CHARACTER* pCharacter = NULL, BOOL bSetMovementFalse = FALSE);
extern void Attack(CHARACTER* c);
extern bool SkillElf(CHARACTER* c, ITEM* p);
extern bool SkillWarrior(CHARACTER* c, ITEM* p);
extern void UseSkillWarrior(CHARACTER* c, OBJECT* o);
extern void UseSkillWizard(CHARACTER* c, OBJECT* o);
extern void UseSkillElf(CHARACTER* c, OBJECT* o);
extern void UseSkillSummon(CHARACTER* pCha, OBJECT* pObj);
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
extern void UseSkillRagefighter(CHARACTER* pCha, OBJECT* pObj);
extern void AttackRagefighter(CHARACTER* pCha, int nSkill, float fDistance);
extern bool UseSkillRagePosition(CHARACTER* pCha);
extern bool CheckTarget(CHARACTER* c);
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
extern void MouseRButtonReset();
extern void AttackKnight(CHARACTER* c, int Skill, float Distance);
extern bool IsGMCharacter();
extern bool IsNonAttackGM();
extern bool IsIllegalMovementByUsingMsg(const char* szChatText);
#endif //__ZZZINTERFACE_H__
