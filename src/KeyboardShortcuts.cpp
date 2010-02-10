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
 
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>
#include <map>
#include <algorithm>

#include <wx/wfstream.h>
#include <wx/regex.h>
#include "Strings.h"
#include "jsonreader.h"
#include "jsonwriter.h"

#include "KeyboardShortcuts.h"

using namespace std;


KeyboardShortcuts::KeyboardShortcuts() {
	selectKey = 8;
	verticalSelectKey = 2;
	multiSelectKey = 1;
}

/** File Layout:
	shortcuts: [
		{
			name: "shortcut name",
			menuText: "",
			allowSelection: true,
			allowVerticalSelection: true,
			bindings: [
				{
					code: 0,
					primary: true,
					modifiers: [ctrl, alt, shift, meta, windows]
				},
				...
			]
		},
		...
	];
*/
void KeyboardShortcuts::Init(wxString path) {
	LoadCustomShortcuts(path);
	LoadDefaultShortcuts();
	MergeShortcuts();
	SetupShortcutIntMappings();
}

void KeyboardShortcuts::LoadDefaultShortcuts() {
	RegisterDefaultShortcutAndBinding(wxT("New File"), wxT("&New"), wxT("Ctrl+N"));
	RegisterDefaultShortcutAndBinding(wxT("New Tab"), wxT("&New Tab"), wxT("Ctrl+N"));
	RegisterDefaultShortcutAndBinding(wxT("Open File"), wxT("&Open..."), wxT("Ctrl+O"));
	RegisterDefaultShortcut(wxT("Compare..."), wxT("&Compare..."));
	RegisterDefaultShortcut(wxT("Open New Window"), wxT("New Window"));
	RegisterDefaultShortcutAndBinding(wxT("Save File"), wxT("&Save"), wxT("Ctrl+S"));
	RegisterDefaultShortcutAndBinding(wxT("Save File as"), wxT("Save &As..."), wxT("Ctrl+Shift+S"));
	RegisterDefaultShortcutAndBinding(wxT("Save All Files"), wxT("Save A&ll"), wxT("Ctrl+Shift+A"));
	RegisterDefaultShortcutAndBinding(wxT("Make Milestone of current revision"), wxT("&Make Milestone..."), wxT("Ctrl+M"));
	RegisterDefaultShortcut(wxT("Open Dir as Project"), wxT("Open &Dir as Project..."));
	RegisterDefaultShortcut(wxT("Open Remote Folder"), wxT("Open Remote Folder (&FTP)..."));
	RegisterDefaultShortcut(wxT("Close Project"), wxT("Close Pro&ject"));
	RegisterDefaultShortcut(wxT("Save format"), wxT("Sa&ve format"));
	RegisterDefaultShortcut(wxT("Import"), wxT("&Import"));
	RegisterDefaultShortcut(wxT("Open recent Files"), wxT("&Recent Files"));
	RegisterDefaultShortcut(wxT("Open recent Projects"), wxT("&Recent Projects"));
	RegisterDefaultShortcut(wxT("Page setup"), wxT("Page Set&up..."));
	RegisterDefaultShortcut(wxT("Print"), wxT("&Print..."));
	RegisterDefaultShortcutAndBinding(wxT("Close Tab"), wxT("&Close Tab"), wxT("Ctrl+W"));
	RegisterDefaultShortcut(wxT("Close all Tabs"), wxT("Close all &Tabs"));
	RegisterDefaultShortcutAndBinding(wxT("Close other Tabs"), wxT("Clos&e other Tabs"), wxT("Ctrl+Alt+W"));
	RegisterDefaultShortcut(wxT("Close Window"), wxT("Close &Window"));
	RegisterDefaultShortcut(wxT("Exit"), wxT("E&xit"));
	RegisterDefaultShortcutAndBinding(wxT("Undo"), wxT("&Undo"), wxT("Ctrl+Z"));
	RegisterDefaultShortcutAndBinding(wxT("Redo"), wxT("&Redo"), wxT("Ctrl+Y"));
	RegisterDefaultShortcutAndBinding(wxT("Cut"), wxT("&Cut"), wxT("Ctrl+X"));
	RegisterDefaultShortcutAndBinding(wxT("Copy"), wxT("&Copy"), wxT("Ctrl+C"));
	RegisterDefaultShortcutAndBinding(wxT("Paste"), wxT("&Paste"), wxT("Ctrl+V"));
	RegisterDefaultShortcutAndBinding(wxT("Select All"), wxT("&All"), wxT("Ctrl+A"));
	RegisterDefaultShortcutAndBinding(wxT("Select Word"), wxT("&Word"), wxT("Ctrl+Shift+W"));
	RegisterDefaultShortcutAndBinding(wxT("Select Line"), wxT("&Line"), wxT("Ctrl+Shift+L"));
	RegisterDefaultShortcutAndBinding(wxT("Select Current Scope"), wxT("&Current Scope"), wxT("Ctrl+Shift+Space"));
	RegisterDefaultShortcutAndBinding(wxT("Select Current Fold"), wxT("Current &Fold"), wxT("Shift+F1"));
	RegisterDefaultShortcutAndBinding(wxT("Select Parent Tag Content"), wxT("Tag &Content"), wxT("Ctrl+,"));
	RegisterDefaultShortcut(wxT("Select"), wxT("&Select"));
	RegisterDefaultShortcutAndBinding(wxT("Find"), wxT("&Find"), wxT("Ctrl+F"));
	RegisterDefaultShortcutAndBinding(wxT("Find in Selection"), wxT("Find &in Selection"), wxT("Ctrl+Shift+F"));
	RegisterDefaultShortcutAndBinding(wxT("Find in Project"), wxT("Find in &Project"), wxT("Ctrl+Alt+F"));
	RegisterDefaultShortcutAndBinding(wxT("Replace"), wxT("&Replace"), wxT("Ctrl+R"));
	RegisterDefaultShortcut(wxT("Find and Replace"), wxT("&Find and Replace"));
	RegisterDefaultShortcut(wxT("Syntax"), wxT("S&yntax"));
	RegisterDefaultShortcut(wxT("Edit Theme"), wxT("Edit &Theme..."));
	RegisterDefaultShortcut(wxT("Edit Settings"), wxT("S&ettings..."));
	RegisterDefaultShortcutAndBinding(wxT("Show Project Pane"), wxT("&Project Pane"), wxT("Ctrl+P"));
	RegisterDefaultShortcutAndBinding(wxT("Show Symbol List"), wxT("&Symbol List"), wxT("Ctrl+Alt+L"));
	RegisterDefaultShortcutAndBinding(wxT("Show Snippet List"), wxT("&Snippet List"), wxT("Ctrl+Alt+S"));
	RegisterDefaultShortcutAndBinding(wxT("Show Revision History"), wxT("&Revision History"), wxT("F6"));
	RegisterDefaultShortcutAndBinding(wxT("Show Undo History"), wxT("&Undo History"), wxT("F7"));
	RegisterDefaultShortcutAndBinding(wxT("Show Command Output"), wxT("&Command Output"), wxT("F12"));
	RegisterDefaultShortcutAndBinding(wxT("Show Web Preview"), wxT("Web Pre&view"), wxT("Ctrl+Alt+P"));
	RegisterDefaultShortcut(wxT("Show Line Numbers"), wxT("&Line Numbers"));
	RegisterDefaultShortcut(wxT("Show Indent Guides"), wxT("&Indent Guides"));
	RegisterDefaultShortcut(wxT("Disable Word Wrap"), wxT("&None"));
	RegisterDefaultShortcut(wxT("Enable Word Wrap"), wxT("Nor&mal"));
	RegisterDefaultShortcut(wxT("Smart Word Wrap"), wxT("&Smart"));
	RegisterDefaultShortcut(wxT("Word Wrap"), wxT("&Word Wrap"));
	RegisterDefaultShortcut(wxT("Show Statusbar"), wxT("Show Status&bar"));
	RegisterDefaultShortcutAndBinding(wxT("Toggle Fold"), wxT("&Toggle Fold"), wxT("F1"));
	RegisterDefaultShortcutAndBinding(wxT("Fold All"), wxT("Fold &All"), wxT("Ctrl+F1"));
	RegisterDefaultShortcutAndBinding(wxT("Fold Others"), wxT("Fold &Others"), wxT("Alt+F1"));
	RegisterDefaultShortcutAndBinding(wxT("Unfold &All"), wxT("&Unfold All"), wxT("Ctrl+Alt+F1"));
	RegisterDefaultShortcutAndBinding(wxT("Convert to Uppercase"), wxT("to &Uppercase"), wxT("Ctrl+U"));
	RegisterDefaultShortcutAndBinding(wxT("Convert to Lowercase"), wxT("to &Lowercase"), wxT("Ctrl+Shift+U"));
	RegisterDefaultShortcutAndBinding(wxT("Convert to Titlecase"), wxT("to &Titlecase"), wxT("Ctrl+Alt+U"));
	RegisterDefaultShortcutAndBinding(wxT("Convert to Opposite Case"), wxT("to &Opposite Case"), wxT("Alt+G"));
	RegisterDefaultShortcutAndBinding(wxT("Transpose text"), wxT("T&ranspose"), wxT("Ctrl+T"));
	RegisterDefaultShortcut(wxT("Convert Selections"), wxT("&Convert"));
	RegisterDefaultShortcut(wxT("Shift Left"), wxT("Shift &Left"));
	RegisterDefaultShortcut(wxT("Shift Right"), wxT("Shift &Right"));
	RegisterDefaultShortcut(wxT("Tabs to Spaces"), wxT("&Tabs to Spaces"));
	RegisterDefaultShortcut(wxT("Spaces to Tabs"), wxT("&Spaces to Tabs"));
	RegisterDefaultShortcutAndBinding(wxT("Complete Word"), wxT("Complete &Word"), wxT("Escape"));
	RegisterDefaultShortcutAndBinding(wxT("Filter Through Command..."), wxT("&Filter Through Command..."), wxT("Ctrl+H"));
	RegisterDefaultShortcutAndBinding(wxT("Run current line/selection"), wxT("&Run current line/selection"), wxT("Ctrl+Alt+R"));
	RegisterDefaultShortcutAndBinding(wxT("Add Bookmark"), wxT("Add &Bookmark"), wxT("Ctrl+F2"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Next Bookmark"), wxT("&Next Bookmark"), wxT("F2"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Previous Bookmark"), wxT("&Previous Bookmark"), wxT("Shift+F2"));
	RegisterDefaultShortcutAndBinding(wxT("Remove All Bookmarks"), wxT("&Remove All Bookmarks"), wxT("Ctrl+Shift+F2"));
	RegisterDefaultShortcutAndBinding(wxT("Next Tab"), wxT("N&ext Tab"), wxT("Ctrl+Tab"));
	RegisterDefaultShortcutAndBinding(wxT("Previous Tab"), wxT("Pre&vious Tab"), wxT("Ctrl+Shift+Tab"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Tab..."), wxT("Go to &Tab..."), wxT("Ctrl+0"));
	RegisterDefaultShortcutAndBinding(wxT("Last used Tab"), wxT("L&ast used Tab"), wxT("Ctrl+Alt+0"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Header/Source"), wxT("Go to &Header/Source"), wxT("Ctrl+Alt+Up"));
	RegisterDefaultShortcutAndBinding(wxT("Go to File..."), wxT("Go to &File..."), wxT("Ctrl+Shift+T"));
	RegisterDefaultShortcutAndBinding(wxT("Show Symbol List"), wxT("Go to &Symbol..."), wxT("Ctrl+L"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Matching Bracket"), wxT("Go to &Matching Bracket"), wxT("Ctrl+B"));
	RegisterDefaultShortcutAndBinding(wxT("Go to Line..."), wxT("Go to &Line..."), wxT("Ctrl+G"));
	RegisterDefaultShortcut(wxT("Buy"), wxT("&Buy"));
	RegisterDefaultShortcut(wxT("Enter Registration Code"), wxT("&Enter Registration Code"));
	RegisterDefaultShortcut(wxT("Help Contents"), wxT("&Help Contents"));
	RegisterDefaultShortcut(wxT("Go to Support Forum"), wxT("&Go to Support &Forum"));
	RegisterDefaultShortcut(wxT("Goto Website"), wxT("Go to &Website"));
	RegisterDefaultShortcut(wxT("About"), wxT("&About e"));
	
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 0"), wxT("Open &Tab 0"), wxT("Ctrl+0"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 1"), wxT("Open &Tab 1"), wxT("Ctrl+1"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 2"), wxT("Open &Tab 2"), wxT("Ctrl+2"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 3"), wxT("Open &Tab 3"), wxT("Ctrl+3"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 4"), wxT("Open &Tab 4"), wxT("Ctrl+4"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 5"), wxT("Open &Tab 5"), wxT("Ctrl+5"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 6"), wxT("Open &Tab 6"), wxT("Ctrl+6"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 7"), wxT("Open &Tab 7"), wxT("Ctrl+7"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 8"), wxT("Open &Tab 8"), wxT("Ctrl+8"));
	RegisterDefaultShortcutAndBinding(wxT("Open Tab 9"), wxT("Open &Tab 9"), wxT("Ctrl+9"));
	
	RegisterDefaultShortcutAndBinding(wxT("Show Bundle Editor"), wxT("Show Bundle &Editor"), wxT("Ctrl+Shift+B"));
	RegisterDefaultShortcut(wxT("Show Bundle Manager"), wxT("&Manage Bundles"));
	RegisterDefaultShortcut(wxT("Enable Debug Mode"), wxT("Enable &Debug Mode"));
	RegisterDefaultShortcut(wxT("Reload Bundles"), wxT("&Reload Bundles"));
	RegisterDefaultShortcut(wxT("Edit Bundles"), wxT("&Edit Bundles"));
	RegisterDefaultShortcutAndBinding(wxT("Select Bundle Item..."), wxT("&Select Bundle Item..."), wxT("Ctrl+Alt+T"));
	
	RegisterDefaultShortcut(wxT("DOS/Windows (CR LF)"), wxT("&DOS/Windows (CR LF)"));
	RegisterDefaultShortcut(wxT("Unix (LF)"), wxT("&Unix (LF)"));
	RegisterDefaultShortcut(wxT("Mac (CR)"), wxT("&Mac (CR)"));
	RegisterDefaultShortcut(wxT("Always save native"), wxT("&Always save native"));
	RegisterDefaultShortcut(wxT("Byte Order Marker (BOM)"), wxT("&Byte Order Marker (BOM)"));
	
	RegisterDefaultShortcut(wxT("Copy Path to Clipboard"), wxT("Copy &Path to Clipboard"));

	//These shortcuts come from the OnChar function.
	//They are a pain, because the keycodes are sometimes ints, and sometimes constants.
	//They often need to work regardless of whether or not the user is pressing the select/alt keys
	//Some of them overlap with Menu shortcuts, so we don't want to clobber them.
	//And you can't use strings in a switch, which means each of these needs to ultimately map to a unique integer at compile time.
	RegisterDefaultOnCharShortcut(wxT("Copy"), WXK_INSERT, false, false, false, true);
	RegisterDefaultOnCharShortcut(wxT("Delete the Current Line"), 11, true, false, false, true); // ctrl-k
	RegisterDefaultOnCharShortcut(wxT("Show Scope Tip"), 16, true, false, false, true); // ctrl-p
	RegisterDefaultOnCharShortcut(wxT("Complete Word"), WXK_SPACE, false, false, false, true); // ctrl-space
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Word Left"), WXK_LEFT, true, true, true, true); // ctrl ->
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Word Right"), WXK_RIGHT, true, true, true, true); // ctrl <-
	RegisterDefaultOnCharShortcut(wxT("Move Editor Up One Line"), WXK_UP, true, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Editor Down One Line"), WXK_DOWN, true, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Editor to Top"), WXK_HOME, true, true, false, true);
	RegisterDefaultOnCharShortcut(wxT("Move Editor to Bottom"), WXK_END, true, true, false, true);
	RegisterDefaultOnCharShortcut(wxT("Close Tab"), WXK_F4, false, false, false, true);

	RegisterDefaultOnCharShortcut(wxT("Move Cursor Left"), WXK_LEFT, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Left"), WXK_NUMPAD_LEFT, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Right"), WXK_RIGHT, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Right"), WXK_NUMPAD_RIGHT, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Up"), WXK_UP, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Up"), WXK_NUMPAD_UP, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Down"), WXK_DOWN, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move Cursor Down"), WXK_NUMPAD_DOWN, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Move to Beginning of Line"), WXK_HOME, true, true, false);
	RegisterDefaultOnCharShortcut(wxT("Move to Beginning of Line"), WXK_NUMPAD_HOME, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Move to Beginning of Line"), WXK_NUMPAD_BEGIN, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Move to End of Line"), WXK_END, true, true, false);
	RegisterDefaultOnCharShortcut(wxT("Move to End of Line"), WXK_NUMPAD_END, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Pageup"), WXK_PAGEUP, true, true, false);
	RegisterDefaultOnCharShortcut(wxT("Pageup"), WXK_NUMPAD_PAGEUP, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Pagedown"), WXK_PAGEDOWN, true, true, false);
	RegisterDefaultOnCharShortcut(wxT("Pagedown"), WXK_NUMPAD_PAGEDOWN, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Paste"), WXK_INSERT, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Paste"), WXK_NUMPAD_INSERT, false, true, false);
	RegisterDefaultOnCharShortcut(wxT("Delete"), WXK_DELETE, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Delete"), WXK_NUMPAD_DELETE, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Delete"), WXK_DELETE, true, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Delete"), WXK_NUMPAD_DELETE, false, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Backspace"), WXK_BACK, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Newline"), WXK_RETURN, true, true, true);
	RegisterDefaultOnCharShortcut(wxT("Newline"), WXK_NUMPAD_ENTER, false, true, true);
	RegisterDefaultOnCharShortcut(wxT("Tab"), WXK_TAB, true, false, false);
	RegisterDefaultOnCharShortcut(wxT("Tab"), WXK_NUMPAD_TAB, false, false, false);
	RegisterDefaultOnCharShortcut(wxT("Escape"), WXK_ESCAPE, true, true, true);
}

