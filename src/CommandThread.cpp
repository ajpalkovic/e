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

CommandThread::CommandThread(wxString& command, cxEnv& env, bool* shouldExecute) : 
	m_command(command), m_env(env), m_shouldExecute(shouldExecute), ready(true) {
	
}

CommandThread::CommandThread(bool* shouldExecute) :
	m_shouldExecute(shouldExecute), ready(false) {
	
}

void UpdateCommand(wxString& command, cxEnv& env) {
	//need mutex for this part probably
	this->command = command;
	this->env = env;
	ready = true;
}

int CommandThread::Execute() {
	Create();
	Run();
}

void* CommandThread::Entry() {
	while(true) {
		if(*m_shouldExecute) {
			*m_shouldExecute = false;
			wxString output = shell.RunShellCommand(m_command, m_env);
			Notify(output);
		}
		wxMilliSleep(50);
	}
}