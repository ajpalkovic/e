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

#ifndef __BUILDPANE_H__
#define __BUILDPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

class EditorFrame;
class BuildErrorsManager;

class BuildPane : public wxPanel {
public:
	BuildPane(EditorFrame* editorFrame, BuildErrorsManager* errorManager, bool keepOpen=true);
	bool Destroy();

private:
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	bool m_keepOpen;
	int m_outputLastUpdated;
	BuildErrorsManager* m_errorManager;
	
	wxStaticText* m_staticTextCtrl;
};

#endif // __BUILDPANE_H__