void KeyboardShortcuts::RegisterDefaultOnCharShortcut(wxString name, int code, bool primary, bool allowSelection, bool allowVerticalSelection, bool ctrl, bool alt, bool shift, bool meta, bool windows) {
    CreateEventType(m_defaultShortcuts, name, name, allowSelection, allowVerticalSelection);
    CreateKeyBinding(m_shortcuts, name, primary, code, ctrl, alt, shift, meta, windows);
}
void KeyboardShortcuts::RegisterDefaultShortcut(wxString name, wxString menuText, bool allowSelection, bool allowVerticalSelection) {
	CreateEventType(m_defaultShortcuts, name, menuText, allowSelection, allowVerticalSelection);
}

void KeyboardShortcuts::RegisterDefaultShortcutAndBinding(wxString name, wxString menuText, wxString binding, bool allowSelection, bool allowVerticalSelection) {
	//If the event doesn't exist, create it
	CreateEventType(m_defaultShortcuts, name, menuText, allowSelection, allowVerticalSelection);

	int code, start;
	bool ctrl, alt, shift, meta, windows;
	
	for(start = binding.size()-1; start >= 0; start--) {
		if(binding.GetChar(start) == '+') {
			if(start > 0 && binding.GetChar(start-1) == '+') {
				continue;
			}
			break;
		}
	}

	binding = binding.Lower();
	ctrl = binding.Contains(wxT("ctrl"));
	alt = binding.Contains(wxT("alt"));
	shift = binding.Contains(wxT("shift"));
	meta = binding.Contains(wxT("meta"));
	windows = binding.Contains(wxT("windows"));

	binding = binding.SubString(start+1, binding.size());
	code = TranslateStringToCode(binding);

	CreateKeyBinding(m_defaultShortcuts, name, true, code, ctrl, alt, shift, meta, windows);

	wxString str = GetEventMenuText(name);
}

