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
	#include <wx/wx.h>
#endif

#include <vector>

#include "BuildThread.h"
#include "BuildError.h"
#include "BuildErrorsManager.h"
#include "BuildSettings.h"

#include "ShellRunner.h"
#include "Env.h"
//#include "eSettings.h"

BuildThread::BuildThread(BuildErrorsManager* errorManager) : m_errorManager(errorManager) {
	Create();
	Run();
}

void* BuildThread::Entry() {
	while(true) {
		TestDestroy();
		
		if(m_errorManager->ShouldBuild()) {
			wxLogDebug(wxT("!!!!!!!!!run command"));
			m_errorManager->PerformBuild();
			BuildSettings settings = m_errorManager->GetBuildSettings();
			
			cxEnv env;
			env.SetToCurrent();
			//env.SetEnv(eGetSettings().env);
			wxString output = shell.RunShellCommand(settings.commandVector, env, true);
			
			m_errorManager->BuildComplete(output);
		}
		
		TestDestroy();
		wxMilliSleep(50);
	}
}