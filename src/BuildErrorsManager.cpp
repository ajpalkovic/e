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
#include "wx/filename.h"

#include <vector>
#include <map>

#include "BuildSettings.h"
#include "BuildError.h"
#include "BuildThread.h"
#include "BuildPane.h"
#include "BuildErrorsManager.h"

#include "EditorCtrl.h"
#include "Env.h"

using namespace std;

/**
 * This is the core class for the build system.
 * All of the data is stored here, and all the different components just call methods to access it from here.
 * So, this is the only class that really needs a mutex.
 */
BuildErrorsManager::BuildErrorsManager() : 
  m_outputLastChanged(0), m_settingsChanged(true), m_settingsSet(false), m_needBuild(true) {
	m_buildThread = new BuildThread(this);
	GetBuildSettings();
}

BuildErrorsManager::~BuildErrorsManager() {
	m_buildThread->Delete();
}

wxString& BuildErrorsManager::GetOutput(bool* outputChanged, int* lastPaneUpdate) {
	wxMutexLocker lock(m_mutex);
	
	if(*lastPaneUpdate >= m_outputLastChanged) {
		*outputChanged = false;
	} else {
		*lastPaneUpdate = m_outputLastChanged;
		*outputChanged = true;
	}
	
	return m_output;
}


void BuildErrorsManager::BuildSettingsChanged() {
	wxMutexLocker lock(m_mutex);
	m_settingsChanged = true;
	m_needBuild = true;
}

BuildSettings BuildErrorsManager::GetBuildSettings() {
	wxMutexLocker lock(m_mutex);
	if(m_settingsSet && !m_settingsChanged) return m_settings;
	
	m_settings = BuildSettings(wxString(wxT("tail -n 50 /cygdrive/c/Dev/errors.txt")));
	m_settings.AddRegex(wxString(_("[0-9]*>\\s*([^\\(\\) \\n]*)\\(([0-9]*)\\)\\s*:\\s*(fatal error|error)(.*)")), 1, 2, -1, 4, 3, false);
	m_settings.AddRegex(wxString(_("[0-9]*>\\s*([^\\(\\) \\n]*)\\(([0-9]*)\\)\\s*:\\s*(warning)(.*)")), 1, 2, -1, 4, 3, false);
	
	m_settingsSet = true;
	m_settingsChanged = false;
	return m_settings;
}


cxEnv BuildErrorsManager::GetEnv() {
	//Is this right?????
	wxMutexLocker lock(m_mutex);
	return m_env;
}

void BuildErrorsManager::UpdateEnv(EditorCtrl* editor) {
	wxString fileList = wxT("");
	for(unsigned int c = 0; c < m_files.size()-1; c++) {
		fileList += m_files[c].GetFullPath();
		fileList += wxT(";");
	}
	fileList += m_files[m_files.size()-1].GetFullPath();

	//Update the cxEnv inside of the wx event thread, instead of in the build thread.
	m_env = cxEnv();
	editor->SetEnv(m_env, true, NULL, true);
	m_env.SetEnv(wxT("BUILD_FILES"), fileList);
}


bool BuildErrorsManager::ShouldBuild() {
	wxMutexLocker lock(m_mutex);
	bool ret = m_settingsSet && m_needBuild;
	if(ret) {
		m_needBuild = false;
	}
	//Clear the file list before the build starts so that it is not picked up by another build command.
	m_files.clear();
	return ret;
}


void BuildErrorsManager::OnFileSaved(EditorCtrl* editor) {
	wxMutexLocker lock(m_mutex);

	m_needBuild = true;
	m_files.push_back(editor->GetFilePath());

	UpdateEnv(editor);
}

void BuildErrorsManager::OnFilesSaved(std::vector<EditorCtrl*> editors) {
	wxMutexLocker lock(m_mutex);

	if(editors.size() == 0) return;

	m_needBuild = true;

	for(unsigned int c = 0; c < editors.size(); c++) {
		m_files.push_back(editors[c]->GetFilePath());
	}

	UpdateEnv(editors[0]);
}

