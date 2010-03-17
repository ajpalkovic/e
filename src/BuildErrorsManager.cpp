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

#include <wx/wfstream.h>
#include <wx/regex.h>
#include "Strings.h"
#include "jsonreader.h"
#include "jsonwriter.h"

using namespace std;

/**
 * This is the core class for the build system.
 * All of the data is stored here, and all the different components just call methods to access it from here.
 * So, this is the only class that really needs a mutex.
 */
BuildErrorsManager::BuildErrorsManager() : 
  m_outputLastChanged(0), m_needBuild(true) {
	m_buildThread = new BuildThread(this);
	
	LoadSettings();
	if(m_settings.size() == 0) {
		m_settings.push_back(BuildSettings());
	}
	m_currentSettings = &(m_settings[0]);
}

BuildErrorsManager::~BuildErrorsManager() {
	m_buildThread->Kill();
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

void BuildErrorsManager::LoadSettings() {
	wxMutexLocker lock(m_mutex);

	wxString path = eGetSettings().GetPath();
	//remove e.cfg
	path = path.Truncate(path.size()-5);
	m_path = path + wxT("e_build_settings.cfg");

	if(!wxFileExists(m_path)) return;
	wxFileInputStream fstream(m_path);
	if(!fstream.IsOk()) {
		wxLogDebug(wxT("Could not open build errors settings file."));
		return;
	}
 
	// Parse the JSON contents
	wxJSONReader reader;
	wxJSONValue jsonRoot;
	const int numErrors = reader.Parse(fstream, &jsonRoot);
	if(numErrors > 0) {
		// if there are errors in the JSON document, print the errors
		const wxArrayString& errors = reader.GetErrors();
		wxString msg = _("Invalid JSON in build errors settings:\n\n") + wxJoin(errors, wxT('\n'), '\0');
		wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
		return;
	}

	for(int c = 0; c < jsonRoot.Size(); c++) {
		m_settings.push_back(BuildSettings(jsonRoot.ItemAt(c)));
	}
}

void BuildErrorsManager::SaveSettings() {
	wxASSERT(!m_path.empty());
 
	// Open or create the settings file
	wxFileOutputStream fstream(m_path);
	if(!fstream.IsOk()) {
		wxLogDebug(wxT("Could not open build errors settings file."));
		return;
	}

	wxJSONValue root;
	for(unsigned int c = 0; c < m_settings.size(); c++) {
		root.Append(m_settings[c].ToJson());
	}
	
	wxJSONWriter writer(wxJSONWRITER_STYLED);
	writer.Write(root, fstream);
}

void BuildErrorsManager::BuildSettingsChanged() {
	wxMutexLocker lock(m_mutex);
	m_needBuild = true;
}

/**
 * These two methods are used by the thread to access the env and settings variables.
 * Rather than worrying about on-access mutexes, I instead just make a copy of the variable and pass that to the thread.
 * This lets the thread do whatever it wants to the variable, without worrying about race conditions.
 * I'm not sure if the destructor in wxMutexLocker will be called before or after the a copy of the variable is placed on the stack.
 * To compensate, I just create a local copy first, and then return that local copy.
 */
BuildSettings BuildErrorsManager::GetBuildSettings() {
	wxMutexLocker lock(m_mutex);
	BuildSettings settings = *m_currentSettings;
	return settings;
}

cxEnv BuildErrorsManager::GetEnv() {
	wxMutexLocker lock(m_mutex);
	cxEnv env = m_env;
	return env;
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
	bool ret = m_needBuild; //m_settingsSet && 
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
	
	//now secure the mutex and copy the local variables over to the instance variables
	wxMutexLocker lock(m_mutex);
	
	m_outputLastChanged++;
	m_output = buildOutput;
	
	m_errors = errors;
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
	wxFileName filename = editor.GetFilePath();
	unsigned int id = editor.GetDocID().document_id;
	
	m_editorErrors[id] = map<int, BuildErrorLine>();

	for(unsigned int c = 0; c < m_errors.size(); c++) {
		BuildError* error = &(m_errors[c]);
		if(error->lineNumber > -1 && error->Matches(filename)) {
			m_editorErrors[id][error->lineNumber].AddError(error);
		}
	}
}
