// MQ2Farm.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


//place all headers in this region
#pragma region Headers

#include "../MQ2Plugin.h"
#include <vector>
#pragma endregion Headers

PreSetup("MQ2Farm");

// place all prototypes in this region
#pragma region Prototypes
void CheckAlias();
void FarmCommand(PSPAWNINFO pChar, PCHAR szLine);
void ListCommands();
#pragma endregion Prototypes

// Place all variables in this region
#pragma region Variables
bool activated = false, bDebugging = false;
char szMyTargetID[MAX_STRING] = {0};
int iPullRange=0,iZRadius=0,iPulses=0,iPulseDelay=0;
vector<string> vFarmMobs;

#pragma endregion Variables

// place all initial settings in this region
#pragma region Settings
iPulseDelay=3;
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
        char aliases[MAX_STRING] = "";
        if (RemoveAlias("/ignorethis"))
                strcat_s(aliases, " /ignorethis ");
        if (RemoveAlias("/ignorethese"))
                strcat_s(aliases, " /ignorethese ");
        if (strlen(aliases) > 0)
                WriteChatf("\ar[MQ2Farm]\ao::\ayWARNING\ao::\awAliases for \ao%s\aw were detected and temporarily removed.", aliases);
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

#pragma endregion PrimaryFunctions

#pragma region Commands

void FarmCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    if (!InGameOK())
                return;
    char buffer[MAX_STRING] = "";
    if (strlen(szLine)==0)
    {
        ListCommands();
    }
    else
    {
        CHAR Arg[MAX_STRING];
        for (int i=1;i++;i<100)
        {
            GetArg(Arg, szLine, i);
            if(strlen(szLine)==0)
                break;
            else
            {
                if (IsNumber(Arg))
                    iPullRange = atoi(Arg);
                else
                    vFarmMobs.push_back(Arg);
            }
        }
    }
    WriteChatf("Searching for %s in %d radius", szFarmMob,iPullRange);
}
#pragma endregion Commands

            GetArg(Arg2, szLine, 2);
// Call to activate plugin.
// Add commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOn()
{
if(activated)
    return;
activated=true;
AddCommand("/farm", FarmCommand);

}

// Call to deactivate plugin.
// Remove commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOff()
{
if(!activated)
    return;
activated=false;

}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
        DebugSpewAlways("Initializing MQ2Farm");
        if (InGameOK())
            PluginOn();
        InitLib(PLUGIN_NAME);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
        DebugSpewAlways("Shutting down MQ2Farm");
        PluginOff();
        FreeLib(PLUGIN_NAME);
}

// things you want to do when you begin zoning
PLUGIN_API VOID OnBeginZone(VOID)
{
}

// things you want to do once you are done zoning
PLUGIN_API VOID OnEndZone(VOID)
{

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
iPulse=0;
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
