/* notes:
Specific notes:
FixNote 20160728.1 - Need to check vSpawn, if not there, insert vAdd to there, also double check the vAdds[i].Add flag.
FixNote 20160728.2 - this check will need readded for AACutOffTime
FixNote 20160731.1  might need to add more members here, but for now just setting ID and spawn

General notes overall:
Safety string and POSIX updates complete.

General notes spells:
Fully flesh out AAs, disc, heals before moving on.  No need to redo all routines 20 times for no reason.

General notes spawns/storage:
vAdds need checked against vSpawn, the Spawns need to be populated, and then both checked/cleared via OnRemoveSpawn
KillTarget needs to get set somehow.  can re-use some/most of mq2bot code GetKillTarget() for that.
Once we have kill target, each detrimental spell needs to have it added to their target struct member.

General notes casting/spell storage:
I think the CastHandle would actually work as is, but it can be more efficient by adding struct members directly (this is sort of done) so that checks dont need made multiple times for unchanging members.


save for later:
// to use discs: pCharData->DoCombatAbility(vMaster[i].ID);
try
{
if(pSpawn)
{
SPAWNINFO tSpawn;
memcpy(&tSpawn,pSpawn,sizeof(SPAWNINFO));
RemoveFromNotify(&tSpawn, true);
}
}
catch(...)
{
DebugSpewAlways("MQ2Bot::LoadZoneTargets() **Exception**");
}

*/
#define MQ2Bot2 1
#ifdef MQ2Bot2
#pragma region Headers
// Can safely ignore these warnings
//#pragma warning ( disable : 4710 4365 4018 4244 4505 4189 4101 4100 4482 )
#pragma warning( push )
#pragma warning ( disable : 4482 )
// includes

#include "../MQ2Plugin.h"
#include "../moveitem2.h"
#include "../Blech/Blech.h"
#include <vector>
#include <list>
#include <map>
#include <iterator>
#include <deque>
#include <Windows.h>
#include <cstdio>
using namespace std;
#include <algorithm>
/*
#ifndef MMOBUGS
extern char PLUGIN_NAME[MAX_PATH];
#else

#endif
*/
char PLUGIN_NAME[MAX_PATH] = "MQ2Bot"; // this needs reenabled when not using mmobugs source code
PreSetup("MQ2Bot");

// defines
#define NewSource 1
#define	TargetIT(X)			*(PSPAWNINFO*)ppTarget=X
#define	ADD_HP vAdds[0]->HPCurrent*100/vAdds[0]->HPMax
#define	ISINDEX() (Index[0])
#define	ISNUMBER() (IsNumber(Index))
#define	GETNUMBER() (atoi(Index))
#define	GETFIRST() Index
#define CAST_SUCCESS      0
#define CAST_INTERRUPTED  1
#define CAST_RESIST       2
#define CAST_COLLAPSE     3
#define CAST_RECOVER      4
#define CAST_FIZZLE       5
#define CAST_STANDING     6
#define CAST_STUNNED      7
#define CAST_INVISIBLE    8
#define CAST_NOTREADY     9
#define CAST_OUTOFMANA   10
#define CAST_OUTOFRANGE  11
#define CAST_NOTARGET    12
#define CAST_CANNOTSEE   13
#define CAST_COMPONENTS  14
#define CAST_OUTDOORS    15
#define CAST_TAKEHOLD    16
#define CAST_IMMUNE      17
#define CAST_DISTRACTED  18
#define CAST_ABORTED     19
#define CAST_UNKNOWN     20
#define CAST_IMMUNE_RUN  21
#define CAST_IMMUNE_MEZ  22
#define CAST_IMMUNE_SLOW 23

#define FLAG_COMPLETE     0
#define FLAG_REQUEST     -1
#define FLAG_PROGRESS1   -2
#define FLAG_PROGRESS2   -3
#define FLAG_PROGRESS3   -4
#define FLAG_PROGRESS4   -5

#define DONE_COMPLETE    -3
#define DONE_ABORTED     -2
#define DONE_PROGRESS    -1
#define DONE_SUCCESS      0

#define RECAST_LAND       1

#define DELAY_CAST    12000
#define DELAY_MEMO     6000
#define DELAY_STOP     3000

#define CASTERANY 0
#define CASTERME 1
#define CASTEROTHER 2
#define CASTEREVENT 3
#define EVENTSLENGTH 75
#define aCastEvent(List,Value,Filter) List.AddEvent(Filter,CastEvent,(void*)Value);
#pragma endregion Headers

#pragma region Prototypes
// enums
enum ATTRIBSLOTS { ATTRIB, BASE, BASE2, CALC, MAX }; // attrib enum
enum OPTIONS { // used to check routines automatically
	ZERO, AA, AGGRO, AURA, BARD, BUFF, CALL, CHARM, CLICKIES, CLICKYBUFFS, ITEMS,
	CLICKYNUKES, DEBUFF, DOT, ENDURANCE, FADE, FIGHTBUFF, GRAB, HEAL, HEALPET, HEALTOT,
	IMHIT, JOLT, KNOCKBACK, LIFETAP, MAINTANKBUFF, MANA, NUKE, NUKETOT, PET, REZ,
	ROOT, SELFBUFF, SNARE, SUMMONITEM, SWARM, DISC, CURE, MEZ
};
enum SKILLTYPES { TYPE_SPELL = 1, TYPE_AA = 2, TYPE_ITEM = 3, TYPE_DISC = 4 }; // struct for spell data, how to use the spell, and how the spell was used

																			   // structs
																			   //struct _BotSpell;

typedef struct _BotSpell // has to be before the FunctionDeclarations because a bunch use it
{
	PSPELL				Spell;					// store the spell itself so I can access all of its members
	char				SpellName[MAX_STRING];	// Check ini to see if they want to use a custom spell, if not then store the name of the spell/AA/Disc/item we actually want to /cast since that is often different
	bool				CanIReprioritize;		// if a specific custom spell or priority was set, then i want to ignore reprioritizing it
	char				SpellIconName[MAX_STRING];	// store the name of the icon in case it is special like mana reiteration buff icon
	char				Gem[MAX_STRING];		// in case they want to use a custom gem, otherwise this will be alt, disc, item, or default gem
	char				If[MAX_STRING];			// is there a custom if statement in the ini?
	char				Target[MAX_STRING];		// is there a custom target in the ini?  this is really a placeholder until i make a final decision on how to use spells
	char				SpellCat[MAX_STRING];	// what SpellType() is this, so i dont have to check over and over
	char				SpellType[MAX_STRING];	// this will be a custom spell type for each routine.  Ie. Fast Heal, Normal Heal, Fire, Cold, Tash, because I will use this info later on checks and prioritization
	OPTIONS				SpellTypeOption;		// int conversion from spell type
	ULONGLONG			Duration;				// my actual duration, will use GetDuration2() and store it so i dont have to calculate every cast.
	int					UseOnce;				// only use this spell once per mob? this will be an option ini entry
	int					ForceCast;				// should i force memorize this spell if it is not already memmed?  ini entry, will be 1 on buffs, optional on rest
	int					Use;					// should i use this spell at all?  this will allow people to memorize but not use spells in case they want manual control
	int					StartAt;				// individual control of when to start using this spell on friend or foe
	int					StopAt;					// individual control of when to stop using this spell on friend or foe
	int					NamedOnly;				// ini entry to let user determine if they only want to use this on a named.  could use it as a level to say only use it on named above this level to avoid wasting it on easy named. int for now
	int					Priority;				// ini entry that will default to 0, but we can set this priority internally for things like heals and nukes to determine order of cast.  higher number = higher priority
	ULONGLONG			Recast;					// ini entry that will give us a timestamp to reuse a spell if different from it's fastest refresh.  I may only want to cast swarm pets once every 30 seconds even though i could every 12.
	ULONGLONG			LastCast;				// timestamp of last time i cast this spell
	int					LastTargetID;			// last target id that i cast this spell on.
	char				Color[10];				// color to use when cast a spell
	DWORD				ID;						// ID, for Alt ability for now, maybe others
	DWORD				PreviousID;				// Needed for my current memmed spells to see if something changed
	int					Type;					// TYPE_AA, TYPE_SPELL, TYPE_DISC, TYPE_ITEM
	int					CastTime;				// Casting time
	DWORD				TargetID;				// Id of target i want to cast on
	int					IniMatch;				// what spell number is this in the ini, if any.
												// save this just in case:  void(*CheckFunc)(std::vector<_BotSpell> &, int);
	void(*CheckFunc)(int);
} BotSpell, *PBotSpell;

typedef struct _Spawns
{
	vector<_BotSpell>	vSpellList;
	char				SpawnBuffList[MAX_STRING];
	DWORD				ID;
	bool				Add; // is this a confirmed add?
	PSPAWNINFO			Spawn;
	SPAWNINFO			SpawnCopy;
	int					Priority;
	bool				NeedsCheck;
	ULONGLONG			LastChecked;
	ULONGLONG			Slow;  //mq2 time stamp of when slow should last until, repeat et al for rest
	ULONGLONG			Malo;
	ULONGLONG			Tash;
	ULONGLONG			Haste;
	ULONGLONG			Root;
	ULONGLONG			Snare;
	ULONGLONG			Mez;
	ULONGLONG			DS;
	ULONGLONG			RevDS;
	ULONGLONG			Cripple;
	ULONGLONG			Charge;
	ULONGLONG			Concuss;
	ULONGLONG			MindFroze;
	ULONGLONG			Charm;
	ULONGLONG			Aego;
	ULONGLONG			Skin;
	ULONGLONG			Focus;
	ULONGLONG			Regen;
	ULONGLONG			Symbol;
	ULONGLONG			Clarity;
	ULONGLONG			Pred;
	ULONGLONG			Strength;
	ULONGLONG			Brells;
	ULONGLONG			SV;
	ULONGLONG			SE;
	ULONGLONG			HybridHP;
	ULONGLONG			Growth;
	ULONGLONG			Shining;
	ULONGLONG			DeepSleep;
	int					PoisonCounters;
	int					DiseaseCounters;
	int					CorruptedCounters;
	int					CurseCounters;
	int					DetrimentalCounters;
	ULONGLONG			Hot;
	ULONGLONG			Fero;
	BYTE				State;
} Spawns, *PSpawns, SpawnCopy;

#pragma region FunctionDeclarations
// declaration of create functions
void CreateAA();
void CreateClickies();
void CreateDisc();
void CreateHeal();

// declaration of check routines
void CheckAA(int spell);
void CheckAggro(int spell);
void CheckDisc(int spell);
void CheckHeal(int spell);

// declaration of general functions
int			CalcDuration(PSPELL pSpell);
void		BotCastCommand(_BotSpell &spell);
VOID		DebugWrite(PCHAR szFormat, ...);
void		EQBCSwap(char startColor[MAX_STRING]);
double		botround(double d);

// declaration of spawn functions
void		CheckGroupPets(int i);
int			PctEnd(PSPAWNINFO pSpawn);
int			PctHP(PSPAWNINFO pSpawn);
int			PctMana(PSPAWNINFO pSpawn);

// declaration of spell functions
void		DiscCategory(PSPELL pSpell);
DWORD		GetSpellDuration2(PSPELL pSpell);
void		LoadBotSpell(vector<_BotSpell> &v, char VectorName[MAX_STRING]);
void		PopulateIni(vector<_BotSpell> &v, char VectorName[MAX_STRING]);
void		SortSpellVector(vector<_BotSpell> &v);
void		SpellCategory(PSPELL pSpell);
void		SpellType(PSPELL pSpell);
bool		ValidDet(PSPELL pSpell, PSPAWNINFO Target);
bool		ValidBen(PSPELL pSpell, PSPAWNINFO Target);
#pragma endregion FunctionDeclarations

#pragma region VariableDefinitions
// bool declares
bool	BardClass = false, DEBUG_DUMPFILE = false, ConfigureLoaded = false, InCombat = false, summoned = false, UseManualAssist = false, UseNetBots = false;

// char declares
char	AddList[MAX_STRING] = { 0 }, AssistName[MAX_STRING] = { 0 }, BodyTypeFix[MAX_STRING] = { 0 }, CurrentRoutine[MAX_STRING] = { 0 }, EQBCColor[MAX_STRING] = { 0 },
INISection[MAX_STRING] = { 0 }, NetBotsName[MAX_STRING] = "NULL", spellCat[MAX_STRING] = { 0 }, spellType[MAX_STRING] = { 0 },
conColor[MAX_STRING] = { 0 };

// DWORD declares
DWORD LastBodyID;

// int declares
int		AnnounceAdds = 1, AnnounceEcho = 1, AnnounceEQBC = 0, AssistAt = 100, AssistRange = 100, DefaultGem = 1, dotExtend = 0, hotExtend = 0,
mezExtend = 0;

// float declares
float	benDurExtend = 0.00, benRngExtend = 1.00, detDurExtend = 0.00, detRngExtend = 1.00, fCampX, fCampY, fCampZ, FightX = 0, FightY = 0, FightZ = 0,
reinforcement = 0.00, WarpDistance = 0.00;

// ULONGLONG declares
ULONGLONG AssistTimer = 0, LastAnnouncedSpell = 0, SpellTimer = 0;

//vector _BotSpell declares
//vector<_BotSpell> vMaster, vMemmedSpells, vTemp;
vector<_BotSpell> vMaster;
vector<_BotSpell> vMemmedSpells(NUM_SPELL_GEMS - 1, _BotSpell());
vector<_BotSpell> vTemp;

//vector int declares
vector<int> vClickyPrestige;

// vector PSPELL declares
vector<PSPELL> vClickySpell;

// vector PSPAWNINFO declares
_Spawns KillTarget, xNotTargetingMe, xTargetingMe; // we always store the current kill target here
vector<_Spawns> vGroup, vXTarget, vSpawns, vPets; // manage all the various spawns, and if something is mezzed, dont remove it unless it is banished.



												  // vector string declares
vector<string> vClicky;

// map declares
std::map<string, string> SpellIf;
std::map<string, string>::iterator SpellIt;

// deque declares
deque<_Spawns>	vAdds;

// PCHAR declares
PCHAR ChatColors[] = { "\ay", "\a-y", "\ao", "\a-o", "\ag", "\a-g", "\ab", "\a-b", "\ar", "\a-r",
"\at", "\a-t", "\am", "\a-m", "\ap", "\a-p", "\aw", "\a-w", "\ax", "\a-x", "\au", "\a-u", NULL };
PCHAR EQBCColors[] = { "[+y+]", "[+Y+]", "[+o+]", "[+O+]", "[+g+]", "[+G+]", "[+b+]", "[+B+]", "[+r+]", "[+R+]", "[+t+]",
"[+T+]", "[+m+]", "[+M+]", "[+p+]", "[+P+]", "[+w+]", "[+W+]", "[+x+]", "[+X+]", "[+u+]", "[+U+]", NULL };
PCHAR cast_status[] = { "SUCCESS","INTERRUPTED","RESIST","COLLAPSE","RECOVER","FIZZLE","STANDING","STUNNED","INVISIBLE","NOTREADY",
"OUTOFMANA","OUTOFRANGE","NOTARGET","CANNOTSEE","COMPONENTS","OUTDOORS","TAKEHOLD","IMMUNE","DISTRACTED","ABORTED",	"UNKNOWN",	"IMMUNE_RUN", "IMMUNE_MEZ","IMMUNE_SLOW",NULL };
PCHAR DefaultSection[] = { "AA", "Aggro","Aura","Bard","Buff","Call","Charm","Clickies", "ClickyBuffs","Items",
"ClickyNukes","Debuff","Dot","Endurance","Fade","FightBuff","Grab","Heal","HealPet","HealToT",
"ImHit","Jolt","KnockBack","Lifetap","MainTankBuff","Mana","Nuke","NukeToT","Pet","Rez",
"Root","SelfBuff","Snare","SummonItem","Swarm", "Disc","Cure", "Mez", NULL }; // 10 per line
PCHAR DefaultColor[] = { "\ap", "\a-r","\a-m","\a-t","\aw","\ao","\am","\a-o","\a-o","\a-o",
"\a-o","\ay","\ay","\ag","\a-w","\am","\a-g","\at","\a-t","\a-t",
"\ay","\at","\a-g","\ag","\ag","\ao","\ar","\a-t","\a-t","\a-o",
"\a-g","\aw","\a-g","\a-o","\a-r", "\a-y","\ag","\a-m", NULL };// 10 per line
PCHAR DefaultPriority[] = { "30","90", "0", "0", "0", "0", "0", "0", "0", "0",
"0","80", "70", "0", "95", "0", "0", "99", "85", "0",
"98","100", "0", "91", "0", "0", "45", "0", "0", "0",
"50","0", "75", "0", "72","60","0", "100", NULL };// 10 per line
PCHAR DefaultStartAt[] = { "99","100", "100", "100", "100", "100", "100", "100", "100", "100",
"99","100", "99", "17", "50", "100", "100", "80", "80", "80",
"80","70", "100", "80", "100", "80", "99", "99", "100", "100",
"100","100", "100", "100", "99","99", "99", "100", NULL };// 10 per line
PCHAR DefaultStopAt[] = { "0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0","0", "0","0",NULL };// 10 per line
PCHAR DefaultUse[] = { "1","1", "1", "1", "1", "1", "1", "1", "1", "1",
"1","1", "1", "1", "1", "1", "1", "1", "1", "1",
"1","1", "1", "1", "1", "1", "1", "1", "1", "1",
"1","1", "1", "1", "1","1","1","1", NULL };// 10 per line
PCHAR DefaultForceCast[] = { "0","0", "0", "0", "1", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "1", "0", "1",
"0","1", "0", "1", "0", "0","0","0", NULL };// 10 per line
PCHAR DefaultNamedOnly[] = { "0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0","0","0", NULL };// 10 per line
PCHAR DefaultUseOnce[] = { "0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0", "0", "0", "0", "0",
"0","0", "0", "0", "0", "0","0","0", NULL };// 10 per line

											// special declares
