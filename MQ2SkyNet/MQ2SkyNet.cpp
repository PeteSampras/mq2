// MQ2SkyNet.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.

#pragma region header
//defines
#define MMOBUGS_LOADER
#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index
#define        DEBUGGING             0


#include "../MQ2Plugin.h"
#include "../MMOBugs.h"
#include "../MQ2MMOBugs/CSL.h"
#include <string>
#include <map>
#include <vector>
#include "../Blech/Blech.h"
#include <Windows.h>
#include <cstdio>
using namespace std;
PreSetup("MQ2SkyNet");
PLUGIN_VERSION(1.00);
#pragma endregion header


#pragma region prototypes
// enums
enum ATTRIBSLOTS { ATTRIB, BASE, BASE2, CALC, MAX }; // attrib enum
enum OPTIONS { // used to check routines automatically
	ZERO, AA, AGGRO, AURA, BARD, BUFF, CALL, CHARM, CLICKIES, CLICKYBUFFS, ITEMS,
	CLICKYNUKES, DEBUFF, DOT, ENDURANCE, FADE, FIGHTBUFF, GRAB, HEAL, HEALPET, HEALTOT,
	IMHIT, JOLT, KNOCKBACK, LIFETAP, MAINTANKBUFF, MANA, NUKE, NUKETOT, PET, REZ,
	ROOT, SELFBUFF, SNARE, SUMMONITEM, SWARM, DISC, CURE, MEZ
};
enum CATEGORIES { //used to categorize spells
	Slow, Malo, Tash, Haste, Root, Snare, Mez, DS, RevDS, Cripple,
	Charge, Concuss, MindFroze, Charm, Aego, Skin, Focus, Regen, Symbol, Clarity,
	Pred, Strength, Brells, SV, SE, HybridHP, Growth, Shining, DeepSleep, Hot, Fero,
	CATSIZE
};
enum SKILLTYPES { TYPE_SPELL = 1, TYPE_AA = 2, TYPE_ITEM = 3, TYPE_DISC = 4 }; // struct for spell data, how to use the spell, and how the spell was used

enum SPAWNTYPE { PC = 1, PCPET = 2, MERCENARY = 3, NPC = 4};

char           Sep[8] = { '"',' ','^','|',':','=','\n',0 }; // separators

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

typedef struct NetBuff
{
	PSPELL				Spell;
	ULONGLONG			Expires;
	CATEGORIES			Category;
	OPTIONS				Option;
	int					Amount;

} NetBuffs, *PNetBuffs;

typedef struct _Spawns
{
	NetBuff				Slow;  //track all these
	NetBuff				Malo;
	NetBuff				Tash;
	NetBuff				Haste;
	NetBuff				Root;
	NetBuff				Snare;
	NetBuff				Mez;
	NetBuff				DS;
	NetBuff				RevDS;
	NetBuff				Cripple;
	NetBuff				Charge;
	NetBuff				Concuss;
	NetBuff				MindFroze;
	NetBuff				Charm;
	NetBuff				Aego;
	NetBuff				Skin;
	NetBuff				Focus;
	NetBuff				Regen;
	NetBuff				Symbol;
	NetBuff				Clarity;
	NetBuff				Pred;
	NetBuff				Strength;
	NetBuff				Brells;
	NetBuff				SV;
	NetBuff				SE;
	NetBuff				HybridHP;
	NetBuff				Growth;
	NetBuff				Shining;
	NetBuff				DeepSleep;
	NetBuff				Hot;
	NetBuff				Fero;
	int					PoisonCounters;
	int					DiseaseCounters;
	int					CorruptedCounters;
	int					CurseCounters;
	int					DetrimentalCounters;
	BYTE				State;
	vector<_BotSpells>	vSpellList;
	char				SpawnBuffList[MAX_STRING];
	DWORD				ID;
	bool				Add; // is this a confirmed add?
	int					Priority;
	bool				NeedsCheck;
	ULONGLONG			LastChecked;
	SPAWNTYPE			SpawnType;
} Spawns, *PSpawns;
#pragma endregion prototypes

#pragma region variables
Blech               Packet('#');         // BotInfo Event Triggers
ULONGLONG           sTimers[CATSIZE];      // Save Timers
int					iAmount[CATSIZE];
char                sBuffer[CATSIZE][MAX_STRING]; // Save Buffer
char                wBuffer[CATSIZE][MAX_STRING]; // Work Buffer
bool                wChange[CATSIZE];      // Work Change
bool                wUpdate[CATSIZE];      // Work Update

//vectors
vector<_Spawns> vGroup, vRaid, vEQBC, vFriends, vAdds;

//bools
bool DEBUG_DUMPFILE = false, InCombat=false, bSummoned=false, bEnabled = false;