int KeyboardShortcuts::TranslateStringToCode(wxString binding) {
	if(binding.size() == 1) {
		return wxToupper(binding.GetChar(0));
	}
	
	int keyCode = IsNumberedAccelKey(binding, wxTRANSLATE("F"), WXK_F1, 1, 12);
	if(!keyCode) {
		for(int n = 0; n < WXSIZEOF(wxKeyNames); n++) {
			const wxKeyName& kn = wxKeyNames[n];
			if(CompareAccelString(binding, kn.name)) {
				keyCode = kn.code;
				break;
			}
		}
	}
	
	if(!keyCode)
		keyCode = IsNumberedAccelKey(binding, wxTRANSLATE("KP_"), WXK_NUMPAD0, 0, 9);
		
	if(!keyCode)
		keyCode = IsNumberedAccelKey(binding, wxTRANSLATE("SPECIAL"), WXK_SPECIAL1, 1, 20);
		
	return keyCode;
}

void KeyboardShortcuts::LoadCustomShortcuts(wxString path) {
	m_path = path + wxT("e_keyboard_shortcuts.cfg");
	LoadSavedShortcuts();
}

bool ReadBoolean(wxJSONValue& json, wxString property) {
	if(!json.HasMember(property)) return false;
	wxJSONValue value = json.ItemAt(property);
	if(value.IsBool()) return value.AsBool();
	if(value.IsInt()) return value.AsInt() != 0;
	return false;
}

