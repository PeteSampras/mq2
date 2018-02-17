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
#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index
#define TargetIT(X)			*(PSPAWNINFO*)ppTarget=X
#pragma endregion Defines


// place all prototypes in this region
#pragma region Prototypes
void CheckAlias();
LONG Evaluate(PCHAR szLine);
void FarmCommand(PSPAWNINFO pChar, PCHAR szLine);
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

void FarmCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    if (!InGameOK())
                return;
    char szTemp[MAX_STRING] = "";
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

#pragma endregion Commands

// Call to activate plugin.
// Add commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOn()
{
if(activated)
    return;
activated=true;
AddCommand("/farm", FarmCommand);
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