//chars
char szCurrentRoutine[MAX_STRING] = { 0 }, szConColor[MAX_STRING] = { 0 }, szAddList[MAX_STRING] = { 0 }, szBodyTypeFix[MAX_STRING] = { 0 };

//DWORDS
DWORD xNotTargetingMe, xTargetingMe, xLastBodyID;

//floats
float fFightX = 0, fFightY = 0, fFightZ = 0, fWarpX = 0, fWarpY = 0, fWarpZ = 0, fWarpDistance = 0;

#pragma endregion variables

#pragma region settings
int UseNetBots = 0;
char NetBotsName[MAX_STRING] = "NULL";
char AssistName[MAX_STRING] = { 0 };
int	AssistAt = 100;
int	AssistRange = 100;

int AnnounceEQBC = 0;
int AnnounceEcho = 1;
int AnnounceAdds = 1;
#pragma endregion settings


#pragma region EQBC
//EQBC related code to send information

bool EQBCConnected() {
	typedef WORD(__cdecl *fEqbcIsConnected)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fEqbcIsConnected checkf = (fEqbcIsConnected)GetProcAddress(pLook->hModule, "isConnected"))
			if (checkf()) return true;
	return false;
}

//need to add NetBotSendMsg equiv routine to eqbc
void EQBCBroadCast(PCHAR Buffer) {
	typedef VOID(__cdecl *fEqbcNetBotSendMsg)(PCHAR);
	if (strlen(Buffer) > 9) {
		PMQPLUGIN pLook = pPlugins;
		while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
		if (pLook)
			if (fEqbcNetBotSendMsg requestf = (fEqbcNetBotSendMsg)GetProcAddress(pLook->hModule, "NetBotSendMsg")) {
#if    DEBUGGING>1
				DebugSpewAlways("%s->BroadCasting(%s)", PLUGIN_TITLE, Buffer);
#endif DEBUGGING
				requestf(Buffer);
			}
	}
}

bool NetBotsEnabled() {
	typedef WORD(__cdecl *fNetBotsEnabled)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2netbots", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNetBotsEnabled checkf = (fNetBotsEnabled)GetProcAddress(pLook->hModule, "NetBotsEnabled"))
			if (checkf()) return true;
	return false;
}

PLUGIN_API bool SkyNetEnabled() // Used to check if NetBots is Enabled
{
	return bEnabled;
}

