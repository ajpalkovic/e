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

#include "BuildPane.h"
#include "BuildErrorsManager.h"

#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "Env.h"

enum {
	CTRL_OUTPUT,
	CTRL_WINDOW
};

BEGIN_EVENT_TABLE(BuildPane, wxPanel)
	EVT_IDLE(BuildPane::OnIdle)
END_EVENT_TABLE()

BuildPane::BuildPane(EditorFrame* editorFrame, BuildErrorsManager* errorManager, bool keep_open) : 
	wxPanel(editorFrame, wxID_ANY),
	m_keepOpen(keep_open), m_errorManager(errorManager), m_outputLastUpdated(0) {
	
	m_staticTextCtrl = new wxStaticText(this, CTRL_OUTPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_staticTextCtrl, 0, wxEXPAND);

	SetSizer(mainSizer);
}

bool BuildPane::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

void BuildPane::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	bool outputChanged = false;
	wxString output = m_errorManager->GetOutput(&outputChanged, &m_outputLastUpdated);
	if(outputChanged) {
		m_staticTextCtrl->SetLabel(output);
	}
}