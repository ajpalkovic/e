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
#include <map>

#include "BuildSettings.h"
#include "BuildError.h"
#include "BuildThread.h"
#include "BuildPane.h"
#include "BuildErrorsManager.h"

#include "EditorCtrl.h"

using namespace std;

BuildErrorsManager::BuildErrorsManager() : 
  m_outputLastChanged(0), m_settingsChanged(true), m_settingsSet(false), m_needBuild(true), m_pane(NULL) {
	m_buildThread = new BuildThread(this);
	GetBuildSettings();
}

 BuildErrorsManager::~BuildErrorsManager() {
	m_buildThread->Delete();
}


void BuildErrorsManager::OpenBuildPane(BuildPane* pane) {
	m_pane = pane;
}

void BuildErrorsManager::CloseBuildPane() {
	m_pane = NULL;
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
	m_settings.SetErrorRegex(wxString(_("[0-9]*>\\s*([^\\(\\)]*)\\(([0-9]*)\\)\\s*:\\s*(error|warning)(.*)")), 1, 2, -1, 4, 3);
	m_settings.SetWarningRegex(wxString(_("[0-9]*>\\s*([^\\(\\)]*)\\(([0-9]*)\\)\\s*:\\s*(error|warning)(.*)")), 1, 2, -1, 4, 3);
	
	m_settingsSet = true;
	m_settingsChanged = false;
	return m_settings;
}


bool BuildErrorsManager::CanBuild() {
	return m_pane != NULL && m_settingsSet;
}

bool BuildErrorsManager::ShouldBuild() {
	wxMutexLocker lock(m_mutex);
	return CanBuild() && m_needBuild;
}

void BuildErrorsManager::PerformBuild() {
	wxMutexLocker lock(m_mutex);
	m_needBuild = false;
}

void BuildErrorsManager::OnFileSaved() {
	wxMutexLocker lock(m_mutex);
	m_needBuild = true;
}

void BuildErrorsManager::BuildComplete(wxString& buildOutput) {
	//create a copy of BuildSettings so that we can do the regex search without locking the mutex
	BuildSettings settings = GetBuildSettings();
	
	unsigned int pos = 0;
	std::vector<BuildError> errors;
	std::vector<char> buildOutputVector;
	for(unsigned int c = 0; c < buildOutput.length(); c++) {
		buildOutputVector.push_back(buildOutput[c]);
	}
	
	//can i do m_errors = errors?
	//when i call .push_back is the BuildError copied?
	//if i do the assignment do the BuildError objects get copied again?
	
	while(pos < buildOutput.length()) {
		map<unsigned int, interval> matches;
		search_result sr = EditorCtrl::RawRegexSearch("[0-9]*>\\s*([^\\(\\)]*)\\(([0-9]*)\\)\\s*:\\s*(error|warning)(.*)", buildOutputVector, pos, &matches);
		BuildError error(buildOutput, matches, settings.error);
		
		wxLogDebug(wxT("File: %s\n Line: %d\n Message: %s\n"), error.filename, error.lineNumber, error.message);
		errors.push_back(error);
		
		//avoid infinite loop on non-matches
		if(sr.end == pos) sr.end++;
		pos = sr.end;
	}
	
	//now secure the mutex and update the instance variables
	wxMutexLocker lock(m_mutex);
	
	//will this copy buildOutput?
	m_outputLastChanged++;
	m_output = buildOutput;
	
	m_errors.clear();
	for(unsigned int c = 0; c < errors.size(); c++) {
		m_errors.push_back(errors[c]);
	}
}


bool BuildErrorsManager::HasError(EditorCtrl& editor, unsigned int line) {
	wxMutexLocker lock(m_mutex);
	unsigned int id = editor.GetDocID().document_id;
	if(m_editorErrorsLastUpdated.find(id) == m_editorErrorsLastUpdated.end() || m_editorErrorsLastUpdated[id] < m_outputLastChanged) {
		ProcessEditorErrors(editor);
	}
	m_editorErrorsLastUpdated[id] = m_outputLastChanged;
	return m_editorErrors[id].find(line) !=  m_editorErrors[id].end();
}

void BuildErrorsManager::ProcessEditorErrors(EditorCtrl& editor) {
	//if i set m_errors = errors first, will that copy errors into the map?
	//can i just update errors and m_errors will be auto updated?
	map<int, BuildError*> errors;
	wxFileName filename = editor.GetFilePath();
	unsigned int id = editor.GetDocID().document_id;
	
	for(unsigned int c = 0; c < m_errors.size(); c++) {
		BuildError* error = &(m_errors[c]);
		if(error->lineNumber > -1 && error->Matches(filename)) {
			errors[error->lineNumber] = error;
		}
	}
	
	m_editorErrors[id] = errors;
}
