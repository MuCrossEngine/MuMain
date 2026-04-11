#include "stdafx.h"
#include "NewUISystem.h"
#include "DSPlaySound.h"
#include "CSItemOption.h"
#include "NewUIMasterSkillTree.h"

#if MAIN_UPDATE > 303
MASTER_SKILL_DATA stMasterSkillTreeData[MAX_MASTER_SKILL_DATA];
MASTER_SKILL_TOOLTIP stMasterSkillTooltip[MAX_MASTER_SKILL_DATA];

SEASON3B::CNewUIMasterSkillTree::CNewUIMasterSkillTree()
{
	this->pBitMap[0] = NULL;

	this->pBitMap[1] = NULL;

	this->ConsumePoint = 0;

	this->CurSkillID = 0;

	this->classCode = 0;

	this->categoryTextIndex = 0;

	this->categoryPos[0] = { 11, 55 };

	this->categoryPos[1] = { 221, 55 };

	this->categoryPos[2] = { 431, 55 };

	this->InitMasterSkillPoint();

	this->ClearSkillTreeInfo();

	this->ClearSkillTreeData();

	this->ClearSkillTooltipData();
}

SEASON3B::CNewUIMasterSkillTree::~CNewUIMasterSkillTree()
{
	this->Release();
}

bool SEASON3B::CNewUIMasterSkillTree::Create()
{
	if (g_pNewUIMng)
	{
		g_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MASTER_LEVEL, this);

		this->SetPos();

		this->LoadImages();

		this->m_BtnExit.ChangeButtonImgState(1, IMAGE_MASTER_INTERFACE + 5, 0, 0, 0);

		this->m_BtnExit.ChangeButtonInfo(m_Pos.x + 611, m_Pos.y + 9, 13, 14);

		this->m_BtnExit.ChangeToolTipText(GlobalText[1002]);

		for (int i = 0; i < MASTER_SKILL_CATEGORY; i++)
		{
			this->ButtonX[i] = 0;

			this->ButtonY[i] = 0;
		}

		return true;
	}

	return false;
}

void SEASON3B::CNewUIMasterSkillTree::Release()
{
	this->UnloadImages();

	g_pNewUIMng->RemoveUIObj(this);

	this->ClearSkillTreeInfo();

	this->ClearSkillTreeData();

	this->ClearSkillTooltipData();
}

void SEASON3B::CNewUIMasterSkillTree::SetPos()
{
	this->m_Pos.x = PositionX_The_Mid(640);

	this->m_Pos.y = PositionY_In_The_Mid(0);

	this->m_Size.cx = 640;

	this->m_Size.cy = 428;

	this->categoryPos[0].x = this->m_Pos.x + 11;
	this->categoryPos[0].y = this->m_Pos.y + 55;

	this->categoryPos[1].x = this->m_Pos.x + 221;
	this->categoryPos[1].y = this->m_Pos.y + 55;

	this->categoryPos[2].x = this->m_Pos.x + 431;
	this->categoryPos[2].y = this->m_Pos.y + 55;

	this->m_BtnExit.SetPos(m_Pos.x + 611, m_Pos.y + 9);
}

