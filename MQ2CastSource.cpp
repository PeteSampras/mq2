#ifdef taco
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2Bot.cpp    | Set DEBUGGING to true for DEBUGGING msg
// Author: s0rCieR        | 
//         A_Enchanter_00 |
//         htw            |
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Last edited by: htw 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
#include "../MQ2Plugin.h"
#include "../Blech/Blech.h"
#include <vector>
#include <list>


#define       DELAY_STOP     3000
#define       DELAY_PULSE     125

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

PCHAR cast_status[] = { "SUCCESS","INTERRUPTED","RESIST","COLLAPSE","RECOVER","FIZZLE","STANDING","STUNNED","INVISIBLE","NOTREADY",	
"OUTOFMANA","OUTOFRANGE","NOTARGET","CANNOTSEE","COMPONENTS","OUTDOORS","TAKEHOLD","IMMUNE","DISTRACTED","ABORTED",	"UNKNOWN",	NULL };

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

#define       TYPE_SPELLS       1
#define       TYPE_ALT          2
#define       TYPE_ITEMS        3

#define       RECAST_LAND       1

#define       NOID             -1

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

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
unsigned long  DELAY_MEMO = 12000UL;
unsigned long  DELAY_CAST = 20000UL;

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
long          CastR = 0;                 // Cast Retry Counter
long          CastW = 0;                 // Cast Retry Type
char          CastC[MAX_STRING];       // Cast SpellType
char          CastN[MAX_STRING];       // Cast SpellName
PSPELL        CastS = NULL;              // Cast Spell Pointer

bool          Parsed = false;            // BTree List Found Flags
Blech         LIST013('#');            // BTree List for OnChat Message on Color  13
Blech         LIST264('#');            // BTree List for OnChat Message on Color 264
Blech         LIST289('#');            // BTree List for OnChat Message on Color 289
Blech         UNKNOWN('#');            // BTree List for OnChat Message on UNKNOWN Yet Color
Blech         SUCCESS('#');            // BTree List for OnChat Message on SUCCESS Detection

PSPELL        fFIND;                   // SpellFind - Casting Spell Effect
void         *fINFO;                   // SpellFind - Casting Type Structure
int           fTYPE;                   // SpellFind - Casting Type
int           fTIME;                   // SpellFind - Casting Time
PCHAR         fNAME;                   // SpellFind - Casting Name

SPELLFAVORITE SpellToMemorize;         // Favorite Spells Array
long          SpellTotal;              // Favorite Spells Total

PCHAR         ListGems[] = { "1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" };
// REMOVE THIS
VOID DebugWrite(PCHAR szFormat, ...);
inline bool InGameOK()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}
bool twisting = false, BardClass = false;
static inline LONG GetSpellBaseX(PSPELL spell, int slot) {
	return slot < GetSpellNumEffects(spell) ? GetSpellBase(spell, slot) : 0;
}
// END REMOVE
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#define aCastEvent(List,Value,Filter) List.AddEvent(Filter,CastEvent,(void*)Value);

void __stdcall CastEvent(unsigned int ID, void *pData, PBLECHVALUE pValues) {
	Parsed = true;
	if (CastingE<(long)pData) CastingE = (long)pData;
	DebugWrite("[%llu] MQ2Bot:[OnChat]: Result=[%s (%d)] Called=[%d].", MQGetTickCount64(), cast_status[CastingE], CastingE, (long)pData);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PALTABILITY AltAbility(PCHAR ID) {
	if (ID[0]) {
		int Number = IsNumber(ID);
		int Values = atoi(ID);
		for (DWORD nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
			if (GetCharInfo2()->AAList[nAbility].AAIndex) if (PALTABILITY pAbility = pAltAdvManager->GetAAById(GetCharInfo2()->AAList[nAbility].AAIndex)) {
				if (Number) {
					if (pAbility->ID == Values) return pAbility;
				}
				else if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
					if (!_stricmp(ID, pName)) return pAbility;
				}
			}
		}
	}
	return NULL;
}

void Cast(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList); if (!zOutput[0]) return;
	Cast(GetCharInfo()->pSpawn, zOutput);
}

