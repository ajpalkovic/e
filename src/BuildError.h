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

#ifndef __BUILDERROR_H__
#define __BUILDERROR_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/file.h>
#include <wx/filename.h>


#include "Interval.h"
#include <map>

class BuildRegex;
class wxFileName;

class BuildError {
public:
	BuildError(wxString& output, std::map<unsigned int, interval>& matches, BuildRegex& regex);
	bool Matches(wxFileName& file);
	int lineNumber, columnNumber;
	wxString filename, message;
	bool isError;
};

#endif