void SEASON3B::CNewUIMasterSkillTree::OpenMasterSkillTreeData(const char* FileName)
{
	memset(stMasterSkillTreeData, 0, sizeof(stMasterSkillTreeData));

	FILE* fp = fopen(FileName, "rb");

	if (fp)
	{
		int Size = sizeof(MASTER_SKILL_DATA);
		int MaxSize = Size * MAX_MASTER_SKILL_DATA;

		std::vector<BYTE> Buffer(MaxSize, 0);

		fread(Buffer.data(), MaxSize, 1u, fp);

		DWORD dwCheckSum;

		fread(&dwCheckSum, sizeof(DWORD), 1u, fp);

		fclose(fp);

		if (dwCheckSum == GenerateCheckSum2(Buffer.data(), MaxSize, 0x2BC1u))
		{
			BYTE* pSeek = Buffer.data();
			for (int i = 0; i < MAX_MASTER_SKILL_DATA; i++)
			{
				BuxConvert(pSeek, Size);
				memcpy(&stMasterSkillTreeData[i], pSeek, Size);
				pSeek += Size;

				if (!pSeek) break;
			}
		}
		else
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", FileName);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}

void SEASON3B::CNewUIMasterSkillTree::OpenMasterSkillTooltip(const char* FileName)
{
	memset(stMasterSkillTooltip, 0, sizeof(stMasterSkillTooltip));

	FILE* fp = fopen(FileName, "rb");

	if (fp)
	{
		int Size = sizeof(MASTER_SKILL_TOOLTIP);
		int MaxSize = Size * MAX_MASTER_SKILL_DATA;

		std::vector<BYTE> Buffer(MaxSize, 0);

		fread(Buffer.data(), MaxSize, 1u, fp);

		DWORD dwCheckSum;

		fread(&dwCheckSum, sizeof(DWORD), 1u, fp);

		fclose(fp);

		if (dwCheckSum == GenerateCheckSum2(Buffer.data(), MaxSize, 0x2BC1u))
		{
			BYTE* pSeek = Buffer.data();
			for (int i = 0; i < MAX_MASTER_SKILL_DATA; i++)
			{
				BuxConvert(pSeek, Size);
				memcpy(&stMasterSkillTooltip[i], pSeek, Size);
				pSeek += Size;
				if (!pSeek) break;
			}
		}
		else
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", FileName);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}

void SEASON3B::CNewUIMasterSkillTree::InitMasterSkillPoint()
{
	for (int i = 0; i < 3; i++)
	{
		this->CategoryPoint[i] = 0;

		for (int k = 0; k < 10; k++)
		{
			this->skillPoint[i][k] = 0;
		}
	}
}

void SEASON3B::CNewUIMasterSkillTree::SetMasterType(unsigned __int8 byClass)
{
	switch (byClass)
	{
	case Grand_Master:
	case Soul_Wizard:
		this->classCode = 2;
		this->categoryTextIndex = 1751;
		this->classNameTextIndex = 1669;
		break;
	case Blade_Master:
	case Dragon_Knight:
		this->classCode = 1;
		this->categoryTextIndex = 1755;
		this->classNameTextIndex = 1668;
		break;
	case High_Elf:
	case Noble_Elves:
		this->classCode = 4;
		this->categoryTextIndex = 1759;
		this->classNameTextIndex = 1670;
		break;
	case Duel_Master:
	case Magic_Knight:
		this->classCode = 16;
		this->categoryTextIndex = 1763;
		this->classNameTextIndex = 1671;
		break;
	case Lord_Emperor:
	case Empire_Road:
		this->classCode = 32;
		this->categoryTextIndex = 1767;
		this->classNameTextIndex = 1672;
		break;
	case Dimension_Master:
	case Dimension_Summoner:
		this->classCode = 8;
		this->categoryTextIndex = 3136;
		this->classNameTextIndex = 1689;
		break;
	case Fist_Master:
	case Fist_Blazer:
		this->classCode = 64;
		this->categoryTextIndex = 3330;
		this->classNameTextIndex = 3151;
		break;
	case Mirage_Lancer:
	case Shining_Lancer:
		this->classCode = 128;
		this->categoryTextIndex = 3330;
		this->classNameTextIndex = 3151;
		break;
	case Rune_Grand:
	case Majestic_Rune:
		this->classCode = 256;
		this->categoryTextIndex = 3330;
		this->classNameTextIndex = 3151;
		break;
	case Master_Slayer:
	case Slaughterer:
		this->classCode = 512;
		this->categoryTextIndex = 3330;
		this->classNameTextIndex = 3151;
		break;
	case Master_Gun_Breaker:
	case Heist_Gun_Crasher:
		this->classCode = 1024; //-- Gun Crasher
		break;
	case Shine_Wizard:
	case Luminous_Wizard:
		this->classCode = 2048; //-- White Mage
		break;
	case Arch_Mage:
	case Mystic_Mage:
		this->classCode = 4096; //-- Lemuria Mage
		break;
	case Illusion_Master:
	case Mystic_Knight:
		this->classCode = 8192; //-- Illusion Knight
		break;
	default:
		return;
	}

	this->SetMasterSkillTreeData();

	this->SetMasterSkillToolTipData();
}

void SEASON3B::CNewUIMasterSkillTree::SetMasterSkillTreeData()
{
	this->ClearSkillTreeData();

	for (int i = 0; i < MAX_MASTER_SKILL_DATA; i++)
	{
		if (!stMasterSkillTreeData[i].Id)
			break;

		if (((this->classCode & (unsigned __int16)stMasterSkillTreeData[i].ClassCode) != 0) == 1)
		{
			auto insertionResult = this->masterData.insert(type_map_master_data::value_type(stMasterSkillTreeData[i].Id, stMasterSkillTreeData[i]));

			if (!insertionResult.second)
				break;
		}
	}
}

void SEASON3B::CNewUIMasterSkillTree::SetMasterSkillToolTipData()
{
	this->ClearSkillTooltipData();

	for (int i = 0; i < MAX_MASTER_SKILL_DATA; i++)
	{
		if (!stMasterSkillTooltip[i].MasterId)
			break;

		if (((this->classCode & (unsigned __int16)stMasterSkillTooltip[i].ClassCode) != 0) == 1)
		{
			auto insertionResult = this->masterToolTip.insert(type_map_master_tooltip::value_type(stMasterSkillTooltip[i].MasterId, stMasterSkillTooltip[i]));

			if (!insertionResult.second)
				break;
		}
	}
}

bool SEASON3B::CNewUIMasterSkillTree::SetMasterSkillTreeInfo(BYTE Index, BYTE SkillPoint, float Value, float NextValue)
{
	type_map_master_data::iterator it = this->masterData.find(Index);

	if (it == this->masterData.end())
	{
		return true;
	}

	CSkillTreeInfo skillInfo;

	skillInfo.SetSkillTreeInfo(SkillPoint, Value, NextValue);

	this->skilltreeinfo.insert(type_map_master_info::value_type(it->second.Skill, skillInfo));

	switch (it->second.Group)
	{
	case 0:
		this->CategoryPoint[0] += SkillPoint;
		break;
	case 1:
		this->CategoryPoint[1] += SkillPoint;
		break;
	case 2:
		this->CategoryPoint[2] += SkillPoint;
		break;
	}
	return false;
}

int SEASON3B::CNewUIMasterSkillTree::SetDivideString(char* text, int isItemTollTip, int TextNum, int iTextColor, int iTextBold, bool isPercent)
{
	if (NULL == text)
	{
		return TextNum;
	}
	char alpszDst[10][256];

	memset(alpszDst, 0, sizeof(alpszDst));

	int nLine = 0;
	if (isItemTollTip == 0)
	{
		nLine = DivideStringByPixel((LPTSTR)alpszDst, 10, 256, text, 150, 1, 35);
	}
	else if (isItemTollTip == 1)
	{
		nLine = DivideStringByPixel((LPTSTR)alpszDst, 10, 256, text, 200, 1, 35);
	}

	for (int i = 0; i < nLine; i++)
	{
		TextBold[TextNum] = iTextBold;
		TextListColor[TextNum] = iTextColor;

		std::string cText = alpszDst[i];
		if (isPercent == true)
		{
			for (int j = cText.find("%", 0); j != -1; j = cText.find("%", j + 2))
			{
				cText.insert(j, "%");
			}
		}
		sprintf(TextList[TextNum], cText.c_str());
		TextNum++;
	}

	return TextNum;
}

bool SEASON3B::CNewUIMasterSkillTree::Render()
{
	EnableAlphaTest();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderImage(IMAGE_MASTER_INTERFACE, m_Pos.x, m_Pos.y, pBitMap[0]->output_width, pBitMap[0]->output_height);

	RenderImage(IMAGE_MASTER_INTERFACE + 1, m_Pos.x + pBitMap[0]->output_width, m_Pos.y, pBitMap[1]->output_width, pBitMap[1]->output_height);

	RenderIcon();

	m_BtnExit.Render();

	DisableAlphaBlend();

	this->RenderText();

	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::Update()
{
	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::UpdateMouseEvent()
{
	if (this->m_BtnExit.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MASTER_LEVEL);
		return true;
	}

	bool result = true;

	if (SEASON3B::IsPress(VK_LBUTTON) == true)
	{
		result = this->CheckMouse(MouseX, MouseY);

		if (result == false)
		{
			PlayBuffer(SOUND_CLICK01);
		}
	}

	for (int i = 0; i < MASTER_SKILL_CATEGORY; i++)
	{
		if (this->ButtonX[i] == 1 && SEASON3B::IsPress(VK_LBUTTON) == true)
		{
			this->ButtonX[i] = 0;
			return true;
		}
	}

	this->CheckBtn();

	if (SEASON3B::CheckMouseIn(this->m_Pos.x, this->m_Pos.y, this->m_Size.cx, this->m_Size.cy) == true)
	{
		return false;
	}

	return result;
}

bool SEASON3B::CNewUIMasterSkillTree::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL) && (IsPress(VK_ESCAPE) || IsPress('A')))
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MASTER_LEVEL);

		PlayBuffer(SOUND_CLICK01);

		return false;
	}

	return true;
}

