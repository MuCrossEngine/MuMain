#pragma once
class CUSTOM_EFFECT_STACK;

typedef struct
{
	int pet23;
	std::vector<CUSTOM_EFFECT_STACK> cstackeffect[2];
}EFFECT_STACK;

typedef std::map<int, EFFECT_STACK> type_effect;

class CGMItemEffect
{
public:
	CGMItemEffect();
	virtual~CGMItemEffect();
	static CGMItemEffect* Instance();

	void LoadDataEffect(int sup23, CUSTOM_EFFECT_STACK* info, int Size);
	void push_back(int ItemIndex, int sup23, CUSTOM_EFFECT_STACK info);

	EFFECT_STACK* Find(int ItemIndex);
	void RenderLinkEffect(int ItemIndex, BMD* b, OBJECT* o);
	void CreateObjectEffect(int sup23, BMD* b, OBJECT* o, std::vector<CUSTOM_EFFECT_STACK>* csMake);
	void RenderPetEffect(int ItemIndex, BMD* b, OBJECT* o);
	void CreatePetEffect(int sup23, BMD* b, OBJECT* o, std::vector<CUSTOM_EFFECT_STACK>* csMake);
private:
	type_effect csItemEffect;
};

#define GMItemEffect			(CGMItemEffect::Instance())