long CastingLeft() {
	long CL = 0;
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		CL = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp;
		if (CL<1) CL = 1;
	}
	return CL;
}
long Evaluate(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList); if (!zOutput[0]) return 1;
	ParseMacroData(zOutput); return atoi(zOutput);
}

void Execute(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf(zOutput, zFormat, vaList); if (!zOutput[0]) return;
	DoCommand(GetCharInfo()->pSpawn, zOutput);
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

long GEMID(DWORD ID) {
	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) if (GetCharInfo2()->MemorizedSpells[GEM] == ID) return GEM;
	return NOID;
}

bool GEMReady(DWORD ID) {
	if (pCastSpellWnd && ID < NUM_SPELL_GEMS)
		if ((long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[ID]->spellicon != NOID)
			if ((long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[ID]->spellstate != 1)
				return true;
	return false;
}

int GiftOfMana() {
	if (InGameOK())
	{
		for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
		{
			if (GetCharInfo2()->ShortBuff[nBuff].SpellID > 0)
			{
				PSPELL tempSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID);
				if (tempSpell && (strstr(tempSpell->Name, "Gift of ") && strstr(tempSpell->Name, "Mana") || strstr(tempSpell->Name, "Gracious Mana")))
				{
					return GetSpellBaseX(tempSpell, 2);
				}
			}
		}
	}
	return 0;
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

PSPELL SpellBook(PCHAR ID) {
	if (ID[0]) {
		if (IsNumber(ID)) {
			int Number = atoi(ID);
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				if (GetCharInfo2()->SpellBook[nSpell] == Number)
					return GetSpellByID(Number);
		}
		else {
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
					if (!_stricmp(ID, pSpell->Name)) return pSpell;
		}
	}
	return NULL;
}
PCONTENTS     fPACK = 0;                 // ItemFound/ItemSearch - Find Pack Contents
PCONTENTS     fITEM = 0;                 // ItemFound/ItemSearch - Find Item Contents
long          fSLOT = 0;                 // ItemFound/ItemSearch - Find Item SlotID
bool SpellFind(PCHAR ID, PCHAR TYPE) {
	DWORD n = 0;
	if (ID[0]) {
		// assume it's an alt ability
		if (!TYPE[0] || !strnicmp(TYPE, "alt", 3)) {
			if (PALTABILITY Search = AltAbility(ID)) {
				if (PSPELL spell = GetSpellByID(Search->SpellID)) {
					fFIND = spell;
					fINFO = Search;
					fTIME = fFIND->CastTime;
					fNAME = (PCHAR)fFIND->Name;
					fTYPE = TYPE_ALT;
					return true;
				}
			}
		}
		// assume it's a spell
		if (!TYPE[0] || !strnicmp(TYPE, "gem", 3) || IsNumber(TYPE)) {
			if (PSPELL Search = SpellBook(ID)) {
				fFIND = Search;
				fINFO = Search;
				fTIME = pCharData1->GetAACastingTimeModifier((EQ_Spell*)fFIND) +
					pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)fFIND, (EQ_Equipment**)&n, 0) +
					fFIND->CastTime;
				fNAME = (PCHAR)fFIND->Name;
				fTYPE = TYPE_SPELLS;
				return true;
			}
		}
		// assume it's an item clicky
		if (ItemSearch(ID, 0, NUM_INV_SLOTS)) if (GetSpellByID(GetItemFromContents(fITEM)->Clicky.SpellID)) {
			fFIND = GetSpellByID(GetItemFromContents(fITEM)->Clicky.SpellID);
			fINFO = fITEM;
			fTIME = GetItemFromContents(fITEM)->Clicky.CastTime;
			if (BardClass)
				fTIME = GetItemFromContents(fITEM)->Clicky.CastTime + 1;
			else
				fTIME = GetItemFromContents(fITEM)->Clicky.CastTime;
			fNAME = (PCHAR)GetItemFromContents(fITEM)->Name;
			fTYPE = TYPE_ITEMS;
			return true;
		}
	}
	fFIND = NULL;
	fINFO = NULL;
	fTYPE = 0;
	return false;
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

bool Moving() {
	ULONGLONG MyTimer = MQGetTickCount64();
	if (Speed() != 0.0f) ImmobileT = MyTimer + (ULONGLONG)500;
	return (!MQ2Globals::gbMoving && (!ImmobileT || MyTimer>ImmobileT));
}

BOOL Open(PCHAR WindowName) {
	PCSIDLWND pWnd = (PCSIDLWND)FindMQ2Window(WindowName);
	return (!pWnd) ? false : (BOOL)pWnd->dShow;
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
	CastW = 0;                 // Cast Retry Type
	Invisible = false;         // Invisibility Check?
	ZeroMemory(&SpellToMemorize, sizeof(SPELLFAVORITE));
	strcpy(SpellToMemorize.Name, "Mem a Spell");
	SpellToMemorize.inuse = 1;
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
	SpellTotal = 0;
}


PSPELL SpellBook(PCHAR ID) {
	if (ID[0]) {
		if (IsNumber(ID)) {
			int Number = atoi(ID);
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				if (GetCharInfo2()->SpellBook[nSpell] == Number)
					return GetSpellByID(Number);
		}
		else {
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++)
				if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell]))
					if (!_stricmp(ID, pSpell->Name)) return pSpell;
		}
	}
	return NULL;
}


