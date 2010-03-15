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

#ifndef __BUILDERRORSMANAGER_H__
#define __BUILDERRORSMANAGER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include <map>

#include "BuildSettings.h"
#include "BuildError.h"

class BuildPane;
class BuildThread;
class EditorCtrl;

class BuildErrorsManager {
public:
	BuildErrorsManager();
	~BuildErrorsManager();
	
	void OpenBuildPane(BuildPane* pane);
	void CloseBuildPane();
	wxString& GetOutput(bool* outputChanged, int* lastPaneUpdate);
	
	BuildSettings GetBuildSettings();
	void BuildSettingsChanged();
	
	bool CanBuild();
	bool ShouldBuild();
	void PerformBuild();
	void BuildComplete(wxString& output);
	void ParseErrors(BuildSettings& settings, std::vector<BuildError>& errors, wxString& buildOutput, std::vector<char>& buildOutputVector, const char* regex);
	
	void OnFileSaved();
	
	bool HasError(EditorCtrl& editor, unsigned int line);
	bool HasWarning(EditorCtrl& editor, unsigned int line);
private:
	void ProcessEditorErrors(EditorCtrl& editor);
	
	BuildPane* m_pane;
	BuildThread* m_buildThread;
	
	bool m_settingsChanged, m_settingsSet;
	BuildSettings m_settings;
	
	wxString m_output;
	int m_outputLastChanged;
	
	bool m_needBuild;
	
	std::map<int, int> m_editorErrorsLastUpdated;
	std::map<int, std::map<int, BuildError*>> m_editorErrors;
	std::vector<BuildError> m_errors;
	
	wxMutex m_mutex;
};

#endif