OPTIONS	spellTypeInt;
_BotSpell EndRegenSpell;
#pragma endregion VariableDefinitions
#pragma endregion Prototypes

#pragma region Inlines
// Returns TRUE if character is in game and has valid character data structures
inline bool InGameOK()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

// Returns TRUE if the specified UI window is visible
static inline BOOL WinState(CXWnd *Wnd)
{
	return (Wnd && ((PCSIDLWND)Wnd)->dShow);
}

static inline LONG GetSpellAttribX(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellAttrib(spell, slot) : 254;
}
static inline LONG GetSpellBaseX(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellBase(spell, slot) : 0;
}
static inline LONG GetSpellBase2X(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellBase2(spell, slot) : 0;
}
static inline LONG GetSpellMaxX(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellMax(spell, slot) : 0;
}
static inline LONG GetSpellCalcX(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellCalc(spell, slot) : 0;
}
#pragma endregion Inlines

#pragma region GeneralFunctionDefinitions
void Announce(_BotSpell &spell)
{
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(spell.TargetID))
	{
		if (AnnounceEcho && pSpawn && LastAnnouncedSpell != spell.Spell->ID)
		{
			WriteChatf("%s%s \aw--> %s", spell.Color, spell.SpellName, pSpawn->DisplayedName);
		}
		if (AnnounceEQBC && pSpawn && LastAnnouncedSpell != spell.Spell->ID)
		{
			EQBCSwap(spell.Color);
			char castLine[MAX_STRING] = { 0 };
			sprintf_s(castLine, "/bc %s%s [+x+]--> %s", EQBCColor, spell.SpellName, pSpawn->DisplayedName);
			HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
		}
		LastAnnouncedSpell = spell.Spell->ID;
	}
}

DWORD ChkCreateDir(char* pszDir)
{
	DWORD dwAtt = 0;
	dwAtt = GetFileAttributes(pszDir);

	if (FILE_ATTRIBUTE_DIRECTORY == dwAtt) return(dwAtt);

	char  acDir[MAX_PATH];
	strcpy_s(acDir, pszDir);

	bool bCreateFolder = true;
	static bool bIsFile = false;

	char* pc = NULL;
	pc = strrchr(acDir, '\\');

	if (pc) {
		if (!bIsFile) {
			bIsFile = true;
			bCreateFolder = false;
		}

		*pc = (char)0;
	}
	else
		return(0xffffffff);

	dwAtt = ChkCreateDir(acDir);

	if (FILE_ATTRIBUTE_DIRECTORY == dwAtt && bCreateFolder) {
		if (!CreateDirectory(pszDir, NULL))
			return GetLastError();

		return(dwAtt);
	}

	return(0);
}

VOID DebugWrite(PCHAR szFormat, ...)
{
	if (!DEBUG_DUMPFILE)
		return;
	try
	{
		va_list vaList;
		va_start(vaList, szFormat);
		int len = _vscprintf(szFormat, vaList) + 1;
		if (char *szOutput = (char *)LocalAlloc(LPTR, len + 32))
		{
			vsprintf_s(szOutput, len + 32, szFormat, vaList);
			FILE *fOut = NULL;
			CHAR Filename[MAX_STRING] = { 0 };
			CHAR szBuffer[MAX_STRING] = { 0 };
			DWORD i;
			sprintf_s(Filename, "%s\\MQ2Bot_Debug.log", gszLogPath);
			ChkCreateDir(gszLogPath);
			for (i = 0; i < strlen(Filename); i++)
			{
				if (Filename[i] == '\\')
				{
					strncpy_s(szBuffer, Filename, i);
					szBuffer[i] = 0;
				}
			}
			fOut = NULL;
			errno_t err = fopen_s(&fOut, Filename, "at");
			if (err)
			{
				return;
			}
			char tmpbuf[128];
			struct tm today;
			time_t tm;
			tm = time(NULL);
			localtime_s(&today, &tm);
			strftime(tmpbuf, 128, "%Y/%m/%d %H:%M:%S", &today);
			CHAR PlainText[MAX_STRING] = { 0 };
			StripMQChat(szOutput, PlainText);
			sprintf_s(szBuffer, "[%s] %s", tmpbuf, PlainText);
			for (unsigned int i = 0; i < strlen(szBuffer); i++) {
				if (szBuffer[i] == 0x07) {
					if ((i + 2) < strlen(szBuffer))
						strcpy_s(&szBuffer[i], MAX_STRING, &szBuffer[i + 2]);
					else
						szBuffer[i] = 0;
				}
			}
			fprintf(fOut, "%s\n", szBuffer);
			fclose(fOut);
			LocalFree(szOutput);
		}
	}
	catch (exception &e)
	{
		DEBUG_DUMPFILE = false;
		FatalError("[%s::%s] Severe Error: %s : %s", PLUGIN_NAME, __FUNCTION__, e.what(), typeid(e).name());
	}
	catch (...)
	{
		DEBUG_DUMPFILE = false;
		FatalError("[%s::%s] Severe Error: Unknown", PLUGIN_NAME, __FUNCTION__);
	}
}

void DurationSetup()
{
	if (!InGameOK())
		return;
	PCHARINFO2 pChar2 = GetCharInfo2();
	int ben = 0, det = 0, mez = 0;
	//remove char itemName[MAX_STRING], spellName[MAX_STRING], itemlevel[MAX_STRING];
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray)
	{
		for (int i = 1; i < NUM_INV_SLOTS; i++)
		{
			if (PCONTENTS Cont = GetCharInfo2()->pInventoryArray->InventoryArray[i])
				if (PSPELL pFocus = GetSpellByID(GetItemFromContents(Cont)->Focus.SpellID))
				{
					if (GetSpellAttribX(pFocus, 0) == 128 && GetSpellBaseX(pFocus, 2) == 1)
						benDurExtend = (float)GetSpellBaseX(pFocus, 0) / 100;
					if (GetSpellAttribX(pFocus, 0) == 128 && GetSpellBaseX(pFocus, 2) == 0)
						detDurExtend = (float)GetSpellBaseX(pFocus, 0) / 100;
					if (GetSpellAttribX(pFocus, 0) == 129 && GetSpellBaseX(pFocus, 3) == 1)
						benRngExtend = (float)GetSpellBaseX(pFocus, 0) / 100 + 1;
					if (GetSpellAttribX(pFocus, 0) == 129 && GetSpellBaseX(pFocus, 3) == 0)
						detRngExtend = (float)GetSpellBaseX(pFocus, 0) / 100 + 1;
				}
		}
		if (reinforcement < 0.01 && GetCharInfo()->pSpawn->Level>55) {
			long Index = GetAAIndexByName("Spell Casting Reinforcement");
			if (Index > 0)
			{
				if (PlayerHasAAAbility(Index))
					for (int j = 0; j < AA_CHAR_MAX_REAL; j++) {
						if (pPCData->GetAlternateAbilityId(j) == Index)
							reinforcement = (float)(0.025*GetCharInfo2()->AAList[j].PointsSpent);
						if (reinforcement > 0.01 || !pPCData->GetAlternateAbilityId(j))
							break;
					}
			}
			Index = GetAAIndexByName("Prolonged Salve");
			if (Index > 0)
			{
				if (PlayerHasAAAbility(Index))
					for (int j = 0; j < AA_CHAR_MAX_REAL; j++) {
						if (pPCData->GetAlternateAbilityId(j) == Index)
							hotExtend = (int)botround(GetCharInfo2()->AAList[j].PointsSpent / 12);
						if (hotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
							break;
					}
			}
			Index = GetAAIndexByName("Powerful Elixirs");
			if (Index > 0)
			{
				if (PlayerHasAAAbility(Index))
					for (int j = 0; j < AA_CHAR_MAX_REAL; j++) {
						if (pPCData->GetAlternateAbilityId(j) == Index)
							hotExtend = (int)botround(GetCharInfo2()->AAList[j].PointsSpent / 12);
						if (hotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
							break;
					}
			}
			if (PlayerHasAAAbility(GetAAIndexByName("Mesmerization Mastery"))) {
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
					if (pPCData->GetAlternateAbilityId(j)) {
						if (pPCData->GetAlternateAbilityId(j) == GetAAIndexByName("Mesmerization Mastery")) {
							mez = GetCharInfo2()->AAList[j].PointsSpent;
							if (mez >= 28) // level 3+ (add more checks before this if expansions add more ranks)
								mezExtend += 3;
							else if (mez >= 20) // level 2
								mezExtend += 2;
							else if (mez >= 12) // level 1
								mezExtend += 1;
							break;
						}
					}
				}
			}
			Index = GetAAIndexByName("Lingering Death");
			if (Index > 0)
			{
				if (PlayerHasAAAbility(Index))
					for (int j = 0; j < AA_CHAR_MAX_REAL; j++) {
						if (pPCData->GetAlternateAbilityId(j) == Index)
							dotExtend = (int)botround(GetCharInfo2()->AAList[j].PointsSpent / 13);
						if (dotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
							break;
					}
			}
			Index = GetAAIndexByName("Extended Pestilence");
			if (Index > 0)
			{
				if (PlayerHasAAAbility(Index))
					for (int j = 0; j < AA_CHAR_MAX_REAL; j++) {
						if (pPCData->GetAlternateAbilityId(j) == Index)
							if (GetCharInfo2()->AAList[j].PointsSpent >= 48)
								dotExtend = 3;
						if (dotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
							break;
					}
			}

		}
	}
}

void EQBCSwap(char startColor[MAX_STRING])
{
	strcpy_s(EQBCColor, "[+x+]");
	for (int i = 0; ChatColors[i] && EQBCColors[i]; i++)
	{
		if (strstr(startColor, ChatColors[i]))
		{
			strcpy_s(EQBCColor, EQBCColors[i]);
			return;
		}
	}
}

bool FindPlugins(PCHAR PluginName)
{
	PMQPLUGIN pPlugin = pPlugins;
	while (pPlugin)
	{
		if (!_stricmp(pPlugin->szFilename, PluginName))
			return true;
		pPlugin = pPlugin->pNext;
	}
	return false;
}

int FindSpellAttrib(PSPELL pSpell, int attrib)
{
	for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot)
	{
		if (GetSpellAttrib(pSpell, slot) == attrib)
			return slot;
	}

	return -1; // not found
}

bool HasSpellAttrib(PSPELL pSpell, int attrib)
{
	return FindSpellAttrib(pSpell, attrib) != -1;
}

PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	if (DEBUG_DUMPFILE)
		DebugWrite(Line);
	return 0;
}

int PctEnd(PSPAWNINFO pSpawn)
{
	if (pSpawn)
	{
		if (pSpawn->EnduranceMax > 0)
			return ((pSpawn->EnduranceCurrent * 100) / pSpawn->EnduranceMax);
		else
			return 0;
	}
	return 0;
}

int PctHP(PSPAWNINFO pSpawn)
{
	if (pSpawn)
	{
		if (pSpawn->HPMax > 0)
			return (unsigned int)((pSpawn->HPCurrent * 100) / pSpawn->HPMax);
		else
			return 0;
	}
	return 0;
}

int PctMana(PSPAWNINFO pSpawn)
{
	if (pSpawn)
	{
		if (pSpawn->ManaMax > 0)
			return ((pSpawn->ManaCurrent * 100) / pSpawn->ManaMax);
		else
			return 0;
	}
	return 0;
}

