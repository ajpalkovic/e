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

void KeyboardShortcuts::RegisterDefaultShortcuts() {
	RegisterEvent(wxT("Copy"), KEY_EDITOR_CTRL_COPY, WXK_INSERT, true);
	RegisterEvent(wxT("Copy"), KEY_EDITOR_CTRL_COPY, 3, true); //Ctrl-C
	RegisterEvent(wxT("Delete the Current Line"), KEY_EDITOR_CTRL_DELETE_CURRENT_LINE, 11, true); //Ctrl-K
	RegisterEvent(wxT("Show Scope Tip"), KEY_EDITOR_CTRL_SHOW_SCOPE_TIP, 16, true); //Ctrl-P
	RegisterEvent(wxT("Paste"), KEY_EDITOR_CTRL_PASTE, 22, true); //Ctrl-V
	RegisterEvent(wxT("Paste"), KEY_EDITOR_CTRL_PASTE, WXK_INSERT, false, true);
	RegisterEvent(wxT("Paste"), KEY_EDITOR_CTRL_PASTE, WXK_NUMPAD_INSERT, false, true);
	RegisterEvent(wxT("Cut"), KEY_EDITOR_CTRL_CUT, 24, true); //Ctrl-X
	RegisterEvent(wxT("Show Auto-Complete"), KEY_EDITOR_CTRL_COMPLETION, WXK_SPACE, true); //Ctrl-space
	RegisterEvent(wxT("Move Cursor Word Left"), KEY_EDITOR_CTRL_CURSOR_WORD_LEFT, WXK_LEFT, true); //Ctrl <-
	RegisterEvent(wxT("Move Cursor Word Right"), KEY_EDITOR_CTRL_CURSOR_WORD_RIGHT, WXK_RIGHT, true); //Ctrl ->
	RegisterEvent(wxT("Move Editor Up One Line"), KEY_EDITOR_CTRL_CURSOR_UP, WXK_UP, true); //Ctrl-arrow up
	RegisterEvent(wxT("Move Editor Down One Line"), KEY_EDITOR_CTRL_CURSOR_DOWN, WXK_DOWN, true); //Ctrl-arrow down
	RegisterEvent(wxT("Move Editor to Top"), KEY_EDITOR_CTRL_TOP, WXK_HOME, true);
	RegisterEvent(wxT("Move Editor to Bottom"), KEY_EDITOR_CTRL_BOTTOM, WXK_END, true);
	RegisterEvent(wxT("Close Current Tab"), KEY_EDITOR_CTRL_CLOSE_TAB, WXK_F4, true);

	RegisterEvent(wxT("Move Cursor Left"), KEY_EDITOR_CTRL_LEFT, WXK_LEFT);
	RegisterEvent(wxT("Move Cursor Right"), KEY_EDITOR_CTRL_RIGHT, WXK_RIGHT);
	RegisterEvent(wxT("Move Cursor Up"), KEY_EDITOR_CTRL_UP, WXK_UP);
	RegisterEvent(wxT("Move Cursor Down"), KEY_EDITOR_CTRL_DOWN, WXK_DOWN);
	RegisterEvent(wxT("Move Cursor Left"), KEY_EDITOR_CTRL_LEFT, WXK_NUMPAD_LEFT);
	RegisterEvent(wxT("Move Cursor Right"), KEY_EDITOR_CTRL_RIGHT, WXK_NUMPAD_RIGHT);
	RegisterEvent(wxT("Move Cursor Up"), KEY_EDITOR_CTRL_UP, WXK_NUMPAD_UP);
	RegisterEvent(wxT("Move Cursor Down"), KEY_EDITOR_CTRL_DOWN, WXK_NUMPAD_DOWN);
	
	RegisterEvent(wxT("Move to Beginning of Line"), KEY_EDITOR_CTRL_HOME, WXK_HOME);
	RegisterEvent(wxT("Move to Beginning of Line"), KEY_EDITOR_CTRL_HOME, WXK_NUMPAD_HOME);
	RegisterEvent(wxT("Move to Beginning of Line"), KEY_EDITOR_CTRL_HOME, WXK_NUMPAD_BEGIN);
	
	RegisterEvent(wxT("Move to End of Line"), KEY_EDITOR_CTRL_END, WXK_END);
	RegisterEvent(wxT("Move to End of Line"), KEY_EDITOR_CTRL_END, WXK_NUMPAD_END);
	
	RegisterEvent(wxT("Pageup"), KEY_EDITOR_CTRL_PAGEUP, WXK_PAGEUP);
	RegisterEvent(wxT("Pageup"), KEY_EDITOR_CTRL_PAGEUP, WXK_NUMPAD_PAGEUP);
	RegisterEvent(wxT("Pagedown"), KEY_EDITOR_CTRL_PAGEDOWN, WXK_PAGEDOWN);
	RegisterEvent(wxT("Pagedown"), KEY_EDITOR_CTRL_PAGEDOWN, WXK_NUMPAD_PAGEDOWN);
	
	RegisterEvent(wxT("Delete"), KEY_EDITOR_CTRL_DELETE, WXK_DELETE);
	RegisterEvent(wxT("Delete"), KEY_EDITOR_CTRL_DELETE, WXK_NUMPAD_DELETE);
	RegisterEvent(wxT("Backspace"), KEY_EDITOR_CTRL_BACKSPACE, WXK_BACK);
	
	RegisterEvent(wxT("New line"), KEY_EDITOR_CTRL_NEWLINE, WXK_RETURN);
	RegisterEvent(wxT("New line"), KEY_EDITOR_CTRL_NEWLINE, WXK_NUMPAD_ENTER);
	RegisterEvent(wxT("Tab"), KEY_EDITOR_CTRL_TAB, WXK_TAB);
	RegisterEvent(wxT("Tab"), KEY_EDITOR_CTRL_TAB, WXK_NUMPAD_TAB);

	RegisterEvent(wxT("Escape"), KEY_EDITOR_CTRL_ESCAPE, WXK_ESCAPE);
}


