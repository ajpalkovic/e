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

#include "KeyboardShortcuts.h"

using namespace std;

KeyboardShortcuts::KeyboardShortcuts() {
	
}

/** File Layout:
	var shortcuts = [
		{
			name: "shortcut name",
			allowSelection: true,
			allowVerticalSelection: true,
			shortcuts: [
				{
					code: 0,
					primary: true,
					modifiers: [ctrl, alt, shift, meta, windows]
				},
				...
			]
		}
	];
*/
void KeyboardShortcuts::Init() {
	LoadDefaultShortcuts();
	LoadCustomShortcuts();
	SetupShortcutIntMapping();
}
void KeyboardShortcuts::LoadDefaultShortcuts() {}
void KeyboardShortcuts::LoadCustomShortcuts() {
	CreateEventType(m_shortcuts, wxT("Copy"), true, true);
	CreateEventType(m_shortcuts, wxT("Cut"), false, false);
	CreateKeyBinding(m_shortcuts, wxT("Copy"), true, 3, true, false, false, false, false);
	CreateKeyBinding(m_shortcuts, wxT("Copy"), false, 1, false, false, false, false, false);

	CreateKeyBinding(m_shortcuts, wxT("Cut"), false, 13, true, false, false, false, false);
	CreateKeyBinding(m_shortcuts, wxT("Cut"), true, 11, false, false, false, false, false);
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
	RegisterShortcut(wxT("Copy"), KEY_EDITOR_CTRL_COPY);
	RegisterShortcut(wxT("Delete the Current Line"), KEY_EDITOR_CTRL_DELETE_CURRENT_LINE);
	RegisterShortcut(wxT("Show Scope Tip"), KEY_EDITOR_CTRL_SHOW_SCOPE_TIP);
	RegisterShortcut(wxT("Paste"), KEY_EDITOR_CTRL_PASTE);
	RegisterShortcut(wxT("Cut"), KEY_EDITOR_CTRL_CUT);
	RegisterShortcut(wxT("Show Auto-Complete"), KEY_EDITOR_CTRL_COMPLETION);
	RegisterShortcut(wxT("Move Cursor Word Left"), KEY_EDITOR_CTRL_CURSOR_WORD_LEFT);
	RegisterShortcut(wxT("Move Cursor Word Right"), KEY_EDITOR_CTRL_CURSOR_WORD_RIGHT);
	RegisterShortcut(wxT("Move Editor Up One Line"), KEY_EDITOR_CTRL_CURSOR_UP);
	RegisterShortcut(wxT("Move Editor Down One Line"), KEY_EDITOR_CTRL_CURSOR_DOWN);
	RegisterShortcut(wxT("Move Editor to Top"), KEY_EDITOR_CTRL_TOP);
	RegisterShortcut(wxT("Move Editor to Bottom"), KEY_EDITOR_CTRL_BOTTOM);
	RegisterShortcut(wxT("Close Current Tab"), KEY_EDITOR_CTRL_CLOSE_TAB);
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
void KeyboardShortcuts::CreateEventType(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool allowSelection, bool allowVerticalSelection) {
	if(shortcuts.find(name) == shortcuts.end()) {
		//this event doesn't exist, lets add it
		KeyboardShortcutType* type = new KeyboardShortcutType(name, allowSelection, allowVerticalSelection);
		shortcuts.insert(shortcuts.begin(), pair<wxString, KeyboardShortcutType*>(name, type));
	} else {
		//this event already exists.  o noz
	}
}

//This function will be called multiple times to associate a key binding with an event type
void KeyboardShortcuts::CreateKeyBinding(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool primary, int code, bool ctrl, bool alt, bool shift, bool meta, bool windows) {
	map<wxString, KeyboardShortcutType*>::iterator iterator;
	iterator = shortcuts.find(name);
	if(iterator == shortcuts.end()) {
		//no event exists with this name
		return;
	}
	
	KeyboardShortcutType* type = iterator->second;
	KeyboardShortcut* event = new KeyboardShortcut(type, code, ctrl, shift, alt, meta, windows);
	if(primary) type->AddPrimaryShortcut(event);
	else type->AddShortcut(event);
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
	ret += primary->code;
	
	return ret;
}
wxString KeyboardShortcuts::GetEventMenuText(wxString eventName) {
	wxString binding = GetEventKeyBinding(eventName);
	if(binding.IsEmpty()) return eventName;
	//TODO: Put in the ampersand here
	return eventName+wxT("\t")+binding;
}