/* NOTE: This all needs to be customized but it is pretty straight forward
void BroadCast() {

Slow, Malo, Tash, Haste, Root, Snare, Mez, DS, RevDS, Cripple,
Charge, Concuss, MindFroze, Charm, Aego, Skin, Focus, Regen, Symbol, Clarity,
Pred, Strength, Brells, SV, SE, HybridHP, Growth, Shining, DeepSleep, Hot, Fero,
CATSIZE
	char Buffer[MAX_STRING];
	long nChange = false;
	long nUpdate = false;
	ZeroMemory(wBuffer, sizeof(wBuffer));
	ZeroMemory(wChange, sizeof(wChange));
	ZeroMemory(wUpdate, sizeof(wUpdate));
	sprintf_s(wBuffer[BUFFS], "B=%s|", MakeBUFFS(Buffer));
	sprintf_s(wBuffer[CASTD], "C=%s|", MakeCASTD(Buffer));
	if (!bExtended)
		sprintf_s(wBuffer[ENDUS], "E=0|");
	else
		sprintf_s(wBuffer[ENDUS], "E=%s|", MakeENDUS(Buffer));
	sprintf_s(wBuffer[EXPER], "X=%s|", MakeEXPER(Buffer));
	sprintf_s(wBuffer[LEADR], "N=%s|", MakeLEADR(Buffer));
	sprintf_s(wBuffer[LEVEL], "L=%s|", MakeLEVEL(Buffer));
	sprintf_s(wBuffer[LIFES], "H=%s|", MakeLIFES(Buffer));
	sprintf_s(wBuffer[MANAS], "M=%s|", MakeMANAS(Buffer));
	sprintf_s(wBuffer[PBUFF], "W=%s|", MakePBUFF(Buffer));
	sprintf_s(wBuffer[PETIL], "P=%s|", MakePETIL(Buffer));
	if (!bExtended)
		sprintf_s(wBuffer[SPGEM], "G=0|");
	else
		sprintf_s(wBuffer[SPGEM], "G=%s|", MakeSPGEM(Buffer));
	sprintf_s(wBuffer[SONGS], "S=%s|", MakeSONGS(Buffer));
	sprintf_s(wBuffer[STATE], "Y=%s|", MakeSTATE(Buffer));
	sprintf_s(wBuffer[TARGT], "T=%s|", MakeTARGT(Buffer));
	sprintf_s(wBuffer[ZONES], "Z=%s|", MakeZONES(Buffer));
	if (!bExtended)
		sprintf_s(wBuffer[DURAS], "D=0|");
	else
		sprintf_s(wBuffer[DURAS], "D=%s|", MakeDURAS(Buffer));
	sprintf_s(wBuffer[AAPTS], "A=%s|", MakeAAPTS(Buffer));
	sprintf_s(wBuffer[OOCST], "O=%s|", MakeOOCST(Buffer));
	sprintf_s(wBuffer[NOTE], "U=%s|", MakeNOTE(Buffer));
	sprintf_s(wBuffer[DETR], "R=%s|", MakeDETR(Buffer));
	sprintf_s(wBuffer[LOCAT], "@=%s|", MakeLOCAT(Buffer));
	sprintf_s(wBuffer[HEADN], "$=%s|", MakeHEADN(Buffer));
	sprintf_s(wBuffer[MSTATE], "&=%s|", MakeMSTATE(Buffer));
	sprintf_s(wBuffer[MNAME], "^=%s|", MakeMNAME(Buffer));
	sprintf_s(wBuffer[NAVACT], "<=%s|", MakeNAVACT(Buffer));
	sprintf_s(wBuffer[NAVPAU], ">=%s|", MakeNAVPAU(Buffer));
	sprintf_s(wBuffer[NVERS], "V=%s|", MakeNVERS(Buffer));

	//  WriteChatf("D=%s|", Buffer);
	for (int i = 0; i < ESIZE; i++)
		if ((clock() > sTimers[i] && clock() > sTimers[i] + UPDATES) || 0 != strcmp(wBuffer[i], sBuffer[i])) {
			wChange[i] = true;
			nChange++;
		}
		else if (clock() < sTimers[i] && clock() + UPDATES > sTimers[i]) {
			wUpdate[i] = true;
			nUpdate++;
		}
		if (nChange) {
			strcpy_s(Buffer, "[NB]|");
			for (int i = 0; i < ESIZE; i++)
				if (wChange[i] || wUpdate[i] && (strlen(Buffer) + strlen(wBuffer[i])) < MAX_STRING - 5) {
					strcat_s(Buffer, wBuffer[i]);
					sTimers[i] = (long)clock() + REFRESH;
				}
			strcat_s(Buffer, "[NB]");
			//   WriteChatf("Broadcast %s", Buffer);
			EQBCBroadCast(Buffer);
			memcpy(sBuffer, wBuffer, sizeof(wBuffer));
			if (CurBot = BotLoad(GetCharInfo()->Name)) {
				Packet.Feed(Buffer);
				CurBot->Updated = clock();
				CurBot = 0;
			}
		}
}
//This will change to on OnNetEvent or something
PLUGIN_API VOID OnNetBotEVENT(PCHAR Msg) {
	if (!mmo.Active)
		return;
#if    DEBUGGING>0
	DebugSpewAlways("%s->OnNetBotEVENT(%s)", PLUGIN_TITLE, Msg);
#endif DEBUGGING
	if (!strncmp(Msg, "NBQUIT=", 7))      BotQuit(&Msg[7]);
	else if (!strncmp(Msg, "NBJOIN=", 7)) ZeroMemory(sTimers, sizeof(sTimers));
	else if (!strncmp(Msg, "NBEXIT", 6))  NetMap.clear();
}
//This will change to on OnNetMSG or something
PLUGIN_API VOID OnNetBotMSG(PCHAR Name, PCHAR Msg) {
	if (!mmo.Active)
		return;
	if (NetStat && NetGrab && !strncmp(Msg, "[NB]|", 5) &&
		GetCharInfo() && GetCharInfo()->pSpawn && _stricmp(GetCharInfo()->Name, Name)) {
#if    DEBUGGING>1
		DebugSpewAlways("%s->OnNetBotMSG(From:[%s] Msg[%s])", PLUGIN_TITLE, Name, Msg);
#endif DEBUGGING
		CHAR szCmd[MAX_STRING] = { 0 };
		strcpy_s(szCmd, Msg);
		if (CurBot = BotLoad(Name)) {
			Packet.Feed(szCmd);
			CurBot->Updated = clock();
			CurBot = 0;
		}
	}
}
*/
#pragma endregion EQBC

#pragma region utility

void ConColorSwap(PSPAWNINFO pSpawn)
{
	if (!pSpawn)
		return;
	if (ConColor(pSpawn) == CONCOLOR_GREY)
		strcpy_s(szConColor, "\a-w");
	if (ConColor(pSpawn) == CONCOLOR_GREEN)
		strcpy_s(szConColor, "\ag");
	if (ConColor(pSpawn) == CONCOLOR_LIGHTBLUE)
		strcpy_s(szConColor, "\at");
	if (ConColor(pSpawn) == CONCOLOR_BLUE)
		strcpy_s(szConColor, "\au");
	if (ConColor(pSpawn) == CONCOLOR_WHITE)
		strcpy_s(szConColor, "\aw");
	if (ConColor(pSpawn) == CONCOLOR_YELLOW)
		strcpy_s(szConColor, "\ay");
	if (ConColor(pSpawn) == CONCOLOR_RED)
		strcpy_s(szConColor, "\ay");
}

