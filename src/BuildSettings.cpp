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

#include "BuildSettings.h"

#include "jsonreader.h"
#include "jsonwriter.h"

//TODO: should this stuff be passed by reference?
BuildSettings::BuildSettings(wxString command) : command(command) {
	for(unsigned int c = 0; c < command.size(); c++) {
		commandVector.push_back(command[c]);
	}
}

BuildSettings::BuildSettings(wxJSONValue& root) {
	if(root.HasMember(wxT("name"))) name = root.ItemAt(wxT("name")).AsString();

	if(root.HasMember(wxT("command"))) command = root.ItemAt(wxT("command")).AsString();
	for(unsigned int c = 0; c < command.size(); c++) {
		commandVector.push_back(command[c]);
	}

	if(root.HasMember(wxT("regexes"))) {
		wxJSONValue regexesJson = root.ItemAt(wxT("regexes"));
		for(int c = 0; c < regexesJson.Size(); c++) {
			regexes.push_back(BuildRegex(regexesJson.ItemAt(c)));
		}
	}
}

void BuildSettings::AddRegex(wxString regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch, bool incrementLineNumbers) {
	regexes.push_back(BuildRegex(regex, filenameMatch, lineMatch, columnMatch, messageMatch, errorMatch, incrementLineNumbers));
}

/**
 * {command: 'command', name: 'name', regexes: [{regex: 'regex', filenameMatch: #, lineMath: #, columnMatch: #, messageMatch: #, errorMatch: #, incrementLineNumbers: true}, ...]}
 */
wxJSONValue BuildSettings::ToJson() {
	wxJSONValue root;
	root[wxT("name")] = name;
	root[wxT("command")] = command;
	for(unsigned int c = 0; c < regexes.size(); c++) {
		root[wxT("regexes")].Append(regexes[c].ToJson());
	}

	return root;
}
	
BuildRegex::BuildRegex(wxString& regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch, bool incrementLineNumbers) :
	regex(regex), incrementLineNumbers(incrementLineNumbers), 
	filenameMatch(filenameMatch), lineMatch(lineMatch), columnMatch(columnMatch), messageMatch(messageMatch), errorMatch(errorMatch) {
}

BuildRegex::BuildRegex(wxJSONValue& root) {
	if(root.HasMember(wxT("regex"))) regex = root.ItemAt(wxT("regex")).AsString();
	
	if(root.HasMember(wxT("filenameMatch"))) filenameMatch = root.ItemAt(wxT("filenameMatch")).AsInt();
	else filenameMatch = -1;

	if(root.HasMember(wxT("lineMatch"))) lineMatch = root.ItemAt(wxT("lineMatch")).AsInt();
	else lineMatch = -1;

	if(root.HasMember(wxT("columnMatch"))) columnMatch = root.ItemAt(wxT("columnMatch")).AsInt();
	else columnMatch = -1;

	if(root.HasMember(wxT("messageMatch"))) messageMatch = root.ItemAt(wxT("messageMatch")).AsInt();
	else messageMatch = -1;

	if(root.HasMember(wxT("errorMatch"))) errorMatch = root.ItemAt(wxT("errorMatch")).AsInt();
	else errorMatch = -1;
	
	if(root.HasMember(wxT("incrementLineNumbers"))) incrementLineNumbers = root.ItemAt(wxT("incrementLineNumbers")).AsBool();
	else incrementLineNumbers = false;
}

wxJSONValue BuildRegex::ToJson() {
	wxJSONValue root;
	root[wxT("regex")] = regex;
	root[wxT("filenameMatch")] = filenameMatch;
	root[wxT("lineMatch")] = lineMatch;
	root[wxT("columnMatch")] = columnMatch;
	root[wxT("messageMatch")] = messageMatch;
	root[wxT("errorMatch")] = errorMatch;
	root[wxT("incrementLineNumbers")] = incrementLineNumbers;
	
	return root;
}