void PopulateIni(vector<_BotSpell> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	int vSize = v.size();
	for (int i = 0; i < vSize; i++)
	{
		sprintf_s(szSpell, "%sTotal", VectorName);
		_itoa_s(v.size(), szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dName", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].SpellName, INIFileName);
		//sprintf_s(szSpell, "%sSpellIconName%d", VectorName, i);
		//WritePrivateProfileString(INISection, szSpell, v[i].SpellIconName, INIFileName);
		sprintf_s(szSpell, "%s%dIf", VectorName, i);
		if (IsNumber(v[i].If))
		{
			_itoa_s(atoi(v[i].If), szTemp, 10);
			WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		}

		else
			WritePrivateProfileString(INISection, szSpell, v[i].If, INIFileName);
		sprintf_s(szSpell, "%s%dGem", VectorName, i);
		if (IsNumber(szSpell))
		{
			_itoa_s(atoi(v[i].Gem), szTemp, 10);
			WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		}

		else
			WritePrivateProfileString(INISection, szSpell, v[i].Gem, INIFileName);
		sprintf_s(szSpell, "%s%dUseOnce", VectorName, i);
		_itoa_s(v[i].UseOnce, szTemp, 10);

		sprintf_s(szSpell, "%s%dForceCast", VectorName, i);
		_itoa_s(v[i].ForceCast, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dUse", VectorName, i);
		_itoa_s(v[i].Use, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dStartAt", VectorName, i);
		_itoa_s(v[i].StartAt, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dStopAt", VectorName, i);
		_itoa_s(v[i].StopAt, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dNamedOnly", VectorName, i);
		_itoa_s(v[i].NamedOnly, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%s%dPriority", VectorName, i);
		_itoa_s(v[i].Priority, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
	}
}

double botround(double d)
{
	return floor(d + 0.5);
}

bool SpellStacks(PSPELL pSpell)
{
	if (!InGameOK())
		return true;
	unsigned long nBuff;
	PCHARINFO2 pChar = GetCharInfo2();
	bool stacks = true;
	//remove char cTest[MAX_STRING];
	PSPELL tmpSpell2;
	// Check Buffs
	for (nBuff = 0; nBuff < NUM_LONG_BUFFS && pChar; nBuff++)
	{
		if (pChar->Buff[nBuff].SpellID > 0 && GetSpellByID(pChar->Buff[nBuff].SpellID)) {
			PSPELL tmpSpell = GetSpellByID(pChar->Buff[nBuff].SpellID);
			if (tmpSpell && pSpell)
			{
				for (int nSlot = 0; nSlot < GetSpellNumEffects(pSpell); nSlot++)
				{
					int attrib = GetSpellAttrib(pSpell, nSlot);

					if (attrib == 374 || attrib == 419 || attrib == 339)
					{
						tmpSpell2 = GetSpellByID(GetSpellBase2(pSpell, nSlot));
						if (tmpSpell2 && !BuffStackTest(tmpSpell, tmpSpell2) || !outdoor && tmpSpell2->ZoneType == 1 || tmpSpell == tmpSpell2 || attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name))
						{
							stacks = false;
							return stacks;
						}
					}
					if (attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name) || !outdoor && tmpSpell->ZoneType == 1)
					{
						stacks = false;
						return stacks;
					}
				}
				if ((!BuffStackTest(pSpell, tmpSpell) || pSpell == tmpSpell || strstr(pSpell->Name, tmpSpell->Name)) || !outdoor && pSpell->ZoneType == 1)
				{
					stacks = false;
					return stacks;
				}
			}
		}
	}
	// Check Songs
	for (nBuff = 0; nBuff < NUM_SHORT_BUFFS && pChar; nBuff++)
	{
		if (pChar->ShortBuff[nBuff].SpellID > 0) {
			PSPELL tmpSpell = GetSpellByID(pChar->ShortBuff[nBuff].SpellID);
			if (tmpSpell && pSpell && (!(tmpSpell->ClassLevel[Bard] == 255 ? 0 : tmpSpell->ClassLevel[Bard]) && (!(GetSpellAttribX(pSpell, 0) == 58) || !(pSpell->DurationWindow == 1))))
			{
				for (int nSlot = 0; nSlot < GetSpellNumEffects(pSpell); nSlot++)
				{
					int attrib = GetSpellAttrib(pSpell, nSlot);

					if (attrib == 374 || attrib == 419)
					{
						tmpSpell2 = GetSpellByID(GetSpellBase2(pSpell, nSlot));
						if (tmpSpell2 && !BuffStackTest(tmpSpell, tmpSpell2) || pSpell == tmpSpell2 || tmpSpell == tmpSpell2 || strstr(pSpell->Name, tmpSpell->Name))
						{
							stacks = false;
							return stacks;
						}
					}
					if (attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name))
					{
						stacks = false;
						return stacks;
					}
				}
				if ((!BuffStackTest(pSpell, tmpSpell) || pSpell == tmpSpell || strstr(pSpell->Name, tmpSpell->Name)))
				{
					stacks = false;
					return stacks;
				}
			}
		}
	}
	//check disc
	if (pCombatAbilityWnd) {
		if (CXWnd *Child = ((CXWnd*)pCombatAbilityWnd)->GetChildItem("CAW_CombatEffectLabel")) {
			CHAR szBuffer[2048] = { 0 };
			if (GetCXStr(Child->WindowText, szBuffer, 2047) && szBuffer[0] != '\0') {
				PSPELL tmpSpell = GetSpellByName(szBuffer);
				if (pSpell && tmpSpell) {
					if (!BuffStackTest(pSpell, tmpSpell)) {
						stacks = false;
						return stacks;
					}
				}
			}
		}
	}
	return true;
}

#pragma endregion GeneralFunctionDefinitions

#pragma region SpawnFunctionDefinitions
bool sortSpawnByPriority(const Spawns &lhs, const Spawns &rhs) { return lhs.Priority > rhs.Priority; } // Sort spell vectors by priority

void SortSpawnVector(vector<_Spawns> &v)  // the actual sorting of a spawn vector by priority
{
	sort(v.begin(), v.end(), sortSpawnByPriority);
}
void ConColorSwap(PSPAWNINFO pSpawn)
{
	if (!pSpawn)
		return;
	if (ConColor(pSpawn) == CONCOLOR_GREY)
		strcpy_s(conColor, "\a-w");
	if (ConColor(pSpawn) == CONCOLOR_GREEN)
		strcpy_s(conColor, "\ag");
	if (ConColor(pSpawn) == CONCOLOR_LIGHTBLUE)
		strcpy_s(conColor, "\at");
	if (ConColor(pSpawn) == CONCOLOR_BLUE)
		strcpy_s(conColor, "\au");
	if (ConColor(pSpawn) == CONCOLOR_WHITE)
		strcpy_s(conColor, "\aw");
	if (ConColor(pSpawn) == CONCOLOR_YELLOW)
		strcpy_s(conColor, "\ay");
	if (ConColor(pSpawn) == CONCOLOR_RED)
		strcpy_s(conColor, "\ay");
}

void CheckAdds()
{
	if (!InGameOK())
		return;
	strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
	DebugWrite("Checking %s", CurrentRoutine); // test code
	if (!GetCharInfo()->pXTargetMgr)
		return;
	char szXTAR[MAX_STRING];
	char szXTAR2[MAX_STRING];
	char testAddList[MAX_STRING];
	int xtar = 0;
	if (ExtendedTargetList* etl = GetCharInfo()->pXTargetMgr)
	{
		vAdds.clear();
		InCombat = false;
		int npc = 0;
		int leastaggro = 100;
		int mostaggro = 99;
		_Spawns aggro;
		xNotTargetingMe = aggro;
		xTargetingMe = aggro;
		if (((PCPLAYERWND)pPlayerWnd))
			if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
				if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
					InCombat = true;
		try // access spawn data crashes people when something despawns so we need to try, and we need to use memcpy of the data.
		{
			for (int n = 0; n < etl->XTargetSlots.Count; n++)
			{
				XTARGETSLOT& slot = etl->XTargetSlots[n];
				if (slot.SpawnID > 0)
				{
					if (slot.xTargetType)
						sprintf_s(szXTAR, "%s", GetXtargetType(slot.xTargetType));
					PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(slot.SpawnID);
					if (pSpawn)
					{
						_Spawns add;
						SPAWNINFO tSpawn;
						memcpy(&tSpawn, pSpawn, sizeof(SPAWNINFO));
						if (pSpawn && (strstr(szXTAR, "NPC") || strstr(szXTAR, "Hater") || (strstr(szXTAR, "Target") || strstr(szXTAR, "target") || strstr(szXTAR, "_TARGET") && !strstr(szXTAR, "EMPTY")) && InCombat && tSpawn.Type == SPAWN_NPC && tSpawn.Type != SPAWN_CORPSE))
						{
							add.Spawn = pSpawn;
							add.SpawnCopy = tSpawn;
							add.Add = true;
							vAdds.push_back(add);
						}
						if (pAggroInfo)
						{
							if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct < leastaggro)
							{
								leastaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
								xNotTargetingMe.SpawnCopy = tSpawn;
							}
							if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct > mostaggro && !GetSpawnByID(xTargetingMe.ID))
							{
								mostaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
								xTargetingMe.SpawnCopy = tSpawn;
							}
						}
						if (pSpawn && (strstr(szXTAR, "NPC") || strstr(szXTAR, "Target") || strstr(szXTAR, "target") || strstr(szXTAR, "_TARGET") && !strstr(szXTAR, "EMPTY")) && !InCombat && tSpawn.Type == SPAWN_NPC && tSpawn.Type != SPAWN_CORPSE)
						{
							add.SpawnCopy = tSpawn;
							add.Spawn = pSpawn;
							add.Add = false;
							vAdds.push_back(add);
						}
					}
				}
			}
		}
		catch (...)
		{
			DebugSpewAlways("MQ2Bot::CheckAdds() **Exception** 1");
		}
	}

	if (UseManualAssist && strlen(AssistName) > 3)
	{
		int doAssist = 0;
		PCHARINFO pChar = GetCharInfo();
		char assist[MAX_STRING];
		sprintf_s(assist, "/squelch /assist %s", AssistName);
		if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
		{
			if (DistanceToSpawn(pChar->pSpawn, pMyTarget) > AssistRange && MQGetTickCount64() > AssistTimer)
				doAssist = 1;
		}
		else
		{
			if (MQGetTickCount64() > AssistTimer)
				doAssist = 1;
		}

		if (doAssist)
		{
			HideDoCommand(pChar->pSpawn, assist, FromPlugin);
			AssistTimer = MQGetTickCount64() + 2000LL;
			vAdds.clear();
			InCombat = false;
			if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
			{
				if (pMyTarget->Type == SPAWN_NPC)
				{
					if (((PCPLAYERWND)pPlayerWnd))
						if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
							if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
								InCombat = true;
					if (InCombat || DistanceToSpawn(pChar->pSpawn, pMyTarget) < AssistRange && PctHP(pMyTarget) < AssistAt)
					{
						SPAWNINFO tSpawn;
						memcpy(&tSpawn, pMyTarget, sizeof(SPAWNINFO));
						_Spawns add;
						add.SpawnCopy = tSpawn;
						add.Spawn = pMyTarget;
						add.Add = true;
						vAdds.push_front(add);
					}
					else
					{
						SPAWNINFO tSpawn;
						memcpy(&tSpawn, pMyTarget, sizeof(SPAWNINFO));
						_Spawns add;
						add.SpawnCopy = tSpawn;
						add.Spawn = pMyTarget;
						add.Add = false;
						vAdds.push_back(add);
					}
				}
			}
		}
	}
	if (UseNetBots)
	{
		sprintf_s(testAddList, "${If[${NetBots[%s].InZone} && ${NetBots.Client.Find[%s]} && ${NetBots[%s].TargetID} && ${NetBots[%s].TargetHP}<=%d && (${NetBots[%s].CombatState}==0||${NetBots[%s].Attacking}) && ${Spawn[id ${NetBots[%s].TargetID}].Distance}<=%d,1,0]}", NetBotsName, NetBotsName, NetBotsName, NetBotsName, AssistAt, NetBotsName, NetBotsName, NetBotsName, AssistRange);
		ParseMacroData(testAddList, MAX_STRING);
		if (atoi(testAddList) == 1)
		{
			sprintf_s(testAddList, "${NetBots[%s].TargetID}", NetBotsName);
			ParseMacroData(testAddList, MAX_STRING);
			bool check = false;
			int vSize = vAdds.size();
			for (int i = 0; i < vSize; i++)
			{
				if (vAdds[i].ID == (DWORD)atol(testAddList))
					check = true;
			}
			if (!check)
			{
				if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(testAddList)))
				{
					SPAWNINFO tSpawn;
					memcpy(&tSpawn, pSpawn, sizeof(SPAWNINFO));
					_Spawns add;
					add.SpawnCopy = tSpawn;
					add.Spawn = pSpawn;
					add.Add = false;
					vAdds.push_front(add);
				}
			}

		}
	}
	if (vAdds.size() && AnnounceAdds > 0)
	{
		strcpy_s(szXTAR2, "|");
		int vSize = vAdds.size();
		for (int i = 0; i < vSize; i++)
		{
			try
			{
				if (vAdds[i].Spawn && vAdds[i].Spawn->Type != SPAWN_CORPSE)
				{
					// vAdds[i].Spawn = (PSPAWNINFO)GetSpawnByID(vAdds[i].ID);
					ConColorSwap(vAdds[i].Spawn);
					sprintf_s(szXTAR, "%s%s\ar|", conColor, vAdds[i].SpawnCopy.Name);
					strcat_s(szXTAR2, szXTAR);
				}
			}
			catch (...)
			{
				DebugSpewAlways("MQ2Bot::CheckAdds() **Exception** 2");
			}

		}
		if (strcmp(AddList, szXTAR2))
		{
			strcpy_s(AddList, szXTAR2);
			WriteChatf("\arAdds: %s", szXTAR2);
		}
		if (FightX == 0 && FightY == 0 && FightZ == 0)
		{
			FightX = GetCharInfo()->pSpawn->X;
			FightY = GetCharInfo()->pSpawn->Y;
			FightZ = GetCharInfo()->pSpawn->Z;
		}
	}
	if (vAdds.size())
	{
		if (GetSpawnByID(vAdds[0].ID) && vAdds[0].ID != LastBodyID)
		{
			sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", vAdds[0].SpawnCopy.Name);
			ParseMacroData(BodyTypeFix, MAX_STRING);
			LastBodyID = vAdds[0].ID;
		}

	}
	if (!vAdds.size())
	{
		summoned = false;
		FightX = 0;
		FightY = 0;
		FightZ = 0;
		WarpDistance = 0;
	}

	/* FixNote 20160728.1
	Need to check vSpawn, if not there, insert vAdd to there, also double check the vAdds[i].Add flag.
	*/
	int vSize = vAdds.size();
	for (int i = 0; i < vSize; i++)
	{
		int found = 0;
		int vSize = vSpawns.size();
		for (int x = 0; x < vSize; x++)
		{
			if (vSpawns[x].ID == vAdds[i].ID)
			{
				found++;
			}
		}
		if (!found)
			vSpawns.push_back(vAdds[i]);
	}
	SortSpawnVector(vSpawns);
}

void CheckGroup()
{
	if (!InGameOK())
		return;
	try
	{
		strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
		DebugWrite("Checking %s", CurrentRoutine); // test code
		PCHARINFO pChar = GetCharInfo();
		if (pChar && !pChar->pGroupInfo)
		{
			_Spawns gMember;
			gMember.Spawn = pChar->pSpawn;
			gMember.ID = pChar->pSpawn->SpawnID;
			gMember.State = SPAWN_PLAYER;
			vGroup[0] = gMember;
			return;
		}
		PSPAWNINFO pGroupMember;
		char test[MAX_STRING];
		for (int i = 0; i < 6; i++)
		{
			DebugWrite("Checking %s 5", CurrentRoutine); // test code
			sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID > 0)
				{
					if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->Mercenary)
					{
						pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
						if (pGroupMember && (pGroupMember->RespawnTimer || pGroupMember->StandState == STANDSTATE_DEAD))
						{
							//if (vPets[i].ID != 0)
							//	CheckGroupPets(i); //add this later
							_Spawns gMember;
							gMember.Spawn = pGroupMember;
							gMember.ID = pGroupMember->SpawnID;
							gMember.State = STANDSTATE_DEAD;
							vGroup[i] = gMember;
						}
						if (pGroupMember && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
						{
							if (pGroupMember->SpawnID != vGroup[i].ID)
							{
								_Spawns gMember;
								gMember.Spawn = pGroupMember;
								gMember.ID = pGroupMember->SpawnID;
								gMember.State = SPAWN_PLAYER;
								vGroup[i] = gMember;
							}
						}
					}
				}
			}
			else
			{
				if (vGroup[i].ID)
				{
					_Spawns blank;
					vGroup[i] = blank;
					vGroup[i].ID = 0;
				}
			}
		}
	}
	catch (...)
	{
		DebugSpewAlways("MQ2Bot::CheckGroup() **Exception**");
	}
	DebugWrite("Checking %s 10", CurrentRoutine); // test code
	return;
}