float SEASON3B::CNewUIMasterSkillTree::GetLayerDepth()
{
	return 8.1f;
}

void SEASON3B::CNewUIMasterSkillTree::EventOrderWindows(double WindowsX, double WindowsY)
{
	this->SetPos();
}

void SEASON3B::CNewUIMasterSkillTree::LoadImages()
{
	LoadBitmap("Interface\\new_Master_back01.jpg", IMAGE_MASTER_INTERFACE, GL_LINEAR);	//31532
	LoadBitmap("Interface\\new_Master_back02.jpg", IMAGE_MASTER_INTERFACE + 1, GL_LINEAR);	//31533

	this->pBitMap[0] = &Bitmaps[IMAGE_MASTER_INTERFACE];

	this->pBitMap[1] = &Bitmaps[IMAGE_MASTER_INTERFACE + 1];

	LoadBitmap("Interface\\new_Master_Icon.jpg", IMAGE_MASTER_INTERFACE + 2, GL_LINEAR);	//31534
	LoadBitmap("Interface\\new_Master_Non_Icon.jpg", IMAGE_MASTER_INTERFACE + 3, GL_LINEAR);	//31535
	LoadBitmap("Interface\\new_Master_box.tga", IMAGE_MASTER_INTERFACE + 4, GL_LINEAR);	//31536
	LoadBitmap("Interface\\new_Master_exit.jpg", IMAGE_MASTER_INTERFACE + 5, GL_LINEAR);	//31537
	LoadBitmap("Interface\\new_Master_arrow01.tga", IMAGE_MASTER_INTERFACE + 6, GL_LINEAR);	//31538
	LoadBitmap("Interface\\new_Master_arrow02.tga", IMAGE_MASTER_INTERFACE + 7, GL_LINEAR);	//31539
	LoadBitmap("Interface\\new_Master_arrow03.tga", IMAGE_MASTER_INTERFACE + 8, GL_LINEAR);	//31540
	LoadBitmap("Interface\\new_Master_arrow04.tga", IMAGE_MASTER_INTERFACE + 9, GL_LINEAR);	//31541
	LoadBitmap("Interface\\new_Master_arrow05.tga", IMAGE_MASTER_INTERFACE + 10, GL_LINEAR);	//31542
	LoadBitmap("Interface\\new_Master_arrow06.tga", IMAGE_MASTER_INTERFACE + 11, GL_LINEAR);	//31543
	LoadBitmap("Interface\\new_Master_arrow07.tga", IMAGE_MASTER_INTERFACE + 12, GL_LINEAR);	//31544
	LoadBitmap("Interface\\new_Master_arrow08.tga", IMAGE_MASTER_INTERFACE + 13, GL_LINEAR);	//31545
}

