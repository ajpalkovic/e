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
#include "wx/filename.h"

#include <vector>
#include <map>

#include "BuildSettings.h"
#include "BuildError.h"

#include "Env.h"

class BuildPane;
class BuildThread;
class EditorCtrl;

class BuildErrorsManager {
public:
	BuildErrorsManager();
	~BuildErrorsManager();
	
	wxString& GetOutput(bool* outputChanged, int* lastPaneUpdate);
	
	void LoadSettings();
	void SaveSettings();
	std::vector<BuildSettings>& GetAllSettings() { return m_settings; }
	BuildSettings GetBuildSettings();
	void BuildSettingsChanged();

	cxEnv GetEnv();
	void UpdateEnv(EditorCtrl* editor);
	
	bool ShouldBuild();
	void BuildComplete(wxString& output);
	void ParseErrors(std::vector<BuildError>& errors, wxString& buildOutput, std::vector<char>& buildOutputVector, BuildRegex& regex);
	
	void OnFileSaved(EditorCtrl* editor);
	void OnFilesSaved(std::vector<EditorCtrl*> editors);
	
	bool HasError(EditorCtrl& editor, unsigned int line);
	bool HasWarning(EditorCtrl& editor, unsigned int line);
	wxString GetErrorMessage(EditorCtrl& editor, unsigned int line);
private:
	void ProcessEditorErrors(EditorCtrl& editor);
	
	BuildThread* m_buildThread;
	
	BuildSettings* m_currentSettings;
	std::vector<BuildSettings> m_settings;
	wxString m_path;

	cxEnv m_env;
	
	wxString m_output;
	int m_outputLastChanged;
	
	bool m_needBuild;
	
	std::map<int, int> m_editorErrorsLastUpdated;
	std::map<int, std::map<int, BuildErrorLine>> m_editorErrors;
	std::vector<BuildError> m_errors;

	std::vector<wxFileName> m_files;
	
	wxMutex m_mutex;
};

#endif