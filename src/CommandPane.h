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
	#include <wx/panel.h>
    #include <wx/textctrl.h>
#endif

#include <vector>

class EditorFrame;
class CommandPaneThread;

class CommandPane : public wxPanel {
public:
	CommandPane(EditorFrame* editorFrame, bool keepOpen=true);
	bool Destroy();
	void OnFileSaved();
	void UpdateOutput(wxString& output);

private:
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	bool m_keepOpen, m_outputChanged, m_needBuild;
	wxString* m_output;
	
	wxStaticText* m_outputTextCtrl;
	CommandPaneThread* m_thread;
};

class CommandPaneThread : public CommandThread {
public:
	CommandPaneThread(CommandPane& pane);
	void Notify(wxString& output);
private:
	CommandPane& m_pane;
}
#endif // __COMMANDPANE_H__
