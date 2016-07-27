/* notes:

_SPAWNS KillTarget; // we always store the current kill target here
vector<_Spawns> vGroup, vXTarget, vAdds; // manage all the various spawns, and if something is mezzed, dont remove it unless it is banished.
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

#pragma region Headers
// Comment out MMOBUGS_LOADER to not use MMOBugs specific code
#define MMOBUGS_LOADER
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
#include <stdio.h>
#include "../MQ2VladUtil/MQ2Vlad.h"
#include <algorithm>

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
struct _BotSpells;

typedef struct _BotSpells // has to be before the FunctionDeclarations because a bunch use it
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
	OPTIONS				SpellTypeOption;			// int conversion from spell type
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
												// save this just in case:  void(*CheckFunc)(std::vector<_BotSpells> &, int);
	void(*CheckFunc)(int);
} BotSpells, *PBotSpells;

typedef struct _Spawns
{
	vector<_BotSpells>	vSpellList;
	char				SpawnBuffList[MAX_STRING];
	int					ID;
	PSPAWNINFO			Spawn;
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
void		BotCastCommand(_BotSpells &spell);
VOID		DebugWrite(PCHAR szFormat, ...);
void		EQBCSwap(char startColor[MAX_STRING]);
double		round(double d);

// declaration of spawn functions
int			PctEnd(PSPAWNINFO pSpawn);
int			PctHP(PSPAWNINFO pSpawn);
int			PctMana(PSPAWNINFO pSpawn);

// declaration of spell functions
void		DiscCategory(PSPELL pSpell);
DWORD		GetSpellDuration2(PSPELL pSpell);
void		LoadBotSpell(vector<_BotSpells> &v, char VectorName[MAX_STRING]);
void		PopulateIni(vector<_BotSpells> &v, char VectorName[MAX_STRING]);
void		SortSpellVector(vector<_BotSpells> v);
void		SpellCategory(PSPELL pSpell);
void		SpellType(PSPELL pSpell);
bool		ValidDet(PSPELL pSpell, PSPAWNINFO Target);
bool		ValidBen(PSPELL pSpell, PSPAWNINFO Target);
#pragma endregion FunctionDeclarations

#pragma region VariableDefinitions
// bool declares
bool  BardClass = false, DEBUG_DUMPFILE = false, ConfigureLoaded = false;

// char declares
char BodyTypeFix[MAX_STRING] = { 0 }, CurrentRoutine[MAX_STRING] = { 0 }, EQBCColor[MAX_STRING] = { 0 }, INISection[MAX_STRING] = { 0 }, spellCat[MAX_STRING] = { 0 }, spellType[MAX_STRING] = { 0 };

// int declares
int	AnnounceAdds = 1, AnnounceEcho = 1, AnnounceEQBC = 0, AssistAt = 100, AssistRange = 100, DefaultGem = 1, dotExtend = 0, hotExtend = 0, mezExtend = 0;

// float declares
float benDurExtend = 0.00, benRngExtend = 1.00, detDurExtend = 0.00, detRngExtend = 1.00, reinforcement = 0.00;

// ULONGLONG declares
ULONGLONG LastAnnouncedSpell = 0, SpellTimer = 0;

//vector _BotSpell declares
vector<_BotSpells> vMaster, vMemmedSpells, vTemp;

//vector int declares
vector<int> vClickyPrestige;

// vector PSPELL declares
vector<PSPELL> vClickySpell;
// vector PSPAWNINFO declares

// vector string declares
vector<string> vClicky;

// map declares
std::map<string, string> SpellIf;
std::map<string, string>::iterator SpellIt;

// deque declares
deque<PSPAWNINFO>	vAdds, vPossibleAdds;

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
"50","0", "75", "0", "72","0","0", "100", NULL };// 10 per line
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
_BotSpells EndRegenSpell;
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

// Used by the Adds TLO
// Returns a SPAWNINFO struct pointer for the requested index from vAdds
static inline PSPAWNINFO GetAddInfo(unsigned long N)
{
	if (!vAdds.size())
		return NULL;
	N--;
	if (N >= vAdds.size())
		return NULL;
	return vAdds[N];
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
void Announce(_BotSpells &spell)
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
			::sprintf(castLine, "/bc %s%s [+x+]--> %s", EQBCColor, spell.SpellName, pSpawn->DisplayedName);
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
	strcpy(acDir, pszDir);

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
			vsprintf(szOutput, szFormat, vaList);
			FILE *fOut = NULL;
			CHAR Filename[MAX_STRING] = { 0 };
			CHAR szBuffer[MAX_STRING] = { 0 };
			DWORD i;
			sprintf(Filename, "%s\\MQ2Bot_Debug.log", gszLogPath);
			ChkCreateDir(gszLogPath);
			for (i = 0; i < strlen(Filename); i++)
			{
				if (Filename[i] == '\\')
				{
					strncpy(szBuffer, Filename, i);
				}
			}
			fOut = fopen(Filename, "at");
			if (!fOut)
			{
				return;
			}
			char tmpbuf[128];
			struct tm *today;
			time_t tm;
			tm = time(NULL);
			today = localtime(&tm);
			strftime(tmpbuf, 128, "%Y/%m/%d %H:%M:%S", today);
			CHAR PlainText[MAX_STRING] = { 0 };
			StripMQChat(szOutput, PlainText);
			sprintf(szBuffer, "[%s] %s", tmpbuf, PlainText);
			for (unsigned int i = 0; i < strlen(szBuffer); i++) {
				if (szBuffer[i] == 0x07) {
					if ((i + 2) < strlen(szBuffer))
						strcpy(&szBuffer[i], &szBuffer[i + 2]);
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
	char itemName[MAX_STRING], spellName[MAX_STRING], itemlevel[MAX_STRING];
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
							hotExtend = round(GetCharInfo2()->AAList[j].PointsSpent / 12);
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
							hotExtend = round(GetCharInfo2()->AAList[j].PointsSpent / 12);
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
							dotExtend = round(GetCharInfo2()->AAList[j].PointsSpent / 13);
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
	::sprintf(EQBCColor, "[+x+]");
	for (int i = 0; ChatColors[i] && EQBCColors[i]; i++)
	{
		if (strstr(startColor, ChatColors[i]))
		{
			::sprintf(EQBCColor, EQBCColors[i]);
			return;
		}
	}
}

bool FindPlugins(PCHAR PluginName)
{
	PMQPLUGIN pPlugin = pPlugins;
	while (pPlugin)
	{
		char szTemp[MAX_STRING];
		::sprintf(szTemp, "%s", pPlugin->szFilename);
		if (!_stricmp(strlwr(szTemp), PluginName)) return true;
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
			return ((pSpawn->HPCurrent * 100) / pSpawn->HPMax);
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

void PopulateIni(vector<_BotSpells> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	for (int i = 0; i < v.size(); i++)
	{
		::sprintf(szSpell, "%sTotal", VectorName);
		WritePrivateProfileString(INISection, szSpell, itoa(v.size(), szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sSpellName%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].SpellName, INIFileName);
		//::sprintf(szSpell, "%sSpellIconName%d", VectorName, i);
		//WritePrivateProfileString(INISection, szSpell, v[i].SpellIconName, INIFileName);
		::sprintf(szSpell, "%sIf%d", VectorName, i);
		if (IsNumber(v[i].If))
			WritePrivateProfileString(INISection, szSpell, itoa(atoi(v[i].If), szTemp, 10), INIFileName);
		else
			WritePrivateProfileString(INISection, szSpell, v[i].If, INIFileName);
		::sprintf(szSpell, "%sGem%d", VectorName, i);
		if (IsNumber(szSpell))
			WritePrivateProfileString(INISection, szSpell, itoa(atoi(v[i].Gem), szTemp, 10), INIFileName);
		else
			WritePrivateProfileString(INISection, szSpell, v[i].Gem, INIFileName);
		::sprintf(szSpell, "%sUseOnce%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].UseOnce, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sForceCast%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].ForceCast, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sUse%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].Use, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sStartAt%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].StartAt, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sStopAt%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].StopAt, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sNamedOnly%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].NamedOnly, szTemp, 10), INIFileName);
		::sprintf(szSpell, "%sPriority%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, itoa(v[i].Priority, szTemp, 10), INIFileName);
	}
}

double round(double d)
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
	char cTest[MAX_STRING];
	PSPELL tmpSpell2;
	// Check Buffs
	for (nBuff = 0; nBuff < NUM_LONG_BUFFS && pChar; nBuff++)
	{
		if (pChar->Buff[nBuff].SpellID>0 && GetSpellByID(pChar->Buff[nBuff].SpellID)) {
			PSPELL tmpSpell = GetSpellByID(pChar->Buff[nBuff].SpellID);
			if (tmpSpell && pSpell)
			{
				for (int nSlot = 0; nSlot < GetSpellNumEffects(pSpell); nSlot++)
				{
					int attrib = GetSpellAttrib(pSpell, nSlot);

					if (attrib == 374 || attrib == 419 || attrib == 339)
					{
						tmpSpell2 = GetSpellByID(GetSpellBase2(pSpell, nSlot));
						if (tmpSpell2 && !BuffStackTest(tmpSpell, tmpSpell2) || !outdoor && tmpSpell2->Location == 1 || tmpSpell == tmpSpell2 || attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name))
						{
							stacks = false;
							return stacks;
						}
					}
					if (attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name) || !outdoor && tmpSpell->Location == 1)
					{
						stacks = false;
						return stacks;
					}
				}
				if ((!BuffStackTest(pSpell, tmpSpell) || pSpell == tmpSpell || strstr(pSpell->Name, tmpSpell->Name)) || !outdoor && pSpell->Location == 1)
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
		if (pChar->ShortBuff[nBuff].SpellID>0) {
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
	return (result<0);
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
					if (pPCData->GetCombatAbilityTimer(pSpell->CARecastTimerID, pSpell->SpellGroup) < timeNow)
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
			iCalc = round(fCalc) + dotExtend;
		else
			iCalc = round(fCalc);
		if (iCalc > 3 && pSpell->Category == 31)
			iCalc = round(fCalc) + mezExtend;
		else
			iCalc = round(fCalc);
	}
	if (pSpell->SpellType == 1) {
		fCalc = fCalc + benDurExtend + reinforcement;
		if (iCalc > 3 && pSpell->Subcategory == 32)
			iCalc = round(fCalc) + hotExtend;
		else
			iCalc = round(fCalc);
		if (iCalc > 3 && pSpell->Subcategory == 140)
			iCalc = 4;
	}
	return iCalc;
}

bool CanICast(_BotSpells &spell)
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
							//_BotSpells spell;
							vMemmedSpells[nGem].Spell = pSpell;
							vMemmedSpells[nGem].PreviousID = vMemmedSpells[nGem].ID;
							::strcpy(vMemmedSpells[nGem].SpellName, pSpell->Name);
							vMemmedSpells[nGem].ID = pSpell->ID;
							::strcpy(vMemmedSpells[nGem].Gem, itoa(nGem, szTemp, 10));
							SpellType(pSpell);
							vMemmedSpells[nGem].CheckFunc = NULL; // temp code this needs replaced once i dont crash
							::strcpy(vMemmedSpells[nGem].SpellCat, spellType);
							int defStartAt = 0, defUse = 0, defStopAt = 0, defPriority = 0, defNamedOnly = 0, defUseOnce = 0, defForceCast = 0;
							for (int x = 0; DefaultSection[x]; x++)
							{
								if (!stricmp(spellType, DefaultSection[x]))
								{
									::strcpy(color, DefaultColor[x]);
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
								::sprintf(szSpell, "Spell%dName", x);
								if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
								{
									if (!stricmp(szSpell, pSpell->Name))
										::sprintf(spellNum, "Spell%d", x);
								}
							}
							::sprintf(szSpell, "%sIf", spellNum);
							if (GetPrivateProfileString(INISection, szSpell, "1", szTemp, MAX_STRING, INIFileName))
								::strcpy(vMemmedSpells[nGem].If, szTemp);
							::sprintf(szSpell, "%sGem", spellNum);
							if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
								::strcpy(vMemmedSpells[nGem].Gem, szTemp);
							::sprintf(szSpell, "%sUseOnce", spellNum);
							vMemmedSpells[nGem].UseOnce = GetPrivateProfileInt(INISection, szSpell, defUseOnce, INIFileName);
							::sprintf(szSpell, "%sForceCast", spellNum);
							vMemmedSpells[nGem].ForceCast = GetPrivateProfileInt(INISection, szSpell, defForceCast, INIFileName);
							::sprintf(szSpell, "%sUse", spellNum);
							vMemmedSpells[nGem].Use = GetPrivateProfileInt(INISection, szSpell, defUse, INIFileName);
							::sprintf(szSpell, "%sStartAt", spellNum);
							vMemmedSpells[nGem].StartAt = GetPrivateProfileInt(INISection, szSpell, defStartAt, INIFileName);
							::sprintf(szSpell, "%sStopAt", spellNum);
							vMemmedSpells[nGem].StopAt = GetPrivateProfileInt(INISection, szSpell, defStopAt, INIFileName);
							::sprintf(szSpell, "%sNamedOnly", spellNum);
							vMemmedSpells[nGem].NamedOnly = GetPrivateProfileInt(INISection, szSpell, defNamedOnly, INIFileName);
							::sprintf(szSpell, "%sPriority", spellNum);
							vMemmedSpells[nGem].Priority = GetPrivateProfileInt(INISection, szSpell, defPriority, INIFileName);
							vMemmedSpells[nGem].LastTargetID = 0;
							vMemmedSpells[nGem].LastCast = 0;
							vMemmedSpells[nGem].Recast = 0;
							vMemmedSpells[nGem].TargetID = 0;
							::sprintf(szSpell, "%sDuration", spellNum);
							vMemmedSpells[nGem].Duration = !GetSpellDuration2(vMemmedSpells[nGem].Spell) ? 0 : 6000 * GetSpellDuration2(vMemmedSpells[nGem].Spell) * (ULONGLONG)CalcDuration(vMemmedSpells[nGem].Spell);
							::strcpy(vMemmedSpells[nGem].Color, color);
							DWORD n = 0;
							if (vMemmedSpells[nGem].Type == TYPE_SPELL)
							{
								vMemmedSpells[nGem].CastTime = pCharData1->GetAACastingTimeModifier((EQ_Spell*)vMemmedSpells[nGem].Spell) +
									pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)vMemmedSpells[nGem].Spell, (EQ_Equipment**)&n, 0) +
									vMemmedSpells[nGem].Spell->CastTime;
							}
							change = true;
							if(vMemmedSpells[nGem].SpellTypeOption!=::OPTIONS::ZERO)
								WriteChatf("\arMQ2Bot\aw::\ayAdded: \aw%s (%s)", vMemmedSpells[nGem].SpellName, vMemmedSpells[nGem].SpellCat);
							else
								WriteChatf("\arMQ2Bot\aw::\ayDetected: \aw%s (%s)", vMemmedSpells[nGem].SpellName, vMemmedSpells[nGem].SpellCat);
							for (int i = 0; i < vMaster.size(); i++)
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
						for (int i = 0; i < vMaster.size(); i++)
						{
							if (vMaster[i].ID == vMemmedSpells[nGem].ID)
							{
								WriteChatf("\arMQ2Bot\aw::\aoRemoved: \aw(%s)", vMemmedSpells[nGem].SpellName);
								vMaster.erase(vMaster.begin() + i);
								BotSpells spell;
								vMemmedSpells[nGem] = spell;
								vMemmedSpells[nGem].ID = 0;
								break;
							}
						}
					}
				}
			}
		}
		if (change)
			SortSpellVector(vMaster);
	}
	catch (...)
	{

	}
}

void DiscCategory(PSPELL pSpell)
{
	::sprintf(spellCat, "Unknown");
	if (!pSpell)
		return;
	if (DWORD cat = pSpell->Subcategory) {
		if (PVOID ptr = pCDBStr->GetString(cat, 5, NULL)) {
			::sprintf(spellCat, "%s", pCDBStr->GetString(cat, 5, NULL));
		}
	}

	int attrib0 = GetSpellAttribX(pSpell, 0);

	if (pSpell->Subcategory == 152)
	{
		::sprintf(spellCat, "Aggro");
		return;
	}
	if (attrib0 == 189)
	{
		::sprintf(spellCat, "End Regen");
		return;
	}
	if (attrib0 == 31 || pSpell->Subcategory == 35 || pSpell->Category == 35)
	{
		::sprintf(spellCat, "Mez");
		return;
	}
	if (attrib0 == 86)
	{
		::sprintf(spellCat, "Pacify");
		return;
	}
	if (attrib0 == 180)
	{
		::sprintf(spellCat, "Spell Block");
		return;
	}
	if (!strcmp(szSkills[pSpell->Skill], "Throwing"))
	{
		::sprintf(spellCat, "Throwing");
		return;
	}
	if (strstr(pSpell->Name, "s Aura"))
	{
		::sprintf(spellCat, "Aura");
		return;
	}
	if (HasSpellAttrib(pSpell, 220) || strstr(pSpell->Name, "Dichotomic"))
	{
		::sprintf(spellCat, "Offense");
		return;
	}
	if ((attrib0 == 0 && GetSpellBase(pSpell, 0) < 0) || attrib0 == 300 || attrib0 == 200)
	{
		::sprintf(spellCat, "Offense");
		return;
	}
	for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot)
	{
		int attrib = GetSpellAttrib(pSpell, slot);
		if (attrib == 169 || attrib == 171 || attrib == 182 || attrib == 183 || attrib == 184
			|| attrib == 185 || attrib == 186 || attrib == 193 || attrib == 196)
		{
			::sprintf(spellCat, "Offense");
			return;
		}

		if (attrib == 162 || attrib == 168 || attrib == 172 || attrib == 174
			|| attrib == 175 || attrib == 188 || attrib == 320)
		{
			::sprintf(spellCat, "Defense");
			return;
		}
	}
	if (attrib0 == 147)
	{
		::sprintf(spellCat, "OOC Heal");
		return;
	}
	if (attrib0 == 181)
	{
		::sprintf(spellCat, "Fearless");
		return;
	}
	if (attrib0 == 227 && GetSpellBase2(pSpell, 0) == 8)
	{
		::sprintf(spellCat, "Offense");
		return;
	}
	if (attrib0 == 340 || attrib0 == 374 || attrib0 == 323)
	{
		if (PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, 0)))
		{
			if (GetSpellAttribX(pSpell2, 0) == 328 || HasSpellAttrib(pSpell, 162))
			{
				::sprintf(spellCat, "Defense");
				return;
			}
			if (HasSpellAttrib(pSpell2, 85))
			{
				::sprintf(spellCat, "Combat Innates");
				return;
			}
			if (HasSpellAttrib(pSpell2, 184))
			{
				::sprintf(spellCat, "Offense");
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
				::sprintf(spellCat, "Aggro");
				return;
			}
			if (HasSpellAttrib(pSpell2, 192))
			{
				::sprintf(spellCat, "Aggro");
				return;
			}
			if (attrib0_ == 92 && GetSpellBase(pSpell2, 0) < 0)
			{
				::sprintf(spellCat, "Jolt");
				return;
			}
		}
	}
	if (HasSpellAttrib(pSpell, 192))
	{
		::sprintf(spellCat, "Aggro");
		return;
	}
	if (strstr(pSpell->Name, "Unholy Aura"))
	{
		::sprintf(spellCat, "Lifetap");
		return;
	}
}

bool FindSpell(PSPELL pSpell, vector<_BotSpells> &v)
{
	for (int i = 0; i < v.size(); i++)
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

bool ShouldICast(_BotSpells &spell)
{
	if (spell.If[0] != '\0')
	{
		char sSpell[MAX_STRING] = { 0 };
		::sprintf(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
		ParseMacroData(sSpell);
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

bool ShouldICastDetrimental(_BotSpells &spell)
{
	if (!vAdds.size() && !vPossibleAdds.size())
		return false;
	if (!vAdds[0])
		return false;
	if (GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
		return false;
	return true;
}

bool sortByPriority(const BotSpells &lhs, const BotSpells &rhs) { return lhs.Priority > rhs.Priority; } // Sort spell vectors by priority

void SortSpellVector(vector<_BotSpells> v)  // the actual sorting of a spell vector by priority
{
	sort(v.begin(), v.end(), sortByPriority);
}

void SpellCategory(PSPELL pSpell)
{
	::sprintf(spellCat, "Unknown");
	if (!pSpell)
		return;
	if (DWORD cat = pSpell->Subcategory) {
		if (PVOID ptr = pCDBStr->GetString(cat, 5, NULL)) {
			::sprintf(spellCat, "%s", pCDBStr->GetString(cat, 5, NULL));
		}
	}
	if (pSpell->Subcategory == 81 && strstr(pSpell->Name, "Mal") || pSpell->SpellIcon == 55)
	{
		::sprintf(spellCat, "Malo");
		return;
	}
	if ((pSpell->Subcategory == 81 || pSpell->SpellIcon == 72) && strstr(pSpell->Name, "Tash"))
	{
		::sprintf(spellCat, "Tash");
		return;
	}
	if (pSpell->SpellIcon == 17 || strstr(pSpell->Name, "Helix of the Undying") || strstr(pSpell->Name, "Deep Sleep's Malaise"))
	{
		::sprintf(spellCat, "Slow");
		return;
	}
	if (pSpell->SpellIcon == 5 || pSpell->SpellIcon == 160 && strstr(pSpell->Name, "Darkness"))
	{
		::sprintf(spellCat, "Snare");
		return;
	}
	if (pSpell->SpellIcon == 117)
	{
		::sprintf(spellCat, "Root");
		return;
	}
	if (pSpell->Subcategory == 35 || pSpell->SpellIcon == 25)
	{
		::sprintf(spellCat, "Mez");
		return;
	}
	if (pSpell->Subcategory == 30 || pSpell->SpellIcon == 50)
	{
		::sprintf(spellCat, "Cripple");
		return;
	}
	if (pSpell->SpellIcon == 134 || pSpell->SpellIcon == 16)
	{
		::sprintf(spellCat, "Haste");
		return;
	}
	if (pSpell->SpellIcon == 132 && !strstr(pSpell->Name, "Growth"))
	{
		::sprintf(spellCat, "Aego");
		return;
	}
	if (pSpell->SpellIcon == 131 && pSpell->Subcategory == 46)
	{
		::sprintf(spellCat, "Skin");
		return;
	}
	if (pSpell->SpellIcon == 130 || pSpell->SpellIcon == 133 && strstr(pSpell->Name, "Shield of") || pSpell->SpellIcon == 133 && strstr(pSpell->Name, "Armor of the"))
	{
		::sprintf(spellCat, "Focus");
		return;
	}
	if (pSpell->SpellIcon == 118 && pSpell->Subcategory == 43)
	{
		::sprintf(spellCat, "Regen");
		return;
	}
	if (pSpell->SpellIcon == 150 && pSpell->Subcategory == 112 && pSpell->Category == 45)
	{
		::sprintf(spellCat, "Symbol");
		return;
	}
	if (pSpell->SpellIcon == 21 && !strstr(pSpell->Name, "Knowledge") && !strstr(pSpell->Name, "Recourse") && !strstr(pSpell->Name, "Soothing") && !strstr(pSpell->Name, "Brell") && pSpell->Subcategory == 59 && pSpell->Category == 79)
	{
		::sprintf(spellCat, "Clarity");
		return;
	}
	if (pSpell->SpellIcon == 123 && strstr(pSpell->Name, "of the Predator") || pSpell->SpellIcon == 158 && strstr(pSpell->Name, "Protection of the"))
	{
		::sprintf(spellCat, "Pred");
		return;
	}
	if (pSpell->SpellIcon == 123 && strstr(pSpell->Name, "Strength of the") || pSpell->SpellIcon == 158 && strstr(pSpell->Name, "Protection of the"))
	{
		::sprintf(spellCat, "Strength");
		return;
	}
	if (pSpell->SpellIcon == 90 && strstr(pSpell->Name, "Brell's"))
	{
		::sprintf(spellCat, "Brells");
		return;
	}
	if (pSpell->SpellIcon == 1 && strstr(pSpell->Name, "Spiritual V"))
	{
		::sprintf(spellCat, "SV");
		return;
	}
	if (pSpell->SpellIcon == 72 && strstr(pSpell->Name, "Spiritual E"))
	{
		::sprintf(spellCat, "SE");
		return;
	}
	if (pSpell->SpellIcon == 132 && (strstr(pSpell->Name, "Growth") || strstr(pSpell->Name, "Stance")))
	{
		::sprintf(spellCat, "Growth");
		return;
	}
	if (pSpell->SpellIcon == 70 && strstr(pSpell->Name, "Shining"))
	{
		::sprintf(spellCat, "Shining");
		return;
	}
	if (pSpell->SpellType != 0 && pSpell->Category == 125 && pSpell->Subcategory == 21 && pSpell->TargetType != 6)
	{
		for (int x = 0; x < GetSpellNumEffects(pSpell); x++)
		{
			if (GetSpellAttrib(pSpell, x) == 59)
			{
				::sprintf(spellCat, "DS%d", x + 1);
				return;
			}
		}
	}
	if (HasSpellAttrib(pSpell, 85) && (strstr(pSpell->Name, "Talisman of the ") || strstr(pSpell->Name, "Spirit of the ")) && pSpell->SpellIcon == 2)
	{
		::sprintf(spellCat, "Panther");
		return;
	}
	if (strstr(pSpell->Name, "Ferocity") && pSpell->SpellIcon == 81)
	{
		::sprintf(spellCat, "Fero");
		return;
	}
	if (strstr(pSpell->Name, "Retort") && pSpell->SpellIcon == 2)
	{
		::sprintf(spellCat, "Retort");
		return;
	}
	if (pSpell->Category == 42 && pSpell->Subcategory == 32)
	{
		::sprintf(spellCat, "HoT");
		return;
	}
}

void SpellType(PSPELL pSpell)
{
	if (!InGameOK())
		return;
	if (!pSpell)
		return;
	::sprintf(spellType, "Unknown");
	spellTypeInt = ZERO;
	if ((GetSpellAttribX(pSpell, 0) == 192 && GetSpellBase(pSpell, 0) > 0) || (GetSpellAttribX(pSpell, 4) == 192 && GetSpellBase(pSpell, 4) > 0)) {
		::sprintf(spellType, "Aggro"); spellTypeInt = ::OPTIONS::AGGRO;  return;
	}
	if (pSpell->Category == 132 || pSpell->Subcategory == 132 || pSpell->Category == 71) {
		::sprintf(spellType, "Aura"); spellTypeInt = ::OPTIONS::AURA; return;
	}
	if (pSpell->Category == 13 || pSpell->Subcategory == 13) {
		::sprintf(spellType, "Charm"); spellTypeInt = ::OPTIONS::CHARM;  return;
	}
	if (strstr(pSpell->Name, " Counterbias") || (pSpell->Subcategory == 21 && pSpell->TargetType == 0 || pSpell->Subcategory == 30 || pSpell->Subcategory == 88 || pSpell->Subcategory == 81) && pSpell->Category != 132) {
		::sprintf(spellType, "Debuff"); spellTypeInt = ::OPTIONS::DEBUFF; return;
	}
	if (strstr(pSpell->Name, "Dichotomic Paroxysm") || pSpell->Category == 126 && pSpell->Subcategory == 60 || pSpell->Category == 20 || pSpell->Category == 114 && pSpell->Subcategory == 33 || pSpell->Category == 114 && pSpell->Subcategory == 43 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 0) {
		::sprintf(spellType, "Dot"); spellTypeInt = ::OPTIONS::DOT; return;
	}
	if (strstr(pSpell->Name, "Growl of the") || strstr(pSpell->Name, "Grim Aura") || strstr(pSpell->Name, "Roar of the Lion") || strstr(pSpell->Name, "Chaotic Benefaction") ||
		strstr(pSpell->Name, "Dichotomic Companion") || strstr(pSpell->Name, "Dichotomic Fury") || HasSpellAttrib(pSpell, 323) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61 ||
		pSpell->Category == 45 && pSpell->Subcategory == 141 && pSpell->TargetType == 6 || pSpell->Subcategory == 142 && pSpell->TargetType == 6 && pSpell->Category != 132 ||
		pSpell->Category == 125 && pSpell->Subcategory == 16 && (pSpell->TargetType == 6 || pSpell->TargetType == 41) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61) {
		::sprintf(spellType, "FightBuff"); spellTypeInt = ::OPTIONS::FIGHTBUFF; return;
	}
	if (pSpell->Category == 42 && (pSpell->Subcategory != 19 || strstr(pSpell->Name, "Balm") || strstr(pSpell->Name, "Splash of")) && pSpell->Subcategory != 82 && (pSpell->TargetType == 45 || pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 6 || pSpell->TargetType == 8 || pSpell->TargetType == 41)) {
		::sprintf(spellType, "Heal"); spellTypeInt = ::OPTIONS::HEAL; return;
	}
	//cure has to be after heal else fail
	if (pSpell->Category == 42 && pSpell->Subcategory == 19) {
		::sprintf(spellType, "Cure"); spellTypeInt = ::OPTIONS::CURE; return;
	}
	if (pSpell->Subcategory == 42 && pSpell->Category == 69 || pSpell->Category == 42 && (pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 8)) {
		::sprintf(spellType, "HealPet"); spellTypeInt = ::OPTIONS::HEALPET; return;
	}
	if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0) {
		::sprintf(spellType, "HealToT"); spellTypeInt = ::OPTIONS::HEALTOT; return;
	}
	if (pSpell->Category == 5 && (GetSpellBaseX(pSpell, 0) < 0) || strstr(pSpell->Name, "freeze")) {
		PSPELL pSpell2 = GetSpellByID(GetSpellBase2X(pSpell, 1));
		if (pSpell2 && ((GetSpellAttribX(pSpell2, 2) == 192) || strstr(pSpell->Name, "Concussive ")) && GetCharInfo()->pSpawn->Class == Wizard) {
			::sprintf(spellType, "Jolt");
			spellTypeInt = ::OPTIONS::JOLT;
			return;
		}
	}
	if (strstr(pSpell->Name, "Dichotomic Fang") || pSpell->Category == 114 && (pSpell->Subcategory == 43 || pSpell->Subcategory == 33)) {
		::sprintf(spellType, "Lifetap"); spellTypeInt = ::OPTIONS::LIFETAP; return;
	}
	if (pSpell->Subcategory == 21 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 51 || pSpell->Subcategory == 16 && pSpell->Category != 132 || pSpell->Subcategory == 62 || pSpell->Subcategory == 64 && strstr(pSpell->Name, " Retort") || pSpell->Category == 45 && pSpell->Subcategory == 141 || pSpell->Subcategory == 64 && strstr(pSpell->Name, "Divine I")) {
		::sprintf(spellType, "MainTankBuff"); spellTypeInt = ::OPTIONS::MAINTANKBUFF;  return;
	}
	if (pSpell->Subcategory == 61 || pSpell->Subcategory == 17 && FindSpellInfoForAttrib(pSpell, BASE, 15) > 0 || strstr(pSpell->Name, " Symbiosis")) {
		::sprintf(spellType, "Mana"); spellTypeInt = ::OPTIONS::MANA; return;
	}
	if (pSpell->Subcategory == 35) {
		::sprintf(spellType, "Mez"); spellTypeInt = ::OPTIONS::MEZ; return;
	}
	if (strstr(pSpell->Name, "Dichotomic Fusillade") || strstr(pSpell->Name, "Dichotomic Force") || strstr(pSpell->Name, "Dichotomic Fire") || pSpell->Category == 25 && pSpell->Subcategory != 35) {
		::sprintf(spellType, "Nuke");		spellTypeInt = NUKE; return;
	}
	if (pSpell && pSpell->Autocast == 19780) {
		if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0) {
			::sprintf(spellType, "NukeToT");
			spellTypeInt = ::OPTIONS::NUKETOT;
			return;
		}
		else if (pSpell->TargetType == 5) {
			for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot) {
				if (GetSpellAttrib(pSpell, slot) == 374) {
					PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, slot));
					if (pSpell2 && GetSpellAttribX(pSpell2, 0) == 0 && GetSpellBase(pSpell2, 0) > 0 && pSpell2->TargetType == 46) {
						::sprintf(spellType, "NukeToT");
						spellTypeInt = ::OPTIONS::NUKETOT;
						return;
					}
				}
			}
		}
	}
	if (pSpell->Category == 69 && pSpell->Subcategory > 97 && pSpell->Subcategory < 106 && pSpell->Subcategory != 101 && GetCharInfo()->pSpawn->Class != 12 && GetCharInfo()->pSpawn->Class != 2 && GetCharInfo()->pSpawn->Class != 6) {
		::sprintf(spellType, "Pet");		spellTypeInt = ::OPTIONS::PET; return;
	}
	if (pSpell->Subcategory == 82) {
		::sprintf(spellType, "Rez");		spellTypeInt = ::OPTIONS::REZ; return;
	}
	if (pSpell->Subcategory == 83) {
		::sprintf(spellType, "Root");		spellTypeInt = ::OPTIONS::ROOT; return;
	}
	// skipping SelfBuff for now
	if (pSpell->Subcategory == 89) {
		::sprintf(spellType, "Snare");		spellTypeInt = ::OPTIONS::SNARE; return;
	}
	if (GetSpellAttribX(pSpell, 0) == 32) {
		spellTypeInt = AGGRO;
		::sprintf(spellType, "SummonItem");	spellTypeInt = ::OPTIONS::SUMMONITEM; return;
	}
	if (pSpell->Subcategory == 104 && pSpell->Category == 69 && strstr(pSpell->Extra, "Rk") || pSpell->Subcategory == 99 && (GetCharInfo()->pSpawn->Class == 12 || GetCharInfo()->pSpawn->Class == 2) || pSpell->Subcategory == 139) {
		::sprintf(spellType, "Swarm");		spellTypeInt = ::OPTIONS::SWARM; return;
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
	if (pSpell->Location == 1 && !outdoor)
		return false;
	if (pSpell->SpellType != 0)
		if ((pSpell->Mana < GetCharInfo()->pSpawn->ManaCurrent  &&  GetCharInfo()->pSpawn->ManaMax>0 || pSpell->Mana < 1) && (DistanceToSpawn3D((PSPAWNINFO)pCharSpawn, pSpawn) < pSpell->Range || DistanceToSpawn3D((PSPAWNINFO)pCharSpawn, pSpawn) < pSpell->AERange))
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
	if (pSpell->Location == 1 && !outdoor)
		return false;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(Target->MasterID))
		if (pSpawn->Type == SPAWN_PLAYER && !strstr(EQADDR_SERVERNAME, "Zek"))
			return false;
	char bodyType[MAX_STRING] = "", testBody[MAX_STRING] = "Single";
	bool validDet = true;
	::sprintf(bodyType, "${If[(${Spell[%s].TargetType.Find[%s]}||!${Select[${Spell[%s].TargetType},Animal,Humanoid,Undead,Plant,Summoned,Uber Giants,Uber Dragons,AE Undead,AE Summoned]} && !${Spell[%s].Subcategory.Find[summoned]}||${Spell[%s].TargetType.Find[Undead]} && ${Target.Buff[smoldering bones].ID}||${Spell[%s].Subcategory.Find[summoned]} && ${Select[%d,5,24,27,28]}),1,0]}", pSpell->Name, BodyTypeFix, pSpell->Name, pSpell->Name, pSpell->Name, pSpell->Name, GetBodyType(Target));
	ParseMacroData(bodyType);
	if (atoi(bodyType) == 0 || !outdoor && pSpell->Location == 1)
	{
		validDet = false;
		return validDet;
	}
	if (pSpell->TargetType == 5 && !(CastRay(GetCharInfo()->pSpawn, Target->Y, Target->X, Target->Z)) || pSpell->Mana > GetCharInfo()->pSpawn->ManaCurrent && GetCharInfo()->pSpawn->ManaMax > 0 || DistanceToSpawn3D(GetCharInfo()->pSpawn, Target) > pSpell->Range && pSpell->Range > 0 || DistanceToSpawn3D(GetCharInfo()->pSpawn, Target) > pSpell->AERange && pSpell->Range == 0)
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
	::strcpy(CurrentRoutine, &(__FUNCTION__[6]));
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
		::sprintf(szSpell, "Spell%dName", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpells spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					if (GetCharInfo()->pSpawn->Class == 3) // one off shit that is misnamed.  should probably double check to see if this is fixed now 20160717.
						if (strstr(szTemp, "Inquisitor's Judg"))
							::sprintf(szTemp, "Inquisitor's Judgement");
					::strcpy(spell.SpellName, szTemp);
					spell.CanIReprioritize = 0;
					spell.ID = aa->ID;
					spell.SpellTypeOption = ::OPTIONS::AA;
					spell.CheckFunc = CheckAA;
					::strcpy(spell.Gem, "alt");
					vTemp.push_back(spell);
				}
			}
		}
	}
	int test = 0;
	for (int i = 0; szAA[i]; i++)
	{
		test = 0;
		for (int z = 0; z < vTemp.size(); z++)
		{
			if (!stricmp(vTemp[z].SpellName, szAA[i]))
				test = 1;
		}
		if (!test)
		{
			::sprintf(szTemp, szAA[i]);
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID) && (int)aa->ReuseTimer > 0)
				{
					BotSpells spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					::strcpy(spell.SpellName, szTemp);
					spell.CanIReprioritize = 1;
					spell.Type = TYPE_AA;
					spell.ID = aa->ID;
					spell.SpellTypeOption = ::OPTIONS::AA;
					spell.CheckFunc = CheckAA;
					::strcpy(spell.Gem, "alt");
					vTemp.push_back(spell);
				}
			}
		}
	}
	// load the rest of the AA info
	LoadBotSpell(vTemp, "AA");
	for (int i = 0; i < vTemp.size(); i++)
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
		::sprintf(szSpell, "Spell%dName", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			::sprintf(szSpellGem, "Spell%dGem", i);
			GetPrivateProfileString(INISection, szSpellGem, NULL, szTempGem, MAX_STRING, INIFileName);
			if (stristr(szTempGem, "disc"))
			{
				for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
				{
					if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
						if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
						{
							tempSpell[pSpell->CARecastTimerID] = pSpell;
							tempLevel[pSpell->CARecastTimerID] = pSpell->ClassLevel[GetCharInfo()->pSpawn->Class];
							noOverwrite[pSpell->CARecastTimerID] = 1;
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
				if (!strstr(spellCat, "Summon Weapon") && !strstr(pSpell->Name, "Thief's ") && !strstr(pSpell->Name, "Tireless Sprint") && !strstr(pSpell->Name, "Conditioned Reflexes") && (pSpell->CARecastTimerID != 8 || GetCharInfo()->pSpawn->Class != 16 || strstr(pSpell->Name, "Cry")))
				{
					if ((long)pSpell->CARecastTimerID != -1 && pSpell->ClassLevel[GetCharInfo()->pSpawn->Class] > tempLevel[pSpell->CARecastTimerID] && !noOverwrite[pSpell->CARecastTimerID])
					{
						tempSpell[pSpell->CARecastTimerID] = pSpell;
						tempLevel[pSpell->CARecastTimerID] = pSpell->ClassLevel[GetCharInfo()->pSpawn->Class];
						// adding in low level discs that are the best available
						if (strstr(pSpell->Name, "Twisted Chance") || strstr(pSpell->Name, "Frenzied Stabbing") || strstr(pSpell->Name, "Amplified Frenzy"))
							noOverwrite[pSpell->CARecastTimerID] = 1;
					}
					else if ((long)pSpell->CARecastTimerID == -1)
					{
						tempSpell.push_back(pSpell);
						tempLevel.push_back(pSpell->ClassLevel[GetCharInfo()->pSpawn->Class]);
					}
				}
			}
		}
	}
	// ------------------finalize discs------------------------------
	// first check for if statement for section

	::sprintf(szSpell, "DiscIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("DiscIf: %s", SpellIt->second.c_str());
	}
	// now create and check ini entries on discs
	for (int i = 0; i < tempSpell.size() && i < tempLevel.size(); i++)
	{
		if (tempLevel[i] > 0)
		{
			BotSpells disc;
			disc.Spell = tempSpell[i];
			::strcpy(disc.SpellName, tempSpell[i]->Name);
			disc.Type = TYPE_DISC;
			disc.CheckFunc = CheckDisc;
			disc.SpellTypeOption = ::OPTIONS::DISC;
			disc.ID = tempSpell[i]->ID;
			::strcpy(disc.Gem, "disc");
			vTemp.push_back(disc);
		}

	}
	// load the rest of the disc info
	LoadBotSpell(vTemp, "Disc");
	// find the endurance regen disc
	for (int i = 0; i < vTemp.size(); i++)
	{
		if (strstr(vTemp[i].SpellCat, "End Regen") && !strstr(vTemp[i].SpellName, "Wind"))
		{
			EndRegenSpell = vTemp[i];
			WriteChatf("Endurance Regen Disc: %s", vTemp[i].SpellName);
		}
	}
	for (int i = 0; i < vTemp.size(); i++)
	{
		vMaster.push_back(vTemp[i]);
	}
}
void CreateHeal()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->Class == 11 || GetCharInfo()->pSpawn->Class == 12 || GetCharInfo()->pSpawn->Class == 13 || GetCharInfo()->pSpawn->Class == 14)
		return;
	vTemp.clear();
	::strcpy(CurrentRoutine, &(__FUNCTION__[6]));
	PCHAR szHeal[] = { "Divine Arbitration", "Burst of Life", "Beacon of Life", "Focused Celestial Regeneration", "Celestial Regeneration",
		"Convergence of Spirits", "Union of Spirits", "Focused Paragon of Spirits", "Paragon of Spirit", "Lay on Hands",
		"Hand of Piety", "Ancestral Aid", "Call of the Ancients", "Exquisite Benediction", "Blessing of Tunare", NULL };
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING] = { 0 }, gem[MAX_STRING] = { 0 };
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	int customSpells = GetPrivateProfileInt(INISection, "SpellTotal", 0, INIFileName);
	for (int i = 0; i < customSpells; i++)
	{
		::sprintf(szSpell, "SpellName%d", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpells spell;
					spell.IniMatch = i;
					spell.Spell = GetSpellByID(aa->SpellID);
					::strcpy(spell.SpellName, szTemp);
					spell.CanIReprioritize = 0;
					spell.Type = TYPE_AA;
					spell.CheckFunc = CheckHeal;
					spell.SpellTypeOption = ::OPTIONS::HEAL;
					spell.ID = aa->ID;
					::sprintf(gem, "alt");
					::strcpy(spell.Gem, gem);
					vTemp.push_back(spell);
				}
			}
			else
				for (int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				{
					if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
					{
						if (!stricmp(szTemp, pSpell->Name))
						{
							BotSpells spell;
							spell.IniMatch = i;
							spell.Spell = pSpell;
							::strcpy(spell.SpellName, szTemp);
							spell.CanIReprioritize = 0;
							spell.ID = pSpell->ID;
							spell.CheckFunc = CheckHeal;
							spell.SpellTypeOption = ::OPTIONS::HEAL;
							spell.Type = TYPE_SPELL;
							::sprintf(gem, "%d", DefaultGem);
							::strcpy(spell.Gem, gem);
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
		for (int z = 0; z < vTemp.size(); z++)
		{
			if (!stricmp(vTemp[z].SpellName, szHeal[i]))
				test = 1;
		}
		if (!test)
		{
			::sprintf(szTemp, szHeal[i]);
			aaIndex = GetAAIndexByName(szTemp);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
				{
					BotSpells spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					::strcpy(spell.SpellName, szTemp);
					spell.ID = aa->ID;
					spell.Type = TYPE_AA;
					spell.CanIReprioritize = 1;
					::sprintf(gem, "alt");
					spell.CheckFunc = CheckHeal;
					spell.SpellTypeOption = ::OPTIONS::HEAL;
					::strcpy(spell.Gem, gem);
					vTemp.push_back(spell);
				}
			}
		}
	}
	LoadBotSpell(vTemp, "Heal");
	for (int i = 0; i < vTemp.size(); i++)
	{
		vMaster.push_back(vTemp[i]);
	}
}
#pragma endregion CreateRoutines

#pragma region CheckRoutines
// master check
void CheckMaster()
{
	::strcpy(CurrentRoutine, &(__FUNCTION__[5]));
	CheckMemmedSpells();
	for (int i = 0; i < vMaster.size(); i++)
	{
		if (vMaster[i].CheckFunc != NULL)
			vMaster[i].CheckFunc(i);
	}
}
void CheckAA(int spell)
{
	if (!InGameOK())
		return;
	::strcpy(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("Checking %s: Spell: %s -- Priority: %d", CurrentRoutine, vMaster[spell].SpellName, vMaster[spell].Priority); // test code
	if (vMaster[spell].Spell->SpellType == 0)
		if (!ShouldICastDetrimental(vMaster[spell]))
			return;
	if (vMaster[spell].Spell->SpellType != 0)
		if (SpellStacks(vMaster[spell].Spell) || GetCharInfo()->pSpawn->Class == 16 && strcmp(vMaster[spell].SpellName, "Savage Spirit"))
			return;
	// all the basic checks have cleared, now we need to test the one-offs

	// this check will need readded for AACutOffTime
	// if (valid && (atol(sTest) <= (AACutoffTime * 60) || named && GetSpawnByID((DWORD)atol(sNamed))))  
	if (GetCharInfo()->pSpawn->Class == 16 && (!strcmp(vMaster[spell].SpellName, "Savage Spirit") || !strcmp(vMaster[spell].SpellName, "Untamed Rage")))
	{
		char sstest[MAX_STRING] = "";
		::sprintf(sstest, "${If[${Me.ActiveDisc.ID},1,0]}");
		ParseMacroData(sstest);
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
	::strcpy(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("Checking %s: Spell: %s -- Priority: %d", CurrentRoutine, vMaster[spell].SpellName, vMaster[spell].Priority); // test code
	return;
}
void CheckHeal(int spell)
{
	return;
}
#pragma endregion CheckRoutines

#pragma region Configure
void Configure(char szCustomIni[MAX_STRING], int force)
{
	if (!InGameOK())
		return;
	int i;
	vMaster.clear();
	vMemmedSpells.clear();
	if (vMemmedSpells.size()<12)
	{
		for (int x = 0; x < 11; x++)
		{
			BotSpells spell;
			spell.Spell = NULL;
			spell.ID = 0;
			vMemmedSpells.push_back(spell);
		}
	}
	bool Shrouded;
	char tempINI[MAX_STRING] = { 0 };
	long Class = GetCharInfo()->pSpawn->Class;
	long Races = GetCharInfo2()->Race;
	long Level = GetCharInfo2()->Level;
	::sprintf(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	::sprintf(INISection, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
	Shrouded = GetCharInfo2()->Shrouded;
	if (!Shrouded)
		INISection[strlen(PLUGIN_NAME)] = 0;
	::sprintf(tempINI, INISection);
	if (force && strlen(szCustomIni) > 1 && !strstr(strlwr(szCustomIni), "default"))
		::sprintf(INISection, "%s_%s", tempINI, szCustomIni);
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
	CreateAA();
	CreateDisc();
	//CreateHeal();

	CheckMemmedSpells();
	SortSpellVector(vMemmedSpells);
	ConfigureLoaded = true;
}
void MemmedCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	CheckMemmedSpells();
	SortSpellVector(vMemmedSpells);
}
#pragma endregion Commands

#pragma region Loading
void LoadBotSpell(vector<_BotSpells> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING], color[10] = { 0 };
	int defStartAt = 0, defUse = 0, defStopAt = 0, defPriority = 0, defNamedOnly = 0, defUseOnce = 0, defForceCast = 0;
	for (int x = 0; DefaultSection[x]; x++)
	{
		if (!stricmp(VectorName, DefaultSection[x]))
		{
			::strcpy(color, DefaultColor[x]);
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
	for (int i = 0; i < v.size(); i++)
	{
		if (!stricmp(VectorName, "Disc"))
			DiscCategory(v[i].Spell);
		else
			SpellType(v[i].Spell);
		::strcpy(v[i].SpellCat, spellType);
		::sprintf(szSpell, "SpellIf%d", v[i].IniMatch);
		if (GetPrivateProfileString(INISection, szSpell, "1", szTemp, MAX_STRING, INIFileName))
		{
			::strcpy(v[i].If, szTemp);
		}
		::sprintf(szSpell, "SpellGem%d", v[i].IniMatch);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			::strcpy(v[i].Gem, szTemp);
		}
		::sprintf(szSpell, "SpellUseOnce%d", v[i].IniMatch);
		v[i].UseOnce = GetPrivateProfileInt(INISection, szSpell, defUseOnce, INIFileName);
		::sprintf(szSpell, "SpellForceCast%d", v[i].IniMatch);
		v[i].ForceCast = GetPrivateProfileInt(INISection, szSpell, defForceCast, INIFileName);
		::sprintf(szSpell, "SpellUse%d", v[i].IniMatch);
		v[i].Use = GetPrivateProfileInt(INISection, szSpell, defUse, INIFileName);
		::sprintf(szSpell, "SpellStartAt%d", v[i].IniMatch);
		v[i].StartAt = GetPrivateProfileInt(INISection, szSpell, defStartAt, INIFileName);
		::sprintf(szSpell, "SpellStopAt%d", v[i].IniMatch);
		v[i].StopAt = GetPrivateProfileInt(INISection, szSpell, defStopAt, INIFileName);
		::sprintf(szSpell, "SpellNamedOnly%d", v[i].IniMatch);
		v[i].NamedOnly = GetPrivateProfileInt(INISection, szSpell, defNamedOnly, INIFileName);
		::sprintf(szSpell, "SpellPriority%d", v[i].IniMatch);
		if (v[i].CanIReprioritize)
			v[i].Priority = GetPrivateProfileInt(INISection, szSpell, defPriority, INIFileName);
		v[i].LastTargetID = 0;
		v[i].LastCast = 0;
		v[i].Recast = 0;
		v[i].TargetID = 0;
		::sprintf(szSpell, "SpellDuration%d", v[i].IniMatch);
		v[i].Duration = !GetSpellDuration2(v[i].Spell) ? 0 : 6000 * GetSpellDuration2(v[i].Spell) * (ULONGLONG)CalcDuration(v[i].Spell);
		::strcpy(v[i].Color, color);
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
	PopulateIni(v, VectorName);
}
void PluginOn()
{
	AddCommand("/bot", BotCommand);
	AddCommand("/memmed", MemmedCommand);
}

void PluginOff()
{
	RemoveCommand("/bot");
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
	CheckMaster();
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
	if (CastingE<(long)pData) CastingE = (long)pData;
	DebugWrite("[%llu] MQ2Bot:[OnChat]: Result=[%s (%d)] Called=[%d].", MQGetTickCount64(), cast_status[CastingE], CastingE, (long)pData);
}

void Stick(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *StickCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && strnicmp(pLook->szFilename, "MQ2MoveUtils", 12)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion>0.9999 && pLook->RemoveSpawn)
		if (StickCALL Request = (StickCALL)GetProcAddress(pLook->hModule, "StickCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}

void FollowPath(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion>0.999 && pLook->RemoveSpawn)
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQFollowCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}
void Path(PCHAR zFormat, ...) {
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) pLook = pLook->pNext;
	if (pLook && pLook->fpVersion>0.999 && pLook->RemoveSpawn)
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQPlayCommand"))
			Request(GetCharInfo()->pSpawn, zOutput);
}


long Evaluate(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList); if (!zOutput[0]) return 1;
	ParseMacroData(zOutput); return atoi(zOutput);
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
	if ((pChar->pSpawn->Class)<0 || pChar->pSpawn->Class>17) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - pChar->pSpawn->Class out of range: %d", PLUGIN_NAME, Gem, Spell, pChar->pSpawn->Class);
		return;
	}
	if (Spell->ClassLevel[pChar->pSpawn->Class]>pChar->pSpawn->Level) {
		DebugSpewAlways("%s::MemoLoad(%ld, 0x%08X) - Spell->ClassLevel[pChar->pSpawn->Class] > pChar->pSpawn->Level (%d > %d)", PLUGIN_NAME, Gem, Spell, Spell->ClassLevel[pChar->pSpawn->Class], pChar->pSpawn->Level);
		return;
	}
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++)
		if (SpellToMemorize.SpellId[sp] == Spell->ID)
			SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
	SpellToMemorize.SpellId[((DWORD)Gem<NUM_SPELL_GEMS) ? Gem : 4] = Spell->ID;
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
	strcpy(SpellToMemorize.Name, "Mem a Spell");
	SpellToMemorize.inuse = 1;
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
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
		if (Cast->CastOnYou[0]) {
			sprintf(Temps, "%s#*#", Cast->CastOnYou);
			aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
			Added = true;
		}
		if (Cast->CastOnAnother[0]) {
			sprintf(Temps, "#*#%s#*#", Cast->CastOnAnother);
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
	return (!MQ2Globals::gbMoving && (!ImmobileT || MyTimer>ImmobileT));
}

void StopEnding() {
	if (MoveS != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: Stick UnPause Request.", MQGetTickCount64());
		Stick("unpause");
		MoveS = FLAG_COMPLETE;
	}
	if (MoveA != FLAG_COMPLETE) {
		DebugWrite("[%llu] MQ2Bot:[Immobilize]: AdvPath UnPause Request.", MQGetTickCount64());
		Execute("/varcalc PauseFlag 0");
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
		Execute("/varcalc PauseFlag 1");
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
		for (int sp = 0; sp<NUM_SPELL_GEMS; sp++)
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
				MemoM = MQGetTickCount64() + DELAY_STOP + DELAY_MEMO*SpellTotal;
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
			if (StopE == DONE_ABORTED || MQGetTickCount64()>MemoM)
				MemoE = DONE_ABORTED;
		}
		else {
			if (GEMReady(SP) || MQGetTickCount64()>MemoM) {
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
		Execute("/book");
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
				Execute("/dismount");
			}
		}
		else DuckF = FLAG_PROGRESS2;
	}
	if (DuckF == FLAG_PROGRESS2) {
		DebugWrite("[%llu] MQ2Bot:[Duck]: StopCast.", MQGetTickCount64());
		Execute("/stopcast");
		CastingE = CAST_ABORTED;
		DuckF = FLAG_COMPLETE;
	}
}

void CastHandle(_BotSpells &spell) {

	// we got the casting request cookies, request immobilize/memorize if needed.
	if (CastF == FLAG_REQUEST) {
		DebugWrite("[%llu] MQ2Bot:[Casting]: Request.", MQGetTickCount64());
		CastF = FLAG_PROGRESS1;
		if (StopF == FLAG_COMPLETE) StopF = DONE_SUCCESS;
		if (StopF == FLAG_COMPLETE && CastT>100 && !BardClass) StopF = FLAG_REQUEST;
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
		if (CastE>CastingE) CastingE = CastE;
		CastF = FLAG_COMPLETE;
	}

	// evaluate if we are taking too long, or immobilize/memorize event failed.
	if (CastF != FLAG_COMPLETE) {
		if (StopE == DONE_ABORTED || MemoE == DONE_ABORTED || MQGetTickCount64()>CastM) {
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
			long TimeReady = SpellReady(spell.ID) ? 0 : (long)spell.Spell->FizzleTime;    // get estimate time before it's ready.
			if (TimeReady>3000)  CastE = CAST_NOTREADY;   // if estimate higher than 3 seconds aborts.
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
		if ((long)GetCharInfo()->pSpawn->CastingData.SpellID>0) {
			CastingX = (CastingE<CAST_SUCCESS) ? CAST_SUCCESS : CastingE;
			CastingL = CastingC;
			if (CastK == TYPE_SPELL)     Execute("/multiline ; /stopsong ; /cast \"%s\"", CastN);
			else if (CastK == TYPE_AA) {
				Execute("/alt activate %d", spell.ID);
			}
			else if (CastK == TYPE_ITEM) {
				Execute("/multiline ; /stopsong ; /useitem \"%s\"", CastN);
			}
		}
		else {
			char castN[MAX_STRING];
			::sprintf(castN, "\"%s\"", spell.SpellName);
			if (CastK == TYPE_SPELL)     Cast(GetCharInfo()->pSpawn, castN);
			else if (CastK == TYPE_AA)  Execute("/alt activate %d", spell.ID);
			else if (CastK == TYPE_ITEM) Execute("/multiline ; /stopsong ; /useitem \"%s\"", CastN);
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

void BotCastCommand(_BotSpells &spell) {
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
				CastG = atoi(&spell.Gem[(strnicmp(spell.Gem, "gem", 3)) ? 0 : 3]) - 1;
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
		strcpy(CastN, spell.SpellName);
		DebugWrite("[%llu] MQ2Bot:[Casting]: Name<%s> Type<%d>.", MQGetTickCount64(), CastN, CastK);
		CastHandle(spell);
}
// NEW end functions

#pragma endregion MQ2CastCode
#pragma warning ( pop )
