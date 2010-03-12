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

BuildSettings::BuildSettings(wxString command) : command(command) {
	for(unsigned int c = 0; c < command.size(); c++) {
		commandVector.push_back(command[c]);
	}
}

void BuildSettings::SetErrorRegex(wxString regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch) {
	error = BuildRegex(regex, filenameMatch, lineMatch, columnMatch, messageMatch, errorMatch);
}

void BuildSettings::SetWarningRegex(wxString regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch) {
	warning = BuildRegex(regex, filenameMatch, lineMatch, columnMatch, messageMatch, errorMatch);
}
	
BuildRegex::BuildRegex(wxString& regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch) :
regex(regex), regexArray((const char*)regex.c_str()), filenameMatch(filenameMatch), lineMatch(lineMatch), columnMatch(columnMatch), messageMatch(messageMatch), errorMatch(errorMatch) {}