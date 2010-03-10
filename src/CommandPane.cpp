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

#include "EditorFrame.h"
#include "CommandPane.h"

using namespace std;

enum {
	CTRL_OUTPUT
};

BEGIN_EVENT_TABLE(CommandPane, wxPanel)
	EVT_IDLE(CommandPane::OnIdle)
END_EVENT_TABLE()

CommandPane::CommandPane(EditorFrame* editorFrame, bool keep_open) : 
	wxPanel(editorFrame, wxID_ANY),
	m_keepOpen(keep_open), m_outputChanged(false), m_needBuild(false) {
	
	m_thread = new CommandPaneThread()
	m_staticTextCtrl = new wxStaticText(this, CTRL_OUTPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_staticTextCtrl, 0, wxEXPAND);

	SetSizer(mainSizer);
}

void CommandPane::UpdateOutput(wxString& output) {
	//need mutex
	delete m_output;
	//Right now output is allocated on the thread's stack, so i'm pretty sure we need to move it out.  Even if we don't, this still works.
	m_output = new wxString(output);
	m_outputChanged = true;
}

void CommandPane::OnIdle(wxIdleEvent& event) {
	//need mutex with UpdateOutput
	if(m_outputChanged) {
		m_staticTextCtrl->SetLabel(*m_output);
		m_outputChanged = false;
	}
}

void CommandPane::OnFileSaved() {
	m_needBuild = true;
}

CommandPaneThread::CommandPaneThread(CommandPane& pane) : m_pane(pane) {}
void CommandPaneThread::Notify(wxString& output) {
	m_pane.UpdateOutput(output);
}