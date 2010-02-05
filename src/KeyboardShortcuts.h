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

#ifndef __KEYBOARD_SHORTCUTS_H__
#define __KEYBOARD_SHORTCUTS_H__

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>
#include <map>

using namespace std;

enum KeyboardEvents {
	KEY_EDITOR_CTRL_COPY,
	KEY_EDITOR_CTRL_DELETE_CURRENT_LINE,
	KEY_EDITOR_CTRL_SHOW_SCOPE_TIP,
	KEY_EDITOR_CTRL_PASTE,
	KEY_EDITOR_CTRL_CUT,
	KEY_EDITOR_CTRL_COMPLETION,
	KEY_EDITOR_CTRL_CURSOR_WORD_LEFT,
	KEY_EDITOR_CTRL_CURSOR_WORD_RIGHT,
	KEY_EDITOR_CTRL_CURSOR_UP,
	KEY_EDITOR_CTRL_CURSOR_DOWN,
	KEY_EDITOR_CTRL_TOP,
	KEY_EDITOR_CTRL_BOTTOM,
	KEY_EDITOR_CTRL_CLOSE_TAB,
	KEY_EDITOR_CTRL_LEFT,
	KEY_EDITOR_CTRL_RIGHT,
	KEY_EDITOR_CTRL_UP,
	KEY_EDITOR_CTRL_DOWN,
	KEY_EDITOR_CTRL_HOME,
	KEY_EDITOR_CTRL_END,
	KEY_EDITOR_CTRL_PAGEUP,
	KEY_EDITOR_CTRL_PAGEDOWN,
	KEY_EDITOR_CTRL_DELETE,
	KEY_EDITOR_CTRL_BACKSPACE,
	KEY_EDITOR_CTRL_NEWLINE,
	KEY_EDITOR_CTRL_TAB,
	KEY_EDITOR_CTRL_ESCAPE,
	KEY_DEFAULT
};

class KeyboardShortcuts {
	
private:
	KeyboardShortcuts();
	
	class KeyboardShortcutType;

	struct KeyboardShortcut {
		int code;
		bool ctrl, shift, alt, meta, windows;
		KeyboardShortcutType* type;
		
		KeyboardShortcut(KeyboardShortcutType* type, int code, bool ctrl, bool shift, bool alt, bool meta, bool windows) : 
		type(type), code(code), ctrl(ctrl), shift(shift), alt(alt), meta(meta), windows(windows) {}
	};
	
	class KeyboardShortcutType {
	public:
		wxString name;
		int id;
		vector<KeyboardShortcut*> shortcuts;
		KeyboardShortcut* primaryShortcut;
		bool hasPrimary, allowSelection, allowVerticalSelection;
		
		KeyboardShortcutType(wxString name, bool allowSelection, bool allowVerticalSelection) : 
		name(name), id(-1), hasPrimary(false), allowSelection(allowSelection), allowVerticalSelection(allowVerticalSelection) {}
		void AddShortcut(KeyboardShortcut* shortcut) {
			shortcuts.push_back(shortcut);
		}
		void AddPrimaryShortcut(KeyboardShortcut* shortcut) {
			hasPrimary = true;
			primaryShortcut = shortcut;
			AddShortcut(shortcut);
		}
	};
	
public:
	static KeyboardShortcuts& Instance() {
		static KeyboardShortcuts instance;
		return instance;	
	}
	~KeyboardShortcuts() {}
	
	void Init();
	
	void LoadDefaultShortcuts();
	void LoadCustomShortcuts();
	void SaveShortcuts();
	
	void RegisterShortcut(wxString name, int id);
	void SetupShortcutIntMapping();
	
	void CreateEventType(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool allowSelection, bool allowVerticalSelection);
	void CreateKeyBinding(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool primary, int code, bool ctrl, bool alt, bool shift, bool meta, bool windows);

	int GetEventType(wxKeyEvent& event);
	wxString GetEventKeyBinding(wxString eventName);
	wxString GetEventMenuText(wxString eventName);
	inline wxString Menu(wxString eventName) { return GetEventMenuText(eventName); }
	
private:
	map<wxString, KeyboardShortcutType*> m_shortcuts;
	map<wxString, KeyboardShortcutType*> m_defaultShortcuts;
	multimap<int, KeyboardShortcut*> m_keys;
};

#endif // __KEYBOARD_SHORTCUTS_H__