void BuildErrorsManager::ParseErrors(std::vector<BuildError>& errors, wxString& buildOutput, std::vector<char>& buildOutputVector, BuildRegex& regex) {
	//can i do m_errors = errors?
	//when i call .push_back is the BuildError copied?
	//if i do the assignment do the BuildError objects get copied again?
	
	unsigned int pos = 0;
	while(pos < buildOutput.length()) {
		map<unsigned int, interval> matches;
		search_result sr = EditorCtrl::RawRegexSearch(regex.regex.mb_str(wxConvUTF8), buildOutputVector, pos, &matches);
		BuildError error(buildOutput, matches, regex);
	
		wxLogDebug(wxT("File: %s\n Line: %d\n Message: %s\n"), error.filename, error.lineNumber, error.message);
		errors.push_back(error);
		
		if(sr.error_code < 0) break;
	
		//avoid infinite loop on non-matches
		if(sr.end == pos) sr.end++;
		pos = sr.end;
	}
}

void BuildErrorsManager::BuildComplete(wxString& buildOutput) {
	//create a copy of BuildSettings so that we can do the regex search without locking the mutex
	BuildSettings settings = GetBuildSettings();
	
	std::vector<BuildError> errors;
	std::vector<char> buildOutputVector;
	for(unsigned int c = 0; c < buildOutput.length(); c++) {
		buildOutputVector.push_back(buildOutput[c]);
	}
	
	for(unsigned int c = 0; c < settings.regexes.size(); c++) {
		ParseErrors(errors, buildOutput, buildOutputVector, settings.regexes[c]);
	}
	
	//now secure the mutex and update the instance variables
	wxMutexLocker lock(m_mutex);
	
	//will this copy buildOutput?
	m_outputLastChanged++;
	m_output = buildOutput;
	
	//do I need to manually copy?
	m_errors.clear();
	for(unsigned int c = 0; c < errors.size(); c++) {
		m_errors.push_back(errors[c]);
	}
}

/**
 * Errors are stored in two ways:
 * There is a vector of each build error stored in m_errors.
 * There is a map which breaks down the errors by document and then by line.
 * To save on execution time, the errors are not broken down by line/editor as soon as a build is completed.
 * Instead, that is delayed until the information is actually needed.
 * Therefore, there is a second map, which maps the editor id to a change token, 
 * representing the last time the build errors were parsed for that document.
 */
bool BuildErrorsManager::HasError(EditorCtrl& editor, unsigned int line) {
	wxMutexLocker lock(m_mutex);
	
	//Check if the list of errors has been updated since we last parsed them for this document.
	unsigned int id = editor.GetDocID().document_id;
	if(m_editorErrorsLastUpdated.find(id) == m_editorErrorsLastUpdated.end() || m_editorErrorsLastUpdated[id] < m_outputLastChanged) {
		ProcessEditorErrors(editor);
	}
	m_editorErrorsLastUpdated[id] = m_outputLastChanged;
	
	//Technically, I don't need the first condition, but it should save a bit on memory
	return m_editorErrors[id].find(line) != m_editorErrors[id].end() && m_editorErrors[id][line].hasError;
}

bool BuildErrorsManager::HasWarning(EditorCtrl& editor, unsigned int line) {
	wxMutexLocker lock(m_mutex);
	
	unsigned int id = editor.GetDocID().document_id;
	if(m_editorErrorsLastUpdated.find(id) == m_editorErrorsLastUpdated.end() || m_editorErrorsLastUpdated[id] < m_outputLastChanged) {
		ProcessEditorErrors(editor);
	}
	m_editorErrorsLastUpdated[id] = m_outputLastChanged;
	
	return m_editorErrors[id].find(line) != m_editorErrors[id].end() && m_editorErrors[id][line].hasWarning;
}

void BuildErrorsManager::ProcessEditorErrors(EditorCtrl& editor) {
	//if i set m_errors = errors first, will that copy errors into the map?
	//can i just update errors and m_errors will be auto updated?
	map<int, BuildErrorLine> errors;
	wxFileName filename = editor.GetFilePath();
	unsigned int id = editor.GetDocID().document_id;
	
	for(unsigned int c = 0; c < m_errors.size(); c++) {
		BuildError* error = &(m_errors[c]);
		if(error->lineNumber > -1 && error->Matches(filename)) {
			errors[error->lineNumber].AddError(error);
		}
	}
	
	m_editorErrors[id] = errors;
}