void CheckGroupPets(int i)
{
	if (!InGameOK())
		return;
	try
	{
		strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
		DebugWrite("Checking %s", CurrentRoutine); // test code
		PCHARINFO pChar = GetCharInfo();
		if (i == 0)
		{
			if (vGroup[0].ID == pChar->pSpawn->SpawnID)
				if (!pChar->pSpawn->PetID && vPets[0].ID == 0 || pChar->pSpawn->PetID && pChar->pSpawn->PetID != 0xFFFFFFFF && vPets[0].ID == pChar->pSpawn->PetID)
					return;
				else
				{
					// FixNote 20160731.1  might need to add more members here, but for now just setting ID and spawn
					_Spawns pet;
					ZeroMemory(&vPets[0], sizeof(vPets[0]));
					if (!pChar->pSpawn->PetID)
					{
						pet.ID = 0;
						vPets[0] = pet;
					}
					else
					{
						pet.ID = pChar->pSpawn->PetID;
						pet.Spawn = (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
						vPets[0] = pet;
					}
				}
		}
		else
		{
			PSPAWNINFO pGroupMember;
			char test[MAX_STRING];
			sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID > 0)
				{
					if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->Mercenary)
					{
						pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
						if (pGroupMember && (pGroupMember->RespawnTimer || pGroupMember->StandState == STANDSTATE_DEAD))
						{
							if (!pChar->pSpawn->PetID && vPets[i].ID == 0 || pChar->pSpawn->PetID && pChar->pSpawn->PetID != 0xFFFFFFFF && vPets[i].ID == pChar->pSpawn->PetID)
								return;
							else
							{
								// FixNote 20160731.1  might need to add more members here, but for now just setting ID and spawn
								_Spawns pet;
								ZeroMemory(&vPets[i], sizeof(vPets[i]));
								if (!pChar->pSpawn->PetID)
								{
									pet.ID = 0;
									vPets[i] = pet;
								}
								else
								{
									pet.ID = pChar->pSpawn->PetID;
									pet.Spawn = (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
									vPets[i] = pet;
								}
							}
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		DebugSpewAlways("MQ2Bot::CheckGroupPets() **Exception**");
	}
	return;
}


#pragma endregion SpawnFunctionDefinitions

#pragma region SpellFunctionDefinitions

BOOL AAReady(DWORD index)
{
	int result = 0;
	if (pAltAdvManager)
		if (index)
			if (PALTABILITY ability = pAltAdvManager->GetAAById(index))
			{
				if (pAltAdvManager->GetCalculatedTimer(pPCData, ability) > 0)
					pAltAdvManager->IsAbilityReady(pPCData, ability, &result);
			}
	return (result < 0);
}

BOOL DiscReady(char disc[MAX_STRING])
{
	for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
	{
		if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
			if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
			{
				if (!_stricmp(disc, pSpell->Name))
				{
					DWORD timeNow = (DWORD)time(NULL);
					if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) < timeNow)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

BOOL ItemReady(DWORD itemID)
{
	if (PCONTENTS pCont = FindItemByID(itemID))
	{
		if (PITEMINFO pIteminf = GetItemFromContents(pCont))
		{
			if (pIteminf->Clicky.TimerID != -1)
			{
				DWORD timer = GetItemTimer(pCont);
				if (timer == 0)
				{
					return true;
				}
				else if (pIteminf->Clicky.SpellID != -1)
				{
					return true; // insta-click or instant recast
				}
			}
		}
	}
	return false;
}

BOOL SpellReady(DWORD index)
{
	if (PSPELL mySpell = GetSpellByID(index))
	{
		for (unsigned long nGem = 0; nGem < NUM_SPELL_GEMS; nGem++)
		{
			if (PSPELL pSpell = GetSpellByID(GetMemorizedSpell(nGem)))
			{
				if (!_stricmp(mySpell->Name, pSpell->Name))
					if (((PCDISPLAY)pDisplay)->TimeStamp >((PSPAWNINFO)pLocalPlayer)->SpellGemETA[nGem] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA) {
						return true;
					}
			}
		}
	}
	return false;
}

int CalcDuration(PSPELL pSpell)
{
	float fCalc = 1.0;
	int iCalc = 0;
	if (!pSpell)
		return 0;
	iCalc = GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer);
	if (pSpell->SpellType == 0) {
		fCalc = fCalc + detDurExtend;
		if (iCalc > 3 && pSpell->Category == 20)
			iCalc = (int)botround(fCalc) + dotExtend;
		else
			iCalc = (int)botround(fCalc);
		if (iCalc > 3 && pSpell->Category == 31)
			iCalc = (int)botround(fCalc) + mezExtend;
		else
			iCalc = (int)botround(fCalc);
	}
	if (pSpell->SpellType == 1) {
		fCalc = fCalc + benDurExtend + reinforcement;
		if (iCalc > 3 && pSpell->Subcategory == 32)
			iCalc = (int)botround(fCalc) + hotExtend;
		else
			iCalc = (int)botround(fCalc);
		if (iCalc > 3 && pSpell->Subcategory == 140)
			iCalc = 4;
	}
	return iCalc;
}

bool CanICast(_BotSpell &spell)
{
	if (!InGameOK())
		return false;
	if (spell.Type == TYPE_AA)
		if (!AAReady(spell.ID))
			return false;
	if (spell.Type == TYPE_SPELL)
		if (!SpellReady(spell.ID))
			return false;
	if (spell.Type == TYPE_ITEM)
		if (!ItemReady(spell.ID))
			return false;
	if (spell.Type == TYPE_DISC)
		if (!DiscReady(spell.SpellName))
			return false;
	if (!spell.Spell->SpellType)
		if (!ValidDet(spell.Spell, (PSPAWNINFO)GetSpawnByID(spell.TargetID)))
			return false;
	if (spell.Spell->SpellType)
		if (!ValidBen(spell.Spell, (PSPAWNINFO)GetSpawnByID(spell.TargetID)))
			return false;
	return true;
}

void CheckMemmedSpells()
{
	if (!InGameOK())
		return;
	bool change = false;
	try
	{
		if (pDisplay && pLocalPlayer)
		{
			for (unsigned long nGem = 0; nGem < NUM_SPELL_GEMS; nGem++)
			{
				if (GetMemorizedSpell(nGem) != 0xFFFFFFFF)
				{
					if (PSPELL pSpell = GetSpellByID(GetMemorizedSpell(nGem)))
					{
						if (pSpell->ID != vMemmedSpells[nGem].ID)
						{
							char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING], spellNum[10], color[10] = { 0 };
							//_BotSpell spell;
							vMemmedSpells[nGem].Spell = pSpell;
							vMemmedSpells[nGem].PreviousID = vMemmedSpells[nGem].ID;
							strcpy_s(vMemmedSpells[nGem].SpellName, pSpell->Name);
							vMemmedSpells[nGem].ID = pSpell->ID;
							_itoa_s(nGem, szTemp, MAX_STRING, 10);
							strcpy_s(vMemmedSpells[nGem].Gem, szTemp);
							SpellType(pSpell);
							vMemmedSpells[nGem].CheckFunc = NULL; // temp code this needs replaced once i dont crash
							strcpy_s(vMemmedSpells[nGem].SpellCat, spellType);
							vMemmedSpells[nGem].SpellTypeOption = spellTypeInt;
							int defStartAt = 0, defUse = 0, defStopAt = 0, defPriority = 0, defNamedOnly = 0, defUseOnce = 0, defForceCast = 0;
							for (int x = 0; DefaultSection[x]; x++)
							{
								if (!_stricmp(spellType, DefaultSection[x]))
								{
									strcpy_s(color, DefaultColor[x]);
									defStartAt = atoi(DefaultStartAt[x]);
									defUse = atoi(DefaultUse[x]);
									defStopAt = atoi(DefaultStopAt[x]);
									defPriority = atoi(DefaultPriority[x]);
									// WriteChatf("%s - %d", spellType, defPriority);
									defNamedOnly = atoi(DefaultNamedOnly[x]);
									defUseOnce = atoi(DefaultUseOnce[x]);
									defForceCast = atoi(DefaultForceCast[x]);
									break;
								}
							}
							int i = nGem + 1;
							int customSpells = GetPrivateProfileInt(INISection, "SpellTotal", 0, INIFileName);
							for (int x = 0; x < customSpells; x++)
							{
								sprintf_s(szSpell, "Spell%dName", x);
								if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
								{
									if (!_stricmp(szSpell, pSpell->Name))
										sprintf_s(spellNum, "Spell%d", x);
								}
							}
							sprintf_s(szSpell, "%sIf", spellNum);
							if (GetPrivateProfileString(INISection, szSpell, "1", szTemp, MAX_STRING, INIFileName))
								strcpy_s(vMemmedSpells[nGem].If, szTemp);
							sprintf_s(szSpell, "%sGem", spellNum);
							if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
								strcpy_s(vMemmedSpells[nGem].Gem, szTemp);
							sprintf_s(szSpell, "%sUseOnce", spellNum);
							vMemmedSpells[nGem].UseOnce = GetPrivateProfileInt(INISection, szSpell, defUseOnce, INIFileName);
							sprintf_s(szSpell, "%sForceCast", spellNum);
							vMemmedSpells[nGem].ForceCast = GetPrivateProfileInt(INISection, szSpell, defForceCast, INIFileName);
							sprintf_s(szSpell, "%sUse", spellNum);
							vMemmedSpells[nGem].Use = GetPrivateProfileInt(INISection, szSpell, defUse, INIFileName);
							sprintf_s(szSpell, "%sStartAt", spellNum);
							vMemmedSpells[nGem].StartAt = GetPrivateProfileInt(INISection, szSpell, defStartAt, INIFileName);
							sprintf_s(szSpell, "%sStopAt", spellNum);
							vMemmedSpells[nGem].StopAt = GetPrivateProfileInt(INISection, szSpell, defStopAt, INIFileName);
							sprintf_s(szSpell, "%sNamedOnly", spellNum);
							vMemmedSpells[nGem].NamedOnly = GetPrivateProfileInt(INISection, szSpell, defNamedOnly, INIFileName);
							sprintf_s(szSpell, "%sPriority", spellNum);
							vMemmedSpells[nGem].Priority = GetPrivateProfileInt(INISection, szSpell, defPriority, INIFileName);
							vMemmedSpells[nGem].LastTargetID = 0;
							vMemmedSpells[nGem].LastCast = 0;
							vMemmedSpells[nGem].Recast = 0;
							vMemmedSpells[nGem].TargetID = 0;
							sprintf_s(szSpell, "%sDuration", spellNum);
							vMemmedSpells[nGem].Duration = !GetSpellDuration2(vMemmedSpells[nGem].Spell) ? 0 : 6000 * GetSpellDuration2(vMemmedSpells[nGem].Spell) * (ULONGLONG)CalcDuration(vMemmedSpells[nGem].Spell);
							strcpy_s(vMemmedSpells[nGem].Color, color);
							DWORD n = 0;
							if (vMemmedSpells[nGem].Type == TYPE_SPELL)
							{
								vMemmedSpells[nGem].CastTime = pCharData1->GetAACastingTimeModifier((EQ_Spell*)vMemmedSpells[nGem].Spell) +
									pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)vMemmedSpells[nGem].Spell, (EQ_Equipment**)&n, 0) +
									vMemmedSpells[nGem].Spell->CastTime;
							}
							change = true;
							if (vMemmedSpells[nGem].SpellTypeOption != ::OPTIONS::ZERO)
								WriteChatf("\arMQ2Bot\aw::\ayAdded: \aw%s (%s)", vMemmedSpells[nGem].SpellName, vMemmedSpells[nGem].SpellCat);
							else
								WriteChatf("\arMQ2Bot\aw::\amDetected: \aw%s (%s)", vMemmedSpells[nGem].SpellName, vMemmedSpells[nGem].SpellCat);
							int vSize = vMaster.size();
							for (int i = 0; i < vSize; i++)
							{
								if (vMaster[i].Spell == vMemmedSpells[nGem].Spell)
								{
									WriteChatf("\arMQ2Bot\aw::\amKnown: \aw%s (%s)", vMemmedSpells[nGem].SpellName, vMemmedSpells[nGem].SpellCat);
									vMaster[i] = vMemmedSpells[nGem];
									return;
								}
							}
							if (vMemmedSpells[nGem].SpellTypeOption != ::OPTIONS::ZERO)
								vMaster.push_back(vMemmedSpells[nGem]);
						}
					}
				}
				else
				{
					if (vMemmedSpells[nGem].ID)
					{
						int vSize = vMaster.size();
						for (int i = 0; i < vSize; i++)
						{
							if (vMaster[i].ID == vMemmedSpells[nGem].ID)
							{
								WriteChatf("\arMQ2Bot\aw::\aoRemoved: \aw(%s)", vMemmedSpells[nGem].SpellName);
								vMaster.erase(vMaster.begin() + i);
								BotSpell spell;
								vMemmedSpells[nGem] = spell;
								vMemmedSpells[nGem].ID = 0;
								change = true;
								break;
							}
						}
					}
				}
			}
		}
		if (change) {
			SortSpellVector(vMaster);
		}
	}
	catch (...)
	{
		DebugSpewAlways("MQ2Bot::CheckMemmedSpells() **Exception**");
	}
}

void DiscCategory(PSPELL pSpell)
{
	strcpy_s(spellCat, "Unknown");
	if (!pSpell)
		return;
	if (DWORD cat = pSpell->Subcategory) {
		if (PVOID ptr = pCDBStr->GetString(cat, 5, NULL)) {
			sprintf_s(spellCat, "%s", pCDBStr->GetString(cat, 5, NULL));
		}
	}

	int attrib0 = GetSpellAttribX(pSpell, 0);

	if (pSpell->Subcategory == 152)
	{
		strcpy_s(spellCat, "Aggro");
		return;
	}
	if (attrib0 == 189)
	{
		strcpy_s(spellCat, "End Regen");
		return;
	}
	if (attrib0 == 31 || pSpell->Subcategory == 35 || pSpell->Category == 35)
	{
		strcpy_s(spellCat, "Mez");
		return;
	}
	if (attrib0 == 86)
	{
		strcpy_s(spellCat, "Pacify");
		return;
	}
	if (attrib0 == 180)
	{
		strcpy_s(spellCat, "Spell Block");
		return;
	}
	if (!strcmp(szSkills[pSpell->Skill], "Throwing"))
	{
		strcpy_s(spellCat, "Throwing");
		return;
	}
	if (strstr(pSpell->Name, "s Aura"))
	{
		strcpy_s(spellCat, "Aura");
		return;
	}
	if (HasSpellAttrib(pSpell, 220) || strstr(pSpell->Name, "Dichotomic"))
	{
		strcpy_s(spellCat, "Offense");
		return;
	}
	if ((attrib0 == 0 && GetSpellBase(pSpell, 0) < 0) || attrib0 == 300 || attrib0 == 200)
	{
		strcpy_s(spellCat, "Offense");
		return;
	}
	for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot)
	{
		int attrib = GetSpellAttrib(pSpell, slot);
		if (attrib == 169 || attrib == 171 || attrib == 182 || attrib == 183 || attrib == 184
			|| attrib == 185 || attrib == 186 || attrib == 193 || attrib == 196)
		{
			strcpy_s(spellCat, "Offense");
			return;
		}

		if (attrib == 162 || attrib == 168 || attrib == 172 || attrib == 174
			|| attrib == 175 || attrib == 188 || attrib == 320)
		{
			strcpy_s(spellCat, "Defense");
			return;
		}
	}
	if (attrib0 == 147)
	{
		strcpy_s(spellCat, "OOC Heal");
		return;
	}
	if (attrib0 == 181)
	{
		strcpy_s(spellCat, "Fearless");
		return;
	}
	if (attrib0 == 227 && GetSpellBase2(pSpell, 0) == 8)
	{
		strcpy_s(spellCat, "Offense");
		return;
	}
	if (attrib0 == 340 || attrib0 == 374 || attrib0 == 323)
	{
		if (PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, 0)))
		{
			if (GetSpellAttribX(pSpell2, 0) == 328 || HasSpellAttrib(pSpell, 162))
			{
				strcpy_s(spellCat, "Defense");
				return;
			}
			if (HasSpellAttrib(pSpell2, 85))
			{
				strcpy_s(spellCat, "Combat Innates");
				return;
			}
			if (HasSpellAttrib(pSpell2, 184))
			{
				strcpy_s(spellCat, "Offense");
				return;
			}
		}
	}
	if (attrib0 == 374 || attrib0 == 85)
	{
		if (PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, 0)))
		{
			int attrib0_ = GetSpellAttribX(pSpell2, 0);

			if (attrib0_ == 444)
			{
				strcpy_s(spellCat, "Aggro");
				return;
			}
			if (HasSpellAttrib(pSpell2, 192))
			{
				strcpy_s(spellCat, "Aggro");
				return;
			}
			if (attrib0_ == 92 && GetSpellBase(pSpell2, 0) < 0)
			{
				strcpy_s(spellCat, "Jolt");
				return;
			}
		}
	}
	if (HasSpellAttrib(pSpell, 192))
	{
		strcpy_s(spellCat, "Aggro");
		return;
	}
	if (strstr(pSpell->Name, "Unholy Aura"))
	{
		strcpy_s(spellCat, "Lifetap");
		return;
	}
}

bool FindSpell(PSPELL pSpell, vector<_BotSpell> &v)
{
	int vSize = v.size();
	for (int i = 0; i < vSize; i++)
	{
		if (v[i].Spell == pSpell)
			return true;
	}
	return false;
}

int FindSpellInfoForAttrib(PSPELL pSpell, ATTRIBSLOTS what, int attrib)
{
	int slot = FindSpellAttrib(pSpell, attrib);

	if (slot == -1)
		return 0;

	switch (what)
	{
	case ATTRIB:
		return slot;

	case BASE:
		return GetSpellBase(pSpell, slot);
	case BASE2:
		return GetSpellBase2(pSpell, slot);
	case CALC:
		return GetSpellCalc(pSpell, slot);
	case MAX:
		return GetSpellMax(pSpell, slot);
	}

	return 0;
}

DWORD GetSpellDuration2(PSPELL pSpell)
{
	if (!InGameOK())
		return 0;
	if (!pSpell)
		return 0;
	int pSpellDuration = 0;
	pSpellDuration = GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer);
	for (int i = 0; i < GetSpellNumEffects(pSpell); i++)
	{
		int attrib = GetSpellAttrib(pSpell, i);

		if (attrib == 323 || attrib == 374 || attrib == 419 || attrib == 339 || attrib == 85 || attrib == 340)
		{
			PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, i));
			int duration = pSpell2 ? GetSpellDuration(pSpell2, (PSPAWNINFO)pLocalPlayer) : 0;

			if (duration > pSpellDuration)
				pSpellDuration = duration;
		}
	}
	return pSpellDuration;
}

bool ShouldICast(_BotSpell &spell)
{
	if (spell.If[0] != '\0')
	{
		char sSpell[MAX_STRING] = { 0 };
		sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
		ParseMacroData(sSpell, MAX_STRING);
		if (atoi(sSpell) == 0)
			return false;
	}
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(spell.TargetID))
	{
		if (spell.NamedOnly && !IsNamed(pSpawn) || PctHP(pSpawn) > AssistAt || PctHP(pSpawn) > spell.StartAt || PctHP(pSpawn) <= spell.StopAt || spell.UseOnce && spell.LastTargetID == pSpawn->SpawnID)
			return false;
		if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
			return false;
	}
	return true;
}

bool ShouldICastDetrimental(_BotSpell &spell)
{
	if (!vAdds.size())
		return false;
	if (!GetSpawnByID(vAdds[0].ID))
		return false;
	if (GetDistance(vAdds[0].Spawn->X, vAdds[0].Spawn->Y) > AssistRange)
		return false;
	return true;
}

bool sortByPriority(const BotSpell &lhs, const BotSpell &rhs) { return lhs.Priority > rhs.Priority; } // Sort spell vectors by priority

void SortSpellVector(vector<_BotSpell> &v)  // the actual sorting of a spell vector by priority
{
	sort(v.begin(), v.end(), sortByPriority);
}

void SpellCategory(PSPELL pSpell)
{
	strcpy_s(spellCat, "Unknown");
	if (!pSpell)
		return;
	if (DWORD cat = pSpell->Subcategory) {
		if (PVOID ptr = pCDBStr->GetString(cat, 5, NULL)) {
			sprintf_s(spellCat, "%s", pCDBStr->GetString(cat, 5, NULL));
		}
	}
	if (pSpell->Subcategory == 81 && strstr(pSpell->Name, "Mal") || pSpell->SpellIcon == 55)
	{
		strcpy_s(spellCat, "Malo");
		return;
	}
	if ((pSpell->Subcategory == 81 || pSpell->SpellIcon == 72) && strstr(pSpell->Name, "Tash"))
	{
		strcpy_s(spellCat, "Tash");
		return;
	}
	if (pSpell->SpellIcon == 17 || strstr(pSpell->Name, "Helix of the Undying") || strstr(pSpell->Name, "Deep Sleep's Malaise"))
	{
		strcpy_s(spellCat, "Slow");
		return;
	}
	if (pSpell->SpellIcon == 5 || pSpell->SpellIcon == 160 && strstr(pSpell->Name, "Darkness"))
	{
		strcpy_s(spellCat, "Snare");
		return;
	}
	if (pSpell->SpellIcon == 117)
	{
		strcpy_s(spellCat, "Root");
		return;
	}
	if (pSpell->Subcategory == 35 || pSpell->SpellIcon == 25)
	{
		strcpy_s(spellCat, "Mez");
		return;
	}
	if (pSpell->Subcategory == 30 || pSpell->SpellIcon == 50)
	{
		strcpy_s(spellCat, "Cripple");
		return;
	}
	if (pSpell->SpellIcon == 134 || pSpell->SpellIcon == 16)
	{
		strcpy_s(spellCat, "Haste");
		return;
	}
	if (pSpell->SpellIcon == 132 && !strstr(pSpell->Name, "Growth"))
	{
		strcpy_s(spellCat, "Aego");
		return;
	}
	if (pSpell->SpellIcon == 131 && pSpell->Subcategory == 46)
	{
		strcpy_s(spellCat, "Skin");
		return;
	}
	if (pSpell->SpellIcon == 130 || pSpell->SpellIcon == 133 && strstr(pSpell->Name, "Shield of") || pSpell->SpellIcon == 133 && strstr(pSpell->Name, "Armor of the"))
	{
		strcpy_s(spellCat, "Focus");
		return;
	}
	if (pSpell->SpellIcon == 118 && pSpell->Subcategory == 43)
	{
		strcpy_s(spellCat, "Regen");
		return;
	}
	if (pSpell->SpellIcon == 150 && pSpell->Subcategory == 112 && pSpell->Category == 45)
	{
		strcpy_s(spellCat, "Symbol");
		return;
	}
	if (pSpell->SpellIcon == 21 && !strstr(pSpell->Name, "Knowledge") && !strstr(pSpell->Name, "Recourse") && !strstr(pSpell->Name, "Soothing") && !strstr(pSpell->Name, "Brell") && pSpell->Subcategory == 59 && pSpell->Category == 79)
	{
		strcpy_s(spellCat, "Clarity");
		return;
	}
	if (pSpell->SpellIcon == 123 && strstr(pSpell->Name, "of the Predator") || pSpell->SpellIcon == 158 && strstr(pSpell->Name, "Protection of the"))
	{
		strcpy_s(spellCat, "Pred");
		return;
	}
	if (pSpell->SpellIcon == 123 && strstr(pSpell->Name, "Strength of the") || pSpell->SpellIcon == 158 && strstr(pSpell->Name, "Protection of the"))
	{
		strcpy_s(spellCat, "Strength");
		return;
	}
	if (pSpell->SpellIcon == 90 && strstr(pSpell->Name, "Brell's"))
	{
		strcpy_s(spellCat, "Brells");
		return;
	}
	if (pSpell->SpellIcon == 1 && strstr(pSpell->Name, "Spiritual V"))
	{
		strcpy_s(spellCat, "SV");
		return;
	}
	if (pSpell->SpellIcon == 72 && strstr(pSpell->Name, "Spiritual E"))
	{
		strcpy_s(spellCat, "SE");
		return;
	}
	if (pSpell->SpellIcon == 132 && (strstr(pSpell->Name, "Growth") || strstr(pSpell->Name, "Stance")))
	{
		strcpy_s(spellCat, "Growth");
		return;
	}
	if (pSpell->SpellIcon == 70 && strstr(pSpell->Name, "Shining"))
	{
		strcpy_s(spellCat, "Shining");
		return;
	}
	if (pSpell->SpellType != 0 && pSpell->Category == 125 && pSpell->Subcategory == 21 && pSpell->TargetType != 6)
	{
		for (long x = 0; x < GetSpellNumEffects(pSpell); x++)
		{
			if (GetSpellAttrib(pSpell, x) == 59)
			{
				sprintf_s(spellCat, "DS%d", x + 1);
				return;
			}
		}
	}
	if (HasSpellAttrib(pSpell, 85) && (strstr(pSpell->Name, "Talisman of the ") || strstr(pSpell->Name, "Spirit of the ")) && pSpell->SpellIcon == 2)
	{
		strcpy_s(spellCat, "Panther");
		return;
	}
	if (strstr(pSpell->Name, "Ferocity") && pSpell->SpellIcon == 81)
	{
		strcpy_s(spellCat, "Fero");
		return;
	}
	if (strstr(pSpell->Name, "Retort") && pSpell->SpellIcon == 2)
	{
		strcpy_s(spellCat, "Retort");
		return;
	}
	if (pSpell->Category == 42 && pSpell->Subcategory == 32)
	{
		strcpy_s(spellCat, "HoT");
		return;
	}
}