bool ReadBoolean(wxJSONValue& json, int property) {
	if(!json.HasMember(property)) return false;
	wxJSONValue value = json.ItemAt(property);
	if(value.IsBool()) return value.AsBool();
	if(value.IsInt()) return value.AsInt() != 0;
	return false;
}

void KeyboardShortcuts::LoadSavedShortcuts() {
	if (!wxFileExists(m_path)) return;
	wxFileInputStream fstream(m_path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open keyboard settings file."), _("File error"), wxICON_ERROR|wxOK);
		return;
	}

	// Parse the JSON contents
	wxJSONReader reader;
	wxJSONValue jsonRoot;
	const int numErrors = reader.Parse(fstream, &jsonRoot);
	if ( numErrors > 0 )  {
		// if there are errors in the JSON document, print the errors
		const wxArrayString& errors = reader.GetErrors();
		wxString msg = _("Invalid JSON in settings:\n\n") + wxJoin(errors, wxT('\n'), '\0');
		wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
		return;
	}

	if(jsonRoot.HasMember(wxT("selectKey"))) selectKey = jsonRoot.ItemAt(wxT("selectKey")).AsInt();
	if(jsonRoot.HasMember(wxT("verticalSelectKey"))) verticalSelectKey = jsonRoot.ItemAt(wxT("verticalSelectKey")).AsInt();
	if(jsonRoot.HasMember(wxT("multiSelectKey"))) multiSelectKey = jsonRoot.ItemAt(wxT("multiSelectKey")).AsInt();
	
	if (!jsonRoot.HasMember(wxT("shortcuts"))) return;
	wxJSONValue shortcutsArray = jsonRoot.ItemAt(wxT("shortcuts"));

	wxJSONValue shortcut, bindings, binding, modifiers;
	wxString name, menuText;
	bool allowSelection, allowVerticalSelection;
	int code;
	bool primary, ctrl, alt, shift, meta, windows;

	for(int c = 0; c < shortcutsArray.Size(); c++) {
		shortcut = shortcutsArray.ItemAt(c);
		if(!shortcut.HasMember(wxT("name"))) continue;
		name = shortcut.ItemAt(wxT("name")).AsString();

		if(!shortcut.HasMember(wxT("menuText"))) menuText = name;
		else menuText = shortcut.ItemAt(wxT("menuText")).AsString();

		allowSelection = ReadBoolean(shortcut, wxT("allowSelection"));
		allowVerticalSelection = ReadBoolean(shortcut, wxT("allowVerticalSelection"));

		CreateEventType(m_shortcuts, name, menuText, allowSelection, allowVerticalSelection);

		if(!shortcut.HasMember(wxT("bindings"))) continue;
		bindings = shortcut.ItemAt(wxT("bindings"));

		for(int d = 0; d < bindings.Size(); d++) {
			binding = bindings.ItemAt(d);

			if(!binding.HasMember(wxT("code"))) continue;
			code = binding.ItemAt(wxT("code")).AsInt();

			primary = ReadBoolean(binding, wxT("primary"));

			ctrl = alt = shift = meta = windows = false;
			if(binding.HasMember(wxT("modifiers"))) {
				modifiers = binding.ItemAt(wxT("modifiers"));
				ctrl = ReadBoolean(modifiers, 0);
				alt = ReadBoolean(modifiers, 1);
				shift = ReadBoolean(modifiers, 2);
				meta = ReadBoolean(modifiers, 3);
				windows = ReadBoolean(modifiers, 4);
			}

			CreateKeyBinding(m_shortcuts, name, primary, code, ctrl, alt, shift, meta, windows);
		}
	}
}

