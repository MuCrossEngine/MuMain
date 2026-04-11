#ifndef _TextClien_H
#define _TextClien_H

class cTextClien
{
public:
	void Load();

	char TextClien_ThongTin[50][255];
	char TextClien_Khac[50][255];

};
//===================================================

#define MENU_GAME_FILE "./Data/Custom/TextClien/TextClien.ini"


extern cTextClien gTextClien;

#endif