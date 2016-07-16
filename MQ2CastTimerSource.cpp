#ifdef taco
#ifndef PLUGIN_API
#define MMOBUGS_LOADER
#include "../MQ2Plugin.h"
#include "../MQ2MMOBugs/CSL.h"
#include "../Blech/Blech.h"
#endif


#define       DELAY_CAST    12000
#define       DELAY_MEMO     6000
#define       DELAY_STOP     3000

#define       CAST_SUCCESS      0
#define       CAST_INTERRUPTED  1
#define       CAST_RESIST       2
#define       CAST_COLLAPSE     3
#define       CAST_RECOVER      4
#define       CAST_FIZZLE       5
#define       CAST_STANDING     6
#define       CAST_STUNNED      7
#define       CAST_INVISIBLE    8
#define       CAST_NOTREADY     9
#define       CAST_OUTOFMANA   10
#define       CAST_OUTOFRANGE  11
#define       CAST_NOTARGET    12
#define       CAST_CANNOTSEE   13
#define       CAST_COMPONENTS  14
#define       CAST_OUTDOORS    15
#define       CAST_TAKEHOLD    16
#define       CAST_IMMUNE      17
#define       CAST_DISTRACTED  18
#define       CAST_ABORTED     19
#define       CAST_UNKNOWN     20

#define       FLAG_COMPLETE     0 
#define       FLAG_REQUEST     -1
#define       FLAG_PROGRESS1   -2 
#define       FLAG_PROGRESS2   -3 
#define       FLAG_PROGRESS3   -4 
#define       FLAG_PROGRESS4   -5 

#define       DONE_COMPLETE    -3
#define       DONE_ABORTED     -2 
#define       DONE_PROGRESS    -1 
#define       DONE_SUCCESS      0

#define       TYPE_SPELL        1
#define       TYPE_ALT          2
#define       TYPE_ITEM         3

#define       RECAST_DEAD       2
#define       RECAST_LAND       1
#define       RECAST_ZERO       0

#define       NOID             -1

long          CastingC = NOID;           // Casting Current ID
long          CastingO = NOID;              // Casting OnTarget


#include <vector>
#include <list>

#define SPELLDURATION 4
#define MAXSPELLLEVEL MAX_PC_LEVEL
#define EVENTSLENGTH 75
#define SPAWNSEARCHLOOP 100
#define TARGET_SEARCH_ZRADIUS 40.0f
#define TARGET_SEARCH_RADIUS 200.0f
#define SPELLTIMER_SEARCH_ZRADIUS 100.0f
#define SPELLTIMER_SEARCH_RADIUS 200.0f
#define CASTERANY 0
#define CASTERME 1
#define CASTEROTHER 2
#define CASTEREVENT 3

											// CastTimer Function Definitions
VOID MQTimerCommand(PSPAWNINFO pChar, PCHAR szLine);

VOID ParseSpellsOnce();
PCONTENTS ItemWithSpell(long SpellID);
VOID ClearAll();
VOID ClearTimer(long SpawnID, eSpawnType SpawnType);
VOID AddSpell(int TargetID, int SpellID, int TickE);
VOID AddSpawnTimer(int SpawnID);

bool VerifyTarget(long SpawnID, PSPELL pSpell);
VOID AddSpellTimer(int SpawnID, PSPELL pSpell, int TickE, bool ignoreMod = false);

CHAR Buffer[MAX_STRING] = { 0 };					// Buffer for String manipulation

bool isParsed = false;							// "END" the plugin when an error happens
bool winActive = true;
bool bInGame = false;

long OldSpawnID = 0;								// Previous Spawn ID

time_t lastRun = 0;
long lastTargetID = 0;							// ID of curent Last Target
long currentTargetID = 0;						// ID of curent Target
long TimerCount = 0;							// Timer ID Count
ULONGLONG UITimer = 0;