void SpellType(PSPELL pSpell)
{
	if (!InGameOK())
		return;
	if (!pSpell)
		return;
	strcpy_s(spellType, "Unknown");
	spellTypeInt = ZERO;
	if ((GetSpellAttribX(pSpell, 0) == 192 && GetSpellBase(pSpell, 0) > 0) || (GetSpellAttribX(pSpell, 4) == 192 && GetSpellBase(pSpell, 4) > 0)) {
		strcpy_s(spellType, "Aggro"); spellTypeInt = ::OPTIONS::AGGRO;  return;
	}
	if (pSpell->Category == 132 || pSpell->Subcategory == 132 || pSpell->Category == 71) {
		strcpy_s(spellType, "Aura"); spellTypeInt = ::OPTIONS::AURA; return;
	}
	if (pSpell->Category == 13 || pSpell->Subcategory == 13) {
		strcpy_s(spellType, "Charm"); spellTypeInt = ::OPTIONS::CHARM;  return;
	}
	if (strstr(pSpell->Name, " Counterbias") || (pSpell->Subcategory == 21 && pSpell->TargetType == 0 || pSpell->Subcategory == 30 || pSpell->Subcategory == 88 || pSpell->Subcategory == 81) && pSpell->Category != 132) {
		strcpy_s(spellType, "Debuff"); spellTypeInt = ::OPTIONS::DEBUFF; return;
	}
	if (strstr(pSpell->Name, "Dichotomic Paroxysm") || pSpell->Category == 126 && pSpell->Subcategory == 60 || pSpell->Category == 20 || pSpell->Category == 114 && pSpell->Subcategory == 33 || pSpell->Category == 114 && pSpell->Subcategory == 43 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 0) {
		strcpy_s(spellType, "Dot"); spellTypeInt = ::OPTIONS::DOT; return;
	}
	if (strstr(pSpell->Name, "Growl of the") || strstr(pSpell->Name, "Grim Aura") || strstr(pSpell->Name, "Roar of the Lion") || strstr(pSpell->Name, "Chaotic Benefaction") ||
		strstr(pSpell->Name, "Dichotomic Companion") || strstr(pSpell->Name, "Dichotomic Fury") || HasSpellAttrib(pSpell, 323) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61 ||
		pSpell->Category == 45 && pSpell->Subcategory == 141 && pSpell->TargetType == 6 || pSpell->Subcategory == 142 && pSpell->TargetType == 6 && pSpell->Category != 132 ||
		pSpell->Category == 125 && pSpell->Subcategory == 16 && (pSpell->TargetType == 6 || pSpell->TargetType == 41) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61) {
		strcpy_s(spellType, "FightBuff"); spellTypeInt = ::OPTIONS::FIGHTBUFF; return;
	}
	if (pSpell->Category == 42 && (pSpell->Subcategory != 19 || strstr(pSpell->Name, "Balm") || strstr(pSpell->Name, "Splash of")) && pSpell->Subcategory != 82 && (pSpell->TargetType == 45 || pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 6 || pSpell->TargetType == 8 || pSpell->TargetType == 41)) {
		strcpy_s(spellType, "Heal"); spellTypeInt = ::OPTIONS::HEAL; return;
	}
	//cure has to be after heal else fail
	if (pSpell->Category == 42 && pSpell->Subcategory == 19) {
		strcpy_s(spellType, "Cure"); spellTypeInt = ::OPTIONS::CURE; return;
	}
	if (pSpell->Subcategory == 42 && pSpell->Category == 69 || pSpell->Category == 42 && (pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 8)) {
		strcpy_s(spellType, "HealPet"); spellTypeInt = ::OPTIONS::HEALPET; return;
	}
	if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0) {
		strcpy_s(spellType, "HealToT"); spellTypeInt = ::OPTIONS::HEALTOT; return;
	}
	if (pSpell->Category == 5 && (GetSpellBaseX(pSpell, 0) < 0) || strstr(pSpell->Name, "freeze")) {
		PSPELL pSpell2 = GetSpellByID(GetSpellBase2X(pSpell, 1));
		if (pSpell2 && ((GetSpellAttribX(pSpell2, 2) == 192) || strstr(pSpell->Name, "Concussive ")) && GetCharInfo()->pSpawn->mActorClient.Class == Wizard) {
			strcpy_s(spellType, "Jolt");
			spellTypeInt = ::OPTIONS::JOLT;
			return;
		}
	}
	if (strstr(pSpell->Name, "Dichotomic Fang") || pSpell->Category == 114 && (pSpell->Subcategory == 43 || pSpell->Subcategory == 33)) {
		strcpy_s(spellType, "Lifetap"); spellTypeInt = ::OPTIONS::LIFETAP; return;
	}
	if (pSpell->Subcategory == 21 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 51 || pSpell->Subcategory == 16 && pSpell->Category != 132 || pSpell->Subcategory == 62 || pSpell->Subcategory == 64 && strstr(pSpell->Name, " Retort") || pSpell->Category == 45 && pSpell->Subcategory == 141 || pSpell->Subcategory == 64 && strstr(pSpell->Name, "Divine I")) {
		strcpy_s(spellType, "MainTankBuff"); spellTypeInt = ::OPTIONS::MAINTANKBUFF;  return;
	}
	if (pSpell->Subcategory == 61 || pSpell->Subcategory == 17 && FindSpellInfoForAttrib(pSpell, BASE, 15) > 0 || strstr(pSpell->Name, " Symbiosis")) {
		strcpy_s(spellType, "Mana"); spellTypeInt = ::OPTIONS::MANA; return;
	}
	if (pSpell->Subcategory == 35) {
		strcpy_s(spellType, "Mez"); spellTypeInt = ::OPTIONS::MEZ; return;
	}
	if (strstr(pSpell->Name, "Dichotomic Fusillade") || strstr(pSpell->Name, "Dichotomic Force") || strstr(pSpell->Name, "Dichotomic Fire") || pSpell->Category == 25 && pSpell->Subcategory != 35) {
		strcpy_s(spellType, "Nuke");		spellTypeInt = NUKE; return;
	}
	if (pSpell && pSpell->Autocast == 19780) {
		if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0) {
			strcpy_s(spellType, "NukeToT");
			spellTypeInt = ::OPTIONS::NUKETOT;
			return;
		}
		else if (pSpell->TargetType == 5) {
			for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot) {
				if (GetSpellAttrib(pSpell, slot) == 374) {
					PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, slot));
					if (pSpell2 && GetSpellAttribX(pSpell2, 0) == 0 && GetSpellBase(pSpell2, 0) > 0 && pSpell2->TargetType == 46) {
						strcpy_s(spellType, "NukeToT");
						spellTypeInt = ::OPTIONS::NUKETOT;
						return;
					}
				}
			}
		}
	}
	if (pSpell->Category == 69 && pSpell->Subcategory > 97 && pSpell->Subcategory < 106 && pSpell->Subcategory != 101 && GetCharInfo()->pSpawn->mActorClient.Class != 12 && GetCharInfo()->pSpawn->mActorClient.Class != 2 && GetCharInfo()->pSpawn->mActorClient.Class != 6) {
		strcpy_s(spellType, "Pet");		spellTypeInt = ::OPTIONS::PET; return;
	}
	if (pSpell->Subcategory == 82) {
		strcpy_s(spellType, "Rez");		spellTypeInt = ::OPTIONS::REZ; return;
	}
	if (pSpell->Subcategory == 83) {
		strcpy_s(spellType, "Root");		spellTypeInt = ::OPTIONS::ROOT; return;
	}
	// skipping SelfBuff for now
	if (pSpell->Subcategory == 89) {
		strcpy_s(spellType, "Snare");		spellTypeInt = ::OPTIONS::SNARE; return;
	}
	if (GetSpellAttribX(pSpell, 0) == 32) {
		spellTypeInt = AGGRO;
		strcpy_s(spellType, "SummonItem");	spellTypeInt = ::OPTIONS::SUMMONITEM; return;
	}
	if (pSpell->Subcategory == 104 && pSpell->Category == 69 && strstr(pSpell->Extra, "Rk") || pSpell->Subcategory == 99 && (GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 2) || pSpell->Subcategory == 139) {
		strcpy_s(spellType, "Swarm");		spellTypeInt = ::OPTIONS::SWARM; return;
	}
}

bool ValidBen(PSPELL pSpell, PSPAWNINFO pSpawn)
{
	if (!InGameOK())
		return false;
	if (!pSpawn)
		return false;
	if (!pSpell)
		return false;
	bool valid = false;
	if (pSpell->ZoneType == 1 && !outdoor)
		return false;
	if (pSpell->SpellType != 0)
		if (((int)pSpell->ManaCost < GetCharInfo()->pSpawn->ManaCurrent  &&  GetCharInfo()->pSpawn->ManaMax>0 || pSpell->ManaCost < 1) && (DistanceToSpawn3D((PSPAWNINFO)pCharSpawn, pSpawn) < pSpell->Range || DistanceToSpawn3D((PSPAWNINFO)pCharSpawn, pSpawn) < pSpell->AERange))
			valid = true;
	return valid;
}

bool ValidDet(PSPELL pSpell, PSPAWNINFO Target)
{
	if (!InGameOK())
		return false;
	if (!Target || Target->Type == SPAWN_CORPSE)
		return false;
	if (!pSpell)
		return false;
	if (pSpell->ZoneType == 1 && !outdoor)
		return false;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(Target->MasterID))
		if (pSpawn->Type == SPAWN_PLAYER && !strstr(EQADDR_SERVERNAME, "Zek"))
			return false;
	char bodyType[MAX_STRING] = "", testBody[MAX_STRING] = "Single";
	bool validDet = true;
	sprintf_s(bodyType, "${If[(${Spell[%s].TargetType.Find[%s]}||!${Select[${Spell[%s].TargetType},Animal,Humanoid,Undead,Plant,Summoned,Uber Giants,Uber Dragons,AE Undead,AE Summoned]} && !${Spell[%s].Subcategory.Find[summoned]}||${Spell[%s].TargetType.Find[Undead]} && ${Target.Buff[smoldering bones].ID}||${Spell[%s].Subcategory.Find[summoned]} && ${Select[%d,5,24,27,28]}),1,0]}", pSpell->Name, BodyTypeFix, pSpell->Name, pSpell->Name, pSpell->Name, pSpell->Name, GetBodyType(Target));
	ParseMacroData(bodyType, MAX_STRING);
	if (atoi(bodyType) == 0 || !outdoor && pSpell->ZoneType == 1)
	{
		validDet = false;
		return validDet;
	}
	if (pSpell->TargetType == 5 && !(CastRay(GetCharInfo()->pSpawn, Target->Y, Target->X, Target->Z)) || (int)pSpell->ManaCost > GetCharInfo()->pSpawn->ManaCurrent && GetCharInfo()->pSpawn->ManaMax > 0 || DistanceToSpawn3D(GetCharInfo()->pSpawn, Target) > pSpell->Range && pSpell->Range > 0 || DistanceToSpawn3D(GetCharInfo()->pSpawn, Target) > pSpell->AERange && pSpell->Range == 0)
		validDet = false;
	return validDet;
}
#pragma endregion SpellFunctionDefinitions

