#pragma once

#define MAX_CHAT			120

typedef struct
{
	char      ID[32];
	char      Union[30];
	char      Guild[30];
	char      szShopTitle[16];
	char      Color;
	char      GuildColor;
	steady_clock_ IDLifeTime;
	char      Text[2][256];
	steady_clock_ LifeTime[2];
	CHARACTER* Owner;
	int       x, y;
	int       Width;
	int       Height;
	vec3_t    Position;
	bool      RenderUIChat;
	bool      RemoveUIChat;
} CHAT;

enum _GENS_TYPE
{
	GENS_TYPE_NONE = 0,
	GENS_TYPE_DUPRIAN,
	GENS_TYPE_BARNERT,
};

class CGMHeadChat
{

public:
	CGMHeadChat();
	virtual~CGMHeadChat();
	static CGMHeadChat* Instance();

	void AddGuildName(CHAT* c, CHARACTER* Owner);
	void AddChat(CHAT* c, const char* Text, int Flag = 0);
	void AssignChat(char* ID, const char* Text, int Flag = 0);
	void CreateChat(char* ID, const char* Text, CHARACTER* Owner, int Flag = 0, int SetColor = -1);
	void SetBooleanPosition(CHAT* c);
	void RenderBooleans();
	void RenderBoolean(int x, int y, CHAT* c);

	void MoveChat();
	void SetPlayerColor(BYTE PK);

	bool EnableView(CHAT* c);
private:
	std::vector<CHAT> Chat;
};

#define gmHeadChat			(CGMHeadChat::Instance())