void KeyboardShortcuts::SaveShortcuts() {
	wxASSERT(!m_path.empty());
	
	// Open or create the settings file
	wxFileOutputStream fstream(m_path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open settings file."), _("File error"), wxICON_ERROR|wxOK);
		return;
	}
	
	wxJSONValue root, shortcuts;
	
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	KeyboardShortcutType* type;
	KeyboardShortcut* binding;

    int index = 0;
	for(iterator = m_defaultShortcuts.begin(); iterator != m_defaultShortcuts.end(); ++iterator) {
		type = iterator->second;
		
		shortcuts[index][wxT("name")] = type->name;
		shortcuts[index][wxT("menuText")] = type->menuText;
		shortcuts[index][wxT("allowSelection")] = type->allowSelection;
		shortcuts[index][wxT("allowVerticalSelection")] = type->allowVerticalSelection;

		for(unsigned int c = 0; c < type->shortcuts.size(); c++) {
			binding = type->shortcuts[c];
			
			shortcuts[index][wxT("bindings")][c][wxT("code")] = binding->code;
			shortcuts[index][wxT("bindings")][c][wxT("primary")] = binding->primary;
			
			shortcuts[index][wxT("bindings")][c][wxT("modifiers")].Append(binding->ctrl);
			shortcuts[index][wxT("bindings")][c][wxT("modifiers")].Append(binding->alt);
			shortcuts[index][wxT("bindings")][c][wxT("modifiers")].Append(binding->shift);
			shortcuts[index][wxT("bindings")][c][wxT("modifiers")].Append(binding->meta); 
			shortcuts[index][wxT("bindings")][c][wxT("modifiers")].Append(binding->windows);
		}
		
		index++;
	}

	root[wxT("shortcuts")] = shortcuts;
	root[wxT("selectKey")] = selectKey;
	root[wxT("verticalSelectKey")] = verticalSelectKey;
	root[wxT("multiSelectKey")] = multiSelectKey;

	wxJSONWriter writer(wxJSONWRITER_STYLED);
	writer.Write( root, fstream );
}

void KeyboardShortcuts::MergeShortcuts() {
	map<wxString, KeyboardShortcutType*>::iterator iterator, customIterator;
	KeyboardShortcutType* type;
	bool anyMerged = false;

	for(iterator = m_defaultShortcuts.begin(); iterator != m_defaultShortcuts.end(); ++iterator) {
		type = iterator->second;

		//If this key event, ie "copy" is already defined in m_shortcuts, then there is no need to add it again.
		customIterator = m_shortcuts.find(type->name);
		if(customIterator != m_shortcuts.end()) {
			//wxLogDebug(wxT("Found custom shortcut: %s"), type->name);
			continue;
		}
		wxLogDebug(wxT("Did not find custom shortcut: %s"), type->name);

		anyMerged = true;
		m_shortcuts.insert(m_shortcuts.begin(), pair<wxString, KeyboardShortcutType*>(type->name, type));
	}

	//Create a single hashtable of all of the key codes for faster lookup when a key is pressed.
	for(iterator = m_shortcuts.begin(); iterator != m_shortcuts.end(); ++iterator) {
		type = iterator->second;
		for(unsigned int c = 0; c < type->shortcuts.size(); c++) {
			m_keys.insert(m_keys.begin(), pair<int, KeyboardShortcut*>(type->shortcuts[c]->code, type->shortcuts[c]));
		}
	}

	if(anyMerged) {
		SaveShortcuts();
	}
}