#pragma region CreateRoutines
void CreateAA()
{
	if (!InGameOK())
		return;
	vTemp.clear();
	strcpy_s(CurrentRoutine, &(__FUNCTION__[6]));
	PCHAR szAA[] = { "Frenzied Devastation", "Prolonged Destruction", "Improved Sustained Destruction", "Arcane Destruction", "Fury of Kerfyrm", "Fury of Ro", "Sustained Devastation",
		"Calculated Insanity", "Chromatic Haze", "Improved Reactive Rune", "Reactive Rune", "Illusions of Grandeur", "Improved Twincast",
		"Sustained Destruction", "Frenzied Burnout", "Virulent Talon", "Fire Core", "Elemental Union", "Visage of Death", "Gift of the Quick Spear",
		"Heart of Flames", "Heart of Vapor", "Heart of Ice", "Heart of Stone", "Fury of Eci", "Fury of Druzzil",
		"Valorous Rage", "Rabid Bear", "Group Spirit of the Black Wolf", "Nature's Blessing", "Group Spirit of the White Wolf", "Spirit of the Wood",
		"Divine Avatar", "Celestial Rapidity", "Silent Casting", "Mercurial Torment", "Funeral Pyre", "Gift of Deathly Resolve",
		"Group Guardian of the Forest", "Outrider's Accuracy", "Outrider's Attack", "Auspice of the Hunter", "Guardian of the Forest",
		"Outrider's Evasion", "Imperator's Command", "Wars Sheol's Heroic Blade", "Healing Frenzy", "Flurry of Life", "Roar of Thunder",
		"Frenzied Swipes", "Chameleon Strike", "Frenzy of Spirit", "Group Bestial Alignment", "Bestial Bloodrage", "Zan Fi's Whistle",
		"Infusion of Thunder", "Hand of Death", "Embalmer's Carapace", "Bestial Alignment", "Ragged Bite of Agony", "Group Armor of the Inquisitor",
		"Inquisitor's Judgement", "Armor of the Inquisitor", "Imbued Ferocity", "Soul Flay", "Quick Time", "Fierce Eye",
		"Selo's Kick", "Bladed Song", "Arcane Fury", "Inquisitor's Judgment", "Dance of Blades", "Channeling the Divine", "Life Burn",
		"A Tune Stuck in Your Head", "Feral Swipe", "Vehement Rage", "Distraction Attack", "Juggernaut Surge", "Reckless Abandon", "Savage Spirit",
		"Untamed Rage", "War Cry of the Braxi", "Five Point Palm", "Battle Stomp", "Blinding Fury", "Desperation", "Stunning Kick",
		"Two-Finger Wasp Touch", "Bloodlust", "Cascading Rage", "6492", "Hand of Tunare", "Thunder of Karana", "Destructive Vortex",
		"Nature's Fury", "Elemental Arrow", "Reinforced Malaise", "T'Vyl's Resolve", "Flurry of Notes", "Frenzied Kicks", "Rake's Rampage",
		"War Stomp", "Lyrical Prankster", "Enduring Frenzy", "Rallying Call", "Fleeting Spirit",
		"Fundament: First Spire of Ancestors", "Fundament: First Spire of Arcanum", "Fundament: First Spire of Divinity",
		"Fundament: Second Spire of the Elements", "Fundament: First Spire of Enchantment",
		"Fundament: Third Spire of Holiness", "Fundament: First Spire of Nature", "Fundament: Second Spire of Necromancy",
		"Fundament: First Spire of Savagery", "Fundament: First Spire of the Minstrels",
		"Fundament: Second Spire of the Pathfinders", "Fundament: First Spire of the Rake",
		"Fundament: Third Spire of the Reavers", "Fundament: First Spire of the Savage Lord",
		"Fundament: First Spire of the Sensei", "Fundament: First Spire of the Warlord", "Fundament: Second Spire of Ancestors",
		"Fundament: Third Spire of Arcanum", "Fundament: Second Spire of Divinity", "Fundament: Third Spire of the Elements",
		"Fundament: Third Spire of Enchantment", "Fundament: Second Spire of Holiness", "Fundament: Second Spire of Nature",
		"Fundament: First Spire of Necromancy", "Fundament: Second Spire of Savagery", "Fundament: Third Spire of the Minstrels",
		"Fundament: First Spire of the Pathfinders", "Fundament: Third Spire of the Rake", "Fundament: First Spire of the Reavers",
		"Fundament: Second Spire of the Savage Lord", "Fundament: Third Spire of the Sensei",
		"Fundament: Second Spire of the Warlord", "Fundament: Third Spire of Ancestors", "Fundament: Second Spire of Arcanum",
		"Fundament: Third Spire of Divinity", "Fundament: First Spire of the Elements", "Fundament: Second Spire of Enchantment",
		"Fundament: First Spire of Holiness", "Fundament: Third Spire of Nature", "Fundament: Third Spire of Necromancy",
		"Fundament: Third Spire of Savagery", "Fundament: Second Spire of the Minstrels",
		"Fundament: Third Spire of the Pathfinders", "Fundament: Second Spire of the Rake",
		"Fundament: Second Spire of the Reavers", "Fundament: Third Spire of the Savage Lord",
		"Fundament: Second Spire of the Sensei", "Fundament: Third Spire of the Warlord",
		NULL };
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	int aaIndex;
	PALTABILITY aa;
	int customSpells = GetPrivateProfileInt(INISection, "SpellTotal", 0, INIFileName);
	for (int i = 0; i < customSpells; i++)
	{
		sprintf_s(szSpell, "Spell%dName", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpell spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					if (GetCharInfo()->pSpawn->mActorClient.Class == 3) // one off shit that is misnamed.  should probably double check to see if this is fixed now 20160717.
						if (strstr(szTemp, "Inquisitor's Judg"))
							strcpy_s(szTemp, "Inquisitor's Judgement");
					strcpy_s(spell.SpellName, szTemp);
					spell.CanIReprioritize = 0; // i dont think i need this variable any more
					spell.ID = aa->ID;
					spell.SpellTypeOption = ::OPTIONS::AA;
					spell.CheckFunc = CheckAA;
					strcpy_s(spell.Gem, "alt");
					vTemp.push_back(spell);
				}
			}
		}
	}
	int test = 0;
	for (int i = 0; szAA[i]; i++)
	{
		test = 0;
		int vSize = vTemp.size();
		for (int z = 0; z < vSize; z++)
		{
			if (!_stricmp(vTemp[z].SpellName, szAA[i]))
				test = 1;
		}
		if (!test)
		{
			strcpy_s(szTemp, szAA[i]);
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID) && (int)aa->ReuseTimer > 0)
				{
					BotSpell spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					strcpy_s(spell.SpellName, szTemp);
					spell.CanIReprioritize = 1;
					spell.Type = TYPE_AA;
					spell.ID = aa->ID;
					spell.SpellTypeOption = ::OPTIONS::AA;
					spell.CheckFunc = CheckAA;
					strcpy_s(spell.Gem, "alt");
					vTemp.push_back(spell);
				}
			}
		}
	}
	// load the rest of the AA info
	LoadBotSpell(vTemp, "AA");
	int vSize = vTemp.size();
	for (int i = 0; i < vSize; i++)
	{
		vMaster.push_back(vTemp[i]);
	}
}
void CreateDisc()
{
	if (!InGameOK())
		return;
	vTemp.clear();
	// discs are special due to timer ID and how this detects, I really dont want to screw with how they are ordered, even in a custom ini scenario.
	vector<PSPELL> tempSpell;
	vector<int> tempLevel;
	vector<int> tempTimer;
	vector<int> noOverwrite;
	for (int i = 0; i < 21; i++)
	{
		tempSpell.push_back(NULL);
		tempLevel.push_back(0);
		tempTimer.push_back(i);
		noOverwrite.push_back(0);
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING] = { 0 }, szSpellGem[MAX_STRING] = { 0 }, szTempGem[MAX_STRING] = { 0 };
	int customSpells = GetPrivateProfileInt(INISection, "SpellTotal", 0, INIFileName);
	for (int i = 0; i < customSpells; i++)
	{
		sprintf_s(szSpell, "Spell%dName", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			sprintf_s(szSpellGem, "Spell%dGem", i);
			GetPrivateProfileString(INISection, szSpellGem, NULL, szTempGem, MAX_STRING, INIFileName);
			_strlwr_s(szTempGem);
			if (strstr(szTempGem, "disc"))
			{
				for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
				{
					if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
						if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
						{
							if (strstr(pSpell->Name, szTemp))
							{
								tempSpell[pSpell->ReuseTimerIndex] = pSpell;
								tempLevel[pSpell->ReuseTimerIndex] = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
								noOverwrite[pSpell->ReuseTimerIndex] = 1;
							}
						}
					}
				}
			}
		}
	}
	for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
	{
		if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
			if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
			{
				DiscCategory(pSpell);
				if (!strstr(spellCat, "Summon Weapon") && !strstr(pSpell->Name, "Thief's ") && !strstr(pSpell->Name, "Tireless Sprint") && !strstr(pSpell->Name, "Conditioned Reflexes") && (pSpell->ReuseTimerIndex != 8 || GetCharInfo()->pSpawn->mActorClient.Class != 16 || strstr(pSpell->Name, "Cry")))
				{
					if ((long)pSpell->ReuseTimerIndex != -1 && pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > tempLevel[pSpell->ReuseTimerIndex] && !noOverwrite[pSpell->ReuseTimerIndex])
					{
						tempSpell[pSpell->ReuseTimerIndex] = pSpell;
						tempLevel[pSpell->ReuseTimerIndex] = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
						// adding in low level discs that are the best available
						if (strstr(pSpell->Name, "Twisted Chance") || strstr(pSpell->Name, "Frenzied Stabbing") || strstr(pSpell->Name, "Amplified Frenzy"))
							noOverwrite[pSpell->ReuseTimerIndex] = 1;
					}
					else if ((long)pSpell->ReuseTimerIndex == -1)
					{
						tempSpell.push_back(pSpell);
						tempLevel.push_back(pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]);
					}
				}
			}
		}
	}
	// ------------------finalize discs------------------------------
	// first check for if statement for section

	strcpy_s(szSpell, "DiscIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("DiscIf: %s", SpellIt->second.c_str());
	}
	// now create and check ini entries on discs
	int vSize = tempSpell.size();
	int lSize = tempLevel.size();
	for (int i = 0; i < vSize && i < lSize; i++)
	{
		if (tempLevel[i] > 0)
		{
			BotSpell disc;
			disc.Spell = tempSpell[i];
			strcpy_s(disc.SpellName, tempSpell[i]->Name);
			disc.Type = TYPE_DISC;
			disc.CheckFunc = CheckDisc;
			disc.SpellTypeOption = ::OPTIONS::DISC;
			disc.ID = tempSpell[i]->ID;
			disc.CanIReprioritize = 1;
			strcpy_s(disc.Gem, "disc");
			vTemp.push_back(disc);
		}

	}
	// load the rest of the disc info
	LoadBotSpell(vTemp, "Disc");
	// find the endurance regen disc
	int tSize = vTemp.size();
	for (int i = 0; i < tSize; i++)
	{
		if (strstr(vTemp[i].SpellCat, "End Regen") && !strstr(vTemp[i].SpellName, "Wind"))
		{
			EndRegenSpell = vTemp[i];
			WriteChatf("Endurance Regen Disc: %s", vTemp[i].SpellName);
		}
	}
	int sSize = vTemp.size();
	for (int i = 0; i < sSize; i++)
	{
		vMaster.push_back(vTemp[i]);
	}
}
void CreateGroup()
{
	vGroup.clear();
	_Spawns group;
	group.ID = 0;
	for (int i = 0; i < 6; i++)
	{
		vGroup.push_back(group);
	}
}
void CreateHeal()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vTemp.clear();
	strcpy_s(CurrentRoutine, &(__FUNCTION__[6]));
	PCHAR szHeal[] = { "Divine Arbitration", "Burst of Life", "Beacon of Life", "Focused Celestial Regeneration", "Celestial Regeneration",
		"Convergence of Spirits", "Union of Spirits", "Focused Paragon of Spirits", "Paragon of Spirit", "Lay on Hands",
		"Hand of Piety", "Ancestral Aid", "Call of the Ancients", "Exquisite Benediction", "Blessing of Tunare", NULL };
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING] = { 0 }, gem[MAX_STRING] = { 0 };
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	int customSpells = GetPrivateProfileInt(INISection, "SpellTotal", 0, INIFileName);
	for (int i = 0; i < customSpells; i++)
	{
		sprintf_s(szSpell, "SpellName%d", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpell spell;
					spell.IniMatch = i;
					spell.Spell = GetSpellByID(aa->SpellID);
					strcpy_s(spell.SpellName, szTemp);
					spell.CanIReprioritize = 0;
					spell.Type = TYPE_AA;
					spell.CheckFunc = CheckHeal;
					spell.SpellTypeOption = ::OPTIONS::HEAL;
					spell.ID = aa->ID;
					strcpy_s(gem, "alt");
					strcpy_s(spell.Gem, gem);
					vTemp.push_back(spell);
				}
			}
			else
				for (int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				{
					if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
					{
						if (!_stricmp(szTemp, pSpell->Name))
						{
							BotSpell spell;
							spell.IniMatch = i;
							spell.Spell = pSpell;
							strcpy_s(spell.SpellName, szTemp);
							spell.CanIReprioritize = 0;
							spell.ID = pSpell->ID;
							spell.CheckFunc = CheckHeal;
							spell.SpellTypeOption = ::OPTIONS::HEAL;
							spell.Type = TYPE_SPELL;
							sprintf_s(gem, "%d", DefaultGem);
							strcpy_s(spell.Gem, gem);
							vTemp.push_back(spell);
						}
					}
				}
		}
	}
	int test = 0;
	for (int i = 0; szHeal[i]; i++)
	{
		test = 0;
		int vSize = vTemp.size();
		for (int z = 0; z < vSize; z++)
		{
			if (!_stricmp(vTemp[z].SpellName, szHeal[i]))
				test = 1;
		}
		if (!test)
		{
			strcpy_s(szTemp, szHeal[i]);
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpell spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					strcpy_s(spell.SpellName, szTemp);
					spell.ID = aa->ID;
					spell.Type = TYPE_AA;
					spell.CanIReprioritize = 1;
					strcpy_s(gem, "alt");
					spell.CheckFunc = CheckHeal;
					spell.SpellTypeOption = ::OPTIONS::HEAL;
					strcpy_s(spell.Gem, gem);
					vTemp.push_back(spell);
				}
			}
		}
	}
	LoadBotSpell(vTemp, "Heal");
	int vSize = vTemp.size();
	for (int i = 0; i < vSize; i++)
	{
		vMaster.push_back(vTemp[i]);
	}
}
#pragma endregion CreateRoutines

#pragma region CheckRoutines
// master check
void CheckMaster()
{
	strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
	CheckMemmedSpells();
	int vSize = vMaster.size();
	for (int i = 0; i < vSize; i++)
	{
		if (vMaster[i].CheckFunc != NULL)
			vMaster[i].CheckFunc(i);
	}
}
void CheckAA(int spell)
{
	if (!InGameOK())
		return;
	strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
	DebugWrite("Checking %s: Spell: %s -- Priority: %d", CurrentRoutine, vMaster[spell].SpellName, vMaster[spell].Priority); // test code
	if (vMaster[spell].Spell->SpellType == 0)
		if (!ShouldICastDetrimental(vMaster[spell]))
			return;
	if (vMaster[spell].Spell->SpellType != 0)
		if (SpellStacks(vMaster[spell].Spell) || GetCharInfo()->pSpawn->mActorClient.Class == 16 && strcmp(vMaster[spell].SpellName, "Savage Spirit"))
			return;
	// all the basic checks have cleared, now we need to test the one-offs

	// FixNote 20160728.2 - this check will need readded for AACutOffTime
	// if (valid && (atol(sTest) <= (AACutoffTime * 60) || named && GetSpawnByID((DWORD)atol(sNamed))))
	if (GetCharInfo()->pSpawn->mActorClient.Class == 16 && (!strcmp(vMaster[spell].SpellName, "Savage Spirit") || !strcmp(vMaster[spell].SpellName, "Untamed Rage")))
	{
		char sstest[MAX_STRING] = "";
		sprintf_s(sstest, "${If[${Me.ActiveDisc.ID},1,0]}");
		ParseMacroData(sstest, MAX_STRING);
		if (atoi(sstest) == 1)
			return;
	}
	BotCastCommand(vMaster[spell]);
}
void CheckAggro(int spell)
{
	return;
}
void CheckDisc(int spell)
{
	strcpy_s(CurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
	DebugWrite("Checking %s: Spell: %s -- Priority: %d", CurrentRoutine, vMaster[spell].SpellName, vMaster[spell].Priority); // test code
	return;
}
void CheckHeal(int spell)
{
	return;
}
#pragma endregion CheckRoutines

#pragma region Configure
//void Configure(char szCustomIni[MAX_STRING], int force)
void Configure(PCHAR szCustomIni, int force)
{
	if (!InGameOK())
		return;
	//save for later // int i;
	vMaster.clear();
	vMemmedSpells.clear();
	//if (vMemmedSpells.size() < 12)
	//{
	//	for (int x = 0; x < 11; x++)
	//	{
	//		BotSpell spell;
	//		spell.Spell = nullptr; //NULL
	//		spell.ID = 0;
	//		vMemmedSpells.push_back(spell);
	//	}
	//}
	vMemmedSpells.assign(NUM_SPELL_GEMS - 1, _BotSpell());
	DWORD Shrouded;
	char tempINI[MAX_STRING] = { 0 }, tempCustomINI[MAX_STRING] = { 0 };
	long Class = GetCharInfo()->pSpawn->mActorClient.Class;
	long Races = GetCharInfo2()->Race;
	long Level = GetCharInfo2()->Level;
	sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	sprintf_s(INISection, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
	Shrouded = GetCharInfo2()->Shrouded;
	if (!Shrouded)
		INISection[strlen(PLUGIN_NAME)] = 0;
	strcpy_s(tempINI, INISection);
	if (szCustomIni != nullptr) {
		_strlwr_s(szCustomIni, MAX_STRING);
		if (force && strlen(szCustomIni) > 1 && !strstr(szCustomIni, "default"))
			sprintf_s(INISection, "%s_%s", tempINI, szCustomIni);
	}
	char szTemp[MAX_STRING] = { 0 };
	DEBUG_DUMPFILE = 0 != GetPrivateProfileInt(INISection, "Debugging", 0, INIFileName);//log to file?
	WritePrivateProfileString(INISection, "Debugging", DEBUG_DUMPFILE ? "1" : "0", INIFileName);
	DefaultGem = GetPrivateProfileInt(INISection, "DefaultGem", 1, INIFileName); //default gem to cast with
}
#pragma endregion Configure

#pragma region Commands
void BotCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	Configure(NULL, 0);
	DebugWrite("BotCommand");
	DurationSetup();
	CreateGroup();
	CreateAA();
	CreateDisc();
	CreateHeal();

	CheckMemmedSpells();
	SortSpellVector(vMaster);
	ConfigureLoaded = true;
	if (strlen(szLine) != 0)
	{
		CHAR Arg1[MAX_STRING] = { 0 }, Arg2[MAX_STRING] = { 0 };
		GetArg(Arg1, szLine, 1);
		if (!_stricmp(Arg1, "populate"))
		{
			GetArg(Arg2, szLine, 2);
			if (strlen(Arg2) != 0)
			{
				if (!_stricmp(Arg2, "spell"))
				{
					PopulateIni(vMaster, "Spell");
				}
			}
		}
	}
}
void ListCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (!vMaster.size())
		return;
	int vSize = vMaster.size();
	for (int i = 0; i < vSize; i++)
	{
		WriteChatf("\aoSpell%d: \awName: %s%s, \awGem: %s, Priority: %d", i, vMaster[i].Color, vMaster[i].SpellName, vMaster[i].Gem, vMaster[i].Priority);
	}
}
void MemmedCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	WriteChatf("MemmedCommand::No more crashing please!");
	DebugSpewAlwaysFile("MemmedCommand::No more crashing please!");
	CheckMemmedSpells();
	//SortSpellVector(vMemmedSpells);
}
#pragma endregion Commands

