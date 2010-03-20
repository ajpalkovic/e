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

#ifndef __BUILDSETTINGSDLG_H__
#define __BUILDSETTINGSDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>

class BuildErrorsManager;
class BuildSettingsDlg;
class BuildSettings;
class BuildRegex;
class BuildSettingsPanel;

class BuildRegexPanel : public wxPanel {
public:
	BuildRegexPanel(wxWindow* parent, BuildSettingsPanel& settingsPanel, BuildRegex& regex);

private:
	DECLARE_EVENT_TABLE();
	void OnButtonDeleteRegex(wxCommandEvent& event);

	BuildSettingsPanel& m_settingsPanel;
	BuildRegex& m_regex;

	wxTextCtrl* regexCtrl;
	wxTextCtrl* filenameMatchCtrl;
	wxTextCtrl* lineMatchCtrl;
	wxTextCtrl* columnMatchCtrl;
	wxTextCtrl* messageMatchCtrl;
	wxTextCtrl* errorMatchCtrl;
	wxCheckBox* lineStartBox;
};

class BuildSettingsPanel : public wxPanel {
public:
	BuildSettingsPanel(wxWindow* parent, BuildSettings& settings);

private:
	DECLARE_EVENT_TABLE();
	void OnButtonAddRegex(wxCommandEvent& event);

	BuildSettings& m_settings;
	std::vector<BuildRegexPanel> m_regexes;

	wxTextCtrl* nameCtrl;
	wxTextCtrl* commandCtrl;
	wxSizer* regexSizer;
};

class BuildSettingsDlg : public wxDialog {
public:
	BuildSettingsDlg(wxWindow* parent, BuildErrorsManager* errorManager);

private:
	DECLARE_EVENT_TABLE();
	void OnButtonOverwrite(wxCommandEvent& WXUNUSED(event));
	void OnButtonSaveNew(wxCommandEvent& WXUNUSED(event));
	void OnButtonDelete(wxCommandEvent& WXUNUSED(event));
	void OnButtonHelp(wxCommandEvent& WXUNUSED(event));

	BuildErrorsManager* m_errorManager;
	std::vector<BuildSettings>& m_settings;
	std::vector<BuildSettingsPanel*> m_panels;
};

#endif