// Returns TRUE if character is in game and has valid character data structures
inline bool InGameOK()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}
PMQPLUGIN Plugin(PCHAR PluginName)
{
	long Length = strlen(PluginName) + 1;
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(PluginName, pLook->szFilename, Length))
		pLook = pLook->pNext;
	return pLook;
}
/* NOTE: Parsing will need changed to my final format, this will mirror the PluginOn() code
void __stdcall ParseInfo(unsigned int ID, void *pData, PBLECHVALUE pValues) {
	if (CurBot) while (pValues) {
		//     WriteChatf("Parsing=%s", pValues->Name);
		switch (atoi(pValues->Name)) {
		case  1: CurBot->ZoneID = atol(pValues->Value);        break;
		case  2: CurBot->InstID = atol(pValues->Value);        break;
		case  3: CurBot->SpawnID = atol(pValues->Value);        break;
		case  4: CurBot->Level = atol(pValues->Value);        break;
		case  5: CurBot->ClassID = atol(pValues->Value);        break;
		case  6: CurBot->LifeCur = atol(pValues->Value);        break;
		case  7: CurBot->LifeMax = atol(pValues->Value);        break;
		case  8: if (!bExtended) CurBot->EnduCur = 0; else CurBot->EnduCur = atol(pValues->Value);        break;
		case  9: if (!bExtended) CurBot->EnduMax = 0; else CurBot->EnduMax = atol(pValues->Value);        break;
		case 10: CurBot->ManaCur = atol(pValues->Value);        break;
		case 11: CurBot->ManaMax = atol(pValues->Value);        break;
		case 12: CurBot->PetID = atol(pValues->Value);        break;
		case 13: CurBot->PetHP = atol(pValues->Value);        break;
		case 14: CurBot->TargetID = atol(pValues->Value);        break;
		case 15: CurBot->TargetHP = atol(pValues->Value);        break;
		case 16: CurBot->CastID = atol(pValues->Value);        break;
		case 17: CurBot->State = (WORD)atol(pValues->Value);  break;
		case 18: CurBot->XP = (DWORD)atol(pValues->Value); break;
		case 19: CurBot->aaXP = (DWORD)atol(pValues->Value); break;
#ifdef EMU
		case 20: CurBot->glXP = atof(pValues->Value);        break;
#endif
		case 21: CurBot->FreeBuff = atol(pValues->Value);        break;
		case 22: strcpy_s(CurBot->Leader, pValues->Value);         break;
		case 30: InfoGems(pValues->Value);                      break;
		case 31: InfoBuff(pValues->Value);                      break;
		case 32: InfoSong(pValues->Value);                      break;
		case 33: InfoPets(pValues->Value);                      break;
		case 34: InfoDura(pValues->Value);                     break;
		case 35: CurBot->TotalAA = atol(pValues->Value);        break;
		case 36: CurBot->UsedAA = atol(pValues->Value);        break;
		case 37: CurBot->UnusedAA = atol(pValues->Value);        break;
		case 38: CurBot->CombatState = atol(pValues->Value);      break;
		case 39: strcpy_s(CurBot->Note, pValues->Value);	      break;
		case 40: InfoDetr(pValues->Value);                      break;
		case 89: strcpy_s(CurBot->Location, pValues->Value);		  break;
		case 90: strcpy_s(CurBot->Heading, pValues->Value);		  break;
		case 91: CurBot->MacroState = atoi(pValues->Value);    break;
		case 92: strcpy_s(CurBot->MacroName, pValues->Value);  break;
		case 93: CurBot->NavActive = atol(pValues->Value) ? true : false;  break;
		case 94: CurBot->NavPaused = atol(pValues->Value) ? true : false;  break;
		case 95: CurBot->Version = atof(pValues->Value); break;
		}
		pValues = pValues->pNext;
	}
}
*/
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
		size_t thelen = len + 1000;
		if (char *szOutput = (char *)LocalAlloc(LPTR, thelen)) {
			vsprintf_s(szOutput, thelen, szFormat, vaList);
			FILE *fOut = NULL;
			CHAR Filename[MAX_STRING] = { 0 };
			CHAR Name[256] = "Unknown";
			if (PCHARINFO pCharInfo = GetCharInfo())
			{
				strcpy_s(Name, pCharInfo->Name);
			}
			sprintf_s(Filename, "%s\\MQ2SkyNet_%s_%s.log", gszLogPath, EQADDR_SERVERNAME, Name);
			ChkCreateDir(gszLogPath);
			errno_t err = fopen_s(&fOut, Filename, "at");
			if (err)
			{
				DEBUG_DUMPFILE = false;
				FatalError("[%s] Severe Error: DebugWrite() couldn't open %s (%d) - debugging disabled.", PLUGIN_NAME, Filename, err);
				LocalFree(szOutput);
				return;
			}
			if (char *PlainText = (char *)LocalAlloc(LPTR, strlen(szOutput) + MAX_STRING))
			{
				StripMQChat(szOutput, PlainText);
				strcpy_s(szOutput, thelen, PlainText);
				for (unsigned int i = 0; i < strlen(szOutput); i++) {
					if (szOutput[i] == 0x07) {
						if ((i + 2) < strlen(szOutput))
							strcpy_s(&szOutput[i], thelen, &szOutput[i + 2]);
						else
							szOutput[i] = 0;
					}
				}
				LocalFree(PlainText);
			}
			char szTemp[128];
			struct tm today;
			time_t tm;
			tm = time(NULL);
			localtime_s(&today, &tm);
			strftime(szTemp, 128, "%Y/%m/%d %H:%M:%S", &today);
			fprintf(fOut, "[%s] - %s\n", szTemp, szOutput);
			fclose(fOut);
			LocalFree(szOutput);
		}
	}
	catch (...)
	{
		DEBUG_DUMPFILE = false;
		FatalError("[%s] Severe Error: DebugWrite() - debugging disabled.", PLUGIN_NAME);
	}
}
#pragma endregion utility