#pragma region Loading
void LoadBotSpell(vector<_BotSpell> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING], color[10] = { 0 };
	int defStartAt = 0, defUse = 0, defStopAt = 0, defPriority = 0, defNamedOnly = 0, defUseOnce = 0, defForceCast = 0;
	for (int x = 0; DefaultSection[x]; x++)
	{
		if (!_stricmp(VectorName, DefaultSection[x]))
		{
			strcpy_s(color, DefaultColor[x]);
			defStartAt = atoi(DefaultStartAt[x]);
			defUse = atoi(DefaultUse[x]);
			defStopAt = atoi(DefaultStopAt[x]);
			defPriority = atoi(DefaultPriority[x]);
			defNamedOnly = atoi(DefaultNamedOnly[x]);
			defUseOnce = atoi(DefaultUseOnce[x]);
			defForceCast = atoi(DefaultForceCast[x]);
			break;
		}
	}
	int vSize = v.size();
	for (int i = 0; i < vSize; i++)
	{
		if (!_stricmp(VectorName, "Disc"))
			DiscCategory(v[i].Spell);
		else
			SpellType(v[i].Spell);
		strcpy_s(v[i].SpellCat, spellType);
		sprintf_s(szSpell, "SpellIf%d", v[i].IniMatch);
		if (GetPrivateProfileString(INISection, szSpell, "1", szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].If, szTemp);
		}
		sprintf_s(szSpell, "SpellGem%d", v[i].IniMatch);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].Gem, szTemp);
		}
		sprintf_s(szSpell, "SpellUseOnce%d", v[i].IniMatch);
		v[i].UseOnce = GetPrivateProfileInt(INISection, szSpell, defUseOnce, INIFileName);
		sprintf_s(szSpell, "SpellForceCast%d", v[i].IniMatch);
		v[i].ForceCast = GetPrivateProfileInt(INISection, szSpell, defForceCast, INIFileName);
		sprintf_s(szSpell, "SpellUse%d", v[i].IniMatch);
		v[i].Use = GetPrivateProfileInt(INISection, szSpell, defUse, INIFileName);
		sprintf_s(szSpell, "SpellStartAt%d", v[i].IniMatch);
		v[i].StartAt = GetPrivateProfileInt(INISection, szSpell, defStartAt, INIFileName);
		sprintf_s(szSpell, "SpellStopAt%d", v[i].IniMatch);
		v[i].StopAt = GetPrivateProfileInt(INISection, szSpell, defStopAt, INIFileName);
		sprintf_s(szSpell, "SpellNamedOnly%d", v[i].IniMatch);
		v[i].NamedOnly = GetPrivateProfileInt(INISection, szSpell, defNamedOnly, INIFileName);
		sprintf_s(szSpell, "SpellPriority%d", v[i].IniMatch);
		if (v[i].CanIReprioritize)
			v[i].Priority = GetPrivateProfileInt(INISection, szSpell, defPriority, INIFileName);
		v[i].LastTargetID = 0;
		v[i].LastCast = 0;
		v[i].Recast = 0;
		v[i].TargetID = 0;
		sprintf_s(szSpell, "SpellDuration%d", v[i].IniMatch);
		v[i].Duration = !GetSpellDuration2(v[i].Spell) ? 0 : 6000 * GetSpellDuration2(v[i].Spell) * (ULONGLONG)CalcDuration(v[i].Spell);
		strcpy_s(v[i].Color, color);
		DWORD n = 0;
		if (v[i].Type == TYPE_SPELL)
		{
			v[i].CastTime = pCharData1->GetAACastingTimeModifier((EQ_Spell*)v[i].Spell) +
				pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)v[i].Spell, (EQ_Equipment**)&n, 0) +
				v[i].Spell->CastTime;
		}
		if (v[i].Type == TYPE_AA)
		{
			v[i].CastTime = v[i].Spell->CastTime;
		}
		if (v[i].Type == TYPE_ITEM)
		{

		}
		if (v[i].Type == TYPE_DISC)
		{

		}
	}
	// PopulateIni(v, VectorName);
}
void PluginOn()
{
	AddCommand("/bot", BotCommand);
	AddCommand("/botlist", ListCommand);
	AddCommand("/memmed", MemmedCommand);
}

void PluginOff()
{
	RemoveCommand("/bot");
	RemoveCommand("/botlist");
	RemoveCommand("/memmed");
}

// Called when plugin is loading
PLUGIN_API VOID InitializePlugin(VOID)
{
	PluginOn();
}

// Called when plugin is unloading
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2Bot");
	PluginOff();
}

PLUGIN_API VOID OnPulse(VOID)
{
	if (!InGameOK())
		return;

	if (!ConfigureLoaded)
		return;
	CheckAdds();
	CheckGroup();
	CheckMaster();
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	if (gGameState == GAMESTATE_INGAME)
	{
		//Compare to spawn lists
		if (pSpawn) {
			SPAWNINFO tSpawn;
			memcpy(&tSpawn, pSpawn, sizeof(SPAWNINFO));
			// check against vSpawn/vAdds
			//RemoveFromNotify(&tSpawn, true);
		}
	}
}
#pragma endregion Loading

#pragma region UnusedCode


#pragma endregion UnusedCode

#pragma region MQ2CastCode
#pragma region PrototypesNew


SPELLFAVORITE SpellToMemorize;         // Favorite Spells Array
long          SpellTotal;              // Favorite Spells Total



bool          Parsed = false;            // BTree List Found Flags
Blech         LIST013('#');            // BTree List for OnChat Message on Color  13
Blech         LIST264('#');            // BTree List for OnChat Message on Color 264
Blech         LIST289('#');            // BTree List for OnChat Message on Color 289
Blech         UNKNOWN('#');            // BTree List for OnChat Message on UNKNOWN Yet Color
Blech         SUCCESS('#');            // BTree List for OnChat Message on SUCCESS Detection

#pragma endregion PrototypesNew
bool          Immobile = false;        // Immobile?
bool          Invisible = false;        // Invisibility Check?

bool          Casting = false;        // Casting Window was opened?
long          Resultat = CAST_SUCCESS; // Resultat
ULONGLONG     ImmobileT = 0;            // Estimate when it be immobilized!
long          CastingD = NOID;           // Casting Spell Detected
long          CastingC = NOID;           // Casting Current ID
long          CastingE = CAST_SUCCESS;   // Casting Current Result
long          CastingL = NOID;           // Casting LastOne ID
long          CastingX = CAST_SUCCESS;   // Casting LastOne Result
ULONGLONG     CastingT = 0;              // Casting Timeout
long          CastingO = NOID;           // Casting OnTarget
ULONGLONG     CastingP = 0;              // Casting Pulse

long          TargI = 0;                 // Target ID
long          TargC = 0;                 // Target Current

long          StopF = FLAG_COMPLETE;     // Stop Event Flag Progress?
long          StopE = DONE_SUCCESS;      // Stop Event Exit Value
ULONGLONG     StopM = 0;                 // Stop Event Mark

long          MoveA = FLAG_COMPLETE;     // Move Event AdvPath?
long          MoveS = FLAG_COMPLETE;     // Move Event Stick?
long          MoveF = FLAG_COMPLETE;     // Move Event MQ2AdvPath Following?
long          MoveP = FLAG_COMPLETE;     // Move Event MQ2AdvPath Pathing?

long          MemoF = FLAG_COMPLETE;     // Memo Event Flag
long          MemoE = DONE_SUCCESS;      // Memo Event Exit
ULONGLONG     MemoM = 0;                 // Memo Event Mark

long          DuckF = FLAG_COMPLETE;     // Duck Flag
ULONGLONG     DuckM = 0;                 // Duck Time Stamp

long          CastF = FLAG_COMPLETE;     // Cast Flag
long          CastE = CAST_SUCCESS;      // Cast Exit Return value
long          CastG = NOID;              // Cast Gem ID
void         *CastI = NULL;              // Cast ID   [spell/alt/item]
long          CastK = NOID;              // Cast Kind [spell/alt/item]
long          CastT = 0;                 // Cast Time [spell/alt/item]
ULONGLONG     CastM = 0;                 // Cast TimeMark Start Casting
long          CastR = 3;                 // Cast Retry Counter
long          CastW = RECAST_LAND;                 // Cast Retry Type
char          CastC[MAX_STRING];       // Cast SpellType
char          CastN[MAX_STRING];       // Cast SpellName


PSPELL        CastS = NULL;              // Cast Spell Pointer
										 // Timer Structure
struct TimerSingle {
	long TimerID;
	PSPELL pSpell;
	long EventID;
	int CastE;
	long SpawnID;
	time_t timestamp;
	int Caster;
	bool AutoRecast;
	char SpawnName[64];
	time_t TimeOut;
} pTimerList;

// Spell Event Structure
struct SpellEvent {
	PSPELL pHSpell;								// Higest spell ID
	vector<PSPELL> pSpells;						// All Matching spell ID'S
	char Event[EVENTSLENGTH];
} pSpellEventList;

struct EventTimer {
	char Name[MAX_STRING];
	//	int IsSpell;
	char Event[MAX_STRING];
	int Duration;
	int SpawnType;
} pEventList;
list<TimerSingle>    TimerList;          // timer list
vector<SpellEvent>  SpellEventList;
vector<EventTimer>  EventList;
vector<unsigned long>  BleachList;
time_t lastRun = 0;
long lastTargetID = 0;							// ID of curent Last Target
long currentTargetID = 0;						// ID of curent Target
long TimerCount = 0;							// Timer ID Count
ULONGLONG UITimer = 0;
VOID ClearOne(list<TimerSingle>::iterator &CurList);
VOID DefaultTimer(TimerSingle &MyData);
VOID SetTimerData(TimerSingle &MyData, list<TimerSingle>::iterator &CurList);
long getSpellTimer(long SpawnID, int Caster, vector<PSPELL> SpellSearch);
eSpawnType SpawnType(PSPAWNINFO tSpawn);
VOID RestackTimer(list<TimerSingle>::iterator &CurList, TimerSingle MyData, int Remove);



VOID ClearOne(list<TimerSingle>::iterator &CurList)
{
	return;
}
VOID DefaultTimer(TimerSingle &MyData)
{
	return;
}
VOID SetTimerData(TimerSingle &MyData, list<TimerSingle>::iterator &CurList)
{
	return;
}
VOID RestackTimer(list<TimerSingle>::iterator &CurList, TimerSingle MyData, int Remove)
{
	return;
}


void __stdcall CastEvent(unsigned int ID, void *pData, PBLECHVALUE pValues) {
	Parsed = true;
	if (CastingE < (long)pData) CastingE = (long)pData;
	DebugWrite("[%llu] MQ2Bot:[OnChat]: Result=[%s (%d)] Called=[%d].", MQGetTickCount64(), cast_status[CastingE], CastingE, (long)pData);
}

void Stick(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *StickCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2MoveUtils", 12)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion > 0.9999 && pLook->RemoveSpawn)
		if (StickCALL Request = (StickCALL)GetProcAddress(pLook->hModule, "StickCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}

void FollowPath(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion > 0.999 && pLook->RemoveSpawn)
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQFollowCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}
void Path(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion > 0.999 && pLook->RemoveSpawn)
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQPlayCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}


long Evaluate(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList); if (!zOutput[0]) return 1;
	ParseMacroData(zOutput, MAX_STRING); return atoi(zOutput);
}

void MemoLoad(long Gem, PSPELL Spell)
{
	if (!Spell) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - PSPELL Spell == NULL", PLUGIN_NAME, Gem, Spell);
		return;
	}
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - PCHARINFO pChar == NULL", PLUGIN_NAME, Gem, Spell);
		return;
	}
	if (!pChar->pSpawn) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - pChar->pSpawn == NULL", PLUGIN_NAME, Gem, Spell);
		return;
	}
	if ((pChar->pSpawn->mActorClient.Class) < 0 || pChar->pSpawn->mActorClient.Class > 17) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - pChar->pSpawn->mActorClient.Class out of range: %d", PLUGIN_NAME, Gem, Spell, pChar->pSpawn->mActorClient.Class);
		return;
	}
	if (Spell->ClassLevel[pChar->pSpawn->mActorClient.Class] > pChar->pSpawn->Level) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - Spell->ClassLevel[pChar->pSpawn->mActorClient.Class] > pChar->pSpawn->Level (%d > %d)", PLUGIN_NAME, Gem, Spell, Spell->ClassLevel[pChar->pSpawn->mActorClient.Class], pChar->pSpawn->Level);
		return;
	}
	for (int sp = 0; sp < NUM_SPELL_GEMS; sp++)
		if (SpellToMemorize.SpellId[sp] == Spell->ID)
			SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
	SpellToMemorize.SpellId[((DWORD)Gem < NUM_SPELL_GEMS) ? Gem : 4] = Spell->ID;
}

void Reset() {
	TargI = 0;                 // Target ID
	TargC = 0;                 // Target Check ID
	StopF = FLAG_COMPLETE;     // Stop Event Flag Progress?
	StopE = DONE_SUCCESS;      // Stop Event Exit Value
	MoveA = FLAG_COMPLETE;     // Stop Event AdvPath?
	MoveS = FLAG_COMPLETE;     // Stop Event Stick?
	MoveF = FLAG_COMPLETE;     // Stop Event MQ2AdvPath Following?
	MoveP = FLAG_COMPLETE;     // Stop Event MQ2AdvPath Pathing?
	MemoF = FLAG_COMPLETE;     // Memo Event Flag
	MemoE = DONE_SUCCESS;      // Memo Event Exit
	DuckF = FLAG_COMPLETE;     // Duck Flag
	CastF = FLAG_COMPLETE;     // Cast Flag
	CastE = CAST_SUCCESS;      // Cast Exit Return value
	CastG = NOID;              // Cast Gem ID
	CastI = NULL;              // Cast ID   [spell/alt/item/disc]
	CastK = NOID;              // Cast Kind [spell/alt/item/disc] [-1=unknown]
	CastT = 0;                 // Cast Time [spell/alt/item/disc]
	CastC[0] = 0;              // Cast SpellType
	CastN[0] = 0;              // Cast SpellName
	CastR = 1;                 // Cast Retry Counter
	CastW = RECAST_LAND;       // Cast Retry Type
	Invisible = false;         // Invisibility Check?
	ZeroMemory(&SpellToMemorize, sizeof(SPELLFAVORITE));
	strcpy_s(SpellToMemorize.Name, "Mem a Spell");
	SpellToMemorize.inuse = 1;
	for (int sp = 0; sp < NUM_SPELL_GEMS; sp++) SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
	SpellTotal = 0;
}

VOID AddSpellTimer(int SpawnID, PSPELL pSpell, int TickE, bool ignoreMod)
{
	if (!SpawnID || SpawnID == NOID || !pSpell->ID || !gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) return;
	time_t now = time(NULL);
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();
	switch (TickE) {
	case CAST_SUCCESS: // CAST_SUCCESS
		now = now + CalcDuration(pSpell);
		break;
	case CAST_IMMUNE: // CAST_IMMUNE
		now = now + 100000;
		break;
	case CAST_IMMUNE_RUN: // CAST_IMMUNE
		now = now + 100000;
		break;
	case CAST_IMMUNE_MEZ: // CAST_IMMUNE
		now = now + 100000;
		break;
	case CAST_IMMUNE_SLOW: // CAST_IMMUNE
		now = now + 100000;
		break;
	case CAST_TAKEHOLD: // CAST_TAKEHOLD
		now = now + CalcDuration(pSpell);
		break;
	default: // Unknown
		return;
	}
	bool AutoRecast = false;

	bool Remove = false;
	while (CurList != EndList) {
		if (CurList->SpawnID && CurList->SpawnID == SpawnID && CurList->pSpell && CurList->pSpell->ID == pSpell->ID) {
			if ((ignoreMod && CurList->Caster == CASTEROTHER) || !ignoreMod) {
				if ((now - CurList->timestamp) <= 1) TickE = CurList->CastE;
				AutoRecast = CurList->AutoRecast;
			}
			Remove = true;
			break;
		}
		else CurList++;
	}

	TimerSingle MyData;
	DefaultTimer(MyData);
	TimerCount++;
	MyData.TimerID = TimerCount;
	MyData.pSpell = pSpell;
	MyData.CastE = TickE;
	MyData.SpawnID = SpawnID;
	MyData.AutoRecast = AutoRecast;
	MyData.timestamp = now;
	if (ignoreMod) MyData.Caster = CASTEROTHER;

	RestackTimer(CurList, MyData, Remove);
}
/*
//	Add spell self casted spells spell list including instant cast
//	only usede from /casting
*/
VOID AddSpell(int TargetID, int SpellID, int TickE)
{

	if (PSPELL pSpell = (PSPELL)GetSpellByID(SpellID)) {
		if (!GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer)) return;
		switch (pSpell->TargetType) {
		case  3: // Group v1
		case  6: // Self
		case 41: // Group v2
			AddSpellTimer(GetCharInfo()->pSpawn->SpawnID, pSpell, TickE, false);
			break;
		case 14: // Pet
			AddSpellTimer(GetCharInfo()->pSpawn->PetID, pSpell, TickE, false);
			break;
		case  5: // Single
		case  8: // Targeted AE
			AddSpellTimer(TargetID, pSpell, TickE, false);
			break;
		default: break; // Unknown
		}
	}
}
/*
//	Handle Spell Casting done by yourself and call AddSpell() to
//	add a spell and call CastTimerWnd->UpdateList() to update the Target window
*/
void CastTimer(int TargetID, int SpellID, long TickE)
{
	if (SpellID && TickE == CAST_RECOVER) TickE = CAST_SUCCESS;
	if (SpellID && TickE == CAST_OUTDOORS) TickE = CAST_IMMUNE;
	if (TickE != CAST_SUCCESS && TickE != CAST_IMMUNE && TickE != CAST_TAKEHOLD) return;
	AddSpell(TargetID, SpellID, TickE);
}

void Success(PSPELL Cast) {
	SUCCESS.Reset();
	// WriteChatf("Success(Cast) = %d",Cast);
	if (Cast) {
		char Temps[MAX_STRING];
		bool Added = false;
		/*
		if (Cast->CastOnYou[0]) {
		sprintf_s(Temps, "%s#*#", Cast->CastOnYou);
		aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
		Added = true;
		}
		if (Cast->CastOnAnother[0]) {
		sprintf_s(Temps, "#*#%s#*#", Cast->CastOnAnother);
		aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
		Added = true;
		}
		*/
		//20181224:renji
		//Updated to change PSPELL->CastOnYou to use GetSpellString per Feb 21 2018 patch
		/*CastByMe,CastByOther,CastOnYou,CastOnAnother,WearOff*/
		//CastOnYou
		if (char*str = GetSpellString(Cast->ID, 2)) {
			sprintf_s(Temps, "%s#*#", str);
			aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
			Added = true;
		}
		//CastOnAnother
		if (char*str = GetSpellString(Cast->ID, 3)) {
			sprintf_s(Temps, "%s#*#", str);
			aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
			Added = true;
		}
		if (!Added)
			aCastEvent(SUCCESS, CAST_SUCCESS, "You begin casting#*#");
	}
}