Blech			CASTONANOTHER('#');			// BTree List for OnChat CASTONANOTHER
Blech			CASTWORNOFF('#');			// BTree List for OnChat CASTWORNOFF
Blech			CUSTOMEVENT('#');			// BTree List for OnChat CUSTOMEVENT
Blech			SUCCESS('#');				// BTree List for OnChat Message on SUCCESS Detection

long CastingLeft()
{
	long CL = 0;
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		CL = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp;
		if (CL<1) CL = 1;
	}
	return CL;
}

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
vector<long> XmlList;

VOID ClearOne(list<TimerSingle>::iterator &CurList);
VOID DefaultTimer(TimerSingle &MyData);
VOID SetTimerData(TimerSingle &MyData, list<TimerSingle>::iterator &CurList);
long getSpellTimer(long SpawnID, int Caster, vector<PSPELL> SpellSearch);
eSpawnType SpawnType(PSPAWNINFO tSpawn);
VOID RestackTimer(list<TimerSingle>::iterator &CurList, TimerSingle MyData, int Remove);

PCONTENTS ItemWithSpell(long SpellID)
{
	for (int iSlot = 0; iSlot < NUM_INV_SLOTS; iSlot++) {
		if (GetCharInfo2()->pInventoryArray) {
			if (PCONTENTS cSlot = GetCharInfo2()->pInventoryArray->InventoryArray[iSlot]) {
				if (GetItemFromContents(cSlot)) {
					if (SpellID == GetItemFromContents(cSlot)->Clicky.SpellID) {
						return cSlot;
					}
					else if (GetItemFromContents(cSlot)->Type == ITEMTYPE_PACK) {
						for (int iPack = 0; iPack < GetItemFromContents(cSlot)->Slots; iPack++) {
							if (cSlot->pContentsArray && cSlot->pContentsArray->Contents) {
								if (PCONTENTS cPack = cSlot->pContentsArray->Contents[iPack])
									if (GetItemFromContents(cPack))
										if (SpellID == GetItemFromContents(cPack)->Clicky.SpellID)
											return cPack;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

eSpawnType SpawnType(PSPAWNINFO tSpawn)
{
	if (!tSpawn) return NONE;
	eSpawnType eST = GetSpawnType(tSpawn);
	if (eST != PET) return eST;
	PSPAWNINFO mST = (PSPAWNINFO)GetSpawnByID(tSpawn->MasterID);
	return (!mST || GetSpawnType(mST) != PC) ? NPC : PET;
}

/*
// Clear all timer's from list
*/
VOID ClearAll()
{
	TimerCount = 0;
	TimerList.clear();
	XmlList.clear();
}

/*
// Clear one timer's from list
*/
VOID ClearOne(list<TimerSingle>::iterator &CurList)
{
	list<TimerSingle>::iterator PosList;
	PosList = CurList;
	CurList++;
	TimerList.erase(PosList);
}

/*
// UpdateBuffs from Target
*/
VOID UpdateBuffsTarget()
{
	static unsigned long TargetChangeTime = clock();
	static long LastTargetID = NOID;

	int i;
	int buffID = 0;
	time_t now = time(NULL);
	time_t thisBuffTimestamp = time(NULL);

	long TargetID = (pTarget) ? ((PSPAWNINFO)pTarget)->SpawnID : NOID;

	if (TargetID == NOID) {
		LastTargetID = NOID;
		return;
	}
	PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(TargetID);

	if (GetSpawnType(pSpawn) != PC && GetSpawnType(pSpawn) != MERCENARY) {
		int Gcount = 0;
		if (!GetCharInfo()->pGroupInfo) return;
		if (GetCharInfo()->pGroupInfo) {
			for (i = 1; i<6; i++) if (GetCharInfo()->pGroupInfo->pMember[i]) Gcount++;
		}
		if (Gcount <= 1) return;
	}

	if (!pConnection)
		return;

	if (LastTargetID != TargetID) {
		LastTargetID = TargetID;
		TargetChangeTime = clock();
		return;
	}
	else if (TargetChangeTime + 200 + pConnection->Last > (unsigned long)clock()) {
		for (i = 0; i < 0x55; i++) {
			buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
			if (buffID != 0xffffffff) {
				if (PSPELL pSpell = GetSpellByID(buffID)) TargetChangeTime = 0;
			}
		}
		if (TargetChangeTime) return;
	}

	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();

	bool spellNotFouned = false;
	//update spell timers on target and removing spells no longer on target.
	CurList = TimerList.begin();
	while (CurList != EndList) {
		spellNotFouned = false;

		if (CurList->SpawnID != NOID && CurList->SpawnID == TargetID && CurList->pSpell && CurList->pSpell->ID  && CurList->pSpell->DurationWindow == 0 && CurList->CastE == CAST_SUCCESS) {
			if (CurList->AutoRecast) {
				spellNotFouned = false;
			}
			else {
				spellNotFouned = true;
			}
			for (i = 0; i < 0x55; i++) {
				buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
				if (buffID != 0xffffffff) {
					if (PSPELL pSpell = GetSpellByID(buffID)) {
						if (CurList->pSpell->ID == pSpell->ID) {
							thisBuffTimestamp = now + (((PCTARGETWND)pTargetWnd)->BuffTimer[i] / 1000);
							if (CurList->timestamp != thisBuffTimestamp) {
								TimerSingle MyData;
								DefaultTimer(MyData);
								MyData.TimerID = CurList->TimerID;
								MyData.pSpell = CurList->pSpell;
								MyData.CastE = CAST_SUCCESS;
								MyData.SpawnID = CurList->SpawnID;
								MyData.timestamp = thisBuffTimestamp;
								MyData.Caster = CurList->Caster;
								MyData.AutoRecast = CurList->AutoRecast;
								RestackTimer(CurList, MyData, 1);
							}
							spellNotFouned = false;
							break;
						}
					}
				}
			}
		}
		if (spellNotFouned) ClearOne(CurList);
		else CurList++;
	}

	//add new spells to target will be added as spells others have casted unless it is spell you can cast on them.
	for (i = 0; i < 0x55; i++) {
		spellNotFouned = true;
		buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
		thisBuffTimestamp = now + (((PCTARGETWND)pTargetWnd)->BuffTimer[i] / 1000);
		if (buffID != 0xffffffff) {
			if (PSPELL pSpell = GetSpellByID(buffID)) {
				if (pSpell->DurationWindow == 0) {
					CurList = TimerList.begin();
					while (CurList != EndList) {
						if (CurList->SpawnID != NOID && CurList->SpawnID == TargetID && CurList->pSpell && CurList->pSpell->ID == pSpell->ID) {
							spellNotFouned = false;
							break;
						}
						CurList++;
					}
					if (spellNotFouned) {
						TimerSingle MyData;
						DefaultTimer(MyData);
						TimerCount++;
						MyData.TimerID = TimerCount;
						MyData.pSpell = pSpell;
						MyData.CastE = CAST_SUCCESS;
						MyData.SpawnID = TargetID;
						MyData.timestamp = thisBuffTimestamp;
						MyData.Caster = CASTEROTHER;
						for (DWORD N = 0; N < NUM_BOOK_SLOTS; N++) {
							if (PSPELL pTempSpell = GetSpellByID(GetCharInfo2()->SpellBook[N])) {
								if (pTempSpell->ID == pSpell->ID) {
									MyData.Caster = CASTERME;
									break;
								}
							}
						}
						if (TargetID == GetCharInfo()->pSpawn->SpawnID && MyData.Caster != CASTERME) {
							if (PCONTENTS pItem = ItemWithSpell(pSpell->ID)) MyData.Caster = CASTERME;
						}
						RestackTimer(CurList, MyData, 0);
					}
				}
			}
		}
	}
}

/*
* Set default timer data
*/
VOID DefaultTimer(TimerSingle &MyData)
{
	MyData.TimerID = NOID;
	MyData.pSpell = NULL;
	MyData.EventID = NOID;
	MyData.CastE = CAST_SUCCESS;
	MyData.SpawnID = NOID;
	MyData.timestamp = 0;
	MyData.Caster = CASTERME;
	MyData.AutoRecast = false;

	char emptyName[64] = { NULL };
	strcpy(MyData.SpawnName, emptyName);

	MyData.TimeOut = 0;
}

/*
* Set timer data from CurList
*/
VOID SetTimerData(TimerSingle &MyData, list<TimerSingle>::iterator &CurList)
{
	MyData.TimerID = CurList->TimerID;
	MyData.pSpell = CurList->pSpell;
	MyData.EventID = CurList->EventID;
	MyData.CastE = CurList->CastE;
	MyData.SpawnID = CurList->SpawnID;
	MyData.timestamp = CurList->timestamp;
	MyData.Caster = CurList->Caster;
	MyData.AutoRecast = CurList->AutoRecast;
	strcpy(MyData.SpawnName, CurList->SpawnName);
	MyData.TimeOut = CurList->TimeOut;
}

/*
// Restack Timer
*/
VOID RestackTimer(list<TimerSingle>::iterator &CurList, TimerSingle MyData, int Remove)
{
	if (Remove) ClearOne(CurList);

	if (Remove > -1) {
		CurList = TimerList.begin();
		list<TimerSingle>::iterator EndList = TimerList.end();
		while (CurList != EndList && MyData.timestamp>CurList->timestamp) CurList++;
		TimerList.insert(CurList, MyData);
	}
	else {
		TimerList.push_back(MyData);
	}
	if (Remove == 1) CurList = TimerList.begin();
}


/*
// Clear timer by ....
*/
VOID ClearTimer(long SpawnID, eSpawnType SpawnType)
{
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) return;
	int ZoneID = 0;
	if (GetCharInfo())
		ZoneID = GetCharInfo()->zoneId;
	if (ZoneID > MAX_ZONES)
		ZoneID &= 0x7FFF;
	if (ZoneID <= 0 || ZoneID >= MAX_ZONES)
		return;
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();
	time_t now = time(NULL);
	TimerSingle MyData;
	int Remove = 0;
	while (CurList != EndList) {
		Remove = 0;
			if (CurList->SpawnID != NOID) {
				if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(CurList->SpawnID)) {
					if ((pSpawn->Type == SPAWN_CORPSE) || SpawnType != NOID && GetSpawnType(pSpawn) == SpawnType) Remove = 1;
				}
			}
			if (CurList->SpawnName[0] || CurList->TimeOut != 0) {
				if (difftime(CurList->TimeOut, now)<0) Remove = 1;
			}
			else if (difftime(CurList->timestamp, now)<0 && CurList->Caster != CASTERME) {
				if (CurList->Caster != CASTERME) Remove = 1;
			}
			else if (CurList->SpawnID == SpawnID && SpawnID != NOID) Remove = 1;
		if (Remove == 2) RestackTimer(CurList, MyData, -1);
		else if (Remove) ClearOne(CurList);
		else CurList++;
	}
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
			AddSpellTimer(GetCharInfo()->pSpawn->SpawnID, pSpell, TickE);
			break;
		case 14: // Pet
			AddSpellTimer(GetCharInfo()->pSpawn->PetID, pSpell, TickE);
			break;
		case  5: // Single
		case  8: // Targeted AE
			if (VerifyTarget(TargetID, pSpell)) AddSpellTimer(TargetID, pSpell, TickE);
			break;
		default: break; // Unknown
		}
	}
}

/*
//	Verity Target is correct for spell when the spell is casted
//	from MQ2Cast using /casting
case 2: // Beneficial(Group) , case 1: // Beneficial , case 0: // Detrimental , default: // Unknown
*/
bool VerifyTarget(long SpawnID, PSPELL pSpell)
{
	if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(SpawnID)) {
		if (pSpawn->Type == SPAWN_CORPSE) return false;
		switch (SpawnType(pSpawn)) {
		case NPC: // NPC
			switch (pSpell->SpellType) {
			case 2: // Beneficial(Group)
			case 1: // Beneficial				
				return false;
			case 0: // Detrimental
			default: // Unknown
				return true;
			}
			break;
		case PET: // PET
			switch (pSpell->SpellType) {
			case 2: // Beneficial(Group)
			case 1: // Beneficial
				return true;
			case 0: // Detrimental
				return false;
			default: // Unknown
				return true;
			}
			break;
		case NONE: // NONE
			switch (pSpell->SpellType) {
			case 2: // Beneficial(Group)
			case 1: // Beneficial
				return true;
			case 0: // Detrimental
				return false;
			default: // Unknown
				return true;
			}
			break;
		default: // Unknown / PC / MERCENARY
			switch (GetSpawnType(pSpawn)) {
			case PC: // PC
				switch (pSpell->SpellType) {
				case 2: // Beneficial(Group)
				case 1: // Beneficial
					return true;
				case 0: // Detrimental
					return false;
				default: // Unknown
					return true;
				}
				break;
			case MERCENARY: // MERCENARY
				switch (pSpell->SpellType) {
				case 2: // Beneficial(Group)
				case 1: // Beneficial
					return true;
				case 0: // Detrimental
					return false;
				default: // Unknown
					return true;
				}
				break;
			default: // Unknown
				return true;
			}
			break;
		}
		return true;
	}
	return false;
}

/*
//	Add spell timer to spell timer list
*/
VOID AddSpellTimer(int SpawnID, PSPELL pSpell, int TickE, bool ignoreMod)
{
	if (!SpawnID || SpawnID == NOID || !pSpell->ID || !gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) return;
#if DEBUGGING_INGAME
	DebugSpewAlways("MQ2CastTimer::AddSpellTimer()::Spell: %s To %d", pSpell->Name, SpawnID);
	if (ignoreMod) WriteChatf("NOT MINE");
	else WriteChatf("MINE");
#endif

	time_t now = time(NULL);
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();
	switch (TickE) {
	case CAST_SUCCESS: // CAST_SUCCESS
		now = now + CalcDuration(pSpell, ignoreMod);
		break;
	case CAST_IMMUNE: // CAST_IMMUNE
		now = now + 100000;
		break;
	case CAST_TAKEHOLD: // CAST_TAKEHOLD
		now = now + CalcDuration(pSpell, ignoreMod);
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
//	Add event timer to spell timer list
*/
VOID AddEventTimer(int EventNum, int SpawnID)
{
	time_t now = time(NULL);
	now = now + EventList[EventNum].Duration;
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();
	TimerSingle MyData;
	DefaultTimer(MyData);

	while (CurList != EndList) {
		if (CurList->SpawnID && CurList->EventID == EventNum && CurList->SpawnID == SpawnID) {
			ClearOne(CurList);
			break;
		}
		else CurList++;
	}
	TimerCount++;
	MyData.TimerID = TimerCount;
	MyData.EventID = EventNum;
	MyData.SpawnID = SpawnID;
	MyData.timestamp = now;
	MyData.Caster = CASTEREVENT;

	RestackTimer(CurList, MyData, 0);
}


/*
//	Handle Spell Casting done by yourself and call AddSpell() to
//	add a spell and call CastTimerWnd->UpdateList() to update the Target window
*/
PLUGIN_API VOID TimerCastHandle(int TargetID, int SpellID, long TickE)
{
	if (SpellID && TickE == CAST_RECOVER) TickE = CAST_SUCCESS;
	if (SpellID && TickE == CAST_OUTDOORS) TickE = CAST_IMMUNE;
	if (TickE != CAST_SUCCESS && TickE != CAST_IMMUNE && TickE != CAST_TAKEHOLD) return;
	AddSpell(TargetID, SpellID, TickE);
}

PLUGIN_API VOID OnPulse(VOID)
{
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn)
		return;
	if (!mmo.Active)
		return;

	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();

	if (OldSpawnID != GetCharInfo()->pSpawn->SpawnID) {
		TimerSingle MyData;
		DefaultTimer(MyData);
		char emptyName[64] = { NULL };
		while (CurList != EndList) {
			if (CurList->SpawnID && OldSpawnID == CurList->SpawnID) {
				SetTimerData(MyData, CurList);
				MyData.SpawnID = GetCharInfo()->pSpawn->SpawnID;
				strcpy(MyData.SpawnName, emptyName);

				RestackTimer(CurList, MyData, 1);
			}
			else {
				CurList++;
			}
		}
		OldSpawnID = GetCharInfo()->pSpawn->SpawnID;
	}

	ParseSpellsOnce();


	CastingC = GetCharInfo()->pSpawn->CastingData.SpellID ? GetCharInfo()->pSpawn->CastingData.SpellID : NOID;

	// casting window currently openened?
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		if (CastingO == NOID) CastingO = (pTarget) ? ((long)((PSPAWNINFO)pTarget)->SpawnID) : 0;
	}
	if (CastingC == NOID) CastingO = NOID;

	UpdateBuffsTarget();

	currentTargetID = (pTarget) ? ((long)((PSPAWNINFO)pTarget)->SpawnID) : NOID;
	time_t Now = time(NULL);
	if (Now < lastRun || Now >= lastRun + 1) ClearTimer(NOID, NONE);
}

void __stdcall TimerEventOnAnother(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
	isParsed = true;
	if (!strlen(pValues->Value)) return;
	bool selfCast = false;
	PSPELL pSpell = NULL;
	PSPAWNINFO pSpawn = NULL;			// Output Spawn
	SEARCHSPAWN ssSpawn;				// Search String ( Spawn )
	ClearSearchSpawn(&ssSpawn);
	ssSpawn.ZRadius = SPELLTIMER_SEARCH_ZRADIUS;
	ssSpawn.FRadius = SPELLTIMER_SEARCH_RADIUS;
	for (int i = 0; i<SPAWNSEARCHLOOP; i++) {
		if (pSpawn = NthNearestSpawn(&ssSpawn, i, (PSPAWNINFO)pCharSpawn)) if (pSpawn->Type != SPAWN_CORPSE) {
			if (!_stricmp(pValues->Value, pSpawn->DisplayedName)) {
				//It was not a spell you cast yourself, finding the highest level spell and assuming that is the right one.
				if (pData && SpellEventList[(long)pData].pHSpell && SpellEventList[(long)pData].pHSpell->ID) {
					if (CastingC != NOID && !CastingLeft()) for (long c = 0; c<(long)SpellEventList[(long)pData].pSpells.size(); c++) {
						if (CastingC == SpellEventList[(long)pData].pSpells[c]->ID) {
							selfCast = true;
							pSpell = SpellEventList[(long)pData].pSpells[c];
							break;
						}
					}
					if (!selfCast) pSpell = SpellEventList[(long)pData].pHSpell;
					if (selfCast) {
						if (pSpell->TargetType == 5) AddSpellTimer(CastingO, pSpell, CAST_SUCCESS);
						else AddSpellTimer(pSpawn->SpawnID, pSpell, CAST_SUCCESS);
					}
					else {
						AddSpellTimer(pSpawn->SpawnID, pSpell, CAST_SUCCESS, true);
					}
				}
				return;
			}
		}
	}
}

void __stdcall TimerEventWornOff(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
	isParsed = true;
	char *SpellName = 0;
	char *SpawnName = 0;
	// find spawn name
	PBLECHVALUE pValue = pValues;
	while (pValue) {
		if (!strcmp(pValue->Name, "SpellName")) SpellName = pValue->Value;
		else if (!strcmp(pValue->Name, "SpawnName")) {
			SpawnName = pValue->Value;
			break;
		}
		pValue = pValue->pNext;
	}

	list<TimerSingle>::iterator CurList;
	list<TimerSingle>::iterator EndList;
	list<TimerSingle>::iterator RemoveList;

	int spellCount = 0;
	if (SpellName && SpawnName) {
		CurList = TimerList.begin();
		EndList = TimerList.end();
		while (CurList != EndList) {
			if (CurList->SpawnID && CurList->pSpell && CurList->pSpell->ID && !_stricmp(SpellName, CurList->pSpell->Name)) {
				if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(CurList->SpawnID)) {
					if (!_stricmp(SpawnName, pSpawn->DisplayedName)) {
						RemoveList = CurList;
						if (pSpawn->Type == SPAWN_PLAYER) {
							spellCount = 1;
							break;
						}
						else {
							spellCount++;
							if (spellCount >= 2) return;
						}
					}
				}
			}
			CurList++;
		}
		if (spellCount == 1) {
			ClearOne(RemoveList);
		}
	}
}

/*
Handle incomming Text lines:
Goal is to make a qualified GUESS on what spell was casted
*/
#if MQ2BLECH == 0
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) return 0;
	isParsed = false;
	CUSTOMEVENT.Feed(Line);
	if (!isParsed && Color == 264) CASTONANOTHER.Feed(Line);
	if (!isParsed && Color == 284) CASTWORNOFF.Feed(Line);
	return 0;
}
#endif

