// MQ2Farm.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


//place all headers in this region
#pragma region Headers
#include "../MQ2Plugin.h"
#include <Windows.h>
#include <stdio.h>

#include <vector>
using namespace std;
#pragma endregion Headers

PreSetup("MQ2Farm");

#pragma region Defines
#define TargetIT(X)			*(PSPAWNINFO*)ppTarget=X
#pragma endregion Defines


// place all prototypes in this region
#pragma region Prototypes
void CheckAlias();
void Configure(char szCustomIni[MAX_STRING], int force);
LONG Evaluate(PCHAR szLine);
void FarmCommand(PSPAWNINFO pChar, PCHAR szLine);
void FarmIniCommand(PSPAWNINFO pChar, PCHAR szLine);
void IgnoreThisCommand(PSPAWNINFO pChar, PCHAR szLine);
void IgnoreTheseCommand(PSPAWNINFO pChar, PCHAR szLine);
void ListCommands();
DWORD Search(char szLine[MAX_STRING]);
#pragma endregion Prototypes

// Place all variables in this region
#pragma region Variables
bool activated = false, bDebugging = false;
char szMyTargetID[MAX_STRING] = {0},IgnoreINISection[MAX_STRING] = { 0 }, IgnoreList[MAX_STRING] = { 0 },INISection[MAX_STRING] = { 0 }, 
ImmuneINIFileName[MAX_PATH] = { 0 };
int iPullRange=0,iZRadius=0,iPulses=0;
vector<string> vFarmMobs, vIgnoreMobs;

#pragma endregion Variables

// place all initial settings in this region
#pragma region Settings
int iPulseDelay = 3;

//ini entries
DWORD MinLevel=0;
DWORD MaxLevel=200;
eSpawnType SpawnType;
DWORD SpawnID;
DWORD FromSpawnID;
FLOAT Radius;
CHAR szName[MAX_STRING];
CHAR szBodyType[MAX_STRING];
CHAR szRace[MAX_STRING];
CHAR szClass[MAX_STRING];
CHAR szLight[MAX_STRING];
BOOL bSpawnID;
BOOL bNotNearAlert;
BOOL bNearAlert;
BOOL bNoAlert;
BOOL bAlert;
BOOL bLight;
BOOL bTargNext;
BOOL bTargPrev;
BOOL bXTarHater;
BOOL bGM;
BOOL bNamed;
BOOL bMerchant=false;
BOOL bAura;
BOOL bBanner;
BOOL bCampfire;
DWORD NotNearAlertList;
DWORD NearAlertList;
DWORD NoAlertList;
DWORD AlertList;
DOUBLE ZRadius;
DOUBLE FRadius;
FLOAT xLoc;
FLOAT yLoc;
FLOAT zLoc;
DWORD SortBy;
BOOL bLoS;
BOOL bExactName;
BOOL bTargetable;
DWORD PlayerState;

#pragma endregion Settings

// place all Inline functions in this region
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
#pragma endregion Inlines

//place all structs in this region
#pragma region Structs
#pragma endregion Structs

// place all utility functions in this region
#pragma region UtilityFunctions
void CheckAlias()
{
    char aliases[MAX_STRING] = {0};
    if (RemoveAlias("/farm"))
        strcat_s(aliases, " /farm ");
    if (RemoveAlias("/ignorethis"))
        strcat_s(aliases, " /ignorethis ");
    if (RemoveAlias("/ignorethese"))
        strcat_s(aliases, " /ignorethese ");
    if (RemoveAlias("/imob"))
        strcat_s(aliases, " /imob ");
    if (RemoveAlias("/loadignore"))
        strcat_s(aliases, " /loadignore ");
    if (strlen(aliases) > 0)
           WriteChatf("\ar[MQ2Farm]\ao::\ayWARNING\ao::\awAliases for \ao%s\aw were detected and temporarily removed.", aliases);
  }