void KeyboardShortcuts::SetupShortcutIntMappings() {
	//If we ever want to be able to process keyboard events in a switch, we need to be able to map the event type to an int
	//This function defines that mapping.  There can be keyboard events that do not map to an int, but any event defined here must be listed in m_defaultShortcuts
	RegisterShortcutIntMapping(wxT("Close Tab"), KEY_EDITOR_CTRL_CLOSE_TAB);
	
	RegisterShortcutIntMapping(wxT("Copy"), KEY_EDITOR_CTRL_COPY);
	RegisterShortcutIntMapping(wxT("Paste"), KEY_EDITOR_CTRL_PASTE);
	RegisterShortcutIntMapping(wxT("Cut"), KEY_EDITOR_CTRL_CUT);
	
	RegisterShortcutIntMapping(wxT("Delete the Current Line"), KEY_EDITOR_CTRL_DELETE_CURRENT_LINE);
	RegisterShortcutIntMapping(wxT("Show Scope Tip"), KEY_EDITOR_CTRL_SHOW_SCOPE_TIP);
	RegisterShortcutIntMapping(wxT("Complete Word"), KEY_EDITOR_CTRL_COMPLETION);
	
	RegisterShortcutIntMapping(wxT("Move Cursor Word Left"), KEY_EDITOR_CTRL_CURSOR_WORD_LEFT);
	RegisterShortcutIntMapping(wxT("Move Cursor Word Right"), KEY_EDITOR_CTRL_CURSOR_WORD_RIGHT);
	
	RegisterShortcutIntMapping(wxT("Move Editor Up One Line"), KEY_EDITOR_CTRL_CURSOR_UP);
	RegisterShortcutIntMapping(wxT("Move Editor Down One Line"), KEY_EDITOR_CTRL_CURSOR_DOWN);
	
	RegisterShortcutIntMapping(wxT("Move Editor to Top"), KEY_EDITOR_CTRL_TOP);
	RegisterShortcutIntMapping(wxT("Move Editor to Bottom"), KEY_EDITOR_CTRL_BOTTOM);
	
	RegisterShortcutIntMapping(wxT("Move Cursor Left"), KEY_EDITOR_CTRL_LEFT);
	RegisterShortcutIntMapping(wxT("Move Cursor Right"), KEY_EDITOR_CTRL_RIGHT);
	RegisterShortcutIntMapping(wxT("Move Cursor Up"), KEY_EDITOR_CTRL_UP);
	RegisterShortcutIntMapping(wxT("Move Cursor Down"), KEY_EDITOR_CTRL_DOWN);
	
	RegisterShortcutIntMapping(wxT("Move to Beginning of Line"), KEY_EDITOR_CTRL_HOME);
	RegisterShortcutIntMapping(wxT("Move to End of Line"), KEY_EDITOR_CTRL_END);
	
	RegisterShortcutIntMapping(wxT("Pageup"), KEY_EDITOR_CTRL_PAGEUP);
	RegisterShortcutIntMapping(wxT("Pagedown"), KEY_EDITOR_CTRL_PAGEDOWN);
	
	RegisterShortcutIntMapping(wxT("Delete"), KEY_EDITOR_CTRL_DELETE);
	RegisterShortcutIntMapping(wxT("Backspace"), KEY_EDITOR_CTRL_BACKSPACE);
	RegisterShortcutIntMapping(wxT("Newline"), KEY_EDITOR_CTRL_NEWLINE);
	RegisterShortcutIntMapping(wxT("Tab"), KEY_EDITOR_CTRL_TAB);
	RegisterShortcutIntMapping(wxT("Escape"), KEY_EDITOR_CTRL_ESCAPE);
}

void KeyboardShortcuts::RegisterShortcutIntMapping(wxString name, int id) {
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	iterator = m_shortcuts.find(name);
	if(iterator == m_shortcuts.end()) return;

	KeyboardShortcutType* existingEvent = iterator->second;
	existingEvent->id = id;
}

//This funciton will be called once for each event type to set basic properties about the event
void KeyboardShortcuts::CreateEventType(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, wxString menuText, bool allowSelection, bool allowVerticalSelection) {
	if(shortcuts.find(name) == shortcuts.end()) {
		//this event doesn't exist, lets add it
		KeyboardShortcutType* type = new KeyboardShortcutType(name, menuText, allowSelection, allowVerticalSelection);
		shortcuts.insert(shortcuts.begin(), pair<wxString, KeyboardShortcutType*>(name, type));
	} else {
		//this event already exists.  o noz
	}
}

//This function will be called multiple times to associate a key binding with an event type
KeyboardShortcut* KeyboardShortcuts::CreateKeyBinding(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool primary, int code, bool ctrl, bool alt, bool shift, bool meta, bool windows) {
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	iterator = shortcuts.find(name);
	if(iterator == shortcuts.end()) {
		//no event exists with this name
		return NULL;
	}
	
	KeyboardShortcutType* type = iterator->second;
	KeyboardShortcut* event = new KeyboardShortcut(type, code, ctrl, shift, alt, meta, windows);
	if(primary) type->AddPrimaryShortcut(event);
	else type->AddShortcut(event);

	return event;
}

int KeyboardShortcuts::GetEventType(wxKeyEvent& event) {
	multimap<int, KeyboardShortcut*>::iterator iterator;
	pair<multimap<int, KeyboardShortcut*>::iterator, multimap<int, KeyboardShortcut*>::iterator> matches;
	
	matches = m_keys.equal_range(event.GetKeyCode());
	int eventCode = (event.ControlDown() ? 1 : 0) | (event.AltDown() ? 2 : 0) | (event.MetaDown() ? 4 : 0) | (event.ShiftDown() ? 8 : 0);
	//mask is gonna be 00110 then we invert it to 11001.  now we and it with each event which removes the 'optional' keys.
	//for instance, if ctrl and shift are pressed: 00011, but only ctrl is required, we do: 00011 & 11001 = 00001
	int mask = selectKey | verticalSelectKey;
	mask = ~mask;

	//If the code is 11000 and the select/alt keys are shift and alt, this would be:
	//11000 ^ 01000 = 10000 ^ 00010 = 10000
	int eventCodeWithoutSelection = eventCode & mask;
		
	int keyCode = 0, keyCodeWithoutSelection;;
	
	KeyboardShortcut* cur;
	
	//Multiple values might be mapped to the C key for instance, but in theory, only one is mapped to ctrl-c.
	//If there are multiple values, only the first is returned.
	//We need to check now if keys like ctrl and shift are pressed as well.
	//However, the user could also be selecting text.  Some of the existing code currently checks that inside of later method calls.  
	//This means we need to ignore keys like shift and alt if the keyboard event type accepts selection input
	for(iterator = matches.first; iterator != matches.second; ++iterator) {
		cur = iterator->second;
		keyCode = (cur->ctrl ? 1 : 0) | (cur->alt ? 2 : 0) | (cur->meta ? 4 : 0) | (cur->shift ? 8 : 0) | (cur->windows ? 16 : 0);
		keyCodeWithoutSelection = keyCode & mask;
		
		if(keyCode == eventCode || (cur->type->allowSelection && keyCodeWithoutSelection == eventCodeWithoutSelection)) {
			return cur->type->id;
		}
		
	}
	
	return KEY_DEFAULT;
}

