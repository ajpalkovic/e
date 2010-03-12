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

#ifndef __COMMANDPANE_H__
#define __COMMANDPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include "CommandThread.h"

class EditorFrame;
class CommandPaneThread;

class ErrorMessage {
public:
	ErrorMessage(wxString& output, interval& filenameInterval, interval& lineNumberInterval, interval& errorInterval, interval& messageInterval);
	bool Matches(wxFileName& file);
	unsigned int lineNumber;
	wxString filename, message;
	bool isError;
};

class CommandPane : public wxPanel {
public:
	CommandPane(EditorFrame* editorFrame, bool keepOpen=true);
	bool Destroy();
	void OnFileSaved();
	void UpdateOutput(wxString& output);
	void GetErrors(std::vector<ErrorMessage>& errors, EditorCtrl& editor);

private:
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	bool m_keepOpen, m_outputChanged, m_needBuild;
	wxString* m_output;
	std::vector<ErrorMessage> m_errors;
	
	wxStaticText* m_staticTextCtrl;
	CommandPaneThread* m_thread;
	wxMutex mutex;
};

class CommandPaneThread : public CommandThread {
public:
	CommandPaneThread(CommandPane& pane, bool* shouldExecute);
	void Notify(wxString& output);
private:
	CommandPane& m_pane;
	wxMutex* mutex;
};

#endif // __COMMANDPANE_H__