bool AcceptedSpells(PSPELL pSpell)
{
	if (GetSpellDuration(pSpell, (PSPAWNINFO)pLocalPlayer) <= SPELLDURATION) return false;
	else if (!pSpell->CastOnAnother[0]) return false;
	else if (!strcmp(pSpell->CastOnAnother, " staggers.")) return false;
	return true;
}

/*
Parser that Select the spells that we want to look for, and save them in SpellEventList
*/
VOID ParseSpellsOnce()
{
	if (!gbInZone || SpellEventList.size() || !GetCharInfo() || !GetCharInfo()->pSpawn) return;

	int i;                  // Index Var's
	int tcount = 0;         // Total # of spells
	int Level = 0;

	vector<int>  LastLevel;
	bool noAdd = false;
	PSPELL pSpell = NULL;

	// Parse through all the spells to find the spells we want to pfind.
	for (DWORD dwSpellID = 0; dwSpellID < TOTAL_SPELL_COUNT; dwSpellID++) {
		pSpell = &(*((PSPELLMGR)pSpellMgr)->Spells[dwSpellID]);
		if (pSpell && pSpell->ID && AcceptedSpells(pSpell)) {
			Level = 0;
			for (int c = 1; c <= 17; c++) {
				if (pSpell->ClassLevel[c] <= MAXSPELLLEVEL && pSpell->ClassLevel[c] > Level) Level = pSpell->ClassLevel[c];
			}
			if (Level) {
				if (pSpell->SpellType == 1 || pSpell->SpellType == 2 || pSpell->SpellType == 0) {
					noAdd = false;
					for (i = 0; i<(int)LastLevel.size(); i++) {
						if (!strcmp(SpellEventList[i].Event, pSpell->CastOnAnother)) {
							if (Level > LastLevel[i]) {
								SpellEventList[i].pHSpell = pSpell;
								LastLevel[i] = Level;
							}
							SpellEventList[i].pSpells.push_back(pSpell);
							noAdd = true;
							break;
						}
					}
					if (!noAdd) {
						SpellEvent MyData;
						strcpy(MyData.Event, pSpell->CastOnAnother);
						MyData.pHSpell = pSpell;
						MyData.pSpells.push_back(pSpell);
						SpellEventList.push_back(MyData);
						LastLevel.push_back(Level);
						sprintf(Buffer, "#*#%s", pSpell->CastOnAnother);
						BleachList.push_back(pMQ2Blech->AddEvent(Buffer, TimerEventOnAnother, (void*)((int)SpellEventList.size() - 1)));
					}
				}
			}
			tcount++;
		}
	}
	strcpy(Buffer, "Your #SpellName# spell has worn off of #SpawnName#.#*#");
	BleachList.push_back(pMQ2Blech->AddEvent(Buffer, TimerEventWornOff, (void*)0));
	DebugSpewAlways("[MQ2CastTimer::ParseSpellsOnce] Total Number of spells %d, Spells Selected %d", tcount, SpellEventList.size());
}