long SpellTimer(long Type, void *Data) {
	int Ready;
	switch (Type) {
	case TYPE_SPELLS:
		if (GEMReady(GEMID(((PSPELL)Data)->ID))) return 0;
		if (BardClass) return 2;
		if (GetCharInfo()->pSpawn->Level<4) return (long)((PSPELL)Data)->FizzleTime * 2;
		return (long)((PSPELL)Data)->FizzleTime;
	case TYPE_ALT:
		if (pAltAdvManager->GetCalculatedTimer(pPCData, (PALTABILITY)Data)>0) {
			pAltAdvManager->IsAbilityReady(pPCData, (PALTABILITY)Data, &Ready);
			return (Ready<1) ? 0 : Ready * 1000;
		}
		return 999999;
	}
	return 999999;
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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

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
				if (GetCharInfo()->pSpawn->Level<4 && DELAY_MEMO<17000UL)
					DELAY_MEMO = 17000UL;
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

void CastHandle() {

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
			long TimeReady = SpellTimer(CastK, CastI);    // get estimate time before it's ready.
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
			if (CastK == TYPE_SPELLS)     Execute("/multiline ; /stopsong ; /cast \"%s\"", CastN);
			else if (CastK == TYPE_ALT) {
					Execute("/alt activate %d", ((PALTABILITY)CastI)->ID);
			}
		}
		else {
			if (CastK == TYPE_SPELLS)     Cast("\"%s\"", CastN);
			else if (CastK == TYPE_ALT)  Execute("/alt activate %d", ((PALTABILITY)CastI)->ID);
		}
	}
}





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
//	from MQ2Bot using /casting
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
void CastTimer(int TargetID, int SpellID, long TickE)
{
	if (SpellID && TickE == CAST_RECOVER) TickE = CAST_SUCCESS;
	if (SpellID && TickE == CAST_OUTDOORS) TickE = CAST_IMMUNE;
	if (TickE != CAST_SUCCESS && TickE != CAST_IMMUNE && TickE != CAST_TAKEHOLD) return;
	AddSpell(TargetID, SpellID, TickE);
}

