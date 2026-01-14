#include <iostream>
using namespace std;

#include "Core.h"
#include "Console.h"
#include "Export.h"
#include "PluginManager.h"
#include <vector>
#include <string>
#include "modules/World.h"
#include <stdlib.h>
using namespace DFHack;
using namespace df::enums;

command_result mode (color_ostream &out, vector <string> & parameters);

DFHACK_PLUGIN("mode");

DFhackCExport command_result plugin_init ( color_ostream &out, std::vector <PluginCommand> &commands)
{
    commands.push_back(PluginCommand(
        "mode",
        "View, change and track game mode.",
        mode));
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( color_ostream &out )
{
    return CR_OK;
}

void printCurrentModes(t_gamemodes gm, color_ostream &out)
{
    out.print("Current game type:\t (");
    switch(gm.g_type)
    {
    case game_type::DWARF_MAIN:
        out.print("Fortress)\n");
        break;
    case game_type::ADVENTURE_MAIN:
        out.print("Adventurer)\n");
        break;
    case game_type::VIEW_LEGENDS:
        out.print("Legends)\n");
        break;
    case game_type::DWARF_RECLAIM:
        out.print("Reclaim)\n");
        break;
    case game_type::DWARF_ARENA:
        out.print("Arena)\n");
        break;
    case game_type::ADVENTURE_ARENA:
        out.print("Arena - control creature)\n");
        break;
    case game_type::num:
        out.print("INVALID)\n");
        break;
    case game_type::NONE:
        out.print("NONE)\n");
        break;
    default:
        out.print("!!UNKNOWN!!)\n");
        break;
    }
    out.print("Current game mode:\t (");
    switch (gm.g_mode)
    {
    case game_mode::DWARF:
        out.print("Dwarf)\n");
        break;
    case game_mode::ADVENTURE:
        out.print("Adventure)\n");
        break;
    case game_mode::num:
        out.print("INVALID)\n");
        break;
    case game_mode::NONE:
        out.print("NONE)\n");
        break;
    default:
        out.print("!!UNKNOWN!!)\n");
        break;
    }
}

command_result mode (color_ostream &out, vector <string> & parameters)
{
    string selected = "";
    bool set = false;
    bool abuse = false;
    t_gamemodes gm;
    for(auto iter = parameters.begin(); iter != parameters.end(); iter++)
    {
        if((*iter) == "-set")
        {
            set = true;
        }
        else if((*iter) == "-abuse")
        {
            set = abuse = true;
        }
        else if((*iter) == "-choice")
        {    
            iter++;
            selected = (*iter);
        }
        else
            return CR_WRONG_USAGE;
    }

    {
        CoreSuspender suspend;
        World::ReadGameMode(gm);
    }

    printCurrentModes(gm, out);

    out.print("\nPossible choices:\n");
    out.print("0 = Fortress Mode\n");
    out.print("1 = Adventurer Mode\n");
    out.print("2 = Arena Mode\n");
    out.print("3 = Arena, controlling creature\n");
    out.print("4 = Reclaim Fortress Mode\n");

    if(set)
    {
        if(!abuse)
        {
            if( gm.g_mode == game_mode::NONE || gm.g_type == game_type::VIEW_LEGENDS)
            {
                out.printerr("It is not safe to set modes in menus.\n");
                return CR_FAILURE;
            }
            uint32_t select=99;
            const char * start = selected.c_str();
            char * end = 0;
            select = strtol(start, &end, 10);
            if(!end || end==start || select > 4)
            {
                out.printerr("This is not a valid selection.\n");
                return CR_FAILURE;
            }
            switch(select)
            {
                case 0:
                    gm.g_mode = game_mode::DWARF;
                    gm.g_type = game_type::DWARF_MAIN;
                    break;
                case 1:
                    gm.g_mode = game_mode::ADVENTURE;
                    gm.g_type = game_type::ADVENTURE_MAIN;
                    break;
                case 2:
                    gm.g_mode = game_mode::DWARF;
                    gm.g_type = game_type::DWARF_ARENA;
                    break;
                case 3:
                    gm.g_mode = game_mode::ADVENTURE;
                    gm.g_type = game_type::ADVENTURE_ARENA;
                    break;
                case 4:
                    gm.g_mode = game_mode::DWARF;
                    gm.g_type = game_type::DWARF_RECLAIM;
                    break;
            }
        }
        else
        {
            const char * start = selected.c_str();
            gm.g_mode = (GameMode) strtol(start, 0, 10);
            start = selected.c_str();
            gm.g_type = (GameType) strtol(start, 0, 10);
        }

        {
            CoreSuspender suspend;
            World::WriteGameMode(gm);
        }

        out.print("\n");
    }
    return CR_OK;
}