LONG Evaluate(PCHAR szLine)
{
        char test[MAX_STRING];
        sprintf_s(test, "${If[%s,1,0]}", szLine);
        ParseMacroData(test,MAX_STRING);
        return atoi(test);
}

void ListCommands()
{
    WriteChatf("\ar[MQ2Farm]\ao::\ayCommands Available");
    WriteChatf("\ar[MQ2Farm]\ao::\ay/farm --- Will output this help menu");
    WriteChatf("\ar[MQ2Farm]\ao::\ay/ignorethis --- Will ignore your current target");
    WriteChatf("\ar[MQ2Farm]\ao::\ay/ignorethese --- Will ignore all spawns with this targets clean name");
}



#pragma endregion UtilityFunctions


// place all primary functions here
#pragma region PrimaryFunctions
DWORD SearchSpawns(char szIndex[MAX_STRING])
{
    unsigned long nth;
    SEARCHSPAWN ssSpawn;
    ClearSearchSpawn(&ssSpawn);
    ssSpawn.FRadius = 999999.0f;
	PCHAR pSearch;
	if (pSearch = strchr(szIndex, ','))
	{
		*pSearch = 0;
		++pSearch;
		ParseSearchSpawn(pSearch, &ssSpawn);
		// nth = atoi(GETFIRST(szIndex)); // have to get the number from the first left of the szIndex to the pSearch
	}
	else
	{
		if (IsNumberToComma(szIndex))
		{
			// nth = atoi(GETFIRST(szIndex)); // have to get the number from the first left of the szIndex to the pSearch
		}
		else
		{
			nth = 1;
			ParseSearchSpawn(szIndex, &ssSpawn);
		}
	}
	for (unsigned long N = 0; N < gSpawnCount; N++)
	{
		if (EQP_DistArray[N].Value.Float > ssSpawn.FRadius && !ssSpawn.bKnownLocation)
			return false;
		if (SpawnMatchesSearch(&ssSpawn, (PSPAWNINFO)pCharSpawn, (PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr))
		{
			if (--nth == 0)
			{
				DWORD thingy = 100;
				return thingy;
			}
		}
	}
	// No spawn
	return false;    
}
#pragma endregion PrimaryFunctions

#pragma region Commands
void FarmIniCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (strlen(szLine) != 0)
	{
		CHAR Arg1[MAX_STRING] = { 0 };
		GetArg(Arg1, szLine, 1);
		WriteChatf("Loading custom INI: %s", Arg1);
		Configure(Arg1, 1);
	}
}

void FarmCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    if (!InGameOK())
                return;
    char szTemp[MAX_STRING] = {0};
	strcpy_s(szTemp, szLine);
    if (strlen(szLine)==0)
    {
        ListCommands();
    }
    else
    {
        CHAR Arg1[MAX_STRING];
        bool bFound=false;
        for (int i=1;i<100;i++)
        {
            GetArg(Arg1, szLine, i);
            if(strlen(szLine)==0)
                break;
            else
            {
                if (IsNumber(Arg1))
                {
                    iPullRange = atoi(Arg1);
                    bFound;
                }
                else
                    vFarmMobs.push_back(Arg1);
            }
        }
        if(bFound)
        {
			
            std::string str1 = szLine; //lets create a new string, replace out the number
			_itoa_s(iPullRange, Arg1, 10);
            str1.replace(str1.find(Arg1), 1, ""); //actually replace
            sprintf_s(szTemp, "%s", str1.c_str()); // change szLine back to original minus the number
        }
		char szNull[MAX_STRING] = { 0 }; //REMINDER: Remove this once we redo command
		Configure(szNull, 0);			 //REMINDER: Remove this once we redo command
        WriteChatf("Searching for %s in %d radius", szLine,iPullRange);
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

void IgnoreThisCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    bool bRemove = false;
    if (strlen(szLine) != 0)
    {
            CHAR Arg1[MAX_STRING] = { 0 };
            GetArg(Arg1, szLine, 1);
            if (!_stricmp(Arg1, "remove"))
                bRemove=true;
            if (!_stricmp(Arg1, "clear"))
            {
                vIgnoreMobs.clear();
                WriteChatf("Ignore list cleared");
            }

    }
    if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget) // if target exists, then lets execute some code
    {
        char szName[MAX_STRING] = {0};
        sprintf_s(szName,"%s",pMyTarget->Name);
        bool bFound = false;
		int iSize = vIgnoreMobs.size();
        for(int i=0;i<iSize;i++)
        {
            if(!_stricmp(vIgnoreMobs[i].c_str(),szName))
            {
                if(bRemove)
                {
                    vIgnoreMobs.erase(vIgnoreMobs.begin()+i);
                    WriteChatf("Removed %s from ignore list.",szName);
                    break;
                }
                bFound=true;
            }
        }
        if(!bFound)
        {
            vIgnoreMobs.push_back(szName);
            WriteChatf("%s added to ignore list.",szName);
        }
    }
    else
    {
        WriteChatf("No valid target to add to ignore list.");
    }
}

void IgnoreTheseCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    bool bRemove = false;
    if (strlen(szLine) != 0)
    {
            CHAR Arg1[MAX_STRING] = { 0 };
            GetArg(Arg1, szLine, 1);
            if (!_stricmp(Arg1, "remove"))
                bRemove=true;
            if (!_stricmp(Arg1, "clear"))
            {
                vIgnoreMobs.clear();
                WriteChatf("Ignore list cleared");
            }

    }
    if (PSPAWNINFO pMyTarget = (PSPAWNINFO)pTarget) // if target exists, then lets execute some code
    {
        char szName[MAX_STRING] = {0};
        sprintf_s(szName,"%s",pMyTarget->DisplayedName);
        bool bFound = false;
        for(int i=0;i<vIgnoreMobs.size();i++)
        {
            if(!_stricmp(vIgnoreMobs[i].c_str(),szName))
            {
                if(bRemove)
                {
                    vIgnoreMobs.erase(vIgnoreMobs.begin()+i);
                    WriteChatf("Removed %s from ignore list.",szName);
                    break;
                }
                bFound=true;
            }
        }
        if(!bFound)
        {
            vIgnoreMobs.push_back(szName);
            WriteChatf("%s added to ignore list.",szName);
        }
        EzCommand("/squelch /target clear");
    }
    else
    {
        WriteChatf("No valid target to add to ignore list.");
    }
}

void LoadIgnoreCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGameOK())
		return;
	else
		sprintf_s(ImmuneINIFileName, "%s\\macros\\Mob_Ignore_List.ini", gszINIPath);
	if (!pZoneInfo)
		return;
	sprintf_s(IgnoreINISection, "Ignores.%s", ((PZONEINFO)pZoneInfo)->ShortName);
	GetPrivateProfileString(IgnoreINISection, "Ignore", "|", IgnoreList, MAX_STRING, ImmuneINIFileName);
	if (!strcmp(IgnoreList, "|"))
	{
		WritePrivateProfileString(IgnoreINISection, "Ignore", IgnoreList, ImmuneINIFileName);
	}
	CHAR szList[MAX_STRING];
	sprintf_s(szList, "\arIgnores:\aw%s", IgnoreList);

}
#pragma endregion Commands

// Call to activate plugin.
// Add commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOn()
{
if(activated)
    return;
activated=true;
AddCommand("/farm", FarmCommand);
AddCommand("/botini", FarmIniCommand);
AddCommand("/ignorethis", IgnoreThisCommand);
AddCommand("/ignorethese", IgnoreTheseCommand);
AddCommand("/imob", IgnoreMobCommand);
AddCommand("/loadignore", LoadIgnoreCommand);
EzCommand("/loadignore");


}