PLUGIN_API VOID OnPulse(VOID) {

	if (gbInZone && MQGetTickCount64()>CastingP && GetCharInfo() && GetCharInfo()->pSpawn) {
		CastingP = MQGetTickCount64() + DELAY_PULSE;

		// evaluate immobile flag and handle immobilize request
		Immobile = Moving();
		if (StopF != FLAG_COMPLETE) StopHandle();
		CastingD = GetCharInfo()->pSpawn->CastingData.SpellID;

		// casting window currently openened?
		if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
			Casting = true;
			if (CastingO == NOID) CastingO = (pTarget) ? ((long)((PSPAWNINFO)pTarget)->SpawnID) : 0;

			// was this an unecpected cast?
			if (CastingD != CastingC && CastingD != NOID) {
				CastingE = DONE_PROGRESS;
				CastingC = CastingD;
				CastingT = GetCharInfo()->pSpawn->CastingData.SpellETA -
					GetCharInfo()->pSpawn->TimeStamp +
					MQGetTickCount64() + 450 + (pConnection->Last) * 4;
				Success(GetSpellByID(CastingD));
			}

			// are we attempting to interrupt this?
			if (DuckF != FLAG_COMPLETE) DuckHandle();
			return;
		}

		// wait for incoming chat, timers, and windows to be closed.
		DuckF = FLAG_COMPLETE;
		twisting = Evaluate("${If[${Twist.twisting},1,0]}") ? true : false;
		if (Casting) {
			if (CastingC == CastingD)
				if (PSPELL Spell = GetSpellByID(CastingC))
					switch (Spell->TargetType) {
					case 18: // Uber Dragons
					case 17: // Uber Giants
					case 16: // Plant
					case 15: // Corpse
					case 14: // Pet
					case 11: // Summoned
					case 10: // Undead
					case  9: // Animal
					case  5: // Single
						if (!pTarget) CastingE = CAST_NOTARGET;
						break;
					}

			// re-evaluate casting timer after cast window close
			CastingT = MQGetTickCount64() + 450 + (pConnection->Last) * 2;
			Casting = false;
		}
		if (CastingE == DONE_PROGRESS) {
			if (MQGetTickCount64()>CastingT) CastingE = CAST_SUCCESS;
			else if (!twisting) return;
		}
		// give time to proceed other casting events
		if (MemoF != FLAG_COMPLETE) MemoHandle();
		if (MemoF != FLAG_COMPLETE) return;
		if (CastF != FLAG_COMPLETE) CastHandle();

		// make sure we get final casting results
		if ((CastF == FLAG_COMPLETE && CastingC != NOID && CastingD == NOID) ||
			(BardClass && CastingC != NOID && (CastingD != NOID))) {
			CastingX = (CastingE<CAST_SUCCESS) ? CAST_SUCCESS : CastingE;
			CastingL = CastingC;
			CastingE = DONE_COMPLETE;
			if (!twisting) {
				DebugWrite("[%llu] MQ2Bot:: Casting Complete ID[%d] Result=[%s (%d)]", MQGetTickCount64(), CastingL, cast_status[CastingX], CastingX);
				CastTimer(CastingO, CastingC, CastingX); // patches for ae but sound illogicials
			}
			CastingC = NOID;
			CastingO = NOID;
		}
	}
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn)
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
	DebugSpewAlways("[MQ2BotTimer::ParseSpellsOnce] Total Number of spells %d, Spells Selected %d", tcount, SpellEventList.size());
}


PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("MQ2BotTimer::ShutdownPlugin()");
	SpellEventList.clear();
	for (unsigned long i = 0; i<BleachList.size(); i++) pMQ2Blech->RemoveEvent(BleachList[i]);
}


PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
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

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


