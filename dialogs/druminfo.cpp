#include "druminfo.h"
#include "resource.h"

struct d_info_str default_dinfo[] = {
	{"bassdrum",		"bd",	36,	NORMAL_NOTE,	NULL,	 1 },
	{"sidestick",		"ss",	37,	CROSS_NOTE3,	NULL,	 5 },
	{"snare",		"sn",	38,	NORMAL_NOTE,	NULL,	 5 },
	{"handclap",		"hc",	39,	TRIAG_NOTE2,	NULL,	 5 },
	{"lowfloortom",		"tomfl",41,	NORMAL_NOTE,	NULL,	 0 },
	{"closedhihat",		"hhc",	42,	CLOSED_HIGH_HAT,"stopped", 7 },
	{"hihat",		"hh",	42,	CROSS_NOTE3,	NULL,	 7 },
	{"highfloortom",	"tomfh",43,	NORMAL_NOTE,	NULL,	 2 },
	{"pedalhihat",		"hhp",	44,	CROSS_NOTE3,	NULL,	-1 },
	{"lowtom",		"toml",	45,	NORMAL_NOTE,	NULL,	 3 },
	{"openhihat",		"hho",	46,	OPEN_HIGH_HAT,	"open",	 7 },
	{"halfopenhihat",	"hhho",	46,	CROSS_NOTE2,	NULL,	 7 },
	{"lowmidtom",		"tomml",47,	NORMAL_NOTE,	NULL,	 4 },
	{"himidtom",		"tommh",48,	NORMAL_NOTE,	NULL,	 6 },
	{"crashcymbal",		"cymc",	49,	CROSS_NOTE2,	NULL,	 9 },
	{"hightom",		"tomh",	50,	NORMAL_NOTE,	NULL,	 8 },
	{"ridecymbal",		"cymr",	51,	CROSS_NOTE3,	NULL,	 9 },
	{"splashcymbal",	"cyms",	55,	RECT_NOTE2,	NULL,	 9 },
	{"cowbell",		"cb",	56,	TRIAG_NOTE2,	NULL,	 9 },
	{"chinesecymbal",	"cymch",52,	RECT_NOTE1,	NULL,	9 },
	/* -------------------------------------------------------------- */
	{"acousticbassdrum",	"bda",	35,	NORMAL_NOTE,	NULL,	1 },
	{"claves",		"cl",	75,	NORMAL_NOTE,	NULL,	3 },
	{"loagogo",		"agl",	68,	NORMAL_NOTE,	NULL,	3 },
	{"crashcymbalb",	"cymcb",57,	CROSS_NOTE3,	NULL,	9 },
	{"hitimbale",		"timh",	65,	NORMAL_NOTE,	NULL,	3 },
	{"maracas",		"mar",	70,	NORMAL_NOTE,	NULL,	3 },
	{"fivedown",		"de",	52,	NORMAL_NOTE,	NULL,	3 },
	{"electricsnare",	"sne",	40,	NORMAL_NOTE,	NULL,	5 },
	{"threeup",		"uc",	65,	NORMAL_NOTE,	NULL,	3 },
	{"acousticsnare",	"sna",	38,	NORMAL_NOTE,	NULL,	5 },
	{"mutehiconga",		"cghm",	62,	NORMAL_NOTE,	NULL,	3 },
	{"lowoodblock",		"wbl",	77,	NORMAL_NOTE,	NULL,	3 },
	{"hiconga",		"cgh",	63,	NORMAL_NOTE,	NULL,	3 },
	{"oneup",		"ua",	62,	NORMAL_NOTE,	NULL,	3 },
	{"guiro",		"gui",	74,	NORMAL_NOTE,	NULL,	3 },
	{"opencuica",		"cuio",	79,	NORMAL_NOTE,	NULL,	3 },
	{"mutehibongo",		"bohm",	60,	NORMAL_NOTE,	NULL,	3 },
	{"longguiro",		"guil",	74,	NORMAL_NOTE,	NULL,	3 },
	{"crashcymbala",	"cymca",49,	CROSS_NOTE2,	NULL,	9 },
	{"hiagogo",		"agh",	67,	NORMAL_NOTE,	NULL,	3 },
	{"hisidestick",		"ssh",	37,	NORMAL_NOTE,	NULL,	3 },
	{"openlobongo",		"bolo",	61,	NORMAL_NOTE,	NULL,	3 },
	{"mutetriangle",	"trim",	80,	NORMAL_NOTE,	NULL,	3 },
	{"opentriangle",	"trio",	81,	NORMAL_NOTE,	NULL,	3 },
	{"fiveup",		"ue",	69,	NORMAL_NOTE,	NULL,	3 },
	{"tambourine",		"tamb",	54,	NORMAL_NOTE,	NULL,	3 },
	{"longwhistle",		"whl",	72,	NORMAL_NOTE,	NULL,	3 },
	{"cabasa",		"cab",	69,	NORMAL_NOTE,	NULL,	3 },
	{"ridecymbalb",		"cymrb",59,	CROSS_NOTE3,	NULL,	9 },
	{"fourdown",		"dd",	53,	NORMAL_NOTE,	NULL,	3 },
	{"triangle",		"tri",	81,	NORMAL_NOTE,	NULL,	3 },
	{"hiwoodblock",		"wbh",	76,	NORMAL_NOTE,	NULL,	3 },
	{"onedown",		"da",	59,	NORMAL_NOTE,	NULL,	3 },
	{"twoup",		"ub",	64,	NORMAL_NOTE,	NULL,	3 },
	{"mutelobongo",		"bolm",	61,	NORMAL_NOTE,	NULL,	3 },
	{"loconga",		"cgl",	64,	NORMAL_NOTE,	NULL,	3 },
	{"openloconga",		"cglo",	64,	NORMAL_NOTE,	NULL,	3 },
	{"lobongo",		"bol",	61,	NORMAL_NOTE,	NULL,	3 },
	{"vibraslap",		"vibs",	58,	RECT_NOTE2,	NULL,	8 },
	{"mutecuica",		"cuim",	78,	NORMAL_NOTE,	NULL,	3 },
	{"losidestick",		"ssl",	37,	NORMAL_NOTE,	NULL,	3 },
	{"shortguiro",		"guis",	73,	NORMAL_NOTE,	NULL,	3 },
	{"twodown",		"db",	57,	NORMAL_NOTE,	NULL,	3 },
	{"shortwhistle",	"whs",	71,	NORMAL_NOTE,	NULL,	3 },
	{"openhibongo",		"boho",	60,	NORMAL_NOTE,	NULL,	3 },
	{"openhiconga",		"cgho",	63,	NORMAL_NOTE,	NULL,	3 },
	{"threedown",		"dc",	55,	NORMAL_NOTE,	NULL,	3 },
	{"fourup",		"ud",	67,	NORMAL_NOTE,	NULL,	3 },
	{"muteloconga",		"cglm",	62,	NORMAL_NOTE,	NULL,	3 },
	{"lotimbale",		"timl",	66,	NORMAL_NOTE,	NULL,	3 },
	{"ridecymbala",		"cymra",51,	CROSS_NOTE3,	NULL,	9 },
	{"hibongo",		"boh",	60,	NORMAL_NOTE,	NULL,	3 },
	{"ridebell",		"rb",	53,	NORMAL_NOTE,	NULL,	9 },
	{NULL,	NULL,	0,	NULL,	NULL}
};

struct d_info_str current_dinfo[G_N_ELEMENTS(default_dinfo)];

int getNumberOfDrumTabElements() {
	return G_N_ELEMENTS(default_dinfo);
}

void make_default_drums_current() {
	d_info_str *dptr1, *dptr2;

	dptr1 = default_dinfo;
	dptr2 = current_dinfo;

	do {
		memcpy(dptr2, dptr1, sizeof(struct d_info_str));
		if (dptr1->name) {
			dptr1++; dptr2++;
		}
	}
	while(dptr1->name);
}
