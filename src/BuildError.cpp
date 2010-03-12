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
#include <wx/file.h>
#include <wx/filename.h>


#include "Interval.h"
#include <map>

#include "BuildError.h"
#include "BuildSettings.h"

BuildError::BuildError(wxString& output, std::map<unsigned int, interval>& matches, BuildRegex& regex) {
	if(regex.lineMatch > 0) {
		interval i = matches[regex.lineMatch];
		long num;
		output.SubString(i.start, i.end-1).ToLong(&num);
		lineNumber = (unsigned int) num;
	} else {
		lineNumber = -1;
	}
	
	if(regex.columnMatch > 0) {
		interval i = matches[regex.columnMatch];
		long num;
		output.SubString(i.start, i.end-1).ToLong(&num);
		columnNumber = (unsigned int) num;
	} else {
		columnNumber = -1;
	}
	
	if(regex.filenameMatch > 0) {
		interval i = matches[regex.filenameMatch];
		filename = output.SubString(i.start, i.end-1);
	}
	
	if(regex.messageMatch > 0) {
		interval i = matches[regex.messageMatch];
		message = output.SubString(i.start, i.end-1);
	}
	
	if(regex.errorMatch > 0) {
		interval i = matches[regex.errorMatch];
		isError = output.SubString(i.start, i.end-1).Contains(wxT("error"));
	}
}

bool BuildError::Matches(wxFileName& file) {
	return file.SameAs(filename);
}