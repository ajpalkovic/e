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
#include "EditorCtrl.h"
#include "CommandPane.h"
#include "Env.h"

enum {
	CTRL_OUTPUT
};

BEGIN_EVENT_TABLE(CommandPane, wxPanel)
	EVT_IDLE(CommandPane::OnIdle)
END_EVENT_TABLE()

ErrorMessage::ErrorMessage(wxString& output, interval& filenameInterval, interval& lineNumberInterval, interval& errorInterval, interval& messageInterval) {
	long num;
	wxString l = output.SubString(lineNumberInterval.start, lineNumberInterval.end-1);
	l.ToLong(&num);
	lineNumber = (unsigned int) num;

	filename = output.SubString(filenameInterval.start, filenameInterval.end-1);
	message = output.SubString(messageInterval.start, messageInterval.end-1);

	wxString e = output.SubString(errorInterval.start, errorInterval.end);
	isError = e.Lower().Contains(wxT("error"));
}

bool ErrorMessage::Matches(wxFileName& file) {
	return file.SameAs(filename);
}

CommandPane::CommandPane(EditorFrame* editorFrame, bool keep_open) : 
	wxPanel(editorFrame, wxID_ANY),
	m_keepOpen(keep_open), m_outputChanged(false), m_needBuild(true), m_output(NULL) {
	
	m_thread = new CommandPaneThread(*this, &m_needBuild);
	m_staticTextCtrl = new wxStaticText(this, CTRL_OUTPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_staticTextCtrl, 0, wxEXPAND);

	SetSizer(mainSizer);
	
	cxEnv* env = new cxEnv();
	env->SetToCurrent();
	env->SetEnv(eGetSettings().env);
	m_thread->UpdateCommand(wxString(_("tail -n 50 /cygdrive/c/Dev/errors.txt")), env);
}

bool CommandPane::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

void CommandPane::UpdateOutput(wxString& output) {
	const char* errorRegex = "[0-9]*>\\s*([^\\(\\)]*)\\(([0-9]*)\\)\\s*:\\s*(error|warning)(.*)";
	unsigned int pos = 0;
	std::vector<ErrorMessage> errors;
	while(pos < output.length()) {
		std::map<unsigned int, interval> matches;
		std::vector<char> outputVector;
		for(unsigned int c = 0; c < output.length(); c++) {
			outputVector.push_back(output[c]);
		}
		search_result sr = EditorCtrl::RawRegexSearch(errorRegex, outputVector, pos, &matches);
		wxLogDebug(wxT("File: %s\n Line: %s\n Message: %s\n"), output.SubString(matches[1].start, matches[1].end), output.SubString(matches[2].start, matches[2].end-1), output.SubString(matches[4].start, matches[4].end));
		errors.push_back(ErrorMessage(output, matches[1], matches[2], matches[3], matches[4]));
		if(sr.end == pos) sr.end++;
		pos = sr.end;
	}

	wxMutexLocker lock(mutex);
	//copy the errors into the instance variable
	m_errors.clear();
	for(int c = 0; c < errors.size(); c++) {
		m_errors.push_back(errors[c]);
	}
	
	//Right now output is allocated on the thread's stack, so i'm pretty sure we need to move it out.  Even if we don't, this still works.
	//Moreover, I don't think I should call m_staticTextCtrl->SetLabel from the thread, so I just copy the output to an instance variable and call SetLabel in the OnIdle event.
	if(m_output != NULL) {
		delete m_output;
	}
	m_output = new wxString(output);
	m_outputChanged = true;
}

void CommandPane::GetErrors(std::vector<ErrorMessage>& editorErrors, EditorCtrl& editor) {
	editorErrors.clear();
	wxFileName filename = editor.GetFilePath();

	wxMutexLocker lock(mutex);	
	for(int c = 0; c < m_errors.size(); c++) {
		if(m_errors[c].Matches(filename)) {
			editorErrors.push_back(m_errors[c]);
		}
	}
}

void CommandPane::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	wxMutexLocker lock(mutex);
	if(m_outputChanged) {
		m_staticTextCtrl->SetLabel(*m_output);
		m_outputChanged = false;
	}
}

void CommandPane::OnFileSaved() {
	m_needBuild = true;
}

CommandPaneThread::CommandPaneThread(CommandPane& pane, bool* shouldExecute) : CommandThread(shouldExecute), m_pane(pane) {}
void CommandPaneThread::Notify(wxString& output) {
	m_pane.UpdateOutput(output);
}