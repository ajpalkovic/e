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

#ifndef __BUILDTHREAD_H__
#define __BUILDTHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>
#include "ShellRunner.h"

class BuildErrorsManager;

class BuildThread : public wxThread {
public:
	BuildThread(BuildErrorsManager* errorManager);
	void* Entry();
private:
	BuildErrorsManager* m_errorManager;
	ShellRunner shell;
};

#endif