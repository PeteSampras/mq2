// MQ2Bot.cpp : Defines the entry point for the DLL application.
// Author: PeteSampras
// Trusty side-kick: htw
#pragma region Headers
// Comment out MMOBUGS_LOADER to not use MMOBugs specific code
#define MMOBUGS_LOADER
// Can safely ignore these warnings
#include "../MQ2Plugin.h"
#ifdef MMOBUGS_LOADER
#include "../MMOBugs.h"
#ifdef MMOBUGS_LOADER
#include "../MQ2MMOBugs/CSL.h"
#endif
#endif
#include "../moveitem2.h"
#include "../Blech/Blech.h"
#include <vector>
#include <map>
#include <iterator>
#include <deque>
#include <Windows.h>
#include <stdio.h>
using namespace std;
#pragma endregion Headers

PreSetup("MQ2Bot");

#pragma region DefinesMacros
#define TargetIT(X)			*(PSPAWNINFO*)ppTarget=X
#define ADD_HP vAdds[0]->HPCurrent*100/vAdds[0]->HPMax
#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index

#define MAX_CLICKIES 20
#pragma endregion DefinesMacros

#pragma region NotUsed
////information sharing feasibility
//#define        GEMS_MAX              NUM_SPELL_GEMS
//#define        BUFF_MAX              NUM_BUFF_SLOTS
//#define        SONG_MAX              NUM_SHORT_BUFFS
//#define        PETS_MAX              NUM_BUFF_SLOTS
//
//enum { STATE_DEAD =0x0001, STATE_FEIGN =0x0002, STATE_DUCK  =0x0004, STATE_BIND =0x0008,
//	STATE_STAND=0x0010, STATE_SIT   =0x0020, STATE_MOUNT =0x0040, STATE_INVIS=0x0080,
//	STATE_LEV  =0x0100, STATE_ATTACK=0x0200, STATE_MOVING=0x0400, STATE_STUN =0x0800,
//	STATE_RAID =0x1000, STATE_GROUP =0x2000, STATE_LFG   =0x4000, STATE_AFK  =0x8000 };
//
//enum { BUFFS,CASTD,ENDUS,EXPER,LEADR,LEVEL,LIFES,MANAS, 
//	PBUFF,PETIL,SPGEM,SONGS,STATE,TARGT,ZONES,DURAS, 
//	AAPTS,OOCST,ESIZE }; 
//class BotInfo {
//public:
//	CHAR              Name[0x40];          // Client NAME
//	CHAR              Leader[0x40];        // Leader Name
//	WORD              State;               // State
//	long              ZoneID;              // Zone ID
//	long              InstID;              // Instance ID
//	long              SpawnID;             // Spawn ID
//	long              ClassID;             // Class ID
//	long              Level;               // Level
//	long              CastID;              // Casting Spell ID
//	long              LifeCur;             // HP Current
//	long              LifeMax;             // HP Maximum
//	long              EnduCur;             // ENDU Current
//	long              EnduMax;             // ENDU Maximum
//	long              ManaCur;             // MANA Current
//	long              ManaMax;             // MANA Maximum
//	long              PetID;               // PET ID
//	long              PetHP;               // PET HP Percentage
//	long              TargetID;            // Target ID
//	long              TargetHP;            // Target HP Percentage
//	long              Gem[GEMS_MAX];       // Spell Memorized
//	long              Pets[PETS_MAX];      // Spell Pets
//	long              Song[SONG_MAX];      // Spell Song
//	long              Buff[BUFF_MAX];      // Spell Buff
//	long              Duration[BUFF_MAX];  // Buff duration 
//	long              FreeBuff;            // FreeBuffSlot;
//	double            glXP;                // glXP
//	DWORD             aaXP;                // aaXP
//	DWORD             XP;                  // XP
//	DWORD             Updated;             // Update
//	long              TotalAA;             // totalAA 
//	long              UsedAA;              // usedAA 
//	long              UnusedAA;            // unusedAA 
//	DWORD             CombatState;         // CombatState 
//};
//
//long                NetInit=0;           // Plugin Initialized?
//long                NetStat=0;           // Plugin is On?
//long                NetGrab=0;           // Grab Information?
//long                NetSend=0;           // Send Information?
//long                NetLast=0;           // Last Send Time Mark
//
//map<string,BotInfo> NetMap;              // BotInfo Mapped List
//Blech               Packet('#');         // BotInfo Event Triggers
//BotInfo            *CurBot=0;            // BotInfo Current
////=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//
//bool EQBCConnected() {
//	typedef WORD (__cdecl *fEqbcIsConnected)(VOID);
//	PMQPLUGIN pLook=pPlugins;
//	while(pLook && _strnicmp(pLook->szFilename,"mq2eqbc",8)) pLook=pLook->pNext;
//	if(pLook)
//		if(fEqbcIsConnected checkf=(fEqbcIsConnected)GetProcAddress(pLook->hModule,"isConnected"))
//			if(checkf()) return true;
//	return false;
//}
//
//void EQBCBroadCast(PCHAR Buffer) {
//	typedef VOID (__cdecl *fEqbcNetBotSendMsg)(PCHAR);
//	if(strlen(Buffer)>9) {
//		PMQPLUGIN pLook=pPlugins;
//		while(pLook && _strnicmp(pLook->szFilename,"mq2eqbc",8)) pLook=pLook->pNext;
//		if(pLook)
//			if(fEqbcNetBotSendMsg requestf=(fEqbcNetBotSendMsg)GetProcAddress(pLook->hModule,"NetBotSendMsg")) {
//#if    DEBUGGING>1
//				DebugSpewAlways("%s->BroadCasting(%s)",PLUGIN_TITLE,Buffer);
//#endif DEBUGGING
//				requestf(Buffer);
//			}
//	}
//}
//
//BotInfo* BotFind(PCHAR Name) {
//	map<string,BotInfo>::iterator f=NetMap.find(Name);
//	return(NetMap.end()==f)?NULL:&(*f).second;
//}
//
//BotInfo* BotLoad(PCHAR Name) {
//	map<string,BotInfo>::iterator f=NetMap.find(Name);
//	if(NetMap.end()==f) {
//		BotInfo RecInfo;
//		ZeroMemory(&RecInfo.Name,sizeof(BotInfo));
//		strcpy_s(RecInfo.Name,Name);
//		NetMap.insert(map<string,BotInfo>::value_type(RecInfo.Name,RecInfo));
//		f=NetMap.find(Name);
//	}
//	return &(*f).second;
//}
//
//void BotQuit(PCHAR Name) {
//	map<string,BotInfo>::iterator f=NetMap.find(Name);
//	if(NetMap.end()!=f) NetMap.erase(f);
//}
//
//void __stdcall ParseInfo(unsigned int ID, void *pData, PBLECHVALUE pValues) {
//	if(CurBot) while(pValues) {
//		//     WriteChatf("Parsing=%s", pValues->Name);
//		switch(atoi(pValues->Name)) {
//		case  1: CurBot->ZoneID   =atol(pValues->Value);        break;
//		case  2: CurBot->InstID   =atol(pValues->Value);        break;
//		case  3: CurBot->SpawnID  =atol(pValues->Value);        break;
//		case  4: CurBot->Level    =atol(pValues->Value);        break;
//		case  5: CurBot->ClassID  =atol(pValues->Value);        break;
//		case  6: CurBot->LifeCur  =atol(pValues->Value);        break;
//		case  7: CurBot->LifeMax  =atol(pValues->Value);        break;
//		case  8: if(!bExtended) CurBot->EnduCur=0; else CurBot->EnduCur  =atol(pValues->Value);        break;
//		case  9: if(!bExtended) CurBot->EnduMax=0; else CurBot->EnduMax  =atol(pValues->Value);        break;
//		case 10: CurBot->ManaCur  =atol(pValues->Value);        break;
//		case 11: CurBot->ManaMax  =atol(pValues->Value);        break;
//		case 12: CurBot->PetID    =atol(pValues->Value);        break;
//		case 13: CurBot->PetHP    =atol(pValues->Value);        break;
//		case 14: CurBot->TargetID =atol(pValues->Value);        break;
//		case 15: CurBot->TargetHP =atol(pValues->Value);        break;
//		case 16: CurBot->CastID   =atol(pValues->Value);        break;
//		case 17: CurBot->State    =(WORD)atol(pValues->Value);  break;
//		case 18: CurBot->XP       =(DWORD)atol(pValues->Value); break;
//		case 19: CurBot->aaXP     =(DWORD)atol(pValues->Value); break;
//		case 20: CurBot->glXP     =(float)atof(pValues->Value); break;
//		case 21: CurBot->FreeBuff =atol(pValues->Value);        break;
//		case 22: strcpy_s(CurBot->Leader,pValues->Value);         break;
//		case 30: InfoGems(pValues->Value);                      break;
//		case 31: InfoBuff(pValues->Value);                      break;
//		case 32: InfoSong(pValues->Value);                      break;
//		case 33: InfoPets(pValues->Value);                      break;
//		case 34: InfoDura(pValues->Value);                     break;
//		case 35: CurBot->TotalAA  =atol(pValues->Value);        break; 
//		case 36: CurBot->UsedAA   =atol(pValues->Value);        break; 
//		case 37: CurBot->UnusedAA =atol(pValues->Value);        break; 
//		case 38: CurBot->CombatState=atol(pValues->Value);      break; 
//		}
//		pValues=pValues->pNext;
//	}
//}
#pragma endregion NotUsed

#pragma region Prototypes
DWORD		ChkCreateDir(char* pszDir);
VOID		DebugWrite(PCHAR szFormat, ...);
bool		GetVersionInfo(char *buffer, int len = MAX_STRING);
void		ShowVersion();
void		CheckAlways();
char*		GetBuild();
int			AddCountByDistance(int rangeCheck);
int			CalcDuration(PSPELL pSpell);
void		CheckAlias();
void		CreatePetGear();
bool		CheckIVUStatus();
bool		CheckInvisStatus();
void		CheckDots();
void		CheckTimer();
void		Configure(char szCustomIni[MAX_STRING], int force);
void		DiscCategory(PSPELL pSpell);
void		EQBCSwap(char startColor[MAX_STRING]);
enum		ATTRIBSLOTS { ATTRIB, BASE, BASE2, CALC, MAX };
int			FindSpellAttrib(PSPELL pSpell, int attrib);
bool		HasSpellAttrib(PSPELL, int attrib);
int			FindSpellInfoForAttrib(PSPELL pSpell, ATTRIBSLOTS what, int attrib);
ULONGLONG	MQGetTickCount64();
bool		GroupFarthest(int rangeCheck);
bool		GroupMinHP(int rangeCheck);
bool		GroupMinPetHP(int rangeCheck);
bool		GroupMinMana(int rangeCheck);
int			HPCount(int HPLevel);
bool		NetBotsMinHP(int rangeCheck);
int			PctEnd(PSPAWNINFO pSpawn);
int			PctHP(PSPAWNINFO pSpawn);
int			PctMana(PSPAWNINFO pSpawn);
ULONGLONG	SpawnTimer(PSPAWNINFO pSpawn);
void		SpellCategory(PSPELL pSpell);
bool		XTMinHP(int rangeCheck);
bool		XTMinPetHP(int rangeCheck);
bool		XTMinMana(int rangeCheck);
bool		XTPriority(int rangeCheck);
double		botround(double d);
void		Test();
void		UnloadSpells();
bool		ValidDet(PSPELL pSpell, PSPAWNINFO Target);
float		Face(PSPAWNINFO pSpawn);
float		FaceMe(PSPAWNINFO pSpawn);
DWORD		GetSpellDuration2(PSPELL pSpell);
void		SpawnTrack(PSPAWNINFO pSpawn);
#pragma endregion Prototypes

#pragma region Variables
long				CycleDelay = 3;
long				Pulses = 0;	// Pulses Skipped Counter
long				zoneID = 0;	// Set zone check

								// spell/spawn related vectors and timers

vector<PSPELL>		vBuff, vHealToT, vNukeToT, vDisc, vTwistSpell, vBuffQ, vSummonItems, vGrab, vKnockback, vAggro, vCharm,
vCall, vRez, vHeal, vSelfBuff, vFade, vJolt, vNuke, vSwarm, vDebuff, vMez, vRoot, vSnare, vFightBuff,
vMainTankBuff, vPetBuff, vPet, vMana, vEndurance, vAura, vAuraEffect, vDot, vLifetap, vHealPet, vTimerSpell, vClickySpell,
vItemsSpell, vCrescendo, vCure;
vector<string>		vAggroName, vImHit, vNamed, vClicky, vAA, vItems, vInvis, vInvisUndead, vLevitate, vPetGearNeed, vPetGearHave, vCauldron,
vDiscType, vTells, vManaName, vSelfBuffName, vHealName, vHealPetName, vDebuffName, vNukeName, vFightBuffName, vSwarmName, vMainTankBuffName,
vSnareName, vPetBuffName, vDotName, vFadeName, vGrabName, vJoltName, vKnockbackName, vLifetapName, vMezName, vRootName, vCallName, vRezName, vHealToTName, vCureName, vBuffType, vBuffName;
vector<PSPAWNINFO>	vTimerSpawn, vBuffQSpawn, vSpawnInZone;
vector<INT>			vTwist, vClickyPrestige;
vector<ULONGLONG>	vTimerTimestamp, vRezTimeStamp, vSpawnInZoneTimer, vCallTimeStamp;
deque<PSPAWNINFO>	vAdds, vPossibleAdds;

ULONGLONG			SelfBuffTimer = 0, MTBTimer = 0, DoorTimer = 0, PetGearTimer = 0, TwistTimer = 0, SpellTimer = 0, TargetTimer = 0,
ClickyTimer = 0, AuraTimer = 0, ManaTimer = 0, MedTimer = 0, modrodTimer = 0, modrodUseTimer = 0, mercTimer = 0, LootTimer = 0,
ImHitTimer = 0, StunTimer = 0, RezTimer = 0, CallTimer = 0, WarpActionTimer = 0, WarpTimer = 0, BardMezTimer = 0, AssistTimer = 0,
CampfireTimer = 0, ZoneTimer = 0, LastAnnouncedSpell = 0, HealTimer = 0, ImHitTimeStamp = 0, DiscTimer = 0,
PetAttackTimer = 0, HateTimer = 0, SummonTimer = 0, PullTimer = 0, modrodAITimer = 0, modrodRecheckTimer = 0,
SpawnTrackGroupTimer = 0, BuffTimer = 0;

// misc variables that are set in the routines
DWORD		Shrouded = false, LastBodyID;
PSPAWNINFO	pCharFix, gMinMana, gMinHP, gMinPetHP, KillTarget = NULL, BodyTypeSpawn = NULL, xMinHP, xMinPetHP, xMinMana, xPriority,
nMinHP, nMinPetHP, CharmMob, gCotH, pFindMob, xNotTargetingMe, xTargetingMe, pPullMob;
PSPELL		petSpell, customPetSpell = NULL, modrodSpell1 = NULL, modrodSpell2 = NULL, modrodSpellAlt = NULL, EndRegenSpell = NULL, pPullSpell;
PALTABILITY invisSpell, ivuSpell, levitateSpell;
bool		cauldron = false, outside = true, ImmunityCheck = true, twisting = false, camping = false, InvisCheck, IVUCheck,
InCombat, zoning, DoingFeignDeath = false, BardClass = false, CombatTwisting = false, looting = false,
LastMezSingle = false, LastSnareSingle = false, MacroPulling = false, summoned = false, bAmIPuller=false, bCanIPull, bEndClass = false,
bPulling, bReturning, PausedWarp, ShouldIExit, bVersionShown = false, DEBUG_DUMPFILE = false;
char		BodyTypeFix[MAX_STRING] = { 0 }, AddList[MAX_STRING] = { 0 }, EQBCColor[MAX_STRING] = { 0 }, MercName[MAX_STRING] = { 0 },
LastRezSpawn[MAX_STRING] = { 0 }, modrodAlt[MAX_STRING] = { 0 }, modrod1[MAX_STRING] = { 0 }, modrod2[MAX_STRING] = { 0 },
INISection[MAX_STRING] = { 0 }, ImmuneINISection[MAX_STRING] = { 0 }, ImmuneINIFileName[MAX_PATH] = { 0 },
IgnoreINISection[MAX_STRING] = { 0 }, MezImmuneINISection[MAX_STRING] = { 0 }, SnareImmuneINISection[MAX_STRING] = { 0 },
IgnoreList[MAX_STRING] = { 0 }, MezImmuneList[MAX_STRING] = { 0 }, SnareImmuneList[MAX_STRING] = { 0 },
CharmImmuneINISection[MAX_STRING] = { 0 }, CharmImmuneList[MAX_STRING] = { 0 }, Spire[MAX_STRING] = { 0 },
CurrentRoutine[MAX_STRING] = { 0 }, IgnoreBaneStrike[MAX_STRING] = "|", spellCat[MAX_STRING] = { 0 },
twistList[MAX_STRING] = { 0 }, conColor[MAX_STRING] = { 0 }, myModRod[MAX_STRING] = { 0 }, AttackCommand[MAX_STRING] = { 0 },
pullif[MAX_STRING] = { 0 }, campSpot[MAX_STRING] = { 0 }, PullSkill[MAX_STRING] = { 0 }, LastTell[MAX_STRING] = { 0 };
float		reinforcement = 0.00, benDurExtend = 0.00, detDurExtend = 0.00, benRngExtend = 1.00, detRngExtend = 1.00, facing,
fPullX, fPullY, fPullZ, fCampX, fCampY, fCampZ, FightX = 0, FightY = 0, FightZ = 0, WarpX = 0, WarpY = 0, WarpZ = 0,
WarpDistance = 0;
int			hotExtend = 0, dotExtend = 0, Bot = -1, AddHP, NetBotsHP, invis, ivu, addSize, petGearSize = 0, FindDoor = 0,
CampfireStep = 0, songCurrent = 0, songNext = 0, songPrev = 0, OffTank, activated, AnnounceBehaviorChange,
DoDebug, PullingDistance, PullSkillDistance, PetReagentCheck, PauseOnWarp, SafetyInvisOnWarp, SafetyCheck,
HPcount, gAvgMana, gAvgHP, MQ2NavDistance;
long		INDEX, mercTgtID;
#pragma endregion Variables

#pragma region Settings
double botVersion = 1.0; // This is auto-detected by the GetBuild() function
int DefaultGem = 1;
int AACutoffTime = 10; //in minutes
int AAStopAt = 20;
int AggroAt = 70;
int	DotAt = 99;
int	DebuffAdds = 1;
int	DebuffAt = 100;
int	DotStopAt = 10;
int	DebuffStopAt = 10;
int FadeAt = 20;
int GrabAdds = 0;
int GrabPrimary = 0;
int	HealAt = 80;
int	HealToTAt = 80;
int	NukeToTAt = 90;
int InterruptToHealAt = 0;
int HealDurationAt = 95;
int HealDelayedAt = 95;
int HealGroupAt = 80;
int HealSelfAt = 80;
int	HealPetAt = 80;
int ImHitAt = 50;
int JoltAt = 70;
int KnockbackAdds = 0;
int KnockbackPrimary = 0;
int	LifetapAt = 80;
int LootRadius = 0;
int LootInCombat = 0;
int	ManaAt = 80;
int	NukeAt = 99;
int RootAdds = 15;
int RootPrimary = 0;
int	SnareAt = 10;
int	SwarmAt = 99;
int UseAENukes = 1;
int	UseAura = 1;
int	UseClickyBuffs = 1;
int	UseClickyNukes = 1;
int UseDiscsOffensive = 0;
int UseDiscsDefensive = 0;
int	UseFightBuffs = 1;
int	UseMainTankBuffs = 1;
int UseManaInCombat = 1;
int UseManaOnOthers = 1;
int UseMez = 1;
int	iUsePet = 1;
int	UsePetBuffs = 1;
int StandOnPetAttack = 0;
int RezRadius = 100;
int	UseSelfBuffs = 1;
int AutoFollowLeader = 0;
int AutoInvisDistance = 0;
int ManaToSummon = 100;
int AlwaysCheckAdds = 0;
int DebuffAEAmount = 3;
int RepositionIfSummoned = 0;
char CustomPetSpell[MAX_STRING] = "NULL";
char PetIllusionItem[MAX_STRING] = "NULL";
PSPELL PetIllusionSpell = NULL;
char AutoFollowSettings[MAX_STRING] = "30 healer";
char BuffPassword[MAX_STRING] = "NULL";
int UseNetBots = 0;
char NetBotsName[MAX_STRING] = "NULL";
char RestTwist[MAX_STRING] = {0};
char CombatTwist[MAX_STRING] = {0};
char RestrictedZones[MAX_STRING] = "344,202,151,345";
char AuthorizedUsers[MAX_STRING] = "|guild|group|netbots|";
char BotCommandsOn[MAX_STRING] = {0};
char BotCommandsOff[MAX_STRING] = {0};

// Safety
char SafetyCommandOnTell[MAX_STRING] = { 0 }, SafetyCommandOnGM[MAX_STRING] = { 0 }, SafetyCommandOnMyWarp[MAX_STRING] = { 0 },
SafetyCommandOnOthersWarp[MAX_STRING] = { 0 }, SafetyCommandOnTooFarFromCamp[MAX_STRING] = { 0 };

// Buffs
char DSClasses[MAX_STRING], HasteClasses[MAX_STRING], AegoClasses[MAX_STRING], SkinClasses[MAX_STRING],
SymbolClasses[MAX_STRING], FocusClasses[MAX_STRING], RegenClasses[MAX_STRING], ClarityClasses[MAX_STRING],
PredClasses[MAX_STRING], StrengthClasses[MAX_STRING], BrellsClasses[MAX_STRING], SVClasses[MAX_STRING],
SEClasses[MAX_STRING], FeroClasses[MAX_STRING];
// Assist settings
int MyClass;
int UseManualAssist = 0;
char AssistName[MAX_STRING] = {0};
int	AssistAt = 100;
int	AssistRange = 100;
int DoPetSwarm = 0;
int	PetAttackAt = 100;
int	PetAttackRange = 60;
int MeleeAt = 0;
int MeleeDistance = 40;
int AutofireAt = 0;
int AutofireDistance = 100;
int	UseAutoFire = 0;
int MercAssistAt = 100;
int MercAssistRange = 0;
//general settings
int AnnounceEQBC = 0;
int AnnounceEcho = 1;
int AnnounceAdds = 1;
int MedStartAt = 0;
int MedStopAt = 0;
int MedToFull = 0;
int MedDuringCombat = 0;
int EndMedStartAt = 0;
int EndMedStopAt = 0;
int EndMedToFull = 0;
int MinManaToNuke = 0;
bool Medding = false;
bool EndMedding = false;
int FaceAngle = 360;
int DebuffsSpellFirst = 0;
int SummonModRods = 0;
int UseModRods = 0;
#pragma endregion SettingsVars


#pragma region UnusedTLOandData
/*
struct _ADDINFO {
DWORD id;  //id of spawn
ULONGLONG Slowed;  //mq2 time stamp of when slow should last until, repeat et al for rest
ULONGLONG Malod;
ULONGLONG Tashed;
ULONGLONG Hasted;
ULONGLONG Rooted;
ULONGLONG Snared;
ULONGLONG Mezzed;
ULONGLONG DSed;
ULONGLONG RevDSed;
ULONGLONG Crippled;
ULONGLONG Charged;
ULONGLONG Concussed;
ULONGLONG MindFroze;
ULONGLONG Charmed;
};

struct _ADDINFO AddArray[20];

class MQ2AddType *pAddTypes=nullptr;
class MQ2AddType : public MQ2Type {
private:
char Tempos[MAX_STRING];
public:
static enum AddMembers
{
Slowed=1,
Malod=2,
Tashed=3,
Hasted=4,
Rooted=5,
Snared=6,
Mezzed=7,
DSed=8,
RevDSed=9,
Crippled=10,
Charged=11,
Concussed=12,
MindFroze=13,
Charmed=14,
id=15,
};
static enum AddMethods
{
};
MQ2AddType():MQ2Type("Add")
{
TypeMember(Slowed);
TypeMember(Malod);
TypeMember(Tashed);
TypeMember(Hasted);
TypeMember(Rooted);
TypeMember(Snared);
TypeMember(Mezzed);
TypeMember(DSed);
TypeMember(RevDSed);
TypeMember(Crippled);
TypeMember(Charged);
TypeMember(Concussed);
TypeMember(MindFroze);
TypeMember(Charmed);
TypeMember(id);
}

~MQ2AddType()
{
}

bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
PMQ2TYPEMEMBER pMember=MQ2AddType::FindMember(Member);
#define pAdd ((_ADDINFO*)VarPtr.Ptr)
if(pMember)
switch((AddMembers)pMember->ID) {

case Slowed:
if(pAdd->Slowed<MQGetTickCount64())
Dest.UInt64=0LL;
else
Dest.UInt64=pAdd->Slowed;
Dest.Type=pInt64Type;
return true;
case Malod:
if(pAdd->Malod<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Malod;
Dest.Type = pInt64Type;
return true;
case Tashed:
if(pAdd->Tashed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Tashed;
Dest.Type = pInt64Type;
return true;
case Hasted:
if(pAdd->Hasted<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Hasted;
Dest.Type = pInt64Type;
return true;
case Rooted:
if(pAdd->Rooted<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Rooted;
Dest.Type = pInt64Type;
return true;
case Snared:
if(pAdd->Snared<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Snared;
Dest.Type = pInt64Type;
return true;
case Mezzed:
if(pAdd->Mezzed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Mezzed;
Dest.Type = pInt64Type;
return true;
case DSed:
if(pAdd->DSed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->DSed;
Dest.Type = pInt64Type;
return true;
case RevDSed:
if(pAdd->RevDSed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->RevDSed;
Dest.Type = pInt64Type;
return true;
case Crippled:
if(pAdd->Crippled<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Crippled;
Dest.Type = pInt64Type;
return true;
case Charged:
if(pAdd->Charged<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Charged;
Dest.Type = pInt64Type;
return true;
case Concussed:
if(pAdd->Concussed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Concussed;
Dest.Type = pInt64Type;
return true;
case MindFroze:
if(pAdd->Malod<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Malod;
Dest.Type = pInt64Type;
return true;
case Charmed:
if(pAdd->Charmed<MQGetTickCount64())
Dest.UInt64 = 0LL;
else
Dest.UInt64 = pAdd->Charmed;
Dest.Type = pInt64Type;
return true;
case id:
if(!GetSpawnByID(pAdd->id))
Dest.DWord=0;
else
Dest.DWord=pAdd->id;
Dest.Type=pIntType;
return true;
}
strcpy_s(Tempos,"NULL");
Dest.Type=pStringType;
Dest.Ptr=Tempos;
return true;
}
//DECLAREGETMETHOD();

INHERITINDIRECT(pSpawnType,Temp.Ptr=GetAddInfo(ObjectData.DWord),0);

bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
strcpy_s(Destination,"TRUE");
return true;
}

bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source)
{
if (Source.Type!=pAddTypes)
return false;
VarPtr.Ptr=Source.Ptr;
return true;
}
bool FromString(MQ2VARPTR &VarPtr, PCHAR Source)
{
return false;
}
};
*/
#pragma endregion UnusedTLOandData

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

#pragma region TLO
class MQ2BotType *pBotTypes = nullptr;
class MQ2BotType : public MQ2Type
{
private:
	char Tempos[MAX_STRING];
public:
	enum BotMembers
	{
		Adds = 1,
		Heal,
		Nuke,
		Dot,
		Swarm,
		HealPet,
		Aggro,
		Rez,
		SelfBuff,
		Fade,
		Jolt,
		Debuff,
		Mez,
		Root,
		Snare,
		FightBuff,
		MainTankBuff,
		PetBuff,
		Pet,
		Mana,
		Endurance,
		Buff,
		Aura,
		Lifetap,
		PossibleAdds,
		Clicky,
		AA,
		MinHP,
		MinPetHP,
		MinMana,
		ValidDetSpell,
		SpellGem,
		SpellETA,
		CastReady,
		Slowed,
		Rooted,
		Mezzed,
		Crippled,
		Malod,
		Tashed,
		Snared,
		Hasted,
		Skin,
		Focus,
		Regen,
		Symbol,
		Clarity,
		Pred,
		Strength,
		Brells,
		SV,
		SE,
		HybridHP,
		Growth,
		Shining,
		DeepSleep,
		Aego,
		CurrentSub,
		Merc,
		Named,
		QuestMob,
		MezImmune,
		SnareImmune,
		Ignore,
		XMinHP,
		XMinPetHP,
		XMinMana,
		XPriority,
		Facing,
		FacingMe,
		NetBotMinHP,
		ImHit,
		InvisSpell,
		IVUSpell,
		Levitate,
		Ignores,
		MezImmunes,
		SnareImmunes,
		CharmImmunes,
		CharmImmune,
		IVU,
		Invis,
		Freeze,
		Twincast,
		GoM,
		DA,
		Twist,
		TwistList,
		Twisting,
		TwistLast,
		TwistCurrent,
		TwistNext,
		Body,
		SpawnTime,
		DiscCat,
		Farthest,
		Restricted,
		SpellCat,
		AddByID,
		AddCount,
		Active,
		Outside,
		ZoneTimeSec,
		ZoneTimeMin,
		ZoneTimeHour,
		Version,
		FileVersion,
		LastHit,
		Zoning,
		Instance,
		SpellDuration,
		INI,
		BuffList,
		TargetBuffList,
		PetBuffList,
		Disc,
		HoT,
		HPLessThan,
		Knockback,
		Grab,
		Tells,
		HaveBuff,
		Status,
		AvgMana,
		AvgHP,
		TargetingMe,
		NotTargetingMe,
		UsePet,
		Pulling,
		Returning,
		PullMob,
		NukeToT,
		HealToT,
		Call,
		FindItem,
	};
	MQ2BotType() :MQ2Type("Bot")
	{
		TypeMember(Adds);
		TypeMember(Heal);
		TypeMember(Nuke);
		TypeMember(Dot);
		TypeMember(Swarm);
		TypeMember(HealPet);
		TypeMember(Aggro);
		TypeMember(Rez);
		TypeMember(SelfBuff);
		TypeMember(Fade);
		TypeMember(Jolt);
		TypeMember(Debuff);
		TypeMember(Mez);
		TypeMember(Root);
		TypeMember(Snare);
		TypeMember(FightBuff);
		TypeMember(MainTankBuff);
		TypeMember(PetBuff);
		TypeMember(Pet);
		TypeMember(Mana);
		TypeMember(Endurance);
		TypeMember(Buff);
		TypeMember(Aura);
		TypeMember(Lifetap);
		TypeMember(PossibleAdds);
		TypeMember(Clicky);
		TypeMember(AA);
		TypeMember(MinHP);
		TypeMember(MinPetHP);
		TypeMember(MinMana);
		TypeMember(ValidDetSpell);
		TypeMember(SpellGem); //gem of currently playing song/spell, if none, then you arent singing
		TypeMember(SpellETA); //ETA of when spell will finish
		TypeMember(CastReady); //can cast spell gem?
		TypeMember(Slowed);
		TypeMember(Rooted);
		TypeMember(Mezzed);
		TypeMember(Crippled);
		TypeMember(Malod);
		TypeMember(Tashed);
		TypeMember(Snared);
		TypeMember(Hasted);
		TypeMember(Aego);
		TypeMember(Skin);
		TypeMember(Focus);
		TypeMember(Regen);
		TypeMember(Symbol);
		TypeMember(Clarity);
		TypeMember(Pred);
		TypeMember(Strength);
		TypeMember(Brells);
		TypeMember(SV);
		TypeMember(SE);
		TypeMember(HybridHP);
		TypeMember(Growth);
		TypeMember(Shining);
		TypeMember(DeepSleep);
		TypeMember(CurrentSub);
		TypeMember(Merc);
		TypeMember(Named);
		TypeMember(QuestMob);
		TypeMember(MezImmune);
		TypeMember(SnareImmune);
		TypeMember(Ignore);
		TypeMember(XMinHP);
		TypeMember(XMinPetHP);
		TypeMember(XMinMana);
		TypeMember(XPriority);
		TypeMember(Facing);
		TypeMember(FacingMe);
		TypeMember(NetBotMinHP);
		TypeMember(Rez);
		TypeMember(ImHit);
		TypeMember(InvisSpell);
		TypeMember(IVUSpell);
		TypeMember(Levitate);
		TypeMember(Ignores);
		TypeMember(MezImmunes);
		TypeMember(SnareImmunes);
		TypeMember(CharmImmunes);
		TypeMember(CharmImmune);
		TypeMember(IVU);
		TypeMember(Invis);
		TypeMember(Freeze);
		TypeMember(Twincast);
		TypeMember(GoM);
		TypeMember(DA);
		TypeMember(Twist);
		TypeMember(TwistList);
		TypeMember(Twisting);
		TypeMember(TwistLast);
		TypeMember(TwistCurrent);
		TypeMember(TwistNext);
		TypeMember(Body);
		TypeMember(SpawnTime);
		TypeMember(DiscCat);
		TypeMember(Farthest);
		TypeMember(Restricted);
		TypeMember(SpellCat);
		TypeMember(AddByID);
		TypeMember(AddCount);
		TypeMember(Active);
		TypeMember(Outside);
		TypeMember(ZoneTimeSec);
		TypeMember(ZoneTimeMin);
		TypeMember(ZoneTimeHour);
		TypeMember(Version);
		TypeMember(FileVersion);
		TypeMember(LastHit);
		TypeMember(Zoning);
		TypeMember(Instance);
		TypeMember(SpellDuration);
		TypeMember(INI);
		TypeMember(BuffList);
		TypeMember(TargetBuffList);
		TypeMember(PetBuffList);
		TypeMember(Disc);
		TypeMember(HoT);
		TypeMember(HPLessThan);
		TypeMember(Knockback);
		TypeMember(Grab);
		TypeMember(Tells);
		TypeMember(HaveBuff);
		TypeMember(Status);
		TypeMember(AvgMana);
		TypeMember(AvgHP);
		TypeMember(TargetingMe);
		TypeMember(NotTargetingMe);
		TypeMember(UsePet);
		TypeMember(Pulling);
		TypeMember(Returning);
		TypeMember(PullMob);
		TypeMember(NukeToT);
		TypeMember(HealToT);
		TypeMember(Call);
		TypeMember(FindItem);
	};
	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest)
	{
#define pItem ((PCONTENTS)VarPtr.Ptr)
		int buffID = 0;
		char myFileVersion[MAX_STRING] = { 0 };
		bool bVersionResult;
		PMQ2TYPEMEMBER pMember = MQ2BotType::FindMember(Member);
		if (pMember)
		{
			switch ((BotMembers)pMember->ID)
			{
			case Adds:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vAdds.size())
							return false;
						Dest.Ptr = vAdds[Count];
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vAdds.size();
				Dest.Type = pIntType;
				return true;
			case Heal:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vHeal.size())
							return false;
						Dest.Ptr = vHeal[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vHeal.size();
				Dest.Type = pIntType;
				return true;
			case Nuke:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vNuke.size())
							return false;
						Dest.Ptr = vNuke[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vNuke.size();
				Dest.Type = pIntType;
				return true;
			case Dot:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vDot.size())
							return false;
						Dest.Ptr = vDot[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vDot.size();
				Dest.Type = pIntType;
				return true;
			case Swarm:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vSwarm.size())
							return false;
						Dest.Ptr = vSwarm[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vSwarm.size();
				Dest.Type = pIntType;
				return true;
			case HealPet:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vHealPet.size())
							return false;
						Dest.Ptr = vHealPet[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vHealPet.size();
				Dest.Type = pIntType;
				return true;
			case Aggro:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vAggro.size())
							return false;
						Dest.Ptr = vAggro[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vAggro.size();
				Dest.Type = pIntType;
				return true;
			case Rez:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vRez.size())
							return false;
						Dest.Ptr = vRez[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vRez.size();
				Dest.Type = pIntType;
				return true;
			case SelfBuff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vSelfBuff.size())
							return false;
						Dest.Ptr = vSelfBuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vSelfBuff.size();
				Dest.Type = pIntType;
				return true;
			case Fade:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vFade.size())
							return false;
						Dest.Ptr = vFade[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vFade.size();
				Dest.Type = pIntType;
				return true;
			case Jolt:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vJolt.size())
							return false;
						Dest.Ptr = vJolt[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vJolt.size();
				Dest.Type = pIntType;
				return true;
			case Debuff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vDebuff.size())
							return false;
						Dest.Ptr = vDebuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vDebuff.size();
				Dest.Type = pIntType;
				return true;
			case Mez:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vMez.size())
							return false;
						Dest.Ptr = vMez[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vMez.size();
				Dest.Type = pIntType;
				return true;
			case Root:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vRoot.size())
							return false;
						Dest.Ptr = vRoot[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vRoot.size();
				Dest.Type = pIntType;
				return true;
			case Snare:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vSnare.size())
							return false;
						Dest.Ptr = vSnare[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vSnare.size();
				Dest.Type = pIntType;
				return true;
			case FightBuff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vFightBuff.size())
							return false;
						Dest.Ptr = vFightBuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vFightBuff.size();
				Dest.Type = pIntType;
				return true;
			case MainTankBuff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vMainTankBuff.size())
							return false;
						Dest.Ptr = vMainTankBuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vMainTankBuff.size();
				Dest.Type = pIntType;
				return true;
			case PetBuff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vPetBuff.size())
							return false;
						Dest.Ptr = vPetBuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vPetBuff.size();
				Dest.Type = pIntType;
				return true;
			case Pet:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vPet.size())
							return false;
						Dest.Ptr = vPet[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vPet.size();
				Dest.Type = pIntType;
				return true;
			case Mana:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vMana.size())
							return false;
						Dest.Ptr = vMana[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vMana.size();
				Dest.Type = pIntType;
				return true;
			case Endurance:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vEndurance.size())
							return false;
						Dest.Ptr = vEndurance[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vEndurance.size();
				Dest.Type = pIntType;
				return true;
			case Buff:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vBuff.size())
							return false;
						Dest.Ptr = vBuff[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vBuff.size();
				Dest.Type = pIntType;
				return true;
			case Aura:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vAura.size())
							return false;
						Dest.Ptr = vAura[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vAura.size();
				Dest.Type = pIntType;
				return true;
			case Lifetap:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vLifetap.size())
							return false;
						Dest.Ptr = vLifetap[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vLifetap.size();
				Dest.Type = pIntType;
				return true;
			case PossibleAdds:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vPossibleAdds.size())
							return false;
						Dest.Ptr = vPossibleAdds[Count];
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vPossibleAdds.size();
				Dest.Type = pIntType;
				return true;
			case Clicky:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vClicky.size())
							return false;
						strcpy_s(DataTypeTemp, vClicky[Count].c_str());
						Dest.Ptr = DataTypeTemp;
						Dest.Type = pStringType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.Type = pIntType;
				Dest.Int = vClicky.size();
				return true;
			case AA:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vAA.size())
							return false;
						char test[MAX_STRING];
						strcpy_s(test, vAA[Count].c_str());
						DWORD aaIndex = GetAAIndexByName(test);
						if (aaIndex > 0)
						{
							if (pAltAdvManager->GetAAById(aaIndex))
							{
								Dest.Ptr = pAltAdvManager->GetAAById(aaIndex);
								Dest.Type = pAltAbilityType;
								return true;
							}
						}
						return false;
					}
					else
					{
						return false;
					}
				}
				Dest.Type = pIntType;
				Dest.DWord = vAA.size();
				return true;
			case MinHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupMinHP(GETNUMBER());
						Dest.Ptr = gMinHP;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupMinHP(250);
				Dest.Ptr = gMinHP;
				Dest.Type = pSpawnType;
				return true;
			case MinPetHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupMinPetHP(GETNUMBER());
						Dest.Ptr = gMinPetHP;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupMinPetHP(250);
				Dest.Ptr = gMinPetHP;
				Dest.Type = pSpawnType;
				return true;
			case MinMana:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupMinMana(GETNUMBER());
						Dest.Ptr = gMinMana;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupMinMana(250);
				Dest.Ptr = gMinMana;
				Dest.Type = pSpawnType;
				return true;
			case ValidDetSpell:
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					if (ISINDEX())
					{
						if (ISNUMBER())
						{
							if (PSPELL pSpell = GetSpellByID(GETNUMBER()))
							{
								Dest.DWord = ValidDet(pSpell, pMyTarget);
								Dest.Type = pBoolType;
								return true;
							}
						}
						else
						{
							if (PSPELL pSpell = GetSpellByName(GETFIRST()))
							{
								Dest.DWord = ValidDet(pSpell, pMyTarget);
								Dest.Type = pBoolType;
								return true;
							}
						}
					}
				}
				return false;
			case SpellGem:
				Dest.DWord = 0;
				if (InGameOK())
					Dest.Int = GetCharInfo()->pSpawn->CastingData.SpellSlot == 255 ? 0 : GetCharInfo()->pSpawn->CastingData.SpellSlot + 1;
				Dest.Type = pIntType;
				return true;
			case SpellETA:
				Dest.DWord = 0;
				if (InGameOK())
					Dest.Int = (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 ? ((GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) / 100) : 0;
				Dest.Type = pIntType;
				return true;
			case CastReady:
				if (pCastSpellWnd && InGameOK())
				{
					for (unsigned long nGem = 0; nGem < NUM_SPELL_GEMS; nGem++)
					{
						if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[nGem]))
						{
							if (((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[nGem] && ((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[nGem]->spellstate != 1)
							{
								Dest.DWord = true;
								Dest.Type = pBoolType;
								return true;
							}
						}
					}
				}
				Dest.DWord = false;
				Dest.Type = pBoolType;
				return true;
			case DeepSleep:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && strstr(tempSpell->Name, "Deep Sleep"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Slowed:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 17 || strstr(tempSpell->Name, "Deep Sleep's Malaise")))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Rooted:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 117)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Mezzed:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 25)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Crippled:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 50)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Malod:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 55)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Tashed:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Tash"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Snared:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 5 || tempSpell->SpellIcon == 160 && strstr(tempSpell->Name, "Darkness")))
						{
							((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Hasted:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell)
						{
							if (tempSpell->SpellIcon == 16 || tempSpell->SpellIcon == 134)
							{
								Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
								Dest.Type = pTimeStampType;
								return true;
							}
						}
					}
				}
				return false;
			case Aego:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 132 && !strstr(tempSpell->Name, "Growth"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Skin:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 131)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Focus:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 130 || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Shield of") || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Armor of the")))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Regen:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 118)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Symbol:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 150)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Clarity:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 21 && !strstr(tempSpell->Name, "Knowledge") && !strstr(tempSpell->Name, "Recourse") && !strstr(tempSpell->Name, "Soothing") && !strstr(tempSpell->Name, "Brell"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Pred:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "of the Predator") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Strength:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Brells:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 90 && strstr(tempSpell->Name, "Brell's"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case SV:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case SE:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Spiritual E"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case HybridHP:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V") || tempSpell->SpellIcon == 80 && strstr(tempSpell->Name, "Brell's") || tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Growth:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && (tempSpell->SpellIcon == 132 && (strstr(tempSpell->Name, "Growth") || strstr(tempSpell->Name, "Stance"))))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
			case Shining:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && tempSpell->SpellIcon == 70 && strstr(tempSpell->Name, "Shining"))
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
			case CurrentSub:
				Dest.Ptr = CurrentRoutine;
				Dest.Type = pStringType;
				return true;
			case Merc:
				Dest.Ptr = MercName;
				Dest.Type = pStringType;
				return true;
			case Named:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					if (strstr(pMyTarget->Name, "#") && !pMyTarget->MasterID && strlen(pMyTarget->Lastname) < 1 && pMyTarget->Type == SPAWN_NPC)
					{
						Dest.DWord = true;
					}
				}
				return true;
			case QuestMob:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					if (strstr(pMyTarget->Name, "#") && !pMyTarget->MasterID && pMyTarget->Lastname && pMyTarget->Type == SPAWN_NPC)
					{
						Dest.DWord = true;
					}
				}
				return true;
			case MezImmune:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					char test[MAX_STRING];
					sprintf_s(test, "|%s|", &pMyTarget->DisplayedName[0]);
					if (strstr(MezImmuneList, test))
					{
						Dest.DWord = true;
					}
				}
				return true;
			case SnareImmune:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					char test[MAX_STRING];
					sprintf_s(test, "|%s|", &pMyTarget->DisplayedName[0]);
					if (strstr(SnareImmuneList, test))
					{
						Dest.DWord = true;
					}
				}
				return true;
			case Ignore:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					char test[MAX_STRING];
					sprintf_s(test, "|%s|", &pMyTarget->DisplayedName[0]);
					if (strstr(IgnoreList, test))
					{
						Dest.DWord = true;
						return true;
					}

				}
				return true;
			case XMinHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						XTMinHP(GETNUMBER());
						Dest.Ptr = xMinHP;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				XTMinHP(250);
				Dest.Ptr = xMinHP;
				Dest.Type = pSpawnType;
				return true;
			case XMinPetHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						XTMinPetHP(GETNUMBER());
						Dest.Ptr = xMinPetHP;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				XTMinPetHP(250);
				Dest.Ptr = xMinPetHP;
				Dest.Type = pSpawnType;
				return true;
			case XMinMana:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						XTMinMana(GETNUMBER());
						Dest.Ptr = xMinMana;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				XTMinMana(250);
				Dest.Ptr = xMinMana;
				Dest.Type = pSpawnType;
				return true;
			case XPriority:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						XTPriority(GETNUMBER());
						Dest.Ptr = xPriority;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				XTPriority(250);
				Dest.Ptr = xPriority;
				Dest.Type = pSpawnType;
				return true;
			case Facing:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					if (ISINDEX())
					{
						if (ISNUMBER())
						{
							Face(pMyTarget);
							if (facing < GETNUMBER())
								Dest.DWord = true;
						}
						else
						{
							return false;
						}
					}
					Face(pMyTarget);
					if (facing < 5)
						Dest.DWord = true;
				}
				return true;
			case FacingMe:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					if (ISINDEX())
					{
						if (ISNUMBER())
						{
							FaceMe(pMyTarget);
							if (abs(180 - facing) < GETNUMBER())
								Dest.DWord = true;
						}
						else
						{
							return false;
						}
					}
					FaceMe(pMyTarget);
					if (abs(180 - facing) < 5)
						Dest.DWord = true;
				}
				return true;
			case NetBotMinHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						NetBotsMinHP(GETNUMBER());
						Dest.Ptr = nMinHP;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				NetBotsMinHP(250);
				Dest.Ptr = nMinHP;
				Dest.Type = pSpawnType;
				return true;
			case ImHit:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vImHit.size())
							return false;
						char test[MAX_STRING];
						strcpy_s(test, vImHit[Count].c_str());
						DWORD aaIndex = GetAAIndexByName(test);
						if (aaIndex > 0)
						{
							if (pAltAdvManager->GetAAById(aaIndex))
							{
								Dest.Ptr = pAltAdvManager->GetAAById(aaIndex);
								Dest.Type = pAltAbilityType;
								return true;
							}
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				Dest.Type = pIntType;
				Dest.DWord = vImHit.size();
				return true;
			case InvisSpell:
				if (vInvis.size())
				{
					char test[MAX_STRING];
					strcpy_s(test, vInvis[0].c_str());
					DWORD aaIndex = GetAAIndexByName(test);
					if (aaIndex > 0)
					{
						if (pAltAdvManager->GetAAById(aaIndex))
						{
							Dest.Ptr = test;
							Dest.Type = pStringType;
							return true;
						}
					}
				}
				return false;
			case IVUSpell:
				if (vInvisUndead.size())
				{
					char test[MAX_STRING];
					strcpy_s(test, vInvisUndead[0].c_str());
					DWORD aaIndex = GetAAIndexByName(test);
					if (aaIndex > 0)
					{
						if (pAltAdvManager->GetAAById(aaIndex))
						{
							Dest.Ptr = test;
							Dest.Type = pStringType;
							return true;
						}
					}
				}
				return false;
			case Levitate:
				if (vLevitate.size())
				{
					char test[MAX_STRING];
					strcpy_s(test, vLevitate[0].c_str());
					DWORD aaIndex = GetAAIndexByName(test);
					if (aaIndex > 0)
					{
						if (pAltAdvManager->GetAAById(aaIndex))
						{
							Dest.Ptr = test;
							Dest.Type = pStringType;
							return true;
						}
					}
				}
				return false;
			case Ignores:
				Dest.Type = pStringType;
				Dest.Ptr = IgnoreList;
				return true;
			case SnareImmunes:
				Dest.Type = pStringType;
				Dest.Ptr = SnareImmuneList;
				return true;
			case MezImmunes:
				Dest.Type = pStringType;
				Dest.Ptr = MezImmuneList;
				return true;
			case CharmImmunes:
				Dest.Type = pStringType;
				Dest.Ptr = CharmImmuneList;
				return true;
			case CharmImmune:
				Dest.Type = pBoolType;
				Dest.DWord = false;
				if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
				{
					char test[MAX_STRING];
					sprintf_s(test, "|%s|", &pMyTarget->DisplayedName[0]);
					if (strstr(CharmImmuneList, test))
					{
						Dest.DWord = true;
						return true;
					}

				}
				return true;
			case IVU:
				Dest.Type = pBoolType;
				Dest.DWord = CheckIVUStatus();
				return true;
			case Invis:
				Dest.Type = pBoolType;
				Dest.DWord = CheckInvisStatus();
				return true;
			case Freeze:
				if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
					return false;
				for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
				{
					buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
					if (buffID > 0 && buffID != -1)
					{
						PSPELL tempSpell = GetSpellByID(buffID);
						if (tempSpell && GetSpellAttribX(tempSpell, 2) == 192)
						{
							Dest.UInt64 = ((PCTARGETWND)pTargetWnd)->BuffTimer[i];
							Dest.Type = pTimeStampType;
							return true;
						}
					}
				}
				return false;
			case Twincast:
				if (InGameOK())
				{
					for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++)
					{
						if (GetCharInfo2()->Buff[nBuff].SpellID > 0)
						{
							PSPELL tempSpell = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID);
							if (tempSpell && strstr(tempSpell->Name, "Twincast") && !strstr(tempSpell->Name, "Healing"))
							{
								Dest.DWord = true;
								Dest.Type = pBoolType;
								return true;
							}
						}
					}
				}
				Dest.DWord = false;
				Dest.Type = pBoolType;
				return true;
			case GoM:
				if (InGameOK())
				{
					for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
					{
						if (GetCharInfo2()->ShortBuff[nBuff].SpellID > 0)
						{
							PSPELL tempSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID);
							if (tempSpell && (strstr(tempSpell->Name, "Gift of ") && strstr(tempSpell->Name, "Mana") || strstr(tempSpell->Name, "Gracious Mana")))
							{
								Dest.DWord = GetSpellBaseX(tempSpell, 2);
								Dest.Type = pIntType;
								return true;
							}
						}
					}
				}
				Dest.DWord = 0;
				Dest.Type = pIntType;
				return true;
			case DA:
			{
				Dest.DWord = 0;
				if (InGameOK())
				{
					unsigned long nBuff = 0;
					for (nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
					{
						if (GetCharInfo2()->ShortBuff[nBuff].SpellID > 0)
						{
							PSPELL tempSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID);
							if (tempSpell && GetSpellAttribX(tempSpell, 0) == 40)
							{
								Dest.DWord = nBuff;
								Dest.Type = pIntType;
								return true;
							}
						}
					}
					Dest.DWord = nBuff;
				}
				Dest.Type = pIntType;
				return true;
			}
			case Twist:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vTwist.size() || Count >= vTwistSpell.size())
							return false;
						Dest.Ptr = vTwistSpell[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						for (unsigned int i = 0; i < vTwistSpell.size(); i++)
						{
							if (strstr(vTwistSpell[i]->Name, GETFIRST()))
							{
								Dest.DWord = i + 1;
								Dest.Type = pIntType;
								return true;
							}
						}
					}
				}
				return false;
			case TwistList:
				if (!vTwist.size())
					return false;
				Dest.Ptr = twistList;
				Dest.Type = pStringType;
				return true;
			case Twisting:
				Dest.DWord = twisting;
				Dest.Type = pBoolType;
				return true;
			case TwistLast:
				if (!vTwist.size())
					return false;
				Dest.DWord = songPrev + 1;
				Dest.Type = pIntType;
				return true;
			case TwistCurrent:
				if (!vTwist.size())
					return false;
				Dest.DWord = songCurrent + 1;
				Dest.Type = pIntType;
				return true;
			case TwistNext:
				if (!vTwist.size())
					return false;
				Dest.DWord = songNext + 1;
				Dest.Type = pIntType;
				return true;
			case Body:
				Dest.Ptr = BodyTypeFix;
				Dest.Type = pStringType;
				return true;
			case SpawnTime:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(GETNUMBER()))
						{

							Dest.Int64 = SpawnTimer(pSpawn);
							Dest.Type = pInt64Type;
							return true;
						}
					}
					else
					{
						if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByName(GETFIRST()))
						{
							Dest.Int64 = SpawnTimer(pSpawn);
							Dest.Type = pInt64Type;
							return true;
						}
					}
				}
				return false;
			case DiscCat:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						if (GetSpellByID(GETNUMBER()))
						{
							DiscCategory(GetSpellByID(GETNUMBER()));
							Dest.Ptr = spellCat;
							Dest.Type = pStringType;
							return true;
						}
					}
					else
					{
						if (GetSpellByName(GETFIRST()))
						{
							DiscCategory(GetSpellByName(GETFIRST()));
							Dest.Ptr = spellCat;
							Dest.Type = pStringType;
							return true;
						}
					}
				}
				return false;
			case Farthest:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupFarthest(GETNUMBER());
						Dest.Ptr = gCotH;
						Dest.Type = pSpawnType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupFarthest(250);
				Dest.Ptr = gCotH;
				Dest.Type = pSpawnType;
				return true;
			case Restricted:
				Dest.Type = pStringType;
				Dest.Ptr = RestrictedZones;
				return true;
			case SpellCat:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						if (GetSpellByID(GETNUMBER()))
						{
							SpellCategory(GetSpellByID(GETNUMBER()));
							Dest.Ptr = spellCat;
							Dest.Type = pStringType;
							return true;
						}
					}
					else
					{
						if (GetSpellByName(GETFIRST()))
						{
							SpellCategory(GetSpellByName(GETFIRST()));
							Dest.Ptr = spellCat;
							Dest.Type = pStringType;
							return true;
						}
					}
				}
				return false;
			case AddByID:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (!Count || !vAdds.size())
							return false;
						for (unsigned int x = 0; x < vAdds.size(); x++)
						{
							if (vAdds[x]->SpawnID == Count)
							{
								Dest.Int = x + 1;
								Dest.Type = pIntType;
								return true;
							}
						}

					}
				}
				return false;
			case AddCount:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						int adds = AddCountByDistance(GETNUMBER());
						Dest.Int = adds;
						Dest.Type = pIntType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vAdds.size();
				Dest.Type = pIntType;
				return true;
			case Active:
				Dest.DWord = false;
				if (Bot == 1)
					Dest.DWord = true;
				Dest.Type = pBoolType;
				return true;
			case Outside:
				Dest.DWord = outside;
				Dest.Type = pBoolType;
				return true;
			case ZoneTimeSec:
				Dest.UInt64 = ((MQGetTickCount64() - ZoneTimer) / 1000LL);
				Dest.Type = pInt64Type;
				return true;
			case ZoneTimeMin:
				Dest.UInt64 = ((MQGetTickCount64() - ZoneTimer) / 60000LL);
				Dest.Type = pInt64Type;
				return true;
			case ZoneTimeHour:
				Dest.UInt64 = ((MQGetTickCount64() - ZoneTimer) / 3600000LL);
				Dest.Type = pInt64Type;
				return true;
			case Version:
				sprintf_s(DataTypeTemp, "%.4f", botVersion);
				Dest.Ptr = DataTypeTemp;
				Dest.Type = pStringType;
				return true;
			case FileVersion:
				bVersionResult = GetVersionInfo(myFileVersion);
				sprintf_s(DataTypeTemp, "%s", bVersionResult ? myFileVersion : "Unknown");
				Dest.Ptr = DataTypeTemp;
				Dest.Type = pStringType;
				return true;
			case LastHit:
				Dest.UInt64 = ((MQGetTickCount64() - ImHitTimeStamp) / 1000LL);
				Dest.Type = pInt64Type;
				return true;
			case Zoning:
				Dest.DWord = zoning;
				Dest.Type = pBoolType;
				return true;
			case Instance:
				Dest.DWord = false;
				if (HIWORD(pLocalPlayer->Data.Zone))
					Dest.DWord = true;
				Dest.Type = pBoolType;
				return true;
			case SpellDuration:
				Dest.Int = 0;
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						if (PSPELL test = GetSpellByID(GETNUMBER()))
						{
							Dest.DWord = GetSpellDuration2(GetSpellByID(GETNUMBER()));
							Dest.Type = pTicksType;
							return true;
						}
					}
					else
					{
						if (PSPELL test = GetSpellByName(GETFIRST()))
						{
							Dest.DWord = GetSpellDuration2(GetSpellByName(GETFIRST()));
							Dest.Type = pTicksType;
							return true;
						}
					}
				}
				return false;
			case INI:
				Dest.Ptr = INISection;
				Dest.Type = pStringType;
				return true;
			case BuffList:
				if (InGameOK())
				{
					char bufflist[MAX_STRING] = "|";
					char temp[MAX_STRING] = { 0 };
					for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
					{

						if (GetCharInfo2()->Buff[i].SpellID > 0) {
							PSPELL tempSpell = GetSpellByID(GetCharInfo2()->Buff[i].SpellID);
							if (tempSpell && tempSpell->SpellIcon == 117)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRoot|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 17 || strstr(tempSpell->Name, "Deep Sleep's Malaise")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSlow|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 25)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMez|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 50)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sCripple|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 55)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMalo|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Tash"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sTash|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 5 || tempSpell->SpellIcon == 160 && strstr(tempSpell->Name, "Darkness")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSnare|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 16 || tempSpell->SpellIcon == 134))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHaste|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 132 && !strstr(tempSpell->Name, "Growth"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sAego|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 131)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSkin|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 130 || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Shield of") || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Armor of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sFocus|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 118)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRegen|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 150)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSymbol|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 21 && !strstr(tempSpell->Name, "Knowledge") && !strstr(tempSpell->Name, "Recourse") && !strstr(tempSpell->Name, "Soothing") && !strstr(tempSpell->Name, "Brell"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sClarity|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "of the Predator") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sPred|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sStrength|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 90 && strstr(tempSpell->Name, "Brell's"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sBrells|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSV|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Spiritual E"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSE|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V") || tempSpell->SpellIcon == 80 && strstr(tempSpell->Name, "Brell's") || tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHybridHP|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 132 && (strstr(tempSpell->Name, "Growth") || strstr(tempSpell->Name, "Stance"))))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sGrowth|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 70 && strstr(tempSpell->Name, "Shining"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sShining|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 21)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sDS|", temp);
							}
						}
					}
					strcpy_s(Tempos, bufflist);
					Dest.Type = pStringType;
					Dest.Ptr = Tempos;
					return true;
				}
			case TargetBuffList:
				if (InGameOK())
				{
					if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
						return false;
					char bufflist[MAX_STRING] = "|";
					char temp[MAX_STRING] = { 0 };
					for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
					{
						buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
						if (buffID > 0 && buffID != -1)
						{
							PSPELL tempSpell = GetSpellByID(buffID);
							if (tempSpell && GetSpellAttribX(tempSpell, 2) == 192)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sFreeze|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 117)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRoot|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 17 || strstr(tempSpell->Name, "Deep Sleep's Malaise")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSlow|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 25)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMez|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 50)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sCripple|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 55)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMalo|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Tash"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sTash|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 5 || tempSpell->SpellIcon == 160 && strstr(tempSpell->Name, "Darkness")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSnare|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 16 || tempSpell->SpellIcon == 134))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHaste|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 132 && !strstr(tempSpell->Name, "Growth"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sAego|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 131)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSkin|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 130 || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Shield of") || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Armor of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sFocus|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 118)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRegen|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 150)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSymbol|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 21 && !strstr(tempSpell->Name, "Knowledge") && !strstr(tempSpell->Name, "Recourse") && !strstr(tempSpell->Name, "Soothing") && !strstr(tempSpell->Name, "Brell"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sClarity|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "of the Predator") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sPred|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sStrength|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 90 && strstr(tempSpell->Name, "Brell's"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sBrells|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSV|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Spiritual E"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSE|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V") || tempSpell->SpellIcon == 80 && strstr(tempSpell->Name, "Brell's") || tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHybridHP|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 132 && (strstr(tempSpell->Name, "Growth") || strstr(tempSpell->Name, "Stance"))))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sGrowth|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 70 && strstr(tempSpell->Name, "Shining"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sShining|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 21)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sDS|", temp);
							}
						}
					}
					strcpy_s(Tempos, bufflist);
					Dest.Type = pStringType;
					Dest.Ptr = Tempos;
					return true;
				}
			case PetBuffList:
				if (InGameOK())
				{
#define pPetInfoWindow ((PEQPETINFOWINDOW)pPetInfoWnd)
					char bufflist[MAX_STRING] = "|";
					char temp[MAX_STRING] = { 0 };
					for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
					{

						if (PSPELL tempSpell = GetSpellByID(pPetInfoWindow->Buff[i])) {
							if (tempSpell && tempSpell->SpellIcon == 117)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRoot|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 17 || strstr(tempSpell->Name, "Deep Sleep's Malaise")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSlow|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 25)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMez|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 50)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sCripple|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 55)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sMalo|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Tash"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sTash|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 5 || tempSpell->SpellIcon == 160 && strstr(tempSpell->Name, "Darkness")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSnare|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 16 || tempSpell->SpellIcon == 134))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHaste|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 132 && !strstr(tempSpell->Name, "Growth"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sAego|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 131)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSkin|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 130 || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Shield of") || tempSpell->SpellIcon == 133 && strstr(tempSpell->Name, "Armor of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sFocus|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 118)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sRegen|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 150)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSymbol|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 21 && !strstr(tempSpell->Name, "Knowledge") && !strstr(tempSpell->Name, "Recourse") && !strstr(tempSpell->Name, "Soothing") && !strstr(tempSpell->Name, "Brell"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sClarity|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "of the Predator") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sPred|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sStrength|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 90 && strstr(tempSpell->Name, "Brell's"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sBrells|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSV|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 72 && strstr(tempSpell->Name, "Spiritual E"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSE|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 1 && strstr(tempSpell->Name, "Spiritual V") || tempSpell->SpellIcon == 80 && strstr(tempSpell->Name, "Brell's") || tempSpell->SpellIcon == 123 && strstr(tempSpell->Name, "Strength of the") || tempSpell->SpellIcon == 158 && strstr(tempSpell->Name, "Protection of the")))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sHybridHP|", temp);
							}
							if (tempSpell && (tempSpell->SpellIcon == 132 && (strstr(tempSpell->Name, "Growth") || strstr(tempSpell->Name, "Stance"))))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sGrowth|", temp);
							}
							if (tempSpell && tempSpell->SpellIcon == 70 && strstr(tempSpell->Name, "Shining"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sShining|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 21)
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sDS|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, "Spirit of"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sSpirit|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, "Iceflame"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sIceflame|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, "Empowered Minion"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sEmpowered|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, " Warder"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sWarder|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, "Aggression"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sAggression|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, "Aegis of "))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sAegis|", temp);
							}
							if (tempSpell && tempSpell->Subcategory == 71 && strstr(tempSpell->Name, " Stance"))
							{
								strcpy_s(temp, bufflist);
								sprintf_s(bufflist, "%sStance|", temp);
							}
						}
					}
					strcpy_s(Tempos, bufflist);
					Dest.Type = pStringType;
					Dest.Ptr = Tempos;
					return true;
				}
			case Disc:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vDisc.size())
							return false;
						Dest.Ptr = vDisc[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vDisc.size();
				Dest.Type = pIntType;
				return true;
			case HoT:
				if (InGameOK())
				{
					for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
					{
						if (GetCharInfo2()->ShortBuff[nBuff].SpellID > 0)
						{
							PSPELL tempSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID);
							if (tempSpell && tempSpell->Subcategory == 32 && tempSpell->Category == 42 && GetSpellAttribX(tempSpell, 0) == 100)
							{
								Dest.DWord = GetSpellBaseX(tempSpell, 0);
								Dest.Type = pIntType;
								return true;
							}
						}
					}
				}
				Dest.DWord = 0;
				Dest.Type = pIntType;
				return true;
			case HPLessThan:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						Dest.DWord = HPCount(GETNUMBER());
						Dest.Type = pIntType;
						return true;
					}
				}
				Dest.DWord = 0;
				Dest.Type = pIntType;
				return true;
			case Knockback:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vKnockback.size())
							return false;
						Dest.Ptr = vKnockback[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vKnockback.size();
				Dest.Type = pIntType;
				return true;
			case Grab:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vGrab.size())
							return false;
						Dest.Ptr = vGrab[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vGrab.size();
				Dest.Type = pIntType;
				return true;
			case Tells:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vTells.size())
							return false;
						strcpy_s(Tempos, vTells[Count].c_str());
						Dest.Type = pStringType;
						Dest.Ptr = Tempos;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vTells.size();
				Dest.Type = pIntType;
				return true;
			case HaveBuff:
				if (!ISINDEX())
					return false;
				if (ISNUMBER())
				{
					return false;
				}
				else
				{
					for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++)
					{
						if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID))
						{
							if (!_strnicmp(GETFIRST(), pSpell->Name, strlen(GETFIRST())))
							{
								Dest.Ptr = &GetCharInfo2()->Buff[nBuff];
								Dest.Type = pBuffType;
								return true;
							}
						}
					}
					for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
					{
						if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID))
						{
							if (!_strnicmp(GETFIRST(), pSpell->Name, strlen(GETFIRST())))
							{
								Dest.Ptr = &GetCharInfo2()->ShortBuff[nBuff];
								Dest.Type = pBuffType;
								return true;
							}
						}
					}
				}
				return false;
			case Status:
				Dest.Type = pStringType;
				sprintf_s(DataTypeTemp, "%s", Bot == 0 ? "OFF" : Bot == 1 ? "ON" : Bot == -1 ? "PRE" : "Unknown");
				Dest.Ptr = DataTypeTemp;
				return true;
			case AvgMana:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupMinMana(GETNUMBER());
						Dest.Int = gAvgMana;
						Dest.Type = pIntType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupMinMana(250);
				Dest.Int = gAvgMana;
				Dest.Type = pIntType;
				return true;
			case AvgHP:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						GroupMinHP(GETNUMBER());
						Dest.Int = gAvgHP;
						Dest.Type = pIntType;
						return true;
					}
					else
					{
						return false;
					}
				}
				GroupMinHP(250);
				Dest.Int = gAvgHP;
				Dest.Type = pIntType;
				return true;
			case TargetingMe:
				Dest.Ptr = xTargetingMe;
				Dest.Type = pSpawnType;
				return true;
			case NotTargetingMe:
				Dest.Ptr = xNotTargetingMe;
				Dest.Type = pSpawnType;
				return true;
			case UsePet:
				Dest.Int = iUsePet;
				Dest.Type = pIntType;
				return true;
			case Pulling:
				Dest.DWord = bPulling;
				Dest.Type = pBoolType;
				return true;
			case Returning:
				Dest.DWord = bReturning;
				Dest.Type = pBoolType;
				return true;
			case PullMob:
				Dest.Ptr = pPullMob;
				Dest.Type = pSpawnType;
				return true;
			case NukeToT:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vNukeToT.size())
							return false;
						Dest.Ptr = vNukeToT[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vNukeToT.size();
				Dest.Type = pIntType;
				return true;
			case HealToT:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vHealToT.size())
							return false;
						Dest.Ptr = vHealToT[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vHealToT.size();
				Dest.Type = pIntType;
				return true;
			case Call:
				if (ISINDEX())
				{
					if (ISNUMBER())
					{
						DWORD Count = GETNUMBER();
						if (Count<0 || Count >= vCall.size())
							return false;
						Dest.Ptr = vCall[Count];
						Dest.Type = pSpellType;
						return true;
					}
					else
					{
						return false;
					}
				}
				Dest.DWord = vCall.size();
				Dest.Type = pIntType;
				return true;
			case FindItem:
				if (!ISINDEX())
					return false;
				if (ISNUMBER()) {
					if (PCONTENTS myItem = FindItemByID(GETNUMBER())) {
						return true;
					}
				}
				else {
					PCHAR pName = GETFIRST();
					BOOL bExact = false;

					if (*pName == '=')
					{
						bExact = true;
						pName++;
					}
					if (PCONTENTS myItem = FindItemByName(pName, bExact)) {
						return true;
					}
					return false;
				}
			}
		}
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination)
	{
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}

	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source)
	{
		return false;
	}

	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source)
	{
		return false;
	}

	~MQ2BotType()
	{
	}
};

BOOL DataBot(PCHAR Index, MQ2TYPEVAR &Dest)
{
	Dest.Type = pBotTypes;
	Dest.DWord = 1;
	return true;
}

/*
BOOL DataAdd(PCHAR Index, MQ2TYPEVAR &Dest)
{
Dest.Type=pAddTypes;
Dest.DWord=1;
return true;
}
*/

#pragma endregion TLO

#pragma region AdditionalVars
std::map<string, string> SpellIf;
std::map<string, string>::iterator SpellIt;


enum _SpellsType { SINGLE_SPELL, GROUP_SPELL, AE_SPELL };

typedef struct _Spawns
{
	vector<PSPELL>		vSpellList;
	vector<ULONGLONG>	vSpellTimers;
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
} Spawns, *pSpawns;

// struct for spell data, how to use the spell, and how the spell was used
typedef struct _BotSpells
{
	PSPELL				Spell;					// store the spell itself so I can access all of its members
	char				SpellName[MAX_STRING];	// Check ini to see if they want to use a custom spell, if not then store the name of the spell/AA/Disc/item we actually want to /cast since that is often different
	bool				CanIReprioritize;		// if a specific custom spell or priority was set, then i want to ignore reprioritizing it
	char				SpellIconName[MAX_STRING];	// store the name of the icon in case it is special like mana reiteration buff icon
	char				Gem[MAX_STRING];		// in case they want to use a custom gem, otherwise this will be alt, disc, item, or default gem
	char				If[MAX_STRING];			// is there a custom if statement in the ini?
	char				Target[MAX_STRING];		// is there a custom target in the ini?  this is really a placeholder until i make a final decision on how to use spells
	char				SpellCat[MAX_STRING];	// what SpellCategory() is this, so i dont have to check over and over
	char				SpellType[MAX_STRING];	// this will be a custom spell type for each routine.  Ie. Fast Heal, Normal Heal, Fire, Cold, Tash, because I will use this info later on checks and prioritization
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
												// will need to add more members here once i go back and determine if i want to use mq2cast at all or create an internal casting mechanism.
} BotSpells, *PBotSpells;

//create our botspells vector
void LoadBotSpell(vector<_BotSpells> &v, char VectorName[MAX_STRING]);
void PopulateIni(vector<_BotSpells> &v, char VectorName[MAX_STRING]);
vector<_BotSpells> vHeal2;
void CreateHeal2();

std::map<int, _Spawns> SpawnTracker;
std::map<int, _Spawns>::iterator SpawnTrackerIt;

PCHAR szClassIDs[] = {
	"0", // 0x0
	"1", // Warrior
	"2", // Cleric
	"3", // Paladin
	"4", // Ranger
	"5", // Shadow Knight
	"6", // Druid
	"7", // Monk
	"8", // Bard
	"9", // Rogue
	"10", // Shaman
	"11", // Necromancer
	"12", // Wizard
	"13", // Magician
	"14", // Enchanter
	"15", // Beastlord
	"16" // Berserker
};

PCHAR szOptions[] = { "Heal", "SelfBuff", "Fade", "Jolt", "Nuke", "Debuff", "Mez", "Root", "Snare", "FightBuff",
"MainTankBuff", "PetBuff", "Mana", "Endurance", "Buff", "Aura", "Dot", "Lifetap", "HealPet", NULL
};
enum _ModRodType { MOD_SPELL, MOD_AA };
enum _ModRodTarget { MOD_SELF, MOD_MASS };

struct _ModRods
{
	char			szSpell[MAX_STRING];
	char			szItem[MAX_STRING];
	int				ItemID;
	int				ManaAdded;
	int				HPCost;
	int				ReqLevel;
	_ModRodType		ModRodType;
	_ModRodTarget	ModRodTarget;
	int				InStock;
	ULONGLONG		RecheckTimer;
}
ModRods[] = {
	{ "Modulating Rod", "Summoned: Modulating Rod", 6346, 150, 225, 1, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Rod of Mystical Transvergence", "Rod of Mystical Transvergence", 3426, 360, 450, 1, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Mystical Transvergence", "Rod of Mystical Transvergence", 3426, 360, 450, 1, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Elemental Transvergence", "Wand of Elemental Transvergence", 52674, 656, 820, 71, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Elemental Transvergence", "Wand of Elemental Transvergence", 52674, 656, 820, 71, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Prime Transvergence", "Rod of Prime Transvergence", 52709, 800, 1000, 76, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Prime Transvergence", "Rod of Prime Transvergence", 52709, 800, 1000, 76, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Ethereal Transvergence", "Rod of Ethereal Transvergence", 52817, 976, 1220, 81, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Ethereal Transvergence", "Rod of Ethereal Transvergence", 52817, 976, 1220, 81, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Spectral Transvergence", "Rod of Spectral Transvergence", 57264, 1350, 1620, 86, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Spectral Transvergence", "Rod of Spectral Transvergence", 57264, 1350, 1620, 86, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Arcane Transvergence", "Rod of Arcane Transvergence", 64951, 2025, 2430, 91, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Arcane Transvergence", "Rod of Arcane Transvergence", 64951, 2025, 2430, 91, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Phantasmal Modulation", "Wand of Phantasmal Modulation", 76502, 7500, 11000, 96, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Phantasmal Transvergence", "Wand of Phantasmal Transvergence", 76503, 2531, 3038, 96, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Wand of Dark Modulation", "Wand of Pelagic Modulation", 99782, 8500, 12750, 101, MOD_SPELL, MOD_SELF, 1, 0 },
{ "Mass Dark Transvergence", "Wand of Pelagic Transvergence", 99783, 2970, 3494, 101, MOD_SPELL, MOD_MASS, 1, 0 },
{ "Summon Modulation Shard", "Summoned: Small Modulation Shard", 79320, 2500, 7500, 1, MOD_AA, MOD_MASS, 1, 0 },
{ "Summon Modulation Shard", "Summoned: Medium Modulation Shard", 79321, 5000, 15000, 1, MOD_AA, MOD_MASS, 1, 0 },
{ "Summon Modulation Shard", "Summoned: Large Modulation Shard", 79322, 7500, 23000, 1, MOD_AA, MOD_MASS, 1, 0 },
{ "Summon Modulation Shard", "Summoned: Giant Modulation Shard", 79497, 10000, 30000, 1, MOD_AA, MOD_MASS, 1, 0 },
{ "Summon Modulation Shard", "Summoned: Glowing Modulation Shard", 79498, 15000, 45000, 1, MOD_AA, MOD_MASS, 1, 0 },
{ "Azure Mind Crystal", "Azure Mind Crystal", 999999, 6000, 0, 1, MOD_AA, MOD_SELF, 1, 0 },  // Azure Mind Crystal ranks, starts 1000 mana rk1
};

PCHAR szSHD_Call[] = { "Call of Darkness", "Call of Dusk", "Call of Dusk Rk. II", "Call of Dusk Rk. III", "Call of Shadow",
"Call of Shadow Rk. II", "Call of Shadow Rk. III", "Call of Gloomhaze", "Call of Gloomhaze Rk. II",
"Call of Gloomhaze Rk. III", NULL
};


PCHAR ChatColors[] = { "\ay", "\a-y", "\ao", "\a-o", "\ag", "\a-g", "\ab", "\a-b", "\ar", "\a-r",
"\at", "\a-t", "\am", "\a-m", "\ap", "\a-p", "\aw", "\a-w", "\ax", "\a-x", "\au", "\a-u", NULL
};
PCHAR EQBCColors[] = { "[+y+]", "[+Y+]", "[+o+]", "[+O+]", "[+g+]", "[+G+]", "[+b+]", "[+B+]", "[+r+]", "[+R+]", "[+t+]",
"[+T+]", "[+m+]", "[+M+]", "[+p+]", "[+P+]", "[+w+]", "[+W+]", "[+x+]", "[+X+]", "[+u+]", "[+U+]", NULL
};

PCHAR szSummonedItems[] = { "Summoned: Imperious Minion", "Summoned: Imperious Minion II", "Summoned: Imperious Minion III",
"Summoned: Imperious Servant", "Summoned: Imperious Servant II", "Summoned: Imperious Servant III",
"Blazing Orb", "Summoned: Icebound Sliver", "Summoned: Exigent Minion", "Summoned: Exigent Minion II",
"Summoned: Exigent Minion III", "Summoned: Exigent Servant", "Summoned: Exigent Servant II",
"Summoned: Exigent Servant III", "Summoned: Elemental Ice Sliver", "Summoned: Elemental Ice Fragment",
"Summoned: Elemental Ice Shard", "Ether-Fused Shard", "Summoned: Icebound Fragment", "Summoned: Icebound Shard",
"Summoned: Exigent Minion XXI", "Summoned: Exigent Servant XXI", NULL
};
PCHAR szSummonedItemsID[] = { "76533", "76534", "76535", "76530", "76531", "76532", "64978", "76504", "64982", "64983", "64984", "64979",
"64980", "64981", "52819", "52820", "52821", "40512", "76505", "76506", "99813", "99810", NULL
};
PCHAR szSummonedEnc[] = { "Sanguine Mind Crystal", "Azure Mind Crystal", NULL };
PCHAR szAutoInv[] = { "Sanguine Mind Crystal", "Azure Mind Crystal", "Summoned: Imperious Minion", "Summoned: Imperious Minion II",
"Summoned: Imperious Minion III", "Summoned: Imperious Servant", "Summoned: Imperious Servant II",
"Summoned: Imperious Servant III", "Blazing Orb", "Summoned: Icebound Sliver", "Summoned: Exigent Minion",
"Summoned: Exigent Minion II", "Summoned: Exigent Minion III", "Summoned: Exigent Servant",
"Summoned: Exigent Servant II", "Summoned: Exigent Servant III", "Summoned: Elemental Ice Sliver",
"Summoned: Elemental Ice Fragment", "Summoned: Elemental Ice Shard", "Tavon's Polished Gemstone", "Brightedge",
"Aircrisp Apple", "Ether-Fused Tea", "Ether-Fused Shard", "Worlu's Windcloak", "Worlu's Prying Eyes",
"Tavon's Burnished Gemstone", "Bulwark of Many Portals", "Imprint of the Enhanced Minion",
"Exalted Tonic of Healing", "Crystallized Sulfur", "Summoned: Icebound Fragment", "Summoned: Icebound Shard",
"Summoned: Icebound Sliver", "Summoned: Exigent Minion XXI", "Summoned: Exigent Servant XXI", NULL
};
PCHAR szCauldron[] = { "Cauldron of Countless Goods", "Cauldron of Endless Goods", "Cauldron of Many Things", NULL };
#pragma endregion AdditionalVars

#pragma comment(lib,"version.lib")

#pragma region UtilityFunctions
bool GetVersionInfo(char *buffer, int len)
{
	DWORD   verBufferSize;
	char    verBuffer[MAX_STRING];
	CHAR	FullFilename[MAX_STRING] = { 0 };
	sprintf_s(FullFilename, "%s\\%s.dll", gszINIPath, PLUGIN_NAME);
	verBufferSize = GetFileVersionInfoSize(FullFilename, NULL);
	if (verBufferSize > 0 && verBufferSize <= sizeof(verBuffer))
	{
		if (TRUE == GetFileVersionInfo(FullFilename, NULL, verBufferSize, verBuffer))
		{
			UINT length;
			VS_FIXEDFILEINFO *verInfo = NULL;
			if (TRUE == VerQueryValue(verBuffer, "\\", reinterpret_cast<LPVOID*>(&verInfo), &length))
			{
				int major = verInfo->dwFileVersionMS >> 16;
				int minor = verInfo->dwFileVersionMS & 0x00FF;
				int build = verInfo->dwFileVersionLS >> 16;
				int series = verInfo->dwFileVersionLS & 0x00FF;
				sprintf_s(buffer, MAX_STRING, "%d.%d.%d.%d", major, minor, build, series);
				return true;
			}
		}
	}
	return false;
}
#pragma endregion UtilityFunctions

LONG Evaluate(PCHAR szLine)
{
	char test[MAX_STRING];
	sprintf_s(test, "${If[%s,1,0]}", szLine);
	ParseMacroData(test, MAX_STRING);
	return atoi(test);
}

void WriteBotSetting(int activate)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	sprintf_s(szTemp, "%d", activate);
	Bot = activate;
	WritePrivateProfileString(INISection, "Bot", szTemp, INIFileName);
}

int AddCountByDistance(int rangeCheck)
{
	if (!vAdds.size())
		return 0;
	int adds = 0;
	for (unsigned int i = 0; i < vAdds.size(); i++)
	{
		if (vAdds[i])
		{
			if (GetDistance(vAdds[i]->X, vAdds[i]->Y) < rangeCheck)
				adds++;
		}
	}
	return adds;
}

void AddTimer(PSPAWNINFO pSpawn, PSPELL pSpell, ULONGLONG timer)
{
	DebugWrite("Adding timer info for Spawn: %s | Spell: %s | Current Time: %llu | Timer: %llu",
		pSpawn ? pSpawn->Name : "NULL", pSpell ? pSpell->Name : "NULL", MQGetTickCount64(), timer);
	if (timer == 443)
		timer = 0;
	if (pSpawn)
	{
		/*
		SpawnTrackerIt = SpawnTracker.find(pSpawn->SpawnID);
		if (SpawnTrackerIt != SpawnTracker.end())
		{
		for (unsigned int i = 0; i < SpawnTrackerIt->second.vSpellList.size(); i++)
		{
		if (pSpell == SpawnTrackerIt->second.vSpellList[i])
		{
		SpawnTrackerIt->second.vSpellTimers[i] = timer;
		}
		}
		}
		else
		{
		_Spawns tempspawn;
		SpawnTracker[pSpawn->SpawnID] = tempspawn;
		SpawnTrackerIt = SpawnTracker.find(pSpawn->SpawnID);
		SpawnTrackerIt->second.vSpellList.push_back(pSpell);
		SpawnTrackerIt->second.vSpellTimers.push_back(timer);
		//WriteChatf("%d added with timer %llu", pSpawn->SpawnID, timer);  //DEBUG20151026
		}
		*/
		vTimerTimestamp.push_back(timer);
		vTimerSpawn.push_back(pSpawn);
		vTimerSpell.push_back(pSpell);
		/*
		if(DoDebug)
		WriteChatf("DEBUG: Adding timer: %s - %s - %d to %d",pSpawn->Name,pSpell->Name,MQGetTickCount64(),timer);
		*/
	}
}

bool CACheck(DWORD id)
{
	PSPELL pSpell = nullptr;
	if (id <= 0 || !GetCharInfo2())
		return false;
	__try
	{
		for (DWORD nCombat = 0; nCombat < NUM_COMBAT_ABILITIES; nCombat++)
		{
			if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombat))
			{
				pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombat));
				if (pSpell != nullptr)
				{
					if (id == GetCharInfo2()->CombatAbilities[nCombat])
					{
						return true;
					}
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DebugSpewAlways("%s: Exception CACheck(%d)", PLUGIN_NAME, id);
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

void CastAA(PSPAWNINFO pSpawn, PSPELL castSpell, char spellColor[MAX_STRING], string aaName)
{
	if (!InGameOK())
		return;
	if (gGameState != GAMESTATE_INGAME || !pSpawn || GetCharInfo()->Stunned == 1 || !castSpell)
		return;
	if (!pSpawn)
		return;
	CheckTimer();
	DebugWrite("Attempting to cast %s using alt", aaName.c_str());
	if (GetCharInfo()->standstate != STANDSTATE_STAND)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/stand", FromPlugin);
		DoingFeignDeath = 0;
	}
	char castType[MAX_STRING], castLine[MAX_STRING], castTimerInfo[MAX_STRING];
	sprintf_s(castType, "${If[${Me.AltAbilityReady[%s]} && ${Me.AltAbility[%s].Spell.ID},1,0]}", aaName.c_str(), aaName.c_str());
	ParseMacroData(castType, MAX_STRING);
	if (atoi(castType) == 1)
	{
		// if (castSpell->TargetType != 6 && castSpell->TargetType != 3 && castSpell->TargetType != 4 && castSpell->TargetType != 40)
		//	TargetIT(GetCharInfo()->pSpawn);
		sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", aaName.c_str());
		ParseMacroData(castTimerInfo, MAX_STRING);
		SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 300LL;
		sprintf_s(castLine, "/multiline ; /stopcast ; /casting \"%s\" alt", aaName.c_str());
		if (BardClass)
		{
			sprintf_s(castLine, "/alt act ${Me.AltAbility[%s].ID}", aaName.c_str());
			ParseMacroData(castLine, MAX_STRING);
		}
	}
	if (strlen(castLine) > 5)
	{
		DoCommand(GetCharInfo()->pSpawn, castLine);
		if (AnnounceEcho && pSpawn && LastAnnouncedSpell != castSpell->ID)
		{
			WriteChatf("%s%s \aw--> %s", spellColor, castSpell->Name, pSpawn->DisplayedName);
		}
		if (AnnounceEQBC && pSpawn && LastAnnouncedSpell != castSpell->ID)
		{
			EQBCSwap(spellColor);
			sprintf_s(castLine, "/bc %s%s [+x+]--> %s", EQBCColor, castSpell->Name, pSpawn->DisplayedName);
			HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
		}
		LastAnnouncedSpell = castSpell->ID;
		if (GetSpellDuration(castSpell, (PSPAWNINFO)pLocalPlayer) > 0 && castSpell->TargetType != 6)
		{
			char test[MAX_STRING] = {0};
			sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", castSpell->Name);
			ParseMacroData(test, MAX_STRING);
			ULONGLONG timer = MQGetTickCount64() + (ULONGLONG)floor((atof(test) * 1000));
			AddTimer(pSpawn, castSpell, timer);
		}
	}
}

void CastSpell(PSPAWNINFO pSpawn, PSPELL castSpell, char spellColor[MAX_STRING], int force)
{
	if (!InGameOK())
		return;
	if (gGameState != GAMESTATE_INGAME || !pSpawn || GetCharInfo()->Stunned == 1 || !castSpell)
		return;
	if (!pSpawn || !castSpell)
		return;
	CheckTimer();
	DebugWrite("Attempting to cast %s", castSpell ? castSpell->Name : "NULL");
	if (GetCharInfo()->standstate != STANDSTATE_STAND)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/stand", FromPlugin);
		DoingFeignDeath = 0;
	}
	char castType[MAX_STRING], castLine[MAX_STRING], castTimerInfo[MAX_STRING], spellGem[MAX_STRING];
	if (force == 1)
		sprintf_s(castType, "${If[!${Me.GemTimer[%s]} && ${Me.Book[%s]} && !${Me.AltAbility[%s]}  && !${Me.AltAbility[%s].Name.Equal[twincast]},1,0]}", castSpell->Name, castSpell->Name, castSpell->Name, castSpell->Name);
	if (force == 0)
		sprintf_s(castType, "${If[!${Me.GemTimer[%s]} && ${Me.SpellReady[%s]} && !${Me.AltAbility[%s].Spell.ID},1,0]}", castSpell->Name, castSpell->Name, castSpell->Name);
	ParseMacroData(castType, MAX_STRING);
	bool Done = false;
	if (atoi(castType) == 1)
	{
		sprintf_s(castTimerInfo, "${Spell[%s].MyCastTime.Float}", castSpell->Name);
		ParseMacroData(castTimerInfo, MAX_STRING);
		if (force == 1)
			SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 6000LL;
		if (force == 0)
			SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 300LL;
		sprintf_s(spellGem, "gem%d", DefaultGem);
		sprintf_s(castLine, "/multiline ; /stopcast ; /stopsong ; /casting \"%s\" %s", castSpell->Name, spellGem);
		DebugWrite("Attempting to cast %s using %s", castSpell ? castSpell->Name : "NULL", spellGem);
		Done = true;
	}
	if (!Done)
	{
		sprintf_s(castType, "${If[${Me.AltAbilityReady[%s]} && ${Me.AltAbility[%s].Spell.ID},1,0]}", castSpell->Name, castSpell->Name);
		ParseMacroData(castType, MAX_STRING);
		if (atoi(castType) == 1)
		{
			sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", castSpell->Name);
			ParseMacroData(castTimerInfo, MAX_STRING);
			SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 300LL;
			strcpy_s(spellGem, "alt");
			DebugWrite("Attempting to cast %s using alt", castSpell ? castSpell->Name : "NULL");
			sprintf_s(castLine, "/multiline ; /stopcast ; /casting \"%s\" alt", castSpell->Name);
			if (BardClass)
			{
				sprintf_s(castLine, "/alt act ${Me.AltAbility[%s].ID}", castSpell->Name);
				ParseMacroData(castLine, MAX_STRING);
			}
			Done = true;
		}
	}
	if (!Done)
	{
		sprintf_s(castType, "${If[${Me.CombatAbilityReady[%s]},1,0]}", castSpell->Name);
		ParseMacroData(castType, MAX_STRING);
		if (atoi(castType) == 1)
		{
			sprintf_s(castTimerInfo, "${Spell[%s].MyCastTime.Float}", castSpell->Name);
			ParseMacroData(castTimerInfo, MAX_STRING);
			SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 300LL;
			DebugWrite("Attempting to /disc %s", castSpell ? castSpell->Name : "NULL");
			sprintf_s(castLine, "/disc %s", castSpell->Name);
			Done = true;
		}
	}
	if (!Done)
	{
		sprintf_s(castType, "${If[(${Me.CombatAbility[%s]}||${Me.AltAbility[%s]}||${Me.Book[%s]}),1,0]}", castSpell->Name, castSpell->Name, castSpell->Name);
		ParseMacroData(castType, MAX_STRING);
		if (atoi(castType) == 1)
		{
			Done = true;
		}
	}
	if (!Done)
	{
		sprintf_s(castType, "${If[${Cast.Ready[%s]} && ${FindItem[%s].ItemSlot} && !${Me.Gem[%s]} && !${Me.AltAbility[%s]},1,0]}", castSpell->Name, castSpell->Name, castSpell->Name, castSpell->Name);
		ParseMacroData(castType, MAX_STRING);
		if (atoi(castType) == 1)
		{
			sprintf_s(castTimerInfo, "${FindItem[%s].CastTime.Float}", castSpell->Name);
			ParseMacroData(castTimerInfo, MAX_STRING);
			SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 300LL;
			strcpy_s(spellGem, "item");
			DebugWrite("Attempting to cast %s using item", castSpell ? castSpell->Name : "NULL");
			sprintf_s(castLine, "/multiline ; /stopcast ; /stopsong ; /casting \"%s\"|item", castSpell->Name);
			Done = true;
		}
	}
	if (strlen(castLine) > 5)
	{
		DoCommand(GetCharInfo()->pSpawn, castLine);
		if (AnnounceEcho && pSpawn && LastAnnouncedSpell != castSpell->ID)
		{
			WriteChatf("%s%s \aw--> %s", spellColor, castSpell->Name, pSpawn->DisplayedName);
		}
		if (AnnounceEQBC && pSpawn && LastAnnouncedSpell != castSpell->ID)
		{
			EQBCSwap(spellColor);
			sprintf_s(castLine, "/bc %s%s [+x+]--> %s", EQBCColor, castSpell->Name, pSpawn->DisplayedName);
			HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
		}
		LastAnnouncedSpell = castSpell->ID;
		if (GetSpellDuration(castSpell, (PSPAWNINFO)pLocalPlayer) > 0 && castSpell->TargetType != 6)
			AddTimer(pSpawn, castSpell, 0);
	}
}

void CheckTimer()
{
	char LastCast[MAX_STRING], spell1[MAX_STRING], spell2[MAX_STRING];
	for (unsigned int i = 0; i < vTimerSpawn.size(); i++)
	{
		if (!vTimerSpawn[i])
			continue;
		if (vTimerTimestamp[i] == 0)
			sprintf_s(LastCast, "${If[${Cast.Stored.ID} && (${Cast.Result.Equal[CAST_TAKEHOLD]}||${Cast.Result.Equal[CAST_SUCCESS]}),1,0]}");
		ParseMacroData(LastCast, MAX_STRING);
		if (atoi(LastCast) == 1)
		{
			strcpy_s(LastCast, "${Cast.Stored.ID}");
			ParseMacroData(LastCast, MAX_STRING);
			PSPELL pSpell = GetSpellByID(atol(LastCast));
			if (!pSpell)
				return;
			char test[MAX_STRING] = {0};
			SpellCategory(pSpell);
			strcpy_s(spell1, spellCat);
			SpellCategory(vTimerSpell[i]);
			strcpy_s(spell2, spellCat);
			if (pSpell && (!strcmp(pSpell->Name, vTimerSpell[i]->Name) || strcmp(spell1, "Unknown") && !strcmp(spell1, spell2)))
			{
				ULONGLONG dur = 0;
				sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", pSpell->Name);
				ParseMacroData(test, MAX_STRING);
				ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test);
				ULONGLONG recastTime = MQGetTickCount64() + test2;
				vTimerTimestamp[i] = (recastTime);
			}

		}

	}
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
	if (!GetCharInfo()->pXTargetMgr)
		return;
	char szXTAR[MAX_STRING];
	char szXTAR2[MAX_STRING];
	char testAddList[MAX_STRING];
	int xtar = 0;
	if (ExtendedTargetList* etl = GetCharInfo()->pXTargetMgr)
	{
		vAdds.clear();
		vPossibleAdds.clear();
		InCombat = false;
		int npc = 0;
		int leastaggro = 100;
		int mostaggro = 99;
		xNotTargetingMe = NULL;
		xTargetingMe = NULL;
		if (((PCPLAYERWND)pPlayerWnd))
			if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
				if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
					InCombat = true;

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
					if (pSpawn && (strstr(szXTAR, "NPC") || strstr(szXTAR, "Hater") || (strstr(szXTAR, "Target") || strstr(szXTAR, "target") || strstr(szXTAR, "_TARGET") && !strstr(szXTAR, "EMPTY")) && InCombat && pSpawn->Type == SPAWN_NPC && pSpawn->Type != SPAWN_CORPSE))
					{
						vAdds.push_back(pSpawn);
						SpawnTrackerIt = SpawnTracker.find((int)pSpawn->SpawnID);
						if (SpawnTrackerIt == SpawnTracker.end())
						{
							SpawnTrack(pSpawn);
						}
					}
					if (pAggroInfo)
					{
						if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct < leastaggro)
						{
							leastaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
							xNotTargetingMe = pSpawn;
						}
						if (pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct > mostaggro && !xTargetingMe)
						{
							mostaggro = pAggroInfo->aggroData[AD_xTarget1 + n].AggroPct;
							xTargetingMe = pSpawn;
						}
					}
					if (pSpawn && (strstr(szXTAR, "NPC") || strstr(szXTAR, "Target") || strstr(szXTAR, "target") || strstr(szXTAR, "_TARGET") && !strstr(szXTAR, "EMPTY")) && !InCombat && pSpawn->Type == SPAWN_NPC && pSpawn->Type != SPAWN_CORPSE)
						vPossibleAdds.push_back(pSpawn);
				}
			}
		}
	}
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
			vAdds.clear();
			vPossibleAdds.clear();
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
	if (UseNetBots)
	{
		sprintf_s(testAddList, "${If[${NetBots[%s].InZone} && ${NetBots.Client.Find[%s]} && ${NetBots[%s].TargetID} && ${NetBots[%s].TargetHP}<=%d && (${NetBots[%s].CombatState}==0||${NetBots[%s].Attacking}) && ${Spawn[id ${NetBots[%s].TargetID}].Distance}<=%d,1,0]}", NetBotsName, NetBotsName, NetBotsName, NetBotsName, AssistAt, NetBotsName, NetBotsName, NetBotsName, AssistRange);
		ParseMacroData(testAddList, MAX_STRING);
		if (atoi(testAddList) == 1)
		{
			sprintf_s(testAddList, "${NetBots[%s].TargetID}", NetBotsName);
			ParseMacroData(testAddList, MAX_STRING);
			unsigned int cap = vAdds.size();
			bool check = false;
			for (unsigned int i = 0; i < cap; i++)
			{
				if (vAdds[i] == (PSPAWNINFO)GetSpawnByID((DWORD)atol(testAddList)))
					check = true;
			}
			if (!check)
				vAdds.push_front((PSPAWNINFO)GetSpawnByID((DWORD)atol(testAddList)));
		}
	}
	if (vPossibleAdds.size() && !vAdds.size() && AnnounceAdds>0)
	{
		strcpy_s(testAddList, AddList);
		sprintf_s(szXTAR, "|%s|", vPossibleAdds[0]->Name);
		if (vPossibleAdds[0])
		{
			if (!strstr(testAddList, szXTAR))
			{
				sprintf_s(AddList, "|%s|", vPossibleAdds[0]->Name);
				WriteChatf("\ayPossible add: %s", vPossibleAdds[0]->Name);
			}
		}
	}
	if (vAdds.size() && AnnounceAdds > 0)
	{
		strcpy_s(szXTAR2, "|");
		if (vPossibleAdds.size())
			vAdds.push_front(vPossibleAdds[0]);
		for (unsigned int i = 0; i < vAdds.size(); i++)
		{
			if (vAdds[i] && vAdds[i]->Type != SPAWN_CORPSE)
			{
				ConColorSwap(vAdds[i]);
				sprintf_s(szXTAR, "%s%s\ar|", conColor, vAdds[i]->Name);
				strcat_s(szXTAR2, szXTAR);
			}

		}
		strcpy_s(testAddList, AddList);
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
		if (vAdds[0] && vAdds[0]->SpawnID != LastBodyID)
		{
			sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", vAdds[0]->Name);
			ParseMacroData(BodyTypeFix, MAX_STRING);
			LastBodyID = vAdds[0]->SpawnID;
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
}

void CheckAlias()
{
	char aliases[MAX_STRING] = { 0 };
	if (RemoveAlias("/bot"))
		strcat_s(aliases, " /bot ");
	if (RemoveAlias("/petgear"))
		strcat_s(aliases, " /petgear ");
	if (RemoveAlias("/campfire"))
		strcat_s(aliases, " /campfire ");
	if (RemoveAlias("/invis"))
		strcat_s(aliases, " /invis ");
	if (RemoveAlias("/ivu"))
		strcat_s(aliases, " /ivu ");
	if (RemoveAlias("/lev"))
		strcat_s(aliases, " /lev ");
	if (RemoveAlias("/imob"))
		strcat_s(aliases, " /imob ");
	if (RemoveAlias("/smob"))
		strcat_s(aliases, " /smob ");
	if (RemoveAlias("/mmob"))
		strcat_s(aliases, " /mmob ");
	if (RemoveAlias("/cmob"))
		strcat_s(aliases, " /cmob ");
	if (RemoveAlias("/coth"))
		strcat_s(aliases, " /coth ");
	if (RemoveAlias("/doorclick"))
		strcat_s(aliases, " /doorclick ");
	if (RemoveAlias("/immune"))
		strcat_s(aliases, " /immune ");
	if (RemoveAlias("/invitegroup"))
		strcat_s(aliases, " /invitegroup ");
	if (RemoveAlias("/inviteraid"))
		strcat_s(aliases, " /inviteraid ");
	if (strlen(aliases) > 0)
		WriteChatf("\ar[MQ2Bot]\ao::\ayWARNING\ao::\awAliases for \ao%s\aw were detected and temporarily removed.", aliases);
}

bool CheckInvisStatus()
{
	if (!InGameOK())
		return false;
	InvisCheck = false;
	for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++)
	{
		if (GetCharInfo2()->Buff[nBuff].SpellID > 0)
		{
			PSPELL tempSpell = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID);
			if (tempSpell && HasSpellAttrib(tempSpell, 12) || HasSpellAttrib(tempSpell, 314))
			{
				InvisCheck = true;
				return InvisCheck;
			}
		}
	}
	return InvisCheck;
}

bool CheckIVUStatus()
{
	if (!InGameOK())
		return false;
	IVUCheck = false;
	for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++)
	{
		if (GetCharInfo2()->Buff[nBuff].SpellID > 0)
		{
			PSPELL tempSpell = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID);
			if (tempSpell && HasSpellAttrib(tempSpell, 28) || HasSpellAttrib(tempSpell, 315))
			{
				IVUCheck = true;
				return IVUCheck;
			}
		}
	}
	return IVUCheck;
}

void CheckInterrupt()
{
	if (!InGameOK())
		return;
	if (BardClass || MQGetTickCount64() < RezTimer)
		return;
	if ((GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
	{
		PSPELL tempSpell = GetSpellByID(GetCharInfo()->pSpawn->CastingData.SpellID);
		if (tempSpell && tempSpell->TargetType == 0)
		{
			if (!vAdds.size())
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stopcast", FromPlugin);
				return;
			}
			if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stopcast", FromPlugin);
				return;
			}
			//DebugSpewAlways("Interrupt");
			/*if(vAdds[0])
			if(vAdds[0]->Type==SPAWN_CORPSE)
			{
			HideDoCommand(GetCharInfo()->pSpawn, "/stopcast", FromPlugin);
			return;
			}*/
		}
	}
}

void CheckMoveBack()
{
	if (!InGameOK())
		return;
	if (RepositionIfSummoned > 0)
	{
		PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
		if (GetDistance3D(pSpawn->X, pSpawn->Y, pSpawn->Z, FightX, FightY, FightZ) > 10)
		{
			char test[MAX_STRING];
			sprintf_s(test, "/moveto loc %f %f %f", FightY, FightX, FightZ);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			SpellTimer = MQGetTickCount64() + 2000LL;
		}
	}
}

void ClearTimers()
{
	//clear all vectors with spawns in them
	vAdds.clear();
	vPossibleAdds.clear();
	vBuffQSpawn.clear();
	vBuffQ.clear();
	vTimerTimestamp.clear();
	vTimerSpawn.clear();
	vTimerSpell.clear();
	vSpawnInZone.clear();
	vSpawnInZoneTimer.clear();
	//clear all spawns stored
	pCharFix = NULL;
	gMinMana = NULL;
	gAvgMana = NULL;
	gMinHP = NULL;
	gMinPetHP = NULL;
	KillTarget = NULL;
	BodyTypeSpawn = NULL;
	xMinHP = NULL;
	xMinPetHP = NULL;
	xMinMana = NULL;
	xPriority = NULL;
	nMinHP = NULL;
	nMinPetHP = NULL;
	CharmMob = NULL;
	gCotH = NULL;
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

bool EQBCConnected() {
	typedef WORD(__cdecl *fEqbcIsConnected)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fEqbcIsConnected checkf = (fEqbcIsConnected)GetProcAddress(pLook->hModule, "isConnected"))
			if (checkf()) return true;
	return false;
}

void EQBCSwap(char startColor[MAX_STRING])
{
	strcpy_s(EQBCColor, "[+x+]");
	for (unsigned int i = 0; ChatColors[i] && EQBCColors[i]; i++)
	{
		if (strstr(startColor, ChatColors[i]))
		{
			strcpy_s(EQBCColor, EQBCColors[i]);
			return;
		}
	}
}

float Face(PSPAWNINFO pSpawn)
{
	if (!InGameOK())
		return false;
	float fMyHeading = GetCharInfo()->pSpawn->Heading*0.703125f;
	float fSpawnHeadingTo = (FLOAT)(atan2f(((PSPAWNINFO)pCharSpawn)->Y - pSpawn->Y, pSpawn->X - ((PSPAWNINFO)pCharSpawn)->X) * 180.0f / PI + 90.0f);
	if (fSpawnHeadingTo < 0.0f)
		fSpawnHeadingTo += 360.0f;
	else if (fSpawnHeadingTo >= 360.0f)
		fSpawnHeadingTo -= 360.0f;
	facing = abs(fSpawnHeadingTo - fMyHeading);
	return facing;
}

float FaceMe(PSPAWNINFO pSpawn)
{
	char test[MAX_STRING];
	if (!pSpawn)
		return 0.0;
	int eval = 0;
	sprintf_s(test, "${Math.Abs[${Math.Calc[${Spawn[%d].HeadingTo.Degrees}-${Spawn[%d].Heading.Degrees}].Int}]}", pSpawn->SpawnID, pSpawn->SpawnID);
	ParseMacroData(test, MAX_STRING);
	facing = (float)atof(test);
	return facing;
}

void FindModRodInfo(PSPAWNINFO pSpawn)
{

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

char* GetBuild()
{
	static char Buffer[MAX_STRING];
	char *szMonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	char szDate[256], szTime[256];
	strcpy_s(szDate, __DATE__);
	strcpy_s(szTime, __TIME__);
	Buffer[0] = 0;

	char TmpBuf[256] = { 0 }, tMon[64] = { 0 }, *pTmpBuf = NULL, *pTmpBufNext = NULL;
	int iYear = 0, iMon = 0, iDay = 0, iHour = 0, iMin = 0, iSec = 0, j;
	sprintf_s(TmpBuf, "%s %s", __DATE__, __TIME__);
	pTmpBuf = strtok_s(TmpBuf, " ", &pTmpBufNext);
	if (pTmpBuf) {
		strcpy_s(tMon, pTmpBuf);
		for (j = 0; j < 12; j++) {
			if (!_strnicmp(tMon, szMonth[j], 3)) {
				iMon = j + 1;
			}
		}
		if (iMon >= 1 && iMon <= 12)
		{
			pTmpBuf = strtok_s(NULL, " ", &pTmpBufNext);
			if (pTmpBuf)
			{
				iDay = atoi(pTmpBuf);
				pTmpBuf = strtok_s(NULL, " ", &pTmpBufNext);
				if (pTmpBuf)
				{
					iYear = atoi(pTmpBuf);
					pTmpBuf = strtok_s(NULL, ":", &pTmpBufNext);
					if (pTmpBuf)
					{
						iHour = atoi(pTmpBuf);
						pTmpBuf = strtok_s(NULL, ":", &pTmpBufNext);
						if (pTmpBuf)
						{
							iMin = atoi(pTmpBuf);
							pTmpBuf = strtok_s(NULL, ":", &pTmpBufNext);
							if (pTmpBuf)
							{
								iSec = atoi(pTmpBuf);
							}
						}
					}
				}
			}
		}
	}
	sprintf_s(Buffer, "%d%02d%02d.%02d%02d", iYear, iMon, iDay, iHour, iMin);
	botVersion = atof(Buffer);
	return(Buffer);
}

PSPAWNINFO GetKillTarget()
{
	if (!InGameOK())
		return NULL;
	PSPAWNINFO pFail = NULL;
	if (MQGetTickCount64() < TargetTimer)
		return pFail;
	if (((PCPLAYERWND)pPlayerWnd))
		if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
			if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
				InCombat = true;
	char AddTest[MAX_STRING];
	if (OffTank)
	{
		sprintf_s(AddTest, "${If[${Me.XTarget[%d].ID} && ${Me.XTarget[%d].Distance}<%d && ${Me.XTarget[%d].PctHPs}<=%d,1,0]}", OffTank, OffTank, AssistRange, OffTank, AssistAt);
		ParseMacroData(AddTest, MAX_STRING);
		if (atoi(AddTest) == 1)
		{
			sprintf_s(AddTest, "${Me.XTarget[%d].ID}", OffTank);
			ParseMacroData(AddTest, MAX_STRING);
			PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(atol(AddTest));
			if (InCombat)
			{
				if (pSpawn && (BodyTypeSpawn == NULL || BodyTypeSpawn != pSpawn))
				{
					BodyTypeSpawn = pSpawn;
					sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", pSpawn->Name);
					ParseMacroData(BodyTypeFix, MAX_STRING);
					WriteChatColor(BodyTypeFix, USERCOLOR_DEFAULT);
				}
				return pSpawn;
			}
			if (!InCombat && pSpawn)
			{
				if (!vPossibleAdds.size() && !vAdds.size())
					return pFail;
				TargetIT(pSpawn);
				PCHAR pTargetAggroHolder = EQADDR_TARGETAGGROHOLDER;
				if (pTargetAggroHolder[0] != '\0')
				{
					PSPAWNINFO pAggroHolder = (PSPAWNINFO)GetSpawnByName(pTargetAggroHolder);
					if (pAggroHolder)
					{
						char test[MAX_STRING];
						sprintf_s(test, "${If[(${SpawnCount[group ${Spawn[%s]}]}||${SpawnCount[group ${Spawn[%s].Master}]}||${SpawnCount[group ${Spawn[%s].Owner}]}),1,0]}", pAggroHolder->DisplayedName, pAggroHolder->DisplayedName, pAggroHolder->DisplayedName);
						ParseMacroData(test, MAX_STRING);
						if (atoi(test) == 1)
						{
							if (pSpawn && (BodyTypeSpawn == NULL || BodyTypeSpawn != pSpawn))
							{
								BodyTypeSpawn = pSpawn;
								sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", pSpawn->Name);
								ParseMacroData(BodyTypeFix, MAX_STRING);
								WriteChatColor(BodyTypeFix, USERCOLOR_DEFAULT);
							}
							return pSpawn;
						}
					}
				}
			}
		}
	}
	sprintf_s(AddTest, "${If[${Me.XTarget[1].ID} && ${Me.XTarget[1].Distance}<%d && ${Me.XTarget[1].PctHPs}<=%d,1,0]}", AssistRange, AssistAt);
	ParseMacroData(AddTest, MAX_STRING);
	if (atoi(AddTest) == 1)
	{
		strcpy_s(AddTest, "${Me.XTarget[1].ID}");
		ParseMacroData(AddTest, MAX_STRING);
		PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(atol(AddTest));
		if (InCombat)
		{
			if (pSpawn && (BodyTypeSpawn == NULL || BodyTypeSpawn != pSpawn))
			{
				BodyTypeSpawn = pSpawn;
				sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", pSpawn->Name);
				ParseMacroData(BodyTypeFix, MAX_STRING);
				WriteChatColor(BodyTypeFix, USERCOLOR_DEFAULT);
			}
			return pSpawn;
		}
		if (!InCombat && pSpawn)
		{
			if (!vPossibleAdds.size() && !vAdds.size())
				return pFail;
			TargetIT(pSpawn);
			PCHAR pTargetAggroHolder = EQADDR_TARGETAGGROHOLDER;
			if (pTargetAggroHolder[0] != '\0')
			{
				PSPAWNINFO pAggroHolder = (PSPAWNINFO)GetSpawnByName(pTargetAggroHolder);
				if (pAggroHolder)
				{
					char test[MAX_STRING];
					sprintf_s(test, "${If[(${SpawnCount[group ${Spawn[%s]}]}||${SpawnCount[group ${Spawn[%s].Master}]}||${SpawnCount[group ${Spawn[%s].Owner}]}),1,0]}", pAggroHolder->DisplayedName, pAggroHolder->DisplayedName, pAggroHolder->DisplayedName);
					ParseMacroData(test, MAX_STRING);
					if (atoi(test) == 1)
					{
						if (pSpawn && (BodyTypeSpawn == NULL || BodyTypeSpawn != pSpawn))
						{
							BodyTypeSpawn = pSpawn;
							sprintf_s(BodyTypeFix, "${Spawn[%s].Body}", pSpawn->Name);
							ParseMacroData(BodyTypeFix, MAX_STRING);
							WriteChatColor(BodyTypeFix, USERCOLOR_DEFAULT);
						}
						return pSpawn;
					}
				}
			}
		}
	}
	TargetTimer = MQGetTickCount64() + 1000LL;
	return pFail;
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

ULONGLONG GTC64()
{
	// return milliseconds passed since Jan 1, 2012
	// assume all months are 30 days
	// it doesn't need to be exact with relation to actual days of month, or days in year, just a repeatable reliable timestamp in ms
	ULONGLONG RetVal = 0;
	SYSTEMTIME st;
	GetSystemTime(&st);
	if (st.wYear > 2012)
		RetVal += (ULONGLONG)(st.wYear - 2012) * 365 * 30 * 24 * 60 * 60 * 1000;
	RetVal += (ULONGLONG)(st.wMonth - 1) * 30 * 24 * 60 * 60 * 1000;
	RetVal += (ULONGLONG)(st.wDay - 1) * 24 * 60 * 60 * 1000;
	RetVal += (ULONGLONG)st.wHour * 60 * 60 * 1000;
	RetVal += (ULONGLONG)st.wMinute * 60 * 1000;
	RetVal += (ULONGLONG)st.wSecond * 1000;
	RetVal += (ULONGLONG)st.wMilliseconds;
	return RetVal;
}
/*
bool GroupCheck(char searchtype[MAX_STRING])
{
if (!InGameOK())
return true;
PCHARINFO pChar = GetCharInfo();
if (!pChar)
return false;
if (pChar && !pChar->pGroupInfo)
return false;
char ClassList[MAX_STRING];
_strlwr_s(searchtype);
if (strstr(searchtype, "slower"))
strcpy_s(ClassList, "|BRD|BST|ENC|SHM|");
if (strstr(searchtype, "priest"))
strcpy_s(ClassList, "|CLR|DRU|SHM|");
if (strstr(searchtype, "healer"))
strcpy_s(ClassList, "|CLR|DRU|SHM|PAL|BST|RNG|");
if (strstr(searchtype, "cc"))
strcpy_s(ClassList, "|BRD|ENC|");
if (strstr(searchtype, "tank"))
strcpy_s(ClassList, "|WAR|SHD|PAL|");
if (strstr(searchtype, "rez"))
strcpy_s(ClassList, "|CLR|DRU|SHM|PAL|");
if (strstr(searchtype, "malo"))
strcpy_s(ClassList, "|SHM|MAG|");
if (strstr(searchtype, "snarer"))
strcpy_s(ClassList, "|WIZ|RNG|DRU|NEC|");
if (strstr(searchtype, "stunner"))
strcpy_s(ClassList, "|CLR|PAL|ENC|");


}
*/

bool GroupFarthest(int rangeCheck)
{
	if (!InGameOK())
		return true;
	PCHARINFO pChar = GetCharInfo();
	if (pChar && !pChar->pGroupInfo)
		return true;
	long fDistance = 0;
	float iCheck = 0;
	float iFarthest = 0;
	gCotH = NULL;
	PSPAWNINFO iMember = NULL;
	PSPAWNINFO pGroupMember;
	if (!rangeCheck)
		fDistance = 0;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	for (unsigned int i = 1; i < 6; i++)
	{
		sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}),1,0]}", i);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			if (pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
			{
				if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER)
				{
					pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
					if (pGroupMember && GetDistance(pGroupMember->X, pGroupMember->Y) > fDistance && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
					{
						iCheck = GetDistance(pGroupMember->X, pGroupMember->Y);
						if (iCheck > iFarthest)
						{
							if (pGroupMember)
							{
								iFarthest = iCheck;
								gCotH = pGroupMember;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool GroupMinMana(int rangeCheck)
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	gAvgMana = 0;
	int gManaCount = 0;
	if (!pChar)
		return false;
	CHAR sClassList[MAX_STRING] = "|CLR|DRU|SHM|ENC|NEC|MAG|WIZ|BST|PAL|SHD|RNG|";
	if (!pChar->pGroupInfo && !strstr(sClassList, pEverQuest->GetClassThreeLetterCode(pChar->pSpawn->mActorClient.Class)))
		return false;
	if (pChar && !pChar->pGroupInfo && strstr(sClassList, pEverQuest->GetClassThreeLetterCode(pChar->pSpawn->mActorClient.Class)))
	{
		gMinMana = GetCharInfo()->pSpawn;
		gAvgMana = PctMana(GetCharInfo()->pSpawn);
		return true;
	}
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	PSPAWNINFO pGroupMember;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	for (unsigned int i = 0; i < 6; i++)
	{
		sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn)
			{
				if (strstr(sClassList, pEverQuest->GetClassThreeLetterCode(pChar->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)) && (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->pSpawn->Mercenary))
				{
					pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
					if (pGroupMember && GetDistance(pGroupMember->X, pGroupMember->Y) < fDistance && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
					{
						if (pGroupMember)
						{

							if (unsigned long maxmana = pGroupMember->ManaMax)
								iCheck = maxmana > 0 ? pGroupMember->ManaCurrent * 100 / maxmana : 0;
							else
								iCheck = 0;
							gAvgMana = gAvgMana + iCheck;
							gManaCount++;
							if (iCheck < iLowest)
							{
								if (pGroupMember)
								{
									iLowest = iCheck;
									gMinMana = pGroupMember;
								}
							}
						}
					}
				}
			}
		}
		gAvgMana = gManaCount > 0 ? (int(gAvgMana / gManaCount)) : 0;
	}
	return false;
}

bool GroupMinHP(int rangeCheck)
{
	if (!InGameOK())
		return true;
	PCHARINFO pChar = GetCharInfo();
	gAvgHP = 0;
	int gHPCount = 0;
	if (pChar && !pChar->pGroupInfo)
	{
		gMinHP = GetCharInfo()->pSpawn;
		gAvgHP = PctHP(GetCharInfo()->pSpawn);
		return true;
	}
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	PSPAWNINFO pGroupMember;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	for (unsigned int i = 0; i < 6; i++)
	{
		sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
			{
				if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->Mercenary)
				{
					pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
					if (pGroupMember && GetDistance(pGroupMember->X, pGroupMember->Y) < fDistance && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
					{
						//SpawnTimer(pGroupMember);
						if (unsigned long maxhp = pGroupMember->HPMax)
							iCheck = maxhp > 0 ? pGroupMember->HPCurrent * 100 / maxhp : 0;
						else
							iCheck = 0;
						gAvgHP = gAvgHP + iCheck;
						gHPCount++;
						if (iCheck < iLowest)
						{
							if (pGroupMember)
							{
								iLowest = iCheck;
								gMinHP = pGroupMember;
							}
						}
					}
				}
			}
		}
		gAvgHP = gHPCount > 0 ? (int(gAvgHP / gHPCount)) : 0;
	}
	return false;
}

bool GroupMinPetHP(int rangeCheck)
{
	if (!InGameOK())
		return false;
	gMinPetHP = NULL;
	PCHARINFO pChar = GetCharInfo();
	if (pChar && !pChar->pGroupInfo)
	{
		if (pChar->pSpawn && pChar->pSpawn->PetID && pChar->pSpawn->PetID != 0xFFFFFFFF)
		{
			gMinPetHP = (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
			if (GetSpawnType(gMinPetHP) == 3)
				return true;
			else
				return false;
		}
		return false;
	}
	int fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	PSPAWNINFO pGroupMember;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	for (unsigned int i = 0; i < 6; i++)
	{
		sprintf_s(test, "${If[${Group.Member[%d].Type.Equal[PC]} && ${Group.Member[%d].Pet.ID} && ${Group.Member[%d].Pet.Distance}<%d && ${Group.Member[%d].Pet.Type.Equal[pet]},1,0]}", i, i, i, fDistance, i);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && !pChar->pGroupInfo->pMember[i]->Mercenary)
			{
				if (pChar->pGroupInfo->pMember[i]->pSpawn->PetID && pChar->pGroupInfo->pMember[i]->pSpawn->PetID != 0xFFFFFFFF)
				{
					pGroupMember = (PSPAWNINFO)GetSpawnByID(pChar->pGroupInfo->pMember[i]->pSpawn->PetID);
					if (pGroupMember)
					{
						//SpawnTimer(pGroupMember);
						if (unsigned long maxhp = pGroupMember->HPMax)
							iCheck = maxhp > 0 ? pGroupMember->HPCurrent * 100 / maxhp : 0;
						else
							iCheck = 0;
						if (iCheck < iLowest)
						{
							if (pGroupMember = (PSPAWNINFO)GetSpawnByID(pChar->pGroupInfo->pMember[i]->pSpawn->PetID))
							{
								iLowest = iCheck;
								gMinPetHP = pGroupMember;
							}
						}

					}
				}
			}
		}
	}
	return false;
}

int HPCount(int HPLevel)
{
	if (!InGameOK())
		return true;
	HPcount = 0;
	PCHARINFO pChar = GetCharInfo();
	if (pChar && !pChar->pGroupInfo)
	{
		if (PctHP(GetCharInfo()->pSpawn) < HPLevel)
			return 1;
		else
			return 0;
	}
	long fDistance = 100;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	PSPAWNINFO pGroupMember;
	char test[MAX_STRING];
	for (unsigned int i = 0; i < 6; i++)
	{
		sprintf_s(test, "${If[(${Group.Member[%d].Type.Equal[PC]}||${Group.Member[%d].Type.Equal[mercenary]}),1,0]}", i, i);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			if (pChar && pChar->pGroupInfo && pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->pSpawn && pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
			{
				if (pChar->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || pChar->pGroupInfo->pMember[i]->Mercenary)
				{
					pGroupMember = pChar->pGroupInfo->pMember[i]->pSpawn;
					if (pGroupMember && GetDistance(pGroupMember->X, pGroupMember->Y) < fDistance && !pGroupMember->RespawnTimer && pGroupMember->StandState != STANDSTATE_DEAD)
					{
						//SpawnTimer(pGroupMember);
						if (unsigned long maxhp = pGroupMember->HPMax)
							iCheck = maxhp > 0 ? pGroupMember->HPCurrent * 100 / maxhp : 0;
						else
							iCheck = 0;
						if (iCheck < HPLevel)
						{
							if (pGroupMember)
							{
								HPcount++;
							}
						}
					}
				}
			}
		}
	}
	return HPcount;
}

PSPAWNINFO GroupMainTank()
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	PSPAWNINFO pFail = NULL;
	if (pChar && !pChar->pGroupInfo)
		return false;
	if (pChar->pGroupInfo->pMember[0]->MainTank)
	{
		if ((pChar->pSpawn->mActorClient.Class == 11 || pChar->pSpawn->mActorClient.Class == 13) && pChar->pSpawn->PetID && pChar->pSpawn->PetID != 0xFFFFFFFF)
			return (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
		else
			return pChar->pGroupInfo->pMember[0]->pSpawn;
	}
	for (unsigned int i = 1; i < 6; i++)
	{
		if (pChar->pGroupInfo->pMember[i])
		{
			if (pChar->pGroupInfo->pMember[i]->MainTank)
			{
				if (PSPAWNINFO pSpawn = pChar->pGroupInfo->pMember[i]->pSpawn)
				{
					if (pSpawn && pSpawn->Type != SPAWN_CORPSE)
					{
						if ((pChar->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class == 11 || pChar->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class == 13) && pChar->pGroupInfo->pMember[i]->pSpawn->PetID && pChar->pGroupInfo->pMember[i]->pSpawn->PetID != 0xFFFFFFFF)
							return (PSPAWNINFO)GetSpawnByID(pChar->pGroupInfo->pMember[i]->pSpawn->PetID);
						return pChar->pGroupInfo->pMember[i]->pSpawn;
					}
				}
			}
		}
	}
	return pFail;
}

PSPAWNINFO GroupPuller()
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	PSPAWNINFO pFail = NULL;
	if (pChar && !pChar->pGroupInfo)
		return false;
	if (pChar->pGroupInfo->pMember[0]->Puller)
	{
		return pChar->pGroupInfo->pMember[0]->pSpawn;
	}
	for (unsigned int i = 1; i < 6; i++)
	{
		if (pChar->pGroupInfo->pMember[i])
		{
			if (pChar->pGroupInfo->pMember[i]->Puller)
			{
				if (PSPAWNINFO pSpawn = pChar->pGroupInfo->pMember[i]->pSpawn)
				{
					if (pSpawn && pSpawn->Type != SPAWN_CORPSE)
					{
						return pChar->pGroupInfo->pMember[i]->pSpawn;
					}
				}
			}
		}
	}
	return pFail;
}

bool IfCheck(char routine[MAX_STRING])
{
	bool valid = true;
	char test[MAX_STRING] = "";
	sprintf_s(test, "%sIf", routine);
	SpellIt = SpellIf.find(test);
	if (SpellIt != SpellIf.end())
	{
		sprintf_s(test, "${If[(%s),1,0]}", SpellIt->second.c_str());
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 0)
			valid = false;
	}
	return valid;
}

LONG ItemTimer(PCONTENTS myItem)
{
	if (!myItem)
		return 999999;
	PITEMINFO p = GetItemFromContents(myItem);
	if (!p)
		return 999999;
	if (p->Clicky.TimerID != ULONG_MAX)
		return GetItemTimer(myItem);
	if (p->Clicky.SpellID != ULONG_MAX)
		return 0;
	return 999999;
}

void InvisProximity(PSPAWNINFO pSpawn, FLOAT Radius)
{
	invis = 0;
	PSPAWNINFO pClose = NULL;
	if (ppSpawnManager && pSpawnList)
	{
		pClose = (PSPAWNINFO)pSpawnList;
	}
	while (pClose)
	{
		if (GetSpawnType(pClose) == NPC && !strlen(pClose->Lastname))
		{
			if (pClose != pSpawn && DistanceToSpawn(pClose, pSpawn) < Radius && !strstr(IgnoreList, pClose->DisplayedName) && GetBodyType(pClose) != 3)
			{
				invis = 1;
				return;
			}
		}
		pClose = pClose->pNext;
	}
	return;
}

void IVUProximity(PSPAWNINFO pSpawn, FLOAT Radius)
{
	ivu = 0;
	PSPAWNINFO pClose = NULL;
	if (ppSpawnManager && pSpawnList)
	{
		pClose = (PSPAWNINFO)pSpawnList;
	}
	while (pClose)
	{
		if (GetSpawnType(pClose) == NPC && !strlen(pClose->Lastname))
		{
			if (pClose != pSpawn && DistanceToSpawn(pClose, pSpawn) < Radius && !strstr(IgnoreList, pClose->DisplayedName) && GetBodyType(pClose) == 3)
			{
				ivu = 1;
				return;
			}
		}
		pClose = pClose->pNext;
	}
	return;
}

bool NetBotsMinHP(int rangeCheck)
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	strcpy_s(test, "${If[${NetBots.Counts},${NetBots.Counts},0]}");
	ParseMacroData(test, MAX_STRING);
	int loop = atoi(test);
	for (int i = 1; i <= loop; i++)
	{
		sprintf_s(test, "${If[${NetBots[${NetBots.Client[%d]}].InZone} && !${NetBots[${NetBots.Client[%d]}].InGroup} && ${Spawn[${NetBots[${NetBots.Client[%d]}].ID}].Distance}<%d && ${NetBots[${NetBots.Client[%d]}].PctHPs}<%d,${NetBots[${NetBots.Client[%d]}].ID},0]}", i, i, i, fDistance, i, iLowest, i);
		ParseMacroData(test, MAX_STRING);
		if (atol(test) > 1)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(test)))
			{
				sprintf_s(test, "${NetBots[${NetBots.Client[%d]}].PctHPs}", i);
				ParseMacroData(test, MAX_STRING);
				iCheck = atoi(test);
				if (iCheck < iLowest)
				{
					if (pSpawn)
					{
						iLowest = iCheck;
						NetBotsHP = iCheck;
						nMinHP = pSpawn;
					}

				}
			}
		}
	}
	return false;
}

double botround(double d)
{
	return floor(d + 0.5);
}

bool SpellStacks(PSPELL pSpell)
{
	if (!InGameOK())
		return true;
	if (pSpell == nullptr || !pSpell)
		return true;
	unsigned long nBuff;
	PCHARINFO2 pChar = GetCharInfo2();
	bool stacks = true;
	PSPELL tmpSpell2;
	// Check Buffs
	for (nBuff = 0; nBuff < NUM_LONG_BUFFS && pChar; nBuff++)
	{
		if (pChar && pChar->Buff[nBuff].SpellID>0 && GetSpellByID(pChar->Buff[nBuff].SpellID)) {
			PSPELL tmpSpell = GetSpellByID(pChar->Buff[nBuff].SpellID);
			if (tmpSpell && pSpell && tmpSpell != nullptr && pSpell != nullptr)
			{
				for (int nSlot = 0; nSlot < GetSpellNumEffects(pSpell); nSlot++)
				{
					int attrib = GetSpellAttrib(pSpell, nSlot);

					if (attrib == 374 || attrib == 419 || attrib == 339)
					{
						tmpSpell2 = GetSpellByID(GetSpellBase2(pSpell, nSlot));
						if (tmpSpell2 && tmpSpell2 != nullptr)
						{
							if (!BuffStackTest(tmpSpell, tmpSpell2) || !outdoor && tmpSpell2->ZoneType == 1 || tmpSpell == tmpSpell2 || attrib == 339 && GetSpellAttribX(tmpSpell, nSlot) == 339 || strstr(pSpell->Name, tmpSpell->Name))
							{
								stacks = false;
								return stacks;
							}
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
		if (pChar->ShortBuff[nBuff].SpellID>0) {
			PSPELL tmpSpell = GetSpellByID(pChar->ShortBuff[nBuff].SpellID);
			if (tmpSpell && tmpSpell != nullptr && pSpell && (!(tmpSpell->ClassLevel[Bard] == 255 ? 0 : tmpSpell->ClassLevel[Bard]) && (!(GetSpellAttribX(pSpell, 0) == 58) || !(pSpell->DurationWindow == 1))))
			{
				for (int nSlot = 0; nSlot < GetSpellNumEffects(pSpell); nSlot++)
				{
					int attrib = GetSpellAttrib(pSpell, nSlot);

					if (attrib == 374 || attrib == 419)
					{
						tmpSpell2 = GetSpellByID(GetSpellBase2(pSpell, nSlot));
						if (tmpSpell2 && tmpSpell2 != nullptr && !BuffStackTest(tmpSpell, tmpSpell2) || pSpell == tmpSpell2 || tmpSpell == tmpSpell2 || strstr(pSpell->Name, tmpSpell->Name))
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
				if (pSpell && tmpSpell && pSpell != nullptr && tmpSpell != nullptr) {
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

ULONGLONG SpawnTimer(PSPAWNINFO pSpawn)
{
	if (pSpawn)
	{
		bool found = false;
		bool timerOK = true;
		for (unsigned int x = 0; x < vSpawnInZone.size(); x++)
		{
			if (!vSpawnInZone[x])
				continue;
			if (vSpawnInZone[x]->Type == SPAWN_CORPSE || !vSpawnInZone[x])
			{
				vSpawnInZone.erase(vSpawnInZone.begin() + x);
				vSpawnInZoneTimer.erase(vSpawnInZoneTimer.begin() + x);
			}
			if (vSpawnInZone[x] == pSpawn)
			{
				found = true;
				return (MQGetTickCount64() - vSpawnInZoneTimer[x]);
			}
		}
		if (!found)
		{
			vSpawnInZone.push_back(pSpawn);
			vSpawnInZoneTimer.push_back(MQGetTickCount64());
			timerOK = false;
			return 0;
		}
	}
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

void SpawnTrack(PSPAWNINFO pSpawn)
{
	// return; // make sure it isnt this
	if (pSpawn && pSpawn->Type != SPAWN_CORPSE)
	{
		int done = 0;
		while (pSpawn && pSpawn->Type != SPAWN_CORPSE && !done)
		{
			SpawnTrackerIt = SpawnTracker.find((int)pSpawn->SpawnID);
			if (SpawnTrackerIt == SpawnTracker.end())
			{
				_Spawns SpawnID;
				SpawnID.ID = (int)pSpawn->SpawnID;
				// SpawnID.Spawn = pSpawn;
				SpawnID.NeedsCheck = true;
				SpawnID.LastChecked = 0;
				SpawnID.Slow = 0;  //mq2 time stamp of when slow should last until, repeat et al for rest
				SpawnID.Malo = 0;
				SpawnID.Tash = 0;
				SpawnID.Haste = 0;
				SpawnID.Root = 0;
				SpawnID.Snare = 0;
				SpawnID.Mez = 0;
				SpawnID.DS = 0;
				SpawnID.RevDS = 0;
				SpawnID.Cripple = 0;
				SpawnID.Charge = 0;
				SpawnID.Concuss = 0;
				SpawnID.MindFroze = 0;
				SpawnID.Charm = 0;
				SpawnID.Aego = 0;
				SpawnID.Skin = 0;
				SpawnID.Focus = 0;
				SpawnID.Regen = 0;
				SpawnID.Symbol = 0;
				SpawnID.Clarity = 0;
				SpawnID.Pred = 0;
				SpawnID.Strength = 0;
				SpawnID.Brells = 0;
				SpawnID.SV = 0;
				SpawnID.SE = 0;
				SpawnID.HybridHP = 0;
				SpawnID.Growth = 0;
				SpawnID.Shining = 0;
				SpawnID.DeepSleep = 0;
				SpawnID.PoisonCounters = 0;
				SpawnID.DiseaseCounters = 0;
				SpawnID.CorruptedCounters = 0;
				SpawnID.CurseCounters = 0;
				SpawnID.DetrimentalCounters = 0;
				SpawnID.Hot = 0;
				SpawnID.Fero = 0;
				SpawnTracker.insert(make_pair<int, _Spawns>((int)pSpawn->SpawnID, (_Spawns)SpawnID));
				//WriteChatf("\awAdding SpawnID: \ay%d \awto tracker.", (int)pSpawn.SpawnID);
				DebugWrite("\awAdding SpawnID: \ay%d \awto tracker.", pSpawn ? (int)pSpawn->SpawnID : -1);

#if 0
				SpawnTrackerIt = SpawnTracker.find(0);
				if (SpawnTrackerIt != SpawnTracker.end())
				{
					SpawnTracker.erase(SpawnTrackerIt);
					DebugWrite("DEBUG: Found a 0 SpawnID, removing.");
				}
#endif // 0
			}
			done = 1;
		}
	}
}

void SpawnTrackGroup()
{
	if (!InGameOK())
		return;
	SpawnTrackGroupTimer = MQGetTickCount64() + 10000LL;
	for (unsigned int i = 0; i < 6; i++)
	{
		if (GetCharInfo() && GetCharInfo()->pGroupInfo && GetCharInfo()->pGroupInfo->pMember[i] && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
		{
			if (GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || GetCharInfo()->pGroupInfo->pMember[i]->Mercenary)
			{
				SpawnTrack(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
			}
		}
	}
}

void SpawnTrackRemove()
{

#if 0
	SpawnTrackerIt = SpawnTracker.find(0);
	if (SpawnTrackerIt != SpawnTracker.end())
	{
		SpawnTracker.erase(SpawnTrackerIt);
		DebugWrite("DEBUG: Found a 0 SpawnID, removing.");
	}
#endif // 0

	for (std::map<int, _Spawns>::iterator it = SpawnTracker.begin(); it != SpawnTracker.end();)
	{
		if (it->first > 0)  //i shouldnt have to add this but maybe because it was passing a 0.
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)it->first))
			{
				++it;
			}
			else
			{
				DebugWrite("Spawn ID %d no longer exists, removing.", it->first);
				SpawnTracker.erase(it++);
			}
		}
	}
}

void SpawnTrackReport(PSPAWNINFO pSpawn)
{
	SpawnTrackerIt = SpawnTracker.find((int)pSpawn->SpawnID);
	if (SpawnTrackerIt != SpawnTracker.end())
	{
		WriteChatf("Spawn info for id %d with current time %llu:", SpawnTrackerIt->second.ID, MQGetTickCount64());
		if (SpawnTrackerIt->second.Slow)
			WriteChatf("Slow timestamp: %llu", SpawnTrackerIt->second.Slow);
		if (SpawnTrackerIt->second.Malo)
			WriteChatf("Malo timestamp: %llu", SpawnTrackerIt->second.Malo);
		if (SpawnTrackerIt->second.Tash)
			WriteChatf("Tash timestamp: %llu", SpawnTrackerIt->second.Tash);
		if (SpawnTrackerIt->second.Haste)
			WriteChatf("Haste timestamp: %llu", SpawnTrackerIt->second.Haste);
		if (SpawnTrackerIt->second.Root)
			WriteChatf("Root timestamp: %llu", SpawnTrackerIt->second.Root);
		if (SpawnTrackerIt->second.Mez)
			WriteChatf("Mez timestamp: %llu", SpawnTrackerIt->second.Mez);
		if (SpawnTrackerIt->second.DS)
			WriteChatf("DS timestamp: %llu", SpawnTrackerIt->second.DS);
		if (SpawnTrackerIt->second.Cripple)
			WriteChatf("Cripple timestamp: %llu", SpawnTrackerIt->second.Cripple);
		if (SpawnTrackerIt->second.Aego)
			WriteChatf("Aego timestamp: %llu", SpawnTrackerIt->second.Aego);
		if (SpawnTrackerIt->second.Skin)
			WriteChatf("Skin timestamp: %llu", SpawnTrackerIt->second.Skin);
		if (SpawnTrackerIt->second.Focus)
			WriteChatf("Focus timestamp: %llu", SpawnTrackerIt->second.Focus);
		if (SpawnTrackerIt->second.Regen)
			WriteChatf("Regen timestamp: %llu", SpawnTrackerIt->second.Regen);
		if (SpawnTrackerIt->second.Symbol)
			WriteChatf("Symbol timestamp: %llu", SpawnTrackerIt->second.Symbol);
		if (SpawnTrackerIt->second.Clarity)
			WriteChatf("Clarity timestamp: %llu", SpawnTrackerIt->second.Clarity);
		if (SpawnTrackerIt->second.Pred)
			WriteChatf("Pred timestamp: %llu", SpawnTrackerIt->second.Pred);
		if (SpawnTrackerIt->second.Strength)
			WriteChatf("Strength timestamp: %llu", SpawnTrackerIt->second.Strength);
		if (SpawnTrackerIt->second.Brells)
			WriteChatf("Brells timestamp: %llu", SpawnTrackerIt->second.Brells);
		if (SpawnTrackerIt->second.SV)
			WriteChatf("SV timestamp: %llu", SpawnTrackerIt->second.SV);
		if (SpawnTrackerIt->second.SE)
			WriteChatf("SE timestamp: %llu", SpawnTrackerIt->second.SE);
		if (SpawnTrackerIt->second.HybridHP)
			WriteChatf("HybridHP timestamp: %llu", SpawnTrackerIt->second.HybridHP);
		if (SpawnTrackerIt->second.Growth)
			WriteChatf("Growth timestamp: %llu", SpawnTrackerIt->second.Growth);
		if (SpawnTrackerIt->second.Shining)
			WriteChatf("Shining timestamp: %llu", SpawnTrackerIt->second.Shining);
		if (SpawnTrackerIt->second.Hot)
			WriteChatf("Hot timestamp: %llu", SpawnTrackerIt->second.Hot);
		if (SpawnTrackerIt->second.Fero)
			WriteChatf("Fero timestamp: %llu", SpawnTrackerIt->second.Fero);
	}
}

void SpawnTrackUpdate(PSPAWNINFO pSpawn, int type)  // 1 = self, 2 = group, 3 = needs checked
{
	SpawnTrackerIt = SpawnTracker.find((int)pSpawn->SpawnID);
	if (SpawnTrackerIt != SpawnTracker.end())
	{
		if (pSpawn && type != 1)
			if (!(((PCTARGETWND)pTargetWnd)->Type > 0))
				return;
		SpawnTrackerIt->second.LastChecked = MQGetTickCount64() + 3000LL;
		SpawnTrackerIt->second.NeedsCheck = false;
		int buffID;
		ULONGLONG buffDur = 0;
		for (unsigned int i = 0; i < NUM_BUFF_SLOTS; i++)
		{
			if (type != 1)
				buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
			else
				buffID = GetCharInfo2()->Buff[i].SpellID;
			if (buffID > 0 && buffID != -1)
			{
				if (type != 1)
				{
					buffDur = ((PCTARGETWND)pTargetWnd)->BuffTimer[i] * 1000;
				}
				else
				{
					buffDur = GetCharInfo2()->Buff[i].Duration * 1000;
				}
				PSPELL tempSpell = GetSpellByID(buffID);
				SpellCategory(tempSpell);
				int poison = 0, disease = 0, corruption = 0, curse = 0, detrimental = 0;
				for (int x = 0; x < GetSpellNumEffects(tempSpell); x++) {
					if (GetSpellAttrib(tempSpell, x) == 35)
						disease = disease + (int)GetSpellBase(tempSpell, x);
					if (GetSpellAttrib(tempSpell, x) == 36 && !strstr(tempSpell->Name, "Tash"))
						poison = poison + (int)GetSpellBase(tempSpell, x);
					if (GetSpellAttrib(tempSpell, x) == 116)
						curse = curse + (int)GetSpellBase(tempSpell, x);
					if (GetSpellAttrib(tempSpell, x) == 369)
						corruption = corruption + (int)GetSpellBase(tempSpell, x);
					if (tempSpell->SpellType == 0 && !strstr(tempSpell->Name, "Tash") && !strstr(tempSpell->Name, "Malo"))
						detrimental = detrimental + 1;
				}
				// log detrimental info
				SpawnTrackerIt->second.DiseaseCounters = disease;
				SpawnTrackerIt->second.PoisonCounters = poison;
				SpawnTrackerIt->second.CurseCounters = curse;
				SpawnTrackerIt->second.CorruptedCounters = corruption;
				SpawnTrackerIt->second.DetrimentalCounters = detrimental;
				if (!strcmp(spellCat, "Haste"))
				{
					SpawnTrackerIt->second.Haste = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Malo"))
				{
					SpawnTrackerIt->second.Malo = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Tash"))
				{
					SpawnTrackerIt->second.Tash = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Root"))
				{
					SpawnTrackerIt->second.Root = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Mez"))
				{
					SpawnTrackerIt->second.Mez = MQGetTickCount64() + buffDur;
				}
				if (strstr(spellCat, "DS"))
				{
					SpawnTrackerIt->second.DS = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Slow"))
				{
					SpawnTrackerIt->second.Slow = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Cripple"))
				{
					SpawnTrackerIt->second.Cripple = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Aego"))
				{
					SpawnTrackerIt->second.Aego = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Growth"))
				{
					SpawnTrackerIt->second.Growth = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Skin"))
				{
					SpawnTrackerIt->second.Skin = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Focus"))
				{
					SpawnTrackerIt->second.Focus = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Regen"))
				{
					SpawnTrackerIt->second.Regen = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Symbol"))
				{
					SpawnTrackerIt->second.Symbol = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Clarity"))
				{
					SpawnTrackerIt->second.Clarity = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Pred"))
				{
					SpawnTrackerIt->second.Pred = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Focus"))
				{
					SpawnTrackerIt->second.Focus = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "SV"))
				{
					SpawnTrackerIt->second.SV = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "SE"))
				{
					SpawnTrackerIt->second.SE = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Strength"))
				{
					SpawnTrackerIt->second.Shining = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Brells"))
				{
					SpawnTrackerIt->second.Brells = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Shining"))
				{
					SpawnTrackerIt->second.Shining = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Fero"))
				{
					SpawnTrackerIt->second.Fero = MQGetTickCount64() + buffDur;
				}
				if (!strcmp(spellCat, "Hot"))
				{
					SpawnTrackerIt->second.Hot = MQGetTickCount64() + buffDur;
				}
			}
		}
	}
	return;
	/*
	vector<PSPELL>		vSpellList;
	vector<ULONGLONG>	vSpellTimers;
	char				SpawnBuffList[MAX_STRING];
	int					ID;
	PSPAWNINFO			Spawn;

	ULONGLONG			RevDSed;
	ULONGLONG			Crippled;
	ULONGLONG			Charged;
	ULONGLONG			Concussed;
	ULONGLONG			MindFroze;
	ULONGLONG			Charmed;
	ULONGLONG			PoisonCounters;
	ULONGLONG			DiseaseCounters;
	ULONGLONG			CorruptedCounters;
	ULONGLONG			CurseCounters;
	ULONGLONG			DetrimentalCounters;
	*/
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
		for (int x = 0; x < GetSpellNumEffects(pSpell); x++)
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

void TargetSpawn(PSPAWNINFO pSpawn)
{
	if (MQGetTickCount64() > TargetTimer && pSpawn && pSpawn->Type != SPAWN_CORPSE)
	{
		TargetIT(pSpawn);
		TargetTimer = MQGetTickCount64() + 700LL;
	}
	return;
}

void Test()
{
	//to test
	if (vPossibleAdds.size() && !vAdds.size())
	{
		char test[MAX_STRING];
		sprintf_s(test, "${If[${Me.XTarget[1].ID} && ${Me.XTarget[1].Distance}<%d,1,0]}", AssistRange);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1 && vPossibleAdds[0])
		{
			TargetIT(vPossibleAdds[0]);
			strcpy_s(test, "${If[${Target.AggroHolder.ID},1,0]}");
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				vAdds.push_back(vPossibleAdds[0]);
				vPossibleAdds.clear();
			}
		}
		return;
	}
	//end test
}

void TimerCleanup()
{
	if (!vTimerTimestamp.size())
		return;
	for (unsigned int x = 0; x < vTimerTimestamp.size(); x++)
	{
		if ((MQGetTickCount64() > vTimerTimestamp[x] && vTimerTimestamp[x] != 0LL) ||
			(x >= vTimerSpawn.size()) ||
			(x <= vTimerSpawn.size() && !vTimerSpawn[x]) ||
			(x <= vTimerSpawn.size() && vTimerSpawn[x] && vTimerSpawn[x]->Type == SPAWN_CORPSE))
		{
			//WriteChatf("Erasing: %s - %s - %d",vTimerSpawn[x]->Name,vTimerSpell[x]->Name,vTimerTimestamp[x]);
			vTimerTimestamp.erase(vTimerTimestamp.begin() + x);
			if (x < vTimerSpell.size() && vTimerSpell[x])
				vTimerSpell.erase(vTimerSpell.begin() + x);
			if (x < vTimerSpawn.size() && vTimerSpawn[x])
				vTimerSpawn.erase(vTimerSpawn.begin() + x);
		}
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

bool XTMinHP(int rangeCheck)
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	// Get xtarget slots for processing
	int XTSlots = 0;
	ExtendedTargetList* etl = pChar->pXTargetMgr;
	if (etl)
		XTSlots = etl->XTargetSlots.Count;
	// end xtarget slots
	for (int i = 1; i <= XTSlots; i++)
	{
		sprintf_s(test, "${If[(${Me.XTarget[%d].Type.Equal[PC]}||${Me.XTarget[%d].Type.Equal[mercenary]}),${Me.XTarget[%d].ID},0]}", i, i, i);
		ParseMacroData(test, MAX_STRING);
		if (atol(test) > 1)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(test)))
			{
				if (unsigned long maxhp = pSpawn->HPMax)
					iCheck = pSpawn->HPCurrent * 100 / maxhp;
				else
					iCheck = 0;
				if (iCheck < iLowest)
				{
					if (pSpawn)
					{
						iLowest = iCheck;
						xMinHP = pSpawn;
					}

				}
			}
		}
	}
	return false;
}

bool XTMinPetHP(int rangeCheck)
{
	if (!InGameOK())
		return false;
	xMinPetHP = NULL;
	PCHARINFO pChar = GetCharInfo();
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	// Get xtarget slots for processing
	int XTSlots = 0;
	ExtendedTargetList* etl = pChar->pXTargetMgr;
	if (etl)
		XTSlots = etl->XTargetSlots.Count;
	// end xtarget slots
	for (int i = 1; i <= XTSlots; i++)
	{
		sprintf_s(test, "${If[${Me.XTarget[%d].Master.Type.Equal[PC]},${Me.XTarget[%d].ID},0]}", i, i);
		ParseMacroData(test, MAX_STRING);
		if (atol(test) > 1)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(test)))
			{
				if (unsigned long maxhp = pSpawn->HPMax)
					iCheck = pSpawn->HPCurrent * 100 / maxhp;
				else
					iCheck = 0;
				if (iCheck < iLowest)
				{
					if (pSpawn)
					{
						iLowest = iCheck;
						xMinPetHP = pSpawn;
					}

				}
			}
		}
	}
	return false;
}

bool XTMinMana(int rangeCheck)
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	long fDistance = 0;
	int iCheck;
	int iLowest = 101;
	PSPAWNINFO iMember = NULL;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	// Get xtarget slots for processing
	int XTSlots = 0;
	ExtendedTargetList* etl = pChar->pXTargetMgr;
	if (etl)
		XTSlots = etl->XTargetSlots.Count;
	// end xtarget slots
	for (int i = 1; i <= XTSlots; i++)
	{
		sprintf_s(test, "${If[(${Me.XTarget[%d].Type.Equal[PC]}||${Me.XTarget[%d].Type.Equal[mercenary]}) && ${Me.XTarget[%d].CurrentMana}>0,${Me.XTarget[%d].ID},0]}", i, i, i, i);
		ParseMacroData(test, MAX_STRING);
		if (atol(test) > 1)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(test)))
			{
				if (unsigned long maxhp = pSpawn->ManaMax)
					iCheck = pSpawn->ManaCurrent * 100 / maxhp;
				else
					iCheck = 0;
				if (iCheck < iLowest)
				{
					if (pSpawn)
					{
						iLowest = iCheck;
						xMinMana = pSpawn;
					}

				}
			}
		}
	}
	return false;
}

bool XTPriority(int rangeCheck)
{
	if (!InGameOK())
		return false;
	PCHARINFO pChar = GetCharInfo();
	long fDistance = 0;
	int iCheck;
	int iPriority = 0;
	PSPAWNINFO iMember = NULL;
	if (!rangeCheck)
		fDistance = 250;
	if (rangeCheck)
		fDistance = rangeCheck;
	char test[MAX_STRING];
	// Get xtarget slots for processing
	int XTSlots = 0;
	ExtendedTargetList* etl = pChar->pXTargetMgr;
	if (etl)
		XTSlots = etl->XTargetSlots.Count;
	// end xtarget slots
	for (int i = 1; i <= XTSlots; i++)
	{
		sprintf_s(test, "${If[(${Me.XTarget[%d].Type.Equal[NPC]}||${Me.XTarget[%d].Master.Type.Equal[NPC]}),${Me.XTarget[%d].ID},0]}", i, i, i);
		ParseMacroData(test, MAX_STRING);
		if (atol(test) > 1)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(test)))
			{
				SPAWNINFO tSpawn;
				memcpy(&tSpawn, pSpawn, sizeof(SPAWNINFO));
				iCheck = tSpawn.Level;
				if (strstr(tSpawn.Name, "#"))
					iCheck = iCheck + 15;
				if (tSpawn.MasterID)
					iCheck = iCheck - 25;
				if (strstr(MezImmuneList, tSpawn.DisplayedName))
					iCheck = iCheck + 10;
				if (strstr(SnareImmuneList, tSpawn.DisplayedName))
					iCheck = iCheck + 5;
				if (iCheck > iPriority)
				{
					if (pSpawn)
					{
						iPriority = iCheck;
						xPriority = pSpawn;
					}

				}
			}
		}
	}
	return false;
}

//Pulling
void CheckNavigation()
{
	if (!InGameOK())
		return;
	bCanIPull = false;
	if (Evaluate("${Navigation.MeshLoaded}"))
	{
		bCanIPull = true;
		if (AnnounceBehaviorChange)
			WriteChatf("Pulling enabled in this zone.");
	}
	else
	{
		if (AnnounceBehaviorChange)
			WriteChatf("Pulling disabled in this zone.");
	}
}

void CheckPullAggro()
{
	if (MQGetTickCount64() < PullTimer)
		return;
	PullTimer = MQGetTickCount64() + 500LL;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking pull aggro.", CurrentRoutine);
	if (pFindMob && pFindMob->Type != SPAWN_CORPSE && pFindMob->Type == SPAWN_NPC)
	{
		if (pLocalPlayer->CanSee((EQPlayer *)pFindMob) && GetDistance(GetCharInfo()->pSpawn, pFindMob) < PullSkillDistance)
		{
			DebugWrite("Mob: %s, Distance: %f", pFindMob ? pFindMob->Name : "NULL", GetDistance(GetCharInfo()->pSpawn, pFindMob));
			char test[MAX_STRING] = { 0 }, sSpell[MAX_STRING] = { 0 };
			sprintf_s(test, "${If[(${Me.CombatAbilityReady[%s]}||${Me.AltAbilityReady[%s]}||${Me.SpellReady[%s]}),1,0]}", PullSkill, PullSkill, PullSkill);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				DebugWrite("Using %s to pull %s", PullSkill, pFindMob ? pFindMob->Name : "NULL");
				//TargetIT(pFindMob);
				HideDoCommand(GetCharInfo()->pSpawn, "/squelch /nav stop", FromPlugin);
				if (GetAAIndexByName(PullSkill))
				{
					sprintf_s(sSpell, "/alt act ${Me.AltAbility[%s].ID}", PullSkill);
					ParseMacroData(sSpell, MAX_STRING);
					DoCommand(GetCharInfo()->pSpawn, sSpell);
				}
				else
					CastSpell(pFindMob, GetSpellByName(PullSkill), "/ag", 0);

				if (LastAnnouncedSpell != (DWORD)atol(PullSkill) && AnnounceEcho)
					WriteChatf("\ap%s \aw--> %s", PullSkill,pFindMob->Name);
				if (LastAnnouncedSpell != (DWORD)atol(PullSkill) && AnnounceEQBC)
				{
					EQBCSwap("\ap");
					sprintf_s(sSpell, "/bc %s%s [+x+]--> %s", EQBCColor, PullSkill, pFindMob->Name);
					HideDoCommand(GetCharInfo()->pSpawn, sSpell, FromPlugin);
				}
				LastAnnouncedSpell = (DWORD)atol(PullSkill);
				//HideDoCommand(GetCharInfo()->pSpawn, "/alt act ${Me.AltAbility[Throw Stone].ID}", FromPlugin);
			}
		}
	}
}

PSPAWNINFO FindMob()
{
	if (!InGameOK() || bPulling)
		return NULL;
	PSPAWNINFO pSpawn = (PSPAWNINFO)pSpawnList;

	int iDist = PullingDistance;
	char test[MAX_STRING];
	bool SkipOne = false;
	while (pSpawn)
	{
		SkipOne = false;
		if (GetDistance(GetCharInfo()->pSpawn, pSpawn) < iDist && !pSpawn->MasterID)
		{
			sprintf_s(test, "${Spawn[%s].Type.Equal[npc]} && !${Select[${Spawn[%s].Type},pet,aura,campfire,banner,mercenary,pc,merc]} && ${SpawnCount[%s noalert 1 npc]} && !${String[%s].Find[%s]} && !${String[%s].Find[%s]}", pSpawn->Name, pSpawn->Name, pSpawn->Name, IgnoreList, pSpawn->DisplayedName, IgnoreList, pSpawn->Name);
			if (Evaluate(test))
			{
				sprintf_s(test, "${Navigation.PathLength[locxyz %f %f %f]}", pSpawn->X, pSpawn->Y, pSpawn->Z);
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) < iDist)
				{
					for (unsigned int i = 0; i < vAdds.size(); i++)
					{
						if (vAdds[i] == pSpawn)
						{
							SkipOne = true;
							break;
						}
					}
					if (SkipOne)
					{
						pSpawn = pSpawn->pNext;
						continue;
					}
					iDist = (int)GetDistance(GetCharInfo()->pSpawn, pSpawn);
					pPullMob = pSpawn;
					pFindMob = pSpawn;
				}
			}
		}
		pSpawn = pSpawn->pNext;
	}
	return pPullMob;
}

void UpdatePullingNavigation()
{
	if (!InGameOK() || !bPulling || bReturning || MQGetTickCount64() < PullTimer)
		return;
	//WriteChatf("void UpdatePullingNavigation");
	CheckPullAggro();
	if (pFindMob && pFindMob->Type == SPAWN_NPC)
	{
		//WriteChatf("void UpdatePullingNavigation2");
		if (fPullX != pFindMob->X || fPullY != pFindMob->Y)
		{
			//WriteChatf("Mob Loc changed, updating void UpdatePullingNavigation3");
			char test[MAX_STRING] = { 0 };
			sprintf_s(test, "/nav locxyz %f %f %f", pFindMob->X, pFindMob->Y, pFindMob->Z);
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /nav stop", FromPlugin);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			PullTimer = MQGetTickCount64() + 1000LL;
			fPullX = pFindMob->X;
			fPullY = pFindMob->Y;
			fPullZ = pFindMob->Z;
			TargetIT(pFindMob);
			if (AnnounceBehaviorChange)
				WriteChatf("Pulling: %s", pFindMob->Name);
		}
	}
}

void UpdateReturnNavigation()
{
	if (!InGameOK() || bPulling || !bReturning || MQGetTickCount64() < PullTimer)
		return;
	char test[MAX_STRING] = { 0 };
	PSPAWNINFO pChar = GetCharInfo()->pSpawn;
	if (GetDistance3D(fCampX, fCampY, fCampZ, pChar->X, pChar->Y, pChar->Z) < 10)
	{
		bReturning = false;
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /nav stop", FromPlugin);
		pFindMob = NULL;
	}
	sprintf_s(test, "${Navigation.Active} && ${Int[${Navigation.PathLength[locxyz %f %f %f]}]}>20", fCampX, fCampY, fCampZ);
	if (!Evaluate(test))
	{
		sprintf_s(test, "/nav locxyz %f %f %f", fCampX, fCampY, fCampZ);
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Navigating back to camp...");
	}
}

void CheckReturn()
{
	if (!InGameOK() || !bPulling || bReturning)
		return;
	if (((PCPLAYERWND)pPlayerWnd))
		if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
			if (((PCPLAYERWND)pPlayerWnd)->CombatState == 0)
				InCombat = true;
	if (vAdds.size()||InCombat)
	{
		strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
		DebugWrite("%s: Checking return.", CurrentRoutine);
		bPulling = false;
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /nav stop", FromPlugin);
		bReturning = true;
		char test[MAX_STRING] = { 0 };
		sprintf_s(test, "/nav locxyz %f %f %f", fCampX, fCampY, fCampZ);
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Navigating back to camp...");
	}
}

void CheckPull()
{
	if (!InGameOK() || bPulling || bReturning)
		return;
	CheckReturn();
	//WriteChatf("void CheckPull");
	if (!vAdds.size())
	{
		pFindMob = NULL;
		if (PSPAWNINFO pSpawn = FindMob())
		{
			char test[MAX_STRING] = { 0 };
			//WriteChatf("voidCheckPull2 %s",pSpawn->Name);
			bReturning = false;
			bPulling = true;
			SpellIt = SpellIf.find("PullIf");
			if (SpellIt != SpellIf.end())
			{
				strcpy_s(test, SpellIt->second.c_str());
				if (Evaluate(test) == 0)
				{
					bPulling = false;
					return;
				}
			}
			//WriteChatf("voidCheckPull3 %s",pSpawn->Name);
			fCampY = GetCharInfo()->pSpawn->Y;
			fCampX = GetCharInfo()->pSpawn->X;
			fCampZ = GetCharInfo()->pSpawn->Z;
			fPullX = 0;
			fPullY = 0;
			fPullZ = 0;
		}
	}
}

//Routines
void CheckAAs()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!vAA.size() || MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > AssistAt || (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) < AAStopAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "", sNamed[MAX_STRING];
	char castLine[MAX_STRING], castTimerInfo[MAX_STRING];
	PSPELL pSpell;
	bool valid = true;
	bool named = false;
	strcpy_s(sNamed, "${If[!${Target.Surname.Length} && ${Target.Type.Equal[NPC]} && ${Target.Named},${Target.ID},0]}");
	ParseMacroData(sNamed, MAX_STRING);
	if (atol(sNamed) > 1)
		named = true;
	int capAmt = vAA.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (MQGetTickCount64() < SpellTimer)
			return;
		valid = true;
		sprintf_s(sTest, "${If[${Me.AltAbilityReady[%s]},1,0]}", vAA[i].c_str());
		if (GetCharInfo()->pSpawn->mActorClient.Class == 3)
			if (strstr(vAA[i].c_str(), "Inquisitor's Judg"))
				strcpy_s(sTest, "${If[${Me.AltAbilityReady[Inquisitor's Judgement]},1,0]}");
		ParseMacroData(sTest, MAX_STRING);
		if (atoi(sTest) == 1)
		{
			sprintf_s(sSpell, "AAIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			sprintf_s(sTest, "${Me.AltAbility[%s].MyReuseTime}", vAA[i].c_str());
			ParseMacroData(sTest, MAX_STRING);
			if (valid && (atol(sTest) <= (AACutoffTime * 60) || named && GetSpawnByID((DWORD)atol(sNamed))))
			{
				//WriteChatf("%s - %d",vAA[i].c_str(),atol(sTest));
				sprintf_s(sSpell, "${Me.AltAbility[%s].Spell.ID}", vAA[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				pSpell = GetSpellByID((DWORD)atol(sSpell));
				int AAOkay = 1;
				if (GetCharInfo()->pSpawn->mActorClient.Class == 16 && (!strcmp(vAA[i].c_str(), "Savage Spirit") || !strcmp(vAA[i].c_str(), "Untamed Rage")))
				{
					char sstest[MAX_STRING] = "";
					strcpy_s(sstest, "${If[${Me.ActiveDisc.ID},1,0]}");
					ParseMacroData(sstest, MAX_STRING);
					if (atoi(sstest) == 1)
						AAOkay = 0;
				}
				if (pSpell && pSpell->SpellType != 0 && AAOkay == 1)
				{
					if (SpellStacks(pSpell) || GetCharInfo()->pSpawn->mActorClient.Class == 16 && strcmp(vAA[i].c_str(), "Savage Spirit"))
					{

						sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", vAA[i].c_str());
						ParseMacroData(castTimerInfo, MAX_STRING);
						SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 600LL;
						sprintf_s(castLine, "/alt act ${Me.AltAbility[%s].ID}", vAA[i].c_str());
						ParseMacroData(castLine, MAX_STRING);
						DoCommand(GetCharInfo()->pSpawn, castLine);
						if (LastAnnouncedSpell != (DWORD)atol(sSpell) && AnnounceEcho)
							WriteChatf("\ap%s", vAA[i].c_str());
						if (LastAnnouncedSpell != (DWORD)atol(sSpell) && AnnounceEQBC)
						{
							EQBCSwap("\ap");
							sprintf_s(castLine, "/bc %s%s", EQBCColor, vAA[i].c_str());
							HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
						}
						LastAnnouncedSpell = (DWORD)atol(sSpell);
						return;
					}
				}
				if (pSpell && pSpell->SpellType == 0 && vAdds[0])
				{
					valid = ValidDet(pSpell, vAdds[0]);
					if (valid)
					{
						TargetIT(vAdds[0]);
						sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", vAA[i].c_str());
						ParseMacroData(castTimerInfo, MAX_STRING);
						SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 600LL;
						sprintf_s(castLine, "/alt act ${Me.AltAbility[%s].ID}", vAA[i].c_str());
						ParseMacroData(castLine, MAX_STRING);
						DoCommand(GetCharInfo()->pSpawn, castLine);
						if (LastAnnouncedSpell != (DWORD)atol(sSpell) && AnnounceEcho)
							WriteChatf("\ap%s \aw--> %s", vAA[i].c_str(), vAdds[0]->Name);
						if (LastAnnouncedSpell != (DWORD)atol(sSpell) && AnnounceEQBC)
						{
							EQBCSwap("\ap");
							sprintf_s(castLine, "/bc %s%s [+x+]--> %s", EQBCColor, vAA[i].c_str(), vAdds[0]->Name);
							HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
						}
						LastAnnouncedSpell = (DWORD)atol(sSpell);
						if (GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 0)
						{
							char test[MAX_STRING];
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", pSpell->Name);
							ParseMacroData(test, MAX_STRING);
							ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test);
							ULONGLONG recastTime = MQGetTickCount64() + test2;
							AddTimer(vAdds[0], pSpell, recastTime);
							//AddTimer(vAdds[0],pSpell,0);
						}
						return;
					}
				}
			}
		}
	}
}

void CheckAggro()
{
	{
		if (!InGameOK())
			return;
		if (!vAdds.size() && !vPossibleAdds.size())
			return;
		if (!vAdds[0])
			return;
		if (!vAggro.size() || MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
			return;
		strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
		DebugWrite("%s: Checking routine", CurrentRoutine);
		char sSpell[MAX_STRING] = "";
		int valid = 0, pctAggro = 0;
		strcpy_s(sSpell, "${If[${Group.MainTank.ID}==${Me.ID},1,0]}");
		ParseMacroData(sSpell, MAX_STRING);
		if (atoi(sSpell) == 0)
			return;
		bool SkipOne = false;
		for (unsigned int i = 0; i < vAggro.size(); i++)
		{
			if (!vAggro[i])
				continue;
			SkipOne = false;
			valid = 1;
			if (MQGetTickCount64() < SpellTimer)
				return;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				if (vAdds[0] && vTimerSpawn[x])
					if (vAggro[i] == vTimerSpell[x] && vAdds[0] == vTimerSpawn[x])
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			if (vAggro[i]->AERange == 0)
			{
				if (vAggro[i]->Range < AssistRange)
					valid = AddCountByDistance((int)vAggro[i]->Range);
				else
					valid = AddCountByDistance(AssistRange);
			}
			if (vAggro[i]->AERange > 0)
			{
				if (vAggro[i]->Range < AssistRange)
					valid = AddCountByDistance((int)vAggro[i]->AERange);
				else
					valid = AddCountByDistance(AssistRange);
			}
			if (pAggroInfo)
				pctAggro = pAggroInfo->aggroData[AD_Secondary].AggroPct;
			sprintf_s(sSpell, "AggroIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = 0;
			}
			if (valid > 0 && (valid > 1 && vAggro[i]->AERange > 0 || pctAggro > AggroAt && AggroAt != 0))
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID} && !${Me.Song[${Spell[%s].AutoCast}].ID},1,0]}", vAggroName[i].c_str(), vAggroName[i].c_str(), vAggroName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastSpell(vAdds[0], vAggro[i], "\a-r", 0);
					//AddTimer(vAdds[0],vAggro[i],0);
					break;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[Improved %s]},1,0]}", vAggroName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					if (AnnounceEcho && vAdds[0] && HateTimer < MQGetTickCount64())
					{
						WriteChatf("\a-rImproved %s \aw-> %s", vAggroName[i].c_str(), vAdds[0]->DisplayedName);
						HateTimer = MQGetTickCount64() + 2000LL;
					}
					if (AnnounceEQBC && vAdds[0] && HateTimer < MQGetTickCount64())
					{
						EQBCSwap("\a-r");
						char castLine[MAX_STRING];
						sprintf_s(castLine, "/bc %sImproved %s [+x+]--> %s", EQBCColor, vAggroName[i].c_str(), vAdds[0]->DisplayedName);
						HideDoCommand(GetCharInfo()->pSpawn, castLine, FromPlugin);
						HateTimer = MQGetTickCount64() + 2000LL;
					}
					sprintf_s(sSpell, "/casting \"Improved %s\" alt", vAggroName[i].c_str());
					HideDoCommand(GetCharInfo()->pSpawn, sSpell, FromPlugin);
					//CastSpell(vAdds[0],vAggro[i],"\a-r",0);
					//AddTimer(vAdds[0],vAggro[i],0);
					break;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]}  && !${Target.Buff[%s].ID} && !${Me.Song[${Spell[%s].AutoCast}].ID},1,0]}", vAggroName[i].c_str(), vAggro[i]->Name, vAggro[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastAA(vAdds[0], vAggro[i], "\a-r", vAggroName[i].c_str());
					//AddTimer(vAdds[0],vAggro[i],0);
					break;
				}
			}
		}
	}
}

void CheckAura()
{
	DWORD MaxAuras = 1, AAIndex = 0;
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || UseAura == 0 || !GetCharInfo()->pSpawn || !vAura.size() || MQGetTickCount64() < AuraTimer || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (GetCharInfo() && (AAIndex = GetAAIndexByName("Spirit Mastery") && PlayerHasAAAbility(AAIndex)) ||
		(AAIndex = GetAAIndexByName("Auroria Mastery") && PlayerHasAAAbility(AAIndex)))
		MaxAuras++;
	char aura[MAX_STRING]; // , petaura[MAX_STRING];
	char sSpell[MAX_STRING]; // if statement check
	int valid = 1;
	AuraTimer = MQGetTickCount64() + 8000LL;
	if (PAURAMGR pAura = (PAURAMGR)pAuraMgr)
	{
		if (pAura->NumAuras && pAura->NumAuras >= MaxAuras)
			return;
		for (unsigned int i = 0; i < vAura.size() && vAura[i] && pAura->NumAuras < MaxAuras; i++)
		{
			sprintf_s(aura, "${If[${Me.Aura[%s].Name}", vAura[i]->Name);
			/* Disable the pet aura for now
			if (GetCharInfo()->pSpawn->mActorClient.Class != 2)
			{
			if (GetCharInfo()->pSpawn->PetID > 0)
			{
			sprintf_s(petaura, " && ${Me.Pet.Buff[%s].ID}", vAura[i]->Name);
			strcat_s(aura, petaura);
			}
			}
			strcat_s(aura, ",1,0]}");
			*/
			sprintf_s(sSpell, "AuraIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = 0;
			}
			ParseMacroData(aura, MAX_STRING);
			if (atoi(aura) != 0 && valid>0)
				continue;
			if (SpellStacks(vAura[i]) && SpellStacks(vAuraEffect[i]))
			{
				if (!BardClass)
				{
					CastSpell(GetCharInfo()->pSpawn, vAura[i], "\a-m", 1);
					AuraTimer = MQGetTickCount64() + 8000LL;
					break;
				}
				else
				{
					sprintf_s(aura, "/squelch /bottwist once ${Me.Gem[%s]}", vAura[i]->Name);
					ParseMacroData(aura, MAX_STRING);
					HideDoCommand(GetCharInfo()->pSpawn, aura, FromPlugin);
					WriteChatf("\a-m%s \aw --> %s", vAura[i]->Name, GetCharInfo()->Name);
				}
			}
		}
	}
}

void CheckBard()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (vAdds.size() && strlen(CombatTwist) > 0 && !CombatTwisting)
	{
		char twist[MAX_STRING];
		sprintf_s(twist, "/squelch /bottwist %s", CombatTwist);
		ParseMacroData(twist, MAX_STRING);
		HideDoCommand(GetCharInfo()->pSpawn, twist, FromPlugin);
		CombatTwisting = true;
	}
	if (!vAdds.size() && strlen(RestTwist) > 0 && CombatTwisting)
	{
		char twist[MAX_STRING];
		sprintf_s(twist, "/squelch /bottwist %s", RestTwist);
		ParseMacroData(twist, MAX_STRING);
		HideDoCommand(GetCharInfo()->pSpawn, twist, FromPlugin);
		CombatTwisting = false;
	}
	if (vCrescendo.size())
	{
		char CresLine[MAX_STRING], twist[MAX_STRING];
		for (unsigned int i = 0; i < vCrescendo.size(); i++)
		{
			GroupMinMana((int)vCrescendo[i]->AERange);
			if (gMinMana)
			{
				if (PctMana(gMinMana) < ManaAt)
				{
					if (strstr(vCrescendo[i]->Name, "Crescendo"))
						sprintf_s(CresLine, "${If[!${Me.GemTimer[%s]} && !${Me.Buff[Crescendo Effect].ID},${Me.Gem[%s]},0]}", vCrescendo[i]->Name, vCrescendo[i]->Name);
					else
						sprintf_s(CresLine, "${If[!${Me.GemTimer[%s]},${Me.Gem[%s]},0]}", vCrescendo[i]->Name, vCrescendo[i]->Name);
					ParseMacroData(CresLine, MAX_STRING);
					if (atoi(CresLine)>0)
					{
						sprintf_s(twist, "/squelch /bottwist once %s", CresLine);
						ParseMacroData(twist, MAX_STRING);
						HideDoCommand(GetCharInfo()->pSpawn, twist, FromPlugin);
						WriteChatf("\a-t%s \aw --> %s", vCrescendo[i]->Name, gMinMana->Name);
						SpellTimer = MQGetTickCount64() + 3400LL;
					}
				}
			}
		}

	}
}

void CheckBuffs()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < BuffTimer)
		return;
	if (GetCharInfo() && !GetCharInfo()->pGroupInfo)
	{
		SpawnTrack(GetCharInfo()->pSpawn);
		return;
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		if (GetCharInfo() && GetCharInfo()->pGroupInfo && GetCharInfo()->pGroupInfo->pMember[i] && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
		{
			if (GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || GetCharInfo()->pGroupInfo->pMember[i]->Mercenary)
			{
				SpawnTrackerIt = SpawnTracker.find((int)GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->SpawnID);
				if (SpawnTrackerIt == SpawnTracker.end())
				{
					SpawnTrack(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
				}
				else
				{
					if (SpawnTrackerIt->second.NeedsCheck)
					{
						BuffTimer = MQGetTickCount64() + 1000LL;
						TargetIT(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
						SpawnTrackUpdate(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn, 3); //this should update all the known buffs for that target and then get out for the next pass
						DebugWrite("Adding %s to Buff Queue", pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
						break;
					}
					// do the buffq add here
					for (unsigned int x = 0; vBuffType.size(); x++)
					{
						// reminder: change to vBuffType[i] = "Haste"
						if (!strcmp(vBuffType[i].c_str(), "Haste") && strstr(HasteClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Haste < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Haste spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Aego") && strstr(AegoClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Aego < MQGetTickCount64() && SpawnTrackerIt->second.Skin < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Aego spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Skin") && strstr(SkinClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Skin < MQGetTickCount64() && SpawnTrackerIt->second.Aego < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Skin spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Focus") && strstr(FocusClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Focus < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Focus spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Regen") && strstr(RegenClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Regen < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Regen spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Clarity") && strstr(ClarityClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Clarity < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Clarity spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Fero") && strstr(FeroClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Fero < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Fero spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Pred") && strstr(PredClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Pred < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Pred spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "Symbol") && strstr(SymbolClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.Symbol < MQGetTickCount64() && SpawnTrackerIt->second.Aego < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding Symbol spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
						if (!strcmp(vBuffType[i].c_str(), "SE") && strstr(SEClasses, pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class)))
						{
							if (SpawnTrackerIt->second.SE < MQGetTickCount64())
							{
								vBuffQ.push_back(vBuff[i]);
								vBuffQSpawn.push_back(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
								DebugWrite("Adding SE spell %s to buff queue for %s", vBuffName[i].c_str(), pEverQuest->GetClassThreeLetterCode(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->mActorClient.Class));
							}
						}
					}

				}
			}
		}
	}

	if (gTargetbuffs && pTarget)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
		{
			SpawnTrack(pSpawn);
			SpawnTrackUpdate(pSpawn, 3);
			// SpawnTrackReport(pSpawn);
		}
	}
}

void CheckBuffQ()
{
	if (!InGameOK())
		return;
	if (gGameState != GAMESTATE_INGAME || MQGetTickCount64() < SpellTimer || MQGetTickCount64() < SelfBuffTimer || !vBuffQ.size() || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 && !BardClass)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "", spellName[MAX_STRING];
	for (unsigned int i = 0; i<vBuffQ.size(); i++)
	{
		if (!vBuffQ[i])
			continue;
		int x = (int)(vBuffQ[i]->Range>vBuffQ[i]->AERange ? vBuffQ[i]->Range : vBuffQ[i]->AERange);
		if (MQGetTickCount64() < SpellTimer || !vBuffQSpawn[i] || GetDistance(GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Y, vBuffQSpawn[i]->X, vBuffQSpawn[i]->Y) > x)
			return;
		sprintf_s(sSpell, "${If[!${Me.AltAbility[%s]} && ${Me.CurrentMana}>${Spell[%s].Mana} && ${Me.Book[%s]} && !${Me.Casting.ID},1,0]}", vBuffQ[i]->Name, vBuffQ[i]->Name, vBuffQ[i]->Name);
		ParseMacroData(sSpell, MAX_STRING);
		if (atoi(sSpell) == 1)
		{
			DebugWrite("Buffing %d with Spell %s", vBuffQSpawn[i] ? vBuffQSpawn[i]->SpawnID : -1, vBuffQ[i] ? vBuffQ[i]->Name : "NULL");
			TargetIT(vBuffQSpawn[i]);
			CastSpell(vBuffQSpawn[i], vBuffQ[i], "\aw", 1);
			SelfBuffTimer = MQGetTickCount64() + (ULONGLONG)((FLOAT)vBuffQ[i]->RecastTime / 1000.0) + (ULONGLONG)((FLOAT)vBuffQ[i]->RecoveryTime / 1000.0) + 1000LL;
			vBuffQ.erase(vBuffQ.begin() + i);
			vBuffQSpawn.erase(vBuffQSpawn.begin() + i);
			return;
		}
		else
		{
			sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Me.Casting.ID},1,0]}", spellName);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				DebugWrite("Buffing %d with AA %s", vBuffQSpawn[i] ? vBuffQSpawn[i]->SpawnID : -1, vBuffQ[i] ? vBuffQ[i]->Name : "NULL");
				TargetIT(vBuffQSpawn[i]);
				CastSpell(vBuffQSpawn[i], vBuffQ[i], "\aw", 1);
				SelfBuffTimer = MQGetTickCount64() + (ULONGLONG)((FLOAT)vBuffQ[i]->RecastTime / 1000.0) + (ULONGLONG)((FLOAT)vBuffQ[i]->RecoveryTime / 1000.0) + 1000LL;
				vBuffQ.erase(vBuffQ.begin() + i);
				vBuffQSpawn.erase(vBuffQSpawn.begin() + i);
				return;
			}
		}
	}

}

void CheckCall()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || RezRadius == 0 || !GetCharInfo()->pSpawn || !vCall.size() || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char rezCheck[MAX_STRING], rezName[MAX_STRING], sSpell[MAX_STRING];
	bool valid;
	for (unsigned int i = 0; i < 6; i++)
	{
		if (MQGetTickCount64() < vCallTimeStamp[i])
			continue;
		sprintf_s(rezCheck, "${If[!${SpawnCount[${Group.Member[%d]} pc radius 200]} && ${Spawn[${Group.Member[%d]} corpse radius %d].ID} && !${Group.Member[%d].Linkdead} && !${Spawn[${Group.Member[%d]} corpse radius %d].Name.Find[_pet]},1,0]}", i, i, RezRadius, i, i, RezRadius);
		ParseMacroData(rezCheck, MAX_STRING);
		if (atoi(rezCheck) == 1)
		{
			for (unsigned int x = 0; x < vCall.size(); x++)
			{
				if (!vCall[x])
					continue;
				sprintf_s(sSpell, "CallIf%d", x);
				valid = true;
				SpellIt = SpellIf.find(sSpell);
				if (SpellIt != SpellIf.end())
				{
					sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 0)
						valid = false;
				}
				sprintf_s(rezCheck, "${If[${Me.AltAbilityReady[%s]},1,0]}", vCallName[x].c_str());
				ParseMacroData(rezCheck, MAX_STRING);
				if (valid && atoi(rezCheck) == 1)
				{
					sprintf_s(rezName, "${Spawn[${Group.Member[%d]} corpse radius %d].ID}", i, RezRadius);
					ParseMacroData(rezName, MAX_STRING);
					if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(rezName)))
					{
						sprintf_s(rezName, "/squelch /tar ${Group.Member[%d]} corpse", i);
						ParseMacroData(rezName, MAX_STRING);
						//TargetIT(pSpawn);
						HideDoCommand(GetCharInfo()->pSpawn, rezName, FromPlugin);
						if (pSpawn && pSpawn->Type == SPAWN_CORPSE)
							CastAA(pSpawn, vCall[x], "\ao", vCallName[x].c_str());
						HideDoCommand(GetCharInfo()->pSpawn, "/corpse", FromPlugin);
						CallTimer = MQGetTickCount64() + 6000LL;
						if (pSpawn && pSpawn->Type == SPAWN_CORPSE)
							vCallTimeStamp[i] = MQGetTickCount64() + 60000LL;
						return;
					}
				}
			}
		}
	}
}

void CheckCampfire()
{
	if (MQGetTickCount64() < CampfireTimer)
		return;
	if (CampfireStep == 5)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate FellowshipWnd close", FromPlugin);
		CampfireTimer = MQGetTickCount64() + 100LL;
		CampfireStep = 0;
		return;
	}
	if (CampfireStep == 4)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify FP_CampPage FP_CreateCampsite leftmouseup", FromPlugin);
		CampfireTimer = MQGetTickCount64() + 500LL;
		WriteChatf("Campfire creation attempt completed.");
		CampfireStep = 5;
		return;
	}
	if (CampfireStep == 3)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify FP_CampPage FP_CampsiteKitList listselect 1", FromPlugin);
		CampfireTimer = MQGetTickCount64() + 600LL;
		CampfireStep = 4;
		return;
	}
	if (CampfireStep == 2)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify FellowshipWnd FP_Subwindows tabselect 2", FromPlugin);
		HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify FP_CampPage FP_RefreshList leftmouseup", FromPlugin);
		CampfireTimer = MQGetTickCount64() + 600LL;
		CampfireStep = 3;
		return;
	}
	if (CampfireStep == 1)
	{
		char test[MAX_STRING];
		strcpy_s(test, "${If[${Window[FellowshipWnd]},1,0]}");
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
			CampfireStep = 2;
		if (atoi(test) == 0)
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate FellowshipWnd open", FromPlugin);
		HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify FellowshipWnd FP_Subwindows tabselect 2", FromPlugin);
		CampfireTimer = MQGetTickCount64() + 300LL;
		return;
	}
}

void CheckCharm()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vCharm.size())
		return;
	if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if (BardClass)
		return;
	if (CharmMob)
	{
		if (CharmMob->Type == SPAWN_CORPSE || GetDistance(GetCharInfo()->pSpawn, CharmMob) > vCharm[0]->Range && GetCharInfo()->pSpawn->PetID == 0xFFFFFFFF)
			CharmMob = NULL;
	}
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	for (unsigned int i = 0; i < vCharm.size(); i++)
	{
		if (!vCharm[i])
			continue;
		valid = true;
		if (MQGetTickCount64() < SpellTimer)
			return;
		if (CharmMob)
		{
			if (GetDistance(GetCharInfo()->pSpawn, CharmMob) < vCharm[i]->Range)
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Me.Pet.ID},1,0]}", vCharm[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && CharmMob)
				{
					TargetIT(CharmMob);
					CastSpell(CharmMob, vCharm[i], "\am", 0);
				}
			}
		}
		if (!CharmMob || CharmMob->Type == SPAWN_CORPSE)
		{
			for (unsigned int z = 0; z < vAdds.size(); z++)
			{
				if (strstr(CharmImmuneList, vAdds[z]->DisplayedName) && ImmunityCheck || vAdds[z]->MasterID>0)
					continue;
				valid = false;
				if (GetSpellMaxX(vCharm[i], 0) >= vAdds[z]->Level && GetDistance(GetCharInfo()->pSpawn, CharmMob) < vCharm[i]->Range)
					valid = ValidDet(vCharm[i], vAdds[z]);
				sprintf_s(sSpell, "CharmIf%d", i);
				SpellIt = SpellIf.find(sSpell);
				if (SpellIt != SpellIf.end())
				{
					sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 0)
						valid = false;
				}
				if (valid)
				{
					CharmMob = vAdds[z];
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Me.Pet.ID},1,0]}", vCharm[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && CharmMob)
					{
						TargetIT(CharmMob);
						CastSpell(CharmMob, vCharm[i], "\am", 0);
					}
				}
			}
		}
	}
}

void CheckClickies()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || UseClickyBuffs == 0 || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000 && !BardClass))
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	PCHARINFO2 pChar2 = GetCharInfo2();
	int ben = 0, det = 0;

	if (!vClicky.size() || MQGetTickCount64() < ClickyTimer)
		return;
	char CastLine[MAX_STRING];
	for (unsigned int i = 0; i < vClicky.size() && i < vClickySpell.size(); i++)
	{
		if (MQGetTickCount64() < SpellTimer || MQGetTickCount64() < ClickyTimer)
			return;
		if (!vClickySpell[i])
			continue;
		char sSpell[MAX_STRING];
		bool clickyOK = true;
		sprintf_s(sSpell, "ClickyBuffIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				sprintf_s(sSpell, "${If[${Me.FreeBuffSlots}>0 && ${Spell[%s].Stacks} && !${Bot.HaveBuff[%s].ID} && ${Cast.Ready[%s]} && (!${Me.Casting.ID}||${Me.Class.ShortName.Equal[BRD]}) && (!${Bot.Hasted}||${String[%s].NotEqual[Twitching Speed]}) && (${Me.Subscription.Equal[GOLD]}||!${Bool[%d]}),1,0]}", vClickySpell[i]->Name, vClickySpell[i]->Name, vClicky[i].c_str(), vClickySpell[i]->Name, vClickyPrestige[i]);
				ParseMacroData(sSpell, MAX_STRING);
				if ((atoi(sSpell) == 1))
				{
					WriteChatf("\awClicking --> \a-o%s", vClicky[i].c_str());
					sprintf_s(CastLine, "/multiline ; /stopsong ; /casting \"%s\" item", vClicky[i].c_str());
					DoCommand(GetCharInfo()->pSpawn, CastLine);
					SpellTimer = MQGetTickCount64() + 1000LL;
					ClickyTimer = MQGetTickCount64() + 5000LL;
					break;
				}
			}
		}
		else
		{
			sprintf_s(sSpell, "${If[${Me.FreeBuffSlots}>0 && ${Spell[%s].Stacks} && !${Bot.HaveBuff[%s].ID} && ${Cast.Ready[%s]} && (!${Me.Casting.ID}||${Me.Class.ShortName.Equal[BRD]}) && (!${Bot.Hasted}||${String[%s].NotEqual[Twitching Speed]}) && (${Me.Subscription.Equal[GOLD]}||!${Bool[%d]}),1,0]}", vClickySpell[i]->Name, vClickySpell[i]->Name, vClicky[i].c_str(), vClickySpell[i]->Name, vClickyPrestige[i]);
			ParseMacroData(sSpell, MAX_STRING);
			if ((atoi(sSpell) == 1))
			{
				WriteChatf("\awClicking --> \a-o%s", vClicky[i].c_str());
				sprintf_s(CastLine, "/multiline ; /stopsong ; /casting \"%s\" item", vClicky[i].c_str());
				DoCommand(GetCharInfo()->pSpawn, CastLine);
				SpellTimer = MQGetTickCount64() + 1000LL;
				ClickyTimer = MQGetTickCount64() + 5000LL;
				break;
			}
		}
	}
}

void CheckClickyBuffs()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || vAdds.size() || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000))
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING], clicky[MAX_STRING];
	for (unsigned int i = 0; i < MAX_CLICKIES; i++)
	{
		if (MQGetTickCount64() < SpellTimer)
			break;
		sprintf_s(sSpell, "Clicky%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(${Me.FreeBuffSlots}>0 && ${Cast.Ready[%s]}),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				sprintf_s(clicky, "%s", SpellIt->second.c_str());
				sprintf_s(sSpell, "ClickyIf%d", i);
				SpellIt = SpellIf.find(sSpell);
				if (SpellIt != SpellIf.end())
				{
					sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1)
					{
						sprintf_s(sSpell, "/casting \"%s\" item", clicky);
						SpellTimer = MQGetTickCount64() + 1000LL;
						WriteChatf("\awClicking --> \a-o%s", clicky);
						HideDoCommand(GetCharInfo()->pSpawn, sSpell, FromPlugin);
					}
				}
				else
				{
					sprintf_s(sSpell, "/casting \"%s\" item", clicky);
					SpellTimer = MQGetTickCount64() + 1000LL;
					WriteChatf("\awClicking --> \a-o%s", clicky);
					HideDoCommand(GetCharInfo()->pSpawn, sSpell, FromPlugin);
				}
			}
		}
	}
}

void CheckItems()
{
	if (!InGameOK())
		return;
	if (!vAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < SpellTimer || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000))
		return;
	if (UseClickyNukes == 0 || MQGetTickCount64() < ClickyTimer)
		return;
	if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > AssistAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING];
	for (unsigned int i = 0; i < vItems.size(); i++)
	{
		if (MQGetTickCount64() < SpellTimer || i >= vItemsSpell.size())
			return;
		char sSpell[MAX_STRING] = "";
		bool valid = true;
		sprintf_s(sSpell, "ClickyNukeIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		sprintf_s(test, "${If[(!${FindItem[%s].Prestige}||${Me.Subscription.Equal[GOLD]}) && ${FindItem[%s].Charges}!=0 && ${Cast.Ready[%s]} && (${Spell[%s].SpellType.Equal[Beneficial]} && ${Spell[%s].Stacks}||${Spell[%s].SpellType.Equal[Detrimental]} && ${Target.Type.Equal[NPC]} && ${Target.Distance}<${Spell[%s].Range}),1,0]}", vItems[i].c_str(), vItems[i].c_str(), vItems[i].c_str(), vItemsSpell[i]->Name, vItemsSpell[i]->Name, vItemsSpell[i]->Name, vItemsSpell[i]->Name);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1 && valid)
		{
			if (BardClass)
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stopsong", FromPlugin);
				TwistTimer = MQGetTickCount64() + 1500LL;
				SpellTimer = MQGetTickCount64() + 2000LL;
			}
			sprintf_s(test, "/casting \"%s\" item", vItems[i].c_str());
			WriteChatf("\awClicking --> \a-o%s", vItems[i].c_str());
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			SpellTimer = MQGetTickCount64() + 1000LL;
		}
	}
	if (GetCharInfo()->pSpawn->mActorClient.Class == 13)
	{
		char szTempItem[MAX_STRING] = { 0 };
		CItemLocation cFindItem;
		for (unsigned int i = 0; szSummonedItems[i]; i++)
		{
			if (MQGetTickCount64() < SpellTimer)
				return;
			sprintf_s(szTempItem, "%s", szSummonedItems[i]);
			if (ItemFind(&cFindItem, szTempItem))
			{
				PCONTENTS find = cFindItem.pBagSlot;
				if (!find || ItemTimer(find))
					continue;
			}
			if (strstr(szTempItem, "Ether-Fused Shard"))
				sprintf_s(test, "${If[${FindItem[%s].Charges} && ${Cast.Ready[%s]} && ${Target.Type.Equal[NPC]} && ${Target.Distance}<%d,1,0]}", szTempItem, szSummonedItems[i], AssistAt);
			else
				sprintf_s(test, "${If[${Cast.Ready[%s]} && ${Target.Type.Equal[NPC]} && ${Target.Distance}<%d,1,0]}", szSummonedItems[i], AssistAt);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				sprintf_s(test, "/multiline ; /stopsong ; /casting \"%s\" item", szSummonedItems[i]);
				WriteChatf("\awClicking --> \a-o%s", szSummonedItems[i]);
				HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
				SpellTimer = MQGetTickCount64() + 1000LL;
			}
		}
	}
}

void CheckDebuffs()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!vDebuff.size() || MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > DebuffAt || (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) < DebuffStopAt)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", bSpell[MAX_STRING] = {0};
	bool valid = false;
	int adds = vAdds.size();
	if (DebuffAdds == 0 && adds > 0)
		adds = 1;
	bool SkipOne = false;
	for (int z = 0; z < adds; z++)
	{
		DebugWrite("%s: Checking add #%d.", CurrentRoutine, z);
		if (!vAdds[z])
			continue;
		for (unsigned int i = 0; i < vDebuff.size(); i++)
		{
			if (!vDebuff[i])
				continue;
			SkipOne = false;
			if (MQGetTickCount64() < SpellTimer)
				return;
			DebugWrite("%s: Checking debuff #%d.", CurrentRoutine, i);
			for (unsigned int x = 0; x < vTimerSpawn.size() && x < vTimerTimestamp.size(); x++)
			{
				if (!vTimerSpawn[x])
					continue;
				DebugWrite("%s: Checking spawn #%d.", CurrentRoutine, x);
				if (vAdds[z]->Type == SPAWN_CORPSE)
				{
					SkipOne = true;
					break;
				}
				if (vAdds[z] && vTimerSpawn[x] && vTimerSpell[x])
					if (vDebuff[i] && !strcmp(vDebuff[i]->Name, vTimerSpell[x]->Name) && vAdds[z] == vTimerSpawn[x] && (vTimerTimestamp[x] == 0 || MQGetTickCount64() < vTimerTimestamp[x] || GetDistance(vAdds[z]->X, vAdds[z]->Y) > AssistRange))
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			DebugWrite("%s: Checking valid #%d (#%d).", CurrentRoutine, i, z);
			valid = ValidDet(vDebuff[i], vAdds[z]);
			sprintf_s(sSpell, "DebuffIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				DebugWrite("%s: Parsing '%s'", CurrentRoutine, sSpell);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid && (!vDebuff[i]->AERange || adds > DebuffAEAmount || strstr(vDebuff[i]->Name, "Argent")) && vAdds[z])
			{

				if (!BardClass)
				{
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vDebuffName[i].c_str(), vDebuffName[i].c_str());
					DebugWrite("%s: Parsing '%s'", CurrentRoutine, sSpell);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && vAdds[z])
					{
						TargetIT(vAdds[z]);
						char test[MAX_STRING] = {0};
						if (GetSpellAttribX(vDebuff[i], 0) != 374)
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vDebuffName[i].c_str());
						else if (PSPELL debuff = GetSpellByID(GetSpellBase2X(vDebuff[i], 0)))
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", debuff->Name);
						DebugWrite("%s: Parsing '%s'", CurrentRoutine, test);
						ParseMacroData(test, MAX_STRING);
						ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vDebuff[i]);
						ULONGLONG recastTime = MQGetTickCount64() + test2;
						AddTimer(vAdds[z], vDebuff[i], recastTime);
						CastSpell(vAdds[z], vDebuff[i], "\a-y", 0);
						return;
					}
				}
				if (BardClass) {
					sprintf_s(sSpell, "${If[!${Me.GemTimer[%s]} && !${Bot.Slowed},1,0]}", vDebuffName[i].c_str());
					DebugWrite("%s: Parsing '%s'", CurrentRoutine, sSpell);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && vAdds[z])
					{
						if (vDebuff[i]->TargetType == 5)
							TargetIT(vAdds[z]);
						sprintf_s(bSpell, "/squelch /bottwist once ${Me.Gem[%s]}", vDebuffName[i].c_str());
						DebugWrite("%s: Parsing '%s'", CurrentRoutine, bSpell);
						ParseMacroData(bSpell, MAX_STRING);
						HideDoCommand(GetCharInfo()->pSpawn, bSpell, FromPlugin);
						WriteChatf("\a-y%s \aw --> %s", vDebuff[i]->Name, vAdds[z]->Name);
						char test[MAX_STRING] = {0};
						sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vDebuffName[i].c_str());
						DebugWrite("%s: Parsing '%s'", CurrentRoutine, test);
						ParseMacroData(test, MAX_STRING);
						ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vDebuff[i]);
						ULONGLONG recastTime = MQGetTickCount64() + test2;
						AddTimer(vAdds[z], vDebuff[i], recastTime);
						//AddTimer(vAdds[z],vDebuff[i],MQGetTickCount64()+90000LL);
						SpellTimer = MQGetTickCount64() + 8000LL;
						return;
					}
				}
				if (!strstr(vDebuff[i]->Name, "Eradicate")) //this was checking spells instead of AAs
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vDebuffName[i].c_str(), vDebuffName[i].c_str());
				else
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && ${Target.Beneficial.ID} && ${Target.ID}==%d,1,0]}", vDebuffName[i].c_str(), vAdds[z]->SpawnID);
				DebugWrite("%s: Parsing '%s'", CurrentRoutine, sSpell);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					DebugWrite("%s: Target/Cast/timer add #%d (%s) for debuff #%d (%s)", CurrentRoutine, z, vAdds[z]->Name, i, vDebuffName[i].c_str());
					TargetIT(vAdds[z]);
					CastAA(vAdds[z], vDebuff[i], "\a-y", vDebuffName[i].c_str());
					AddTimer(vAdds[z], vDebuff[i], (ULONGLONG)CalcDuration(vDebuff[i]));
					return;
				}
			}
		}
	}
}

void CheckDiscs()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < DiscTimer || !UseDiscsOffensive && !UseDiscsDefensive || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", discCheck[MAX_STRING] = {0};

	int capAmt = vDisc.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vDisc[i])
			continue;
		bool valid = true;
		sprintf_s(sSpell, "DiscIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		sprintf_s(discCheck, "${If[(!${Me.CombatAbilityReady[%s]}||${Bot.SpellDuration[%s]} && ${Me.ActiveDisc.ID} && ${Target.Type.Equal[NPC]}),1,0]}", vDisc[i]->Name, vDisc[i]->Name);
		ParseMacroData(discCheck, MAX_STRING);
		if (atoi(discCheck) == 1 || vDisc[i]->AERange>0 || (int)vDisc[i]->EnduranceCost > GetCharInfo()->pSpawn->EnduranceCurrent)
			valid = false;
		if (GetCharInfo()->pSpawn->mActorClient.Class == 16 && (vDisc[i]->Skill == 74 || strstr(vDisc[i]->Name, "Frenz")))
			if (PctHP(GetCharInfo()->pSpawn) > 89)
				valid = false;
		if (UseDiscsOffensive && valid && (strstr(vDiscType[i].c_str(), "Offense") || strstr(vDiscType[i].c_str(), "Combat Innates") || strstr(vDiscType[i].c_str(), "Throwing")))
		{
			if (pAggroInfo)
			{
				if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
				{
					if (pSpawn->Type == SPAWN_NPC && vAdds[0])
					{
						PSPAWNINFO pChar = GetCharInfo()->pSpawn;
						FLOAT MobDist = GetDistance3D(vAdds[0]->X, vAdds[0]->Y, vAdds[0]->Z, pChar->X, pChar->Y, pChar->Z);
						if (MobDist < vDisc[i]->Range && MobDist < MeleeDistance)
						{
							if (strstr(vDisc[i]->Name, "Dichotomic"))
							{
								strcpy_s(sSpell, vDisc[i]->Name);
								int discl = strlen(sSpell);
								sSpell[discl - 2] = '\0';
								sprintf_s(discCheck, "/disc %s", sSpell);
								DoCommand(NULL, discCheck);
								WriteChatf("\ay%s \aw-> %s", sSpell, vAdds[0]->DisplayedName);
							}
							else
								CastSpell(vAdds[0], vDisc[i], "\ay", 0);
							DiscTimer = MQGetTickCount64() + 1500LL;
							return;
						}
					}
				}
			}
		}
		if (UseDiscsDefensive && valid && strstr(vDiscType[i].c_str(), "Defense") && (MQGetTickCount64() - ImHitTimeStamp) < 5000)
		{
			if (pAggroInfo)
			{
				if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
				{
					if (pSpawn->Type == SPAWN_NPC && vAdds[0])
					{
						PSPAWNINFO pChar = GetCharInfo()->pSpawn;
						FLOAT MobDist = GetDistance3D(vAdds[0]->X, vAdds[0]->Y, vAdds[0]->Z, pChar->X, pChar->Y, pChar->Z);
						if (MobDist < vDisc[i]->Range && MobDist < MeleeDistance)
						{
							if (strstr(vDisc[i]->Name, "Dichotomic"))
							{
								strcpy_s(sSpell, vDisc[i]->Name);
								int discl = strlen(sSpell);
								sSpell[discl - 2] = '\0';
								sprintf_s(discCheck, "/disc %s", sSpell);
								DoCommand(NULL, discCheck);
								WriteChatf("\ay%s \aw-> %s", sSpell, vAdds[0]->DisplayedName);
							}
							else
								CastSpell(vAdds[0], vDisc[i], "\ay", 0);
							DiscTimer = MQGetTickCount64() + 1500LL;
							return;
						}
					}
				}
			}
		}
	}
	DiscTimer = MQGetTickCount64() + 1000LL;
}

void CheckDoor()
{
	if (!FindDoor || MQGetTickCount64() < DoorTimer)
		return;
	char test[MAX_STRING];
	strcpy_s(test, "${If[${Switch.ID} && ${Switch.Distance}<250 && ${String[${MoveTo}].Equal[OFF]},1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face door nolook", FromPlugin);
		strcpy_s(test, "/moveto loc ${Switch.Y} ${Switch.X} ${Switch.Z}");
		ParseMacroData(test, MAX_STRING);
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
	}
	strcpy_s(test, "${If[${Switch.ID} && ${Switch.Distance3D}<18,1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/moveto off", FromPlugin);
		HideDoCommand(GetCharInfo()->pSpawn, "/click left door", FromPlugin);
		FindDoor = 0;
	}
	strcpy_s(test, "${If[${Switch.ID},1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 0)
		FindDoor = 0;
}

void CheckDots()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!vDot.size() || MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > DotAt || (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) < DotStopAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
		return;
	PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
	if (pSpawn->ManaCurrent)
		if (PctMana(pSpawn) < MinManaToNuke)
			return;
	CheckAlways();
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	bool SkipOne = false;
	for (unsigned int i = 0; i < vDot.size(); i++)
	{
		if (!vDot[i])
			continue;
		SkipOne = false;
		if (MQGetTickCount64() < SpellTimer)
			return;
		for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
		{
			if (vAdds[0] && vTimerSpawn[x])
				if (vDot[i] == vTimerSpell[x] && vAdds[0] == vTimerSpawn[x])
				{
					SkipOne = true;
					break;
				}
		}
		if (SkipOne)
			continue;
		valid = ValidDet(vDot[i], vAdds[0]);
		sprintf_s(sSpell, "DotIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (valid && vAdds[0])
		{
			sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vDotName[i].c_str(), vDotName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1 && vAdds[0])
			{
				TargetIT(vAdds[0]);
				char test[MAX_STRING] = {0};
				sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vDot[i]->Name);
				ParseMacroData(test, MAX_STRING);
				ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vDot[i]);
				ULONGLONG recastTime = MQGetTickCount64() + test2;
				AddTimer(pSpawn, vDot[i], recastTime);
				CastSpell(vAdds[0], vDot[i], "\ay", 0);
				//AddTimer(vAdds[0],vDot[i],0);
			}
			sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vDotName[i].c_str(), vDot[i]->Name);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1 && vAdds[0])
			{
				TargetIT(vAdds[0]);
				char test[MAX_STRING];
				sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vDot[i]->Name);
				ParseMacroData(test, MAX_STRING);
				ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vDot[i]);
				ULONGLONG recastTime = MQGetTickCount64() + test2;
				AddTimer(pSpawn, vDot[i], recastTime);
				CastAA(vAdds[0], vDot[i], "\ay", vDotName[i].c_str());
				//AddTimer(vAdds[0],vDot[i],0);
			}
		}
	}
}

void CheckEndurance()
{
	if (!InGameOK())
		return;
	PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
	if (EndMedding && (PctEnd(pSpawn) > 99 || PctEnd(pSpawn) > EndMedStopAt && !EndMedToFull))
	{
		if (pSpawn->StandState == STANDSTATE_SIT)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Endurance medding done, standing.");
		}
		DoingFeignDeath = 0;
		EndMedding = false;

		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: End at %d percent.  Finished medding.", PctEnd(GetCharInfo()->pSpawn));
	}
	if (MQGetTickCount64() < MedTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (vAdds.size() || GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT)
		return;
	if (EndRegenSpell)
	{
		if (PctEnd(pSpawn) < GetSpellBaseX(EndRegenSpell, 2) && PctEnd(pSpawn) < EndMedStartAt)
		{
			char test[MAX_STRING];
			sprintf_s(test, "${If[${Me.CombatAbilityReady[%s]} && !${Me.ActiveDisc.ID},1,0]}", EndRegenSpell->Name);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
			{
				CastSpell(GetCharInfo()->pSpawn, EndRegenSpell, "/ag", 0);
			}
		}
	}
	if ((PctEnd(pSpawn) < EndMedStartAt || EndMedding && PctEnd(pSpawn) < EndMedStopAt || EndMedding && PctEnd(pSpawn) < 98 && EndMedToFull) && pSpawn->StandState == STANDSTATE_STAND && EndMedStartAt)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/sit on", FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: End at %d percent.  Medding.", PctEnd(GetCharInfo()->pSpawn));
		EndMedding = true;
		if (MedToFull)
			MedTimer = MQGetTickCount64() + 2000LL;
		else
			MedTimer = MQGetTickCount64() + 15000LL;
	}
}

void CheckFade()
{
	if (!InGameOK())
		return;
	if (!vFade.size() || !vAdds.size() || !GetCharInfo()->pSpawn)
		return;
	if ((GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 && !BardClass)
		return;
	if (!vFade[0])
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING];
	sprintf_s(test, "${If[${Me.AltAbilityReady[%s]} && ${Me.PctHPs}<%d && ${Group.MainTank.ID}!=${Me.ID},1,0]}", vFadeName[0].c_str(), FadeAt);
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		if (*EQADDR_ATTACK)
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /attack off", FromPlugin);
		if (GetCharInfo()->pSpawn->mActorClient.Class == 8)
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /bottwist off", FromPlugin);
		CastAA(GetCharInfo()->pSpawn, vFade[0], "\a-w", vFadeName[0].c_str());
	}
}

void CheckFightBuffs()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if (UseFightBuffs == 0 || !vFightBuff.size())
		return;
	if (GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange && GroupMainTank() != GetCharInfo()->pSpawn)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "";
	bool valid = true;
	strcpy_s(sSpell, "SpireIf");
	SpellIt = SpellIf.find(sSpell);
	if (SpellIt != SpellIf.end())
	{
		sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
		ParseMacroData(sSpell, MAX_STRING);
		if (atoi(sSpell) == 0)
			valid = false;
	}
	sprintf_s(sTest, "${If[${Me.AltAbilityReady[%s]},1,0]}", Spire);
	ParseMacroData(sTest, MAX_STRING);
	if (atoi(sTest) == 1 && valid)
	{
		sprintf_s(sTest, "/alt act ${Me.AltAbility[%s].ID}", Spire);
		ParseMacroData(sTest, MAX_STRING);
		HideDoCommand(GetCharInfo()->pSpawn, sTest, FromPlugin);
		if (AnnounceEcho && vAdds[0])
		{
			WriteChatf("\am%s", Spire);
		}
		if (AnnounceEQBC && vAdds[0])
		{
			EQBCSwap("\am");
			sprintf_s(sTest, "/bc %s%s", EQBCColor, Spire);
			HideDoCommand(GetCharInfo()->pSpawn, sTest, FromPlugin);
		}
		//WriteChatf("\am%s",Spire);
	}
	int capAmt = vFightBuff.size();


	for (int i = 0; i < capAmt; i++)
	{
		if (!vFightBuff[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
			return;
		bool valid = true;
		sprintf_s(sSpell, "FightBuffIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (valid && SpellStacks(vFightBuff[i]) && (!GetCharInfo()->pSpawn->Mount || !strstr(vFightBuff[i]->Name, "Yaulp"))) {
			sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && ${Spell[%s].Stacks} && !${Bot.HaveBuff[%s].ID} && ${Me.CurrentMana}>${Spell[%s].Mana} && !${Me.Casting.ID},1,0]}", vFightBuff[i]->Name, vFightBuff[i]->Name, vFightBuff[i]->Name, vFightBuff[i]->Name);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				if (vFightBuff[i]->TargetType != 6 && vFightBuff[i]->TargetType != 3 && vFightBuff[i]->TargetType != 29) // && vFightBuff[i]->TargetType != 45
					TargetIT(GetCharInfo()->pSpawn);
				CastSpell(GetCharInfo()->pSpawn, vFightBuff[i], "\am", 0);
			}
			sprintf_s(sSpell, "${If[${Me.AltAbility[%s].Spell.ID} && ${Me.AltAbilityReady[%s]} && ${Spell[${Me.AltAbility[%s].Spell}].Stacks} && !${Bot.HaveBuff[${Me.AltAbility[%s].Spell}].ID},1,0]}", vFightBuffName[i].c_str(), vFightBuffName[i].c_str(), vFightBuffName[i].c_str(), vFightBuffName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				if (vFightBuff[i]->TargetType != 6 && vFightBuff[i]->TargetType != 3 && vFightBuff[i]->TargetType != 29) // && vFightBuff[i]->TargetType != 45
					TargetIT(GetCharInfo()->pSpawn);
				CastAA(GetCharInfo()->pSpawn, vFightBuff[i], "\am", vFightBuffName[i].c_str());
			}
		}
	}
}

void CheckGrab()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if (!GrabAdds && !GrabPrimary || !vGrab.size())
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	int adds = vAdds.size();
	if (adds > 0 && GrabAdds == 0)
		adds = 1;

	for (int z = 0; z < adds; z++)
	{
		if (!vAdds[z])
			return;
		if (GetDistance(vAdds[z]->X, vAdds[z]->Y) < GrabPrimary && z == 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) < GrabAdds && z > 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) > AssistRange)
			continue;
		bool SkipOne = false;
		int capAmt = vGrab.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vGrab[i])
				continue;
			SkipOne = false;
			if (MQGetTickCount64() < SpellTimer)
				return;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				if (vAdds[z] && vTimerSpawn[x])
					if (vGrab[i] == vTimerSpell[x] && vAdds[z] == vTimerSpawn[x])
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			valid = ValidDet(vGrab[i], vAdds[z]);
			sprintf_s(sSpell, "GrabIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid)
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vGrabName[i].c_str(), vGrab[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastSpell(vAdds[z], vGrab[i], "\a-g", 0);
					if (vGrab[i]->TargetType == 5)
						LastSnareSingle = true;
					//AddTimer(vAdds[z],vGrab[i],0);
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vGrabName[i].c_str(), vGrab[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastAA(vAdds[z], vGrab[i], "\a-g", vGrabName[i].c_str());
					if (vGrab[i]->TargetType == 5)
						LastSnareSingle = true;
					//AddTimer(vAdds[z],vGrab[i],0);
				}
			}
		}
	}
}

void CheckHeals()
{
	if (!InGameOK())
		return;
	if (!vHeal.size() || MQGetTickCount64() < HealTimer)
		return;
	if (PSPELL pSpell = GetSpellByID(GetCharInfo()->pSpawn->CastingData.SpellID))
		if (pSpell->Category == 42)
			return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "";
	int HealPct;
	int capAmt = vHeal.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vHeal[i])
			continue;
		bool valid = true;
		bool duration = false;
		sprintf_s(sSpell, "HealIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				continue;
		}
		HealPct = HealAt;
		//if(MQGetTickCount64()<SpellTimer)
		//	return;
		if (vHeal[i]->TargetType == 45 || vHeal[i]->TargetType == 41 || vHeal[i]->TargetType == 3 || vHeal[i]->TargetType == 8)
			HealPct = HealGroupAt;
		if (vHeal[i]->TargetType == 6 && !strstr(vHealName[i].c_str(), "Aurora"))
			HealPct = HealSelfAt;
		if (vHeal[i]->Subcategory == 140)
			HealPct = HealDelayedAt;
		if (vHeal[i]->Subcategory == 32)
			HealPct = HealDurationAt;
		if (vHeal[i]->Range > 0)
		{
			GroupMinHP((int)(botround(vHeal[i]->Range*benRngExtend)));
			if (gMinHP == NULL)
				continue;
			if (gMinHP && gMinHP->HPMax != 0 && GetCharInfo()->pSpawn->ManaCurrent >= (int)vHeal[i]->ManaCost) //&& SpawnTimer(gMinHP)>5000
				if (PctHP(gMinHP) < HealPct && (vHeal[i]->TargetType != 6 || gMinHP == GetCharInfo()->pSpawn || strstr(vHealName[i].c_str(), "Aurora of ")) && (vHeal[i]->Subcategory != 77 || PctHP(gMinHP) < 35 || strstr(vHealName[i].c_str(), "Aurora of ") || strstr(vHealName[i].c_str(), "Burst of ") || strstr(vHealName[i].c_str(), "Remedy")))
				{
					if (PctHP(gMinHP) < InterruptToHealAt && GetCharInfo()->pSpawn->CastingData.SpellID)
						if (PSPELL pSpell = GetSpellByID(GetCharInfo()->pSpawn->CastingData.SpellID))
							if (pSpell->Category != 42)
								HideDoCommand(GetCharInfo()->pSpawn, "/stopcast", FromPlugin);
					bool SkipOne = false;
					if (GetSpellDuration(vHeal[i], (PSPAWNINFO)pLocalPlayer) > 0)
					{
						duration = true;
						for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
						{
							SkipOne = false;
							if (gMinHP && vTimerSpawn[x])
								if (vHeal[i] == vTimerSpell[x] && gMinHP == vTimerSpawn[x])
								{
									SkipOne = true;
									break;
								}
						}
					}
					if (SkipOne)
						continue;
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && (${Bot.SpellDuration[%s]}<4||!${Me.Song[celestial regeneration].ID} && !${Me.Song[ancestral aid].ID}) && !${Bot.HaveBuff[%s].ID} && !${Me.Song[%s].ID},1,0]}", vHealName[i].c_str(), vHealName[i].c_str(), vHealName[i].c_str(), vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinHP && (gMinHP->Type != SPAWN_CORPSE))
					{
						if (vHeal[i]->TargetType != 6 && vHeal[i]->TargetType != 3 && vHeal[i]->TargetType != 41) //&& vHeal[i]->TargetType != 45
							TargetIT(gMinHP);
						CastSpell(gMinHP, vHeal[i], "\at", 0);
						HealTimer = MQGetTickCount64() + 1000LL;
						if (vHeal[i]->Subcategory == 140)
							AddTimer(gMinHP, vHeal[i], 30000);
						if (duration)
						{
							char test[MAX_STRING] = "";
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vHealName[i].c_str());
							ParseMacroData(test, MAX_STRING);
							ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vHeal[i]);
							ULONGLONG recastTime = MQGetTickCount64() + test2;
							AddTimer(gMinHP, vHeal[i], recastTime);
						}
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.HaveBuff[%s].ID} && !${Me.Song[%s].ID},1,0]}", vHealName[i].c_str(), vHealName[i].c_str(), vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinHP && (gMinHP->Type != SPAWN_CORPSE))
					{
						TargetIT(gMinHP);
						CastAA(gMinHP, vHeal[i], "\at", vHealName[i].c_str());
						HealTimer = MQGetTickCount64() + 1000LL;
						if (duration)
						{
							char test[MAX_STRING] = "";
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vHealName[i].c_str());
							ParseMacroData(test, MAX_STRING);
							ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vHeal[i]);
							ULONGLONG recastTime = MQGetTickCount64() + test2;
							AddTimer(gMinHP, vHeal[i], recastTime);
						}
						break;
					}
				}

			XTMinHP((int)(botround(vHeal[i]->Range*benRngExtend)));
			if (xMinHP == NULL)
				continue;
			if (xMinHP && xMinHP->HPMax != 0 && vHeal[i]->TargetType == 5)
				if ((xMinHP->HPCurrent * 100 / xMinHP->HPMax) < HealPct && (vHeal[i]->Subcategory != 77 || (xMinHP->HPCurrent * 100 / xMinHP->HPMax) < 35 || strstr(vHealName[i].c_str(), "Remedy")))
				{
					if (GetSpellDuration(vHeal[i], (PSPAWNINFO)pLocalPlayer) > 0)
					{
						bool SkipOne = false;
						for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
						{
							SkipOne = false;
							if (xMinHP && vTimerSpawn[x])
								if (vHeal[i] == vTimerSpell[x] && xMinHP == vTimerSpawn[x])
								{
									SkipOne = true;
									break;
								}
						}
						if (SkipOne)
							continue;
					}
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && xMinHP && (xMinHP->Type != SPAWN_CORPSE))
					{
						TargetIT(xMinHP);
						CastSpell(xMinHP, vHeal[i], "\at", 0);
						HealTimer = MQGetTickCount64() + 1000LL;
						if (vHeal[i]->Subcategory == 140)
							AddTimer(xMinHP, vHeal[i], 30000);
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]},1,0]}", vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && xMinHP && (xMinHP->Type != SPAWN_CORPSE))
					{
						TargetIT(xMinHP);
						CastAA(xMinHP, vHeal[i], "\at", vHealName[i].c_str());
						HealTimer = MQGetTickCount64() + 1000LL;
						if (duration)
						{
							char test[MAX_STRING] = "";
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vHealName[i].c_str());
							ParseMacroData(test, MAX_STRING);
							ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vHeal[i]);
							ULONGLONG recastTime = MQGetTickCount64() + test2;
							AddTimer(xMinHP, vHeal[i], recastTime);
						}
						break;
					}
				}
			if (UseNetBots && vHeal[i]->TargetType == 5)
			{
				NetBotsMinHP((int)(botround(vHeal[i]->Range*benRngExtend)));
				if (nMinHP == NULL)
					continue;
				if (nMinHP && NetBotsHP != 0)
					if (NetBotsHP < HealPct && (vHeal[i]->Subcategory != 77 || NetBotsHP < 35 || strstr(vHealName[i].c_str(), "Remedy")))
					{
						if (GetSpellDuration(vHeal[i], (PSPAWNINFO)pLocalPlayer) > 0)
						{
							bool SkipOne = false;
							for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
							{
								SkipOne = false;
								if (nMinHP && vTimerSpawn[x])
									if (vHeal[i] == vTimerSpell[x] && nMinHP == vTimerSpawn[x])
									{
										SkipOne = true;
										break;
									}
							}
							if (SkipOne)
								continue;
						}
						sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vHealName[i].c_str());
						ParseMacroData(sSpell, MAX_STRING);
						if (atoi(sSpell) == 1 && nMinHP && (nMinHP->Type != SPAWN_CORPSE))
						{
							TargetIT(nMinHP);
							CastSpell(nMinHP, vHeal[i], "\at", 0);
							HealTimer = MQGetTickCount64() + 1000LL;
							if (vHeal[i]->Subcategory == 140)
								AddTimer(nMinHP, vHeal[i], (ULONGLONG)CalcDuration(vHeal[i]));
							break;
						}
						sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]},1,0]}", vHealName[i].c_str());
						ParseMacroData(sSpell, MAX_STRING);
						if (atoi(sSpell) == 1 && nMinHP && (nMinHP->Type != SPAWN_CORPSE))
						{
							TargetIT(nMinHP);
							CastAA(nMinHP, vHeal[i], "\at", vHealName[i].c_str());
							HealTimer = MQGetTickCount64() + 1000LL;
							if (duration)
							{
								char test[MAX_STRING] = "";
								sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vHealName[i].c_str());
								ParseMacroData(test, MAX_STRING);
								ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vHeal[i]);
								ULONGLONG recastTime = MQGetTickCount64() + test2;
								AddTimer(nMinHP, vHeal[i], recastTime);
							}
							break;
						}
					}
			}
		}
		if (vHeal[i]->AERange > 0 && gMinHP)
		{
			GroupMinHP((int)(botround(vHeal[i]->AERange*benRngExtend)));
			if (gMinHP == NULL)
				continue;
			if (gMinHP && gMinHP->HPMax != 0)
				if (PctHP(gMinHP) < HealPct && (vHeal[i]->Subcategory != 77 || (gMinHP->HPCurrent * 100 / gMinHP->HPMax) < 35 || strstr(vHealName[i].c_str(), "Remedy")))
				{
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Bot.HaveBuff[%s].ID} && !${Me.Song[%s].ID},1,0]}", vHealName[i].c_str(), vHealName[i].c_str(), vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinHP && (gMinHP->Type != SPAWN_CORPSE))
					{
						TargetIT(gMinHP);
						CastSpell(gMinHP, vHeal[i], "\at", 1);
						HealTimer = MQGetTickCount64() + 1000LL;
						if (vHeal[i]->Subcategory == 140)
							AddTimer(gMinHP, vHeal[i], 30000);
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.HaveBuff[%s].ID} && !${Me.Song[%s].ID},1,0]}", vHealName[i].c_str(), vHealName[i].c_str(), vHealName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinHP && (gMinHP->Type != SPAWN_CORPSE))
					{
						TargetIT(gMinHP);
						CastAA(gMinHP, vHeal[i], "\at", vHealName[i].c_str());
						HealTimer = MQGetTickCount64() + 1000LL;
						if (duration)
						{
							char test[MAX_STRING] = "";
							sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vHealName[i].c_str());
							ParseMacroData(test, MAX_STRING);
							ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vHeal[i]);
							ULONGLONG recastTime = MQGetTickCount64() + test2;
							AddTimer(gMinHP, vHeal[i], recastTime);
						}

						break;
					}
				}
		}
	}

}

void CheckHealPets()
{
	if (!InGameOK())
		return;
	if (!vHealPet.size() || MQGetTickCount64() < SpellTimer)
		return;
	PSPAWNINFO pLocalSpawn = GetCharInfo()->pSpawn;
	if (!pLocalSpawn)
		return;
	if (PSPELL pSpell = GetSpellByID(pLocalSpawn->CastingData.SpellID))
		if (pSpell->Category == 42)
			return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "";
	int capAmt = vHealPet.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vHealPet[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
			return;
		if (vHealPet[i]->Range > 0)
		{
			GroupMinPetHP((int)(botround(vHealPet[i]->Range*benRngExtend)));
			if (!gMinPetHP)
				continue;
			// if (pLocalSpawn->Class == 13 || pLocalSpawn->Class == 11 || pLocalSpawn->Class == 14 || pLocalSpawn->Class == 5)
			if (vHealPet[i]->TargetType == 14)
			{
				if (pLocalSpawn->PetID && pLocalSpawn->PetID != 0xFFFFFFFF)
					if (gMinPetHP->SpawnID != pLocalSpawn->PetID)
						continue;
			}
			if (gMinPetHP && gMinPetHP->HPMax > 0 && gMinPetHP->Type != SPAWN_CORPSE)
			{
				if ((gMinPetHP->HPCurrent * 100 / gMinPetHP->HPMax) < HealPetAt && (vHealPet[i]->Subcategory != 77 || (gMinPetHP->HPCurrent * 100 / gMinPetHP->HPMax) < 35 || strstr(vHealPetName[i].c_str(), "Remedy")))
				{
					bool SkipOne = false;
					if (GetSpellDuration(vHealPet[i], (PSPAWNINFO)pLocalPlayer) > 0)
					{
						for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
						{
							SkipOne = false;
							if (gMinPetHP && vTimerSpawn[x] && vTimerSpell[x])
								if (vHealPet[i] == vTimerSpell[x] && gMinPetHP == vTimerSpawn[x])
								{
									SkipOne = true;
									break;
								}
						}
						if (SkipOne)
							continue;
					}
					if (!gMinPetHP || gMinPetHP->Type == SPAWN_CORPSE)
						return;
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vHealPetName[i].c_str(), vHealPet[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinPetHP)
					{
						if (vHealPet[i]->TargetType == 5 || vHealPet[i]->TargetType == 43)
							TargetIT(gMinPetHP);
						CastSpell(gMinPetHP, vHealPet[i], "\a-t", 0);
						if (vHealPet[i]->Subcategory == 140)
							AddTimer(gMinPetHP, vHealPet[i], 30000);
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vHealPetName[i].c_str(), vHealPet[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinPetHP)
					{
						if (vHealPet[i]->TargetType == 5 || vHealPet[i]->TargetType == 43)
							TargetIT(gMinPetHP);
						CastAA(gMinPetHP, vHealPet[i], "\a-t", vHealPetName[i].c_str());
						break;
					}
				}
			}
			XTMinPetHP((int)(botround(vHealPet[i]->Range*benRngExtend)));
			if (!xMinPetHP)
				continue;
			if (xMinPetHP && xMinPetHP->HPMax > 0)
			{
				if ((xMinPetHP->HPCurrent * 100 / xMinPetHP->HPMax) < HealPetAt && (vHealPet[i]->Subcategory != 77 || (xMinPetHP->HPCurrent * 100 / xMinPetHP->HPMax) < 35 || strstr(vHealPetName[i].c_str(), "Remedy")))
				{
					if (GetSpellDuration(vHealPet[i], (PSPAWNINFO)pLocalPlayer) > 0)
					{
						bool SkipOne = false;
						for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
						{
							SkipOne = false;
							if (xMinPetHP && vTimerSpawn[x] && vTimerSpell[x])
								if (vHealPet[i] == vTimerSpell[x] && xMinPetHP == vTimerSpawn[x])
								{
									SkipOne = true;
									break;
								}
						}
						if (SkipOne)
							continue;
					}
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vHealPetName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && xMinPetHP)
					{
						TargetIT(xMinPetHP);
						CastSpell(xMinPetHP, vHealPet[i], "\a-t", 0);
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]},1,0]}", vHealPetName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && xMinPetHP)
					{
						TargetIT(xMinPetHP);
						CastAA(xMinPetHP, vHealPet[i], "\a-t", vHealPetName[i].c_str());
						break;
					}
				}
			}
		}
		if (vHealPet[i]->AERange > 0)
		{
			GroupMinPetHP((int)(botround(vHealPet[i]->AERange*benRngExtend)));
			if (!gMinPetHP)
				continue;
			if (gMinPetHP && gMinPetHP->HPMax > 0)
			{
				if ((gMinPetHP->HPCurrent * 100 / gMinPetHP->HPMax) < HealPetAt && (vHealPet[i]->Subcategory != 77 || (gMinPetHP->HPCurrent * 100 / gMinPetHP->HPMax) < 35 || strstr(vHealPetName[i].c_str(), "Remedy")))
				{
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vHealPetName[i].c_str(), vHealPet[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinPetHP)
					{
						if (vHealPet[i]->TargetType == 5 || vHealPet[i]->TargetType == 43)
							TargetIT(gMinPetHP);
						CastSpell(gMinPetHP, vHealPet[i], "\a-t", 1);
						break;
					}
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vHealPetName[i].c_str(), vHealPet[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && gMinPetHP)
					{
						if (vHealPet[i]->TargetType == 5 || vHealPet[i]->TargetType == 43)
							TargetIT(gMinPetHP);
						CastAA(gMinPetHP, vHealPet[i], "\a-t", vHealPetName[i].c_str());
						break;
					}
				}
			}
		}
	}
}

void CheckHealToT()
{
	if (!InGameOK())
		return;
	if (!pAggroInfo || MQGetTickCount64() < SpellTimer || !vAdds.size())
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (pAggroInfo)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)GetCharInfo()->pSpawn->TargetOfTarget))
		{
			if (PctHP(pSpawn) < HealToTAt)
			{
				bool valid = true;
				char sSpell[MAX_STRING] = "", cSpell[MAX_STRING] = "";
				int capAmt = vHealToT.size();
				for (int i = 0; i <capAmt; i++)
				{
					valid = true;
					sprintf_s(cSpell, "HealToTIf%d", i);
					SpellIt = SpellIf.find(cSpell);
					if (SpellIt != SpellIf.end())
					{
						sprintf_s(cSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
						ParseMacroData(cSpell, MAX_STRING);
						if (atoi(cSpell) == 0)
							valid = false;
					}
					if (valid && ppTarget)
					{
						sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vHealToT[i]->Name);
						ParseMacroData(sSpell, MAX_STRING);
						if (atoi(sSpell) == 1 && valid)
						{
							CastSpell((PSPAWNINFO)pTarget, vHealToT[i], "\a-t", 0);
							break;
						}
						sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]},1,0]}", vHealToTName[i].c_str());
						ParseMacroData(sSpell, MAX_STRING);
						if (atoi(sSpell) == 1 && valid)
						{
							CastAA((PSPAWNINFO)pTarget, vHealToT[i], "\a-t", vHealToTName[i].c_str());
							break;
						}
					}
				}
			}
		}
	}

}

void CheckImHit()
{
	if (!InGameOK())
		return;
	if (!vImHit.size() || !vAdds.size() && !vPossibleAdds.size() || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 && !BardClass)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "";
	char castLine[MAX_STRING], castTimerInfo[MAX_STRING];
	PSPELL pSpell;
	bool valid = false;
	bool named = false;
	int capAmt = vImHit.size();

	for (int i = 0; i < capAmt; i++)
	{
		if (MQGetTickCount64() < SpellTimer)
			return;
		char sSpell[MAX_STRING];
		sprintf_s(sSpell, "ImHitIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				continue;
		}
		char iSpell[MAX_STRING] = "";
		sprintf_s(iSpell, "%s", vImHit[i].c_str());
		if (PSPELL pSpell = GetSpellByName(iSpell))
			if (CACheck(pSpell->ID))
			{
				DWORD timeNow = (DWORD)time(NULL);
				if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) < timeNow && GetCharInfo()->pSpawn->EnduranceCurrent > (int)pSpell->EnduranceCost)
				{
					char test[MAX_STRING];
					sprintf_s(test, "${If[(!${Me.ActiveDisc.ID}||!${Bot.SpellDuration[%s]}),1,0]}", vImHit[i].c_str());
					ParseMacroData(test, MAX_STRING);
					if (atoi(test) == 1)
					{
						char spellColor[MAX_STRING];
						sprintf_s(test, "/disc %s", vImHit[i].c_str());
						HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
						if (AnnounceEcho)
							WriteChatf("\ay%s", vImHit[i].c_str());
						if (AnnounceEQBC)
						{
							strcpy_s(spellColor, "\ay");
							EQBCSwap(spellColor);
							sprintf_s(spellColor, "/bc %s%s", EQBCColor, vImHit[i].c_str());
							HideDoCommand(GetCharInfo()->pSpawn, spellColor, FromPlugin);
						}
						return;
					}
				}
			}
		sprintf_s(sTest, "${If[${Me.AltAbilityReady[%s]} && ${Me.PctHPs}<%d,1,0]}", vImHit[i].c_str(), ImHitAt);
		ParseMacroData(sTest, MAX_STRING);
		if (atoi(sTest) == 1)
		{
			sprintf_s(sSpell, "${Me.AltAbility[%s].Spell.ID}", vImHit[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			pSpell = GetSpellByID((DWORD)atol(sSpell));
			if (pSpell && pSpell->SpellType != 0)
			{
				if (SpellStacks(pSpell))
				{
					if (pSpell->TargetType == 5)
						TargetIT(GetCharInfo()->pSpawn);
					sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", vImHit[i].c_str());
					ParseMacroData(castTimerInfo, MAX_STRING);
					SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 600LL;
					sprintf_s(castLine, "/multiline ; /stopcast ; /casting \"%s\" alt", vImHit[i].c_str());
					DoCommand(GetCharInfo()->pSpawn, castLine);
					if (AnnounceEcho)
						WriteChatf("\ay%s", vImHit[i].c_str());
					if (AnnounceEQBC)
					{
						char spellColor[MAX_STRING];
						strcpy_s(spellColor, "\ay");
						EQBCSwap(spellColor);
						sprintf_s(spellColor, "/bc %s%s", EQBCColor, vImHit[i].c_str());
						HideDoCommand(GetCharInfo()->pSpawn, spellColor, FromPlugin);
					}
					return;
				}
			}
			if (pSpell && pSpell->SpellType == 0 && vAdds[0])
			{
				valid = ValidDet(pSpell, vAdds[0]);
				if (valid)
				{
					TargetIT(vAdds[0]);
					sprintf_s(castTimerInfo, "${Me.AltAbility[%s].Spell.MyCastTime.Float}", vImHit[i].c_str());
					ParseMacroData(castTimerInfo, MAX_STRING);
					SpellTimer = MQGetTickCount64() + (ULONGLONG)floor((atof(castTimerInfo) * 1000)) + 600LL;
					sprintf_s(castLine, "/multiline ; /stopcast ; /casting \"%s\" alt", vImHit[i].c_str());
					DoCommand(GetCharInfo()->pSpawn, castLine);
					if (AnnounceEcho)
						WriteChatf("\ay%s", vImHit[i].c_str());
					if (AnnounceEQBC)
					{
						char spellColor[MAX_STRING];
						strcpy_s(spellColor, "\ay");
						EQBCSwap(spellColor);
						sprintf_s(spellColor, "/bc %s%s", EQBCColor, vImHit[i].c_str());
						HideDoCommand(GetCharInfo()->pSpawn, spellColor, FromPlugin);
					}
					if (GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 0)
						AddTimer(vAdds[0], pSpell, 0);
					return;
				}
			}

		}
	}
}

void CheckInvis()
{
	if (!InGameOK())
		return;
	InvisProximity(GetCharInfo()->pSpawn, (float)AutoInvisDistance);
	IVUProximity(GetCharInfo()->pSpawn, (float)AutoInvisDistance);
	if (vInvis.size() && !CheckInvisStatus() && AutoInvisDistance && invis > 0 && (!vInvisUndead.size() || ivu == 0))
		HideDoCommand(GetCharInfo()->pSpawn, "/invis", FromPlugin);
	if (vInvisUndead.size() && !CheckIVUStatus() && AutoInvisDistance && ivu > 0)
		HideDoCommand(GetCharInfo()->pSpawn, "/ivu", FromPlugin);
}

void CheckJolt()
{
	{
		if (!InGameOK())
			return;
		if (!vAdds.size() && !vPossibleAdds.size())
			return;
		if (!vAdds[0])
			return;
		if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
			return;
		if (!GetCharInfo()->pGroupInfo || !vJolt.size())
			return;
		strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
		DebugWrite("%s: Checking routine", CurrentRoutine);
		char sSpell[MAX_STRING] = "";
		int valid = 0, pctAggro = 0;
		if (GetCharInfo()->standstate == STANDSTATE_FEIGN && DoingFeignDeath)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Standing from Feign");
			DoingFeignDeath = 0;
		}
		int capAmt = vJolt.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vJolt[i])
				continue;
			if (MQGetTickCount64() < SpellTimer)
				return;
			bool SkipOne = false;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				SkipOne = false;
				if (vTimerSpawn[x])
					if (vJolt[i] == vTimerSpell[x] && vAdds[0] == vTimerSpawn[x])
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			if (vJolt[i]->AERange == 0)
			{
				if (vJolt[i]->Range < AssistRange)
					valid = AddCountByDistance((int)vJolt[i]->Range);
				else
					valid = AddCountByDistance(AssistRange);
			}
			if (vJolt[i]->AERange > 0)
			{
				if (vJolt[i]->Range < AssistRange)
					valid = AddCountByDistance((int)vJolt[i]->AERange);
				else
					valid = AddCountByDistance(AssistRange);
			}
			if (pAggroInfo)
				pctAggro = pAggroInfo->aggroData[AD_Player].AggroPct;
			sprintf_s(sSpell, "JoltIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid > 0 && (valid > 1 && vJolt[i]->AERange > 0 || pctAggro > JoltAt && JoltAt != 0))
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID} && !${Me.Song[${Spell[%s].AutoCast}].ID} && (!${Bot.Freeze}||${Spell[%s].Name.Find[freeze]}),1,0]}", vJoltName[i].c_str(), vJoltName[i].c_str(), vJoltName[i].c_str(), vJoltName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastSpell(vAdds[0], vJolt[i], "\at", 0);
					//AddTimer(vAdds[0],vJolt[i],0);
					DoingFeignDeath = true;
					if (GetSpellAttribX(vJolt[i], 0) == 74)
						SpellTimer = MQGetTickCount64() + 3000LL;
					break;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]}  && !${Target.Buff[%s].ID} && !${Me.Song[${Spell[%s].AutoCast}].ID},1,0]}", vJoltName[i].c_str(), vJolt[i]->Name, vJoltName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastAA(vAdds[0], vJolt[i], "\at", vJoltName[i].c_str());
					//AddTimer(vAdds[0],vJolt[i],0);
					DoingFeignDeath = true;
					if (GetSpellAttribX(vJolt[i], 0) == 74)
						SpellTimer = MQGetTickCount64() + 3000LL;
					break;
				}
			}
		}
	}
}

void CheckKnockback()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	if (!KnockbackAdds && !KnockbackPrimary || !vKnockback.size())
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	int adds = vAdds.size();
	if (adds > 0 && KnockbackAdds == 0)
		adds = 1;
	int capAmt = vKnockback.size();
	for (int z = 0; z<adds; z++)
	{
		if (!vAdds[z])
			return;
		if (GetDistance(vAdds[z]->X, vAdds[z]->Y)>KnockbackPrimary && z == 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) > KnockbackAdds && z > 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) > AssistRange)
			continue;
		for (int i = 0; i < capAmt; i++)
		{
			if (!vKnockback[i])
				continue;
			if (MQGetTickCount64() < SpellTimer)
				return;
			bool SkipOne = false;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				if (!vTimerSpawn[x])
					continue;
				SkipOne = false;
				if (vAdds[z] && vTimerSpawn[x])
					if (vKnockback[i] == vTimerSpell[x] && vAdds[z] == vTimerSpawn[x])
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			valid = ValidDet(vKnockback[i], vAdds[z]);
			sprintf_s(sSpell, "KnockbackIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid)
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vKnockbackName[i].c_str(), vKnockback[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastSpell(vAdds[z], vKnockback[i], "\a-g", 0);
					if (vKnockback[i]->TargetType == 5)
						LastSnareSingle = true;
					//AddTimer(vAdds[z],vKnockback[i],0);
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vKnockbackName[i].c_str(), vKnockback[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastAA(vAdds[z], vKnockback[i], "\a-g", vKnockbackName[i].c_str());
					if (vKnockback[i]->TargetType == 5)
						LastSnareSingle = true;
					//AddTimer(vAdds[z],vKnockback[i],0);
				}
			}
		}
	}
}

void CheckLifetaps()
{
	{
		if (!InGameOK())
			return;
		if (!vAdds.size() && !vPossibleAdds.size())
			return;
		if (!vAdds[0])
			return;
		if (!vLifetap.size() || MQGetTickCount64() < SpellTimer || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
			return;
		if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > LifetapAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
			return;
		strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
		DebugWrite("%s: Checking routine", CurrentRoutine);
		char sSpell[MAX_STRING] = "";
		int PctHP = GetCharInfo()->pSpawn->HPCurrent * 100 / GetCharInfo()->pSpawn->HPMax;
		bool valid = false;
		int capAmt = vLifetap.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vLifetap[i])
				continue;
			if (MQGetTickCount64() < SpellTimer)
				return;
			valid = ValidDet(vLifetap[i], vAdds[0]);
			sprintf_s(sSpell, "LifetapIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (vAdds[0] && valid && ((GetCharInfo()->pSpawn->HPMax - GetCharInfo()->pSpawn->HPCurrent) > abs(GetSpellBaseX(vLifetap[i], 0)) && !GetSpellDuration(vLifetap[i], (PSPAWNINFO)pLocalPlayer) && (PctHP < 80)))
			{
				sprintf_s(sSpell, "${If[!${Bot.HaveBuff[${Spell[%s].AutoCast}].ID} && ${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vLifetapName[i].c_str(), vLifetapName[i].c_str(), vLifetap[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastSpell(vAdds[0], vLifetap[i], "\ag", 0);
					break;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vLifetapName[i].c_str(), vLifetap[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastAA(vAdds[0], vLifetap[i], "\ag", vLifetapName[i].c_str());
					break;
				}
			}
		}
	}
}

void CheckLoot()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < LootTimer || !FindPlugins("MQ2AdvLoot"))
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING], lootCommand[MAX_STRING];
	strcpy_s(test, "${If[${MQAdvLoot.Active} && ${Me.FreeInventory}>0,1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1 && LootInCombat == 0 && InCombat)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /mqadvloot abort", FromPlugin);
		looting = false;
		return;
	}
	if (!FindPlugins("MQ2AdvLoot") || LootRadius == 0 || LootInCombat == 0 && InCombat || MQGetTickCount64() < LootTimer)
		return;
	sprintf_s(test, "${If[${Spawn[npccorpse radius %d].ID} && ${Me.FreeInventory},${Spawn[npccorpse radius %d].ID},0]}", LootRadius, LootRadius);
	ParseMacroData(test, MAX_STRING);
	if (atol(test) > 0)
	{
		if (looting == false)
			sprintf_s(campSpot, "/moveto loc %f %f %f loose", GetCharInfo()->pSpawn->Y, GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Z);
		sprintf_s(lootCommand, "/multiline ; /squelch /stick off ; /squelch /tar npccorpse ; /moveto id %s ; /timed 10 ; /squelch /mqadvloot", test);
		HideDoCommand(GetCharInfo()->pSpawn, lootCommand, FromPlugin);
		looting = true;
		LootTimer = MQGetTickCount64() + 3000LL;
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Looting with MQ2AdvLoot");
	}
	if (atol(test) == 0 && looting)
	{
		HideDoCommand(GetCharInfo()->pSpawn, campSpot, FromPlugin);
		looting = false;
	}
}

void CheckMainTankBuff()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < MTBTimer || MQGetTickCount64() < SpellTimer || UseMainTankBuffs == 0 || !vMainTankBuff.size() || !vAdds.size() && !vPossibleAdds.size() || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	PSPAWNINFO pSpawn = GroupMainTank();
	char maintank[MAX_STRING];
	strcpy_s(maintank, "${If[${Group.MainTank.ID},${Group.MainTank.ID},0]}");
	ParseMacroData(maintank, MAX_STRING);
	if (atol(maintank) < 1)
		return;
	if (pSpawn)
	{
		bool valid = false;
		char sSpell[MAX_STRING] = "";
		bool SkipOuterLoop;
		int capAmt = vMainTankBuff.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vMainTankBuff[i])
				continue;
			SkipOuterLoop = false;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				if (pSpawn && vTimerSpawn[x] && pSpawn->Type != SPAWN_CORPSE)
				{
					if (vMainTankBuff[i] == vTimerSpell[x] && pSpawn == vTimerSpawn[x])
					{
						SkipOuterLoop = true;
						break;
					}
				}
			}
			if (SkipOuterLoop)
				continue;
			if (MQGetTickCount64() < SpellTimer || pSpawn->Type == SPAWN_CORPSE)
				return;
			valid = ValidBen(vMainTankBuff[i], pSpawn);
			sprintf_s(sSpell, "MainTankBuffIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid && pSpawn && pSpawn->Type != SPAWN_CORPSE)
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vMainTankBuff[i]->Name, vMainTankBuff[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && pSpawn && pSpawn->Type != SPAWN_CORPSE)
				{
					TargetIT(pSpawn);
					//AddTimer(pSpawn,vMainTankBuff[i],0);
					//edit = add next 3 lines - 29 Jan 2015
					char test[MAX_STRING] = "";
					sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vMainTankBuff[i]->Name);
					ParseMacroData(test, MAX_STRING);
					ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vMainTankBuff[i]);
					ULONGLONG recastTime = MQGetTickCount64() + test2;
					AddTimer(pSpawn, vMainTankBuff[i], recastTime);
					CastSpell(pSpawn, vMainTankBuff[i], "\ag", 0);
					MTBTimer = MQGetTickCount64() + 4000LL;
					return;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.HaveBuff[%s].ID},1,0]}", vMainTankBuffName[i].c_str(), vMainTankBuffName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && pSpawn && pSpawn->Type != SPAWN_CORPSE)
				{
					TargetIT(pSpawn);
					CastAA(pSpawn, vMainTankBuff[i], "\ag", vMainTankBuffName[i].c_str());
					AddTimer(pSpawn, vMainTankBuff[i], (ULONGLONG)CalcDuration(vMainTankBuff[i]));
					MTBTimer = MQGetTickCount64() + 4000LL;
					return;
				}
			}
		}
	}
}

void CheckMana()
{
	//! Call DebugWrite() as few times as necessary for production code
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || Bot != 1 || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	bool valid;
	//? Separate out mod rod auto-inventory check so it's fairly quick
	if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->ManaMax > 0 && !BardClass && MQGetTickCount64() > modrodAITimer && UseModRods)
	{
		char modrod[MAX_STRING];
		modrodAITimer = MQGetTickCount64() + 500LL;
		strcpy_s(modrod, "${Cursor.ID}");
		ParseMacroData(modrod, MAX_STRING);
		int CID = atoi(modrod);
		//DebugWrite("%s: Cursor ID = %d", CurrentRoutine, CID);
		for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); CID > 0 && i >= 0; i--)
		{
			// Check if the modrod we are looking for is on cursor - if so, /autoinv it so we can use it
			//DebugWrite("%s: Checking modrod #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
			if (CID == ModRods[i].ItemID)
			{
				ModRods[i].InStock = 1;
				DebugWrite("%s: Found %s.  Adding to inventory.", CurrentRoutine, ModRods[i].szItem);
				HideDoCommand(GetCharInfo()->pSpawn, "/autoinventory", FromPlugin);
				modrodAITimer = MQGetTickCount64() + 2000LL; // let's give it a tad bit of time more, won't have another that quick except in rare cases
				return; // Give time for inv to update, let's just return, and check on next call
			}
		}
	}
	//?  Added a timer for modroduse, to separate the find and use check from the summon check and general mana route checks
	//!? USE CHECK
	if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->ManaMax > 0 && !BardClass && MQGetTickCount64() > modrodUseTimer && UseModRods)
	{
		char modrod[MAX_STRING];
		//? MOVED: Code for checking modrods at startup removed - there is no need, if we are checking for use/summon in this routine (which we should)
		//? Check for a modrod (any), finding highest level rod/wand
		//? If useable due to current mana need, use it
		//? Continue through each modrod and eval/use again if found/needed
		//? When checking, check for self type first, then AA type, then mass type
		//? For mage, after checking for use, check again to see if summon needed
		//? If summon needed and ready (Self/AA/Mass), then cast (NOTE: Should we have other conditions for mass, e.g. not use it at all? )
		//!? NOTE:  This doesn't need to be terribly difficult, just look/find/use/recast
		//! htw note to self:  If there aren't any condtions other than usemodrod (sic), then add options for If for modrod1/modrod2/modrodalt
		// Just search through and used the highest in our array.  If we should change order of check/use, then change the array, not this check loop!
		for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); i >= 0; i--)
		{
			if (ModRods[i].InStock == 0 && ModRods[i].RecheckTimer < MQGetTickCount64() && modrodRecheckTimer)
				continue;
			// Calculate current mana needed
			long manaReq = (GetCharInfo() && GetCharInfo()->pSpawn) ? GetCharInfo()->pSpawn->ManaMax - GetCharInfo()->pSpawn->ManaCurrent : 0;
			// See if we can find the modrod in inventory
			char szTempItem[MAX_STRING] = { 0 };
			strcpy_s(szTempItem, ModRods[i].szItem);
			CItemLocation cFindItem;
			if (!ItemFind(&cFindItem, szTempItem))
			{
				DebugWrite("%s: Can't find #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				ModRods[i].InStock = 0;
				ModRods[i].RecheckTimer = MQGetTickCount64() + 60000LL;
				continue; // item not found, so not checking to see if we need to use it
			}
			PCONTENTS itemFind = cFindItem.pBagSlot;
			if (!itemFind)
			{
				DebugWrite("%s: Can't find (bagslot) #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				ModRods[i].InStock = 0;
				ModRods[i].RecheckTimer = MQGetTickCount64() + 60000LL;
				continue; // Can't get a pointer to the item loc, so skip it
			}
			// Don't see why we'd even have it if not req, i.e. lowbies with mass, but checking here.
			//? Use the returned item required level
			if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Level < ModRods[i].ReqLevel)
			{
				DebugWrite("%s: Too low level for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				ModRods[i].InStock = 0;
				ModRods[i].RecheckTimer = MQGetTickCount64() + 999999LL;
				continue; // Can't use
			}
			ModRods[i].InStock = 1;
			modrodRecheckTimer = MQGetTickCount64();
			DebugWrite("%s: modrod available #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
			if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->HPCurrent <= (int)(ModRods[i].HPCost * 1.1))
			{
				DebugWrite("%s: Not enough HP for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				continue; // Let's not get crazy LOL, let's our current HP to be > 1.1 times the hp cost
			}
			if (ModRods[i].ManaAdded > (int)(manaReq * 0.8))
			{
				DebugWrite("%s: Current mana too high for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				continue; // don't waste, let's factor in needing it to add at least 80% of needed mana
			}
			// Found this modrod, should we use it?  Let's just use mq2cast for the check
			sprintf_s(modrod, "${If[${Cast.Ready[%s]},1,0]}", ModRods[i].szItem);
			ParseMacroData(modrod, MAX_STRING);
			if (atoi(modrod) == 1) // yep, it's ready to use, and passed the level, HP, mana checks
			{
				char CastLine[MAX_STRING];
				sprintf_s(CastLine, "/multiline ; /stopsong ; /nomodkey /casting \"%s\" item", ModRods[i].szItem);
				WriteChatf("\awClicking --> \a-o%s", ModRods[i].szItem);
				DoCommand(NULL, CastLine);
				modrodUseTimer = MQGetTickCount64() + 5000LL; // mod rod use check routine doesn't really need to be called so often, let's use a reasonable delay for price/perf
				return;
			}
			else
			{
				DebugWrite("%s: Not ready to cast #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
			}
			// Let's always make sure last routine, if falls through and no usage, waits another 5 secs to check again
			modrodUseTimer = MQGetTickCount64() + 5000LL;
		} // end for().  Let's keep checking through for another modrod, either this one wasn't found, or failed a check to be ready for use.
	}
	//!? SUMMON CHECK
	// If this is a mage, and it's been long enough delay, and not in combat & summoning modrod is enabled, check it out
	// We get 1 mass, 1 self, 1 aa, so we'll go through the array top to bottom and check each set
	if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->mActorClient.Class == 13 && MQGetTickCount64() > modrodTimer && !InCombat && SummonModRods)
	{
		// we already did calculations and checked the cursor, let's skip those
		// start the loop
		char sSpell[MAX_STRING];
		char modrod[MAX_STRING];
		bool SelfDone, MassDone, AltDone;
		SelfDone = false;
		MassDone = false;
		AltDone = false;
		DebugWrite("%s: Checking for summon modrod (mage).", CurrentRoutine);
		for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); i >= 0; i--)
		{
			DebugWrite("%s: Checking (summon) modrod #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
			if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Level < ModRods[i].ReqLevel)
				continue;
			char szTempItem[MAX_STRING] = { 0 };
			strcpy_s(szTempItem, ModRods[i].szItem);
			CItemLocation cFindItem;
			if (ItemFind(&cFindItem, szTempItem))
			{ // found item, flag the type as done, and continue
				if (ModRods[i].ModRodType == MOD_AA)
				{
					DebugWrite("%s: Modrod type AA found #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
					AltDone = true;
				}
				else if (ModRods[i].ModRodTarget == MOD_MASS)
				{
					DebugWrite("%s: Modrod type MASS found #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
					MassDone = true;
				}
				else if (ModRods[i].ModRodTarget == MOD_SELF)
				{
					DebugWrite("%s: Modrod type SELF found #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
					SelfDone = true;
				}
			}
		}
		valid = true;
		strcpy_s(sSpell, "ModRodSelfIf");
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (!SelfDone && !valid)
			SelfDone = true;
		valid = true;
		strcpy_s(sSpell, "ModRodMassIf");
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (!MassDone && !valid)
			MassDone = true;
		valid = true;
		strcpy_s(sSpell, "ModRodAltIf");
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (!AltDone && !valid)
			AltDone = true;
		// At this point, let's go through and try to summon any missing type, just loop through - the multiple loops are easy and won't notice a perf hit
		if (!AltDone)
		{
			DebugWrite("%s: Modrod type AA not done - in loop.", CurrentRoutine);
			for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); i >= 0; i--)
			{
				DebugWrite("%s: Checking type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (ModRods[i].ModRodType != MOD_AA)
					continue;
				DebugWrite("%s: Found AA type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Level < ModRods[i].ReqLevel)
				{
					DebugWrite("%s: Too low level for #%d (%s) - skipping.", CurrentRoutine, i, ModRods[i].szItem);
					continue;
				}
				sprintf_s(modrod, "${If[!${Cursor.ID} && ${Me.FreeInventory}>0 && ${Me.AltAbilityReady[%s]},1,0]}", ModRods[i].szSpell);
				ParseMacroData(modrod, MAX_STRING);
				if (atoi(modrod) == 1)
				{
					DebugWrite("%s: Found and using AA type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
					WriteChatf("\awCreating - \ao%s", ModRods[i].szItem);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /tar clear", FromPlugin);
					sprintf_s(modrod, "/multiline ; /stopsong ; /casting \"%s\" alt", ModRods[i].szSpell);
					HideDoCommand(GetCharInfo()->pSpawn, modrod, FromPlugin);
					modrodTimer = MQGetTickCount64() + 5000LL;
					modrodUseTimer = MQGetTickCount64() + 5000LL;
					SpellTimer = MQGetTickCount64() + 1000LL;
					// Since we are summoning one, let's just return and wait for next iteration for any further checks
					return;
				}
				else
				{
					DebugWrite("%s: Failed ready check for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				}
			}
		}
		if (!SelfDone)
		{
			DebugWrite("%s: Modrod type SELF not done - in loop.", CurrentRoutine);
			for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); i >= 0; i--)
			{
				char szTemp[MAX_STRING];
				szTemp[0] = 0;
				DebugWrite("%s: Checking type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (ModRods[i].ModRodTarget != MOD_SELF)
					continue;
				DebugWrite("%s: Found SELF type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Level < ModRods[i].ReqLevel)
				{
					DebugWrite("%s: Too low level for #%d (%s) - skipping.", CurrentRoutine, i, ModRods[i].szItem);
					continue;
				}
				sprintf_s(modrod, "${If[${Me.Book[%s Rk. III]},1,0]}", ModRods[i].szSpell);
				ParseMacroData(modrod, MAX_STRING);
				if (atoi(modrod) == 1)
				{
					sprintf_s(szTemp, "%s Rk. III", ModRods[i].szSpell);
				}
				else
				{
					sprintf_s(modrod, "${If[${Me.Book[%s Rk. II]},1,0]}", ModRods[i].szSpell);
					ParseMacroData(modrod, MAX_STRING);
					if (atoi(modrod) == 1)
					{
						sprintf_s(szTemp, "%s Rk. II", ModRods[i].szSpell);
					}
					else
					{
						sprintf_s(modrod, "${If[${Me.Book[%s]},1,0]}", ModRods[i].szSpell);
						ParseMacroData(modrod, MAX_STRING);
						if (atoi(modrod) == 1)
						{
							sprintf_s(szTemp, "%s", ModRods[i].szSpell);
						}
					}
				}
				if (szTemp[0]) // Found our highest, we can either cast it, or break out (don't cast a lower level one for self/mass)
				{
					DebugWrite("%s: Found and using SELF type for #%d (%s) (%s).", CurrentRoutine, i, ModRods[i].szItem, szTemp);
					// This pSpell shouldn't fail, it's in our book - but let's make sure
					if (PSPELL pSpell = GetSpellByName(szTemp))
					{
						// if we don't have enough  mana, like I said, don't look for a lower level one, break out for next type check
						if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->ManaCurrent <= (int)pSpell->ManaCost)
						{
							DebugWrite("%s: Not enough mana - skipping SELF for now.", CurrentRoutine);
							break;
						}
						// skipping cast ready check - fails if not memmed, let's just call it
						// Let's see if it's ready to cast
						//sprintf_s(modrod, "${If[${Cast.Ready[%s]},1,0]}", szTemp);
						//ParseMacroData(modrod, MAX_STRING);
						//if (atoi(modrod) == 1)
						//{
						// ok, let's cast this one and go out of this function for now
						if (GetCharInfo() && GetCharInfo()->pSpawn)
						{
							sprintf_s(modrod, "${If[${Me.Gem[%s]}>0 && ${Me.SpellReady[%s]},1,0]}", szTemp, szTemp);
							ParseMacroData(modrod, MAX_STRING);
							if (atoi(modrod) > 0)
								WriteChatf("\awCreating - \ao%s", ModRods[i].szItem);
							TargetIT(GetCharInfo()->pSpawn);
							CastSpell(GetCharInfo()->pSpawn, pSpell, "\ao", 1);
						}
						modrodTimer = MQGetTickCount64() + 5000LL;
						modrodUseTimer = MQGetTickCount64() + 5000LL;
						return;
						//}
						// It's not ready, let's skip this type for now
						//DebugWrite("%s: Spell not ready - skipping SELF for now.", CurrentRoutine);
						//break;
					}
				}
			}
		}
		if (!MassDone)
		{
			DebugWrite("%s: Modrod type MASS not done - in loop.", CurrentRoutine);
			for (register int i = (sizeof(ModRods) / sizeof(ModRods[0]) - 1); i >= 0; i--)
			{
				char szTemp[MAX_STRING];
				szTemp[0] = 0;
				DebugWrite("%s: Checking type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (ModRods[i].ModRodTarget != MOD_MASS)
					continue;
				DebugWrite("%s: Found MASS type for #%d (%s).", CurrentRoutine, i, ModRods[i].szItem);
				if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Level < ModRods[i].ReqLevel)
				{
					DebugWrite("%s: Too low level for #%d (%s) - skipping.", CurrentRoutine, i, ModRods[i].szItem);
					continue;
				}
				sprintf_s(modrod, "${If[${Me.Book[%s Rk. III]},1,0]}", ModRods[i].szSpell);
				ParseMacroData(modrod, MAX_STRING);
				if (atoi(modrod) == 1)
				{
					sprintf_s(szTemp, "%s Rk. III", ModRods[i].szSpell);
				}
				else
				{
					sprintf_s(modrod, "${If[${Me.Book[%s Rk. II]},1,0]}", ModRods[i].szSpell);
					ParseMacroData(modrod, MAX_STRING);
					if (atoi(modrod) == 1)
					{
						sprintf_s(szTemp, "%s Rk. II", ModRods[i].szSpell);
					}
					else
					{
						sprintf_s(modrod, "${If[${Me.Book[%s]},1,0]}", ModRods[i].szSpell);
						ParseMacroData(modrod, MAX_STRING);
						if (atoi(modrod) == 1)
						{
							strcpy_s(szTemp, ModRods[i].szSpell);
						}
					}
				}
				if (szTemp[0]) // Found our highest, we can either cast it, or break out (don't cast a lower level one for self/mass)
				{
					DebugWrite("%s: Found and using MASS type for #%d (%s) (%s).", CurrentRoutine, i, ModRods[i].szItem, szTemp);
					// This pSpell shouldn't fail, it's in our book - but let's make sure
					if (PSPELL pSpell = GetSpellByName(szTemp))
					{
						// if we don't have enough  mana, like I said, don't look for a lower level one, break out for next type check
						if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->ManaCurrent <= (int)pSpell->ManaCost)
						{
							DebugWrite("%s: Not enough mana - skipping MASS for now.", CurrentRoutine);
							break;
						}
						// skipping cast ready check - fails if not memmed, let's just call it
						// Let's see if it's ready to cast
						//sprintf_s(modrod, "${If[${Cast.Ready[%s]},1,0]}", szTemp);
						//ParseMacroData(modrod, MAX_STRING);
						//if (atoi(modrod) == 1)
						//{
						// ok, let's cast this one and go out of this function for now
						if (GetCharInfo() && GetCharInfo()->pSpawn)
						{
							sprintf_s(modrod, "${If[${Me.Gem[%s]}>0 && ${Me.SpellReady[%s]},1,0]}", szTemp, szTemp);
							ParseMacroData(modrod, MAX_STRING);
							if (atoi(modrod) > 0)
								WriteChatf("\awCreating - \ao%s", ModRods[i].szItem);
							TargetIT(GetCharInfo()->pSpawn);
							CastSpell(GetCharInfo()->pSpawn, pSpell, "\ao", 1);
						}
						modrodTimer = MQGetTickCount64() + 5000LL;
						modrodUseTimer = MQGetTickCount64() + 5000LL;
						return;
						//}
						// It's not ready, let's skip this type for now
						//DebugWrite("%s: Spell not ready - skipping MASS for now.", CurrentRoutine);
						//break;
					}
				}
			}
		}
	}
	// ok modrod stuff is done, let's check other stuff
	if (ManaTimer > 0LL && ManaTimer > MQGetTickCount64())
	{
		//DebugWrite("%s: Too soon (MT)", CurrentRoutine);
		return;
	}
	// Check once per second
	ManaTimer = MQGetTickCount64() + 1000LL;
	if (!vMana.size() || !GetCharInfo())
		return;
	if (!GetCharInfo()->pSpawn)
		return;
	if (GetCharInfo()->pSpawn->ManaMax < 1)
		return;
	long manaPct = (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->ManaMax > 0) ? GetCharInfo()->pSpawn->ManaCurrent * 100 / GetCharInfo()->pSpawn->ManaMax : 0;
	long dbMana = 15000;
	long dbHP = 20000;
	long manaReq = (GetCharInfo() && GetCharInfo()->pSpawn) ? GetCharInfo()->pSpawn->ManaMax - GetCharInfo()->pSpawn->ManaCurrent : 0;
	int capAmt = vMana.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vMana[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
		{
			DebugWrite("%s: Too soon (ST)", CurrentRoutine);
			return;
		}
		DebugWrite("%s: Checking %d : '%s'", CurrentRoutine, i, vManaName[i].c_str());
		char sSpell[MAX_STRING];
		char testMana[MAX_STRING];
		char name[MAX_STRING]; //added this in for ease
		strcpy_s(name, vManaName[i].c_str()); //added this in for ease
		valid = true;
		sprintf_s(sSpell, "ManaIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (!valid)
		{
			DebugWrite("%s: Not valid (%d)", CurrentRoutine, i);
			return;
		}
		if (strstr(name, "Harvest") && PctMana(GetCharInfo()->pSpawn)<76)
		{

			sprintf_s(testMana, "${If[${Me.SpellReady[%s]},1,0]}", vMana[i]->Name);
			ParseMacroData(testMana, MAX_STRING);
			DebugWrite("%s: Harvest found, PMD Spell = %s", CurrentRoutine, testMana);
			if (atoi(testMana) == 1)
			{
				DebugWrite("%s: Casting '%s'", CurrentRoutine, vMana[i]->Name);
				if (GetCharInfo() && GetCharInfo()->pSpawn)
					CastSpell(GetCharInfo()->pSpawn, vMana[i], "\ao", 0);
				return;
			}
			sprintf_s(testMana, "${If[${Me.AltAbilityReady[%s]},1,0]}", name);
			ParseMacroData(testMana, MAX_STRING);
			DebugWrite("%s: Harvest found, PMD Alt = %s", CurrentRoutine, testMana);
			if (atoi(testMana) == 1)
			{
				DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
				WriteChatf("Activating: \ao%s", name);
				sprintf_s(sSpell, "/multiline ; /stopsong ; /casting \"%s\" alt", name);
				DoCommand(GetCharInfo()->pSpawn, sSpell);
				SpellTimer = MQGetTickCount64() + 1000LL;
				return;
			}
		}
		if (strstr(vMana[i]->Name, "Thaumatize") && manaReq > 750)
		{
			sprintf_s(testMana, "${If[${Me.SpellReady[%s]} && ${Me.Pet.PctHPs}>80 && ${Me.Pet.Distance}<150,1,0]}", vMana[i]->Name);
			ParseMacroData(testMana, MAX_STRING);
			DebugWrite("%s: Thaumatize found, PMD = %s", CurrentRoutine, testMana);
			if (atoi(testMana) == 1)
			{
				DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
				if (GetCharInfo() && GetCharInfo()->pSpawn)
					CastSpell(GetCharInfo()->pSpawn, vMana[i], "\ao", 0);
				return;
			}
		}
		if (strstr(vMana[i]->Name, " Symbiosis"))
		{
			sprintf_s(testMana, "${If[!${Bot.HaveBuff[${Spell[${Spell[%s].AutoCast}].Name}].ID} && ${Me.SpellReady[%s]} && ${Me.Pet.PctHPs}>80 && ${Me.Pet.Distance}<200,1,0]}", vMana[i]->Name, vMana[i]->Name);
			ParseMacroData(testMana, MAX_STRING);
			DebugWrite("%s: Symbiosis found, PMD = %s", CurrentRoutine, testMana);
			if (atoi(testMana) == 1)
			{
				DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
				if (GetCharInfo() && GetCharInfo()->pSpawn)
					CastSpell(GetCharInfo()->pSpawn, vMana[i], "\ao", 0);
				return;
			}
		}
		if ((vMana[i]->TargetType == 5 || vMana[i]->TargetType == 41) && UseManaOnOthers)
		{
			sprintf_s(testMana, "${If[${Me.AltAbilityReady[%s]},1,0]}", name); //had to change this vector due to mana names notmatching the AA names
			ParseMacroData(testMana, MAX_STRING);
			//DebugWrite("%s: %s, TargetType == 5 or 41, ParseMacroData = %s", CurrentRoutine, name, testMana);
			if (atoi(testMana) == 1)
			{
				for (unsigned int x = 0; x < 6; x++)
				{
					sprintf_s(testMana, "${If[${Select[${Group.Member[%d].Class.ShortName},BST,CLR,DRU,ENC,PAL,RNG,SHD,NEC,MAG,WIZ,SHM]} && ${Group.Member[%d].PctMana}<%d && ${Group.Member[%d].PctMana},${Group.Member[%d].ID},0]}", x, x, ManaAt, x, x);
					//DebugWrite("%s: %s, Group member = %d, PreParse = %s", CurrentRoutine, name, x, testMana);
					ParseMacroData(testMana, MAX_STRING);
					//DebugWrite("%s: %s, Group member = %d, ParseMacroData = %s", CurrentRoutine, name, x, testMana);
					if (atoi(testMana) > 0 && GetCharInfo() && GetCharInfo()->pSpawn)
					{
						PSPAWNINFO gMember = (PSPAWNINFO)GetSpawnByID(atoi(testMana));

						sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && (${Spell[${Me.AltAbility[%s].Spell}].Stacks}||!${Spell[${Me.AltAbility[%s].Spell}].Duration}) && !${Bot.HaveBuff[${Me.AltAbility[%s].Spell}].ID}  && !${Me.Song[${Me.AltAbility[%s].Spell}].ID},1,0]}", name, name, name, name, name);
						if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->mActorClient.Class == 15 && strstr(name, "Focused Paragon"))
							strcpy_s(sSpell, "${If[${Me.AltAbilityReady[Focused Paragon of Spirits]} && ${Spell[${Me.AltAbility[Focused Paragon of Spirits].Spell}].Stacks} && !${Me.Song[${Me.AltAbility[Paragon of Spirit].Spell}].ID} && && !${Me.Song[${Me.AltAbility[Focused Paragon of Spirits].Spell}].ID},2,0]}");
						ParseMacroData(sSpell, MAX_STRING);

						DebugWrite("%s: Casting '%s' on %s", CurrentRoutine, name, gMember->DisplayedName);
						if (atoi(sSpell) == 1 && GetCharInfo() && GetCharInfo()->pSpawn && gMember)
						{
							DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
							//if (vMana[i]->TargetType == 41 || vMana[i]->TargetType == 5)
							TargetIT(gMember);
							sprintf_s(sSpell, "/multiline ; /stopsong ; /casting \"%s\" alt", vManaName[i].c_str());
							DoCommand(NULL, sSpell);
							WriteChatf("%s -> %s", vManaName[i].c_str(), gMember->DisplayedName);
							SpellTimer = MQGetTickCount64() + 3000LL;
							//CastSpell(gMember, vMana[i], "\ao", 0);
							return;
						}
						if (atoi(sSpell) == 2 && GetCharInfo() && GetCharInfo()->pSpawn && gMember)
						{
							DebugWrite("%s: Casting '%s' on %s", CurrentRoutine, name, gMember->DisplayedName);
							TargetIT(gMember);
							strcpy_s(sSpell, "/multiline ; /stopsong ; /casting \"Focused Paragon of Spirits\" alt");
							DoCommand(NULL, sSpell);
							SpellTimer = MQGetTickCount64() + 3000LL;
							WriteChatf("\aoFocused Paragon of Spirits");
						}
					}
				}
			}
		}
		if (manaPct > ManaAt || InCombat && UseManaInCombat == 0 || !GetCharInfo() || !GetCharInfo()->pSpawn)
		{
			DebugWrite("%s: Returning, mana=%d>=20 or InCombat=%s and UseManaInCombat=%s or not valid char info",
				CurrentRoutine, manaPct, InCombat ? "true" : "false", UseManaInCombat ? "true" : "false");
			return;
		}
		int attrSlot = -1;
		if ((manaPct < 20 && strstr(name, "Mana Reserve")
			|| ((attrSlot = FindSpellAttrib(vMana[i], 15)) != -1) && (GetSpellBaseX(vMana[i], attrSlot) < manaReq)
			|| strstr(name, "Death Blo") && (((PCPLAYERWND)pPlayerWnd)->CombatState == 3 || ((PCPLAYERWND)pPlayerWnd)->CombatState == 4) && manaReq>dbMana && GetCharInfo()->pSpawn->HPCurrent>dbHP) && (vMana[i]->TargetType == 5 || vMana[i]->TargetType == 6 || vMana[i]->TargetType == 41))
		{
			DebugWrite("%s: In general check for %s", CurrentRoutine, name);
			if (SpellStacks(vMana[i]) || GetSpellDuration(vMana[i], (PSPAWNINFO)pLocalPlayer) < 1 || strstr(name, "Death Blo")) {
				DebugWrite("%s: In general subcheck for %s", CurrentRoutine, name);
				sprintf_s(sSpell, "${If[(${Spell[%s].Stacks}||!${Bot.SpellDuration[%s]}) && !${Bot.HaveBuff[%s].ID} && !${Me.Casting.ID} && ${Me.SpellReady[%s]},1,0]}", vMana[i]->Name, vMana[i]->Name, vMana[i]->Name, vMana[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				DebugWrite("%s: PMD1 = %s", CurrentRoutine, sSpell);
				if (atoi(sSpell) == 1 && GetCharInfo() && GetCharInfo()->pSpawn)
				{
					DebugWrite("%s: Casting '%s'", CurrentRoutine, vMana[i]->Name);
					if (vMana[i]->TargetType == 41 || vMana[i]->TargetType == 5)
						TargetIT(GetCharInfo()->pSpawn);
					CastSpell(GetCharInfo()->pSpawn, vMana[i], "\ao", 0);
					SpellTimer = MQGetTickCount64() + 3000LL;
					return;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && ((${Spell[${Me.AltAbility[%s].Spell}].Stacks}||${Me.AltAbility[%s].Spell.Name.Find[Death Blo]}) && !${Me.Song[Death Bloom].ID} && !${Me.Song[Death Blossom].ID}||!${Spell[${Me.AltAbility[%s].Spell}].Duration}) && !${Bot.HaveBuff[${Me.AltAbility[%s].Spell}].ID},1,0]}", name, name, name, name, name);
				if (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->mActorClient.Class == 15 && strstr(name, "Focused Paragon"))
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[Focused Paragon of Spirits]} && ${Spell[${Me.AltAbility[%s].Spell}].Stacks} && !${Bot.HaveBuff[${Me.AltAbility[Paragon of Spirit].Spell}].ID},2,0]}", name);
				ParseMacroData(sSpell, MAX_STRING);
				DebugWrite("%s: PMD2 = %s", CurrentRoutine, sSpell);
				if (atoi(sSpell) == 1 && GetCharInfo() && GetCharInfo()->pSpawn)
				{
					DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
					if (vMana[i]->TargetType == 41 || vMana[i]->TargetType == 5)
						TargetIT(GetCharInfo()->pSpawn);
					sprintf_s(sSpell, "/multiline ; /stopsong ; /casting \"%s\" alt", vManaName[i].c_str());
					DoCommand(NULL, sSpell);
					SpellTimer = MQGetTickCount64() + 3000LL;
					return;
				}
				if (atoi(sSpell) == 2 && GetCharInfo() && GetCharInfo()->pSpawn)
				{
					DebugWrite("%s: Casting '%s'", CurrentRoutine, name);
					TargetIT(GetCharInfo()->pSpawn);
					strcpy_s(sSpell, "/multiline ; /stopsong ; /casting \"Focused Paragon of Spirits\" alt");
					DoCommand(GetCharInfo()->pSpawn, sSpell);
					SpellTimer = MQGetTickCount64() + 3000LL;
					WriteChatf("\aoFocused Paragon of Spirits");
				}
				if ((GetCharInfo() && GetCharInfo()->pSpawn) && GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13)
				{
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[Forceful Rejuvenation]} && ${Me.GemTimer[%s].TotalSeconds}>360 && ${Me.PctMana}<%d,1,0]}", name, ManaAt);
					ParseMacroData(sSpell, MAX_STRING);
					DebugWrite("%s: PMD3 = %s", CurrentRoutine, sSpell);
					if (atoi(sSpell) == 1 && GetSpellByName("Forceful Rejuvenation"))
					{
						DebugWrite("%s: Casting Forceful Rejuvenation", CurrentRoutine);
						CastSpell(GetCharInfo()->pSpawn, GetSpellByName("Forceful Rejuvenation"), "\ao", 0);
					}
				}
			}
		}
	}
}

void CheckMed()
{
	if (!InGameOK())
		return;
	PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
	if (Medding && (PctMana(pSpawn) > 99 || PctMana(pSpawn) > MedStopAt && !MedToFull))
	{
		if (pSpawn->StandState == STANDSTATE_SIT)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Mana medding done, standing");
		}
		DoingFeignDeath = 0;
		Medding = false;

		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Mana at %d percent.  Finished medding.", PctMana(GetCharInfo()->pSpawn));
	}
	if (MQGetTickCount64() < MedTimer || !GetCharInfo()->pSpawn->ManaMax || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (vAdds.size() && !MedDuringCombat || GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT)
		return;
	if ((PctMana(pSpawn) < MedStartAt || Medding && PctMana(pSpawn) < MedStopAt || Medding && PctMana(pSpawn) < 98 && MedToFull) && pSpawn->StandState == STANDSTATE_STAND)
	{
		HideDoCommand(GetCharInfo()->pSpawn, "/sit on", FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Mana at %d percent.  Medding.", PctMana(GetCharInfo()->pSpawn));
		Medding = true;
		if (MedToFull)
			MedTimer = MQGetTickCount64() + 2000LL;
		else
			MedTimer = MQGetTickCount64() + 15000LL;
	}
}

void CheckMelee()
{
	if (!InGameOK())
		return;
	if (!vAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!vAdds[0]->HPCurrent || !GetCharInfo()->pSpawn || MeleeAt == 0 || MQGetTickCount64() < BardMezTimer)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (!*EQADDR_ATTACK && vAdds[0] && (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) <= MeleeAt && GetDistance(vAdds[0]->X, vAdds[0]->Y) < MeleeDistance && (UseAutoFire == 0 || GetDistance(vAdds[0]->X, vAdds[0]->Y) < MeleeDistance) && vAdds[0]->Type != SPAWN_CORPSE)
	{
		TargetIT(vAdds[0]);
		if (GetCharInfo()->pSpawn->StandState != STANDSTATE_STAND) // && GetCharInfo()->pSpawn->StandState!=STANDSTATE_FEIGN
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Standing to fight");
		}
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
		if (FindPlugins("MQ2Melee") && strstr(AttackCommand, "kill"))
			HideDoCommand(GetCharInfo()->pSpawn, "/killthis", FromPlugin);
		else
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /attack on", FromPlugin);
		if(DoPetSwarm)
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /pet swarm", FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Attacking: %s, Parameters: HP below: %d, Range: %d ", vAdds[0]->Name, MeleeAt, MeleeDistance);
	}
	if (!gAutoFire && vAdds[0] && (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) <= AutofireAt && GetDistance(vAdds[0]->X, vAdds[0]->Y) < AutofireDistance && GetDistance(vAdds[0]->X, vAdds[0]->Y) > 35 && UseAutoFire == 1 && vAdds[0]->Type != SPAWN_CORPSE)
	{
		TargetIT(vAdds[0]);
		if (GetCharInfo()->pSpawn->StandState != STANDSTATE_STAND && GetCharInfo()->pSpawn->StandState != STANDSTATE_FEIGN)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Standing to Autofire.");
		}
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
		HideDoCommand(GetCharInfo()->pSpawn, "/autofire on", FromPlugin);
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Autofire: %s, Parameters: HP below: %d, Range: %d ", vAdds[0]->Name, AutofireAt, AutofireDistance);
	}
}

void CheckMerc()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < mercTimer)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING];
	strcpy_s(test, "${If[${Me.Mercenary.State.Equal[ACTIVE]},1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1 && GetCharInfo()->pGroupInfo && GetCharInfo()->pGroupInfo->pMember[1] && GetCharInfo()->pGroupInfo->pMember[1]->pSpawn)
		strcpy_s(MercName, GetCharInfo()->pGroupInfo->pMember[1]->pSpawn->DisplayedName);
	if (atoi(test) == 0)
	{
		sprintf_s(test, "${If[${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]} && ${Me.Mercenary.State.Equal[DEAD]} && !${Group.Member[1].ID} && ${Group.Member[1].Name.Equal[%s]},1,0]}", MercName);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior: Unsuspending merc");
		}

	}
	mercTimer = MQGetTickCount64() + 10000LL;
}

void CheckMez()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() || !vMez.size())
		return;
	if (MQGetTickCount64() < SpellTimer || UseMez == 0 || vAdds.size() < 2 || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 && !BardClass)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", bSpell[MAX_STRING] = "";
	bool valid = false;
	for (unsigned int z = 1; z < vAdds.size(); z++)
	{
		if (strstr(MezImmuneList, vAdds[z]->DisplayedName) && ImmunityCheck)
			continue;
		bool SkipOne = false;
		for (unsigned int x = 0; x<vTimerSpawn.size(); x++)
		{
			SkipOne = false;
			if (vAdds[z] && vTimerSpawn[x])
				if (vTimerSpell[x]->Subcategory == 35 && vAdds[z] == vTimerSpawn[x] || GetDistance(GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Y, vAdds[z]->X, vAdds[z]->Y)>AssistRange)
				{
					SkipOne = true;
					break;
				}
		}
		if (SkipOne)
			continue;
		int capAmt = vMez.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vMez[i])
				continue;
			if (MQGetTickCount64() < SpellTimer)
				return;
			valid = ValidDet(vMez[i], vAdds[z]);
			sprintf_s(sSpell, "MezIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid && vAdds[z] && GetSpellMaxX(vMez[i], 0) >= vAdds[z]->Level)
			{
				if (!BardClass) {
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vMezName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && vAdds[z])
					{
						LastMezSingle = false;
						TargetIT(vAdds[z]);
						char test[MAX_STRING] = "";
						sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vMezName[i].c_str());
						ParseMacroData(test, MAX_STRING);
						ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vMez[i]);
						ULONGLONG recastTime = MQGetTickCount64() + test2;
						AddTimer(vAdds[z], vMez[i], recastTime);
						CastSpell(vAdds[z], vMez[i], "\ap", 0);
						if (vMez[i]->TargetType == 5)
							LastMezSingle = true;
						//AddTimer(vAdds[z],vMez[i],0);
					}
				}
				if (BardClass) {
					sprintf_s(sSpell, "${If[!${Me.GemTimer[%s]} && ${Me.Gem[%s]},1,0]}", vMezName[i].c_str(), vMezName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 1 && vAdds[z])
					{
						LastMezSingle = false;
						if (vMez[i]->TargetType == 5)
							TargetIT(vAdds[z]);
						sprintf_s(bSpell, "/squelch /bottwist once ${Me.Gem[%s]}", vMezName[i].c_str());
						ParseMacroData(bSpell, MAX_STRING);
						HideDoCommand(GetCharInfo()->pSpawn, bSpell, FromPlugin);
						WriteChatf("\a-m%s \aw --> %s", vMezName[i].c_str(), vAdds[z]->Name);
						SpellTimer = MQGetTickCount64() + 4000LL;
						BardMezTimer = MQGetTickCount64() + 750LL;
						if (vMez[i]->TargetType == 5)
							LastMezSingle = true;
						//AddTimer(vAdds[z],vMez[i],0);
						char test[MAX_STRING] = "";
						sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vMezName[i].c_str());
						ParseMacroData(test, MAX_STRING);
						ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test);
						ULONGLONG recastTime = MQGetTickCount64() + test2;
						AddTimer(vAdds[z], vMez[i], recastTime);
					}
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vMezName[i].c_str(), vMez[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastMezSingle = false;
					TargetIT(vAdds[z]);
					char test[MAX_STRING] = "";
					sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vMez[i]->Name);
					ParseMacroData(test, MAX_STRING);
					ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test);
					ULONGLONG recastTime = MQGetTickCount64() + test2;
					AddTimer(vAdds[z], vMez[i], recastTime);
					CastAA(vAdds[z], vMez[i], "\ap", vMezName[i].c_str());
					if (vMez[i]->TargetType == 5)
						LastMezSingle = true;
					/*if(BardClass)
					AddTimer(vAdds[z],vMez[i],0);*/
				}
			}
		}
	}
}

void CheckMisc()
{
	if (!InGameOK())
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (GetCharInfo()->pSpawn->mActorClient.Class == 11)
	{
		if (GetCharInfo()->pSpawn->HPCurrent < 5000)
		{
			for (unsigned long nBuff = 0; nBuff < 35; nBuff++)
			{
				if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID))
				{
					if (strstr(pSpell->Name, "Death Blo"))
					{
						char test[MAX_STRING];
						sprintf_s(test, "/nomodkey /notify ShortDurationBuffWindow Buff%d leftmouseup", nBuff);
						HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
					}
				}
			}
		}
	}
	if (!vAdds.size() && !vPossibleAdds.size() || !GetCharInfo()->pSpawn)
		return;
	if (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000 && !BardClass)
		return;
	CheckAlways();
	char test[MAX_STRING];
	if (vAdds.size())
	{
		if (vAdds[0] && vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax <= MercAssistAt && vAdds[0]->SpawnID != mercTgtID && GetDistance(vAdds[0]->X, vAdds[0]->Y) < MercAssistRange)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/mercassist", FromPlugin);
			mercTgtID = vAdds[0]->SpawnID;
		}
	}
	if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
	{
		if (GetDistance(pMyTarget->X, pMyTarget->Y) < 60 && MQGetTickCount64() > SpellTimer)
		{
			if (pMyTarget && pMyTarget->Type == SPAWN_NPC && !pMyTarget->MasterID && !pMyTarget->Mercenary)
			{
				sprintf_s(test, "${If[${Me.AltAbilityReady[Banestrike]} && !${String[%s].Find[|${Target.Race}|]} && ${Target.PctHPs}<%d,1,0]}", IgnoreBaneStrike, AssistAt);
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) == 1)
				{
					HideDoCommand(GetCharInfo()->pSpawn, "/alt act 15073", FromPlugin);
					WriteChatf("\arBanestrike\aw --> %s", pMyTarget->Name);
					SpellTimer = MQGetTickCount64() + 600LL;
				}
			}
		}
		if (pMyTarget && pMyTarget->Type == SPAWN_NPC)
		{
			Face(pMyTarget);
			if (facing > FaceAngle)
				HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face nolook", FromPlugin);
		}

	}

}

void CheckNukes()
{
	{
		if (!InGameOK())
			return;
		if (!vAdds.size() && !vPossibleAdds.size())
			return;
		if (!vAdds[0])
			return;
		if (MQGetTickCount64() < SpellTimer || !vAdds[0]->HPCurrent || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
			return;
		if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > NukeAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
			return;
		PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
		if (pSpawn->ManaCurrent)
			if (PctMana(pSpawn) < MinManaToNuke)
				return;
		CheckAlways();
		strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
		DebugWrite("%s: Checking routine", CurrentRoutine);
		char sSpell[MAX_STRING] = "", castTimerInfo[MAX_STRING] = "";
		bool valid = false;
		if (vDot.size())
			CheckDots();
		int capAmt = vNuke.size();
		for (int i = 0; i < capAmt; i++)
		{
			if (!vNuke[i])
				continue;
			if (MQGetTickCount64() < SpellTimer)
				return;
			valid = ValidDet(vNuke[i], vAdds[0]);
			sprintf_s(sSpell, "NukeIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if ((GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 6) && vNuke[i]->Subcategory == 111 && GetBodyType(vAdds[0]) != 28 && GetBodyType(vAdds[0]) != 24 && GetBodyType(vAdds[0]) != 27 && GetBodyType(vAdds[0]) != 5)
				valid = false;

			// 21 == stun, 64 == spin stun
			if (valid && (UseAENukes || vNuke[i]->AERange == 0) && vAdds[0]
				&& (!HasSpellAttrib(vNuke[i], 21) && !HasSpellAttrib(vNuke[i], 64)
					|| MQGetTickCount64() > StunTimer))
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vNuke[i]->Name, vNuke[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (strstr(vNuke[i]->Name, "of Many"))
				{
					sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && ${SpawnCount[pet loc ${Target.X} ${Target.Y} radius 30]}>5,1,0]}", vNuke[i]->Name);
					ParseMacroData(sSpell, MAX_STRING);
				}
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastSpell(vAdds[0], vNuke[i], "\ar", 0);

					int slot;
					if ((slot = FindSpellAttrib(vNuke[i], 21)) != -1)
						StunTimer = MQGetTickCount64() + (ULONGLONG)(GetSpellBase2(vNuke[i], slot) - 1000);
					if ((slot = FindSpellAttrib(vNuke[i], 64)) != -1)
						StunTimer = MQGetTickCount64() + (ULONGLONG)(GetSpellBase2(vNuke[i], slot) - 1000);
					break;
				}

				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vNukeName[i].c_str(), vNukeName[i].c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[0])
				{
					TargetIT(vAdds[0]);
					CastAA(vAdds[0], vNuke[i], "\ar", vNukeName[i].c_str());
					int slot;
					if ((slot = FindSpellAttrib(vNuke[i], 21)) != -1)
						StunTimer = MQGetTickCount64() + (ULONGLONG)(GetSpellBase2(vNuke[i], slot) - 1000);
					if ((slot = FindSpellAttrib(vNuke[i], 64)) != -1)
						StunTimer = MQGetTickCount64() + (ULONGLONG)(GetSpellBase2(vNuke[i], slot) - 1000);
					break;
				}
			}
		}
	}
}

void CheckNukeToT()
{
	if (!InGameOK())
		return;
	if (!pAggroInfo || MQGetTickCount64() < SpellTimer || !vAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (pAggroInfo)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)GetCharInfo()->pSpawn->TargetOfTarget))
		{
			if (PctHP(pSpawn) < NukeToTAt && pSpawn == vAdds[0])
			{
				bool valid = true;
				char sSpell[MAX_STRING] = "", cSpell[MAX_STRING] = "";
				int capAmt = vNukeToT.size();
				for (int i = 0; i < capAmt; i++)
				{
					if (!vNukeToT[i])
						continue;
					valid = true;
					sprintf_s(cSpell, "NukeToTIf%d", i);
					SpellIt = SpellIf.find(cSpell);
					if (SpellIt != SpellIf.end())
					{
						sprintf_s(cSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
						ParseMacroData(cSpell, MAX_STRING);
						if (atoi(cSpell) == 0)
							valid = false;
					}
					if (valid && ppTarget)
					{
						sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vNukeToT[i]->Name);
						ParseMacroData(sSpell, MAX_STRING);
						if (atoi(sSpell) == 1 && valid)
						{
							CastSpell((PSPAWNINFO)pTarget, vNukeToT[i], "\a-t", 0);
							break;
						}
					}
				}
			}
		}
	}

}

void CheckPet()
{
	if (!InGameOK())
		return;
	if (iUsePet == 0 && !CharmMob || !GetCharInfo()->pSpawn || !petSpell)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (!CharmMob || CharmMob->Type == SPAWN_CORPSE)
		CharmMob = NULL;
	PCHARINFO pChar = GetCharInfo();
	if (iUsePet != 0 && petSpell && pChar->pSpawn->PetID && pChar->pSpawn->PetID <= 0 && !*EQADDR_ATTACK && MQGetTickCount64() > SpellTimer)
	{
		DebugWrite("I need to summon a pet with: %s", petSpell->Name);
		char test[MAX_STRING];
		sprintf_s(test, "${If[(!${Spell[%s].ReagentID[1]}||${Bot.FindItem[${Spell[%s].ReagentID[1]}]}),1,0]}", petSpell->Name, petSpell->Name);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1 || !PetReagentCheck)
		{
			DebugWrite("Summoning a pet with: %s", petSpell->Name);
			if (GetCharInfo()->pSpawn->ManaCurrent > (int)petSpell->ManaCost)
				CastSpell(GetCharInfo()->pSpawn, petSpell, "\a-t", 1);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior:  Summoning pet with: %s", petSpell->Name);
		}
	}
	if (pChar->pSpawn->PetID && pChar->pSpawn->PetID == 0xFFFFFFFF && !CharmMob)
		return;
	if (!CharmMob || CharmMob->Type == SPAWN_CORPSE)
		CharmMob = NULL;
	//DebugWrite("%s: Checking if pet needs to attack", CurrentRoutine);
	if (vAdds.size() && vAdds[0])
	{
		char PetAttack[MAX_STRING];
		sprintf_s(PetAttack, "${If[!${Target.Mezzed.ID} && ${Me.Pet.ID} && !${Me.Pet.Following.ID} && ${Spawn[%s].ID} && ${Spawn[%s].Distance}<%d && ${Spawn[%s].PctHPs}<=%d && ${Spawn[%s].Type.NotEqual[corpse]},1,0]}", vAdds[0]->Name, vAdds[0]->Name, PetAttackRange, vAdds[0]->Name, PetAttackAt, vAdds[0]->Name);
		ParseMacroData(PetAttack, MAX_STRING);
		if (atoi(PetAttack) == 1 && vAdds[0]->Type != SPAWN_CORPSE)
		{
			//TargetIT(vAdds[0]);
			if (StandOnPetAttack && GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT)
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stand on", FromPlugin);
				if (AnnounceBehaviorChange)
					WriteChatf("Behavior: Standing to /pet attack.");
			}
			sprintf_s(PetAttack, "/pet attack %d", vAdds[0]->SpawnID);
			HideDoCommand(GetCharInfo()->pSpawn, PetAttack, FromPlugin);
			if(DoPetSwarm)
			{
				sprintf_s(PetAttack, "/pet swarm");
				HideDoCommand(GetCharInfo()->pSpawn, PetAttack, FromPlugin);
			}
			if (AnnounceEcho && PetAttackTimer < MQGetTickCount64())
			{
				WriteChatf("Pet Attacking --> %s", vAdds[0]->Name);
				PetAttackTimer = MQGetTickCount64() + 2000LL;
			}
		}
	}
	if (MQGetTickCount64() < SpellTimer || UsePetBuffs == 0 || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	char sSpell[MAX_STRING] = "", sTest[MAX_STRING] = "", sCommand[MAX_STRING] = "";
	//DebugWrite("%s: Checking pet buffs", CurrentRoutine);
	bool valid = true;
	int capAmt = vPetBuff.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vPetBuff[i])
			continue;
		if (pChar->pSpawn->PetID && pChar->pSpawn->PetID == 0xFFFFFFFF && !CharmMob)
			return;
		sprintf_s(sSpell, "PetBuffIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (valid)
		{

			sprintf_s(sSpell, "${If[${Me.Book[%s]} && ${Me.Pet.ID} && !${Me.AltAbility[%s]} && ${Spell[%s].StacksPet} && !${Me.Pet.Buff[%s]} && ${Me.CurrentMana}>${Spell[%s].Mana} && !${Me.Casting.ID},1,0]}", vPetBuffName[i].c_str(), vPetBuffName[i].c_str(), vPetBuff[i]->Name, vPetBuff[i]->Name, vPetBuff[i]->Name);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				if (vPetBuff[i]->TargetType == 5)
				{
					// strcpy_s(sCommand, "/squelch /target ${Me.Pet}");
					// ParseMacroData(sCommand);
					// DoCommand(GetCharInfo()->pSpawn, sCommand);
					TargetIT((PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID));
				}
				CastSpell((PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID), vPetBuff[i], "\a-t", 1);
				break;
			}
			sprintf_s(sSpell, "${If[${Me.Pet.ID} && ${Me.AltAbilityReady[%s]} && (${Me.CombatState.NotEqual[COMBAT]}||${Me.AltAbility[%s].Name.NotEqual[Aegis of Kildrukaun]})&& ${Spell[${Me.AltAbility[%s].Spell}].StacksPet} && !${Me.Pet.Buff[${Me.AltAbility[%s].Spell}]},1,0]}", vPetBuffName[i].c_str(), vPetBuffName[i].c_str(), vPetBuffName[i].c_str(), vPetBuffName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				if (vPetBuff[i]->TargetType == 5)
				{
					TargetIT((PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID));
				}
				CastAA((PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID), vPetBuff[i], "\a-t", vPetBuffName[i].c_str());
				break;
			}
		}
	}
	if (PetIllusionSpell)
	{
		char test[MAX_STRING];
		sprintf_s(test, "${If[${Spell[%s].StacksPet} && !${Me.Pet.Buff[%s]} && ${Cast.Ready[%s]} && ${Me.Pet.Distance}<${Spell[%s].Range},1,0]}", PetIllusionSpell->Name, PetIllusionSpell->Name, PetIllusionItem, PetIllusionSpell->Name);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			sprintf_s(test, "/casting \"%s\" item", PetIllusionItem);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			SpellTimer = MQGetTickCount64() + (ULONGLONG)((FLOAT)PetIllusionSpell->CastTime / 1000.0) + 400LL;
			WriteChatf("\a-t%s \aw--> Pet", PetIllusionSpell->Name);
		}

	}
}

void CheckPulling()				// CheckPulling -> set MacroPulling if  ${Pulling} || ${Looting} || ${MQ2BotPause} || ${AdvPath.Playing} || ${MoveTo.Moving} 
{
	MacroPulling = false;
	char test[MAX_STRING];

	if (FindMQ2DataVariable("Pulling")) {
		strcpy_s(test, "${Pulling}");
		ParseMacroData(test, MAX_STRING);
		MacroPulling = atoi(test);
	}
	if (!MacroPulling && FindMQ2DataVariable("Looting")) {
		strcpy_s(test, "${Looting}");
		ParseMacroData(test, MAX_STRING);
		MacroPulling = atoi(test);
	}
	if (!MacroPulling && FindMQ2DataVariable("MQ2BotPause")) {
		strcpy_s(test, "${MQ2BotPause}");
		ParseMacroData(test, MAX_STRING);
		MacroPulling = atoi(test);
	}
	if (!MacroPulling && FindMQ2DataType("AdvPath")) {
		strcpy_s(test, "${AdvPath.Playing}");
		ParseMacroData(test, MAX_STRING);
		MacroPulling = atoi(test);
	}
	if (!MacroPulling && FindMQ2DataType("MoveTo")) {
		strcpy_s(test, "${MoveTo.Moving}");
		ParseMacroData(test, MAX_STRING);
		MacroPulling = atoi(test);
	}

	if (MacroPulling)
	{
		if (AnnounceBehaviorChange)
			WriteChatf("Behavior: Pulling via macro, pausing other bot functions");
	}
}

void CheckRez()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || RezRadius == 0 || !GetCharInfo()->pSpawn || !vRez.size() || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char rezCheck[MAX_STRING], rezName[MAX_STRING], sSpell[MAX_STRING];
	bool valid;
	for (unsigned int i = 0; i < 6 && i < vRezTimeStamp.size(); i++)
	{
		if (MQGetTickCount64() < vRezTimeStamp[i])
			continue;
		sprintf_s(rezCheck, "${If[${Spawn[${Group.Member[%d]} corpse radius %d].ID} && !${Group.Member[%d].Linkdead} && !${Spawn[${Group.Member[%d]} corpse radius %d].Name.Find[_pet]},1,0]}", i, RezRadius, i, i, RezRadius);
		ParseMacroData(rezCheck, MAX_STRING);
		if (atoi(rezCheck) == 1)
		{
			DebugWrite("DEBUG: I found a dead group member");
			int capAmt = vRez.size();
			for (int x = 0; x < capAmt; x++)
			{
				if (!vRez[x])
					continue;
				sprintf_s(sSpell, "RezIf%d", i);
				valid = true;
				SpellIt = SpellIf.find(sSpell);
				if (SpellIt != SpellIf.end())
				{
					sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 0)
						valid = false;
				}
				sprintf_s(rezCheck, "${If[(${Me.AltAbilityReady[%s]}||${Me.SpellReady[%s]}) && (${Bot.FindItem[${Spell[%s].ReagentID[1]}]}||${Spell[%s].ReagentID[1]}<1) && (${Select[${Me.CombatState},RESTING,ACTIVE]}||!${Select[${Me.Class.ShortName},PAL,DRU,SHM]}||!${SpawnCount[group pc cleric]}),1,0]}", vRezName[x].c_str(), vRezName[x].c_str(), vRezName[x].c_str(), vRezName[x].c_str());
				ParseMacroData(rezCheck, MAX_STRING);
				if (valid && atoi(rezCheck) == 1)
				{
					DebugWrite("DEBUG: I should rez");
					sprintf_s(rezName, "${Spawn[${Group.Member[%d]} corpse radius %d].ID}", i, RezRadius);
					ParseMacroData(rezName, MAX_STRING);
					if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID((DWORD)atol(rezName)))
					{
						DebugWrite("DEBUG: I am rezzing");
						sprintf_s(rezName, "/squelch /tar ${Group.Member[%d]} corpse", i);
						ParseMacroData(rezName, MAX_STRING);
						//TargetIT(pSpawn);
						HideDoCommand(GetCharInfo()->pSpawn, rezName, FromPlugin);
						if (pSpawn && pSpawn->Type == SPAWN_CORPSE)
							CastSpell(pSpawn, vRez[x], "\ao", 0);
						HideDoCommand(GetCharInfo()->pSpawn, "/corpse", FromPlugin);
						RezTimer = MQGetTickCount64() + 6000LL;
						if (pSpawn && pSpawn->Type == SPAWN_CORPSE)
							vRezTimeStamp[i] = MQGetTickCount64() + 60000LL;
						return;
					}
				}
			}
		}
	}
}

void CheckRoot()
{
	if (!InGameOK())
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (MQGetTickCount64() < SpellTimer || !RootAdds && !RootPrimary || !vRoot.size() || !GetCharInfo()->pSpawn || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	int adds = vAdds.size();
	if (adds > 0 && RootAdds == 0)
		adds = 1;
	int capAmt = vRoot.size();
	for (int z = 0; z<adds; z++)
	{
		if (!vAdds[z])
			return;
		if (GetDistance(vAdds[z]->X, vAdds[z]->Y)>RootPrimary && z == 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) > RootAdds && z > 0 || GetDistance(vAdds[z]->X, vAdds[z]->Y) > AssistRange)
			continue;
		bool SkipOne = false;
		for (int i = 0; i < capAmt; i++)
		{
			if (!vRoot[i])
				continue;
			SkipOne = false;
			if (MQGetTickCount64() < SpellTimer)
				return;
			for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
			{
				if (vAdds[z] && vTimerSpawn[x])
					if (vRoot[i] == vTimerSpell[x] && vAdds[z] == vTimerSpawn[x])
					{
						SkipOne = true;
						break;
					}
			}
			if (SkipOne)
				continue;
			valid = ValidDet(vRoot[i], vAdds[z]);
			sprintf_s(sSpell, "RootIf%d", i);
			SpellIt = SpellIf.find(sSpell);
			if (SpellIt != SpellIf.end())
			{
				sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 0)
					valid = false;
			}
			if (valid)
			{
				sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vRootName[i].c_str(), vRoot[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					char test[MAX_STRING] = "";
					sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vRootName[i].c_str());
					ParseMacroData(test, MAX_STRING);
					ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vRoot[i]);
					ULONGLONG recastTime = MQGetTickCount64() + test2;
					AddTimer(vAdds[z], vRoot[i], recastTime);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastSpell(vAdds[z], vRoot[i], "\a-g", 0);
					if (vRoot[i]->TargetType == 5)
						LastSnareSingle = true;
				}
				sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Target.Buff[%s].ID},1,0]}", vRootName[i].c_str(), vRoot[i]->Name);
				ParseMacroData(sSpell, MAX_STRING);
				if (atoi(sSpell) == 1 && vAdds[z])
				{
					LastSnareSingle = false;
					TargetIT(vAdds[z]);
					char test[MAX_STRING] = "";
					sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vRoot[i]->Name);
					ParseMacroData(test, MAX_STRING);
					ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vRoot[i]);
					ULONGLONG recastTime = MQGetTickCount64() + test2;
					AddTimer(vAdds[z], vRoot[i], recastTime);
					HideDoCommand(GetCharInfo()->pSpawn, "/squelch /face fast nolook", FromPlugin);
					CastAA(vAdds[z], vRoot[i], "\a-g", vRootName[i].c_str());
					if (vRoot[i]->TargetType == 5)
						LastSnareSingle = true;
				}
			}
		}
	}
}

void CheckSelfBuffs()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SelfBuffTimer || MQGetTickCount64() < SpellTimer || UseSelfBuffs == 0 || vAdds.size() || !GetCharInfo()->pSpawn || !vSelfBuff.size() || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "", sSpell2[MAX_STRING] = "";
	int capAmt = vSelfBuff.size();
	bool valid = true;
	for (int i = 0; i < capAmt; i++)
	{
		if (!vSelfBuff[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
			return;
		valid = true;
		if (SpellStacks(vSelfBuff[i])) {
			sprintf_s(sSpell, "${If[${Me.FreeBuffSlots}>0 && (!${Me.AltAbility[%s]}||${Me.AltAbilityReady[%s]}) && !${Me.Song[%s].ID} && !${Bot.HaveBuff[%s].ID} && ${Me.CurrentMana}>${Spell[%s].Mana} && !${Me.Casting.ID},1,0]}", vSelfBuffName[i].c_str(), vSelfBuffName[i].c_str(), vSelfBuff[i]->Name, vSelfBuff[i]->Name, vSelfBuff[i]->Name);
			if (GetCharInfo()->pSpawn->mActorClient.Class == 11)
				if (GetSpellAttribX(vSelfBuff[i], 5) == 15)
					sprintf_s(sSpell, "${If[${Me.FreeBuffSlots}>0 && (!${Me.AltAbility[%s]}||${Me.AltAbilityReady[%s]}) && !${Me.Song[%s].ID} && !${Bot.HaveBuff[%s].ID} && ${Me.CurrentMana}>${Spell[%s].Mana} && !${Me.Casting.ID} && !${Bot.HaveBuff[Death Blossom].ID} && !${Bot.HaveBuff[Death Bloom].ID},1,0]}", vSelfBuff[i]->Name, vSelfBuffName[i].c_str(), vSelfBuff[i]->Name, vSelfBuff[i]->Name, vSelfBuff[i]->Name);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				sprintf_s(sSpell2, "SelfBuffIf%d", i);
				SpellIt = SpellIf.find(sSpell2);
				if (SpellIt != SpellIf.end())
				{
					sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (atoi(sSpell) == 0)
					{
						valid = false;
					}
				}
				if (valid)
				{
					sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]},1,0]}", vSelfBuffName[i].c_str());
					ParseMacroData(sSpell, MAX_STRING);
					if (vSelfBuff[i]->TargetType == 5)
						HideDoCommand(GetCharInfo()->pSpawn, "/squelch /tar myself", FromPlugin);
					if (atoi(sSpell) == 1)
					{
						CastAA(GetCharInfo()->pSpawn, vSelfBuff[i], "\aw", vSelfBuffName[i].c_str());
					}
					else
					{
						CastSpell(GetCharInfo()->pSpawn, vSelfBuff[i], "\aw", 1);
					}
					SelfBuffTimer = MQGetTickCount64() + (ULONGLONG)((FLOAT)vSelfBuff[i]->RecastTime / 1000.0) + (ULONGLONG)((FLOAT)vSelfBuff[i]->RecoveryTime / 1000.0) + 1000LL;
				}
			}
		}
	}
}

void CheckSnare()
{
	if (!InGameOK())
		return;
	if (!vSnare.size() || !vAdds.size() || (!vAdds.size() && !vPossibleAdds.size()))
		return;
	if (MQGetTickCount64() < SpellTimer || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000 && !BardClass))
		return;
	if ((vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > SnareAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange || vAdds[0]->Type == SPAWN_CORPSE)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = { 0 };
	bool valid = false, SkipOne = false;
	if (strstr(SnareImmuneList, vAdds[0]->DisplayedName) && ImmunityCheck)
		return;
	int capAmt = vSnare.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vSnare[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
			return;
		SkipOne = false;
		for (unsigned int x = 0; x < vTimerSpawn.size(); x++)
		{
			if (vAdds[0] && vTimerSpawn[x])
			{
				if (vSnare[i] == vTimerSpell[x] && vAdds[0] == vTimerSpawn[x])
				{
					SkipOne = true;
					break;
				}
			}
		}
		if (SkipOne)
			continue;
		valid = ValidDet(vSnare[i], vAdds[0]);
		sprintf_s(sSpell, "SnareIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (valid && vAdds[0] && vAdds[0]->Type == SPAWN_NPC && PctHP(vAdds[0])>0)
		{
			sprintf_s(sSpell, "${If[${Me.SpellReady[%s]} && !${Bot.Snared},1,0]}", vSnare[i]->Name);
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1)
			{
				LastSnareSingle = false;
				TargetIT(vAdds[0]);
				char test[MAX_STRING] = "";
				sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vSnare[i]->Name);
				ParseMacroData(test, MAX_STRING);
				ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vSnare[i]);
				ULONGLONG recastTime = MQGetTickCount64() + test2;
				AddTimer(vAdds[0], vSnare[i], recastTime);
				CastSpell(vAdds[0], vSnare[i], "\a-g", 0);
				if (vSnare[i]->TargetType == 5)
					LastSnareSingle = true;
			}
			sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && !${Bot.Snared},1,0]}", vSnareName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1 && vAdds[0])
			{
				LastSnareSingle = false;
				TargetIT(vAdds[0]);
				char test[MAX_STRING] = "";
				sprintf_s(test, "${Bot.SpellDuration[%s].TotalSeconds}", vSnare[i]->Name);
				ParseMacroData(test, MAX_STRING);
				ULONGLONG test2 = 1000LL * (ULONGLONG)atoi(test)*(ULONGLONG)CalcDuration(vSnare[i]);
				ULONGLONG recastTime = MQGetTickCount64() + test2;
				AddTimer(vAdds[0], vSnare[i], recastTime);
				if (vSnare[i]->TargetType == 5)
					LastSnareSingle = true;
				CastAA(vAdds[0], vSnare[i], "\a-g", vSnareName[i].c_str());
			}
		}
	}
}

void CheckStick()
{
	if (!InGameOK())
		return;
	if (vAdds.size() || !GetCharInfo()->pSpawn || !AutoFollowLeader || !GetCharInfo()->pGroupInfo || GetCharInfo()->pSpawn->RespawnTimer)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING];
	strcpy_s(test, "${If[${Stick.StickTarget}==${Group.Puller.ID} && ${Group.Puller.ID} && ${Plugin[mq2moveutils].Name.Equal[mq2moveutils]},1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
		HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
	strcpy_s(test, "${If[!${Stick.Distance} && ${Group.Leader.ID} && ${Plugin[mq2moveutils].Name.Equal[mq2moveutils]},1,0]}");
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		strcpy_s(test, "${If[${Group.Puller.ID}!=${Group.Leader.ID} && ${Group.Leader.ID}!=${Me.ID},1,0]}");
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			sprintf_s(test, "/squelch /stick id ${Group.Leader.ID} %s", AutoFollowSettings);
			ParseMacroData(test, MAX_STRING);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			return;
		}
		if (atoi(test) == 0)
		{
			for (unsigned int i = 1; i < 6; i++)
			{
				sprintf_s(test, "${If[${Group.Puller.ID}!=${Group.Member[%d].ID} && ${Group.Member[%d].ID},1,0]}", i, i);
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) == 1)
				{
					sprintf_s(test, "/squelch /stick id ${Group.Member[%d].ID} %s", i, AutoFollowSettings);
					ParseMacroData(test, MAX_STRING);
					HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
					return;
				}
			}

		}
	}
}

void CheckSummonItems()
{
	if (!InGameOK())
		return;
	PSPAWNINFO pLocalSpawn = GetCharInfo()->pSpawn;
	if (!pLocalSpawn)
		return;
	if (InCombat || !pLocalSpawn->ManaMax || !vSummonItems.size() || MQGetTickCount64() < SummonTimer || MQGetTickCount64() < SpellTimer)
		return;
	if ((pLocalSpawn->CastingData.SpellETA - pLocalSpawn->TimeStamp > 0 && pLocalSpawn->CastingData.SpellETA - pLocalSpawn->TimeStamp < 30000) && !BardClass)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	if (cauldron)
	{
		char summonName[MAX_STRING] = { 0 };
		for (unsigned int i = 0; szCauldron[i]; i++)
		{
			sprintf_s(summonName, "${If[${Cast.Ready[%s]} && ${Me.FreeInventory}>0,1,0]}", szCauldron[i]);
			ParseMacroData(summonName, MAX_STRING);
			if (atoi(summonName) == 1)
			{
				sprintf_s(summonName, "/multiline ; /stopcast ; /casting \"%s\" item", szCauldron[i]);
				WriteChatf("\awClicking --> \a-o%s", szCauldron[i]);
				HideDoCommand(GetCharInfo()->pSpawn, summonName, FromPlugin);
				SpellTimer = MQGetTickCount64() + 11000LL;
			}
		}
	}
	if ((pLocalSpawn->ManaCurrent * 100 / pLocalSpawn->ManaMax) <= (int)ManaToSummon)
		return;

	PSPELL summonSpell = NULL;
	char summonName[MAX_STRING] = { 0 };
	int capAmt = vSummonItems.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vSummonItems[i])
			continue;
		sprintf_s(summonName, "%ld", GetSpellBaseX(vSummonItems[i], 0));
		CItemLocation cFindItem;
		if (!ItemFind(&cFindItem, summonName))
		{
			PCONTENTS find = cFindItem.pBagSlot;
			if (!find)
			{

				sprintf_s(summonName, "${If[!${Cursor.ID} && ${Me.FreeInventory}>0 && (${Me.Book[%s]}||${Me.AltAbilityReady[%s]}||${Cast.Ready[%s]}),1,0]}", vSummonItems[i]->Name, vSummonItems[i]->Name, vSummonItems[i]->Name);
				if (strstr(vSummonItems[i]->Name, "Paradox"))
					sprintf_s(summonName, "${If[!${FindItem[Ether-Fused Shard].ItemSlot} && !${Cursor.ID} && ${Me.FreeInventory}>0 && (${Me.Book[%s]}||${Me.AltAbilityReady[%s]}||${Cast.Ready[%s]}),1,0]}", vSummonItems[i]->Name, vSummonItems[i]->Name, vSummonItems[i]->Name);
				ParseMacroData(summonName, MAX_STRING);
				if (atoi(summonName) == 1)
				{

					if (vSummonItems[i]->TargetType == 5)
						TargetIT(pLocalSpawn);
					if (vSummonItems[i]->TargetType == 2)
					{
						if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget)
						{
							CastSpell(pLocalSpawn, vSummonItems[i], "\ao", 1);
							return;
						}
						else
						{
							TargetIT(pLocalSpawn);
						}
					}
					CastSpell(pLocalSpawn, vSummonItems[i], "\ao", 1);
					SummonTimer = MQGetTickCount64() + (ULONGLONG)((FLOAT)vSummonItems[i]->RecastTime / 1000.0) + (ULONGLONG)((FLOAT)vSummonItems[i]->RecoveryTime / 1000.0) + 1000LL;
					return;
				}
			}
		}
	}
}

void CheckSwarm()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer)
		return;
	if (!vAdds.size() && !vPossibleAdds.size())
		return;
	if (!vAdds[0])
		return;
	if (!vAdds[0]->HPCurrent)
		return;
	if ((GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) > 0 && (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp) < 30000 && !BardClass)
		return;
	CheckAlways();
	if (vAdds[0] && (vAdds[0]->HPCurrent * 100 / vAdds[0]->HPMax) > SwarmAt || GetDistance(vAdds[0]->X, vAdds[0]->Y) > AssistRange)
		return;
	PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
	if (pSpawn && pSpawn->ManaCurrent)
		if (PctMana(pSpawn) < MinManaToNuke)
			return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char sSpell[MAX_STRING] = "";
	bool valid = false;
	int capAmt = vSwarm.size();
	for (int i = 0; i < capAmt; i++)
	{
		if (!vSwarm[i])
			continue;
		if (MQGetTickCount64() < SpellTimer)
			return;
		valid = ValidDet(vSwarm[i], vAdds[0]);
		if (vSwarm[i]->Range == 0 && vSwarm[i]->AERange == 0)
			valid = true;
		sprintf_s(sSpell, "SwarmIf%d", i);
		SpellIt = SpellIf.find(sSpell);
		if (SpellIt != SpellIf.end())
		{
			sprintf_s(sSpell, "${If[(%s),1,0]}", SpellIt->second.c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 0)
				valid = false;
		}
		if (valid && vAdds[0] && vSwarm[i])
		{
			sprintf_s(sSpell, "${If[${Me.SpellReady[%s]},1,0]}", vSwarmName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1 && vAdds[0])
			{
				TargetIT(vAdds[0]);
				CastSpell(vAdds[0], vSwarm[i], "\a-r", 0);
				break;
			}
			sprintf_s(sSpell, "${If[${Me.AltAbilityReady[%s]} && (${SpawnCount[corpse radius 50]}>3||!${Me.AltAbility[%s].Name.Find[the dead]}),1,0]}", vSwarmName[i].c_str(), vSwarmName[i].c_str());
			ParseMacroData(sSpell, MAX_STRING);
			if (atoi(sSpell) == 1 && vAdds[0])
			{
				TargetIT(vAdds[0]);
				CastAA(vAdds[0], vSwarm[i], "\ar", vSwarmName[i].c_str());
				break;
			}
		}
	}
}

void CheckTarget()
{
	if (!InGameOK())
		return;
	if (!vPossibleAdds.size() && !vAdds.size())
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	Test();
	if (PSPAWNINFO pSpawn = GetKillTarget())
	{
		//WriteChatColor("Check",USERCOLOR_DEFAULT);
		char targetCheck[MAX_STRING];
		if (gTargetbuffs && pTarget)
		{
			if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
			{
				SpawnTrackerIt = SpawnTracker.find((int)pSpawn->SpawnID);
				if (SpawnTrackerIt == SpawnTracker.end())
				{
					SpawnTrack(pSpawn);
				}
				else
				{
					if (SpawnTrackerIt->second.LastChecked<MQGetTickCount64())
						SpawnTrackUpdate(pSpawn, 3);
				}
			}
		}
		sprintf_s(targetCheck, "${If[${Target.ID} && ${Target.ID}==%d,1,0]}", pSpawn->SpawnID);
		ParseMacroData(targetCheck, MAX_STRING);
		if (atoi(targetCheck) == 1 || pSpawn->Type == SPAWN_CORPSE)
			return;
		if (atoi(targetCheck) == 0 && pSpawn)
		{
			TargetIT(pSpawn);
		}
	}
}

void CheckTwist()
{
	if (!InGameOK())
		return;
	if (GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT || MQGetTickCount64() < TwistTimer || MQGetTickCount64() < SpellTimer)
		return;
	if ((GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000) && !BardClass)
		return;
	if ((GetCharInfo()->pSpawn->CastingData.SpellSlot == 255 ? 0 : GetCharInfo()->pSpawn->CastingData.SpellSlot + 1) == 13)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char song[MAX_STRING];
	//if (songNext > 0x0F)
	if (songNext >= NUM_SPELL_GEMS)
		songNext = 0;
	if ((size_t)songCurrent >= vTwistSpell.size())
	{
		songCurrent = 0;
		songNext = 1;
	}
	if (GetCharInfo2()->MemorizedSpells[songNext] != 0xFFFFFFFF || (int)vTwistSpell[songCurrent]->ManaCost < GetCharInfo()->pSpawn->ManaCurrent) //check the < vs >
	{
		if (GetSpellGemTimer(songNext))
		{
			if ((size_t)songNext + 1 >= vTwist.size())
				songNext = 0;
			else
				songNext++;
			return;
		}
		else
		{
			//if(vTwistSpell[songCurrent]->Mana==0)
			//if((GetCharInfo()->pSpawn->CastingData.SpellSlot==255?0:GetCharInfo()->pSpawn->CastingData.SpellSlot+1)>0)
			HideDoCommand(GetCharInfo()->pSpawn, "/stopsong", FromPlugin);
			sprintf_s(song, "/cast %d", vTwist[songNext]);
			HideDoCommand(GetCharInfo()->pSpawn, song, FromPlugin);
			sprintf_s(song, "${Me.Gem[%d].MyCastTime.Float}", vTwist[songNext]);
			ParseMacroData(song, MAX_STRING);
			TwistTimer = MQGetTickCount64() + (ULONGLONG)(atof(song) * 1000) + 200LL;
			songPrev = songCurrent;
			songCurrent = songNext;
			if ((size_t)songNext + 1 >= vTwist.size())
				songNext = 0;
			else
				songNext++;
			//WriteChatf("P: %s - C: %s - N: %s",vTwistSpell[songPrev]->Name,vTwistSpell[songCurrent]->Name,vTwistSpell[songNext]->Name);
		}
	}
	else
	{
		if ((size_t)songNext + 1 >= vTwist.size())
			songNext = 0;
		else
			songNext++;
	}

}

void CheckAlways()
{
	CheckTimer();
	CheckCharm();
	CheckMez();
	CheckAggro();
	if (vDisc.size())
		CheckDiscs();
	CheckRoot();
	CheckHeals();
	CheckDebuffs();
	if (vHealToT.size())
		CheckHealToT();
}

void CheckAutoInv()
{
	char test[MAX_STRING];
	for (unsigned int i = 0; szAutoInv[i]; i++)
	{
		sprintf_s(test, "${If[${Cursor.Name.Equal[%s]} && ${Me.FreeInventory},1,0]}", szAutoInv[i]);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/autoinventory", FromPlugin);
			if (AnnounceBehaviorChange)
				WriteChatf("Behavior:  /autoinv: %s", szAutoInv[i]);
		}
	}
}

void CheckSafety()
{
	if (!InGameOK())
		return;
	if (zoning)
		return;
	strcpy_s(CurrentRoutine, &(__FUNCTION__[5]));
	DebugWrite("%s: Checking routine", CurrentRoutine);
	char test[MAX_STRING];
	//check tells
	if (EQADDR_LASTTELL[0])
	{
		strcpy_s(test, "${EverQuest.LastTell}");
		ParseMacroData(test, MAX_STRING);
		if (!strstr(LastTell, test))
		{
			WriteChatf("\arWARNING!! \ayTell event detected!");
			strcpy_s(LastTell, test);
			if (strlen(SafetyCommandOnTell))
			{
				strcpy_s(test, SafetyCommandOnTell);
				ParseMacroData(test, MAX_STRING);
				HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			}
			ShouldIExit = true;
		}
	}
	if (MQGetTickCount64() < WarpTimer)
		return;
	//check for GM summons
	GroupFarthest(9000);
	if (gCotH)
	{
		if (GetDistance(gCotH->X, gCotH->Y) > (WarpDistance + 200) && WarpDistance)
		{
			if (PauseOnWarp)
			{
				PausedWarp = true;
				WriteChatf("\arWARNING!! \ayWarp event detected on %s!", gCotH->Name);
				WarpTimer = MQGetTickCount64() + 30000LL;
				ShouldIExit = true;
				if (strlen(SafetyCommandOnOthersWarp))
				{
					strcpy_s(test, SafetyCommandOnOthersWarp);
					ParseMacroData(test, MAX_STRING);
					HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
				}
			}
		}
		WarpDistance = GetDistance(gCotH->X, gCotH->Y);
	}
	if (GetDistance(GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Y, WarpX, WarpY) > 200)
	{
		if (PauseOnWarp)
		{
			PausedWarp = true;
			WriteChatf("\arWARNING!! \ayWarp event detected on me!");
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /attack off", FromPlugin);
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
			WarpTimer = MQGetTickCount64() + 30000LL;
			HideDoCommand(GetCharInfo()->pSpawn, "/mqlog Warp event detected", FromPlugin);
			WarpActionTimer = MQGetTickCount64() + 5000LL;
			ShouldIExit = true;
			if (strlen(SafetyCommandOnMyWarp))
			{
				strcpy_s(test, SafetyCommandOnMyWarp);
				ParseMacroData(test, MAX_STRING);
				HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			}
		}
		if (SafetyInvisOnWarp)
		{
			if (!PauseOnWarp)
				WriteChatf("\arWARNING!! \ayWarp event detected on me!");
			if (invisSpell)
				HideDoCommand(GetCharInfo()->pSpawn, "/invis", FromPlugin);
			ShouldIExit = true;
		}
	}
	else
	{
		WarpX = GetCharInfo()->pSpawn->X;
		WarpY = GetCharInfo()->pSpawn->Y;
	}
	//check for too far from start of fight
	if (FightX && FightY)
	{
		if (GetDistance(GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Y, FightX, FightY) > 150)
		{
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
			WriteChatf("\arWarning::\ayStrayed too far from camp, possible GM controlled npc or you have been warped");
			HideDoCommand(GetCharInfo()->pSpawn, "/mqlog Warning::too far from camp, possible gm controlled npc or warp", FromPlugin);
			WarpTimer = MQGetTickCount64() + 30000LL;
			if (strlen(SafetyCommandOnTooFarFromCamp))
			{
				strcpy_s(test, SafetyCommandOnTooFarFromCamp);
				ParseMacroData(test, MAX_STRING);
				HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			}
		}
	}
}

void EquipPet()
{
	if (!InGameOK())
		return;
	if (MQGetTickCount64() < SpellTimer || MQGetTickCount64() < PetGearTimer || WinState((CXWnd*)pGiveWnd) || (GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp > 0 && GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp < 30000))
		return;
	if (GetCharInfo()->pSpawn->PetID && GetCharInfo()->pSpawn->PetID == 0xFFFFFFFF)
		return;
	PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(GetCharInfo()->pSpawn->PetID);
	if (GetDistance(pSpawn->X, pSpawn->Y) > 20)
		return;
	char test[MAX_STRING];
	int capAmt = vPetGearNeed.size();
	for (int i = 0; i < capAmt; i++)
	{
		strcpy_s(test, vPetGearNeed[i].c_str());
		if (!strstr(test, "0"))
		{
			sprintf_s(test, "/itemnotify \"%s\" leftmouseup", vPetGearNeed[i].c_str());
			ParseMacroData(test, MAX_STRING);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			strcpy_s(test, "/squelch /tar id ${Me.Pet.ID}");
			ParseMacroData(test, MAX_STRING);
			HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
			HideDoCommand(GetCharInfo()->pSpawn, "/click left target", FromPlugin);
			petGearSize--;
			while (i >= capAmt)
				vPetGearHave.push_back("0");
			vPetGearHave[i] = vPetGearNeed[i];
			vPetGearNeed[i] = "0";
			WriteChatf("Giving to pet --> %s", vPetGearHave[i].c_str());
			PetGearTimer = MQGetTickCount64() + 2000LL;
			return;
		}
	}
	if (WinState((CXWnd*)pGiveWnd))
		HideDoCommand(GetCharInfo()->pSpawn, "/notify GiveWnd GVW_Give_Button leftmouseup", FromPlugin);
}

//COMMANDS



PSPELL FindSpell(PSPELL pSpell, vector<PSPELL> &v)
{
	for (unsigned int i = 0; i < v.size(); i++)
	{
		if (v[i]->SpellGroup == pSpell->SpellGroup)
			return pSpell;
	}
	return NULL;
}

void MySpells(PSPAWNINFO pChar, PCHAR szLine)
{
	if (GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2())
	{
		int option = 0;
		if (strlen(szLine) != 0)
		{
			CHAR Arg1[MAX_STRING] = { 0 };
			GetArg(Arg1, szLine, 1);
			if (!_stricmp(Arg1, "have"))
			{
				option = 1;
			}
			if (!_stricmp(Arg1, "missing"))
			{
				option = 0;
			}
		}
		vector<PSPELL> MySpells, HaveSpells, MissingSpells;
		for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
		{
			if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
				if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
				{
					MySpells.push_back(pSpell);
				}
			}
		}
		for (unsigned long nAbility = 0; nAbility < NUM_ALT_ABILITIES; nAbility++)
		{
			if (PALTABILITY pAbility = pAltAdvManager->GetAAById(nAbility)) {
				if (char *pName = pCDBStr->GetString(pAbility->nName, 1, NULL))
				{
					if (pAbility->SpellID != -1) {
						if (PSPELL pSpell = GetSpellByID(pAbility->SpellID))
						{
							MySpells.push_back(pSpell);
						}
					}
				}
			}
		}
		for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
		{
			if (GetCharInfo2()->SpellBook[nSpell] != 0xFFFFFFFF)
			{
				if (PSPELL pSpell = GetSpellByID((GetCharInfo2()->SpellBook[nSpell])))
				{
					MySpells.push_back(pSpell);
				}
			}
		}
		{
			for (unsigned int i = 0; i < 50000; i++)
			{
				if (PSPELL pSpell = GetSpellByID(i))
				{
					if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] <= GetCharInfo2()->Level)
					{
						if (FindSpell(pSpell, MySpells))
							HaveSpells.push_back(pSpell);
						else
						{
							if (!FindSpell(pSpell, MissingSpells))
								MissingSpells.push_back(pSpell);
							else
								continue;
						}
					}
				}
			}
		}
		char key[MAX_STRING] = { 0 }, total[MAX_STRING] = { 0 }, spellName[MAX_STRING] = { 0 };
		if (option == 0)
		{
			sprintf_s(total, "%d", MissingSpells.size());
			WritePrivateProfileString(INISection, "MissingTotal", total, INIFileName);
			for (unsigned int i = 0; i < MissingSpells.size(); i++)
			{
				sprintf_s(key, "Missing%d", i);
				sprintf_s(spellName, "%s|%d", MissingSpells[i]->Name, MissingSpells[i]->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]);
				WritePrivateProfileString(INISection, key, spellName, INIFileName);
			}
		}
		if (option == 1)
		{
			sprintf_s(total, "%d", HaveSpells.size());
			WritePrivateProfileString(INISection, "HaveTotal", total, INIFileName);
			for (unsigned int i = 0; i < HaveSpells.size(); i++)
			{
				sprintf_s(key, "Have%d", i);
				sprintf_s(spellName, "%s|%d", HaveSpells[i]->Name, HaveSpells[i]->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]);
				WritePrivateProfileString(INISection, key, spellName, INIFileName);
			}
		}
		return;
	}
}

void LoadIgnoreCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (GetCharInfo2() && GetCharInfo() && GetCharInfo()->pSpawn)
		sprintf_s(ImmuneINIFileName, "%s\\macros\\Mob_Ignore_List.ini", gszINIPath);
	else
		return;
	if (!pZoneInfo)
		return;
	sprintf_s(IgnoreINISection, "Ignores.%s", ((PZONEINFO)pZoneInfo)->ShortName);
	GetPrivateProfileString(IgnoreINISection, "Ignore", "|", IgnoreList, MAX_STRING, ImmuneINIFileName);
	sprintf_s(MezImmuneINISection, "MezImmunes.%s", ((PZONEINFO)pZoneInfo)->ShortName);
	GetPrivateProfileString(MezImmuneINISection, "MezImmune", "|", MezImmuneList, MAX_STRING, ImmuneINIFileName);
	sprintf_s(SnareImmuneINISection, "SnareImmunes.%s", ((PZONEINFO)pZoneInfo)->ShortName);
	GetPrivateProfileString(SnareImmuneINISection, "SnareImmune", "|", SnareImmuneList, MAX_STRING, ImmuneINIFileName);
	sprintf_s(CharmImmuneINISection, "CharmImmunes.%s", ((PZONEINFO)pZoneInfo)->ShortName);
	GetPrivateProfileString(CharmImmuneINISection, "CharmImmune", "|", CharmImmuneList, MAX_STRING, ImmuneINIFileName);
	if (!strcmp(CharmImmuneList, "|"))
	{
		WritePrivateProfileString(IgnoreINISection, "Ignore", IgnoreList, ImmuneINIFileName);
		WritePrivateProfileString(MezImmuneINISection, "MezImmune", MezImmuneList, ImmuneINIFileName);
		WritePrivateProfileString(SnareImmuneINISection, "SnareImmune", SnareImmuneList, ImmuneINIFileName);
		WritePrivateProfileString(CharmImmuneINISection, "CharmImmune", CharmImmuneList, ImmuneINIFileName);
	}
	CHAR immunelist[MAX_STRING];
	sprintf_s(immunelist, "\apMezImmunes:\aw%s", MezImmuneList);
	WriteChatColor(immunelist, USERCOLOR_DEFAULT);
	sprintf_s(immunelist, "\arIgnores:\aw%s", IgnoreList);
	WriteChatColor(immunelist, USERCOLOR_DEFAULT);
	sprintf_s(immunelist, "\agSnareImmunes:\aw%s", SnareImmuneList);
	WriteChatColor(immunelist, USERCOLOR_DEFAULT);
	sprintf_s(immunelist, "\amCharmImmunes:\aw%s", CharmImmuneList);
	WriteChatColor(immunelist, USERCOLOR_DEFAULT);
}

void BotIniCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (strlen(szLine) != 0)
	{
		CHAR Arg1[MAX_STRING] = { 0 };
		GetArg(Arg1, szLine, 1);
		WriteChatf("Loading custom INI: %s", Arg1);
		Configure(Arg1, 1);
	}
}

void BotCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	PCHAR BotSettings[] = { "DefaultGem", "AACutoffTime", "AAStopAt", "AggroAt", "DotAt", "DebuffAdds", "DebuffAt", "DotStopAt", "DebuffStopAt", "HealAt",
		"HealPetAt", "JoltAt", "LifetapAt", "ManaAt", "NukeAt", "SnareAt", "SwarmAt", "UseAura", "UseClickyBuffs", "UsePetBuffs",
		"UseMainTankBuffs", "UseMez", "UsePet", "UsePetBuffs", "RezRadius", "UseSelfBuffs", "AutoFollowLeader", "MedStopAt", "AssistAt", "AssistRange",
		"PetAttackAt", "PetAttackRange", "MeleeAt", "MeleeDistance", "AutofireAt", "AutofireDistance", "UseAutoFire", "AnnounceEQBC", "AnnounceEcho", "MedStartAt",
		"MedStopAt", "MedToFull", "MinManaToNuke", "FaceAngle", "UseNetBots", "AutoInvisDistance", "FadeAt", "GrabAdds", "GrabPrimary", "HealDurationAt",
		"HealDelayedAt", "HealGroupAt", "ImHitAt", "KnockbackAdds", "KnockbackPrimary", "LootRadius", "LootInCombat", "RootAdds", "RootPrimary", "UseAENukes",
		"UseClickyNukes", "UseManaInCombat", "CustomPetSpell", "AutoFollowSettings", "NetBotsName", "RestTwist", "CombatTwist", "MercAssistAt", "MercAssistRange", "ManaToSummon",
		"PetIllusionItem", "RepositionIfSummoned", "AnnounceAdds", "AlwaysCheckAdds", "CycleDelay", "BotCommandsOn", "BotCommandsOff", "HealToTAt", "UseDiscsOffensive", "UseDiscsDefensive",
		"DebuffAEAmount", "SummonModRods", "UseModRods", "UseManaOnOthers", "DoDebug", "AnnounceBehaviorChange", "PullingDistance", "EndMedToFull", "EndMedStartAt", "EndMedStopAt", "PetReagentCheck",
		"AttackCommand", "NukeToTAt", "StandOnPetAttack", "Debugging", "MedDuringCombat", "AssistName", "UseManualAssist", "MQ2NavDistance", "DoPetSwarm","DoPulls",NULL };
	char buffer[MAX_STRING] = "";
	if (strlen(szLine) != 0)
	{
		CHAR Arg1[MAX_STRING] = { 0 }, Arg2[MAX_STRING] = { 0 };
		GetArg(Arg1, szLine, 1);
		if (!_stricmp(Arg1, "off"))
		{
			//WriteBotSetting(0);
			UnloadSpells();
			strcpy_s(buffer, "[MQ2Bot] Bot Deactivated");
			Bot = 0;
			if (twisting)
				HideDoCommand(GetCharInfo()->pSpawn, "/squelch /bottwist off", FromPlugin);
			if (strlen(BotCommandsOff) > 1)
				HideDoCommand(GetCharInfo()->pSpawn, BotCommandsOff, FromPlugin);
		}
		else if (!_stricmp(Arg1, "on"))
		{
			//WriteBotSetting(1);
			if (GetCharInfo()->pSpawn->mActorClient.Class == 8)
				BardClass = true;
			HideDoCommand(GetCharInfo()->pSpawn, "/loadbot", FromPlugin);
			Bot = 1;
			strcpy_s(IgnoreBaneStrike, "|");
			HideDoCommand(GetCharInfo()->pSpawn, "/loadignore", FromPlugin);
			strcpy_s(buffer, "[MQ2Bot] Bot Activated");
			if (strlen(BotCommandsOn) > 1)
				HideDoCommand(GetCharInfo()->pSpawn, BotCommandsOn, FromPlugin);
		}
		else if (!_stricmp(Arg1, "populate"))
		{
			GetArg(Arg2, szLine, 2);
			if (strlen(Arg2) != 0)
			{
				if (!_stricmp(Arg2, "heal"))
				{
					CreateHeal2();
					PopulateIni(vHeal2, "Heal");
				}
			}
		}
		else if (!_stricmp(Arg1, "unpause"))
		{
			if (MacroPulling == true)
			{
				MacroPulling = false;
				WriteChatf("MQ2Bot \agUnpaused");
			}
		}
		else if (!_stricmp(Arg1, "pause"))
		{
			if (!MacroPulling)
			{
				MacroPulling = true;
				WriteChatf("MQ2Bot \arPaused");
			}
		}
		else if (strstr(Arg1, "version"))
		{
			ShowVersion();
			return;
		}
		else if (strstr(Arg1, "status"))
		{
			WriteChatf("[MQ2Bot] Status = %s", Bot == 0 ? "OFF" : Bot == 1 ? "ON" : Bot == -1 ? "PRE" : "Unknown");
			return;
		}
		else
		{
			char Var[MAX_STRING], Set[MAX_STRING];
			GetArg(Var, szLine, 1, FALSE, FALSE, FALSE, '=');
			GetArg(Set, szLine, 2, FALSE, FALSE, FALSE, '=');
			if (!_stricmp(Var, "AutoFollowLeader") && !_stricmp(Set, "0"))
				HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
			int total = atoi(BotSettings[0]);
			char szTemp[MAX_STRING] = { 0 };
			long Class = GetCharInfo()->pSpawn->mActorClient.Class;
			long Races = GetCharInfo2()->Race;
			long Level = GetCharInfo2()->Level;
			sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
			sprintf_s(INISection, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
			Shrouded = GetCharInfo2()->Shrouded;
			if (!Shrouded)
				INISection[strlen(PLUGIN_NAME)] = 0;
			for (unsigned int i = 0; BotSettings[i]; i++)
			{
				if (!_stricmp(BotSettings[i], Var))
				{
					WritePrivateProfileString(INISection, BotSettings[i], Set, INIFileName);
					// Need to pass a buffer
					char szNull[MAX_STRING] = { 0 };
					Configure(szNull, 0);
					WriteChatf("\ayMQ2Bot::\ao%s \awset to \ag%s", BotSettings[i], Set);
					if (!_stricmp(BotSettings[i], "debugging"))
					{
						char Filename[MAX_STRING] = { 0 };
						sprintf_s(Filename, "%s\\MQ2Bot_Debug.log", gszLogPath);
						if (DEBUG_DUMPFILE)
						{
							__try {
								_unlink(Filename);
							}
							__except (EXCEPTION_EXECUTE_HANDLER)
							{
							}
						}
						WriteChatf("\ar%s\ax::\amDEBUG logging to \at%s\ax is now %s\ax.",
							PLUGIN_NAME,
							Filename,
							DEBUG_DUMPFILE ? "\aoON" : "\agOFF");
					}
					return;
				}
			}

		}
	}
	if (strlen(buffer) > 0)
		WriteChatColor(buffer, USERCOLOR_DEFAULT);
}

void FindMobCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (DoDebug)
		WriteChatf("DEBUG: FindMob");
	//WriteChatf("void FindMobCommand");
	if (PSPAWNINFO pSpawn = FindMob())
		WriteChatf("Nearest Mob: %s, distance: %f", pSpawn->DisplayedName, GetDistance(pSpawn, GetCharInfo()->pSpawn));
	CheckPull();
}

void InvisCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (!vInvis.size())
		return;
	char test[MAX_STRING];
	sprintf_s(test, "${If[${Me.AltAbilityReady[%s]},1,0]}", vInvis[0].c_str());
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		if (BardClass)
			HideDoCommand(GetCharInfo()->pSpawn, "/squelch /bottwist off", FromPlugin);
		sprintf_s(test, "/casting \"%s\" alt", vInvis[0].c_str());
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
	}
}

void InvisUndeadCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (!vInvisUndead.size())
		return;
	char test[MAX_STRING];
	sprintf_s(test, "${If[${Me.AltAbilityReady[%s]},1,0]}", vInvisUndead[0].c_str());
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		sprintf_s(test, "/casting \"%s\" alt", vInvisUndead[0].c_str());
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
	}
}

void LevitateCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (!vLevitate.size())
		return;
	char test[MAX_STRING];
	sprintf_s(test, "${If[${Me.AltAbilityReady[%s]},1,0]}", vLevitate[0].c_str());
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1)
	{
		sprintf_s(test, "/casting \"%s\" alt", vLevitate[0].c_str());
		HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
	}
}

void IgnoreMobCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
	{
		char test[MAX_STRING];
		sprintf_s(test, "|%s|", pSpawn->DisplayedName);
		if (!strstr(IgnoreList, test))
		{
			sprintf_s(IgnoreList, "%s%s|", IgnoreList, pSpawn->DisplayedName);
			WritePrivateProfileString(IgnoreINISection, "Ignore", IgnoreList, ImmuneINIFileName);
			WriteChatf("\ar%s \aw added to Ignores.", pSpawn->DisplayedName);
			GetPrivateProfileString(IgnoreINISection, "Ignore", "|", IgnoreList, MAX_STRING, ImmuneINIFileName);
			WriteChatf("\ar%s", IgnoreList);
		}
	}
}

void SnareImmuneMobCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
	{
		char test[MAX_STRING];
		sprintf_s(test, "|%s|", pSpawn->DisplayedName);
		if (!strstr(SnareImmuneList, test))
		{
			sprintf_s(SnareImmuneList, "%s%s|", SnareImmuneList, pSpawn->DisplayedName);
			WritePrivateProfileString(SnareImmuneINISection, "SnareImmune", SnareImmuneList, ImmuneINIFileName);
			WriteChatf("\ag%s \aw added to SnareImmunes.", pSpawn->DisplayedName);
			GetPrivateProfileString(SnareImmuneINISection, "SnareImmune", "|", SnareImmuneList, MAX_STRING, ImmuneINIFileName);
			WriteChatf("\ag%s", SnareImmuneList);
		}
	}
}

void MezImmuneMobCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
	{
		char test[MAX_STRING];
		sprintf_s(test, "|%s|", pSpawn->DisplayedName);
		if (!strstr(MezImmuneList, test))
		{
			sprintf_s(MezImmuneList, "%s%s|", MezImmuneList, pSpawn->DisplayedName);
			WritePrivateProfileString(MezImmuneINISection, "MezImmune", MezImmuneList, ImmuneINIFileName);
			WriteChatf("\ap%s \aw added to MezImmunes.", pSpawn->DisplayedName);
			GetPrivateProfileString(MezImmuneINISection, "MezImmune", "|", MezImmuneList, MAX_STRING, ImmuneINIFileName);
			WriteChatf("\ap%s", MezImmuneList);
		}
	}
}

void CharmImmuneMobCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
	{
		char test[MAX_STRING];
		sprintf_s(test, "|%s|", pSpawn->DisplayedName);
		if (!strstr(CharmImmuneList, test))
		{
			sprintf_s(CharmImmuneList, "%s%s|", CharmImmuneList, pSpawn->DisplayedName);
			WritePrivateProfileString(CharmImmuneINISection, "CharmImmune", CharmImmuneList, ImmuneINIFileName);
			WriteChatf("\am%s \aw added to CharmImmunes.", pSpawn->DisplayedName);
			GetPrivateProfileString(CharmImmuneINISection, "CharmImmune", "|", CharmImmuneList, MAX_STRING, ImmuneINIFileName);
			WriteChatf("\am%s", CharmImmuneList);
		}
	}
}

void OffTankCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (atoi(szLine))
	{
		OffTank = atoi(szLine);
		if (OffTank)
			WriteChatf("Now off-tanking anything on \agXTarget\ar%s", szLine);
		if (!OffTank)
			WriteChatf("Off-tanking disabled", szLine);
	}
}

void PetEquipCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	CreatePetGear();
}

void COTHCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (!GetCharInfo()->pSpawn || GetCharInfo()->pSpawn->mActorClient.Class != 13)
		return;
	char buffer[MAX_STRING] = "", buffer2[MAX_STRING] = "";
	if (strlen(szLine) != 0)
	{
		CHAR Arg1[MAX_STRING] = { 0 };
		GetArg(Arg1, szLine, 1);
		for (unsigned int i = 1; i < 6; i++)
		{
			_strlwr_s(Arg1);
			if (strstr(Arg1, "group"))
			{
				char test[MAX_STRING], sSpell[MAX_STRING] = "";
				strcpy_s(test, "${If[${Me.Book[Call of the Heroes]},1,0]}");
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) == 1)
					strcpy_s(sSpell, "Call of the Heroes");
				strcpy_s(test, "${If[${Me.Book[Call of the Heroes Rk. II]},1,0]}");
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) == 1)
					strcpy_s(sSpell, "Call of the Heroes Rk. II");
				strcpy_s(test, "${If[${Me.Book[Call of the Heroes Rk. III]},1,0]}");
				ParseMacroData(test, MAX_STRING);
				if (atoi(test) == 1)
					strcpy_s(sSpell, "Call of the Heroes Rk. III");
				if (strlen(sSpell) > 1 && GetSpellByName(sSpell))
					CastSpell(GetCharInfo()->pSpawn, GetSpellByName(sSpell), "\ay", 1);
				else
					WriteChatf("You do not have the spell \"Call of the Heroes\"");
			}
			if (!strstr(Arg1, "group"))
			{
				sprintf_s(buffer, "${If[${Group.Member[%d].Type.Equal[pc]} && ${Group.Member[%d].Name.Equal[%s]} && ${Me.AltAbilityReady[Call of the Hero]},${Group.Member[%d].ID},0]}", i, i, Arg1, i);
				ParseMacroData(buffer, MAX_STRING);
				if (atol(buffer) > 0)
				{
					PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(atol(buffer));
					TargetIT(pSpawn);
					strcpy_s(buffer2, "${Me.AltAbility[Call of the Hero].Spell.ID}");
					ParseMacroData(buffer2, MAX_STRING);
					PSPELL pSpell = GetSpellByID(atol(buffer2));
					if (pSpell)
						CastSpell(pSpawn, pSpell, "\ay", 0);
					return;
				}
			}
		}
	}
	if (strlen(szLine) < 1)
	{
		GroupFarthest(20);
		if (gCotH)
		{
			strcpy_s(buffer, "${If[${Me.AltAbilityReady[Call of the Hero]},${Me.AltAbility[Call of the Hero].Spell.ID},0]}");
			ParseMacroData(buffer, MAX_STRING);
			if (atol(buffer) > 0)
			{
				PSPELL pSpell = GetSpellByID(atol(buffer));
				if (pSpell)
				{
					TargetIT(gCotH);
					CastSpell(gCotH, pSpell, "\ay", 0);
				}
				return;
			}
		}
	}
}

void DoorCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	HideDoCommand(GetCharInfo()->pSpawn, "/squelch /doortar", FromPlugin);
	FindDoor = 1;
	DoorTimer = MQGetTickCount64() + 400LL;
}

void CampfireCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	CampfireStep = 1;
}

void BotTwistCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	int songCount = 0;
	char song[MAX_STRING] = "";
	bool once = false;
	if (!IsNumber(GetArg(song, szLine, 1)))
	{
		char szTemp[MAX_STRING] = { 0 };
		strcpy_s(szTemp, szLine);
		_strlwr_s(szTemp);
		if (GetArg(song, szTemp, 1))
			if (strstr(szTemp, "off"))
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stopsong", FromPlugin);
				vTwist.clear();
				vTwistSpell.clear();
				strcpy_s(twistList, "");
				twisting = false;
				return;
			}
		if (GetArg(song, szTemp, 1))
			if (strstr(szTemp, "once"))
			{
				HideDoCommand(GetCharInfo()->pSpawn, "/stopsong", FromPlugin);
				once = true;
				_strlwr_s(song);

				GetArg(song, szTemp, 1, FALSE, FALSE, FALSE, 'e');
			}
	}
	if (!once)
	{
		twisting = true;
		vTwist.clear();
		vTwistSpell.clear();
		strcpy_s(twistList, "");
	}
	char test[MAX_STRING] = "";
	for (unsigned int i = 1; i <= strlen(szLine); i++)
	{
		if (strlen(GetArg(song, szLine, i)))
			if (atoi(song) <= NUM_SPELL_GEMS && atoi(song) > 0)
			{
				PSPELL pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[atoi(song) - 1]);
				if (pSpell)
				{
					if (!once)
					{
						/*if(int i==1)
						sprintf_s(test,"%d",vTwist[i]);
						else
						sprintf_s(test," %d",vTwist[i]);
						strcat_s(twistList,test);*/
						vTwist.push_back(atoi(song));
						vTwistSpell.push_back(pSpell);
						WriteChatf("%s", pSpell->Name);
					}
					else
					{
						HideDoCommand(GetCharInfo()->pSpawn, "/stopsong", FromPlugin);
						sprintf_s(song, "/cast %d", atoi(song));
						HideDoCommand(GetCharInfo()->pSpawn, song, FromPlugin);
						TwistTimer = MQGetTickCount64() + 3300LL;
					}

				}
			}
	}
	if (vTwist.size())
	{
		songCurrent = 0;
		songPrev = 0;
		songNext = 0;
	}
}

void ImmuneToggleCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	CHAR Arg1[MAX_STRING] = { 0 };
	GetArg(Arg1, szLine, 1);
	if (!_stricmp(Arg1, "off"))
	{
		ImmunityCheck = false;
	}
	else if (!_stricmp(Arg1, "on"))
	{
		ImmunityCheck = true;
	}
	else if (ImmunityCheck)
		ImmunityCheck = false;
	else
		ImmunityCheck = true;
	if (ImmunityCheck)
		WriteChatf("\ar[MQ2Bot]\aw::Now ignoring immune lists.");
	else
		WriteChatf("\ar[MQ2Bot]\aw::No longer ignoring immune lists.");

}

void InviteGuildToGroup(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (GetCharInfo()->pSpawn->GuildID == 0xFFFFFFFF)
		return;
	PSPAWNINFO pSpawns = NULL;
	char invite[MAX_STRING] = "", names[MAX_STRING] = "";
	if (ppSpawnManager && pSpawnList)
		pSpawns = (PSPAWNINFO)pSpawnList;
	int count = 0;
	while (pSpawns && count < 5)
	{
		if (pSpawns->GuildID != 0xFFFFFFFF)
		{
			if (pSpawns->GuildID == GetCharInfo()->pSpawn->GuildID)
			{
				if (GetCharInfo()->pGroupInfo)
				{
					sprintf_s(names, "${If[!${Group.Member[%s].ID} && ${Spawn[%s].Distance}<600 && ${Math.Calc[%d + ${Group.GroupSize}].Int}<6,1,0]}", pSpawns->DisplayedName, pSpawns->DisplayedName, count);
					ParseMacroData(names, MAX_STRING);
					if (atoi(names) == 1)
					{
						sprintf_s(invite, "/invite %s", pSpawns->Name);
						HideDoCommand(GetCharInfo()->pSpawn, invite, FromPlugin);
						++count;
					}
				}
				if (!GetCharInfo()->pGroupInfo)
				{
					if (GetDistance(GetCharInfo()->pSpawn, pSpawns) < 600 && count < 5)
					{
						sprintf_s(invite, "/invite %s", pSpawns->Name);
						HideDoCommand(GetCharInfo()->pSpawn, invite, FromPlugin);
						++count;
					}
				}
			}
		}
		if (count < 5)
			pSpawns = pSpawns->pNext;
	}
}

void InviteGuildToRaid(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	if (GetCharInfo()->pSpawn->GuildID == 0xFFFFFFFF)
		return;
	PSPAWNINFO pSpawns = NULL;
	char invite[MAX_STRING] = "", names[MAX_STRING] = "";
	if (ppSpawnManager && pSpawnList)
		pSpawns = (PSPAWNINFO)pSpawnList;
	int count = 0;
	while (pSpawns && count < 54)
	{
		if (pSpawns->GuildID != 0xFFFFFFFF)
		{
			if (pSpawns->GuildID == GetCharInfo()->pSpawn->GuildID && pSpawns != GetCharInfo()->pSpawn)
			{
				if (pRaid->RaidMemberCount > 0)
				{
					sprintf_s(names, "${If[!${Raid.Member[%s].ID} && ${Spawn[%s].Distance}<1000 && ${Math.Calc[%d + ${Raid.Members}].Int}<6,1,0]}", pSpawns->DisplayedName, pSpawns->DisplayedName, count);
					ParseMacroData(names, MAX_STRING);
					if (atoi(names) == 1)
					{
						sprintf_s(invite, "/raidinvite %s", pSpawns->Name);
						HideDoCommand(GetCharInfo()->pSpawn, invite, FromPlugin);
						++count;
					}
				}
				if (pRaid->RaidMemberCount < 1)
				{
					if (GetDistance(GetCharInfo()->pSpawn, pSpawns) < 1000 && count < 54)
					{
						sprintf_s(invite, "/raidinvite %s", pSpawns->Name);
						HideDoCommand(GetCharInfo()->pSpawn, invite, FromPlugin);
						++count;
					}
				}
			}
		}
		if (count < 54)
		{
			PSPAWNINFO tSpawn = pSpawns;
			pSpawns = pSpawns->pNext;
			if (pSpawns == tSpawn)
				return;
		}

	}
}

void RemoveMyAura(PSPAWNINFO pChar, PCHAR szLine)
{
	int aura = 1;
	if (strstr(szLine, "2"))
		aura = 2;
	char test[MAX_STRING] = "";
	ULONGLONG tempTimer = MQGetTickCount64() + 500LL;
	strcpy_s(test, "${If[!${Window[AuraWindow].Open},/squelch /windowstate AuraWindow Open,]}");
	ParseMacroData(test, MAX_STRING);
	HideDoCommand(GetCharInfo()->pSpawn, test, FromPlugin);
	int c = 0;
	strcpy_s(test, "0");
	while (MQGetTickCount64() < tempTimer)
	{
		++c;
		/*strcpy_s(test,"${If[${Window[AuraWindow].Open},1,0]}");
		ParseMacroData(test,MAX_STRING);
		if(atoi(test)==1)
		break;*/
	}
	if (aura == 1)
		HideDoCommand(GetCharInfo()->pSpawn, "/notify AuraWindow AuraList listselect 1", FromPlugin);
	if (aura == 2)
		HideDoCommand(GetCharInfo()->pSpawn, "/notify AuraWindow AuraList listselect 2", FromPlugin);
	tempTimer = MQGetTickCount64() + 200LL;
	while (MQGetTickCount64() < tempTimer)
	{
		++c;
	}
	HideDoCommand(GetCharInfo()->pSpawn, "/notify AuraWindow 0 contextmenu 1", FromPlugin);
}

void SafetyCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!SafetyCheck)
	{
		SafetyCheck = true;
		char test[MAX_STRING];
		strcpy_s(LastTell, "");
		strcpy_s(test, "${EverQuest.LastTell}");
		ParseMacroData(test, MAX_STRING);
		strcpy_s(LastTell, test);
	}
	else
	{
		SafetyCheck = false;
	}
}

void SpawnTrackCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	SpawnTracker.clear();
	if (GetCharInfo() && !GetCharInfo()->pGroupInfo)
	{
		SpawnTrack(GetCharInfo()->pSpawn);
		return;
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		if (GetCharInfo() && GetCharInfo()->pGroupInfo && GetCharInfo()->pGroupInfo->pMember[i] && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn && GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->SpawnID>0)
		{
			if (GetCharInfo()->pGroupInfo->pMember[i]->pSpawn->Type == SPAWN_PLAYER || GetCharInfo()->pGroupInfo->pMember[i]->Mercenary)
			{
				SpawnTrack(GetCharInfo()->pGroupInfo->pMember[i]->pSpawn);
			}
		}
	}
	if (gTargetbuffs && pTarget)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
		{
			SpawnTrack(pSpawn);
			SpawnTrackUpdate(pSpawn, 3);
			SpawnTrackReport(pSpawn);
		}
	}
	CheckBuffs();
}

//CREATE SPELLS START
void CreateAA()
{
	if (!InGameOK())
		return;
	vAA.clear();
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
		"Bladed Song", "Arcane Fury", "Inquisitor's Judgment", "Dance of Blades", "Channeling the Divine", "Life Burn",
		"A Tune Stuck in Your Head", "Feral Swipe", "Vehement Rage", "Distraction Attack", "Juggernaut Surge", "Reckless Abandon", "Savage Spirit",
		"Untamed Rage", "War Cry of the Braxi", "Five Point Palm", "Battle Stomp", "Blinding Fury", "Desperation", "Stunning Kick",
		"Two-Finger Wasp Touch", "Bloodlust", "Cascading Rage", "6492", "Hand of Tunare", "Thunder of Karana", "Destructive Vortex",
		"Nature's Fury", "Elemental Arrow", "Reinforced Malaise", "T'Vyl's Resolve", "Flurry of Notes", "Frenzied Kicks", "Rake's Rampage",
		"War Stomp", "Lyrical Prankster", "Enduring Frenzy", "Fleeting Spirit", NULL }; // "Taste of Blood", "Selo's Kick",  now permanent AA
	PCHARINFO pChar = GetCharInfo();
	char szBuffer[MAX_STRING];
	int aaIndex;
	PALTABILITY aa;
	for (unsigned int i = 0; szAA[i]; i++)
	{
		strcpy_s(szBuffer, szAA[i]);
		if (atoi(szBuffer) > 0)
			aaIndex = GetAAIndexByID(atoi(szBuffer));
		else
			aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && (int)aa->ReuseTimer > 0)
				vAA.push_back(szBuffer);
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "AAIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("\apAAIf: \aw%s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vAA.size(); i++)
	{
		sprintf_s(szSpell, "AAIf%d", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		}
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("\awAA%d: \ap%s - \awAAIf%d: %s", i, vAA[i].c_str(), i, SpellIt->second.c_str());
		else
			WriteChatf("\awAA%d: \ap%s", i, vAA[i].c_str());
	}
}

void CreateAggro()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vAggro.clear();
	vAggroName.clear();
	PCHAR szAggro[] = { "Improved Explosion of Spite", "Explosion of Spite", "Improved Explosion of Hatred", "Explosion of Hatred",
		"Beacon of the Righteous", "Blast of Anger", "Projection of Fury", "Projection of Piety", "Ageless Enmity",
		"Projection of Doom", "Warlord's Grasp", "Enhanced Area Taunt", "Area Taunt", "Dissonant Chord", "Roaring Strike",
		"Mindless Hatred", "Stream of Hatred", "Veil of Darkness", "Rage of the Forsaken", "Hallowed Lodestar", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szAggro[i]; i++)
	{
		strcpy_s(szBuffer, szAggro[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vAggro.push_back(GetSpellByID(aa->SpellID));
				vAggroName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if ((GetSpellAttribX(pSpell, 0) == 192 && GetSpellBase(pSpell, 0) > 0)
				|| (GetSpellAttribX(pSpell, 4) == 192 && GetSpellBase(pSpell, 4) > 0))
			{
				vAggro.push_back(pSpell);
				vAggroName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "AggroIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("AggroIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vAggro.size(); i++)
	{
		sprintf_s(szSpell, "AggroIf%d", i);
		WriteChatf("\awAggro%d: \a-r%s", i, vAggro[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("AggroIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateAura()
{
	if (!InGameOK())
		return;
	vAura.clear();
	vAuraEffect.clear();
	PCHAR szAura[] = { "Twincast Aura", "Holy Aura", "Master's Aura", "Champion's Aura", "Bloodlust Aura", "Blessed Aura", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	bool effectFound = false;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szAura[i]; i++)
	{
		strcpy_s(szBuffer, szAura[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vAura.push_back(GetSpellByID(aa->SpellID));
				LONG slots = GetSpellNumEffects(GetSpellByID(aa->SpellID));
				for (LONG i = 0; i < slots; i++) {
					LONG attrib = GetSpellAttrib(GetSpellByID(aa->SpellID), i);
					if (attrib == 351)
					{
						effectFound = true;
						vAuraEffect.push_back(GetSpellByID(GetSpellBase2(GetSpellByID(aa->SpellID), i)));
					}
				}
				if (!effectFound)
					vAuraEffect.push_back(GetSpellByID(aa->SpellID));
			}
		}
		for (unsigned long nCombatAbility = 0; nCombatAbility < NUM_COMBAT_ABILITIES; nCombatAbility++)
		{
			if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombatAbility)) {
				if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(nCombatAbility)))
				{
					if (PSPELL pAura = GetSpellByName(szAura[i]))
					{
						if (pAura->ID == pSpell->ID)
						{
							vAura.push_back(pAura);
							char test[MAX_STRING];
							sprintf_s(test, "%s Effect", szAura[i]);
							if (PSPELL pAuraEffect = GetSpellByName(test))
								vAuraEffect.push_back(pAuraEffect);
							else
								vAuraEffect.push_back(pAura);
						}
					}
				}
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == 132 || pSpell->Subcategory == 132 || pSpell->Category == 71)
			{
				vAura.push_back(pSpell);
				char szSpell[MAX_STRING];
				LONG slots = GetSpellNumEffects(pSpell);
				for (LONG i = 0; i < slots; i++) {
					LONG attrib = GetSpellAttrib(pSpell, i);
					if (attrib == 351)
					{
						if (GetSpellBase2(pSpell, i) > 1)
							vAuraEffect.push_back(GetSpellByID(GetSpellBase2(pSpell, i)));
						else
						{
							sprintf_s(szSpell, "%s Effect", pSpell->Name);
							if (strstr(pSpell->Name, "Rk."))
							{
								sprintf_s(szSpell, "%s", pSpell->Name);
								std::string str1 = szSpell;
								str1.replace(str1.find("Rk."), 3, "Effect");
								sprintf_s(szSpell, "%s", str1.c_str());
							}
							if (GetSpellByName(szSpell))
								vAuraEffect.push_back(GetSpellByName(szSpell));
							else
								vAuraEffect.push_back(pSpell);
						}
						effectFound = true;
					}
				}
				if (!effectFound)
					vAuraEffect.push_back(pSpell);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "AuraIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("AuraIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vAura.size(); i++)
	{
		sprintf_s(szSpell, "AuraIf%d", i);
		WriteChatf("\awAura%d: \a-m%s", i, vAura[i]->Name);
		WriteChatf("\awAuraEffect%d: \a-m%s", i, vAuraEffect[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("AuraIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateBuff()
{
	PCHAR szBuff[] = { "Talisman of Celerity", 0 };
}

void CreateCall()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vCall.clear();
	vCallName.clear();
	vCallTimeStamp.clear();
	PCHAR szCall[] = { "Call of the Wild", "Call of the Herald", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szCall[i]; i++)
	{
		strcpy_s(szBuffer, szCall[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vCall.push_back(GetSpellByID(aa->SpellID));
				vCallName.push_back(szBuffer);
			}
		}
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		vCallTimeStamp.push_back(0);
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "CallIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("CallIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vCall.size(); i++)
	{
		sprintf_s(szSpell, "CallIf%d", i);
		WriteChatf("\awCall%d: \ao%s", i, vCall[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("CallIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateCharm()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vCharm.clear();
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == 13 || pSpell->Subcategory == 13)
				vCharm.push_back(pSpell);
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "CharmIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("CharmIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vCharm.size(); i++)
	{
		sprintf_s(szSpell, "CharmIf%d", i);
		WriteChatf("\awCharm%d: \ar%s", i, vCharm[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("CharmIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateClickies()
{
	if (!InGameOK())
		return;
	PCHARINFO2 pChar2 = GetCharInfo2();
	int ben = 0, det = 0;
	vClicky.clear();
	vClickySpell.clear();
	vClickyPrestige.clear();
	//unique items
	char uItems[MAX_STRING], uItemsSpell[MAX_STRING];
	PCHAR szuItems[] = { "Rapier of Somber Notes", "Songblade of the Eternal", "Lizardscale Plated Girdle", "Mighty Cloak of Flames", NULL }; //"Ancient Cloak of Flames", 
	for (unsigned int i = 0; szuItems[i]; i++)
	{
		sprintf_s(uItems, "${If[${FindItem[%s].ID},1,0]}", szuItems[i]);
		ParseMacroData(uItems, MAX_STRING);
		if (atoi(uItems) == 1)
		{
			sprintf_s(uItemsSpell, "${FindItem[%s].Spell.Name}", szuItems[i]);
			ParseMacroData(uItemsSpell, MAX_STRING);
			if (!strstr(uItemsSpell, "NULL") && GetSpellByName(uItemsSpell))
			{
				vClicky.push_back(szuItems[i]);
				vClickySpell.push_back(GetSpellByName(uItemsSpell));
				sprintf_s(uItems, "${If[${FindItem[%s].Prestige},1,0]}", szuItems[i]);
				ParseMacroData(uItems, MAX_STRING);
				if (atoi(uItems) == 1)
					vClickyPrestige.push_back(1);
				else
					vClickyPrestige.push_back(0);
			}
		}
	}
	char clEM[MAX_STRING] = "|CLR|DRU|SHM|ENC|NEC|MAG|WIZ|BST|PAL|SHD|RNG|";
	char clOH[MAX_STRING] = "|MNK|BRD|ROG|WAR|BER|BST|PAL|SHD|RNG|";
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray)
	{
		for (unsigned int i = 1; i < NUM_INV_SLOTS; i++)
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
		char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
		strcpy_s(szSpell, "ClickyBuffIf");
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("ClickyBuffIf: %s", SpellIt->second.c_str());
		}
		for (unsigned int x = 0; x < vClicky.size(); x++)
		{
			sprintf_s(szSpell, "ClickyBuffIf%d", x);
			WriteChatf("\awClickyBuff%d: \aw%s - %s", x, vClicky[x].c_str(), vClickySpell[x]->Name);
			if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
			{
				SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
				SpellIt = SpellIf.find(szSpell);
				if (SpellIt != SpellIf.end())
					WriteChatf("ClickyBuffIf%d: %s", x, SpellIt->second.c_str());
			}
		}
	}
	if (reinforcement < 0.01 && GetCharInfo()->pSpawn->Level>55) {
		long Index = GetAAIndexByName("Spell Casting Reinforcement");
		if (Index > 0)
		{
			if (PlayerHasAAAbility(Index))
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
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
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
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
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
					if (pPCData->GetAlternateAbilityId(j) == Index)
						hotExtend = (int)botround(GetCharInfo2()->AAList[j].PointsSpent / 12);
					if (hotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
						break;
				}
		}
		Index = GetAAIndexByName("Lingering Death");
		if (Index > 0)
		{
			if (PlayerHasAAAbility(Index))
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
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
				for (unsigned int j = 0; j < AA_CHAR_MAX_REAL; j++) {
					if (pPCData->GetAlternateAbilityId(j) == Index)
						if (GetCharInfo2()->AAList[j].PointsSpent >= 48)
							dotExtend = 3;
					if (dotExtend > 0.01 || !pPCData->GetAlternateAbilityId(j))
						break;
				}
		}

	}
}

void CreateClickyBuff()
{
	if (!InGameOK())
		return;
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "ClickyIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("ClickyIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < MAX_CLICKIES; i++)
	{
		sprintf_s(szSpell, "Clicky%d", i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			WriteChatf("Clicky%d: %s", i, szTemp);
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			sprintf_s(szSpell, "ClickyIf%d", i);
			if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
			{
				SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
				SpellIt = SpellIf.find(szSpell);
				if (SpellIt != SpellIf.end())
					WriteChatf("ClickyIf%d: %s", i, SpellIt->second.c_str());
			}
		}
	}
}

void CreateCrescendo()
{
	if (!InGameOK() || !BardClass)
		return;
	vCrescendo.clear();
	PSPELL pSpell;
	char szTemp[MAX_STRING], szSpell[MAX_STRING];
	strcpy_s(szSpell, "CrescendoIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("CrescendoIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (strstr(pSpell->Name, "Lively Crescendo") || strstr(pSpell->Name, "Dichotomic Psalm"))
			{
				vCrescendo.push_back(pSpell);
			}
		}
	}

}

void CreateCure()
{
	if (!InGameOK())
		return;
	vCure.clear();
	vCureName.clear();
	PCHAR szCure[] = { "Radiant Cure","Purified Spirits", "Ward of Purity", "Purification", "Blessing of Purification","Purify Body", "Purify Soul",
		"Group Purify Soul","Embrace the Decay","Nature's Salve","Agony of Absolution","Purge Poison", "Purity of Death", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szCure[i]; i++)
	{
		strcpy_s(szBuffer, szCure[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vCure.push_back(GetSpellByID(aa->SpellID));
				vCureName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == 42 && pSpell->Subcategory == 19)
			{
				vCure.push_back(pSpell);
				vCureName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "CureIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("CureIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vCure.size(); i++)
	{
		sprintf_s(szSpell, "CureIf%d", i);
		WriteChatf("\awCure%d: \at%s", i, vCureName[i].c_str());
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("CureIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateDebuff()
{
	if (!InGameOK())
		return;
	vDebuff.clear();
	vDebuffName.clear();
	PCHAR szDebuff[] = { "Malosinete", "Turgur's Swarm", "Pathosis", "Helix of the Undying", "Bite of Tashani", "Dreary Deeds",
		"Blessing of Ro", "Scent of Terris", "Death's Malaise", "Crippling Sparkle", "Ethereal Manipulation",
		"Crippling Apparition", "Crippling Spirit", "Lower Element", "Sha's Reprisal", "Tigir's Insect Swarm",
		"Wind of Malosinete", "Wind of Pathosis", "Season's Wrath", "Eradicate Magic", NULL };
	char szBuffer[MAX_STRING];
	if (DebuffsSpellFirst)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (strstr(pSpell->Name, " Counterbias") || (pSpell->Subcategory == 21 && pSpell->TargetType == 0 || pSpell->Subcategory == 30 || pSpell->Subcategory == 88 || pSpell->Subcategory == 81) && pSpell->Category != 132)
				{
					vDebuff.push_back(pSpell);
					vDebuffName.push_back(pSpell->Name);
				}
			}
		}
	}
	int aaIndex;
	PALTABILITY aa;
	for (unsigned int i = 0; szDebuff[i]; i++)
	{
		strcpy_s(szBuffer, szDebuff[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vDebuff.push_back(GetSpellByID(aa->SpellID));
				vDebuffName.push_back(szBuffer);
			}
		}
	}
	if (!DebuffsSpellFirst)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (strstr(pSpell->Name, " Counterbias") || (pSpell->Subcategory == 21 && pSpell->TargetType == 0 || pSpell->Subcategory == 30 || pSpell->Subcategory == 88 || pSpell->Subcategory == 81) && pSpell->Category != 132)
				{
					vDebuff.push_back(pSpell);
					vDebuffName.push_back(pSpell->Name);
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "DebuffIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("DebuffIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vDebuff.size(); i++)
	{
		sprintf_s(szSpell, "DebuffIf%d", i);
		WriteChatf("\awDebuff%d: \a-o%s", i, vDebuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("DebuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateDisc()
{
	if (!InGameOK())
		return;
	EndRegenSpell = NULL;
	vDisc.clear();
	vDiscType.clear();
	vector<PSPELL> tempSpell;
	vector<int> tempLevel;
	vector<int> tempTimer;
	vector<int> noOverwrite;
	for (unsigned int i = 0; i < 21; i++)
	{
		tempSpell.push_back(NULL);
		tempLevel.push_back(0);
		tempTimer.push_back(i);
		noOverwrite.push_back(0);
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
	for (unsigned int i = 0; i < tempSpell.size() && i < tempLevel.size(); i++)
	{
		if (tempLevel[i] > 0)
			vDisc.push_back(tempSpell[i]);
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING] = { 0 };
	strcpy_s(szSpell, "DiscIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("DiscIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vDisc.size(); i++)
	{
		sprintf_s(szSpell, "DiscIf%d", i);
		DiscCategory(vDisc[i]);
		vDiscType.push_back(spellCat);
		if (strstr(spellCat, "End Regen") && !strstr(vDisc[i]->Name, "Wind"))
		{
			EndRegenSpell = vDisc[i];
			WriteChatf("Endurance Regen Disc: %s", EndRegenSpell->Name);
		}
		WriteChatf("\awDisc%d: \ay%s \aw[%s]", i, vDisc[i]->Name, spellCat);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("DiscIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateDot()
{
	if (!InGameOK())
		return;
	vDot.clear();
	vDotName.clear();
	PCHAR szDot[] = { "Funeral Dirge", "Bite of the Asp", "Cacophony", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szDot[i]; i++)
	{
		strcpy_s(szBuffer, szDot[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vDot.push_back(GetSpellByID(aa->SpellID));
				vDotName.push_back(szBuffer);
			}
		}
	}
	if (!BardClass)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (strstr(pSpell->Name, "Dichotomic Paroxysm") || pSpell->Category == 126 && pSpell->Subcategory == 60 || pSpell->Category == 20 || pSpell->Category == 114 && pSpell->Subcategory == 33 || pSpell->Category == 114 && pSpell->Subcategory == 43 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer)>0)
				{
					vDot.push_back(pSpell);
					vDotName.push_back(pSpell->Name);
				}
			}
		}
	}
	else
	{
		/*for(int i=0; i < vDot.size(); i++)
		{
		WriteChatf("\ay%s",vDot[i]->Name);
		}
		return;*/
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "DotIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("DotIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vDot.size(); i++)
	{
		sprintf_s(szSpell, "DotIf%d", i);
		WriteChatf("\awDot%d: \ay%s", i, vDot[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("DotIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateEndurance()
{
	PCHAR szEndurance[] = { "Communion of Blood", NULL };
}

void CreateFade()
{
	if (!InGameOK())
		return;
	vFade.clear();
	vFadeName.clear();
	PCHAR szFade[] = { "Howl of the Warlord", "Escape", "Divine Peace", "Cover Tracks", "Fading Memories", "Death's Effigy",
		"Imitate Death", "Cloak of Shadows", "Veil of the Underbrush", "Balefire Burst", "A Hole in Space",
		"Self Preservation", "Inconspicuous Totem", "Self Stasis", "Stealthy Getaway", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szFade[i]; i++)
	{
		strcpy_s(szBuffer, szFade[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID) && (!strstr(szBuffer, "Cloak of Shadow") || GetCharInfo()->pSpawn->mActorClient.Class != 5 && GetCharInfo()->pSpawn->mActorClient.Class != 11))
			{
				vFade.push_back(GetSpellByID(aa->SpellID));
				vFadeName.push_back(szBuffer);
			}
		}
	}
	//PSPELL pSpell;
	//for(int i=0;i<NUM_SPELL_GEMS;i++)
	//{
	//	pSpell=GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
	//	if(pSpell){
	//		if(pSpell->Subcategory==5)
	//		{
	//
	//			vFade.push_back(pSpell);
	//		}
	//	}
	//}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "FadeIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("FadeIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vFade.size(); i++)
	{
		sprintf_s(szSpell, "FadeIf%d", i);
		WriteChatf("\awFade%d: \a-w%s", i, vFade[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("FadeIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateFightBuff()
{
	PCHAR szFightBuff[] = { "Imperator's Command", "Yaulp", "Reluctant Benevolence", "Resplendent Glory", "Shield of Reverence",
		"Scourge Skin", "Dampen Resistance", "Scout's Mastery of Fire", "Envenomed Blades", "Rogue's Fury",
		"Imperator's Charge", NULL };
	PCHAR szSpire[] = { "Fundament: First Spire of Ancestors", "Fundament: First Spire of Arcanum", "Fundament: First Spire of Divinity",
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
		"Fundament: Second Spire of the Sensei", "Fundament: Third Spire of the Warlord", NULL };
	PCHAR szArcanum[] = { "Focus of Arcanum", "Acute Focus of Arcanum", "Enlightened Focus of Arcanum", "Empowered Focus of Arcanum", NULL };
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = NULL;
	if (!InGameOK())
		return;
	vFightBuff.clear();
	vFightBuffName.clear();
	for (unsigned int i = 0; szFightBuff[i]; i++)
	{
		aaIndex = GetAAIndexByName(szFightBuff[i]);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vFightBuff.push_back(GetSpellByID(aa->SpellID));
				vFightBuffName.push_back(szFightBuff[i]);
			}
		}
	}
	Spire[0] = 0;
	for (unsigned int i = 0; szSpire[i]; i++)
	{
		for (unsigned long nAbility = 0; nAbility<AA_CHAR_MAX_REAL; nAbility++)
		{
			if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility)))
			{
				if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL))
				{
					if (!_stricmp(szSpire[i], pName))
					{
						strcpy_s(Spire, szSpire[i]);
					}
				}
			}
		}
	}
	if (Spire[0])
	{
		WriteChatf("\awSpire: \am%s", Spire);
		if (GetPrivateProfileString(INISection, "SpireIf", NULL, szTemp, MAX_STRING, INIFileName) != NULL)
		{
			SpellIf.insert(make_pair<string, string>("SpireIf", szTemp));
			SpellIt = SpellIf.find("SpireIf");
			if (SpellIt != SpellIf.end())
				WriteChatf("SpireIf: %s", SpellIt->second.c_str());
		}
	}
	if (BardClass)
		return;
	for (unsigned int i = 0; szArcanum[i]; i++)
	{
		aaIndex = GetAAIndexByName(szArcanum[i]);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vFightBuff.push_back(GetSpellByID(aa->SpellID));
				vFightBuffName.push_back(szArcanum[i]);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (strstr(pSpell->Name, "Growl of the") ||
				strstr(pSpell->Name, "Grim Aura") ||
				strstr(pSpell->Name, "Roar of the Lion") ||
				strstr(pSpell->Name, "Chaotic Benefaction") ||
				strstr(pSpell->Name, "Dichotomic Companion") ||
				strstr(pSpell->Name, "Dichotomic Fury") ||
				HasSpellAttrib(pSpell, 323) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61 ||
				pSpell->Category == 45 && pSpell->Subcategory == 141 && pSpell->TargetType == 6 ||
				pSpell->Subcategory == 142 && pSpell->TargetType == 6 && pSpell->Category != 132 ||
				pSpell->Category == 125 && pSpell->Subcategory == 16 && (pSpell->TargetType == 6 ||
					pSpell->TargetType == 41) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 61
				)
			{
				vFightBuff.push_back(pSpell);
				vFightBuffName.push_back(pSpell->Name);
			}

		}
	}
	strcpy_s(szSpell, "FightBuffIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("FightBuffIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vFightBuff.size(); i++)
	{
		sprintf_s(szSpell, "FightBuffIf%d", i);
		WriteChatf("\awFightBuff%d: \am%s", i, vFightBuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("FightBuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateGrab()
{
	if (!InGameOK())
		return;
	vGrab.clear();
	vGrabName.clear();
	PCHAR szGrab[] = { "Hate's Attraction", "Divine Call", "Lure of the Siren's Song", "Grappling Strike", "Moving Mountains", "Call of Challenge", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szGrab[i]; i++)
	{
		strcpy_s(szBuffer, szGrab[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vGrab.push_back(GetSpellByID(aa->SpellID));
				vGrabName.push_back(szBuffer);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "GrabIfIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("GrabIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vGrab.size(); i++)
	{
		sprintf_s(szSpell, "GrabIf%d", i);
		WriteChatf("\awGrab%d: \a-g%s", i, vGrab[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("GrabIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateHeal()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vHeal.clear();
	vHealName.clear();
	PCHAR szHeal[] = { "Divine Arbitration", "Burst of Life", "Beacon of Life", "Focused Celestial Regeneration", "Celestial Regeneration",
		"Convergence of Spirits", "Union of Spirits", "Focused Paragon of Spirits", "Paragon of Spirit", "Lay on Hands",
		"Hand of Piety", "Ancestral Aid", "Call of the Ancients", "Exquisite Benediction", "Blessing of Tunare", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szHeal[i]; i++)
	{
		strcpy_s(szBuffer, szHeal[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vHeal.push_back(GetSpellByID(aa->SpellID));
				vHealName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == 42 && pSpell->Subcategory != 19 && pSpell->Subcategory != 82 && (pSpell->TargetType == 45 || pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 6 || pSpell->TargetType == 8 || pSpell->TargetType == 41))
			{
				vHeal.push_back(pSpell);
				vHealName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "HealIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("HealIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vHeal.size(); i++)
	{
		sprintf_s(szSpell, "HealIf%d", i);
		WriteChatf("\awHeal%d: \at%s", i, vHealName[i].c_str());
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("HealIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateHeal2()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vHeal2.clear();
	strcpy_s(CurrentRoutine, &(__FUNCTION__[6]));
	PCHAR szHeal[] = { "Divine Arbitration", "Burst of Life", "Beacon of Life", "Focused Celestial Regeneration", "Celestial Regeneration",
		"Convergence of Spirits", "Union of Spirits", "Focused Paragon of Spirits", "Paragon of Spirit", "Lay on Hands",
		"Hand of Piety", "Ancestral Aid", "Call of the Ancients", "Exquisite Benediction", "Blessing of Tunare", NULL };
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING] = { 0 };
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	// check ini for healif
	strcpy_s(szSpell, "HealIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("HealIf: %s", SpellIt->second.c_str());
	}
	//check ini for custom spell
	// sprintf_s(szSpell, "%sTotal",CurrentRoutine);
	strcpy_s(szSpell, "HealTotal");
	int customSpells = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
	for (int i = 0; i < customSpells; i++)
	{
		//sprintf_s(szSpell, "%sSpellName%d", CurrentRoutine, i);
		sprintf_s(szSpell, "HealSpellName%d", i);
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
					strcpy_s(spell.SpellName, szTemp);
					spell.CanIReprioritize = 0;
					vHeal2.push_back(spell);
				}
			}
			else
				for (unsigned int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				{
					if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
					{
						if (!_stricmp(szTemp, pSpell->Name))
						{
							BotSpells spell;
							spell.Spell = pSpell;
							strcpy_s(spell.SpellName, szTemp);
							spell.CanIReprioritize = 0;
							vHeal2.push_back(spell);
						}
					}
				}
		}
	}
	int test = 0;
	for (unsigned int i = 0; szHeal[i]; i++)
	{
		test = 0;
		for (unsigned int z = 0; z < vHeal2.size(); z++)
		{
			if (!_stricmp(vHeal2[z].SpellName, szHeal[i]))
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
					BotSpells spell;
					spell.Spell = GetSpellByID(aa->SpellID);
					strcpy_s(spell.SpellName, szTemp);
					spell.CanIReprioritize = 1;
					vHeal2.push_back(spell);
				}
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			test = 0;
			for (unsigned int z = 0; z < vHeal2.size(); z++)
			{
				if (!_stricmp(vHeal2[z].SpellName, pSpell->Name))
					test = 1;
			}
			if (!test)
			{
				if (pSpell->Category == 42 && pSpell->Subcategory != 19 && pSpell->Subcategory != 82 && (pSpell->TargetType == 45 || pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 6 || pSpell->TargetType == 8 || pSpell->TargetType == 41))
				{
					BotSpells spell;
					spell.Spell = pSpell;
					strcpy_s(spell.SpellName, pSpell->Name);
					spell.CanIReprioritize = 1;
					vHeal2.push_back(spell);
				}
			}
		}
	}
	LoadBotSpell(vHeal2, "Heal");
}

void CreateHealPet()
{
	if (!InGameOK())
		return;
	if (BardClass || !GetCharInfo()->pSpawn->ManaMax)
		return;
	vHealPet.clear();
	vHealPetName.clear();
	PCHAR szHealPet[] = { "Mend Companion", "Replenish Companion", "Companion's Blessing", "Fortify Companion", "Lingering Nightmares", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	PALTABILITY aa = NULL;
	for (unsigned int i = 0; szHealPet[i]; i++)
	{
		strcpy_s(szBuffer, szHealPet[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vHealPet.push_back(GetSpellByID(aa->SpellID));
				vHealPetName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 42 && pSpell->Category == 69 || pSpell->Category == 42 && (pSpell->TargetType == 3 || pSpell->TargetType == 5 || pSpell->TargetType == 8))
			{
				vHealPet.push_back(pSpell);
				vHealPetName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "HealPetIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("HealPetIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vHealPet.size(); i++)
	{
		sprintf_s(szSpell, "HealPetIf%d", i);
		WriteChatf("\awHealPet%d: \a-t%s", i, vHealPetName[i].c_str());
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("HealPetIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateHealToT()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vHealToT.clear();
	vHealToTName.clear();
	PCHAR szHealToT[] = { "Gift of Life", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (int i = 0; szHealToT[i]; i++)
	{
		strcpy_s(szBuffer, szHealToT[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vHealToT.push_back(GetSpellByID(aa->SpellID));
				vHealToTName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0)
			{
				vHealToT.push_back(pSpell);
				vHealToTName.push_back(pSpell->Name);
			}
			else if (pSpell->TargetType == 5)
			{
				for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot)
				{
					if (GetSpellAttrib(pSpell, slot) == 374)
					{
						PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, slot));

						if (pSpell2 && GetSpellAttribX(pSpell2, 0) == 0
							&& GetSpellBase(pSpell2, 0) > 0
							&& pSpell2->TargetType == 46)
						{
							vHealToT.push_back(pSpell);
							vHealToTName.push_back(pSpell->Name);
							break;
						}
					}
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "HealToTIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("HealToTIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vHealToT.size(); i++)
	{
		sprintf_s(szSpell, "HealToTIf%d", i);
		WriteChatf("\awHealToT%d: \at%s", i, vHealToT[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("HealToTIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateImHit()
{
	if (!InGameOK())
		return;
	vImHit.clear();
	PCHAR szImHit[] = { "Improved Sanctuary", "Sanctuary", "Outrider's Evasion", "Shield of Notes", "Hymn of the Last Stand", "Doppelganger",
		"Mind Crash", "Brace for Impact", "Companion of Necessity", "Hold the Line", "Divine Retribution",
		"Blessing of Sanctuary", "Embalmer's Carapace", "Improved Death Peace", "Lyrical Prankster", "Ancestral Guard", "Death Peace",
		"Armor of Ancestral Spirits", "Uncanny Resilience", "Protection of the Spirit Wolf", "Tumble", "Warlord's Bravery",
		"Mark of the Mage Hunter", "Blade Guardian", "Repel", "Reprove", "Renounce", "Defy", "Withstand", "Rune of Banishment",
		"Color Shock", "Mind Over Matter", nullptr };
	PCHARINFO pChar = GetCharInfo();
	if (pChar == nullptr)
		return;
	char szBuffer[MAX_STRING] = { 0 };
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szImHit[i]; i++)
	{
		strcpy_s(szBuffer, szImHit[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa != nullptr && (GetCharInfo()->pSpawn->mActorClient.Class != 5 || !strstr(szBuffer, "Death")))
				vImHit.push_back(szBuffer);
		}
	}
	PSPELL pSpell = nullptr;
	for (unsigned int i = 0; szImHit[i] != nullptr; i++)
	{
		sprintf_s(szBuffer, "%s Rk. III", szImHit[i]);
		pSpell = GetSpellByName(szBuffer);
		if (pSpell != NULL && pSpell != nullptr)
		{
			if (CACheck(pSpell->ID))
			{
				vImHit.push_back(szBuffer);
				continue;
			}
		}
		sprintf_s(szBuffer, "%s Rk. II", szImHit[i]);
		pSpell = GetSpellByName(szBuffer);
		if (pSpell != NULL && pSpell != nullptr)
		{
			if (CACheck(pSpell->ID))
			{
				vImHit.push_back(szBuffer);
				continue;
			}
		}
		strcpy_s(szBuffer, szImHit[i]);
		pSpell = GetSpellByName(szBuffer);
		if (pSpell != NULL && pSpell != nullptr)
		{
			if (CACheck(pSpell->ID))
			{
				vImHit.push_back(szBuffer);
				continue;
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "ImHitIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("ImHitIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vImHit.size(); i++)
	{
		sprintf_s(szSpell, "ImHitIf%d", i);
		WriteChatf("\awImHit%d: \ay%s", i, vImHit[i].c_str());
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("ImHitIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateInvis()
{
	if (!InGameOK())
		return;
	vInvis.clear();
	PCHAR szInvis[] = { "Group Perfected Invisibility", "Group Silent Presence", "Group Spirit Walk", "Shared Camouflage",
		"Innate Camouflage", "Cover Tracks", "Silent Presence", "Spirit Walk", "Cloak of Shadows",
		"Shauri's Sonorious Clouding", "Gather Shadows", "Improved Natural Invisibility", NULL };
	PCHARINFO pChar = GetCharInfo();
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szInvis[i]; i++)
	{
		strcpy_s(szBuffer, szInvis[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa)
			{
				vInvis.push_back(szBuffer);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < vInvis.size(); i++)
	{
		WriteChatf("\awInvisSpell: \a-w%s", vInvis[i].c_str());
	}
}

void CreateInvisUndead()
{
	if (!InGameOK())
		return;
	vInvisUndead.clear();
	PCHAR szInvisUndead[] = { "Group Perfected Invisibility to Undead", "Innate Invis to Undead", "Cloak of Light", "Fool the Fallen", NULL };
	PCHARINFO pChar = GetCharInfo();
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szInvisUndead[i]; i++)
	{
		strcpy_s(szBuffer, szInvisUndead[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa)
			{
				vInvisUndead.push_back(szBuffer);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < vInvisUndead.size(); i++)
	{
		WriteChatf("\awIVUSpell: \a-w%s", vInvisUndead[i].c_str());
	}
}

void CreateItem()
{
	if (!InGameOK())
		return;
	vItems.clear();
	vItemsSpell.clear();
	PCHAR szItems[] = { "Aurora, the Heartwood Blade", "Crafted Talisman of Fates", "Innoruuk's Dark Blessing",
		"Heartwood Blade", "Fistwraps of Celestial Discipline", "Fatestealer", "Deathwhisper",
		"Champion's Sword of Eternal Power", "Blessed Spiritstaff of the Heyokah", "Blade of Vesagran",
		"Staff of Living Brambles", "Staff of Everliving Brambles", "Soulwhisper", "Redemption",
		"Raging Taelosian Alloy Axe", "Prismatic Dragon Blade", "Nightshade, Blade of Entropy",
		"Nightbane, Sword of the Valiant", "Kreljnok's Sword of Eternal Power", "Innoruuk's Voice",
		"Staff of Prismatic Power", "Transcended Fistwraps of Immortality", "Vengeful Taelosian Blood Axe",
		"Staff of Phenomenal Power", "Blightbringer's Tunic of the Grave", "Blood Drinker's Coating",
		"Aegis of Superior Divinity", "Harmony of the Soul", "Savage Lord's Totem","Spiritcaller Totem of the Feral",  NULL }; //
	char test[MAX_STRING];
	strcpy_s(test, "${If[${Me.Inventory[17].CastTime.Float}>0 && ${Me.Inventory[17].Spell.Name.NotEqual[Invigorate]},1,0]}");// , szItems[i]);
	ParseMacroData(test, MAX_STRING);
	if (atoi(test) == 1 && GetCharInfo()->pSpawn->Level >= 50)
	{
		strcpy_s(test, "${Me.Inventory[17].Name}");
		ParseMacroData(test, MAX_STRING);
		if (strstr(test, "Mana Robe"))
		{
			vManaName.push_back(test);
			strcpy_s(test, "${Me.Inventory[17].Spell.ID}");
			ParseMacroData(test, MAX_STRING);
			if (GetSpellByID(atol(test)))
				vMana.push_back(GetSpellByID(atol(test)));
		}
		else
		{
			vItems.push_back(test);
			strcpy_s(test, "${Me.Inventory[17].Spell.ID}");
			ParseMacroData(test, MAX_STRING);
			if (GetSpellByID(atol(test)))
				vItemsSpell.push_back(GetSpellByID(atol(test)));
		}
	}
	for (unsigned int i = 0; szItems[i]; i++)
	{
		sprintf_s(test, "${If[${FindItem[=%s].ItemSlot} && ${FindItem[=%s].Spell.ID},1,0]}", szItems[i], szItems[i]);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			vItems.push_back(szItems[i]);
			sprintf_s(test, "${FindItem[=%s].Spell.ID}", szItems[i]);
			ParseMacroData(test, MAX_STRING);
			if (GetSpellByID(atol(test)))
				vItemsSpell.push_back(GetSpellByID(atol(test)));
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "ClickyNukeIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("ClickyNukeIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vItems.size(); i++)
	{
		sprintf_s(szSpell, "ClickyNukeIf%d", i);
		WriteChatf("\awClickyNuke%d: \a-o%s - %s", i, vItems[i].c_str(), vItemsSpell[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("ClickyNukeIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateJolt()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vJolt.clear();
	vJoltName.clear();
	PCHAR szJolt[] = { "Mind Crash", "Concussive Intuition", "Improved Death Peace", "Arcane Whisper", "Playing Possum", "Soothing Words",
		"Roar of Thunder", "Sleight of Hand", "Death Peace", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szJolt[i]; i++)
	{
		strcpy_s(szBuffer, szJolt[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vJolt.push_back(GetSpellByID(aa->SpellID));
				vJoltName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == 5
				&& (GetSpellBaseX(pSpell, 0) < 0) || strstr(pSpell->Name, "freeze"))
			{
				PSPELL pSpell2 = GetSpellByID(GetSpellBase2X(pSpell, 1));
				if (pSpell2 && ((GetSpellAttribX(pSpell2, 2) == 192) || strstr(pSpell->Name, "Concussive "))
					&& GetCharInfo()->pSpawn->mActorClient.Class == Wizard)
				{
					vJolt.push_back(pSpell);
					vJoltName.push_back(pSpell->Name);
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "JoltIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("JoltIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vJolt.size(); i++)
	{
		sprintf_s(szSpell, "JoltIf%d", i);
		WriteChatf("\awJolt%d: \at%s", i, vJolt[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("JoltIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateKnockback()
{
	if (!InGameOK())
		return;
	vKnockback.clear();
	vKnockbackName.clear();
	PCHAR szKnockback[] = { "Spiritual Rebuke", "Repel the Wicked", "Silent Displacement", "Sonic Displacement", "Paralytic Spores",
		"Virulent Paralysis", "Wall of Wind", "Press the Attack", "Unbridled Strike of Fear",
		"Beam of Displacement", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szKnockback[i]; i++)
	{
		strcpy_s(szBuffer, szKnockback[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vKnockback.push_back(GetSpellByID(aa->SpellID));
				vKnockbackName.push_back(szBuffer);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "KnockbackIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("KnockbackIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vKnockback.size(); i++)
	{
		sprintf_s(szSpell, "KnockbackIf%d", i);
		WriteChatf("\awKnockback%d: \a-g%s", i, vKnockback[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("KnockbackIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateLevitate()
{
	if (!InGameOK())
		return;
	vLevitate.clear();
	PCHAR szLevitate[] = { "Perfected Dead Men Floating", "Flight of Eagles", "Perfected Dead Man Floating", "Spirit of Eagles",
		"Group Perfected Levitation", "Perfected Levitation", "Spirit of Eagle", "Group Spirit Walk", "Spirit Walk",
		"Dead Man Floating", NULL };
	PCHARINFO pChar = GetCharInfo();
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szLevitate[i]; i++)
	{
		strcpy_s(szBuffer, szLevitate[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa)
			{
				vLevitate.push_back(szBuffer);
				break;
			}
		}
	}
	for (unsigned int i = 0; i < vLevitate.size(); i++)
	{
		WriteChatf("\awLevitate: \a-w%s", vLevitate[i].c_str());
	}
}

void CreateLifetap()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vLifetap.clear();
	vLifetapName.clear();
	PCHAR szLifetap[] = { "Leech Touch", "Vicious Bite of Chaos", "Dying Grasp", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szLifetap[i]; i++)
	{
		strcpy_s(szBuffer, szLifetap[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vLifetap.push_back(GetSpellByID(aa->SpellID));
				vLifetapName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (strstr(pSpell->Name, "Dichotomic Fang") || pSpell->Category == 114 && (pSpell->Subcategory == 43 || pSpell->Subcategory == 33))
			{
				vLifetap.push_back(pSpell);
				vLifetapName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "LifetapIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("LifetapIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vLifetap.size(); i++)
	{
		sprintf_s(szSpell, "LifetapIf%d", i);
		WriteChatf("\awLifetap%d: \ag%s", i, vLifetap[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("LifetapIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateMainTankBuff()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vMainTankBuff.clear();
	vMainTankBuffName.clear();
	PCHAR szMainTankBuff[] = { "Divine Guardian", "Spirit Guardian", "Focused Celestial Regeneration", "Swirl of Fireflies", "Blade Guardian", NULL };
	PCHARINFO pChar = GetCharInfo();
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szMainTankBuff[i]; i++)
	{
		strcpy_s(szBuffer, szMainTankBuff[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vMainTankBuff.push_back(GetSpellByID(aa->SpellID));
				vMainTankBuffName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->TargetType != 6)
			{
				if (pSpell->Subcategory == 21 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 51 || pSpell->Subcategory == 16 && pSpell->Category != 132 || pSpell->Subcategory == 62 || pSpell->Subcategory == 64 && strstr(pSpell->Name, " Retort") || pSpell->Category == 45 && pSpell->Subcategory == 141 || pSpell->Subcategory == 64 && strstr(pSpell->Name, "Divine I"))
				{
					vMainTankBuff.push_back(pSpell);
					vMainTankBuffName.push_back(pSpell->Name);
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "MaintTankIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("MainTankIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vMainTankBuff.size(); i++)
	{
		sprintf_s(szSpell, "MainTankBuffIf%d", i);
		WriteChatf("\awMainTankBuff%d: \ag%s", i, vMainTankBuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("MainTankBuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateMana()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vMana.clear();
	vManaName.clear();
	PCHAR szMana[] = { "Consumption of Spirit", "Harvest of Druzzil", "Quiet Miracle", "Cannibalization", "Gather Mana", "Mana Draw",
		"Death Bloom", "Mana Reserve", "Fundament: Second Spire of Enchantment", "Veturika's Perseverance", "Death Blossom",
		"Paragon of Spirit", "Focused Paragon of Spirits", "Thought Leech", "Marr's Gift", "Rallying Call", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szMana[i]; i++)
	{
		strcpy_s(szBuffer, szMana[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vMana.push_back(GetSpellByID(aa->SpellID));
				vManaName.push_back(szMana[i]);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 61 || pSpell->Subcategory == 17 && FindSpellInfoForAttrib(pSpell, BASE, 15) > 0 || strstr(pSpell->Name, " Symbiosis"))
			{
				vMana.push_back(pSpell);
				vManaName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "ManaIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("ManaIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vMana.size(); i++)
	{
		sprintf_s(szSpell, "ManaIf%d", i);
		WriteChatf("\awMana%d: \ao%s", i, vManaName[i].c_str());
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("ManaIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateMez()
{
	if (!InGameOK())
		return;
	vMez.clear();
	vMezName.clear();
	PCHAR szMez[] = { "Dirge of the Sleepwalker", "Nightmare Stasis", "Beam of Slumber", "Wave of Slumber", "Dead Mesmerization",
		"Scintillating Beam", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szMez[i]; i++)
	{
		strcpy_s(szBuffer, szMez[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vMez.push_back(GetSpellByID(aa->SpellID));
				vMezName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 35)
			{
				vMez.push_back(pSpell);
				vMezName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "MezIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("MezIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vMez.size(); i++)
	{
		sprintf_s(szSpell, "MezIf%d", i);
		WriteChatf("\awMez%d: \ap%s", i, vMez[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("MezIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateModrodSpells()
{
	//? Removing this, as it's a dynamic check now in CheckMana(). All we do here is load the conditionals
	char szTemp[MAX_STRING];
	if (GetPrivateProfileString(INISection, "ModRodSelfIf", NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>("ModRodSelfIf", szTemp));
		SpellIt = SpellIf.find("ModRodSelfIf");
		if (SpellIt != SpellIf.end())
			WriteChatf("ModRodSelfIf: %s", SpellIt->second.c_str());
	}
	if (GetPrivateProfileString(INISection, "ModRodMassIf", NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>("ModRodMassIf", szTemp));
		SpellIt = SpellIf.find("ModRodMassIf");
		if (SpellIt != SpellIf.end())
			WriteChatf("ModRodMassIf: %s", SpellIt->second.c_str());
	}
	if (GetPrivateProfileString(INISection, "ModRodAltIf", NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>("ModRodAltIf", szTemp));
		SpellIt = SpellIf.find("ModRodAltIf");
		if (SpellIt != SpellIf.end())
			WriteChatf("ModRodAltIf: %s", SpellIt->second.c_str());
	}
#if (0)
	if (!InGameOK())
		return;
	if (GetCharInfo()->pSpawn->mActorClient.Class != 13) // mage
		return;
	char modrod[MAX_STRING] = { NULL };
	modrodAlt[0] = NULL;
	modrod[0] = NULL;
	modrod1[0] = NULL;
	modrod2[0] = NULL;
	modrodSpell1 = NULL;
	modrodSpell2 = NULL;
	if (GetCharInfo()->pSpawn->mActorClient.Class == 13)
	{
		//? SELF (1)
		for (unsigned int i = (sizeof(ModRods) / sizeof(ModRods[0])) - 1; i >= 0; i--)
		{
			if (ModRods[i].ModRodType != MOD_SPELL || ModRods[i].ModRodTarget != MOD_SELF)
				continue;
			sprintf_s(modrod, "${If[${Me.Book[%s Rk. III]},1,0]}", ModRods[i].szSpell);
			ParseMacroData(modrod, MAX_STRING);
			if (atoi(modrod) == 1)
			{
				sprintf_s(modrod1, "%s Rk. III", ModRods[i].szItem);
				sprintf_s(modrod, "%s Rk. III", ModRods[i].szSpell);
				modrodSpell1 = GetSpellByName(modrod);
				break;
			}
			else
			{
				sprintf_s(modrod, "${If[${Me.Book[%s Rk. II]},1,0]}", ModRods[i].szSpell);
				ParseMacroData(modrod, MAX_STRING);
				if (atoi(modrod) == 1)
				{
					sprintf_s(modrod1, "%s Rk. II", ModRods[i].szItem);
					sprintf_s(modrod, "%s Rk. II", ModRods[i].szSpell);
					modrodSpell1 = GetSpellByName(modrod);
					break;
				}
				else
				{
					sprintf_s(modrod, "${If[${Me.Book[%s]},1,0]}", ModRods[i].szSpell);
					ParseMacroData(modrod, MAX_STRING);
					if (atoi(modrod) == 1)
					{
						strcpy_s(modrod1, ModRods[i].szItem);
						strcpy_s(modrod, ModRods[i].szSpell);
						modrodSpell1 = GetSpellByName(modrod);
						break;
					}
				}
			}
		}

		//? MASS (2)
		for (unsigned int i = (sizeof(ModRods) / sizeof(ModRods[0])) - 1; i >= 0; i--)
		{
			if (ModRods[i].ModRodType != MOD_SPELL || ModRods[i].ModRodTarget != MOD_MASS)
				continue;
			sprintf_s(modrod, "${If[${Me.Book[%s Rk. III]},1,0]}", ModRods[i].szSpell);
			ParseMacroData(modrod, MAX_STRING);
			if (atoi(modrod) == 1)
			{
				sprintf_s(modrod2, "%s Rk. III", ModRods[i].szItem);
				sprintf_s(modrod, "%s Rk. III", ModRods[i].szSpell);
				modrodSpell2 = GetSpellByName(modrod);
				break;
			}
			else
			{
				sprintf_s(modrod, "${If[${Me.Book[%s Rk. II]},1,0]}", ModRods[i].szSpell);
				ParseMacroData(modrod, MAX_STRING);
				if (atoi(modrod) == 1)
				{
					sprintf_s(modrod2, "%s Rk. II", ModRods[i].szItem);
					sprintf_s(modrod, "%s Rk. II", ModRods[i].szSpell);
					modrodSpell2 = GetSpellByName(modrod);
					break;
				}
				else
				{
					sprintf_s(modrod, "${If[${Me.Book[%s]},1,0]}", ModRods[i].szSpell);
					ParseMacroData(modrod, MAX_STRING);
					if (atoi(modrod) == 1)
					{
						strcpy_s(modrod2, ModRods[i].szItem);
						strcpy_s(modrod, ModRods[i].szSpell);
						modrodSpell2 = GetSpellByName(modrod);
						break;
					}
				}
			}
		}

		//? AA (Alt)
		for (unsigned int i = (sizeof(ModRods) / sizeof(ModRods[0])) - 1; i >= 0; i--)
		{
			if (ModRods[i].ModRodType != MOD_AA)
				continue;
			sprintf_s(modrod, "${If[${Me.AltAbility[%s]},1,0]}", ModRods[i].szSpell);
			ParseMacroData(modrod, MAX_STRING);
			if (atoi(modrod) == 1)
			{
				strcpy_s(modrodAlt, ModRods[i].szItem);
				break;
			}
		}

		WriteChatf("\awModRod for self: \ao%s", modrodSpell1 ? modrodSpell1->Name : "None");
		WriteChatf("\awModRod for others: \ao%s", modrodSpell2 ? modrodSpell2->Name : "None");
		WriteChatf("\awAlt ModRod \ao%s", (strlen(modrodAlt) > 0) ? modrodAlt : "None");
	}
#endif
}

void CreateNuke()
{
	if (!InGameOK())
		return;
	vNuke.clear();
	vNukeName.clear();
	int NukeOrder = 0;
	PCHAR szNuke[] = { "Force of Will", "Force of Flame", "Force of Ice", "Force of Elements", "Vicious Bite of Chaos", "Divine Stun",
		"Harm Touch", "Storm Strike", "Volatile Arrow", "Smite the Wicked", "Turn Undead",
		"Turn Summoned", "Disruptive Persecution", "Fire Storm", "Force of Disruption", "Mana Burn", NULL };

	char szBuffer[MAX_STRING];
	if (GetCharInfo()->pSpawn->mActorClient.Class == 3)
		NukeOrder = 1;
	if (!BardClass && !NukeOrder)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (strstr(pSpell->Name, "Dichotomic Fusillade") || strstr(pSpell->Name, "Dichotomic Force") || strstr(pSpell->Name, "Dichotomic Fire") || pSpell->Category == 25 && pSpell->Category != 35)
				{
					vNuke.push_back(pSpell);
					vNukeName.push_back(pSpell->Name);
				}
			}
		}
	}
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szNuke[i]; i++)
	{
		strcpy_s(szBuffer, szNuke[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vNuke.push_back(GetSpellByID(aa->SpellID));
				vNukeName.push_back(szBuffer);
			}
		}
	}
	if (!BardClass && NukeOrder)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (pSpell->Category == 25)
				{
					vNuke.push_back(pSpell);
					vNukeName.push_back(pSpell->Name);
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "NukeIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("NukeIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vNuke.size(); i++)
	{
		sprintf_s(szSpell, "NukeIf%d", i);
		WriteChatf("\awNuke%d: \ar%s", i, vNuke[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("NukeIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateNukeToT()
{
	if (!InGameOK())
		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vNukeToT.clear();
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell && pSpell->Autocast == 19780)
		{
			if (pSpell->TargetType == 46 && FindSpellInfoForAttrib(pSpell, BASE, 0) > 0)
			{
				vNukeToT.push_back(pSpell);
			}
			else if (pSpell->TargetType == 5)
			{
				for (int slot = 0; slot < GetSpellNumEffects(pSpell); ++slot)
				{
					if (GetSpellAttrib(pSpell, slot) == 374)
					{
						PSPELL pSpell2 = GetSpellByID(GetSpellBase2(pSpell, slot));

						if (pSpell2 && GetSpellAttribX(pSpell2, 0) == 0
							&& GetSpellBase(pSpell2, 0) > 0
							&& pSpell2->TargetType == 46)
						{
							vNukeToT.push_back(pSpell);
							break;
						}
					}
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "NukeToTIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("NukeToTIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vNukeToT.size(); i++)
	{
		sprintf_s(szSpell, "NukeToTIf%d", i);
		WriteChatf("\awNukeToT%d: \at%s", i, vNukeToT[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("NukeToTIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreatePetBuff()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vPetBuff.clear();
	vPetBuffName.clear();
	vPet.clear();
	petSpell = NULL;
	PCHAR szPetBuff[] = { "Aegis of Kildrukaun", "Second Wind Ward", "Host in the Shell", "Hobble of Spirits", "Velocity", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szPetBuff[i]; i++)
	{
		strcpy_s(szBuffer, szPetBuff[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vPetBuff.push_back(GetSpellByID(aa->SpellID));
				vPetBuffName.push_back(szBuffer);
			}
		}
	}
	int petSpellLevel = 0, spirit = 0, iceflame = 0, aggression = 0, haste = 0, warder = 0, growl = 0;
	PSPELL ppetSpell, pspirit, piceflame, paggression, phaste, pwarder, pgrowl;
	customPetSpell = NULL;
	if (strlen(CustomPetSpell) > 5)
		if (GetSpellByName(CustomPetSpell))
		{
			customPetSpell = GetSpellByName(CustomPetSpell);
			petSpell = customPetSpell;
			vPet.push_back(petSpell);
		}
	for (unsigned int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
	{
		if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
		{
			if (pSpell->Category == 69 && pSpell->Subcategory>97 && pSpell->Subcategory < 106 && pSpell->Subcategory != 101 && GetCharInfo()->pSpawn->mActorClient.Class != 12 && GetCharInfo()->pSpawn->mActorClient.Class != 2 && GetCharInfo()->pSpawn->mActorClient.Class != 6)
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > petSpellLevel && !customPetSpell)
				{
					petSpellLevel = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					ppetSpell = pSpell;
					petSpell = pSpell;
				}
			}
			if (GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) < 3)
				continue;
			if (pSpell->Category == 69 && pSpell->Subcategory == 71 && strstr(pSpell->Name, "Spirit of "))
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > spirit)
				{
					spirit = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					pspirit = pSpell;
				}
			}
			if (pSpell->Category == 69 && pSpell->Subcategory == 71 && strstr(pSpell->Name, "Empowered Minion"))
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > spirit)
				{
					spirit = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					pspirit = pSpell;
				}
			}
			if (pSpell->Category == 69 && pSpell->Subcategory == 70)
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > haste)
				{
					haste = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					phaste = pSpell;
				}
			}
			if (pSpell->Category == 69 && pSpell->Subcategory == 71 && strstr(pSpell->Name, " Warder"))
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > warder)
				{
					warder = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					pwarder = pSpell;
				}
			}
			/*
			if(pSpell->Category==69 && pSpell->Subcategory==71 && strstr(pSpell->Name,"Growl of the"))
			{
			if(pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]>growl)
			{
			growl=pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
			pgrowl=pSpell;
			}
			}
			*/
			if (pSpell->Category == 69 && pSpell->Subcategory == 71 && strstr(pSpell->Name, " Aggression"))
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > aggression)
				{
					aggression = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					paggression = pSpell;
				}
			}
			if (pSpell->Category == 69 && pSpell->Subcategory == 71 && strstr(pSpell->Name, "Iceflame"))
			{
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > iceflame)
				{
					iceflame = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
					piceflame = pSpell;
				}
			}
		}
	}

	if (petSpellLevel > 0)
		vPet.push_back(ppetSpell);
	if (haste)
	{
		vPetBuff.push_back(phaste);
		vPetBuffName.push_back(phaste->Name);
	}
	if (growl)
	{
		vPetBuff.push_back(pgrowl);
		vPetBuffName.push_back(pgrowl->Name);
	}
	if (iceflame)
	{
		vPetBuff.push_back(piceflame);
		vPetBuffName.push_back(piceflame->Name);
	}
	if (warder)
	{
		vPetBuff.push_back(pwarder);
		vPetBuffName.push_back(pwarder->Name);
	}
	if (aggression)
	{
		vPetBuff.push_back(paggression);
		vPetBuffName.push_back(paggression->Name);
	}
	if (spirit)
	{
		vPetBuff.push_back(pspirit);
		vPetBuffName.push_back(pspirit->Name);
	}
	if (!BardClass)
	{
		PSPELL pSpell;
		for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
		{
			pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
			if (pSpell)
			{
				if (pSpell->Category == 69 && pSpell->Subcategory == 10)
				{
					vPetBuff.push_back(pSpell);
					vPetBuffName.push_back(pSpell->Name);
				}
			}
		}
	}
	if (vPet.size())
	{
		WriteChatf("\awPetSpell: \a-t%s", vPet[0]->Name);
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "PetBuffIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("PetBuffIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vPetBuff.size(); i++)
	{
		sprintf_s(szSpell, "PetBuffIf%d", i);
		WriteChatf("\awPetBuff%d: \a-t%s", i, vPetBuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("PetBuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}
	if (!strstr(PetIllusionItem, "NULL"))
	{
		char test[MAX_STRING];
		sprintf_s(test, "${If[${FindItem[%s].ItemSlot},1,0]}", PetIllusionItem);
		ParseMacroData(test, MAX_STRING);
		if (atoi(test) == 1)
		{
			sprintf_s(test, "${FindItem[%s].Spell.Name}", PetIllusionItem);
			ParseMacroData(test, MAX_STRING);
			if (GetSpellByName(test))
			{
				PetIllusionSpell = GetSpellByName(test);
				WriteChatf("\awPet Illusion: \a-t%s", PetIllusionSpell->Name);
			}
		}
	}
}

void CreatePetGear()
{
	if (!InGameOK())
		return;
	vPetGearNeed.clear();
	vPetGearHave.clear();
	for (unsigned int i = 1; i < 24; i++)
	{
		vPetGearNeed.push_back("0");
		vPetGearHave.push_back("0");
	}
	char test[MAX_STRING], test2[MAX_STRING];
	for (unsigned int slot = BAG_SLOT_START; slot < NUM_INV_SLOTS; slot++)
	{
		if (PCONTENTS pTempItem = GetCharInfo2()->pInventoryArray->InventoryArray[slot])
		{
			if (GetItemFromContents(pTempItem)->Type == ITEMTYPE_PACK && pTempItem->Contents.ContainedItems.pItems)
			{
				for (DWORD pslot = 0; pslot < (GetItemFromContents(pTempItem)->Slots); pslot++)
				{
					if (pTempItem->GetContent(pslot))
					{
						if (PCONTENTS pSlotItem = pTempItem->GetContent(pslot))
						{
							if ((!strcmp(GetItemFromContents(pSlotItem)->Name, "Brightedge") || !GetItemFromContents(pSlotItem)->NoRent) && !strstr(GetItemFromContents(pSlotItem)->Name, "Rod") && !strstr(GetItemFromContents(pSlotItem)->Name, " Shard") && !strstr(GetItemFromContents(pSlotItem)->Name, "Wand of"))
							{
								//WriteChatf("%s", GetItemFromContents(pSlotItem)->Name);
								sprintf_s(test, "${If[${FindItem[%s].WornSlot[1]},${FindItem[%s].WornSlot[1]},0]}", GetItemFromContents(pSlotItem)->Name, GetItemFromContents(pSlotItem)->Name);
								ParseMacroData(test, MAX_STRING);
								if (atoi(test) > 0)
								{
									sprintf_s(test2, "${If[${FindItem[%s].WornSlot[2]},${FindItem[%s].WornSlot[2]},0]}", GetItemFromContents(pSlotItem)->Name, GetItemFromContents(pSlotItem)->Name);
									ParseMacroData(test2, MAX_STRING);
									if (strstr(vPetGearNeed[atoi(test)].c_str(), "0"))
									{
										vPetGearNeed[atoi(test)] = GetItemFromContents(pSlotItem)->Name;
									}
									else
									{
										if (!strstr(vPetGearNeed[atoi(test)].c_str(), "0") && atoi(test2) == 14 && strstr(vPetGearNeed[14].c_str(), "0"))
											vPetGearNeed[14] = GetItemFromContents(pSlotItem)->Name;
									}
								}
							}
						}
					}
				}
			}
			else {
				if (!GetItemFromContents(pTempItem)->NoRent && !strstr(GetItemFromContents(pTempItem)->Name, "Rod") && !strstr(GetItemFromContents(pTempItem)->Name, " Shard") && !strstr(GetItemFromContents(pTempItem)->Name, "Wand of"))
				{
					sprintf_s(test, "${If[${FindItem[%s].WornSlot[1]},${FindItem[%s].WornSlot[1]},0]}", GetItemFromContents(pTempItem)->Name, GetItemFromContents(pTempItem)->Name);
					ParseMacroData(test, MAX_STRING);
					if (atoi(test) > 0)
					{
						sprintf_s(test2, "${If[${FindItem[%s].WornSlot[2]},${FindItem[%s].WornSlot[2]},0]}", GetItemFromContents(pTempItem)->Name, GetItemFromContents(pTempItem)->Name);
						ParseMacroData(test2, MAX_STRING);
						if (strstr(vPetGearNeed[atoi(test)].c_str(), "0"))
						{
							vPetGearNeed[atoi(test)] = GetItemFromContents(pTempItem)->Name;
						}
						else
						{
							if (!strstr(vPetGearNeed[atoi(test)].c_str(), "0") && atoi(test2) == 14 && strstr(vPetGearNeed[14].c_str(), "0"))
								vPetGearNeed[14] = GetItemFromContents(pTempItem)->Name;
						}
					}
				}
			}
		}
	}
	petGearSize = 0;
	for (unsigned int i = 0; i < vPetGearNeed.size(); i++)
	{
		if (!strstr(vPetGearNeed[i].c_str(), "0"))
			petGearSize++;
		//vPetGearNeed.erase[i];
	}
	WriteChatf("Found %d items in inventory to give to pet when within 20'", petGearSize);
	/*
	for (unsigned int i= 0; i < vPetGearNeed.size(); i++)
	{
	if (!strstr(vPetGearNeed[i].c_str(), "0"))
	WriteChatf("\a-g%s", vPetGearNeed[i].c_str());
	}
	*/
}

void CreatePull()
{
	char szTemp[MAX_STRING];
	strcpy_s(pullif, "PullIf");
	if (GetPrivateProfileString(INISection, pullif, NULL, szTemp, MAX_STRING, INIFileName) != NULL)
	{
		SpellIf.insert(make_pair<string, string>(pullif, szTemp));
		SpellIt = SpellIf.find(pullif);
		if (SpellIt != SpellIf.end())
			WriteChatf("PullIf: %s", SpellIt->second.c_str());
	}
	if (PSPELL pSpell = GetSpellByName(PullSkill))
	{
		PullSkillDistance = (int)pSpell->Range;
		pPullSpell = pSpell;
	}
	else
	{
		PALTABILITY aa;
		if (GetAAIndexByName(PullSkill))
		{
			aa = pAltAdvManager->GetAAById(GetAAIndexByName(PullSkill));
			PullSkillDistance = (int)GetSpellByID(aa->SpellID)->Range;
			pPullSpell = GetSpellByID(aa->SpellID);
		}
	}
	if (strlen(PullSkill) > 0)
		WriteChatf("PullSkillName: %s, Distance: %d", PullSkill, PullSkillDistance);

}

void CreateRez()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vRez.clear();
	vRezName.clear();
	vRezTimeStamp.clear();
	PCHAR szRez[] = { "Gift of Resurrection", "Restoration of Life", "Blessing of Resurrection", "Convergence", "Rejuvenation of Spirit",  NULL };
	PCHAR szRez0[] = { "Call of the Wild", "Call of the Herald", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szRez[i]; i++)
	{
		strcpy_s(szBuffer, szRez[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vRez.push_back(GetSpellByID(aa->SpellID));
				vRezName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 82)
			{
				vRez.push_back(pSpell);
				vRezName.push_back(pSpell->Name);
			}
		}
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		vRezTimeStamp.push_back(0);
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "RezIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("RezIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vRez.size(); i++)
	{
		sprintf_s(szSpell, "RezIf%d", i);
		WriteChatf("\awRez%d: \ao%s", i, vRez[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("RezIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateRoot()
{
	if (!InGameOK())
		return;
	if (BardClass)
		return;
	vRoot.clear();
	vRootName.clear();
	PCHAR szRoot[] = { "Grasp of Sylvan Spirits", "Virulent Paralysis", "Shackles of Tunare", "Beguiler's Directed Banishment",
		"Strong Root", "Paralytic Spores", "Pestilent Paralysis", "Blessed Chains", "Frost Shackles", "Paralytic Spray",
		"Binding Axe", NULL };

	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szRoot[i]; i++)
	{
		strcpy_s(szBuffer, szRoot[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vRoot.push_back(GetSpellByID(aa->SpellID));
				vRootName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 83)
			{
				vRoot.push_back(pSpell);
				vRootName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "RootIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("RootIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vRoot.size(); i++)
	{
		sprintf_s(szSpell, "RootIf%d", i);
		WriteChatf("\awRoot%d: \a-g%s", i, vRoot[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("RootIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateSelfBuff()
{
	if (!InGameOK())
		return;
	vSelfBuff.clear();
	vSelfBuffName.clear();
	vBuff.clear();
	PCHAR szSelfBuff[] = { "Preincarnation", "Selo's Sonata", "Sionachie's Crescendo", "Eldritch Rune", "Voice of Thule",
		"Group Pact of the Wolf", "Elemental Form: Fire", "Cascade of Decay", "Pact of the Wurine", "Talisman of Celerity", NULL
	}; //readd "Veil of Mindshadow" slot 11

	PCHAR szFamiliar[] = { "E'ci's Icy Familiar", "Druzzil's Mystical Familiar", "Ro's Flaming Familiar", "Improved Familiar",
		"Kerafyrm's Prismatic Familiar", NULL };
	PCHAR szMancy[] = { "Cryomancy", "Arcomancy", "Pyromancy", NULL };
	for (unsigned int i = 0; szSelfBuff[i]; i++)
	{
		for (unsigned long nAbility = 0; nAbility<AA_CHAR_MAX_REAL; nAbility++)
		{
			if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility)))
			{
				if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL))
				{
					if (!_stricmp(szSelfBuff[i], pName))
					{
						if (GetSpellByID(pAbility->SpellID))
						{
							vSelfBuff.push_back(GetSpellByID(pAbility->SpellID));
							vSelfBuffName.push_back(szSelfBuff[i]);
						}
					}
				}
			}
		}
	}
	if (!BardClass)
	{
		for (unsigned int i = 0; szFamiliar[i]; i++)
		{
			for (unsigned long nAbility = 0; nAbility<AA_CHAR_MAX_REAL; nAbility++)
			{
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility)))
				{
					if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL))
					{
						if (!_stricmp(szFamiliar[i], pName))
						{
							if (GetSpellByID(pAbility->SpellID))
							{
								vSelfBuff.push_back(GetSpellByID(pAbility->SpellID));
								vSelfBuffName.push_back(szFamiliar[i]);
							}
						}
					}
				}
			}
		}
		for (unsigned int i = 0; szMancy[i]; i++)
		{
			for (unsigned long nAbility = 0; nAbility<AA_CHAR_MAX_REAL; nAbility++)
			{
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility)))
				{
					if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL))
					{
						if (!_stricmp(szMancy[i], pName))
						{
							if (GetSpellByID(pAbility->SpellID))
							{
								vSelfBuff.push_back(GetSpellByID(pAbility->SpellID));
								vSelfBuffName.push_back(szMancy[i]);
							}
						}
					}
				}
			}
		}
		int skin = 0, aego = 0, symbol = 0, type2 = 0, shielding = 0, clarity = 0, se = 0, haste = 0, regen = 0, pred = 0, ds = 0,
			rune = 0, rune2 = 0, mana2 = 0, defensive = 0, shroud = 0, horror = 0, fero = 0, call = 0, convert = 0, killshot = 0;
		PSPELL	pskin, paego, psymbol, ptype2, pshielding, pclarity, pse, phaste, pregen, ppred, pds, prune, prune2, pmana2, pdefensive,
			pshroud, phorror, pfero, pcall, pconvert, pkillshot;
		for (unsigned int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
		{
			if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
			{
				int attrib0 = GetSpellAttrib(pSpell, 0);

				if (attrib0 != 33)
				{
					if ((strstr(pSpell->Name, "Shield of ") || (strstr(pSpell->Name, "Rallied") && strstr(pSpell->Name, "of Vie"))) && pSpell->Category == 125 && pSpell->Subcategory == 62)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > rune2)
						{
							rune2 = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							prune2 = pSpell;
						}
					}
					if (strstr(pSpell->Name, "Ferocity") && !strstr(pSpell->Name, "Sha's") && GetCharInfo()->pSpawn->mActorClient.Class == 15)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > fero)
						{
							fero = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pfero = pSpell;
						}
					}
					if (pSpell->Subcategory == 59 && pSpell->Category == 79 && pSpell->TargetType == 6 || pSpell->Subcategory == 17 && GetSpellAttribX(pSpell, 1) == 15 && GetSpellBaseX(pSpell, 1) > 0 && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 0
						|| pSpell->Subcategory == 59 && GetSpellAttribX(pSpell, 2) == 15 && GetSpellBaseX(pSpell, 2) > 2)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > mana2)
						{
							mana2 = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pmana2 = pSpell;
						}
					}
					if (GetCharInfo()->pSpawn->mActorClient.Class == 5 && !strstr(pSpell->Name, "Shroud of Hate") && !strstr(pSpell->Name, "Shroud of Pain") && (strstr(pSpell->Name, "Shroud of") || strstr(pSpell->Name, "Scream of Death") || strstr(pSpell->Name, "Black Shroud") || strstr(pSpell->Name, "Vampiric Embrace")) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 60)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > shroud)
						{
							shroud = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pshroud = pSpell;
						}
					}
					if (GetCharInfo()->pSpawn->mActorClient.Class == 5 && pSpell->Category == 125 && pSpell->Subcategory == 17 && GetSpellAttribX(pSpell, 1) == 2)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > call)
						{
							call = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pcall = pSpell;
						}
					}
					if ((GetCharInfo()->pSpawn->mActorClient.Class == 5 || GetCharInfo()->pSpawn->mActorClient.Class == 11) && pSpell->Category == 125 && pSpell->Subcategory == 17 && GetSpellAttribX(pSpell, 1) == 15)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > convert)
						{
							convert = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pconvert = pSpell;
						}
					}
					if (GetCharInfo()->pSpawn->mActorClient.Class == 5 && strstr(pSpell->Name, " Horror") && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 60)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > horror)
						{
							horror = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							phorror = pSpell;
						}
					}
					if (HasSpellAttrib(pSpell, 323) && GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) > 60)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > defensive)
						{
							defensive = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pdefensive = pSpell;
						}
					}
					if (pSpell->Subcategory == 84 && pSpell->Category == 125 && pSpell->ReagentID[0] == -1 && pSpell->TargetType == 6)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > rune)
						{
							rune = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							prune = pSpell;
						}
					}
					if (pSpell->Subcategory == 1 && pSpell->TargetType == 41 || strstr(pSpell->Name, "Heroic Bond"))
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > aego)
						{
							aego = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							paego = pSpell;
						}
					}
					if (pSpell->Subcategory == 46 && pSpell->TargetType == 41 && (GetCharInfo()->pSpawn->mActorClient.Class != 2 || GetCharInfo()->pSpawn->Level < 60) && (GetCharInfo()->pSpawn->mActorClient.Class != 3 || GetCharInfo()->pSpawn->Level < 70))
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > skin)
						{
							skin = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pskin = pSpell;
						}
					}
					if (pSpell->Subcategory == 87)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > shielding)
						{
							shielding = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pshielding = pSpell;
						}
					}
					if (pSpell->Subcategory == 59 && pSpell->Category == 79 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > clarity)
						{
							clarity = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pclarity = pSpell;
						}
					}
					if (pSpell->Subcategory == 44 && pSpell->Category == 79 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > se)
						{
							se = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pse = pSpell;
						}
					}
					if (pSpell->Subcategory == 43 && pSpell->Category == 79 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > regen)
						{
							regen = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pregen = pSpell;
						}
					}
					if (pSpell->Subcategory == 7 && pSpell->Category == 95 && pSpell->TargetType == 41 && GetCharInfo()->pSpawn->mActorClient.Class == 4)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > pred)
						{
							pred = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							ppred = pSpell;
						}
					}
					if (pSpell->Subcategory == 112 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > symbol)
						{
							symbol = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							psymbol = pSpell;
						}
					}
					if (pSpell->Subcategory == 47 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > type2)
						{
							type2 = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							ptype2 = pSpell;
						}
					}
					if (pSpell->Subcategory == 41 && pSpell->Category == 125 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > haste)
						{
							haste = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							phaste = pSpell;
						}
					}
					if (pSpell->Subcategory == 21 && pSpell->Category == 125 && pSpell->TargetType == 41)
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > ds)
						{
							ds = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pds = pSpell;
						}
					}
					if ((pSpell->Subcategory == 16 || pSpell->Subcategory == 64) && pSpell->Category == 125 && (strstr(pSpell->Name, "Pack of ") || strstr(pSpell->Name, "by the Hunt") || strstr(pSpell->Name, "Face of ") || strstr(pSpell->Name, "Remorse") || strstr(pSpell->Name, "Natural ")))
					{
						if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > killshot)
						{
							killshot = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
							pkillshot = pSpell;
						}
					}
				}
			}
		}
		if (shroud)
		{
			vSelfBuff.push_back(pshroud);
			vSelfBuffName.push_back(pshroud->Name);
		}
		if (horror)
		{
			vSelfBuff.push_back(phorror);
			vSelfBuffName.push_back(phorror->Name);
		}
		if (call)
		{
			vSelfBuff.push_back(pcall);
			vSelfBuffName.push_back(pcall->Name);
		}
		if (convert)
		{
			vSelfBuff.push_back(pconvert);
			vSelfBuffName.push_back(pconvert->Name);
		}
		if (fero)
		{
			vSelfBuff.push_back(pfero);
			vSelfBuffName.push_back(pfero->Name);
			vBuff.push_back(pfero);
			vBuffName.push_back(pfero->Name);
			SpellCategory(pfero);
			vBuffType.push_back(spellCat);
		}
		if (aego)
		{
			vSelfBuff.push_back(paego);
			vSelfBuffName.push_back(paego->Name);
			vBuff.push_back(paego);
			vBuffName.push_back(paego->Name);
			SpellCategory(paego);
			vBuffType.push_back(spellCat);
		}
		if (skin)
		{
			vSelfBuff.push_back(pskin);
			vSelfBuffName.push_back(pskin->Name);
			vBuff.push_back(pskin);
			vBuffName.push_back(pskin->Name);
			SpellCategory(pskin);
			vBuffType.push_back(spellCat);
		}
		if (symbol)
		{
			vSelfBuff.push_back(psymbol);
			vSelfBuffName.push_back(psymbol->Name);
			vBuff.push_back(psymbol);
			vBuffName.push_back(psymbol->Name);
			SpellCategory(psymbol);
			vBuffType.push_back(spellCat);
		}
		if (clarity)
		{
			vSelfBuff.push_back(pclarity);
			vSelfBuffName.push_back(pclarity->Name);
			vBuff.push_back(pclarity);
			vBuffName.push_back(pclarity->Name);
			SpellCategory(pclarity);
			vBuffType.push_back(spellCat);
		}
		if (se)
		{
			vSelfBuff.push_back(pse);
			vSelfBuffName.push_back(pse->Name);
			vBuff.push_back(pse);
			vBuffName.push_back(pse->Name);
			SpellCategory(pse);
			vBuffType.push_back(spellCat);
		}
		if (haste)
		{
			vSelfBuff.push_back(phaste);
			vSelfBuffName.push_back(phaste->Name);
			vBuff.push_back(phaste);
			vBuffName.push_back(phaste->Name);
			SpellCategory(phaste);
			vBuffType.push_back(spellCat);
		}
		if (shielding)
		{
			vSelfBuff.push_back(pshielding);
			vSelfBuffName.push_back(pshielding->Name);
			if (pshielding->TargetType == 41)
			{
				vBuff.push_back(pshielding);
				vBuffName.push_back(pshielding->Name);
				SpellCategory(pshielding);
				vBuffType.push_back("Focus");
			}
		}
		if (type2)
		{
			vSelfBuff.push_back(ptype2);
			vSelfBuffName.push_back(ptype2->Name);
			if (ptype2->TargetType == 41)
			{
				vBuff.push_back(ptype2);
				vBuffName.push_back(ptype2->Name);
				SpellCategory(ptype2);
				vBuffType.push_back(spellCat);
			}
		}
		if (regen)
		{
			vSelfBuff.push_back(pregen);
			vSelfBuffName.push_back(pregen->Name);
			vBuff.push_back(pregen);
			vBuffName.push_back(pregen->Name);
			SpellCategory(pregen);
			vBuffType.push_back(spellCat);
		}
		if (pred)
		{
			vSelfBuff.push_back(ppred);
			vSelfBuffName.push_back(ppred->Name);
			vBuff.push_back(ppred);
			vBuffName.push_back(ppred->Name);
			SpellCategory(ppred);
			vBuffType.push_back(spellCat);
		}
		if (ds)
		{
			vSelfBuff.push_back(pds);
			vSelfBuffName.push_back(pds->Name);
			if (pds->TargetType == 41)
			{
				vBuff.push_back(pds);
				vBuffName.push_back(pds->Name);
				SpellCategory(pds);
				vBuffType.push_back(spellCat);
			}
		}
		if (rune)
		{
			vSelfBuff.push_back(prune);
			vSelfBuffName.push_back(prune->Name);
		}
		if (rune2)
		{
			vSelfBuff.push_back(prune2);
			vSelfBuffName.push_back(prune2->Name);
		}
		if (mana2)
		{
			vSelfBuff.push_back(pmana2);
			vSelfBuffName.push_back(pmana2->Name);
		}
		if (defensive)
		{
			vSelfBuff.push_back(pdefensive);
			vSelfBuffName.push_back(pdefensive->Name);
		}
		if (killshot)
		{
			vSelfBuff.push_back(pkillshot);
			vSelfBuffName.push_back(pkillshot->Name);
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "SelfBuffIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("SelfBuffIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vSelfBuff.size(); i++)
	{
		sprintf_s(szSpell, "SelfBuffIf%d", i);
		WriteChatf("\awSelfBuff%d: \aw%s", i, vSelfBuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("SelfBuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}

	for (unsigned int i = 0; i < vBuff.size(); i++)
	{
		sprintf_s(szSpell, "BuffIf%d", i);
		WriteChatf("\awBuff%d: \aw%s", i, vBuff[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("BuffIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateSnare()
{
	if (!InGameOK())
		return;
	vSnare.clear();
	vSnareName.clear();
	PCHAR szSnare[] = { "Encroaching Darkness", "Entrap", "Atol's Unresistable Shackles", "Crippling Strike", "Atol's Shackles", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szSnare[i]; i++)
	{
		strcpy_s(szBuffer, szSnare[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vSnare.push_back(GetSpellByID(aa->SpellID));
				vSnareName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Subcategory == 89)
			{
				vSnare.push_back(pSpell);
				vSnareName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "SnareIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("SnareIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vSnare.size(); i++)
	{
		sprintf_s(szSpell, "SnareIf%d", i);
		WriteChatf("\awSnare%d: \a-g%s", i, vSnare[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("SnareIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void CreateSummonItem()
{
	if (!InGameOK())
		return;
	vSummonItems.clear();
	char test[MAX_STRING];
	long ltest;
	if (GetCharInfo()->pSpawn->mActorClient.Class == 14)
	{
		int aaIndex;
		_ALTABILITY* aa = nullptr;
		for (unsigned int i = 0; szSummonedEnc[i]; i++)
		{
			strcpy_s(test, szSummonedEnc[i]);
			aaIndex = GetAAIndexByName(test);
			if (aaIndex > 0)
			{
				aa = pAltAdvManager->GetAAById(aaIndex);
				if (aa && GetSpellByID(aa->SpellID))
					vSummonItems.push_back(GetSpellByID(aa->SpellID));
			}
		}
	}
	if (GetCharInfo()->pSpawn->mActorClient.Class == 13)
	{
		for (unsigned int nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
		{
			if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
			{
				if (GetSpellAttribX(pSpell, 0) == 32 && (int(pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]) + 5)>GetCharInfo()->pSpawn->Level)
				{
					for (unsigned int i = 0; szSummonedItemsID[i]; i++)
					{
						strcpy_s(test, szSummonedItemsID[i]);
						ltest = atol(test);
						if (GetSpellBaseX(pSpell, 0) == ltest)
							vSummonItems.push_back(pSpell);
					}
				}
			}
		}
		for (unsigned int i = 0; szCauldron[i]; i++)
		{
			sprintf_s(test, "${If[${FindItem[%s].ItemSlot},1,0]}", szCauldron[i]);
			ParseMacroData(test, MAX_STRING);
			if (atoi(test) == 1)
				cauldron = true;
		}
	}
	char szTemp[MAX_STRING], szSpell[MAX_STRING];
	strcpy_s(szSpell, "SummonItemIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("SummonItemIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vSummonItems.size(); i++)
	{
		WriteChatf("\awSummonItem%d: \a-o%s", i, vSummonItems[i]->Name);
	}
}

void CreateSwarm()
{
	if (!InGameOK())
		return;
	vSwarm.clear();
	vSwarmName.clear();
	PCHAR szSwarm[] = { "Phantasmal Opponent", "Army of the Dead", "Wake the Dead", "Swarm of Decay", "Rise of Bones", "Song of Stone", "Servant of Ro",
		"Host of the Elements", "Nature's Guardian", "Celestial Hammer", "Chattering Bones", "Call of Xuzl",
		"Spirit Call", "Attack of the Warders", "Illusory Ally", "Pack Hunt", "Spirits of Nature", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (unsigned int i = 0; szSwarm[i]; i++)
	{
		strcpy_s(szBuffer, szSwarm[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
			aa = pAltAdvManager->GetAAById(aaIndex);
			if (aa && GetSpellByID(aa->SpellID))
			{
				vSwarm.push_back(GetSpellByID(aa->SpellID));
				vSwarmName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (unsigned int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if ((FLOAT)pSpell->CastTime <= 3000.0)
			{
				if (!strstr(pSpell->Name, "Nourishing Growth") && !strstr(pSpell->Name, "Nurturing Growth"))
				{
					if (pSpell->Subcategory == 104 && pSpell->Category == 69 && strstr(pSpell->Extra, "Rk") || pSpell->Subcategory == 99 && (GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 2) || pSpell->Subcategory == 139)
					{
						vSwarm.push_back(pSpell);
						vSwarmName.push_back(pSpell->Name);
					}
				}
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	strcpy_s(szSpell, "SwarmIf");
	if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
	{
		SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
		SpellIt = SpellIf.find(szSpell);
		if (SpellIt != SpellIf.end())
			WriteChatf("SwarmIf: %s", SpellIt->second.c_str());
	}
	for (unsigned int i = 0; i < vSwarm.size(); i++)
	{
		sprintf_s(szSpell, "SwarmIf%d", i);
		WriteChatf("\awSwarm%d: \ar%s", i, vSwarm[i]->Name);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>(szSpell, szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("SwarmIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

//LOAD / UNLOAD SPELLS
void LoadBotSpell(vector<_BotSpells> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	for (unsigned int i = 0; i < v.size(); i++)
	{
		// move next 6 lines to the create spell vector
		sprintf_s(szSpell, "%sSpellName%d", VectorName, i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].SpellName, szTemp);
			v[i].CanIReprioritize = 0;
			WriteChatf("%s=%s", szSpell, szTemp);
		}
		// end move
		SpellCategory(v[i].Spell);
		strcpy_s(v[i].SpellCat, spellCat);
		sprintf_s(szSpell, "%sSpellIconName%d", VectorName, i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].SpellIconName, szTemp);
			WriteChatf("%s=%s", szSpell, szTemp);
		}
		sprintf_s(szSpell, "%sIf%d", VectorName, i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].If, szTemp);
			WriteChatf("%s=%s", szSpell, szTemp);
		}
		sprintf_s(szSpell, "%sGem%d", VectorName, i);
		if (GetPrivateProfileString(INISection, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			strcpy_s(v[i].Gem, szTemp);
			WriteChatf("%s=%s", szSpell, szTemp);
		}
		sprintf_s(szSpell, "%sUseOnce%d", VectorName, i);
		v[i].UseOnce = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].UseOnce);
		sprintf_s(szSpell, "%sForceCast%d", VectorName, i);
		v[i].ForceCast = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].ForceCast);
		sprintf_s(szSpell, "%sUse%d", VectorName, i);
		v[i].Use = GetPrivateProfileInt(INISection, szSpell, 1, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].Use);
		sprintf_s(szSpell, "%sStartAt%d", VectorName, i);
		v[i].StartAt = GetPrivateProfileInt(INISection, szSpell, 100, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].StartAt);
		sprintf_s(szSpell, "%sStopAt%d", VectorName, i);
		v[i].StopAt = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].StopAt);
		sprintf_s(szSpell, "%sNamedOnly%d", VectorName, i);
		v[i].NamedOnly = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].NamedOnly);
		sprintf_s(szSpell, "%sPriority%d", VectorName, i);
		v[i].Priority = GetPrivateProfileInt(INISection, szSpell, 0, INIFileName);
		WriteChatf("%s=%d", szSpell, v[i].Priority);
	}
}

void PopulateIni(vector<_BotSpells> &v, char VectorName[MAX_STRING])
{
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	for (unsigned int i = 0; i < v.size(); i++)
	{
		sprintf_s(szSpell, "%sTotal", VectorName);
		_itoa_s(v.size(), szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sSpellName%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].SpellName, INIFileName);
		sprintf_s(szSpell, "%sSpellIconName%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].SpellIconName, INIFileName);
		sprintf_s(szSpell, "%sIf%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].If, INIFileName);
		sprintf_s(szSpell, "%sGem%d", VectorName, i);
		WritePrivateProfileString(INISection, szSpell, v[i].Gem, INIFileName);
		sprintf_s(szSpell, "%sUseOnce%d", VectorName, i);
		_itoa_s(v[i].UseOnce, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sForceCast%d", VectorName, i);
		_itoa_s(v[i].ForceCast, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sUse%d", VectorName, i);
		_itoa_s(v[i].Use, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sStartAt%d", VectorName, i);
		_itoa_s(v[i].StartAt, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sStopAt%d", VectorName, i);
		_itoa_s(v[i].StopAt, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sNamedOnly%d", VectorName, i);
		_itoa_s(v[i].NamedOnly, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
		sprintf_s(szSpell, "%sPriority%d", VectorName, i);
		_itoa_s(v[i].Priority, szTemp, 10);
		WritePrivateProfileString(INISection, szSpell, szTemp, INIFileName);
	}
}

void loadbotCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	SpellIf.clear();
	SpawnTracker.clear();
	// Need to pass a buffer
	char szNull[MAX_STRING] = { 0 };
	Configure(szNull, 0);
	CheckNavigation();
	CreateAA();
	CreateAggro();
	CreateAura();
	CreateBuff();
	CreateCall();
	CreateCharm();
	CreateClickyBuff();
	CreateClickies();
	CreateCrescendo();
	CreateCure();
	//CheckClickies();
	CreateDebuff();
	CreateDisc();
	CreateDot();
	CreateEndurance();
	CreateFade();
	CreateFightBuff();
	CreateGrab();
	CreateHeal();
	CreateHealPet();
	CreateHealToT();
	CreateImHit();
	CreateInvis();
	CreateInvisUndead();
	CreateItem();
	CreateJolt();
	CreateKnockback();
	CreateLevitate();
	CreateLifetap();
	CreateMainTankBuff();
	CreateMana();
	CreateMez();
	CreateNuke();
	CreateNukeToT();
	CreatePetBuff();
	CreatePull();
	CreateRez();
	CreateRoot();
	CreateSelfBuff();
	CreateSnare();
	CreateSwarm();
	if (GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		CreateSummonItem();
	if (GetCharInfo()->pSpawn->mActorClient.Class == 13)
		CreateModrodSpells();
	vBuffQ.clear();
	vBuffQSpawn.clear();

	if (GetCharInfo()->pSpawn->mActorClient.Class == 8 || GetCharInfo()->pSpawn->mActorClient.Class == 1 || GetCharInfo()->pSpawn->mActorClient.Class == 3 || GetCharInfo()->pSpawn->mActorClient.Class == 5 || GetCharInfo()->pSpawn->mActorClient.Class == 7 || GetCharInfo()->pSpawn->mActorClient.Class == 9 || GetCharInfo()->pSpawn->mActorClient.Class == 15 || GetCharInfo()->pSpawn->mActorClient.Class == 16)
		bEndClass = true;
}

void UnloadSpells()
{
	SpellIf.clear();
	vAA.clear();
	vAggro.clear();
	vAggroName.clear();
	vAura.clear();
	vAuraEffect.clear();
	vBuff.clear();
	vCall.clear();
	vCallName.clear();
	vClicky.clear();
	vClickyPrestige.clear();
	vClickySpell.clear();
	vCrescendo.clear();
	vCure.clear();
	vCureName.clear();
	vDebuff.clear();
	vDebuffName.clear();
	vDisc.clear();
	vDot.clear();
	vDotName.clear();
	vEndurance.clear();
	vFade.clear();
	vFadeName.clear();
	vFightBuff.clear();
	vFightBuffName.clear();
	vHeal.clear();
	vHealName.clear();
	vHealPet.clear();
	vHealPetName.clear();
	vHealToT.clear();
	vHealToTName.clear();
	vJolt.clear();
	vJoltName.clear();
	vLifetap.clear();
	vLifetapName.clear();
	vMainTankBuff.clear();
	vMainTankBuffName.clear();
	vMana.clear();
	vManaName.clear();
	vMez.clear();
	vMezName.clear();
	vNuke.clear();
	vNukeName.clear();
	vPet.clear();
	vPetBuff.clear();
	vPetBuffName.clear();
	vRez.clear();
	vRezName.clear();
	vRoot.clear();
	vRootName.clear();
	vSelfBuff.clear();
	vSelfBuffName.clear();
	vSnare.clear();
	vSnareName.clear();
	vSwarm.clear();
	vSwarmName.clear();
	customPetSpell = NULL;
	HideDoCommand(GetCharInfo()->pSpawn, "/squelch /stick off", FromPlugin);
}

void ShowVersion()
{
	char myFileVersion[MAX_STRING] = { 0 };
	bool bVersionResult = GetVersionInfo(myFileVersion);
	GetBuild();
	WriteChatf("\at%s \ay(\ag%.4f%s%s\ay)\ax: \atAuthored by \a-gPeteSampras, \atUpdated by \a-g htw",
		PLUGIN_NAME,
		botVersion,
		(bVersionResult ? " \ax: \ag" : ""),
		(bVersionResult ? myFileVersion : ""));
	// Note:  This should be MMOBUGS, not MMOBUGS_LOADER.  At any given time, for example, we could change to
	//  not enforcing loader code in the plugin.  Some of our plugins use (enforce) it, some don't.
	// However, ANY time we are compiling for our own release, MMOBUGS is defined as a preprocessor constant,
	// so we don't want to tell our own users that it's limited to using MMOLoader/MMOBUGS, because they are
	// already doing that.  Hope that makes sense!   - htw 8/15/15
#ifndef MMOBUGS
	WriteChatf("\atDebugging is currently limited to crash reports via MMOLoader and subscription @ www.mmobugs.com");
#endif
}

//PLUGIN API STUFF
PLUGIN_API VOID SetGameState(DWORD newGameState)
{
	DebugSpewAlways("MQ2Bot::SetGameState(%d)", newGameState);
	if (newGameState == GAMESTATE_INGAME)
	{
		if (InGameOK())
		{
			if (Bot == -1)
			{
				pCharFix = GetCharInfo()->pSpawn;
				sprintf_s(INISection, "%s_%s", pCharFix->Name, EQADDR_SERVERNAME);
				Bot = GetPrivateProfileInt(INISection, "Bot", 0, INIFileName);
			}
			if (Bot == 1)
				BotCommand(pCharFix, "");
		}
	}
	else if (newGameState == GAMESTATE_CHARSELECT)
	{
		camping = false;
		Bot = -1;
		EzCommand("/bot off");
	}
}

// Call to activate plugin.
// Add commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOn()
{
	if (strstr(EQADDR_SERVERNAME, "phin"))
	{
		WriteChatf("\arMQ2Bot is disabled on this server");
		return;
	}
#ifdef MMOBUGS_LOADER
	if (mmo.Active)
		return;
	mmo.Active = true;
#endif
	if (activated)
		return;
	activated = 1;
	if (GetCharInfo() && GetCharInfo()->pSpawn)
		MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
	AddCommand("/bot", BotCommand);
	AddCommand("/botini", BotIniCommand);
	AddCommand("/bottwist", BotTwistCommand);
	AddCommand("/doorclick", DoorCommand);
	AddCommand("/coth", COTHCommand);
	AddCommand("/petgear", PetEquipCommand);
	AddCommand("/loadignore", LoadIgnoreCommand);
	AddCommand("/loadbot", loadbotCommand);
	AddCommand("/invis", InvisCommand);
	AddCommand("/ivu", InvisUndeadCommand);
	//10
	AddCommand("/lev", LevitateCommand);
	AddCommand("/imob", IgnoreMobCommand);
	AddCommand("/mmob", MezImmuneMobCommand);
	AddCommand("/smob", SnareImmuneMobCommand);
	AddCommand("/cmob", CharmImmuneMobCommand);
	AddCommand("/campfire", CampfireCommand);
	AddCommand("/immunes", ImmuneToggleCommand);
	AddCommand("/invitegroup", InviteGuildToGroup);
	AddCommand("/inviteraid", InviteGuildToRaid);
	AddCommand("/removeaura", RemoveMyAura);
	//20
	AddCommand("/offtank", OffTankCommand);
	AddCommand("/findmob", FindMobCommand);
	AddCommand("/safetycheck", SafetyCommand);
	AddCommand("/spawntrack", SpawnTrackCommand);
	CheckAlias();
	//pAddTypes= new MQ2AddType;
	pBotTypes = new MQ2BotType;
	AddMQ2Data("Bot", DataBot);
	//AddMQ2Data("Add",DataAdd);
	EzCommand("/squelch /bot off");
	ZoneTimer = MQGetTickCount64();
}

// Call to deactivate plugin.
// Remove commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOff()
{
#ifdef MMOBUGS_LOADER
	if (!mmo.Active)
		return;
	mmo.Active = false;
#endif
	if (!activated)
		return;
	activated = 0;
	DebugSpewAlways("Shutting down MQ2Bot");
	RemoveCommand("/bot");
	RemoveCommand("/botini");
	RemoveCommand("/bottwist");
	RemoveCommand("/doorclick");
	RemoveCommand("/coth");
	RemoveCommand("/petgear");
	RemoveCommand("/invis");
	RemoveCommand("/ivu");
	RemoveCommand("/lev");
	RemoveCommand("/imob");
	//10
	RemoveCommand("/smob");
	RemoveCommand("/mmob");
	RemoveCommand("/cmob");
	RemoveCommand("/campfire");
	RemoveCommand("/loadignore");
	RemoveCommand("/loadbot");
	RemoveCommand("/immunes");
	RemoveCommand("/invitegroup");
	RemoveCommand("/inviteraid");
	RemoveCommand("/removeaura");
	//20
	RemoveCommand("/offtank");
	RemoveCommand("/findmob");
	RemoveCommand("/safetycheck");
	RemoveCommand("/spawntrack");
	//RemoveMQ2Data("Add");
	//delete pAddTypes;
	//pAddTypes = nullptr;
	RemoveMQ2Data("Bot");
	delete pBotTypes;
	pBotTypes = NULL;
}

// Called when plugin is loading
PLUGIN_API VOID InitializePlugin(VOID)
{
#ifdef MMOBUGS_LOADER
	MMORequiredAccess = AL_Premium;
	if (LOK(MMORequiredAccess))
	{
		PluginOn();
		if (InGameOK())
		{
			ShowVersion();
			bVersionShown = true;
		}
	}
	else
		WriteChatf("\ar%s \aysubscription is required to use \ag%s", MMOAccessName[MMORequiredAccess], PLUGIN_NAME);
#else
	PluginOn();
	if (InGameOK())
	{
		ShowVersion();
		bVersionShown = true;
	}
#endif
}

// Called when plugin is unloading
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2Bot");
	PluginOff();
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pSpawn)
{
	if (camping)
	{
		return;
	}
	if (SafetyCheck)
	{
		if (pSpawn)
		{
			if (pSpawn->GM && pSpawn->Type != SPAWN_CORPSE || strstr(pSpawn->Lastname, "Free Hugs"))
				if (strlen(pSpawn->DisplayedName))
				{
					ShouldIExit = true;
					WriteChatf("/arGM detected!");
					EzCommand("/mqlog GM detected!");
				}
		}
	}

}

// Called when zoning begins 
// This is when the begin zone opcode is received from server, before the client knows zoning is started
PLUGIN_API VOID OnBeginZone(VOID)
{
	FindDoor = 0;
	bCanIPull = false;
	zoning = true;
	CharmMob = NULL;
	xTargetingMe = NULL;
	xNotTargetingMe = NULL;
	pFindMob = NULL;
	pPullMob = NULL;
	//MyClass=GetCharInfo()->pSpawn->mActorClient.Class;
	ClearTimers();
	if (Bot)
		EzCommand("/squelch /bottwist off");
	twisting = false;
	ZoneTimer = MQGetTickCount64();
	WarpDistance = 0;
	ShouldIExit = false;
	SafetyCheck = false;
}

// Called when zoning finishes 
// This when the end zone opcode is received from server, before the client knows zoning is complete
PLUGIN_API VOID OnEndZone(VOID)
{
	if (!InGameOK())
		return;
	MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
	zoning = false;
	ZoneTimer = MQGetTickCount64();
	WarpX = GetCharInfo()->pSpawn->X;
	WarpY = GetCharInfo()->pSpawn->Y;
#ifdef MMOBUGS_LOADER
	if (LOK(MMORequiredAccess))
	{
		PluginOn();
		MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
		EzCommand("/loadignore");
		ZoneTimer = MQGetTickCount64();
	}
	else
	{
		PluginOff();
	}
#else
	PluginOn();
	MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
	EzCommand("/loadignore");
	ZoneTimer = MQGetTickCount64();
#endif
}

// Called on first pulse after zoning 
// This when the end zone opcode is received from server, after the client knows zoning is complete
PLUGIN_API VOID OnZoned(VOID)
{
	if (!InGameOK())
		return;
	CheckAlias();
	zoneID = 0;
	vBuffQSpawn.clear();
	vBuffQ.clear();
	bCanIPull = false;
	CheckNavigation();
	outside = true;
	if (zoneID != GetCharInfo()->zoneId)
	{
		//CheckIgnoreList();
	}
}

// Called every time MQ2 'pulses'
// This is called OnPulse due to the Heartbeat detour, which is called by the client to 'process events'
PLUGIN_API VOID OnPulse(VOID)
{
#ifdef MMOBUGS_LOADER
	if (!mmo.Active)
		return;
#endif
	if (InGameOK() && !bVersionShown)
	{
		ShowVersion();
		bVersionShown = true;
	}
	if (camping)
	{
		return;
	}
	if (MQGetTickCount64() < (ZoneTimer + 10000LL))
		return;
	if (gGameState != GAMESTATE_INGAME && Bot == 1)
		EzCommand("/bot off");
	if (!InGameOK())
		return;
	if (FindDoor)
		CheckDoor();
	if (CampfireStep)
		CheckCampfire();
	if (vTwist.size())
		CheckTwist();
	char test[MAX_STRING];
	sprintf_s(test, ",%d,", GetCharInfo()->zoneId);
	if (strstr(RestrictedZones, test))
		return;
	if (WinState((CXWnd*)pGiveWnd) && MQGetTickCount64() < PetGearTimer)
		HideDoCommand(GetCharInfo()->pSpawn, "/notify GiveWnd GVW_Give_Button leftmouseup", FromPlugin);
	if (petGearSize > 0)
		EquipPet();
	if (WinState((CXWnd*)pGiveWnd))
		return;
	if (WinState((CXWnd*)pSpellBookWnd))
		return;
	if (WinState((CXWnd*)pLootWnd))
		return;
	if (WinState((CXWnd*)pBankWnd))
		return;
	if (WinState((CXWnd*)pMerchantWnd))
		return;
	if (WinState((CXWnd*)pTradeWnd))
		return;
	CheckPulling();
	TimerCleanup();
	if (vBuffQ.size())
		CheckBuffQ();
	if(GetCharInfo()->pSpawn==GroupPuller() && bAmIPuller)
		CheckPull();
	if (SafetyCheck)
		CheckSafety();
	if (bPulling)
	{
		CheckReturn();
		UpdatePullingNavigation();
	}
	if (bReturning)
		UpdateReturnNavigation();
	if (Bot == 1 || AlwaysCheckAdds > 0)
		CheckAdds();
	if (bPulling || bReturning || MacroPulling || zoning || ++Pulses < CycleDelay || gGameState != GAMESTATE_INGAME || !GetCharInfo()->pSpawn || GetCharInfo()->pSpawn->RespawnTimer)
		return;

	if (Bot != 1)
		return;
	CheckMerc();

	CheckFade();
	//if(MQGetTickCount64()<SpellTimer)
	//	CheckInterrupt();
	if (!looting)
	{
		if (SpawnTrackGroupTimer < MQGetTickCount64())
			SpawnTrackGroup();

		CheckCall();
		CheckRez();
		CheckStick();
		CheckTarget();
		CheckMana();
		if (petSpell)
			CheckPet();
		if (!GetCharInfo()->pSpawn->HideMode)
		{
			if (IfCheck("SelfBuff"))
				CheckSelfBuffs();
			CheckClickies();
			CheckClickyBuffs();
		}
		CheckMed();
		if (MedToFull && Medding)
			return;
		if (EndMedStartAt && bEndClass)
			CheckEndurance();
		if (EndMedToFull && EndMedding)
			return;
		CheckHeals();
		CheckHealPets();
		CheckAura();
		if (BardClass)
			CheckBard();
		if (vAdds.size())
		{
			if (summoned)
				CheckMoveBack();
			if (IfCheck("Charm"))
				CheckCharm();
			CheckMelee();
			if (IfCheck("Mez"))
				CheckMez();
			if (IfCheck("Root"))
				CheckRoot();
			if (IfCheck("Knockback"))
				CheckKnockback();
			if (IfCheck("Grab"))
				CheckGrab();
			if (IfCheck("Snare"))
				CheckSnare();
			if (IfCheck("Aggro"))
				CheckAggro();
			if (IfCheck("Jolt"))
				CheckJolt();
			if (IfCheck("MainTank"))
				CheckMainTankBuff();
			if (IfCheck("FightBuff"))
				CheckFightBuffs();
			if (IfCheck("Lifetap"))
				CheckLifetaps();
			CheckMisc();
			if (IfCheck("AA"))
				CheckAAs();
			if (IfCheck("Debuff"))
				CheckDebuffs();
			if (IfCheck("Dot"))
				CheckDots();
			if (IfCheck("Swarm"))
				CheckSwarm();
			if (IfCheck("Nuke"))
				CheckNukes();
			if (vNukeToT.size())
				CheckNukeToT();
			CheckItems();
		}
		if (!vAdds.size())
			CheckInvis();
		if (!GetCharInfo()->pSpawn->HideMode && IfCheck("SummonItem"))
			CheckSummonItems();
		CheckAutoInv();
	}
	CheckLoot();
	Pulses = 0;
	//CheckIgnoreList();
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
			sprintf_s(Filename, "%s\\MQ2Bot_%s_%s.log", gszLogPath, EQADDR_SERVERNAME, Name);
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

PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	if (DEBUG_DUMPFILE)
		DebugWrite(Line);
	return 0;
}

PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
#ifdef MMOBUGS_LOADER
	if (!mmo.Active)
		return;
#endif
	if (!InGameOK())
		return;
	SpawnTrackRemove();
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
#ifdef MMOBUGS_LOADER
	if (!mmo.Active)
		return 0;
#endif
	if (!strcmp(Line, "It will take about 5 more seconds to prepare your camp.") && Bot == 0)
	{
		EzCommand("/bot off");
		WriteChatf("\ar[MQ2Bot]\aw::Bot deactivated to prevent possible crashes.");
	}
	if (camping)
	{
		if (!strcmp(Line, "You abandon your preparations to camp."))
		{
			camping = false;
			Bot = 1;
			WriteChatf("\ar[MQ2Bot]\aw::Camping aborted. \ayUnpausing.");
		}
		return 0;
	}
	if (strstr(Line, " tells you, "))
	{
		if (strlen(Line) < 2000 && !strstr(Line, "'Attacking") && !strstr(Line, "'That'll be") && !strstr(Line, "'I'll give you") && !strstr(Line, "'Come back soon!'") && !strstr(Line, "'Welcome to my bank!'"))
		{
			char Message[MAX_STRING], Tmp[MAX_STRING], Final[MAX_STRING], Name[MAX_STRING];
			GetArg(Tmp, Line, 1);
			sprintf_s(Name, "${String[%s].Right[-2].Left[-1]}", Tmp);
			sprintf_s(Message, "${String[%s].Right[-${Math.Calc[${String[%s].Find[tells you, ']}+%d+2]}].Left[-1]}", Line, Line, strlen(Tmp));
			sprintf_s(Final, "%s - %s", Name, Message);
			vTells.push_back(Final);
		}
	}
	if (Bot != 1)
		return 0;
	if (!strcmp(Line, "It will take you about 30 seconds to prepare your camp."))
	{
		Bot = 0;
		camping = true;
		WriteChatf("\ar[MQ2Bot]\aw::Camping detected. \ayPausing.");
	}
	if (strstr(Line, "Your target resisted the Requiem of Time spell.") && BardClass)
	{
		for (unsigned int x = 0; x < vTimerSpawn.size() && x < vTimerSpell.size() && vAdds.size()>0; x++)
		{
			if (!strcmp(vTimerSpell[x]->Name, "Requiem of Time") && vAdds[0] && vAdds[0] == vTimerSpawn[x])
			{
				vTimerTimestamp.erase(vTimerTimestamp.begin() + x);
				vTimerSpell.erase(vTimerSpell.begin() + x);
				vTimerSpawn.erase(vTimerSpawn.begin() + x);
			}
		}
	}
	if (!strcmp(Line, "This NPC cannot be charmed.") && ImmunityCheck)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
		{
			char test[MAX_STRING];
			sprintf_s(test, "|%s|", pSpawn->DisplayedName);
			if (!strstr(CharmImmuneList, test))
			{
				sprintf_s(CharmImmuneList, "%s%s|", CharmImmuneList, pSpawn->DisplayedName);
				WritePrivateProfileString(CharmImmuneINISection, "CharmImmune", CharmImmuneList, ImmuneINIFileName);
				WriteChatf("\am%s \aw added to CharmImmunes.", pSpawn->DisplayedName);
				GetPrivateProfileString(CharmImmuneINISection, "CharmImmune", "|", CharmImmuneList, MAX_STRING, ImmuneINIFileName);
				WriteChatf("\am%s", CharmImmuneList);
				CharmMob = NULL;
			}
		}
	}
	if (!strcmp(Line, "Your target cannot be mesmerized.") && LastMezSingle && ImmunityCheck)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
		{
			char test[MAX_STRING];
			sprintf_s(test, "|%s|", pSpawn->DisplayedName);
			if (!strstr(MezImmuneList, test))
			{
				sprintf_s(MezImmuneList, "%s%s|", MezImmuneList, pSpawn->DisplayedName);
				WritePrivateProfileString(MezImmuneINISection, "MezImmune", MezImmuneList, ImmuneINIFileName);
				WriteChatf("\ap%s \aw added to MezImmunes.", pSpawn->DisplayedName);
				GetPrivateProfileString(MezImmuneINISection, "MezImmune", "|", MezImmuneList, MAX_STRING, ImmuneINIFileName);
				WriteChatf("\ap%s", MezImmuneList);
			}
		}
	}
	if (!strcmp(Line, "Your target is immune to changes in its run speed.") && LastSnareSingle && ImmunityCheck)
	{
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget)
		{
			char test[MAX_STRING];
			sprintf_s(test, "|%s|", pSpawn->DisplayedName);
			if (!strstr(SnareImmuneList, test))
			{
				sprintf_s(SnareImmuneList, "%s%s|", SnareImmuneList, pSpawn->DisplayedName);
				WritePrivateProfileString(SnareImmuneINISection, "SnareImmune", SnareImmuneList, ImmuneINIFileName);
				WriteChatf("\ag%s \aw added to SnareImmunes.", pSpawn->DisplayedName);
				GetPrivateProfileString(SnareImmuneINISection, "SnareImmune", "|", SnareImmuneList, MAX_STRING, ImmuneINIFileName);
				WriteChatf("\ag%s", SnareImmuneList);
			}
		}
	}
	if (!_stricmp(Line, "Your Banestrike fails because you have not yet mastered slaying this race."))
	{
		char tempList[MAX_STRING] = "";

		PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget;
		if (strlen(IgnoreBaneStrike) < 2028)
		{
			sprintf_s(tempList, "${If[${Target.Race.Name.NotEqual[NULL]},%s${Target.Race}|,]}", IgnoreBaneStrike);
			ParseMacroData(tempList, MAX_STRING);
			strcpy_s(IgnoreBaneStrike, tempList);
			WriteChatf("\a-oBanestrike ignore list: \aw%s", IgnoreBaneStrike);
		}
	}
	if (!_stricmp(Line, "You have been summoned!"))
	{
		if (FightX != 0 && FightY != 0 && FightZ != 0)
		{
			summoned = true;
			CheckMoveBack();
		}
	}
	if (!_stricmp(Line, "You can only cast this spell while in a resting state."))
	{
		if (vRezTimeStamp.size())
		{
			for (unsigned int i = 0; i<6; i++)
			{
				if (i < vRezTimeStamp.size())
					vRezTimeStamp[i] = 0;
			}
		}
	}
	if (ImHitAt > 0 && MQGetTickCount64()>ImHitTimer && InGameOK())
	{
		if (strstr(Line, "s YOU for ") && (GetCharInfo()->pSpawn->HPCurrent * 100 / GetCharInfo()->pSpawn->HPMax) < ImHitAt && GetCharInfo()->pSpawn->StandState == STANDSTATE_STAND)
		{
			CheckImHit();
			ImHitTimer = MQGetTickCount64() + 2000LL;
			ImHitTimeStamp = MQGetTickCount64();
		}
	}
	if (!strcmp(Line, "You can only cast this spell in the outdoors.") || !strcmp(Line, "This spell does not work here.") || !strcmp(Line, "You can not summon a mount here."))
	{
		outside = false;
	}
	if (strstr(Line, " tells ") || strstr(Line, "told you,"))
	{
		char Tmp[MAX_STRING], RequestID[MAX_STRING], SpellName[MAX_STRING], RequestName[MAX_STRING];
		GetArg(RequestID, Line, 1);
		sprintf_s(RequestName, "${String[%s].Right[-2].Left[-1]}", RequestID);
		ParseMacroData(RequestName, MAX_STRING);
		if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByName(RequestName))
		{
			sprintf_s(Tmp, "${If[(${NetBots.Client.Find[%s]} && ${String[%s].Find[netbots]}||${SpawnCount[guild %s]} && ${String[%s].Find[guild]}||${Group.Member[%s].ID} && ${String[%s].Find[group]}||${SpawnCount[raid %s]} && ${String[%s].Find[raid]}||${String[%s].Find[%s]}),1,0]}", RequestName, AuthorizedUsers, RequestName, AuthorizedUsers, RequestName, AuthorizedUsers, RequestName, AuthorizedUsers, AuthorizedUsers, RequestName);
			ParseMacroData(Tmp, MAX_STRING);
			//testing to see if this will allow people to request buffs correctly if they have a buff password.
			if (strstr(Line, BuffPassword) && strstr(Line, "=") || !strcmp(BuffPassword, "NULL") || atoi(Tmp) == 1) //||pSpawn->GuildID==GetCharInfo()->pSpawn->GuildID && pSpawn->GuildID>0 && strstr(AuthorizedUsers,"guild")
			{
				if (atoi(Tmp) == 1)
				{
					GetArg(SpellName, Line, 2, FALSE, FALSE, FALSE, ',');  // note, can't pass more than 1 char, intent?  ',\'');
					sprintf_s(Tmp, "${String[%s]}", SpellName);
				}
				else
				{
					GetArg(SpellName, Line, 2, FALSE, FALSE, FALSE, '=');
					sprintf_s(Tmp, "${String[%s].Left[-1]}", SpellName);
				}
				ParseMacroData(Tmp, MAX_STRING);
				strcpy_s(SpellName, Tmp);
				if (PSPELL pSpell = GetSpellByName(SpellName))
				{
					sprintf_s(Tmp, "${If[${Me.AltAbilityReady[%s]},1,0]}", SpellName);
					ParseMacroData(Tmp, MAX_STRING);
					if (atoi(Tmp) == 1 && pSpell->TargetType != 6)
					{
						vBuffQ.push_back(pSpell);
						vBuffQSpawn.push_back(pSpawn);
						WriteChatf("Requested: Alt: %s, Spawn: %s", SpellName, RequestName);
					}
					sprintf_s(Tmp, "${If[${Me.Book[%s]} && !${Me.AltAbility[%s]},1,0]}", SpellName, SpellName);
					ParseMacroData(Tmp, MAX_STRING);
					if (atoi(Tmp) == 1 && pSpell->TargetType != 6)
					{
						vBuffQ.push_back(pSpell);
						vBuffQSpawn.push_back(pSpawn);
						WriteChatf("Requested: Spell: %s, Spawn: %s", SpellName, RequestName);
					}
				}
				else
				{
					_strlwr_s(SpellName);
					if (!strcmp(SpellName, "cert") || !strcmp(SpellName, "cred") || !strcmp(SpellName, "temp") || !strcmp(SpellName, "tem"))
						strcpy_s(SpellName, "aego");
					if (BardClass && strcmp(SpellName, "selos"))
						return 0;
					if (strstr("|clarity|haste|aego|skin|sv|se|pred|strength|brells|focus|regen|symbol|growth|shining|rez|ds1|ds2|ds3|ds4|ds5|ds6|ds7|ds8|ds9|ds10|ds11|ds12|panther|fero|retort|hot|", SpellName))
					{
						int spellLevel = 0;
						PSPELL pSpellSave = NULL;
						for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
						{
							if (InGameOK() && GetCharInfo2()->SpellBook[nSpell] != 0xFFFFFFFF)
							{
								if (GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
								{
									SpellCategory(GetSpellByID(GetCharInfo2()->SpellBook[nSpell]));
									if (!_stricmp(spellCat, SpellName))
									{
										PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]);
										if (pSpell && pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] > spellLevel && pSpell->TargetType != 6)
										{
											spellLevel = pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class];
											pSpellSave = pSpell;
										}
									}
								}
							}
						}
						if (!strcmp(SpellName, "haste") || !strcmp(spellCat, "haste"))
						{
							char test[MAX_STRING] = "";
							strcpy_s(test, "${If[${Me.AltAbilityReady[Talisman of Celerity]},1,0]}");
							ParseMacroData(test, MAX_STRING);
							if (atoi(test) == 1)
								pSpellSave = GetSpellByName("Talisman of Celerity");
						}
						if (pSpellSave)
						{
							vBuffQ.push_back(pSpellSave);
							vBuffQSpawn.push_back(pSpawn);
							WriteChatf("Requested: Spell: %s, Spawn: %s", pSpellSave->Name, RequestName);
						}
					}
				}
			}
		}
	}
	return 0;
}

// Load configuration settings
// Initialize the INI section if one doesn't exist
void Configure(char szCustomIni[MAX_STRING], int force)
{
	if (!InGameOK())
		return;
	char tempINI[MAX_STRING] = { 0 };
	long Class = GetCharInfo()->pSpawn->mActorClient.Class;
	long Races = GetCharInfo2()->Race;
	long Level = GetCharInfo2()->Level;
	sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	sprintf_s(INISection, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
	Shrouded = GetCharInfo2()->Shrouded;
	if (!Shrouded)
		INISection[strlen(PLUGIN_NAME)] = 0;
	strcpy_s(tempINI, INISection);
	_strlwr_s(szCustomIni, MAX_STRING);
	if (force && strlen(szCustomIni) > 1 && !strstr(szCustomIni, "default"))
		sprintf_s(INISection, "%s_%s", tempINI, szCustomIni);
	char szTemp[MAX_STRING] = { 0 };
	//for (std::map<string,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	//{
	//	it->second =GetPrivateProfileInt(INISection,it->first,it->second,INIFileName);
	//	WritePrivateProfileString(INISection,it->first,_itoa_s(it->second,szTemp,10),INIFileName);
	//}
	DEBUG_DUMPFILE = 0 != GetPrivateProfileInt(INISection, "Debugging", 0, INIFileName);//log to file?
	DefaultGem = GetPrivateProfileInt(INISection, "DefaultGem", 1, INIFileName); //default gem to cast with
	AlwaysCheckAdds = GetPrivateProfileInt(INISection, "AlwaysCheckAdds", 0, INIFileName); //check adds even with /bot off
	NukeAt = GetPrivateProfileInt(INISection, "NukeAt", 99, INIFileName); //nuke at %
	AACutoffTime = GetPrivateProfileInt(INISection, "AACutoffTime", 10, INIFileName); //refresh time on AA to use or not
	AAStopAt = GetPrivateProfileInt(INISection, "AAStopAt", 25, INIFileName); //stop using AA at %
	DotAt = GetPrivateProfileInt(INISection, "DotAt", 99, INIFileName);//dot start at %
	DotStopAt = GetPrivateProfileInt(INISection, "DotStopAt", 10, INIFileName);//dot stop at %
	DebuffAdds = GetPrivateProfileInt(INISection, "DebuffAdds", 1, INIFileName);//debuff adds?
	FadeAt = GetPrivateProfileInt(INISection, "FadeAt", 20, INIFileName);//debuff adds?
	RootAdds = GetPrivateProfileInt(INISection, "RootAdds", 15, INIFileName);//Root adds distance?
	RootPrimary = GetPrivateProfileInt(INISection, "RootPrimary", 0, INIFileName);//Root primary distance?
	KnockbackAdds = GetPrivateProfileInt(INISection, "KnockbackAdds", 0, INIFileName);//Knockback adds distance?
	KnockbackPrimary = GetPrivateProfileInt(INISection, "KnockbackPrimary", 0, INIFileName);//Knockback primary distance?
	GrabAdds = GetPrivateProfileInt(INISection, "GrabAdds", 0, INIFileName);//Grab adds distance?
	GrabPrimary = GetPrivateProfileInt(INISection, "GrabPrimary", 0, INIFileName);//Grab primary distance?
	DebuffAt = GetPrivateProfileInt(INISection, "DebuffAt", 100, INIFileName);//debuff start at %
	DebuffStopAt = GetPrivateProfileInt(INISection, "DebuffStopAt", 10, INIFileName);//debuff stop at %
	DebuffAEAmount = GetPrivateProfileInt(INISection, "DebuffAEAmount", 3, INIFileName);//use AE debuffs at %
	SwarmAt = GetPrivateProfileInt(INISection, "SwarmAt", 99, INIFileName); //swarm pet at %
	InterruptToHealAt = GetPrivateProfileInt(INISection, "InterruptToHealAt", 0, INIFileName);//interrupt to heal at %
	HealAt = GetPrivateProfileInt(INISection, "HealAt", 80, INIFileName);//heal at %
	HealToTAt = GetPrivateProfileInt(INISection, "HealToTAt", 80, INIFileName);//heal at %
	HealSelfAt = GetPrivateProfileInt(INISection, "HealSelfAt", 80, INIFileName);//heal self at %
	HealDurationAt = GetPrivateProfileInt(INISection, "HealDurationAt", 95, INIFileName);//duration heal at %
	HealDelayedAt = GetPrivateProfileInt(INISection, "HealDelayedAt", 95, INIFileName);//delayed heal at %
	HealGroupAt = GetPrivateProfileInt(INISection, "HealGroupAt", 80, INIFileName);//group heal at %
	UseManaInCombat = GetPrivateProfileInt(INISection, "UseManaInCombat", 1, INIFileName);//use mana skills while in combat?
	UseManaOnOthers = GetPrivateProfileInt(INISection, "UseManaOnOthers", 1, INIFileName);//use mana skills on others?
	UseMez = GetPrivateProfileInt(INISection, "UseMez", 1, INIFileName);//use mez?
	RezRadius = GetPrivateProfileInt(INISection, "RezRadius", 100, INIFileName);//use rez?
	iUsePet = GetPrivateProfileInt(INISection, "UsePet", 1, INIFileName);//use pet?
	UseSelfBuffs = GetPrivateProfileInt(INISection, "UseSelfBuffs", 1, INIFileName);//use self buffs?
	UseFightBuffs = GetPrivateProfileInt(INISection, "UseFightBuffs", 1, INIFileName);//use fight buffs?
	UseDiscsOffensive = GetPrivateProfileInt(INISection, "UseDiscsOffensive", 0, INIFileName);//use O discs?
	UseDiscsDefensive = GetPrivateProfileInt(INISection, "UseDiscsDefensive", 0, INIFileName);//use D discs?
	UseClickyBuffs = GetPrivateProfileInt(INISection, "UseClickyBuffs", 1, INIFileName);//use clicky buffs?
	UseClickyNukes = GetPrivateProfileInt(INISection, "UseClickyNukes", 1, INIFileName);//use clicky nukes?
	SnareAt = GetPrivateProfileInt(INISection, "SnareAt", 10, INIFileName);//snare mob at
	UsePetBuffs = GetPrivateProfileInt(INISection, "UsePetBuffs", 1, INIFileName);//use pet buffs?
	StandOnPetAttack = GetPrivateProfileInt(INISection, "StandOnPetAttack", 0, INIFileName);//stand before /pet attack?
	LifetapAt = GetPrivateProfileInt(INISection, "LifetapAt", 80, INIFileName);//lifetap at
	LootInCombat = GetPrivateProfileInt(INISection, "LootInCombat", 0, INIFileName);//loot during combat?
	LootRadius = GetPrivateProfileInt(INISection, "LootRadius", 0, INIFileName);//loot radius for mq2advloot
	UseMainTankBuffs = GetPrivateProfileInt(INISection, "UseMainTankBuffs", 1, INIFileName);//use main tank buffs?
	HealPetAt = GetPrivateProfileInt(INISection, "HealPetAt", 80, INIFileName);//heal pets at
	ManaAt = GetPrivateProfileInt(INISection, "ManaAt", 80, INIFileName); //use mana restoration spells?
	UseAENukes = GetPrivateProfileInt(INISection, "UseAENukes", 1, INIFileName);//use AE nukes?
	UseAura = GetPrivateProfileInt(INISection, "UseAura", 1, INIFileName);//use auras?
	AggroAt = GetPrivateProfileInt(INISection, "AggroAt", 70, INIFileName);//use aggro at %?
	JoltAt = GetPrivateProfileInt(INISection, "JoltAt", 70, INIFileName);//use jolt at %?
	if (MyClass == 1 || MyClass == 3 || MyClass == 4 || MyClass == 7 || MyClass == 8 || MyClass == 9 || MyClass == 15 || MyClass == 16)
		MeleeAt = GetPrivateProfileInt(INISection, "MeleeAt", 100, INIFileName);//use melee at %?
	else
		MeleeAt = GetPrivateProfileInt(INISection, "MeleeAt", 0, INIFileName);//use melee at %?
	AutofireAt = GetPrivateProfileInt(INISection, "AutofireAt", 0, INIFileName);//use autofire at %?
	AutofireDistance = GetPrivateProfileInt(INISection, "AutofireDistance", 100, INIFileName);//use autofire at %?
	ImHitAt = GetPrivateProfileInt(INISection, "ImHitAt", 50, INIFileName);//use melee at %?
																		   //assist settings
	AssistAt = GetPrivateProfileInt(INISection, "AssistAt", 100, INIFileName);//assist at
	AssistRange = GetPrivateProfileInt(INISection, "AssistRange", 100, INIFileName);//assist range
	UseManualAssist = GetPrivateProfileInt(INISection, "UseManualAssist", 0, INIFileName);// use manual /assist
	GetPrivateProfileString(INISection, "AssistName", "", AssistName, sizeof(AssistName), INIFileName);
	MercAssistAt = GetPrivateProfileInt(INISection, "MercAssistAt", 100, INIFileName);//merc assist at
	MercAssistRange = GetPrivateProfileInt(INISection, "MercAssistRange", 0, INIFileName);//merc assist range
	MeleeDistance = GetPrivateProfileInt(INISection, "MeleeDistance", 40, INIFileName);//assist range
	if (MyClass == 5 || MyClass == 6 || MyClass == 8 || MyClass == 7 || MyClass == 10 || MyClass == 11 || MyClass == 13 || MyClass == 14 || MyClass == 15)
		DoPetSwarm = GetPrivateProfileInt(INISection, "DoPetSwarm", 1, INIFileName);//use /pet swarm?
	else
		DoPetSwarm = GetPrivateProfileInt(INISection, "DoPetSwarm", 0, INIFileName);//use /pet swarm?

	PetAttackAt = GetPrivateProfileInt(INISection, "PetAttackAt", 100, INIFileName);//pet attack at
	PetAttackRange = GetPrivateProfileInt(INISection, "PetAttackRange", 60, INIFileName);//pet attack range
	UseAutoFire = GetPrivateProfileInt(INISection, "UseAutoFire", 0, INIFileName);//use autofire? ranger?
																				  //generic settings 
	CycleDelay = GetPrivateProfileInt(INISection, "CycleDelay", 3, INIFileName);//announce in cecho?
	AnnounceEcho = GetPrivateProfileInt(INISection, "AnnounceEcho", 1, INIFileName);//announce in cecho?
	AnnounceEQBC = GetPrivateProfileInt(INISection, "AnnounceEQBC", 0, INIFileName);//announce in eqbc?
	AnnounceAdds = GetPrivateProfileInt(INISection, "AnnounceAdds", 1, INIFileName);//announce adds in echo?
	AutoInvisDistance = GetPrivateProfileInt(INISection, "AutoInvisDistance", 0, INIFileName);//announce in cecho?
	MedStartAt = GetPrivateProfileInt(INISection, "MedStartAt", 0, INIFileName);//med start at?
	MedStopAt = GetPrivateProfileInt(INISection, "MedStopAt", 0, INIFileName);//med stop at?
	MedToFull = GetPrivateProfileInt(INISection, "MedToFull", 0, INIFileName);//med to full?
	MedDuringCombat = GetPrivateProfileInt(INISection, "MedDuringCombat", 0, INIFileName);//med during combat?
	EndMedStartAt = GetPrivateProfileInt(INISection, "EndMedStartAt", 0, INIFileName);//end med start at?
	EndMedStopAt = GetPrivateProfileInt(INISection, "EndMedStopAt", 0, INIFileName);//end med stop at?
	EndMedToFull = GetPrivateProfileInt(INISection, "EndMedToFull", 0, INIFileName);//end med to full?
	NukeToTAt = GetPrivateProfileInt(INISection, "NukeToTAt", 80, INIFileName);//nuke tot at %
	MinManaToNuke = GetPrivateProfileInt(INISection, "MinManaToNuke", 0, INIFileName);//med at?
	ManaToSummon = GetPrivateProfileInt(INISection, "ManaToSummon", 100, INIFileName);//mana req to summon at?
	SummonModRods = GetPrivateProfileInt(INISection, "SummonModRods", 1, INIFileName);//summon mod rods?
	UseModRods = GetPrivateProfileInt(INISection, "UseModRods", 1, INIFileName);//use mod rods?
	AutoFollowLeader = GetPrivateProfileInt(INISection, "AutoFollowLeader", 0, INIFileName);//use stick?
	UseNetBots = GetPrivateProfileInt(INISection, "UseNetBots", 0, INIFileName);//use NetBots?
	FaceAngle = GetPrivateProfileInt(INISection, "FaceAngle", 360, INIFileName);//Angle, if greater, to /face
	DebuffsSpellFirst = GetPrivateProfileInt(INISection, "DebuffsSpellFirst", 0, INIFileName);//use spells before AA for debuffs section?
	if (Class == 2 || Class == 6 || Class == 10 || Class == 11 || Class == 12 || Class == 13 || Class == 14)
		RepositionIfSummoned = GetPrivateProfileInt(INISection, "RepositionIfSummoned", 1, INIFileName);//reposition if summoned?
	else
		RepositionIfSummoned = GetPrivateProfileInt(INISection, "RepositionIfSummoned", 0, INIFileName);//reposition if summoned?
	GetPrivateProfileString(INISection, "RestrictedZones", ",344,202,151,345,", RestrictedZones, sizeof(RestrictedZones), INIFileName);
	GetPrivateProfileString(INISection, "AutoFollowSettings", "30 healer", AutoFollowSettings, sizeof(AutoFollowSettings), INIFileName);
	GetPrivateProfileString(INISection, "NetBotsName", "NULL", NetBotsName, sizeof(NetBotsName), INIFileName);
	GetPrivateProfileString(INISection, "CustomPetSpell", "NULL", CustomPetSpell, sizeof(CustomPetSpell), INIFileName);
	GetPrivateProfileString(INISection, "PetIllusionItem", "NULL", PetIllusionItem, sizeof(PetIllusionItem), INIFileName);
	GetPrivateProfileString(INISection, "BuffPassword", "NULL", BuffPassword, sizeof(BuffPassword), INIFileName);
	GetPrivateProfileString(INISection, "AuthorizedUsers", "|guild|group|netbots|", AuthorizedUsers, sizeof(AuthorizedUsers), INIFileName);
	GetPrivateProfileString(INISection, "BotCommandsOn", "", BotCommandsOn, sizeof(BotCommandsOn), INIFileName);
	GetPrivateProfileString(INISection, "BotCommandsOff", "", BotCommandsOff, sizeof(BotCommandsOff), INIFileName);
	GetPrivateProfileString(INISection, "PullSkillName", "", PullSkill, sizeof(PullSkill), INIFileName);
	GetPrivateProfileString(INISection, "AttackCommand", "/killthis", AttackCommand, sizeof(AttackCommand), INIFileName);
	DoDebug = GetPrivateProfileInt(INISection, "DoDebug", 0, INIFileName);//debug?
	PullingDistance = GetPrivateProfileInt(INISection, "PullingDistance", 0, INIFileName);//debug?
	bAmIPuller = GetPrivateProfileInt(INISection, "DoPulls", 0, INIFileName);//should i pull?
	MQ2NavDistance = GetPrivateProfileInt(INISection, "MQ2NavDistance", 10, INIFileName);//debug?
	AnnounceBehaviorChange = GetPrivateProfileInt(INISection, "AnnounceBehaviorChange", 0, INIFileName);//debug?
	PetReagentCheck = GetPrivateProfileInt(INISection, "PetReagentCheck", 1, INIFileName);//Check for pet reagents before casting?
	SafetyInvisOnWarp = GetPrivateProfileInt(INISection, "SafetyInvisOnWarp", 0, INIFileName);// invis on warp?
	GetPrivateProfileString(INISection, "SafetyCommandOnTell", "", SafetyCommandOnTell, sizeof(SafetyCommandOnTell), INIFileName);
	GetPrivateProfileString(INISection, "SafetyCommandOnGM", "", SafetyCommandOnGM, sizeof(SafetyCommandOnGM), INIFileName);
	GetPrivateProfileString(INISection, "SafetyCommandOnMyWarp", "", SafetyCommandOnMyWarp, sizeof(SafetyCommandOnMyWarp), INIFileName);
	GetPrivateProfileString(INISection, "SafetyCommandOnOthersWarp", "", SafetyCommandOnOthersWarp, sizeof(SafetyCommandOnOthersWarp), INIFileName);
	GetPrivateProfileString(INISection, "SafetyCommandOnTooFarFromCamp", "", SafetyCommandOnTell, sizeof(SafetyCommandOnTooFarFromCamp), INIFileName);
	if (BardClass)
	{
		GetPrivateProfileString(INISection, "RestTwist", "", RestTwist, sizeof(RestTwist), INIFileName);
		GetPrivateProfileString(INISection, "CombatTwist", "", CombatTwist, sizeof(CombatTwist), INIFileName);
	}
	//Buffs
	if (Class == 2) //cleric
	{
		GetPrivateProfileString(INISection, "AegoClasses", "|WAR|MNK|ROG|BER|CLR|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", AegoClasses, sizeof(AegoClasses), INIFileName);
		GetPrivateProfileString(INISection, "SymbolClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SymbolClasses, sizeof(SymbolClasses), INIFileName);
	}
	if (Class == 3) //pal
	{
		GetPrivateProfileString(INISection, "AegoClasses", "|WAR|MNK|ROG|BER|CLR|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", AegoClasses, sizeof(AegoClasses), INIFileName);
		GetPrivateProfileString(INISection, "SymbolClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SymbolClasses, sizeof(SymbolClasses), INIFileName);
		GetPrivateProfileString(INISection, "BrellsClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", BrellsClasses, sizeof(BrellsClasses), INIFileName);
	}
	if (Class == 4) //rng
	{
		GetPrivateProfileString(INISection, "SkinClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SkinClasses, sizeof(SkinClasses), INIFileName);
		GetPrivateProfileString(INISection, "PredClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", PredClasses, sizeof(PredClasses), INIFileName);
		GetPrivateProfileString(INISection, "StrengthClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", StrengthClasses, sizeof(StrengthClasses), INIFileName);
		GetPrivateProfileString(INISection, "DSClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", DSClasses, sizeof(DSClasses), INIFileName);
	}
	if (Class == 6) //dru
	{
		GetPrivateProfileString(INISection, "SkinClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SkinClasses, sizeof(SkinClasses), INIFileName);
		GetPrivateProfileString(INISection, "DSClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", DSClasses, sizeof(DSClasses), INIFileName);
		GetPrivateProfileString(INISection, "RegenClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", RegenClasses, sizeof(RegenClasses), INIFileName);
	}
	if (Class == 10) //shm
	{
		GetPrivateProfileString(INISection, "FocusClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", FocusClasses, sizeof(FocusClasses), INIFileName);
		GetPrivateProfileString(INISection, "HasteClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", HasteClasses, sizeof(HasteClasses), INIFileName);
		GetPrivateProfileString(INISection, "RegenClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", RegenClasses, sizeof(RegenClasses), INIFileName);
	}
	if (Class == 13) //mag
	{
		GetPrivateProfileString(INISection, "DSClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", DSClasses, sizeof(DSClasses), INIFileName);
	}
	if (Class == 14) //enc
	{
		GetPrivateProfileString(INISection, "ClarityClasses", "|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", ClarityClasses, sizeof(ClarityClasses), INIFileName);
		GetPrivateProfileString(INISection, "HasteClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", HasteClasses, sizeof(HasteClasses), INIFileName);
	}
	if (Class == 15) //bst
	{
		GetPrivateProfileString(INISection, "FocusClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", FocusClasses, sizeof(FocusClasses), INIFileName);
		GetPrivateProfileString(INISection, "HasteClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", HasteClasses, sizeof(HasteClasses), INIFileName);
		GetPrivateProfileString(INISection, "SVClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SVClasses, sizeof(SVClasses), INIFileName);
		GetPrivateProfileString(INISection, "SEClasses", "|WAR|MNK|ROG|BER|CLR|DRU|SHM|RNG|BST|PAL|SHD|BRD|ENC|MAG|NEC|WIZ|", SEClasses, sizeof(SEClasses), INIFileName);
		GetPrivateProfileString(INISection, "FeroClasses", "|WAR|MNK|ROG|BER|RNG|BST|PAL|SHD|BRD|", FeroClasses, sizeof(FeroClasses), INIFileName);
	}
	int MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
	sprintf_s(szTemp, "%.4f", botVersion);
	WritePrivateProfileString(INISection, "Debugging", DEBUG_DUMPFILE ? "1" : "0", INIFileName);
	WritePrivateProfileString(INISection, "BotVersion", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "BotCommandsOn", BotCommandsOn, INIFileName);
	WritePrivateProfileString(INISection, "BotCommandsOff", BotCommandsOff, INIFileName);
	_itoa_s(AlwaysCheckAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "AlwaysCheckAdds", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "RestrictedZones", RestrictedZones, INIFileName);
	WritePrivateProfileString(INISection, "BuffPassword", BuffPassword, INIFileName);
	WritePrivateProfileString(INISection, "AuthorizedUsers", AuthorizedUsers, INIFileName);
	_itoa_s(CycleDelay, szTemp, 10);
	WritePrivateProfileString(INISection, "CycleDelay", szTemp, INIFileName);
	_itoa_s(DefaultGem, szTemp, 10);
	WritePrivateProfileString(INISection, "DefaultGem", szTemp, INIFileName);
	_itoa_s(AnnounceEcho, szTemp, 10);
	WritePrivateProfileString(INISection, "AnnounceEcho", szTemp, INIFileName);
	_itoa_s(AnnounceEQBC, szTemp, 10);
	WritePrivateProfileString(INISection, "AnnounceEQBC", szTemp, INIFileName);
	_itoa_s(AnnounceAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "AnnounceAdds", szTemp, INIFileName);
	_itoa_s(AutoInvisDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "AutoInvisDistance", szTemp, INIFileName);
	_itoa_s(AssistAt, szTemp, 10);
	WritePrivateProfileString(INISection, "AssistAt", szTemp, INIFileName);
	_itoa_s(AssistRange, szTemp, 10);
	WritePrivateProfileString(INISection, "AssistRange", szTemp, INIFileName);
	_itoa_s(UseManualAssist, szTemp, 10);
	WritePrivateProfileString(INISection, "UseManualAssist", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "AssistName", AssistName, INIFileName);
	_itoa_s(MercAssistAt, szTemp, 10);
	WritePrivateProfileString(INISection, "MercAssistAt", szTemp, INIFileName);
	_itoa_s(MercAssistRange, szTemp, 10);
	WritePrivateProfileString(INISection, "MercAssistRange", szTemp, INIFileName);
	_itoa_s(MeleeAt, szTemp, 10);
	WritePrivateProfileString(INISection, "MeleeAt", szTemp, INIFileName);
	_itoa_s(MeleeDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "MeleeDistance", szTemp, INIFileName);
	_itoa_s(DoPetSwarm, szTemp, 10);
	WritePrivateProfileString(INISection, "DoPetSwarm", szTemp, INIFileName);
	_itoa_s(PetAttackAt, szTemp, 10);
	WritePrivateProfileString(INISection, "PetAttackAt", szTemp, INIFileName);
	_itoa_s(PetAttackRange, szTemp, 10);
	WritePrivateProfileString(INISection, "PetAttackRange", szTemp, INIFileName);
	_itoa_s(RepositionIfSummoned, szTemp, 10);
	WritePrivateProfileString(INISection, "RepositionIfSummoned", szTemp, INIFileName);
	_itoa_s(MedStartAt, szTemp, 10);
	WritePrivateProfileString(INISection, "MedStartAt", szTemp, INIFileName);
	_itoa_s(MedStopAt, szTemp, 10);
	WritePrivateProfileString(INISection, "MedStopAt", szTemp, INIFileName);
	_itoa_s(MedToFull, szTemp, 10);
	WritePrivateProfileString(INISection, "MedToFull", szTemp, INIFileName);
	_itoa_s(MedDuringCombat, szTemp, 10);
	WritePrivateProfileString(INISection, "MedDuringCombat", szTemp, INIFileName);
	_itoa_s(EndMedStartAt, szTemp, 10);
	WritePrivateProfileString(INISection, "EndMedStartAt", szTemp, INIFileName);
	_itoa_s(EndMedStopAt, szTemp, 10);
	WritePrivateProfileString(INISection, "EndMedStopAt", szTemp, INIFileName);
	_itoa_s(EndMedToFull, szTemp, 10);
	WritePrivateProfileString(INISection, "EndMedToFull", szTemp, INIFileName);
	_itoa_s(MinManaToNuke, szTemp, 10);
	WritePrivateProfileString(INISection, "MinManaToNuke", szTemp, INIFileName);
	_itoa_s(NukeToTAt, szTemp, 10);
	WritePrivateProfileString(INISection, "NukeToTAt", szTemp, INIFileName);
	_itoa_s(ManaToSummon, szTemp, 10);
	WritePrivateProfileString(INISection, "ManaToSummon", szTemp, INIFileName);
	_itoa_s(SummonModRods, szTemp, 10);
	WritePrivateProfileString(INISection, "SummonModRods", szTemp, INIFileName);
	_itoa_s(UseModRods, szTemp, 10);
	WritePrivateProfileString(INISection, "UseModRods", szTemp, INIFileName);
	_itoa_s(AACutoffTime, szTemp, 10);
	WritePrivateProfileString(INISection, "AACutoffTime", szTemp, INIFileName);
	_itoa_s(AAStopAt, szTemp, 10);
	WritePrivateProfileString(INISection, "AAStopAt", szTemp, INIFileName);
	_itoa_s(AggroAt, szTemp, 10);
	WritePrivateProfileString(INISection, "AggroAt", szTemp, INIFileName);
	_itoa_s(AutofireAt, szTemp, 10);
	WritePrivateProfileString(INISection, "AutofireAt", szTemp, INIFileName);
	_itoa_s(AutofireDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "AutofireDistance", szTemp, INIFileName);
	_itoa_s(DebuffAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "DebuffAdds", szTemp, INIFileName);
	_itoa_s(DebuffAt, szTemp, 10);
	WritePrivateProfileString(INISection, "DebuffAt", szTemp, INIFileName);
	_itoa_s(DebuffStopAt, szTemp, 10);
	WritePrivateProfileString(INISection, "DebuffStopAt", szTemp, INIFileName);
	_itoa_s(DebuffAEAmount, szTemp, 10);
	WritePrivateProfileString(INISection, "DebuffAEAmount", szTemp, INIFileName);
	_itoa_s(DotAt, szTemp, 10);
	WritePrivateProfileString(INISection, "DotAt", szTemp, INIFileName);
	_itoa_s(DotStopAt, szTemp, 10);
	WritePrivateProfileString(INISection, "DotStopAt", szTemp, INIFileName);
	_itoa_s(FaceAngle, szTemp, 10);
	WritePrivateProfileString(INISection, "FaceAngle", szTemp, INIFileName);
	_itoa_s(FadeAt, szTemp, 10);
	WritePrivateProfileString(INISection, "FadeAt", szTemp, INIFileName);
	_itoa_s(GrabAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "GrabAdds", szTemp, INIFileName);
	_itoa_s(GrabPrimary, szTemp, 10);
	WritePrivateProfileString(INISection, "GrabPrimary", szTemp, INIFileName);
	_itoa_s(InterruptToHealAt, szTemp, 10);
	WritePrivateProfileString(INISection, "InterruptToHealAt", szTemp, INIFileName);
	_itoa_s(HealAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealAt", szTemp, INIFileName);
	_itoa_s(HealToTAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealToTAt", szTemp, INIFileName);
	_itoa_s(HealSelfAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealSelfAt", szTemp, INIFileName);
	_itoa_s(HealDelayedAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealDelayedAt", szTemp, INIFileName);
	_itoa_s(HealDurationAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealDurationAt", szTemp, INIFileName);
	_itoa_s(HealGroupAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealGroupAt", szTemp, INIFileName);
	_itoa_s(HealPetAt, szTemp, 10);
	WritePrivateProfileString(INISection, "HealPetAt", szTemp, INIFileName);
	_itoa_s(ImHitAt, szTemp, 10);
	WritePrivateProfileString(INISection, "ImHitAt", szTemp, INIFileName);
	_itoa_s(JoltAt, szTemp, 10);
	WritePrivateProfileString(INISection, "JoltAt", szTemp, INIFileName);
	_itoa_s(KnockbackAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "KnockbackAdds", szTemp, INIFileName);
	_itoa_s(KnockbackPrimary, szTemp, 10);
	WritePrivateProfileString(INISection, "KnockbackPrimary", szTemp, INIFileName);
	_itoa_s(LifetapAt, szTemp, 10);
	WritePrivateProfileString(INISection, "LifetapAt", szTemp, INIFileName);
	_itoa_s(LootInCombat, szTemp, 10);
	WritePrivateProfileString(INISection, "LootInCombat", szTemp, INIFileName);
	_itoa_s(LootRadius, szTemp, 10);
	WritePrivateProfileString(INISection, "LootRadius", szTemp, INIFileName);
	_itoa_s(ManaAt, szTemp, 10);
	WritePrivateProfileString(INISection, "ManaAt", szTemp, INIFileName);
	_itoa_s(NukeAt, szTemp, 10);
	WritePrivateProfileString(INISection, "NukeAt", szTemp, INIFileName);
	_itoa_s(RezRadius, szTemp, 10);
	WritePrivateProfileString(INISection, "RezRadius", szTemp, INIFileName);
	_itoa_s(RootAdds, szTemp, 10);
	WritePrivateProfileString(INISection, "RootAdds", szTemp, INIFileName);
	_itoa_s(RootPrimary, szTemp, 10);
	WritePrivateProfileString(INISection, "RootPrimary", szTemp, INIFileName);
	_itoa_s(SnareAt, szTemp, 10);
	WritePrivateProfileString(INISection, "SnareAt", szTemp, INIFileName);
	_itoa_s(SwarmAt, szTemp, 10);
	WritePrivateProfileString(INISection, "SwarmAt", szTemp, INIFileName);
	_itoa_s(UseAutoFire, szTemp, 10);
	WritePrivateProfileString(INISection, "UseAutoFire", szTemp, INIFileName);
	_itoa_s(UseAENukes, szTemp, 10);
	WritePrivateProfileString(INISection, "UseAENukes", szTemp, INIFileName);
	_itoa_s(UseAura, szTemp, 10);
	WritePrivateProfileString(INISection, "UseAura", szTemp, INIFileName);
	_itoa_s(UseClickyBuffs, szTemp, 10);
	WritePrivateProfileString(INISection, "UseClickyBuffs", szTemp, INIFileName);
	_itoa_s(UseClickyNukes, szTemp, 10);
	WritePrivateProfileString(INISection, "UseClickyNukes", szTemp, INIFileName);
	_itoa_s(DebuffsSpellFirst, szTemp, 10);
	WritePrivateProfileString(INISection, "DebuffsSpellFirst", szTemp, INIFileName);
	if (MyClass == 1 || MyClass == 3 || MyClass == 4 || MyClass == 5 || MyClass == 7 || MyClass == 8 || MyClass == 9 || MyClass == 15 || MyClass == 16)
	{
		_itoa_s(UseDiscsOffensive, szTemp, 10);
		WritePrivateProfileString(INISection, "UseDiscsOffensive", szTemp, INIFileName);
		_itoa_s(UseDiscsDefensive, szTemp, 10);
		WritePrivateProfileString(INISection, "UseDiscsDefensive", szTemp, INIFileName);
	}
	_itoa_s(UseFightBuffs, szTemp, 10);
	WritePrivateProfileString(INISection, "UseFightBuffs", szTemp, INIFileName);
	_itoa_s(UseMainTankBuffs, szTemp, 10);
	WritePrivateProfileString(INISection, "UseMainTankBuffs", szTemp, INIFileName);
	_itoa_s(UseManaInCombat, szTemp, 10);
	WritePrivateProfileString(INISection, "UseManaInCombat", szTemp, INIFileName);
	_itoa_s(UseManaOnOthers, szTemp, 10);
	WritePrivateProfileString(INISection, "UseManaOnOthers", szTemp, INIFileName);
	_itoa_s(UseMez, szTemp, 10);
	WritePrivateProfileString(INISection, "UseMez", szTemp, INIFileName);
	_itoa_s(iUsePet, szTemp, 10);
	WritePrivateProfileString(INISection, "UsePet", szTemp, INIFileName);
	_itoa_s(PetReagentCheck, szTemp, 10);
	WritePrivateProfileString(INISection, "PetReagentCheck", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "PetIllusionItem", PetIllusionItem, INIFileName);
	WritePrivateProfileString(INISection, "CustomPetSpell", CustomPetSpell, INIFileName);
	_itoa_s(UsePetBuffs, szTemp, 10);
	WritePrivateProfileString(INISection, "UsePetBuffs", szTemp, INIFileName);
	_itoa_s(StandOnPetAttack, szTemp, 10);
	WritePrivateProfileString(INISection, "StandOnPetAttack", szTemp, INIFileName);
	_itoa_s(UseSelfBuffs, szTemp, 10);
	WritePrivateProfileString(INISection, "UseSelfBuffs", szTemp, INIFileName);
	_itoa_s(AutoFollowLeader, szTemp, 10);
	WritePrivateProfileString(INISection, "AutoFollowLeader", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "AutoFollowSettings", AutoFollowSettings, INIFileName);
	_itoa_s(UseNetBots, szTemp, 10);
	WritePrivateProfileString(INISection, "UseNetBots", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "NetBotsName", NetBotsName, INIFileName);
	_itoa_s(DoDebug, szTemp, 10);
	WritePrivateProfileString(INISection, "DoDebug", szTemp, INIFileName);
	_itoa_s(PullingDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "PullingDistance", szTemp, INIFileName);
	_itoa_s(bAmIPuller, szTemp, 10);
	WritePrivateProfileString(INISection, "DoPulls", szTemp, INIFileName);
	_itoa_s(MQ2NavDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "MQ2NavDistance", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "PullSkillName", PullSkill, INIFileName);
	_itoa_s(AnnounceBehaviorChange, szTemp, 10);
	WritePrivateProfileString(INISection, "AnnounceBehaviorChange", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "AttackCommand", AttackCommand, INIFileName);
	if (BardClass)
	{
		WritePrivateProfileString(INISection, "RestTwist", RestTwist, INIFileName);
		WritePrivateProfileString(INISection, "CombatTwist", CombatTwist, INIFileName);
	}
	if (Class == 2) //cleric
	{
		WritePrivateProfileString(INISection, "AegoClasses", AegoClasses, INIFileName);
		WritePrivateProfileString(INISection, "SymbolClasses", SymbolClasses, INIFileName);
	}
	if (Class == 3) //pal
	{
		WritePrivateProfileString(INISection, "AegoClasses", AegoClasses, INIFileName);
		WritePrivateProfileString(INISection, "SymbolClasses", SymbolClasses, INIFileName);
		WritePrivateProfileString(INISection, "BrellsClasses", BrellsClasses, INIFileName);
	}
	if (Class == 4) //rng
	{
		WritePrivateProfileString(INISection, "SkinClasses", SkinClasses, INIFileName);
		WritePrivateProfileString(INISection, "PredClasses", PredClasses, INIFileName);
		WritePrivateProfileString(INISection, "StrengthClasses", StrengthClasses, INIFileName);
		WritePrivateProfileString(INISection, "DSClasses", DSClasses, INIFileName);
	}
	if (Class == 6) //dru
	{
		WritePrivateProfileString(INISection, "SkinClasses", SkinClasses, INIFileName);
		WritePrivateProfileString(INISection, "RegenClasses", RegenClasses, INIFileName);
		WritePrivateProfileString(INISection, "DSClasses", DSClasses, INIFileName);
	}
	if (Class == 10) //shm
	{
		WritePrivateProfileString(INISection, "FocusClasses", FocusClasses, INIFileName);
		WritePrivateProfileString(INISection, "RegenClasses", RegenClasses, INIFileName);
		WritePrivateProfileString(INISection, "HasteClasses", HasteClasses, INIFileName);
	}
	if (Class == 13) //mag
	{
		WritePrivateProfileString(INISection, "DSClasses", DSClasses, INIFileName);
	}
	if (Class == 14) //enc
	{
		WritePrivateProfileString(INISection, "HasteClasses", HasteClasses, INIFileName);
		WritePrivateProfileString(INISection, "ClarityClasses", ClarityClasses, INIFileName);
	}
	if (Class == 15) //bst
	{
		WritePrivateProfileString(INISection, "HasteClasses", HasteClasses, INIFileName);
		WritePrivateProfileString(INISection, "FocusClasses", FocusClasses, INIFileName);
		WritePrivateProfileString(INISection, "SVClasses", SVClasses, INIFileName);
		WritePrivateProfileString(INISection, "SEClasses", SEClasses, INIFileName);
		WritePrivateProfileString(INISection, "FeroClasses", FeroClasses, INIFileName);
	}
	_itoa_s(SafetyInvisOnWarp, szTemp, 10);
	WritePrivateProfileString(INISection, "SafetyInvisOnWarp", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "SafetyCommandOnTell", SafetyCommandOnTell, INIFileName);
	WritePrivateProfileString(INISection, "SafetyCommandOnGM", SafetyCommandOnGM, INIFileName);
	WritePrivateProfileString(INISection, "SafetyCommandOnMyWarp", SafetyCommandOnMyWarp, INIFileName);
	WritePrivateProfileString(INISection, "SafetyCommandOnOthersWarp", SafetyCommandOnOthersWarp, INIFileName);
	WritePrivateProfileString(INISection, "SafetyCommandOnTooFarFromCamp", SafetyCommandOnTooFarFromCamp, INIFileName);
}
