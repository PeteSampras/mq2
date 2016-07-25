/*Problem: ${Ini[]} parses when you use it.  Eqmule can add a solution to make an unparsed version im sure, but for now here is a method to create unparsed ini entries that you can use in macros without having to re-access the ini.

NOTE:  Default path is your base MQ2Folder
Commands:
/ifadd ini_path,section,key - adds this key and value in this section of this ini file.
/ifdelete key - deletes this key
/iflist [all|key] - lists all or individual key, can be left blank to list all
/ifclear - clears all ifs
/ifrename key1 key2 - renames key1 to key2
/ifswap key1 key2 - swaps the if statements for key1 and key2


TLO/members:
Ifs.Exists[key]  - Does this key exist?
Ifs.Statement[key] - Parses the value of that key.  /if (${Ifs.Statement[key]}) /do this
Ifs.Total  - total number of ifs mapped out

Example:
My ini is \mq2\macros\bot_petesampras.ini
My section is [Heals]
My key is UseHeal1=${If[${Group.MainTank.PctHPs}<30 && ${Me.PctMana}>40]}

/ifadd macros\bot_petesampras.ini,Heals,UseHeal1

Now I can:
Verify it exists: ${If.Exists[UseHeal1]}
View it unparsed: /iflist UseHeal1
Parse it: ${Ifs.Statement[UseHeal1]}
Delete it: /ifdelete UseHeal1


*/
// MQ2Ifs.cpp : PeteSampras.
//

//includes
#include "../MQ2Plugin.h"
#include <map>
#include <iterator>


//defines
#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index

//#define PLUGIN_NAME  "MQ2Ifs"     // This should already be defined, but if not, remove the comment

PreSetup("MQ2Ifs");
PLUGIN_VERSION(1.00);

//variables
char INISection[MAX_STRING]={0}, INIKey[MAX_STRING]={0};
bool announce=true,activated=false;
std::map<string,string> mIf;
std::map<string,string>::iterator mIt;

//pre declares

void CommandCreateIf(PCHARINFO pChar,PCHAR szLine);
void CommandDeleteIf(PCHARINFO pChar,PCHAR szLine);
void CommandListIf(PSPAWNINFO pChar, PCHAR szLine);
void CommandSwapIf(PSPAWNINFO pChar, PCHAR szLine);
void CommandRenameIf(PSPAWNINFO pChar, PCHAR szLine);
void CreateIf(char szFileName[MAX_STRING],char szSection[MAX_STRING],char szKey[MAX_STRING]);
void DeleteIf(char szKey[MAX_STRING]);
void SwapIf(char szKey[MAX_STRING]);
void RenameIf(char szKey1[MAX_STRING], char szKey2[MAX_STRING]);


