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

#ifndef _INCLUDE_SOURCEMOD_SIGNALS_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_SIGNALS_EXTENSION_PROPER_H_

/**
 *  @file		extension.h
 *  @brief		Main definitions file of the extension.
 */

#include "smsdk_ext.h"
#include "signal.h"

enum ErrCodes
{
    NoError = 0,
    Error,
    SigactionError,
    SAHandlerError,
    ForwardError,
    FuncCountError,
    CallbackError
};

class SignalForwards : public SDKExtension
{
public:
    /**
     *  @brief	Initialize Signals extension.
     */
    virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);

    /**
     *  @brief	Cleanup and release all the forwards.
     */
    virtual void SDK_OnUnload();
};

/**
 *  @brief	Native functions exposed to plugins
 */
cell_t CreateHandler(IPluginContext* pContext, const cell_t* params);

/**
 *  @brief	Native functions exposed to plugins
 */
cell_t RemoveHandler(IPluginContext* pContext, const cell_t* params);

/**
 *  @brief	SigAction specifies the signal handler in a sigaction struct where the SA_SIGINFO flag is set.
 *  Only functions that are considered async-signal-safe should be called from within a signal handler.
 * 
 *  Calling an IForwardManager->Execute() MIGHT be reentrant and therefore async-signal-safe
 *  ...\sourcemod\sourcepawn\vm\scripted-invoker.cpp : ScriptedInvoker::Execute(cell_t* result)
 *  and ScriptedInvoker::Invoke(cell_t* result) seem to imply so, also
 *  ...\sourcemod\core\logic\ForwardSys.cpp : CForward::Execute(cell_t *result, IForwardFilter *filter)
 * 
 *  Are HeapAlloc and HeapPop async-safe? If number of params is kept zero then these should never be called
 * 
 *  @param signal       Numeric value of the signal invoking the handler
 *  @param info	        Structure containing further information about the signal
 *  @param ucontext     Pointer to a ucontext_t struct containing user-space stack information
 */
void SigAction(int signal, siginfo_t* info, void* ucontext);

/* Helper functions*/

int SetSAHandler(int signal);
int ResetSAHandler(int signal);

#endif // _INCLUDE_SOURCEMOD_SIGNALS_EXTENSION_PROPER_H_