void SEASON3B::CNewUIMasterSkillTree::UnloadImages()
{
	for (int i = 0; i < 14; i++)	//estaba en 4, el viejo master tenia 4 asique se deben haber olvidado actualizarlo
	{
		DeleteBitmap(i + IMAGE_MASTER_INTERFACE, 0);
	}
}

void SEASON3B::CNewUIMasterSkillTree::RenderText()
{
	g_pRenderText->SetFont(g_hFont);

	float x = this->m_Pos.x;
	float y = this->m_Pos.y;

	if (SEASON3B::IsPress(VK_LBUTTON) == false && SEASON3B::CheckMouseIn(x + 458, y + 11, 81, 10) == true)
	{
		TextList[0][0] = 0;
		TextBold[0] = 0;
		TextListColor[0] = 0;
		sprintf(TextList[0], "%I64d / %I64d", Master_Level_Data.lMasterLevel_Experince, Master_Level_Data.lNext_MasterLevel_Experince);
		RenderTipTextList(x + 466, y + 26, 1, 0, 3, 0, 1);
	}

	g_pRenderText->SetTextColor(255, 255, 255, 0xFFu);

	g_pRenderText->SetBgColor(0, 0, 0, 1u);

	char Buffer[256];

	memset(Buffer, 0, sizeof(Buffer));

	sprintf(Buffer, GlobalText[1746], Master_Level_Data.nMLevel);

	g_pRenderText->RenderText(x + 275, y + 11, Buffer, 0, 0, 1, 0);

	sprintf(Buffer, GlobalText[1747], Master_Level_Data.nMLevelUpMPoint);

	g_pRenderText->RenderText(x + 372, y + 11, Buffer, 0, 0, 1, 0);

	if (Master_Level_Data.lNext_MasterLevel_Experince != 0)
	{
		__int64 iTotalLevel = (__int64)Master_Level_Data.nMLevel + 400;
		__int64 iTOverLevel = iTotalLevel - 255;
		__int64 iBaseExperience = 0;

		__int64 iData_Master =	// A
			(
				(
					(__int64)9 + (__int64)iTotalLevel
					)
				* (__int64)iTotalLevel
				* (__int64)iTotalLevel
				* (__int64)10
				)
			+
			(
				(
					(__int64)9 + (__int64)iTOverLevel
					)
				* (__int64)iTOverLevel
				* (__int64)iTOverLevel
				* (__int64)1000
				);

		iBaseExperience = (iData_Master - (__int64)3892250000) / (__int64)2;

		double fNeedExp = (double)Master_Level_Data.lNext_MasterLevel_Experince - (double)iBaseExperience;

		double fExp = (double)Master_Level_Data.lMasterLevel_Experince - (double)iBaseExperience;

		sprintf(Buffer, GlobalText[3335], fExp / fNeedExp * 100.0);

		g_pRenderText->RenderText(x + 466, y + 11, Buffer, 0, 0, 1, 0);
	}

	g_pRenderText->RenderText(x + 154, y + 11, GlobalText[this->classNameTextIndex], 0, 0, 1, 0);

	g_pRenderText->SetTextColor(255, 155, 0, 0xFFu);

	sprintf(Buffer, GlobalText[this->categoryTextIndex], this->CategoryPoint[0]);

	g_pRenderText->RenderText(x + 92, y + 40, Buffer, 0, 0, 1, 0);

	sprintf(Buffer, GlobalText[this->categoryTextIndex + 1], this->CategoryPoint[1]);

	g_pRenderText->RenderText(x + 302, y + 40, Buffer, 0, 0, 1, 0);

	sprintf(Buffer, GlobalText[this->categoryTextIndex + 2], this->CategoryPoint[2]);

	g_pRenderText->RenderText(x + 513, y + 40, Buffer, 0, 0, 1, 0);
}