int KeyboardShortcuts::CreateKeyCode(int ctrl, int alt, int shift, int meta, int keyCode) {
	int code = ctrl | (alt << 1) | (shift << 2) | (meta << 3);
	return (code << 16) | keyCode;
}

int KeyboardShortcuts::CreateKeyCode(wxKeyEvent& event) {
	int code = event.ControlDown() | (event.AltDown() << 1) | (event.ShiftDown() << 2) | (event.MetaDown() << 3);
	return (code << 16) | event.GetKeyCode();
}

bool KeyboardShortcuts::CheckEventExists(KeyboardShortcut& event) {
	multimap<wxString, KeyboardShortcut>::iterator iterator;
	iterator = m_shortcuts.find(event.name);
	if(iterator == m_shortcuts.end()) return false;

	KeyboardShortcut existingEvent = iterator->second;
	existingEvent.code = event.code;

	return true;
}

void KeyboardShortcuts::RegisterEvent(wxString name, int id, int defaultKeyCode, bool ctrl, bool alt, bool shift, bool meta) {
	KeyboardShortcut event = KeyboardShortcut(name, id, CreateKeyCode(ctrl, alt, shift, meta, defaultKeyCode));
	if(CheckEventExists(event)) return;

	m_shortcuts.insert(m_shortcuts.begin(), pair<wxString, KeyboardShortcut>(name, event));
	m_keys.insert(m_keys.begin(), pair<int, KeyboardShortcut>(event.code, event));
}

vector<int> KeyboardShortcuts::GetEvents(wxKeyEvent& event) {
	int code = CreateKeyCode(event);

	multimap<int, KeyboardShortcut>::iterator iterator;
	pair<multimap<int, KeyboardShortcut>::iterator, multimap<int, KeyboardShortcut>::iterator> matches;
	vector<int> ret;
	
	matches = m_keys.equal_range(code);
	for(iterator = matches.first; iterator != matches.second; ++iterator) {
		ret.push_back(iterator->second.id);
	}
	
	return ret;
}
void KeyboardShortcuts::SaveKeys() {
	
}
void KeyboardShortcuts::LoadKeys() {
	
}