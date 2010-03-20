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
#include <wx/listbook.h>

#include "BuildSettings.h"
#include "BuildSettingsDlg.h"
#include "BuildErrorsManager.h"

enum {
	CTRL_LISTBOX,
	CTRL_NAME,
	CTRL_COMMAND,
	CTRL_REGEX,
	CTRL_FILENAME_MATCH,
	CTRL_LINE_MATCH,
	CTRL_COLUMN_MATCH,
	CTRL_MESSAGE_MATCH,
	CTRL_ERROR_MATCH,
	CTRL_OVERWRITE,
	CTRL_SAVE_NEW,
	CTRL_DELETE,
	CTRL_HELP,
	CTRL_ADD_REGEX,
	CTRL_DELETE_REGEX
};

BEGIN_EVENT_TABLE(BuildSettingsDlg, wxDialog)
	EVT_BUTTON(CTRL_OVERWRITE, BuildSettingsDlg::OnButtonOverwrite)
	EVT_BUTTON(CTRL_SAVE_NEW, BuildSettingsDlg::OnButtonSaveNew)
	EVT_BUTTON(CTRL_DELETE, BuildSettingsDlg::OnButtonDelete)
	EVT_BUTTON(CTRL_HELP, BuildSettingsDlg::OnButtonHelp)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BuildSettingsPanel, wxPanel)
	EVT_BUTTON(CTRL_ADD_REGEX, BuildSettingsPanel::OnButtonAddRegex)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BuildRegexPanel, wxPanel)
	EVT_BUTTON(CTRL_DELETE_REGEX, BuildRegexPanel::OnButtonDeleteRegex)
END_EVENT_TABLE()

#define STEXT(text) new wxStaticText(this, -1, wxT(text)), 0, wxEXPAND
#define SRTEXT(text) new wxStaticText(this, -1, wxT(text), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 0, wxEXPAND

BuildSettingsDlg::BuildSettingsDlg(wxWindow* parent, BuildErrorsManager* errorManager) :
	wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
	m_errorManager(errorManager), m_settings(errorManager->GetAllSettings()) {
		
	SetTitle (_("Build Settings"));

	wxListbook* lbox = new wxListbook(this, CTRL_LISTBOX, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT);

	for(unsigned int c = 0; c < m_settings.size(); c++) {
		m_panels.push_back(new BuildSettingsPanel(lbox, m_settings[c]));
		lbox->AddPage(m_panels[c], m_settings[c].name);
	}

	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(lbox, 0, wxEXPAND|wxALL);

		wxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
			buttonSizer->Add(new wxButton(this, CTRL_OVERWRITE, wxT("Save")));
			buttonSizer->Add(new wxButton(this, CTRL_SAVE_NEW, wxT("Save New")));
			buttonSizer->Add(new wxButton(this, CTRL_DELETE, wxT("Delete")));
			buttonSizer->Add(new wxButton(this, CTRL_HELP, wxT("Help")));
		mainSizer->Add(buttonSizer, 0, wxEXPAND);
		mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);
	//SetMaxSize(wxSize(500, 400));

	Centre();
}

void BuildSettingsDlg::OnButtonOverwrite(wxCommandEvent& WXUNUSED(event)) {

}

void BuildSettingsDlg::OnButtonSaveNew(wxCommandEvent& WXUNUSED(event)) {

}

void BuildSettingsDlg::OnButtonDelete(wxCommandEvent& WXUNUSED(event)) {

}

void BuildSettingsDlg::OnButtonHelp(wxCommandEvent& WXUNUSED(event)) {

}