void PluginOn()
{
	if (mmo.Active)
		return;
	mmo.Active = true;
	AddCommand("/timer", MQTimerCommand);
}

void PluginOff()
{
	if (!mmo.Active)
		return;
	mmo.Active = false;
	RemoveCommand("/timer");
}

PLUGIN_API VOID InitializePlugin(VOID)
{
	ClearAll();
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("MQ2CastTimer::ShutdownPlugin()");
	SpellEventList.clear();
	for (unsigned long i = 0; i<BleachList.size(); i++) pMQ2Blech->RemoveEvent(BleachList[i]);
	PluginOff();
}


PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	if (!mmo.Active)
		return;
	int ZoneID = 0;
	if (GetCharInfo())
		ZoneID = GetCharInfo()->zoneId;
	if (ZoneID > MAX_ZONES)
		ZoneID &= 0x7FFF;
	if (ZoneID <= 0 || ZoneID >= MAX_ZONES)
		return;
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();

	time_t now = time(NULL);

	TimerSingle MyData;
	DefaultTimer(MyData);
	while (CurList != EndList) {
		if (CurList->SpawnID && CurList->SpawnID == pSpawn->SpawnID) {
			eSpawnType eST = GetSpawnType(pSpawn);
			if ((eST == PC || eST == MERCENARY) && CurList->pSpell && CurList->pSpell->ID) {
				SetTimerData(MyData, CurList);
				strcpy(MyData.SpawnName, pSpawn->Name);
				MyData.TimeOut = now + 600;
				MyData.SpawnID = NOID;
				RestackTimer(CurList, MyData, -1);
			}
			else {
				ClearOne(CurList);
			}
		}
		else {
			CurList++;
		}
	}
}

PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	if (!mmo.Active)
		return;
	if (!GetCharInfo()->pSpawn) return;
	int ZoneID = 0;
	if (GetCharInfo())
		ZoneID = GetCharInfo()->zoneId;
	if (ZoneID > MAX_ZONES)
		ZoneID &= 0x7FFF;
	if (ZoneID <= 0 || ZoneID >= MAX_ZONES)
		return;
	list<TimerSingle>::iterator CurList = TimerList.begin();
	list<TimerSingle>::iterator EndList = TimerList.end();

	char emptyName[64] = { NULL };

	TimerSingle MyData;
	while (CurList != EndList) {
		DefaultTimer(MyData);
		if (pNewSpawn->SpawnID == GetCharInfo()->pSpawn->SpawnID) OldSpawnID = pNewSpawn->SpawnID;
		if (!_stricmp(pNewSpawn->Name, CurList->SpawnName) && CurList->pSpell && CurList->pSpell->ID) {
			SetTimerData(MyData, CurList);

			strcpy(MyData.SpawnName, emptyName);
			MyData.TimeOut = 0;
			MyData.SpawnID = pNewSpawn->SpawnID;

			RestackTimer(CurList, MyData, 1);
		}
		CurList++;
	}
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
#endif