void SEASON3B::CNewUIMasterSkillTree::RenderIcon()
{
	int skillPoint = 0;

	type_map_master_data::iterator it = masterData.begin();

	// Recorre el mapa utilizando un bucle while
	while (it != masterData.end())
	{
		MASTER_SKILL_DATA* info = &it->second;

		BYTE Group = info->Group;

		int Skill = info->Skill;

		SKILL_ATTRIBUTE* p = &SkillAttribute[Skill];

		int index = (info->Id - 1) % 4;

		BYTE rank = p->SkillRank;

		type_map_master_info::iterator li = this->skilltreeinfo.find(Skill);

		if (li != this->skilltreeinfo.end())
		{
			skillPoint = li->second.GetSkillPoint();
		}
		else
		{
			skillPoint = 0;
		}

		DWORD textColor = RGBA(255, 255, 255, 255);

		if (this->CheckParentSkill(info->RequireSkill[0], info->RequireSkill[1]))
		{
			if (this->CheckRankPoint(Group, rank, skillPoint))
			{
				if (this->CheckBeforeSkill(Skill, skillPoint))
				{
					if (!g_csItemOption.Special_Option_Check(Skill))
					{
						textColor = RGBA(0x78u, 0x78u, 0x78u, 0xFFu);
					}
				}
				else
				{
					textColor = RGBA(0x78u, 0x78u, 0x78u, 0xFFu);
				}
			}
			else
			{
				textColor = RGBA(0x78u, 0x78u, 0x78u, 0xFFu);
			}
		}
		else
		{
			textColor = RGBA(0x78u, 0x78u, 0x78u, 0xFFu);
		}

		int Magic_Icon = p->Magic_Icon;

		float x = (float)(categoryPos[Group].x + (float)index * 49.f);
		float y = (float)(categoryPos[Group].y + (float)(rank - 1) * 41.f);
		RenderImage(IMAGE_MASTER_INTERFACE + 4, x, y, 50.0, 38.0, 0.0, 0.0, textColor);

		float uw = (20.f / 512.f);
		float uv = (28.f / 512.f);
		float sv = (double)(Magic_Icon / 25) * uv;
		float su = (double)(Magic_Icon % 25) * uw;
		RenderImage(IMAGE_MASTER_INTERFACE + 2, x + 8.5, y + 5.0, 20.0, 28.0, su, sv, uw, uv, textColor);


		char pzText[MAX_TEXT_LENGTH];
		memset(pzText, 0, MAX_TEXT_LENGTH);
		sprintf_s(pzText, "%d", skillPoint);

		g_pRenderText->SetTextColor(textColor);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(x + 36, y + 24, pzText, 0, 0, 1, NULL);

		float AddX = 0.0;
		float AddY = 0.0;
		float width = 0.0;
		float hight = 0.0;

		BYTE ArrowDirection = info->ArrowDirection;

		if (InBounds<unsigned char>(ArrowDirection, 1u, 8u) == 1)
		{
			switch (ArrowDirection)
			{
			case 1u:
				AddX = 30.0;
				AddY = 15.0;
				width = 28.0;
				hight = 7.0;
				break;
			case 2u:
				AddX = -21.0;
				AddY = 15.0;
				width = 28.0;
				hight = 7.0;
				break;
			case 3u:
				AddX = 16.0;
				AddY = 35.0;
				width = 7.0;
				hight = 12.0;
				break;
			case 4u:
				AddX = 16.0;
				AddY = 35.0;
				width = 7.0;
				hight = 52.0;
				break;
			case 5u:
				AddX = 35.0;
				AddY = 15.0;
				width = 42.0;
				hight = 31.0;
				break;
			case 6u:
				AddX = -35.0;
				AddY = 15.0;
				width = 42.0;
				hight = 31.0;
				break;
			case 7u:
				AddX = 19.0;
				AddY = 35.0;
				width = 40.0;
				hight = 28.0;
				break;
			case 8u:
				AddX = -21.0;
				AddY = 35.0;
				width = 40.0;
				hight = 28.0;
				break;
			default:
				break;
			}
			int image = IMAGE_MASTER_INTERFACE + 5;
			RenderImage(ArrowDirection + image, x + AddX, y + AddY, width, hight, 0.0, 0.0, textColor);
		}
		++it;
	}

	this->RenderToolTip();
}

