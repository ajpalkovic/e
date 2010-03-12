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

#ifndef __BUILDSETTINGS_H__
#define __BUILDSETTINGS_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>

class BuildRegex {
public:
	BuildRegex() {}
	BuildRegex(wxString& regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch);
	wxString regex;
	const char* regexArray;
	int filenameMatch, lineMatch, columnMatch, messageMatch, errorMatch;
};

class BuildSettings {
public:
	BuildSettings() {}
	BuildSettings(wxString command);
	void SetErrorRegex(wxString regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch);
	void SetWarningRegex(wxString regex, int filenameMatch, int lineMatch, int columnMatch, int messageMatch, int errorMatch);
	
	wxString command;
	std::vector<char> commandVector;
	BuildRegex error, warning;
};

#endif