bool KeyboardShortcuts::IsSelectDown(wxKeyEvent& event) {
	return IsSpecialKeyDown(selectKey, event);
}

bool KeyboardShortcuts::IsMultiSelectDown(wxKeyEvent& event) {
	return IsSpecialKeyDown(multiSelectKey, event);
}

bool KeyboardShortcuts::IsVerticalSelectDown(wxKeyEvent& event) {
	return IsSpecialKeyDown(verticalSelectKey, event);
}

bool KeyboardShortcuts::IsVerticalSelectCurrentlyDown(wxKeyEvent& event) {
	int code = -1;
	switch(verticalSelectKey) {
		case 1: code = WXK_CONTROL; break;
		case 2: code = WXK_ALT; break;
		//case 4: code = WXK_META;
		case 8: code = WXK_SHIFT; break;
		//case 16: return wxGetKeyState(WXK_WINDOWS_LEFT) || wxGetKeyState(WXK_WINDOWS_RIGHT);
	}

	return event.GetKeyCode() == code;
}

bool KeyboardShortcuts::IsSpecialKeyDown(int key, wxKeyEvent& event) {
	switch(key) {
		case 1: return event.ControlDown();
		case 2: return event.AltDown();
		case 4: return event.MetaDown();
		case 8: return event.ShiftDown();
	}

	//shenanigans
	return false;
}

bool KeyboardShortcuts::IsSelectDown(wxMouseEvent& event) {
	return IsSpecialKeyDown(selectKey, event);
}

bool KeyboardShortcuts::IsMultiSelectDown(wxMouseEvent& event) {
	return IsSpecialKeyDown(multiSelectKey, event);
}

bool KeyboardShortcuts::IsVerticalSelectDown(wxMouseEvent& event) {
	return IsSpecialKeyDown(verticalSelectKey, event);
}

bool KeyboardShortcuts::IsSpecialKeyDown(int key, wxMouseEvent& event) {
	switch(key) {
		case 1: return event.ControlDown();
		case 2: return event.AltDown();
		case 4: return event.MetaDown();
		case 8: return event.ShiftDown();
	}

	//shenanigans
	return false;
}

bool KeyboardShortcuts::IsSelectDown() {
	return IsSpecialKeyDown(selectKey);
}

bool KeyboardShortcuts::IsMultiSelectDown() {
	return IsSpecialKeyDown(multiSelectKey);
}

bool KeyboardShortcuts::IsVerticalSelectDown() {
	return IsSpecialKeyDown(verticalSelectKey);
}

bool KeyboardShortcuts::IsSpecialKeyDown(int key) {
	switch(key) {
		case 1: return wxGetKeyState(WXK_CONTROL);
		case 2: return wxGetKeyState(WXK_ALT);
		case 4: return wxGetKeyState(WXK_SHIFT);
		//case 8: return wxGetKeyState(WXK_META);
		case 16: return wxGetKeyState(WXK_WINDOWS_LEFT) || wxGetKeyState(WXK_WINDOWS_RIGHT);
	}

	//shenanigans
	return false;
}

wxString KeyboardShortcuts::GetEventKeyBinding(wxString eventName) {
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	iterator = m_shortcuts.find(eventName);
	if(iterator == m_shortcuts.end()) return wxT("");

	KeyboardShortcutType* type = iterator->second;
	if(!type->hasPrimary) return wxT("");
	KeyboardShortcut* primary = type->primaryShortcut;
	
	wxString ret = wxT("");
	if(primary->ctrl) ret += wxT("Ctrl+");
	if(primary->alt) ret += wxT("Alt+");
	if(primary->shift) ret += wxT("Shift+");
	if(primary->meta) ret += wxT("Meta+");
	if(primary->windows) ret += wxT("Windows+");
	ret += TranslateCodeToString(primary->code);
	
	return ret;
}
wxString KeyboardShortcuts::GetEventMenuText(wxString eventName) {
	wxString binding = GetEventKeyBinding(eventName);
	if(binding.IsEmpty()) return eventName;
	//TODO: Put in the ampersand here
	return eventName+wxT("\t")+binding;
}