void SEASON3B::CNewUIMasterSkillTree::RenderToolTip()
{
	type_map_master_data::iterator it = masterData.begin();

	// Recorre el mapa utilizando un bucle while
	for (; it != masterData.end(); ++it)
	{
		MASTER_SKILL_DATA* info = &it->second;

		BYTE Group = info->Group;

		int Skill = info->Skill;

		SKILL_ATTRIBUTE* p = &SkillAttribute[Skill];

		if (p == NULL)
		{
			break;
		}

		int index = (info->Id - 1) % 4;

		BYTE rank = p->SkillRank;

		int CalcX = (int)(index * 49.0f + this->categoryPos[Group].x);

		int CalcY = (int)(this->categoryPos[Group].y + (rank - 1) * 41.0f);

		if (SEASON3B::IsPress(VK_LBUTTON) == true || SEASON3B::CheckMouseIn(CalcX + 8, CalcY + 5, 20, 28) == false)
		{
			continue;
		}

		type_map_master_tooltip::iterator mtit = this->masterToolTip.find(Skill);

		if (mtit == this->masterToolTip.end())
		{
			return;
		}

		int skillPoint = 0;

		float skillValue = 0.0f;

		float skillNextValue = 0.0f;

		type_map_master_info::iterator sit = this->skilltreeinfo.find(Skill);

		if (sit != this->skilltreeinfo.end())
		{
			skillPoint = sit->second.GetSkillPoint();

			skillValue = sit->second.GetSkillValue();

			skillNextValue = sit->second.GetSkillNextValue();
		}

		for (int i = 0; i < 30; i++)
		{
			TextList[i][0] = 0;
			TextListColor[i] = i == 0 ? TEXT_COLOR_YELLOW : TEXT_COLOR_WHITE;
		}
		memset(TextBold, 0, sizeof(TextBold));

		int lineCount = 0;

		sprintf(TextList[lineCount], "%s", p->Name);

		TextBold[lineCount] = true;

		lineCount++;

		sprintf(TextList[lineCount], mtit->second.Rank, p->SkillRank, skillPoint, it->second.MaxLevel);

		lineCount++;

		char buffer[512];

		memset(buffer, 0, sizeof(buffer));

		if (it->second.DefValue == -1.0f)
		{
			sprintf(buffer, mtit->second.Info);
		}
		else
		{
			sprintf(buffer, mtit->second.Info, skillPoint != 0 ? skillValue : it->second.DefValue);
		}

		lineCount = this->SetDivideString(buffer, 0, lineCount, 0, 0, true);

		if (skillPoint != 0 && skillPoint < it->second.MaxLevel)
		{
			sprintf(buffer, GlobalText[3328]);

			lineCount = this->SetDivideString(buffer, 0, lineCount, 4, 0, true);

			TextBold[lineCount] = 1;

			sprintf(buffer, mtit->second.Info, skillNextValue);

			lineCount = this->SetDivideString(buffer, 0, lineCount, 0, 0, true);
		}

		if (skillPoint < it->second.MaxLevel)
		{
			sprintf(buffer, GlobalText[3329]);

			lineCount = this->SetDivideString(buffer, 0, lineCount, 1, 0, true);

			TextBold[lineCount] = 1;

			sprintf(buffer, mtit->second.Required, it->second.RequiredPoints);

			if (it->second.RequiredPoints <= Master_Level_Data.nMLevelUpMPoint)
			{
				lineCount = this->SetDivideString(buffer, 0, lineCount, 0, 0, true);
			}
			else
			{
				lineCount = this->SetDivideString(buffer, 0, lineCount, 2, 0, true);
			}
		}

		int iTextColor = this->CheckBeforeSkill(Skill, skillPoint) == true ? 0 : 2;

		sprintf(buffer, mtit->second.Info1);

		lineCount = this->SetDivideString(buffer, 0, lineCount, iTextColor, 0, true);

		if (skillPoint < it->second.MaxLevel && p->SkillRank != 1)
		{
			iTextColor = this->CheckRankPoint(Group, p->SkillRank, skillPoint) == true ? 0 : 2;

			sprintf(buffer, mtit->second.Info2);

			lineCount = this->SetDivideString(buffer, 0, lineCount, iTextColor, 0, true);

			int RequireSkill1 = it->second.RequireSkill[0];

			int RequireSkill2 = it->second.RequireSkill[1];

			if (RequireSkill1 != 0)
			{
				type_map_master_info::iterator rq1it = this->skilltreeinfo.find(RequireSkill1);

				if (rq1it != this->skilltreeinfo.end())
				{
					iTextColor = rq1it->second.GetSkillValue() < 10 ? 2 : 0;
				}
				else
				{
					iTextColor = 2;
				}

				sprintf(buffer, mtit->second.Info3);

				lineCount = this->SetDivideString(buffer, 0, lineCount, iTextColor, 0, true);
			}

			if (RequireSkill2 != 0)
			{
				type_map_master_info::iterator rq1it = this->skilltreeinfo.find(RequireSkill2);

				if (rq1it != this->skilltreeinfo.end())
				{
					iTextColor = rq1it->second.GetSkillValue() < 10 ? 2 : 0;
				}
				else
				{
					iTextColor = 2;
				}

				sprintf(buffer, mtit->second.Info4);

				lineCount = this->SetDivideString(buffer, 0, lineCount, iTextColor, 0, true);
			}
		}

		RenderTipTextList(CalcX + 8, CalcY + 33, lineCount, 0, 3, 0, 1);
	}
}