// Call to deactivate plugin.
// Remove commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOff()
{
    if(!activated)
        return;
    activated=false;
    DebugSpewAlways("Shutting down MQ2Farm");
    RemoveCommand("/farm");
	RemoveCommand("/farmini");
    RemoveCommand("/ignorethis");
    RemoveCommand("/ignorethese");
    RemoveCommand("/imob");
    RemoveCommand("/loadignore");
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
        DebugSpewAlways("Initializing MQ2Farm");
        if (InGameOK())
            PluginOn();
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
        DebugSpewAlways("Shutting down MQ2Farm");
        PluginOff();
}

// things you want to do when you begin zoning
PLUGIN_API VOID OnBeginZone(VOID)
{
}

// things you want to do once you are done zoning
PLUGIN_API VOID OnEndZone(VOID)
{
    EzCommand("/loadignore");
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
        DebugSpewAlways("MQ2Farm::OnZoned()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
        DebugSpewAlways("MQ2Farm::OnCleanUI()");
        // destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
        DebugSpewAlways("MQ2Farm::OnReloadUI()");
        // recreate custom windows, etc
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
        // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Farm::OnDrawHUD()");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
        DebugSpewAlways("MQ2Farm::SetGameState()");
    //if (GameState==GAMESTATE_INGAME)
        // create custom windows if theyre not set up, etc
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
if(++iPulses < iPulseDelay || !InGameOK())
        return;
iPulses=0;
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
        DebugSpewAlways("MQ2Farm::OnWriteChatColor(%s)",Line);
        return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
        DebugSpewAlways("MQ2Farm::OnIncomingChat(%s)",Line);
        return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
        DebugSpewAlways("MQ2Farm::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
        DebugSpewAlways("MQ2Farm::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
        DebugSpewAlways("MQ2Farm::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
        DebugSpewAlways("MQ2Farm::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}

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
	//Shrouded = GetCharInfo2()->Shrouded; //this is used if you are shrouded or in an instance
	//if (!Shrouded)
	//	INISection[strlen(PLUGIN_NAME)] = 0;
	strcpy_s(tempINI, INISection);
	_strlwr_s(szCustomIni, MAX_STRING);
	if (force && strlen(szCustomIni) > 1 && !strstr(szCustomIni, "default"))
		sprintf_s(INISection, "%s_%s", tempINI, szCustomIni);
	char szTemp[MAX_STRING] = { 0 };
/*	eSpawnType SpawnType;
	DWORD SpawnID;
	DWORD FromSpawnID;
	FLOAT Radius;
	CHAR szName[MAX_STRING];
	CHAR szBodyType[MAX_STRING];
	CHAR szRace[MAX_STRING];
	CHAR szClass[MAX_STRING];
	CHAR szLight[MAX_STRING];
	BOOL bSpawnID;
	BOOL bNotNearAlert;
	BOOL bNearAlert;
	BOOL bLight;
	BOOL bTargNext;
	BOOL bTargPrev;
	BOOL bXTarHater;
	BOOL bGM;
	BOOL bMerchant = false;
	BOOL bAura;
	BOOL bBanner;
	BOOL bCampfire;
	DWORD NotNearAlertList;
	DWORD NearAlertList;
	DWORD NoAlertList;
	DWORD AlertList;
	FLOAT xLoc;
	FLOAT yLoc;
	FLOAT zLoc;
	DWORD SortBy;
	BOOL bExactName;
	BOOL bTargetable;
	DWORD PlayerState;
	*/
	// DEBUG_DUMPFILE = 0 != GetPrivateProfileInt(INISection, "Debugging", 0, INIFileName);//log to file?
	iPullRange = GetPrivateProfileInt(INISection, "PullRange", 1000, INIFileName); //default pull range.. but i think this is same as radius.. so.. rename it?
	MinLevel = GetPrivateProfileInt(INISection, "MinLevel", 1, INIFileName); //default min level to search for
	MaxLevel = GetPrivateProfileInt(INISection, "MaxLevel", 200, INIFileName); //default max level to search for
	bLoS = GetPrivateProfileInt(INISection, "LineOfSight", 0, INIFileName); //do I require line of sight?
	ZRadius = GetPrivateProfileInt(INISection, "ZRadius", 1000, INIFileName); // default Zradius
	FRadius = GetPrivateProfileInt(INISection, "Radius", 1000, INIFileName); // Default radius
	bNamed = GetPrivateProfileInt(INISection, "NamedMobs", 0, INIFileName);// default no
	bNoAlert = GetPrivateProfileInt(INISection, "AvoidAlertMobs", 1, INIFileName);//default avoid alert mobs
	bAlert = GetPrivateProfileInt(INISection, "FindAlertMobs", 0, INIFileName);//default avoid alert mobs

	//examples:
	/*
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
	MQ2NavDistance = GetPrivateProfileInt(INISection, "MQ2NavDistance", 10, INIFileName);//debug?
	AnnounceBehaviorChange = GetPrivateProfileInt(INISection, "AnnounceBehaviorChange", 0, INIFileName);//debug?
	PetReagentCheck = GetPrivateProfileInt(INISection, "PetReagentCheck", 1, INIFileName);//Check for pet reagents before casting?
	SafetyInvisOnWarp = GetPrivateProfileInt(INISection, "SafetyInvisOnWarp", 0, INIFileName);// invis on warp?
	if (BardClass)
	{
		GetPrivateProfileString(INISection, "RestTwist", "", RestTwist, sizeof(RestTwist), INIFileName);
		GetPrivateProfileString(INISection, "CombatTwist", "", CombatTwist, sizeof(CombatTwist), INIFileName);
	}
*/

	//now we write to the ini using the values we have to ensure ini is always accurate and presented in the order we want
	int MyClass = GetCharInfo()->pSpawn->mActorClient.Class;
	/*
	sprintf_s(szTemp, "%.4f", botVersion);
	WritePrivateProfileString(INISection, "Debugging", DEBUG_DUMPFILE ? "1" : "0", INIFileName);
	WritePrivateProfileString(INISection, "BotVersion", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "BotCommandsOn", BotCommandsOn, INIFileName);
	WritePrivateProfileString(INISection, "BotCommandsOff", BotCommandsOff, INIFileName);
	*/
	_itoa_s(iPullRange, szTemp, 10);
	WritePrivateProfileString(INISection, "PullRange", szTemp, INIFileName);
	_itoa_s(MinLevel, szTemp, 10);
	WritePrivateProfileString(INISection, "MinLevel", szTemp, INIFileName);
	_itoa_s(MaxLevel, szTemp, 10);
	WritePrivateProfileString(INISection, "MaxLevel", szTemp, INIFileName);
	_itoa_s(bLoS, szTemp, 10);
	WritePrivateProfileString(INISection, "LineOfSight", szTemp, INIFileName);
	_itoa_s(ZRadius, szTemp, 10);
	WritePrivateProfileString(INISection, "ZRadius", szTemp, INIFileName);
	_itoa_s(FRadius, szTemp, 10);
	WritePrivateProfileString(INISection, "Radius", szTemp, INIFileName);
	_itoa_s(bNamed, szTemp, 10);
	WritePrivateProfileString(INISection, "NamedMobs", szTemp, INIFileName);
	_itoa_s(bNoAlert, szTemp, 10);
	WritePrivateProfileString(INISection, "AvoidAlertMobs", szTemp, INIFileName);
	_itoa_s(bAlert, szTemp, 10);
	WritePrivateProfileString(INISection, "FindAlertMobs", szTemp, INIFileName);

// examples that need changed:
	/*
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
	_itoa_s(MQ2NavDistance, szTemp, 10);
	WritePrivateProfileString(INISection, "MQ2NavDistance", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "PullSkillName", PullSkill, INIFileName);
	_itoa_s(AnnounceBehaviorChange, szTemp, 10);
	WritePrivateProfileString(INISection, "AnnounceBehaviorChange", szTemp, INIFileName);
	WritePrivateProfileString(INISection, "AttackCommand", AttackCommand, INIFileName);
	*/
}
