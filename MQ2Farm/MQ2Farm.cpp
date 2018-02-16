// MQ2Farm.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


//place all headers in this region
#pragma region Headers

#include "../MQ2Plugin.h"
#pragma endregion Headers

PreSetup("MQ2Farm");

// place all prototypes in this region
#pragma region Prototypes

#pragma endregion Prototypes

// Place all variables in this region
#pragma region Variables
bool activated = false;

#pragma endregion Variables

// place all initial settings in this region
#pragma region Settings

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
#pragma endregion UtilityFunctions


// place all primary functions here
#pragma region PrimaryFunctions

#pragma endregion PrimaryFunctions


// Call to activate plugin.
// Add commands, aliases, datatypes, benchmarks, UI files, detours, etc.
void PluginOn()
{
if(activated)
    return;
activated=true;


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