//TLO
class MQ2IfsType *pIfsTypes=nullptr;
class MQ2IfsType : public MQ2Type {
private:
   char Tempos[MAX_STRING];
public:
   enum IfsMembers {
      Exists=1,
      Statement=2,
      Total=3,
   };
   MQ2IfsType():MQ2Type("Ifs") {
      TypeMember(Exists);
      TypeMember(Statement);
      TypeMember(Total);
   };
   bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
      PMQ2TYPEMEMBER pMember=MQ2IfsType::FindMember(Member);
      if(pMember)
         switch((IfsMembers)pMember->ID) {
            case Exists:
               if(ISINDEX())
               {
                  if(ISNUMBER())
                  {
                     Dest.DWord=false;
                     Dest.Type=pBoolType;
                     return true;
                  }
                  char szTemp[MAX_STRING]={0};
                  mIt = mIf.find(GETFIRST());
                  if(mIt != mIf.end())
                  {
                     Dest.DWord=true;
                     Dest.Type=pBoolType;
                     return true;
                  }
               }
               Dest.DWord=false;
               Dest.Type=pBoolType;
               return true;
            case Statement:
               if(ISINDEX())
               {
                  char szTemp[MAX_STRING]={0};
                  if(ISNUMBER())
                  {
                     Dest.DWord=false;
                     Dest.Type=pBoolType;
                     return true;
                  }
                  mIt = mIf.find(GETFIRST());
                  if(mIt != mIf.end())
                  {
                     sprintf(szTemp,mIt->second.c_str());
                     Dest.Ptr=szTemp;
                     Dest.Type=pStringType;
                     return true;
                  }
               }
               Dest.DWord=false;
               Dest.Type=pBoolType;
               return true;
            case Total:
               Dest.DWord=mIf.size();
               Dest.Type=pIntType;
               return true;
            }
      strcpy(Tempos,"NULL");
      Dest.Type=pStringType;
      Dest.Ptr=Tempos;
      return true;      
   }
   bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
      strcpy(Destination,"TRUE");
      return true;
   }
   bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
      return false;
   }
   bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
      return false;
   }
   ~MQ2IfsType()
   {
   }
};
BOOL DataIfs(PCHAR Index, MQ2TYPEVAR &Dest)
{
   Dest.Type=pIfsTypes;
   Dest.DWord=1;
   return true;
}
//Commands
void CommandCreateIf(PSPAWNINFO pChar, PCHAR szLine)
{
   char tmpFile[MAX_STRING]={0}, tmpSection[MAX_STRING]={0}, tmpKey[MAX_STRING]={0};
   GetArg(tmpFile,szLine,1,FALSE,FALSE,FALSE,',');
   GetArg(tmpSection,szLine,2,FALSE,FALSE,FALSE,',');
   GetArg(tmpKey,szLine,3,FALSE,FALSE,FALSE,',');
   if(!strlen(tmpFile)||!strlen(tmpSection)||!strlen(tmpKey))
      return;
   CreateIf(tmpFile,tmpSection,tmpKey);
}
void CommandDeleteIf(PSPAWNINFO pChar, PCHAR szLine)
{
   if(!strlen(szLine))
      return;
   DeleteIf(szLine);
}
void CommandListIf(PSPAWNINFO pChar, PCHAR szLine)
{
   char szTemp[MAX_STRING]={0};
   sprintf(szTemp,szLine);
   if(!strlen(szLine)||!strcmp(strlwr(szTemp),"all"))
   {
      for (mIt=mIf.begin(); mIt!=mIf.end(); ++mIt)
      {
         WriteChatf("%s=%s",mIt->first.c_str(),mIt->second.c_str());
      }
   }
   else
   {
      WriteChatf("%s",szLine);
      mIt = mIf.find(szLine);
      if(mIt != mIf.end())
      {
         WriteChatf("%s=%s",mIt->first.c_str(),mIt->second.c_str());
      }
   }
}
void CommandClearIf(PSPAWNINFO pChar, PCHAR szLine)
{
   mIf.clear();
}

void CommandSwapIf(PSPAWNINFO pChar, PCHAR szLine)
{
   char szTemp1[MAX_STRING]={0}, szTemp2[MAX_STRING]={0};
   GetArg(szTemp1,szLine,1,FALSE,FALSE,FALSE,',');
   GetArg(szTemp2,szLine,2,FALSE,FALSE,FALSE,',');

   //sprintf(szTemp,szLine);
   if(!strlen(szTemp1)||!strlen(szTemp2))
   {
      if(announce)
	     WriteChatf("Invalid Swap format");
	  return;
   }
   else
   {
      //WriteChatf("%s",szLine);
      mIt = mIf.find(szTemp1);
      if(mIt == mIf.end())
      {
         if(announce)
            WriteChatf("Ifs %s Not exist. Swap failed.", szTemp1);
		 return;
      }
      mIt = mIf.find(szTemp2);
      if(mIt == mIf.end())
      {
         if(announce)
            WriteChatf("Ifs %s Not exist. Swap failed.", szTemp2);
		 return;
      }
   }
   SwapIf(szTemp1);
}