bool SEASON3B::CNewUIMasterSkillTree::CheckMouse(int posx, int posy)
{
	int PosX = m_Pos.x;
	int PosY = m_Pos.y;

	POINT position[3] = { {185, 65}, {385, 65}, {585, 65} };

	for (int i = 0; i < MASTER_SKILL_CATEGORY; i++)
	{
		if (SEASON3B::CheckMouseIn(position[i].x + PosX, this->ButtonY[i] + position[i].y + PosY, 15, 30) == true && this->ButtonX[i] == 0)
		{
			this->ButtonY[i] = 1;

			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckBtn()
{
	int posX = 220;

	for (int i = 0; i < MASTER_SKILL_CATEGORY; i++)
	{
		if (this->ButtonX[i] == 1 && SEASON3B::IsRelease(VK_LBUTTON))
		{
			this->ButtonX[i] = 0;
			return 0;
		}

		if (this->ButtonX[i] == 1)
		{
			this->ButtonY[i] = MouseY - 65;

			if (this->ButtonY[i] > (unsigned int)posX)
			{
				this->ButtonY[i] = posX;
			}
			else if (this->ButtonY[i] <= 0)
			{
				this->ButtonY[i] = 0;
			}
		}
	}

	for (type_map_master_data::iterator it = this->masterData.begin(); it != this->masterData.end(); it++)
	{
		BYTE Group = it->second.Group;

		switch (Group)
		{
		case 0:
			this->CheckAttributeArea(0, &it->second);
			break;
		case 1:
			this->CheckAttributeArea(1, &it->second);
			break;
		case 2:
			this->CheckAttributeArea(2, &it->second);
			break;
		}
	}

	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckAttributeArea(int group, MASTER_SKILL_DATA* info)
{
	SKILL_ATTRIBUTE* lpskill = &SkillAttribute[info->Skill];

	if (!InBounds<int>(group, 0, 2))
		return false;
	if (lpskill == NULL)
		return false;

	int id = (info->Id - 1) % 4;
	int posX = (int)((double)this->categoryPos[group].x + id * 49.0);
	int posY = (int)((double)this->categoryPos[group].y + (lpskill->SkillRank - 1) * 41.0);

	if (SEASON3B::IsPress(VK_LBUTTON) && SEASON3B::CheckMouseIn(posX + 8, posY + 5, 20, 28))
	{
		type_map_master_info::iterator it = this->skilltreeinfo.find(info->Skill);

		int skillPoint = 0;

		if (it != this->skilltreeinfo.end())
		{
			skillPoint = it->second.GetSkillPoint();
		}
		PlayBuffer(SOUND_CLICK01);

		if (this->CheckSkillPoint(Master_Level_Data.nMLevelUpMPoint, info->RequiredPoints, info->MaxLevel, skillPoint))
		{
			if (this->CheckParentSkill(info->RequireSkill[0], info->RequireSkill[1]) == false || this->CheckRankPoint(group, lpskill->SkillRank, skillPoint) == false || this->CheckBeforeSkill(info->Skill, skillPoint) == false)
			{
				SEASON3B::CreateOkMessageBox(GlobalText[3327]);
				return 1;
			}
			if (g_csItemOption.Special_Option_Check(info->Skill) == false)
			{
				SEASON3B::CreateOkMessageBox(GlobalText[3336]);
				return 1;
			}
			this->SetConsumePoint(info->RequiredPoints);
			this->SetCurSkillID(info->Skill);
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CMaster_Level_Interface));

			MouseLButton = 0;
			MouseLButtonPop = 0;
			MouseLButtonPush = 0;
		}
	}

	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckSkillPoint(WORD mLevelUpPoint, int reqPoint, int maxPoint, WORD skillPoint)
{
	if (skillPoint == maxPoint)
	{
		SEASON3B::CreateOkMessageBox(GlobalText[3326]);
		return false;
	}

	if (mLevelUpPoint >= reqPoint)
	{
		return true;
	}

	char Buffer[358];

	memset(Buffer, 0, sizeof(Buffer));

	sprintf(Buffer, GlobalText[3325], reqPoint - mLevelUpPoint);

	SEASON3B::CreateOkMessageBox(Buffer);

	return false;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckParentSkill(DWORD reqSkill1, DWORD reqSkill2)
{
	if (reqSkill1 == 0)
		return true;

	type_map_master_info::iterator it = this->skilltreeinfo.find(reqSkill1);

	if (it == this->skilltreeinfo.end())
		return false;

	if (it->second.GetSkillPoint() < 10)
	{
		return false;
	}

	if (reqSkill2 == 0)
	{
		return true;
	}

	it = this->skilltreeinfo.find(reqSkill2);

	if (it == this->skilltreeinfo.end())
	{
		return false;
	}

	if (it->second.GetSkillPoint() < 10)
	{
		return false;
	}

	return true;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckRankPoint(BYTE Group, BYTE SkillRank, BYTE skillPoint)
{
	bool success;

	if (this->skillPoint[Group][SkillRank] < skillPoint)
		this->skillPoint[Group][SkillRank] = skillPoint;

	if (SkillRank == 1)
		success = true;
	else
		success = this->skillPoint[Group][SkillRank - 1] >= 10;

	return success;
}

bool SEASON3B::CNewUIMasterSkillTree::CheckBeforeSkill(int skill, BYTE skillPoint)
{
	int Index = this->GetBeforeSkillID(skill);

	if (Index)
	{
		SKILL_ATTRIBUTE* lpSkill = &SkillAttribute[Index];

		if (lpSkill)
		{
			if (lpSkill->SkillUseType == SKILL_USE_TYPE_MASTERACTIVE)
			{
				return true;
			}
			else if (skillPoint)
			{
				return true;
			}
			else
			{
				for (int i = 0; i < MAX_MAGIC; i++)
				{
					if (CharacterAttribute->Skill[i] == Index)
					{
						return true;
					}
				}
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

int SEASON3B::CNewUIMasterSkillTree::GetBeforeSkillID(int index)
{
	return SkillAttribute[index].SkillBrand;
}

int SEASON3B::CNewUIMasterSkillTree::GetBaseSkillID(int index)
{
	if (index >= MAX_SKILLS)
	{
		return 0;
	}

	while (SkillAttribute[index].SkillBrand != 0 && SkillAttribute[index].SkillBrand != 75)
	{
		index = SkillAttribute[index].SkillBrand;
	}

	return index;
}

void SEASON3B::CNewUIMasterSkillTree::SkillUpgrade(int index, char point, float value, float nextvalue)
{
	type_map_master_data::iterator it = this->masterData.find(index);

	int realSkill = 0;

	if (it != this->masterData.end())
	{
		realSkill = it->second.Skill;
	}

	type_map_master_info::iterator sit = this->skilltreeinfo.find(realSkill);

	if (sit != this->skilltreeinfo.end())
	{
		this->skilltreeinfo.erase(sit);
	}

	CSkillTreeInfo skillinfo;

	skillinfo.SetSkillTreeInfo(point, value, nextvalue);

	if (this->skilltreeinfo.insert(type_map_master_info::value_type(realSkill, skillinfo)).second == 0)
	{
		return;
	}

	this->CategoryPoint[it->second.Group] += it->second.RequiredPoints;
}

bool SEASON3B::CNewUIMasterSkillTree::IsLearnSkill(int index)
{
	type_map_master_info::iterator it = this->skilltreeinfo.find(index);

	if (it == this->skilltreeinfo.end())
	{
		return false;
	}

	return true;
}

float SEASON3B::CNewUIMasterSkillTree::GetSkillValue(int index)
{
	type_map_master_info::iterator it = this->skilltreeinfo.find(index);

	if (it == this->skilltreeinfo.end())
	{
		return 0.0f;
	}

	return (it->second.GetSkillValue());
}

float SEASON3B::CNewUIMasterSkillTree::GetSkillCumulativeValue(int index, BYTE damage)
{
	if (!index || index < 300 || index > 608 || damage == 0 || damage >= 5)
	{
		return 0.0;
	}

	float value = 0.0;

	SKILL_ATTRIBUTE* lpSkill;

	for (int i = index; i != 75; i = lpSkill->SkillBrand)
	{
		if (i == 0)
		{
			break;
		}

		lpSkill = &SkillAttribute[i];

		if (damage == lpSkill->IsDamage)
		{
			value += this->GetSkillValue(i);
		}
	}

	return value;
}

BYTE SEASON3B::CNewUIMasterSkillTree::GetSkillLevel(int index)
{
	type_map_master_info::iterator it = this->skilltreeinfo.find(index);

	if (it == this->skilltreeinfo.end())
	{
		return 0;
	}

	return it->second.GetSkillPoint();
}

void SEASON3B::CNewUIMasterSkillTree::RegisterSkillInCharacterAttribute(int index)
{
	if (index >= 300 && index <= 608)
	{
		SKILL_ATTRIBUTE* lpskill = &SkillAttribute[index];

		if (lpskill->SkillUseType == SKILL_USE_TYPE_MASTERACTIVE)
		{
			for (int i = 0; i < MAX_MAGIC; i++)
			{
				if (lpskill->SkillBrand != CharacterAttribute->Skill[i])
				{
					continue;
				}
				CharacterAttribute->Skill[i] = index;
				return;
			}
			return;
		}

		for (int i = 0; i < MAX_MAGIC; i++)
		{
			if (CharacterAttribute->Skill[i] != 0)
			{
				continue;
			}

			CharacterAttribute->Skill[i] = index;
			return;
		}
	}
}

void SEASON3B::CNewUIMasterSkillTree::SetConsumePoint(BYTE ConsumePoint)
{
	this->ConsumePoint = ConsumePoint;
}

void SEASON3B::CNewUIMasterSkillTree::SetCurSkillID(int index)
{
	this->CurSkillID = index;
}

BYTE SEASON3B::CNewUIMasterSkillTree::GetConsumePoint()
{
	return this->ConsumePoint;
}

int SEASON3B::CNewUIMasterSkillTree::GetCurSkillID()
{
	return this->CurSkillID;
}

void SEASON3B::CNewUIMasterSkillTree::ClearSkillTreeInfo()
{
	skilltreeinfo.clear();
}

void SEASON3B::CNewUIMasterSkillTree::ClearSkillTreeData()
{
	masterData.clear();
}

void SEASON3B::CNewUIMasterSkillTree::ClearSkillTooltipData()
{
	masterToolTip.clear();
}
#endif