bool sortSpawnByPriority(const Spawns &lhs, const Spawns &rhs) { return lhs.Priority > rhs.Priority; } // Sort spawn vectors by priority

void SortSpawnVector(vector<_Spawns> &v)  // the actual sorting of a spawn vector by priority
{
	sort(v.begin(), v.end(), sortSpawnByPriority);
}

void AddSpawnMember(_Spawns &pSpawn,CATEGORIES pCategory,ULONGLONG pValue)
{
	
}

#pragma region CheckFunctions
void CheckAdds()
{
	if (!InGameOK())
		return;
	if (!GetCharInfo()->pXTargetMgr)
		return;
	char szXTAR[MAX_STRING];
	char szXTAR2[MAX_STRING];
	char testAddList[MAX_STRING];
	if(!vAdds.empty())
	{
		int iSize = vAdds.size();
		for (int i = iSize; i != 0; i--)
		{
			if (!GetSpawnByID(vAdds[i].ID))
				vAdds.erase(vAdds.begin() + i);
		}
	}
	if (ExtendedTargetList* etl = GetCharInfo()->pXTargetMgr) //check extended target list
	{
		InCombat = false;
		int leastaggro = 100;
		int mostaggro = 99;
		xNotTargetingMe = NULL;
		xTargetingMe = NULL;
		if (((PCPLAYERWND)pPlayerWnd)) //check combat swords window
			if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
				if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
					InCombat = true;

		for (int n = 0; n < etl->XTargetSlots.Count; n++) //run through xtarget
		{
			XTARGETSLOT& slot = etl->XTargetSlots[n];

			if (slot.SpawnID > 0)
			{
				if (slot.xTargetType)
					sprintf_s(szXTAR, "%s", GetXtargetType(slot.xTargetType));
				PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(slot.SpawnID);
				if (pSpawn)
				{
					if (pSpawn && (strstr(szXTAR, "NPC") || strstr(szXTAR, "Hater") || (strstr(szXTAR, "Target") || strstr(szXTAR, "target") || strstr(szXTAR, "_TARGET") && !strstr(szXTAR, "EMPTY")) && InCombat && pSpawn->Type == SPAWN_NPC && pSpawn->Type != SPAWN_CORPSE))
					{
						bool bKnown = false;
						_Spawns newAdd;
						newAdd.ID = pSpawn->SpawnID;
						if(!vAdds.size()) //if no adds, then add it
						{
							vAdds.push_back(newAdd);
							// need if BroadCast() here to announce add
						}
						else
						{
							int iSize = vAdds.size();
							for(int i = 0 ; i<iSize; i++)
							{
								if (vAdds[i].ID == newAdd.ID)
									bKnown = true;
							}
							if (!bKnown) //if not known, add the spawn
							{
								_Spawns newAdd;
								newAdd.ID = pSpawn->SpawnID;
								vAdds.push_back(newAdd);
								// need if BroadCast() here to announce add
							}
						}
					}
					if (pAggroInfo)  //check aggro data to see if something is hitting me
					{
						if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct < leastaggro)
						{
							leastaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
							xNotTargetingMe = pSpawn->SpawnID;
						}
						if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct > mostaggro && !xTargetingMe)
						{
							mostaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
							xTargetingMe = pSpawn->SpawnID;
						}
					}
				}
			}
		}
	}
	/* NOTE: Commenting this out because I dont want to actually /assist currently, but maybe later
	if (UseManualAssist && strlen(AssistName)>3)
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
			InCombat = false;
			if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
			{
				if (pMyTarget->Type == SPAWN_NPC)
				{
					if (((PCPLAYERWND)pPlayerWnd))
						if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
							if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
								InCombat = true;
					if (InCombat || DistanceToSpawn(pChar->pSpawn, pMyTarget) < AssistRange && PctHP(pMyTarget)<AssistAt)
						vAdds.push_back(pMyTarget);
					else
						vPossibleAdds.push_back(pMyTarget);
				}
			}
		}
		
	}
	*/
	if (UseNetBots)
	{
		sprintf_s(testAddList, "${If[${NetBots[%s].InZone} && ${NetBots.Client.Find[%s]} && ${NetBots[%s].TargetID} && ${NetBots[%s].TargetHP}<=%d && (${NetBots[%s].CombatState}==0||${NetBots[%s].Attacking}) && ${Spawn[id ${NetBots[%s].TargetID}].Distance}<=%d,1,0]}", NetBotsName, NetBotsName, NetBotsName, NetBotsName, AssistAt, NetBotsName, NetBotsName, NetBotsName, AssistRange);
		ParseMacroData(testAddList, MAX_STRING);
		if (atoi(testAddList) == 1)
		{
			sprintf_s(testAddList, "${NetBots[%s].TargetID}", NetBotsName);
			ParseMacroData(testAddList, MAX_STRING);
			unsigned int cap = vAdds.size();
			bool bKnown = false;
			for (unsigned int i = 0; i < cap; i++)
			{
				if (vAdds[i].ID == (DWORD)atol(testAddList))
					bKnown = true;
			}
			if (!bKnown)
			{
				_Spawns pSpawn;
				pSpawn.ID = (DWORD)atol(testAddList);
				vAdds.push_back(pSpawn);
			}
				
		}
	}
	if (!vAdds.empty() && AnnounceAdds > 0)
	{
		strcpy_s(szXTAR2, "|");
		for (unsigned int i = 0; i < vAdds.size(); i++)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO(GetSpawnByID(vAdds[i].ID))))
			{
				if (vAdds[i].ID && pSpawn->Type != SPAWN_CORPSE)
				{
					ConColorSwap(pSpawn);
					sprintf_s(szXTAR, "%s%s\ar|", szConColor, pSpawn->Name);
					strcat_s(szXTAR2, szXTAR);
				}
			}
		}
		strcpy_s(testAddList, szAddList);
		if (strcmp(szAddList, szXTAR2))
		{
			strcpy_s(szAddList, szXTAR2);
			WriteChatf("\arAdds: %s", szXTAR2);
		}
		if (fFightX == 0 && fFightY == 0 && fFightZ == 0)
		{
			fFightX = GetCharInfo()->pSpawn->X;
			fFightY = GetCharInfo()->pSpawn->Y;
			fFightZ = GetCharInfo()->pSpawn->Z;
		}
	}
	if (!vAdds.empty())
	{
		if (GetSpawnByID(vAdds[0].ID) && vAdds[0].ID != xLastBodyID)
		{
			sprintf_s(szBodyTypeFix, "${Spawn[%s].Body}", (PSPAWNINFO(GetSpawnByID(vAdds[0].ID))->Name));
			ParseMacroData(szBodyTypeFix, MAX_STRING);
			xLastBodyID = vAdds[0].ID;
		}

	}
	if (vAdds.empty())
	{
		bSummoned = false;
		fFightX = 0;
		fFightY = 0;
		fFightZ = 0;
		fWarpDistance = 0;
	}
}

