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
	
	struct KeyboardShortcut {
		wxString name;
		int id, code;
		
		KeyboardShortcut(wxString name, int id, int code) : name(name), id(id), code(code) {}
	};
	
public:
	static KeyboardShortcuts& Instance() {
		static KeyboardShortcuts instance;
		return instance;	
	}
	~KeyboardShortcuts() {}
	
	void RegisterDefaultShortcuts();
	bool CheckEventExists(KeyboardShortcut& event);
	int CreateKeyCode(int ctrl, int alt, int shift, int meta, int keyCode);
	int CreateKeyCode(wxKeyEvent& event);
	void RegisterEvent(wxString name, int id, int defaultKeyCode, bool ctrl=false, bool alt=false, bool shift=false, bool meta=false);
	vector<int> GetEvents(wxKeyEvent& event);
	void SaveKeys();
	void LoadKeys();
	
private:
	multimap<wxString, KeyboardShortcut> m_shortcuts;
	multimap<int, KeyboardShortcut> m_keys;
};

#endif // __KEYBOARD_SHORTCUTS_H__
