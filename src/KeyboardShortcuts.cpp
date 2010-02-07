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
	//LoadCustomShortcuts(path);
	LoadDefaultShortcuts();
	MergeShortcuts();
	SetupShortcutIntMapping();
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
			continue;
		}

		anyMerged = true;
		m_shortcuts.insert(m_shortcuts.begin(), pair<wxString, KeyboardShortcutType*>(type->name, type));
		for(unsigned int c = 0; c < type->shortcuts.size(); c++) {
			m_keys.insert(m_keys.begin(), pair<int, KeyboardShortcut*>(type->shortcuts[c]->code, type->shortcuts[c]));
		}
	}

	if(anyMerged) {
		SaveShortcuts();
	}
}

void KeyboardShortcuts::LoadDefaultShortcuts() {
	RegisterDefaultShortcutAndBinding(wxT("New File"), wxT("&New"), wxT("Ctrl+N"));
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
	ReadShortcuts(m_path, &m_jsonRoot);
	ParseShortcuts(m_jsonRoot, m_shortcuts);
}

void KeyboardShortcuts::ReadShortcuts(wxString path, wxJSONValue* jsonRoot) {	// Open the settings file
	if (!wxFileExists(path)) return;
	wxFileInputStream fstream(path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open keyboard settings file."), _("File error"), wxICON_ERROR|wxOK);
		return;
	}

	// Parse the JSON contents
	wxJSONReader reader;
	const int numErrors = reader.Parse(fstream, jsonRoot);
	if ( numErrors > 0 )  {
		// if there are errors in the JSON document, print the errors
		const wxArrayString& errors = reader.GetErrors();
		wxString msg = _("Invalid JSON in settings:\n\n") + wxJoin(errors, wxT('\n'), '\0');
		wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
		return;
	}
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

void KeyboardShortcuts::ParseShortcuts(wxJSONValue& jsonRoot, map<wxString, KeyboardShortcutType*>& shortcuts) {
	if (!m_jsonRoot.HasMember(wxT("shortcuts"))) return;
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

		CreateEventType(shortcuts, name, menuText, allowSelection, allowVerticalSelection);

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

			KeyboardShortcut* keyBinding = CreateKeyBinding(shortcuts, name, primary, code, ctrl, alt, shift, meta, windows);
			m_keys.insert(m_keys.begin(), pair<int, KeyboardShortcut*>(code, keyBinding));
		}
	}
}

void KeyboardShortcuts::SaveShortcuts() {}

void KeyboardShortcuts::RegisterShortcut(wxString name, int id) {
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	iterator = m_shortcuts.find(name);
	if(iterator == m_shortcuts.end()) return;

	KeyboardShortcutType* existingEvent = iterator->second;
	existingEvent->id = id;
}

void KeyboardShortcuts::SetupShortcutIntMapping() {
	//If we ever want to be able to process keyboard events in a switch, we need to be able to map the event type to an int
	//This function defines that mapping.  There can be keyboard events that do not map to an int, but any event defined here must be listed in m_defaultShortcuts
	RegisterShortcut(wxT("Close Tab"), KEY_EDITOR_CTRL_CLOSE_TAB);
	
	RegisterShortcut(wxT("Copy"), KEY_EDITOR_CTRL_COPY);
	RegisterShortcut(wxT("Paste"), KEY_EDITOR_CTRL_PASTE);
	RegisterShortcut(wxT("Cut"), KEY_EDITOR_CTRL_CUT);
	
	RegisterShortcut(wxT("Delete the Current Line"), KEY_EDITOR_CTRL_DELETE_CURRENT_LINE);
	RegisterShortcut(wxT("Show Scope Tip"), KEY_EDITOR_CTRL_SHOW_SCOPE_TIP);
	RegisterShortcut(wxT("Complete Word"), KEY_EDITOR_CTRL_COMPLETION);
	
	RegisterShortcut(wxT("Move Cursor Word Left"), KEY_EDITOR_CTRL_CURSOR_WORD_LEFT);
	RegisterShortcut(wxT("Move Cursor Word Right"), KEY_EDITOR_CTRL_CURSOR_WORD_RIGHT);
	
	RegisterShortcut(wxT("Move Editor Up One Line"), KEY_EDITOR_CTRL_CURSOR_UP);
	RegisterShortcut(wxT("Move Editor Down One Line"), KEY_EDITOR_CTRL_CURSOR_DOWN);
	
	RegisterShortcut(wxT("Move Editor to Top"), KEY_EDITOR_CTRL_TOP);
	RegisterShortcut(wxT("Move Editor to Bottom"), KEY_EDITOR_CTRL_BOTTOM);
	
	RegisterShortcut(wxT("Move Cursor Left"), KEY_EDITOR_CTRL_LEFT);
	RegisterShortcut(wxT("Move Cursor Right"), KEY_EDITOR_CTRL_RIGHT);
	RegisterShortcut(wxT("Move Cursor Up"), KEY_EDITOR_CTRL_UP);
	RegisterShortcut(wxT("Move Cursor Down"), KEY_EDITOR_CTRL_DOWN);
	
	RegisterShortcut(wxT("Move to Beginning of Line"), KEY_EDITOR_CTRL_HOME);
	RegisterShortcut(wxT("Move to End of Line"), KEY_EDITOR_CTRL_END);
	
	RegisterShortcut(wxT("Pageup"), KEY_EDITOR_CTRL_PAGEUP);
	RegisterShortcut(wxT("Pagedown"), KEY_EDITOR_CTRL_PAGEDOWN);
	
	RegisterShortcut(wxT("Delete"), KEY_EDITOR_CTRL_DELETE);
	RegisterShortcut(wxT("Backspace"), KEY_EDITOR_CTRL_BACKSPACE);
	RegisterShortcut(wxT("New line"), KEY_EDITOR_CTRL_NEWLINE);
	RegisterShortcut(wxT("Tab"), KEY_EDITOR_CTRL_TAB);
	RegisterShortcut(wxT("Escape"), KEY_EDITOR_CTRL_ESCAPE);
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
	//hardcoded for now.  ignore alt and shift -> 10101 -> 21
	int eventCodeWithoutSelection = eventCode & 21;
		
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
		keyCodeWithoutSelection = keyCode & 21;
		
		if(keyCode == eventCode || (cur->type->allowSelection && keyCodeWithoutSelection == eventCodeWithoutSelection)) {
			return cur->type->id;
		}
		
	}
	
	return KEY_DEFAULT;
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