PLUGIN_API VOID CastCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	Resultat = CAST_DISTRACTED;
	if (!gbInZone || Flags() || (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow))
	{
		DebugWrite("[%llu] MQ2Bot:[Casting]: Complete. [%s (%d)][%s%s%s%s]", MQGetTickCount64(), cast_status[Resultat], Resultat,
			!gbInZone ? " ZONE " : "", Flags() ? " FLAGS " : "", (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow) ? " SHOW " : "");
		return;
	}
	Reset();
	char zParm[MAX_STRING];
	long iParm = 0;
	do {
		GetArg(zParm, Cmd, ++iParm);
		if (zParm[0] == 0) {
			break;
		}
		else if (!strnicmp(zParm, "-targetid|", 10)) {
			TargI = atoi(&zParm[10]);
		}
		else if (!strnicmp(zParm, "-maxtries|", 10)) {
			CastW = RECAST_LAND; CastR = atoi(&zParm[10]);
		}
		else if (!strnicmp(zParm, "-invis", 6)) {
			Invisible = true;
		}
		else if (zParm[0] != '-' && CastN[0] == 0) {
			GetArg(CastN, zParm, 1, FALSE, FALSE, FALSE, '|');
			GetArg(CastC, zParm, 2, FALSE, FALSE, FALSE, '|');
		}
		else if (zParm[0] != '-' && CastC[0] == 0) {
			GetArg(CastC, zParm, 1, FALSE, FALSE, FALSE, '|');
		}
	} while (true);
	Resultat = CAST_SUCCESS;
	if (GetCharInfo()->Stunned)                            Resultat = CAST_STUNNED;
	else if (Invisible && GetCharInfo()->pSpawn->HideMode) Resultat = CAST_INVISIBLE;
	else if (!SpellFind(CastN, CastC))                      Resultat = CAST_UNKNOWN;
	else if (fTYPE != TYPE_SPELLS && SpellTimer(fTYPE, fINFO)) Resultat = CAST_NOTREADY;
	else if (TargI) {
		if (PSPAWNINFO Target = (PSPAWNINFO)GetSpawnByID(TargI)) *(PSPAWNINFO*)ppTarget = Target;
		else Resultat = CAST_NOTARGET;
	}
	if (Resultat == CAST_SUCCESS && fTYPE == TYPE_SPELLS) {
		if (BardClass) {
			if (GetCharInfo()->pSpawn->CastingData.SpellID) Execute("/stopsong");
		}
		CastG = GEMID(fFIND->ID);
		if (CastG == NOID) {
			CastG = atoi(&CastC[(strnicmp(CastC, "gem", 3)) ? 0 : 3]) - 1;
			MemoLoad(CastG, fFIND);
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
	CastI = fINFO;
	CastK = fTYPE;
	CastT = fTIME;
	CastS = fFIND;
	CastM = MQGetTickCount64() + DELAY_CAST;
	strcpy(CastN, fNAME);
	DebugWrite("[%llu] MQ2Bot:[Casting]: Name<%s> Type<%d>.", MQGetTickCount64(), CastN, CastK);
	CastHandle();
}

PLUGIN_API VOID DuckCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	if (gbInZone) {
		if (CastF != FLAG_COMPLETE) CastR = 0;
		if ((pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) && CastingLeft()>500) {
			DuckF = FLAG_REQUEST;
			DuckHandle();
		}
	}
	Resultat = CAST_SUCCESS;
}

PLUGIN_API VOID MemoCommand(PSPAWNINFO pChar, PCHAR zLine) {
	Resultat = CAST_DISTRACTED;
	if (!gbInZone || Flags() || !pSpellBookWnd) {
		DebugWrite("[%llu] MQ2Bot:[MemoCommand]: InZone=%s Flags=%d Wnd=%s.", MQGetTickCount64(), gbInZone ? "True" : "False", Flags(), pSpellBookWnd ? "True" : "False");
		return;
	}
	if (GetCharInfo()->Stunned) {
		Resultat = CAST_STUNNED;
		return;
	}
	Reset();
	long iParm = 0;
	char zParm[MAX_STRING];
	char zTemp[MAX_STRING];
	Resultat = CAST_SUCCESS;
	do {
		GetArg(zParm, zLine, ++iParm);
		if (!zParm[0]) break;
		GetArg(zTemp, zParm, 1, FALSE, FALSE, FALSE, '|');
		if (PSPELL Search = SpellBook(zTemp)) {
			GetArg(zTemp, zParm, 2, FALSE, FALSE, FALSE, '|');
			long Gem = atoi(&zTemp[(strnicmp(zTemp, "gem", 3)) ? 0 : 3]) - 1;
			if (!((DWORD)Gem<NUM_SPELL_GEMS)) {
				GetArg(zTemp, zLine, 1 + iParm);
				Gem = atoi(&zTemp[(strnicmp(zTemp, "gem", 3)) ? 0 : 3]) - 1;
				if ((DWORD)Gem<NUM_SPELL_GEMS) iParm++;
			}
			MemoLoad(Gem, Search);
		}
	} while (true);
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++)
		if (SpellToMemorize.SpellId[sp] != 0xFFFFFFFF &&
			SpellToMemorize.SpellId[sp] !=
			GetCharInfo2()->MemorizedSpells[sp]) SpellTotal++;
	if (SpellTotal) {
		MemoF = FLAG_REQUEST;
		MemoE = DONE_SUCCESS;
		MemoHandle();
	}
}





//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void PluginOn()
{
	AddCommand("/casting", CastCommand);
	AddCommand("/interrupt", DuckCommand);
	AddCommand("/memorize", MemoCommand);

}

