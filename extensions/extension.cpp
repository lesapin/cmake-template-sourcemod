/**
 * SourceMod CMake Template
 * Copyright (C) 2024 bezdmn
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *	@file		extension.cpp
 *	@brief		Main implementation file of the extension.
 */

#include <map>
#include "extension.h"

//#define DEBUG

#ifdef DEBUG
    #define DBGPRINT(...) rootconsole->ConsolePrint(__VA_ARGS__)
#else
    #define DBGPRINT(...) 0
#endif

using namespace std;

SignalForwards g_Extension;    /**< Global singleton for extension's main interface	*/

SMEXT_LINK(&g_Extension);

const sp_nativeinfo_t SignalNatives[] =
{
    {"CreateHandler",   CreateHandler},
    {"RemoveHandler",   RemoveHandler},
    {NULL,              NULL},
};

map<int, IChangeableForward*> g_Forwards;

cell_t CreateHandler(IPluginContext* pContext, const cell_t* params)
{
    int Signal = static_cast<int>(params[1]);

    IChangeableForward* Forward = forwards->CreateForwardEx(NULL, ET_Event, 0, NULL);

    int Err = SetSAHandler(Signal);
    if (Err == NoError)
    {
        auto it = g_Forwards.find(Signal);
        if (it == g_Forwards.end())
        {
            if (Forward == nullptr)
            {
                rootconsole->ConsolePrint("ERROR: Failed to create forward %i", Signal);
                Err = ForwardError;
            }
            else if (!Forward->AddFunction(pContext, static_cast<funcid_t>(params[2])))
            {
                rootconsole->ConsolePrint("ERROR: Failed to add callback function for signal %i", Signal);
                Err = CallbackError;
            }
        }
        else
        {
            DBGPRINT("Forward for signal %i is already assigned", Signal);
            Err = FuncCountError; // not fatal
        }
    }

    if (Err == NoError)
    {
        g_Forwards.insert({ Signal, Forward });
    }
    else
    {
        forwards->ReleaseForward(Forward);
    }

    return Err;
}

cell_t RemoveHandler(IPluginContext* pContext, const cell_t* params)
{
    int Signal = static_cast<int>(params[1]);
    auto it = g_Forwards.find(Signal);

    if (it != g_Forwards.end())
    {
        if (ResetSAHandler(it->first) != NoError)
        {
            // ...
        }

        forwards->ReleaseForward(it->second);
        g_Forwards.erase(it);
    }

    return 1;
}

void SigAction(int signal, siginfo_t* info, void* ucontext)
{
    // sigaction() itself is reentrant and therefore safe to call.
    SetSAHandler(signal); // have to reset these

    // Execute the callback. I'm currently convinced this is reentrant.
    g_Forwards.find(signal)->second->Execute();
}

bool SignalForwards::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    sharesys->AddNatives(myself, SignalNatives);
    DBGPRINT("Signals extension loaded");
    return true;
}

void SignalForwards::SDK_OnUnload()
{
    for (auto it = g_Forwards.begin(); it != g_Forwards.end(); it++)
    {
        ResetSAHandler(it->first);
        forwards->ReleaseForward(it->second); 
    }
}

/* Helper functions*/

int SetSAHandler(int signal)
{
    struct sigaction SigactionNew {}, SigactionOld{};

    // By default, only the signal itself will be blocked. 
    sigemptyset(&SigactionNew.sa_mask);
    sigaddset(&SigactionNew.sa_mask, signal);

    // Use the sa_sigaction() handler instead of the outdated signal().
    SigactionNew.sa_flags = SA_SIGINFO;

    // Set the sigaction handler.
    SigactionNew.sa_sigaction = SigAction;

    if (sigaction(signal, &SigactionNew, &SigactionOld) != 0)
    {
        rootconsole->ConsolePrint("ERROR: Failed to add sigaction handler for signal %i", signal);
        return SigactionError;
    }
    else if (SigactionOld.sa_handler != SIG_DFL && SigactionOld.sa_handler != SIG_IGN)
    {
        DBGPRINT("A handler already exists for signal %i", signal);
        return SAHandlerError;
    }

    return NoError;
}

int ResetSAHandler(int signal)
{
    // Reset the signals disposition to OS default and clear the forward handle.

    struct sigaction SigactionDefault {}, Empty {};

    // Could store previous actions into an array and restore from there.

    sigemptyset(&SigactionDefault.sa_mask);
    sigaddset(&SigactionDefault.sa_mask, signal);

    SigactionDefault.sa_flags = 0;

    // Set the default handler.
    SigactionDefault.sa_handler = SIG_DFL;

    if (sigaction(signal, &SigactionDefault, &Empty) != 0)
    {
        rootconsole->ConsolePrint("ERROR: Failed to reset sa_handler for signal %i", signal);
        return SigactionError;
    }

    return NoError;
}