wxString KeyboardShortcuts::TranslateCodeToString(int code) {
    switch(code) {
        case WXK_BACK: return wxT("Back");
        case WXK_TAB: return wxT("Tab");
        case WXK_RETURN: return wxT("Return");
        case WXK_ESCAPE: return wxT("Escape");
        case WXK_SPACE: return wxT("Space");
        case WXK_DELETE: return wxT("Delete");
        case WXK_START: return wxT("Start");
        case WXK_LBUTTON: return wxT("Lbutton");
        case WXK_RBUTTON: return wxT("Rbutton");
        case WXK_CANCEL: return wxT("Cancel");
        case WXK_MBUTTON: return wxT("Mbutton");
        case WXK_CLEAR: return wxT("Clear");
        case WXK_SHIFT: return wxT("Shift");
        case WXK_ALT: return wxT("Alt");
        case WXK_CONTROL: return wxT("Control");
        case WXK_MENU: return wxT("Menu");
        case WXK_PAUSE: return wxT("Pause");
        case WXK_CAPITAL: return wxT("Capital");
        case WXK_END: return wxT("End");
        case WXK_HOME: return wxT("Home");
        case WXK_LEFT: return wxT("Left");
        case WXK_UP: return wxT("Up");
        case WXK_RIGHT: return wxT("Right");
        case WXK_DOWN: return wxT("Down");
        case WXK_SELECT: return wxT("Select");
        case WXK_PRINT: return wxT("Print");
        case WXK_EXECUTE: return wxT("Execute");
        case WXK_SNAPSHOT: return wxT("Snapshot");
        case WXK_INSERT: return wxT("Insert");
        case WXK_HELP: return wxT("Help");
        case WXK_NUMPAD0: return wxT("Numpad0");
        case WXK_NUMPAD1: return wxT("Numpad1");
        case WXK_NUMPAD2: return wxT("Numpad2");
        case WXK_NUMPAD3: return wxT("Numpad3");
        case WXK_NUMPAD4: return wxT("Numpad4");
        case WXK_NUMPAD5: return wxT("Numpad5");
        case WXK_NUMPAD6: return wxT("Numpad6");
        case WXK_NUMPAD7: return wxT("Numpad7");
        case WXK_NUMPAD8: return wxT("Numpad8");
        case WXK_NUMPAD9: return wxT("Numpad9");
        case WXK_MULTIPLY: return wxT("Multiply");
        case WXK_ADD: return wxT("Add");
        case WXK_SEPARATOR: return wxT("Separator");
        case WXK_SUBTRACT: return wxT("Subtract");
        case WXK_DECIMAL: return wxT("Decimal");
        case WXK_DIVIDE: return wxT("Divide");
        case WXK_F1: return wxT("F1");
        case WXK_F2: return wxT("F2");
        case WXK_F3: return wxT("F3");
        case WXK_F4: return wxT("F4");
        case WXK_F5: return wxT("F5");
        case WXK_F6: return wxT("F6");
        case WXK_F7: return wxT("F7");
        case WXK_F8: return wxT("F8");
        case WXK_F9: return wxT("F9");
        case WXK_F10: return wxT("F10");
        case WXK_F11: return wxT("F11");
        case WXK_F12: return wxT("F12");
        case WXK_F13: return wxT("F13");
        case WXK_F14: return wxT("F14");
        case WXK_F15: return wxT("F15");
        case WXK_F16: return wxT("F16");
        case WXK_F17: return wxT("F17");
        case WXK_F18: return wxT("F18");
        case WXK_F19: return wxT("F19");
        case WXK_F20: return wxT("F20");
        case WXK_F21: return wxT("F21");
        case WXK_F22: return wxT("F22");
        case WXK_F23: return wxT("F23");
        case WXK_F24: return wxT("F24");
        case WXK_NUMLOCK: return wxT("Numlock");
        case WXK_SCROLL: return wxT("Scroll");
        case WXK_PAGEUP: return wxT("Pageup");
        case WXK_PAGEDOWN: return wxT("Pagedown");
        case WXK_NUMPAD_SPACE: return wxT("Numpad_space");
        case WXK_NUMPAD_TAB: return wxT("Numpad_tab");
        case WXK_NUMPAD_ENTER: return wxT("Numpad_enter");
        case WXK_NUMPAD_F1: return wxT("Numpad_F1");
        case WXK_NUMPAD_F2: return wxT("Numpad_F2");
        case WXK_NUMPAD_F3: return wxT("Numpad_F3");
        case WXK_NUMPAD_F4: return wxT("Numpad_F4");
        case WXK_NUMPAD_HOME: return wxT("Numpad_home");
        case WXK_NUMPAD_LEFT: return wxT("Numpad_left");
        case WXK_NUMPAD_UP: return wxT("Numpad_up");
        case WXK_NUMPAD_RIGHT: return wxT("Numpad_right");
        case WXK_NUMPAD_DOWN: return wxT("Numpad_down");
        case WXK_NUMPAD_PAGEUP: return wxT("Numpad_pageup");
        case WXK_NUMPAD_PAGEDOWN: return wxT("Numpad_pagedown");
        case WXK_NUMPAD_END: return wxT("Numpad_end");
        case WXK_NUMPAD_BEGIN: return wxT("Numpad_begin");
        case WXK_NUMPAD_INSERT: return wxT("Numpad_insert");
        case WXK_NUMPAD_DELETE: return wxT("Numpad_delete");
        case WXK_NUMPAD_EQUAL: return wxT("Numpad_equal");
        case WXK_NUMPAD_MULTIPLY: return wxT("Numpad_multiply");
        case WXK_NUMPAD_ADD: return wxT("Numpad_add");
        case WXK_NUMPAD_SEPARATOR: return wxT("Numpad_separator");
        case WXK_NUMPAD_SUBTRACT: return wxT("Numpad_subtract");
        case WXK_NUMPAD_DECIMAL: return wxT("Numpad_decimal");

        default:
			if ( wxIsprint(code) ) {
				wxChar c = (char)code;
				return wxString(c, 1);
			} else if ( code > 0 && code < 27 ) {
			   wxChar c = _T('A') + code - 1;
			   return wxString(c, 1);
		   }
   }

   return wxT("");
}


//This is code from menucmn.cpp thats not publicly accessible, so I copied it here.
// return true if the 2 strings refer to the same accel
//
// as accels can be either translated or not, check for both possibilities and
// also compare case-insensitively as the key names case doesn't count
static inline bool CompareAccelString(const wxString& str, const wxChar *accel) {
	return str.CmpNoCase(accel) == 0 || str.CmpNoCase(wxGetTranslation(accel)) == 0;
}

// return prefixCode+number if the string is of the form "<prefix><number>" and
// 0 if it isn't
// first and last parameter specify the valid domain for "number" part
static int IsNumberedAccelKey(const wxString& str, const wxChar *prefix, wxKeyCode prefixCode, unsigned first, unsigned last) {
    const size_t lenPrefix = wxStrlen(prefix);
    if ( !CompareAccelString(str.Left(lenPrefix), prefix) )
        return 0;

    unsigned long num;
    if ( !str.Mid(lenPrefix).ToULong(&num) )
        return 0;

    if ( num < first || num > last )
    {
        // this must be a mistake, chances that this is a valid name of another
        // key are vanishingly small
        wxLogDebug(_T("Invalid key string \"%s\""), str.c_str());
        return 0;
    }

    return prefixCode + num - first;
}