void CommandRenameIf(PSPAWNINFO pChar, PCHAR szLine)
{
   char szTemp1[MAX_STRING]={0}, szTemp2[MAX_STRING]={0};
   GetArg(szTemp1,szLine,1,FALSE,FALSE,FALSE,',');
   GetArg(szTemp2,szLine,2,FALSE,FALSE,FALSE,',');

   //sprintf(szTemp,szLine);
   if(!strlen(szTemp1)||!strlen(szTemp2))
   {
      if(announce)
	     WriteChatf("Invalid Rename format");
	  return;
   }
   else
   {
      //WriteChatf("%s",szLine);
      mIt = mIf.find(szTemp1);
      if(mIt == mIf.end())
      {
         if(announce)
            WriteChatf("Ifs %s Not Exist. Rename Failed.", szTemp1);
		 return;
      }
      mIt = mIf.find(szTemp2);
      if(mIt != mIf.end())
      {
         if(announce)
            WriteChatf("Ifs %s Exists. Deleting.", szTemp2);
		 DeleteIf(szTemp2);
      }
   }
   RenameIf(szTemp1, szTemp2);
}
//Routines
void CreateIf(char szFileName[MAX_STRING],char szSection[MAX_STRING],char szKey[MAX_STRING])
{
   if(!strlen(szFileName)||!strlen(szSection)||!strlen(szKey))
      return;
   char szTemp[MAX_STRING] = {0};
   if(strstr(strlwr(szFileName),".ini"))
      sprintf(INIFileName,"%s\\%s",gszINIPath,szFileName);
   else
      sprintf(INIFileName,"%s\\%s.ini",gszINIPath,szFileName);
   sprintf(INISection,"%s",szSection);

   if(GetPrivateProfileString(INISection,szKey,NULL,szTemp,MAX_STRING,INIFileName))
   {
      //these 5 lines might need removed
      mIt=mIf.find(szKey);
      if(mIt != mIf.end())
      {
         mIf.erase(mIt);
      }
      mIf.insert(make_pair<string,string>(szKey,szTemp));
      mIt = mIf.find(szKey);
      if(mIt != mIf.end() && announce) 
         WriteChatf("Added:%s=%s",szKey,mIt->second.c_str());
   }
}

void DeleteIf(char szKey[MAX_STRING])
{
   mIt = mIf.find(szKey);
   if(mIt != mIf.end())
   {
      mIf.erase(mIt);
      if(announce)
         WriteChatf("%s deleted",szKey);
   }
}

void SwapIf(char szKey[MAX_STRING])
{
   std::map<string,string>::iterator mIt1;
   //string Ifs2 = mIt->second;
   mIt1 = mIf.find(szKey);
   if(mIt1 != mIf.end() && mIt != mIf.end())
   {
      std::swap(mIt->second, mIt1->second);
      if(announce)
         WriteChatf("Swaped %s with %s",mIt->first.c_str(), mIt1->first.c_str());
   }
}

void RenameIf(char szKey1[MAX_STRING], char szKey2[MAX_STRING])
{
   string szTemp = "${If[1,1,0]}";
   mIt=mIf.find(szKey1);
   if(mIt != mIf.end())
   {
	   szTemp = mIt->second;
       mIf.erase(mIt);
   }
   mIf.insert(make_pair<string,string>(szKey2,szTemp.c_str()));
   mIt = mIf.find(szKey2);
   if(mIt != mIf.end() && announce) 
      WriteChatf("Renamed: %s to %s=%s", szKey1, szKey2, mIt->second.c_str());
}


//required plugin sections
void PluginOn()
{
   if(activated)
      return;
   activated = true;
   AddCommand("/ifadd", CommandCreateIf);
   AddCommand("/ifdelete", CommandDeleteIf);
   AddCommand("/iflist", CommandListIf);
   AddCommand("/ifclear", CommandClearIf);
   AddCommand("/ifswap", CommandSwapIf);
   AddCommand("/ifrename", CommandRenameIf);
   pIfsTypes= new MQ2IfsType;
   AddMQ2Data("Ifs",DataIfs);
}

void PluginOff()
{
   if(!activated)
      return;
   activated = false;
   DebugSpewAlways("Shutting down MQ2Ifs");
   RemoveCommand("/ifadd");
   RemoveCommand("/ifdelete");
   RemoveCommand("/iflist");
   RemoveCommand("/ifclear");
   RemoveCommand("/ifswap");
   RemoveCommand("/ifrename");
   RemoveMQ2Data("Ifs");
   delete pIfsTypes;
   pIfsTypes = nullptr;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
   DebugSpewAlways("Initializing %s", PLUGIN_NAME);
   PluginOn();
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   DebugSpewAlways("Shutting down %s", PLUGIN_NAME);
   PluginOff();
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
   DebugSpewAlways("%s::SetGameState()", PLUGIN_NAME);
   //if (GameState==GAMESTATE_INGAME)
   // create custom windows if theyre not set up, etc
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
   // DONT leave in this debugspew, even if you leave in all the others
   //DebugSpewAlways("%s::OnPulse()", PLUGIN_NAME);
}
