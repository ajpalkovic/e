/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/event.h>
#endif

#include <vector>

#include "CommandThread.h"
#include "ShellRunner.h"
#include "Env.h"

CommandThread::CommandThread(wxString& command, cxEnv* env, bool* shouldExecute) : 
	m_shouldExecute(shouldExecute), ready(true), m_command(NULL) {
	UpdateCommand(command, env);
	Create();
	Run();
}

CommandThread::CommandThread(bool* shouldExecute) :
	m_shouldExecute(shouldExecute), ready(false), m_command(NULL)  {
	Create();
	Run();
}

void CommandThread::UpdateCommand(wxString& command, cxEnv* env) {
	wxMutexLocker lock(mutex);
	if(m_command != NULL) {
		delete m_command;
		delete m_env;
	}
	m_command = new std::vector<char>();
	for(unsigned int c = 0; c < command.length(); c++) {
		m_command->push_back(command[c]);
	}
	m_env = env;
	ready = true;
}

void* CommandThread::Entry() {
	while(true) {
		if(m_command != NULL && (*m_shouldExecute)) {
			wxLogDebug(wxT("!!!!!!!!!run command"));
			*m_shouldExecute = false;
			
			//If the lock extends past the shell.RunShellCommand call, then the lock will effectively negate the benefit of using a thread if UpdateCommand is called.
			//It is sort of debatable whether or not UpdateCommand will ever get called while the command is running.
			//Regardless, to get around it, I just make a copy of the command/env variables so that the lock can be released.
			mutex.Lock();
			std::vector<char> _command = std::vector<char>(*m_command);
			cxEnv _env = cxEnv(*m_env);
			mutex.Unlock();
			wxString output = shell.RunShellCommand(_command, _env, true);
			Notify(output);
		}
		wxMilliSleep(50);
	}
}