void CheckGroup()
{
	if (!InGameOK())
		return;
	try
	{
		strcpy_s(szCurrentRoutine, MAX_STRING, &(__FUNCTION__[5]));
		DebugWrite("Checking %s", szCurrentRoutine); // test code
		PCHARINFO pChar = GetCharInfo();
		if (pChar && !pChar->pGroupInfo)
		{
			_Spawns gMember;
			gMember.ID = pChar->pSpawn->SpawnID;
			gMember.State = SPAWN_PLAYER;
			vGroup[0] = gMember;
			return;
		}
		PSPAWNINFO pGroupMember;
		char test[MAX_STRING];
		for (int i = 0; i < 6; i++)
		{
			//DebugWrite("Checking %s", szCurrentRoutine); // test code
			sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
				{
					if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->Mercenary)
					{
						SPAWNTYPE sType;
						if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER)
							sType = SPAWNTYPE::PC;
						if (pChar->pGroupInfo->pMember[i]->Mercenary)
							sType = SPAWNTYPE::MERCENARY;
						pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
						if (pGroupMember && (pGroupMember->RespawnTimer || pGroupMember->StandState == STANDSTATE_DEAD))
						{
							//if (vPets[i].ID != 0)
							//	CheckGroupPets(i); //add this later
							_Spawns gMember;
							gMember.ID = pGroupMember->SpawnID;
							gMember.State = STANDSTATE_DEAD;
							gMember.SpawnType = sType;
							vGroup[i] = gMember;
						}
						if (pGroupMember && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
						{
							if (pGroupMember->SpawnID != vGroup[i].ID)
							{
								_Spawns gMember;
								gMember.ID = pGroupMember->SpawnID;
								gMember.State = SPAWN_PLAYER;
								gMember.SpawnType = sType;
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
		DebugSpewAlways("MQ2SkyNet::CheckGroup() **Exception**");
	}
	//DebugWrite("Checking %s", szCurrentRoutine); // test code
	return;
}
#pragma endregion CheckFunctions

void PluginOn()
{
	if(mmo.Active)
		return;
	mmo.Active = true;
    //Add commands, MQ2Data items, hooks, etc.
    //AddCommand("/mycommand",MyCommand);
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
	/* NOTE: This parse info needs to match the final mapping
	Packet.Reset();
	NetMap.clear();
	Packet.AddEvent("#*#[NB]#*#|Z=#1#:#2#>#3#|#*#[NB]", ParseInfo, (void *)3);
	Packet.AddEvent("#*#[NB]#*#|L=#4#:#5#|#*#[NB]", ParseInfo, (void *)5);
	Packet.AddEvent("#*#[NB]#*#|H=#6#/#7#|#*#[NB]", ParseInfo, (void *)7);
	Packet.AddEvent("#*#[NB]#*#|E=#8#/#9#|#*#[NB]", ParseInfo, (void *)9);
	Packet.AddEvent("#*#[NB]#*#|M=#10#/#11#|#*#[NB]", ParseInfo, (void *)11);
	Packet.AddEvent("#*#[NB]#*#|P=#12#:#13#|#*#[NB]", ParseInfo, (void *)13);
	Packet.AddEvent("#*#[NB]#*#|T=#14#:#15#|#*#[NB]", ParseInfo, (void *)15);
	Packet.AddEvent("#*#[NB]#*#|C=#16#|#*#[NB]", ParseInfo, (void *)16);
	Packet.AddEvent("#*#[NB]#*#|Y=#17#|#*#[NB]", ParseInfo, (void *)17);
#ifdef EMU
	Packet.AddEvent("#*#[NB]#*#|X=#18#:#19#:#20#|#*#[NB]", ParseInfo, (void *)20);
#else
	Packet.AddEvent("#*#[NB]#*#|X=#18#:#19#|#*#[NB]", ParseInfo, (void *)19);
#endif
	Packet.AddEvent("#*#[NB]#*#|F=#21#:|#*#[NB]", ParseInfo, (void *)21);
	Packet.AddEvent("#*#[NB]#*#|N=#22#|#*#[NB]", ParseInfo, (void *)22);
	Packet.AddEvent("#*#[NB]#*#|G=#30#|#*#[NB]", ParseInfo, (void *)30);
	Packet.AddEvent("#*#[NB]#*#|B=#31#|#*#[NB]", ParseInfo, (void *)31);
	Packet.AddEvent("#*#[NB]#*#|S=#32#|#*#[NB]", ParseInfo, (void *)32);
	Packet.AddEvent("#*#[NB]#*#|W=#33#|#*#[NB]", ParseInfo, (void *)33);
	Packet.AddEvent("#*#[NB]#*#|D=#34#|#*#[NB]", ParseInfo, (void *)34);
	Packet.AddEvent("#*#[NB]#*#|A=#35#:#36#:#37#|#*#[NB]", ParseInfo, (void *)37);
	Packet.AddEvent("#*#[NB]#*#|O=#38#|#*#[NB]", ParseInfo, (void *)38);
	Packet.AddEvent("#*#[NB]#*#|U=#39#|#*#[NB]", ParseInfo, (void *)39);
	Packet.AddEvent("#*#[NB]#*#|R=#40#|#*#[NB]", ParseInfo, (void *)40);
	Packet.AddEvent("#*#[NB]#*#|@=#89#|#*#[NB]", ParseInfo, (void *)89);
	Packet.AddEvent("#*#[NB]#*#|$=#90#|#*#[NB]", ParseInfo, (void *)90);
	Packet.AddEvent("#*#[NB]#*#|&=#91#|#*#[NB]", ParseInfo, (void *)91);
	Packet.AddEvent("#*#[NB]#*#|^=#92#|#*#[NB]", ParseInfo, (void *)92);
	Packet.AddEvent("#*#[NB]#*#|<=#93#|#*#[NB]", ParseInfo, (void *)93);
	Packet.AddEvent("#*#[NB]#*#|>=#94#|#*#[NB]", ParseInfo, (void *)94);
	Packet.AddEvent("#*#[NB]#*#|V=#95#|#*#[NB]", ParseInfo, (void *)95);
	*/
}

void PluginOff()
{
	if(!mmo.Active)
		return;
	mmo.Active = false;
    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
    //RemoveCommand("/mycommand");
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	if(!GetProcAddress(ghModule, "MMOVersion"))
	{
		char szBuffer[MAX_STRING] = {0};
		sprintf_s(szBuffer, "/timed 10 /plugin %s unload", PLUGIN_NAME);
		EzCommand(szBuffer);
		return;
	}
	DebugSpewAlways("Initializing MQ2SkyNet");
	MMORequiredAccess = AL_Premium;
	if(LOK(MMORequiredAccess))
		PluginOn();
	else
		WriteChatf("\ar%s \aysubscription is required to use \ag%s", MMOAccessName[MMORequiredAccess], PLUGIN_NAME);
	InitLib(PLUGIN_NAME);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2SkyNet");
	PluginOff();
	FreeLib(PLUGIN_NAME);
}

PLUGIN_API VOID OnBeginZone(VOID) 
{ 
}

PLUGIN_API VOID OnEndZone(VOID) 
{ 
	if(LOK(MMORequiredAccess))
		PluginOn();
	else
		PluginOff();
} 

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
	DebugSpewAlways("MQ2SkyNet::OnZoned()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
	DebugSpewAlways("MQ2SkyNet::OnCleanUI()");
	// destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
	DebugSpewAlways("MQ2SkyNet::OnReloadUI()");
	// recreate custom windows, etc
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
	// DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2SkyNet::OnDrawHUD()");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	DebugSpewAlways("MQ2SkyNet::SetGameState()");
    //if (GameState==GAMESTATE_INGAME)
	// create custom windows if theyre not set up, etc
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	// DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2SkyNet::OnPulse()");
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	if(!mmo.Active)
		return 0;
	DebugSpewAlways("MQ2SkyNet::OnWriteChatColor(%s)",Line);
	return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if(!mmo.Active)
		return 0;
	DebugSpewAlways("MQ2SkyNet::OnIncomingChat(%s)",Line);
	return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	DebugSpewAlways("MQ2SkyNet::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	DebugSpewAlways("MQ2SkyNet::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
	DebugSpewAlways("MQ2SkyNet::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
	DebugSpewAlways("MQ2SkyNet::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}

// Mq2nav hooks
bool NavInitialized() {
	typedef WORD(__cdecl *fNavInitialized)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavInitialized checkf = (fNavInitialized)GetProcAddress(pLook->hModule, "NavInitialized"))
			if (checkf()) return true;
	return false;
}

bool NavMeshLoaded() {
	typedef WORD(__cdecl *fNavMeshLoaded)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavMeshLoaded checkf = (fNavMeshLoaded)GetProcAddress(pLook->hModule, "NavMeshLoaded"))
			if (checkf()) return true;
	return false;
}
bool NavPathActive() {
	typedef WORD(__cdecl *fNavPathActive)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavPathActive checkf = (fNavPathActive)GetProcAddress(pLook->hModule, "NavPathActive"))
			if (checkf()) return true;
	return false;
}
bool NavPathPaused() {
	typedef WORD(__cdecl *fNavPathPaused)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavPathPaused checkf = (fNavPathPaused)GetProcAddress(pLook->hModule, "NavPathPaused"))
			if (checkf()) return true;
	return false;
}
bool NavPossible(PCHAR szLine) {
	typedef WORD(__cdecl *fNavPossible)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavPossible checkf = (fNavPossible)GetProcAddress(pLook->hModule, "NavPossible"))
			if (checkf()) return true;
	return false;
}
float NavPathLength(PCHAR szLine) {
	typedef WORD(__cdecl *fNavPathLength)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavPathLength checkf = (fNavPathLength)GetProcAddress(pLook->hModule, "NavPathLength"))
			if (checkf()) return true;
	return false;
}
bool NavCommand(PSPAWNINFO pChar, PCHAR szLine) {
	typedef WORD(__cdecl *fNavCommand)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2nav", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fNavCommand checkf = (fNavCommand)GetProcAddress(pLook->hModule, "NavCommand"))
			if (checkf()) return true;
	return false;
}
//============================================================================
