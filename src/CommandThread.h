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

#ifndef __COMMANDTHREAD_H__
#define __COMMANDTHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/event.h>
#endif

#include <vector>

class CommandThread : public wxThread {
public:
	CommandThread(wxString& command, cxEnv& env, bool* shouldExecute);
	virtual void Notify(wxString& output) {}
	int Execute();
	void* Entry();
private:
	bool* m_shouldExecute;
	std::vector<char>& m_command;
	cxEnv& m_env;
	ShellRunner shell;
};