void PluginOff()
{
	RemoveCommand("/casting");
	RemoveCommand("/interrupt");
	RemoveCommand("/memorize");
}

PLUGIN_API VOID InitializePlugin(VOID) {
	ClearAll();
	aCastEvent(LIST289, CAST_COLLAPSE, "Your gate is too unstable, and collapses#*#");
	aCastEvent(LIST289, CAST_CANNOTSEE, "You cannot see your target#*#");
	aCastEvent(LIST289, CAST_COMPONENTS, "You are missing some required components#*#");
	aCastEvent(LIST289, CAST_COMPONENTS, "You need to play a#*#instrument for this song#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You are too distracted to cast a spell now#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You can't cast spells while invulnerable#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You *CANNOT* cast spells, you have been silenced#*#");
	aCastEvent(LIST289, CAST_IMMUNE, "Your target has no mana to affect#*#");
	aCastEvent(LIST013, CAST_IMMUNE, "Your target is immune to changes in its attack speed#*#");
	aCastEvent(LIST013, CAST_IMMUNE, "Your target is immune to changes in its run speed#*#");
	aCastEvent(LIST289, CAST_IMMUNE, "Your target cannot be mesmerized#*#");
	aCastEvent(UNKNOWN, CAST_IMMUNE, "Your target looks unaffected#*#");
	aCastEvent(LIST264, CAST_INTERRUPTED, "Your spell is interrupted#*#");
	aCastEvent(UNKNOWN, CAST_INTERRUPTED, "Your casting has been interrupted#*#");
	aCastEvent(LIST289, CAST_FIZZLE, "Your spell fizzles#*#");
	aCastEvent(LIST289, CAST_FIZZLE, "You miss a note, bringing your song to a close#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "You must first select a target for this spell#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "This spell only works on#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "You must first target a group member#*#");
	aCastEvent(LIST289, CAST_NOTREADY, "Spell recast time not yet met#*#");
	aCastEvent(LIST289, CAST_OUTOFMANA, "Insufficient Mana to cast this spell#*#");
	aCastEvent(LIST289, CAST_OUTOFRANGE, "Your target is out of range, get closer#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "This spell does not work here#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You can only cast this spell in the outdoors#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You can not summon a mount here#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You must have both the Horse Models and your current Luclin Character Model enabled to summon a mount#*#");
	aCastEvent(LIST264, CAST_RECOVER, "You haven't recovered yet#*#");
	aCastEvent(LIST289, CAST_RECOVER, "Spell recovery time not yet met#*#");
	aCastEvent(LIST289, CAST_RESIST, "Your target resisted the#*#spell#*#");
	aCastEvent(LIST289, CAST_STANDING, "You must be standing to cast a spell#*#");
	aCastEvent(LIST289, CAST_STUNNED, "You can't cast spells while stunned#*#");
	aCastEvent(LIST289, CAST_SUCCESS, "You are already on a mount#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell did not take hold#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell would not have taken hold#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell is too powerful for your intended target#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "You need to be in a more open area to summon a mount#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "You can only summon a mount on dry land#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "This pet may not be made invisible#*#");
	aCastEvent(UNKNOWN, CAST_COMPONENTS, "Your ability to use this item has been disabled because you do not have at least a gold membership#*#");

}


PLUGIN_API VOID OnEndZone(VOID) {
	Reset();
	CastingO = NOID;
	CastingC = NOID;
	CastingE = CAST_SUCCESS;
	CastingT = 0;
	ImmobileT = 0;
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) {
	if (gbInZone) {
		if (CastingC != NOID && !twisting) {
			Parsed = false;
			if (Color == 264) { LIST264.Feed(Line); SUCCESS.Feed(Line); }
			else if (Color == 289) { LIST289.Feed(Line); }
			else if (Color == 13) { LIST013.Feed(Line); }
			if (!Parsed) {
				UNKNOWN.Feed(Line);
				if (Parsed) WriteChatf("\arMQ2Bot::Note for Author[\ay%s\ar]=(\ag%d\ar)\ax", Line, Color);
			}
		}
	}
	return 0;
}
#endif