float Speed() {
	float MySpeed = 0.0f;
	if (PSPAWNINFO Self = GetCharInfo()->pSpawn) {
		MySpeed = Self->SpeedRun;
		if (PSPAWNINFO Mount = FindMount(Self))
			MySpeed = Mount->SpeedRun;
	}
	if (BardClass)
		MySpeed = 0.0f;
	return MySpeed;
}

bool Moving() {
	ULONGLONG MyTimer = MQGetTickCount64();
	if (Speed() != 0.0f) ImmobileT = MyTimer + (ULONGLONG)500;
	return (!MQ2Globals::gbMoving && (!ImmobileT || MyTimer > ImmobileT));
}

void StopEnding() {
	char flag[MAX_STRING] = { 0 };
	if (MoveS != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: Stick UnPause Request.", MQGetTickCount64());
		Stick("unpause");
		MoveS = FLAG_COMPLETE;
	}
	if (MoveA != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: AdvPath UnPause Request.", MQGetTickCount64());
		sprintf_s(flag, "/varcalc PauseFlag 0");
		HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
		MoveA = FLAG_COMPLETE;
	}
	if (MoveF != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: MQ2AdvPath UnPause Request.", MQGetTickCount64());
		FollowPath("unpause");
		MoveF = FLAG_COMPLETE;
	}
	if (MoveP != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: MQ2AdvPath UnPause Request.", MQGetTickCount64());
		Path("unpause");
		MoveP = FLAG_COMPLETE;
	}
}

void StopHandle() {
	char flag[MAX_STRING] = { 0 };
	if (StopF == FLAG_REQUEST) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: Request.", MQGetTickCount64());
		StopM = MQGetTickCount64() + (ULONGLONG)DELAY_STOP;
		StopF = FLAG_PROGRESS1;
		StopE = DONE_PROGRESS;
	}
	if (Evaluate("${If[${Stick.Status.Equal[ON]},1,0]}")) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: Stick Pause Request.", MQGetTickCount64());
		Stick("pause");
		MoveS = FLAG_PROGRESS1;
	}
	if (Evaluate("${If[${Bool[${FollowFlag}]},1,0]}")) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: AdvPath Pause Request.", MQGetTickCount64());
		sprintf_s(flag, "/varcalc PauseFlag 1");
		HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
		MoveA = FLAG_PROGRESS1;
	}
	if (Evaluate("${If[${AdvPath.Following} && !${AdvPath.Paused},1,0]}")) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64());
		FollowPath("pause");
		MoveF = FLAG_PROGRESS1;
	}
	if (Evaluate("${If[${AdvPath.Playing} && !${AdvPath.Paused},1,0]}")) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64());
		Path("pause");
		MoveP = FLAG_PROGRESS1;
	}
	if (Immobile = Moving()) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: Complete.", MQGetTickCount64());
		StopF = FLAG_COMPLETE;
		StopE = DONE_SUCCESS;
	}
	if (MQGetTickCount64() > StopM) {
		WriteChatf("[%llu] MQ2Bot:[Immobilize]: Aborting!", MQGetTickCount64());
		StopF = FLAG_COMPLETE;
		StopE = DONE_ABORTED;
		return;
	}
	if (StopF == FLAG_PROGRESS1) {
		StopF = FLAG_PROGRESS2;
		if (Speed() != 0.0f) {
			MQ2Globals::ExecuteCmd(FindMappableCommand("back"), 1, 0);
			MQ2Globals::ExecuteCmd(FindMappableCommand("back"), 0, 0);
		}
	}
}

bool GEMReady(DWORD ID) {
	if (pCastSpellWnd && ID < NUM_SPELL_GEMS)
		if ((long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[ID]->spellicon != NOID)
			if ((long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[ID]->spellstate != 1)
				return true;
	return false;
}
void MemoHandle() {
	static int SP = 0;
	if (!pSpellBookWnd)
		MemoE = DONE_ABORTED;
	else {
		bool Complete = true;
		for (int sp = 0; sp < NUM_SPELL_GEMS; sp++)
			if (SpellToMemorize.SpellId[sp] != 0xFFFFFFFF && SpellToMemorize.SpellId[sp] != GetCharInfo2()->MemorizedSpells[sp]) {
				SP = sp;
				Complete = false;
				break;
			}
		if (!Complete) {
			if (MemoF == FLAG_REQUEST) {
				DebugWrite("[%llu] MQ2Bot:[Memorize]: Immobilize.", MQGetTickCount64());
				MemoF = FLAG_PROGRESS1;
				MemoE = DONE_PROGRESS;
				//if (GetCharInfo()->pSpawn->Level<4 && DELAY_MEMO<17000UL)
				//	DELAY_MEMO = 17000UL;
				MemoM = MQGetTickCount64() + DELAY_STOP + DELAY_MEMO * SpellTotal;
				if (StopF == FLAG_COMPLETE)
					StopE = DONE_SUCCESS;
				if (StopF == FLAG_COMPLETE)
					StopF = FLAG_REQUEST;
				if (StopF != FLAG_COMPLETE)
					StopHandle();
			}
			if (MemoF == FLAG_PROGRESS1 && StopE == DONE_SUCCESS) {
				DebugWrite("[%llu] MQ2Bot:[Memorize]: Spell(s).", MQGetTickCount64());
				MemoF = FLAG_PROGRESS2;
				DWORD Favorite = (DWORD)&SpellToMemorize;
				pSpellBookWnd->MemorizeSet((int*)Favorite, NUM_SPELL_GEMS);
			}
			if (StopE == DONE_ABORTED || MQGetTickCount64() > MemoM)
				MemoE = DONE_ABORTED;
		}
		else {
			if (GEMReady(SP) || MQGetTickCount64() > MemoM) {
				DebugWrite("[%llu] MQ2Bot:[Memorize]: Complete.", MQGetTickCount64());
				MemoF = FLAG_COMPLETE;
				MemoE = DONE_SUCCESS;
			}
		}
	}
	if (MemoE == DONE_ABORTED || !pSpellBookWnd) {
		WriteChatf("[%llu] MQ2Bot:[Memorize]: Aborting!", MQGetTickCount64());
		MemoF = FLAG_COMPLETE;
	}
	if (MemoF == FLAG_COMPLETE && (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow)) {
		DebugWrite("[%llu] MQ2Bot:[Memorize]: Closebook.", MQGetTickCount64());
		EzCommand("/book");
	}
}

void DuckHandle() {
	if (DuckF == FLAG_REQUEST) {
		DebugWrite("[%llu] MQ2Bot:[Duck]: Request.", MQGetTickCount64());
		DuckF = FLAG_PROGRESS1;
		DuckM = 0;
	}
	if (DuckF == FLAG_PROGRESS1) {
		if (GetCharInfo()->pSpawn->Mount) {
			if (!DuckM) {
				DebugWrite("[%llu] MQ2Bot:[Duck]: Dismount.", MQGetTickCount64());
				DuckM = MQGetTickCount64();
				EzCommand("/dismount");
			}
		}
		else DuckF = FLAG_PROGRESS2;
	}
	if (DuckF == FLAG_PROGRESS2) {
		DebugWrite("[%llu] MQ2Bot:[Duck]: StopCast.", MQGetTickCount64());
		EzCommand("/stopcast");
		CastingE = CAST_ABORTED;
		DuckF = FLAG_COMPLETE;
	}
}

void CastHandle(_BotSpell &spell) {
	char flag[MAX_STRING] = { 0 };
	// we got the casting request cookies, request immobilize/memorize if needed.
	if (CastF == FLAG_REQUEST) {
		DebugWrite("[%llu] MQ2Bot:[Casting]: Request.", MQGetTickCount64());
		CastF = FLAG_PROGRESS1;
		if (StopF == FLAG_COMPLETE) StopF = DONE_SUCCESS;
		if (StopF == FLAG_COMPLETE && CastT > 100 && !BardClass) StopF = FLAG_REQUEST;
		if (MemoF != FLAG_COMPLETE) MemoHandle();
		else if (StopF != FLAG_COMPLETE) StopHandle();
	}

	// waiting on the casting results to take actions.
	if (CastF == FLAG_PROGRESS3 && CastingE != DONE_PROGRESS) {
		CastF = FLAG_PROGRESS4;
		if (CastR) CastR--;
		if (CastR) {
			if ((CastingE == CAST_SUCCESS && CastW != RECAST_LAND) ||
				(CastingE == CAST_COLLAPSE) ||
				(CastingE == CAST_FIZZLE) ||
				(CastingE == CAST_INTERRUPTED) ||
				(CastingE == CAST_RECOVER) ||
				(CastingE == CAST_RESIST)) {
				DebugWrite("[%llu] MQ2Bot:[Casting]: AutoRecast [%d].", MQGetTickCount64(), CastingE);
				if (!TargC) TargC = (pTarget) ? ((PSPAWNINFO)pTarget)->SpawnID : 0;
				CastM = MQGetTickCount64() + DELAY_CAST;
				CastF = FLAG_REQUEST;
			}
		}
	}

	// casting is over, grab latest casting results and exit.
	if (CastF == FLAG_PROGRESS4) {
		if (CastE > CastingE) CastingE = CastE;
		CastF = FLAG_COMPLETE;
	}

	// evaluate if we are taking too long, or immobilize/memorize event failed.
	if (CastF != FLAG_COMPLETE) {
		if (StopE == DONE_ABORTED || MemoE == DONE_ABORTED || MQGetTickCount64() > CastM) {
			WriteChatf("[%llu] MQ2Bot:[Casting]: Aborting because it took too long! (%s)", MQGetTickCount64(), StopE == DONE_ABORTED ? "Immobilized" : (MemoE == DONE_ABORTED ? "Memorization Fail" : "Timer Expired"));
			CastF = FLAG_PROGRESS4;
			CastE = CAST_NOTREADY;
		}
	}

	// waiting for opportunity to start casting, end if conditions not favorables.
	if (CastF == FLAG_PROGRESS1) {
		if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) return; // casting going on
		if (!CastS) {
			CastE = CAST_NOTREADY;
			CastF = DONE_PROGRESS;
			return;
		}

		CastingC = CastS->ID;

		CastF = FLAG_PROGRESS4;
		if (TargC && (!pTarget || (pTarget && ((PSPAWNINFO)pTarget)->SpawnID != TargC))) {
			if (CastW == RECAST_LAND) CastE = CAST_ABORTED;
		}
		else if (Invisible && GetCharInfo()->pSpawn->HideMode) {
			CastE = CAST_INVISIBLE;
		}
		else if (GetCharInfo()->Stunned) {
			CastE = CAST_STUNNED;
		}
		else if (StopF != FLAG_COMPLETE || MemoF != FLAG_COMPLETE) {
			CastF = FLAG_PROGRESS1;
		}
		else {
			long TimeReady = SpellReady(spell.ID) ? 0 : (long)spell.Spell->RecoveryTime;    // get estimate time before it's ready.
			if (TimeReady > 3000)  CastE = CAST_NOTREADY;   // if estimate higher than 3 seconds aborts.
			else if (!TimeReady) CastF = FLAG_PROGRESS2;  // estimate says it's ready so cast it
			else CastF = FLAG_PROGRESS1;                 // otherwise give it some time to be ready.
		}
	}

	// we got the final approbation to cast, so lets do it.
	if (CastF == FLAG_PROGRESS2) {
		DebugWrite("[%llu] MQ2Bot:[Casting]: Cast.", MQGetTickCount64());
		Success(CastS);
		CastF = FLAG_PROGRESS3;
		CastE = DONE_PROGRESS;
		CastingT = MQGetTickCount64() + CastT + 250 + (pConnection->Last) * 4;
		CastingE = DONE_PROGRESS;
		CastingC = CastS->ID;
		if ((long)GetCharInfo()->pSpawn->CastingData.SpellID > 0) {
			CastingX = (CastingE < CAST_SUCCESS) ? CAST_SUCCESS : CastingE;
			CastingL = CastingC;
			if (CastK == TYPE_SPELL) {
				sprintf_s(flag, "/multiline ; /stopsong ; /cast \"%s\"", CastN);
				HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
			}
			else if (CastK == TYPE_AA) {
				sprintf_s(flag, "/alt activate %d", spell.ID);
				HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
			}
			else if (CastK == TYPE_ITEM) {
				sprintf_s(flag, "/multiline ; /stopsong ; /useitem \"%s\"", CastN);
				HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
			}
		}
		else {
			char castN[MAX_STRING];
			sprintf_s(castN, "\"%s\"", spell.SpellName);
			if (CastK == TYPE_SPELL)     Cast(GetCharInfo()->pSpawn, castN);
			else if (CastK == TYPE_AA) {
				sprintf_s(flag, "/alt activate %d", spell.ID);
				HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
			}
			else if (CastK == TYPE_ITEM) {
				sprintf_s(flag, "/multiline ; /stopsong ; /useitem \"%s\"", CastN);
				HideDoCommand(GetCharInfo()->pSpawn, flag, FromPlugin);
			}
		}
		Announce(spell);
	}
}

long GEMID(DWORD ID) {
	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) if (GetCharInfo2()->MemorizedSpells[GEM] == ID) return GEM;
	return NOID;
}
bool Flags() {
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) { DebugWrite("MQ2Bot: pCastingWnd=TRUE"); return true; }
	if (CastF != FLAG_COMPLETE) { DebugWrite("MQ2Bot: CastF!=FLAG_COMPLETE"); return true; }
	if (DuckF != FLAG_COMPLETE) { DebugWrite("MQ2Bot: DuckF!=FLAG_COMPLETE"); return true; }
	if (MemoF != FLAG_COMPLETE) { DebugWrite("MQ2Bot: MemoF!=FLAG_COMPLETE"); return true; }
	if (StopF != FLAG_COMPLETE) { DebugWrite("MQ2Bot: StopF!=FLAG_COMPLETE"); return true; }
	if (MoveS != FLAG_COMPLETE) { DebugWrite("MQ2Bot: MoveS!=FLAG_COMPLETE"); return true; }
	if (MoveF != FLAG_COMPLETE) { DebugWrite("MQ2Bot: MoveF!=FLAG_COMPLETE"); return true; }
	if (MoveP != FLAG_COMPLETE) { DebugWrite("MQ2Bot: MoveP!=FLAG_COMPLETE"); return true; }
	if (MoveA != FLAG_COMPLETE) { DebugWrite("MQ2Bot: MoveA!=FLAG_COMPLETE"); return true; }
	return false;
}

void BotCastCommand(_BotSpell &spell) {
	Resultat = CAST_DISTRACTED;
	if (!gbInZone || Flags() || (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow))
	{
		DebugWrite("[%llu] MQ2Bot:[Casting]: Complete. [%s (%d)][%s%s%s%s]", MQGetTickCount64(), cast_status[Resultat], Resultat,
			!gbInZone ? " ZONE " : "", Flags() ? " FLAGS " : "", (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow) ? " SHOW " : "");
		return;
	}
	Reset();
	Resultat = CAST_SUCCESS;
	Invisible = false;
	if (GetCharInfo()->Stunned)                            Resultat = CAST_STUNNED;
	else if (Invisible && GetCharInfo()->pSpawn->HideMode) Resultat = CAST_INVISIBLE;
	//else if (!SpellFind(CastN, CastC))                      Resultat = CAST_UNKNOWN;
	//else if (fTYPE != TYPE_SPELL && !SpellReady(spell.ID)) Resultat = CAST_NOTREADY;
	if (spell.Spell->TargetType != 14 && spell.Spell->TargetType != 6 && spell.Spell->TargetType != 3 && spell.Spell->TargetType != 41) //&& spell.Spell->TargetType != 45
		if (PSPAWNINFO Target = (PSPAWNINFO)GetSpawnByID(TargI)) *(PSPAWNINFO*)ppTarget = Target;
		else Resultat = CAST_NOTARGET;
		if (Resultat == CAST_SUCCESS && spell.Type == TYPE_SPELL) {
			if (BardClass) {
				if (GetCharInfo()->pSpawn->CastingData.SpellID) EzCommand("/stopsong");
			}
			CastG = GEMID(spell.ID);
			if (CastG == NOID) {
				CastG = atoi(&spell.Gem[(_strnicmp(spell.Gem, "gem", 3)) ? 0 : 3]) - 1;
				MemoLoad(CastG, spell.Spell);
				SpellTotal = 1;
				MemoF = FLAG_REQUEST;
				MemoE = DONE_SUCCESS;
			}

		}
		if (Resultat != CAST_SUCCESS) {
			DebugWrite("[%llu] MQ2Bot:[Casting]: Complete. [%s (%d)]", MQGetTickCount64(), cast_status[Resultat], Resultat);
			return;
		}
		CastF = FLAG_REQUEST;
		CastI = spell.Spell;
		CastK = spell.Type;
		CastT = spell.CastTime;
		CastS = spell.Spell;
		CastM = MQGetTickCount64() + DELAY_CAST;
		strcpy_s(CastN, spell.SpellName);
		DebugWrite("[%llu] MQ2Bot:[Casting]: Name<%s> Type<%d>.", MQGetTickCount64(), CastN, CastK);
		CastHandle(spell);
}
// NEW end functions

#pragma endregion MQ2CastCode
#pragma warning ( pop )

#endif MQ2Bot2