BuildSettingsPanel::BuildSettingsPanel(wxWindow* parent, BuildSettings& settings) :
	wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxVSCROLL),
	m_settings(settings) {

	nameCtrl = new wxTextCtrl(this, CTRL_NAME, settings.name, wxDefaultPosition, wxDefaultSize, 0);
	commandCtrl = new wxTextCtrl(this, CTRL_COMMAND, settings.command, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_BESTWRAP);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(STEXT("Name:"));
		mainSizer->Add(nameCtrl, 0, wxEXPAND);
		mainSizer->Add(STEXT("Command:"));
		mainSizer->Add(commandCtrl, 0, wxEXPAND);
		mainSizer->Add(STEXT("Regular Expressions:"));
		regexSizer = new wxBoxSizer(wxVERTICAL);
			for(unsigned int c = 0; c < settings.regexes.size(); c++) {
				regexSizer->Add(new BuildRegexPanel(this, *this, settings.regexes[c]), 0, wxEXPAND);
			}
		mainSizer->Add(regexSizer);

	SetSizer(mainSizer);
}

void BuildSettingsPanel::OnButtonAddRegex(wxCommandEvent& WXUNUSED(event)) {

}

BuildRegexPanel::BuildRegexPanel(wxWindow* parent, BuildSettingsPanel& settingsPanel, BuildRegex& regex) :
	wxPanel(parent, -1),
	m_regex(regex), m_settingsPanel(settingsPanel) {

	regexCtrl = new wxTextCtrl(this, CTRL_REGEX, regex.regex, wxDefaultPosition, wxDefaultSize, 0);
	filenameMatchCtrl = new wxTextCtrl(this, CTRL_FILENAME_MATCH, wxString::Format(wxT("%d"), regex.filenameMatch), wxDefaultPosition, wxDefaultSize, 0);
	filenameMatchCtrl->SetMaxLength(2);
	lineMatchCtrl = new wxTextCtrl(this, CTRL_LINE_MATCH, wxString::Format(wxT("%d"), regex.lineMatch), wxDefaultPosition, wxDefaultSize, 0);
	lineMatchCtrl->SetMaxLength(2);
	columnMatchCtrl = new wxTextCtrl(this, CTRL_COLUMN_MATCH, wxString::Format(wxT("%d"), regex.columnMatch), wxDefaultPosition, wxDefaultSize, 0);
	columnMatchCtrl->SetMaxLength(2);
	messageMatchCtrl = new wxTextCtrl(this, CTRL_MESSAGE_MATCH, wxString::Format(wxT("%d"), regex.messageMatch), wxDefaultPosition, wxDefaultSize, 0);
	messageMatchCtrl->SetMaxLength(2);
	errorMatchCtrl = new wxTextCtrl(this, CTRL_ERROR_MATCH, wxString::Format(wxT("%d"), regex.errorMatch), wxDefaultPosition, wxDefaultSize, 0);
	errorMatchCtrl->SetMaxLength(2);
		

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(STEXT("Regex:"));
		mainSizer->Add(regexCtrl, 0, wxEXPAND);
		
		wxBoxSizer* matchSizer = new wxBoxSizer(wxHORIZONTAL);
			wxGridSizer* leftSizer = new wxGridSizer(2, 3);
				leftSizer->Add(SRTEXT("Filename:"));
				leftSizer->Add(filenameMatchCtrl);
				
				leftSizer->Add(SRTEXT("Line Number:"));
				leftSizer->Add(lineMatchCtrl);

				leftSizer->Add(SRTEXT("Error/Warning:"));
				leftSizer->Add(errorMatchCtrl);
			matchSizer->Add(leftSizer, 0, wxEXPAND);

			wxGridSizer* rightSizer = new wxGridSizer(2, 3);
				rightSizer->Add(SRTEXT("Error Message:"));
				rightSizer->Add(messageMatchCtrl);

				rightSizer->Add(SRTEXT("Column Number:"));
				rightSizer->Add(columnMatchCtrl);

				rightSizer->Add(STEXT(""));
				rightSizer->Add(new wxButton(this, CTRL_DELETE_REGEX, wxT("Delete")));
			matchSizer->Add(rightSizer, 0, wxEXPAND);
		mainSizer->Add(matchSizer, 0, wxEXPAND);

	SetSizer(mainSizer);
}

void BuildRegexPanel::OnButtonDeleteRegex(wxCommandEvent& WXUNUSED(event)) {

}