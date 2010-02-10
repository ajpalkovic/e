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

#include "jsonval.h"

using namespace std;

//A simple macro for looking up the keyboard shortcut for the menus
#define M(name) m_keyboardShortcuts.GetEventMenuText(wxT(name)), wxT(name)
#define M2(name, menu) m_keyboardShortcuts.GetEventMenuText(wxT(name)), menu, wxT(name)

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

class KeyboardShortcutType;

struct KeyboardShortcut {
	int code;
	bool ctrl, shift, alt, meta, windows, primary;
	KeyboardShortcutType* type;
	
	KeyboardShortcut(KeyboardShortcutType* type, int code, bool ctrl, bool shift, bool alt, bool meta, bool windows) : 
	type(type), code(code), ctrl(ctrl), shift(shift), alt(alt), meta(meta), windows(windows), primary(false) {}
};

class KeyboardShortcutType {
public:
	wxString name, menuText;
	int id;
	vector<KeyboardShortcut*> shortcuts;
	KeyboardShortcut* primaryShortcut;
	bool hasPrimary, allowSelection, allowVerticalSelection;
	
	KeyboardShortcutType(wxString name, wxString menuText, bool allowSelection, bool allowVerticalSelection) : 
	name(name), menuText(menuText), id(-1), hasPrimary(false), allowSelection(allowSelection), allowVerticalSelection(allowVerticalSelection) {}
	void AddShortcut(KeyboardShortcut* shortcut) {
		shortcuts.push_back(shortcut);
	}
	void AddPrimaryShortcut(KeyboardShortcut* shortcut) {
		hasPrimary = true;
		primaryShortcut = shortcut;
		shortcut->primary = true;
		AddShortcut(shortcut);
	}
};

class KeyboardShortcuts {
	
private:
	KeyboardShortcuts();
	
public:
	static KeyboardShortcuts& Instance() {
		static KeyboardShortcuts instance;
		return instance;	
	}
	~KeyboardShortcuts() {}
	
	void Init(wxString path);
	
	void LoadCustomShortcuts(wxString path);
	void LoadSavedShortcuts();

	void LoadDefaultShortcuts();
	void RegisterDefaultShortcut(wxString name, wxString menuText, bool allowSelection=false, bool allowVerticalSelection=false);
	void RegisterDefaultShortcutAndBinding(wxString name, wxString menuText, wxString binding, bool allowSelection=false, bool allowVerticalSelection=false);
	void RegisterDefaultOnCharShortcut(wxString name, int code, bool primary, bool allowSelection, bool allowVerticalSelection, bool ctrl=false, bool alt=false, bool shift=false, bool meta=false, bool windows=false);
	int TranslateStringToCode(wxString binding);

	void MergeShortcuts();
	void RegisterShortcutIntMapping(wxString name, int id);
	void SetupShortcutIntMappings();
	void SaveShortcuts();
	
	void CreateEventType(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, wxString menuText, bool allowSelection, bool allowVerticalSelection);
	KeyboardShortcut* CreateKeyBinding(map<wxString, KeyboardShortcutType*>& shortcuts, wxString name, bool primary, int code, bool ctrl, bool alt, bool shift, bool meta, bool windows);

	int GetEventType(wxKeyEvent& event);
	wxString GetEventKeyBinding(wxString eventName);
	wxString GetEventMenuText(wxString eventName);
	inline wxString Menu(wxString eventName) { return GetEventMenuText(eventName); }
	wxString TranslateCodeToString(int keycode);

	bool IsSelectDown(wxKeyEvent& event);
	bool IsMultiSelectDown(wxKeyEvent& event);
	bool IsVerticalSelectDown(wxKeyEvent& event);
	bool IsSpecialKeyDown(int key, wxKeyEvent& event);

	bool IsSelectDown(wxMouseEvent& event);
	bool IsMultiSelectDown(wxMouseEvent& event);
	bool IsVerticalSelectDown(wxMouseEvent& event);
	bool IsSpecialKeyDown(int key, wxMouseEvent& event);
	
	bool IsSelectDown();
	bool IsMultiSelectDown();
	bool IsVerticalSelectDown();
	bool IsSpecialKeyDown(int key);
	
private:
	map<wxString, KeyboardShortcutType*> m_shortcuts;
	map<wxString, KeyboardShortcutType*> m_defaultShortcuts;
	multimap<int, KeyboardShortcut*> m_keys;
	
	wxString m_path;

	//1-Ctrl, 2-Alt, 4-Shift, 8-Meta, 16-Windows
	int selectKey, verticalSelectKey, multiSelectKey;
};

static int IsNumberedAccelKey(const wxString& str, const wxChar *prefix, wxKeyCode prefixCode, unsigned first, unsigned last);
static inline bool CompareAccelString(const wxString& str, const wxChar *accel);
static const struct wxKeyName {
    wxKeyCode code;
    const wxChar *name;
} wxKeyNames[] = {
	{ WXK_DELETE, wxTRANSLATE("DEL") },
	{ WXK_DELETE, wxTRANSLATE("DELETE") },
	{ WXK_BACK, wxTRANSLATE("BACK") },
	{ WXK_INSERT, wxTRANSLATE("INS") },
	{ WXK_INSERT, wxTRANSLATE("INSERT") },
	{ WXK_RETURN, wxTRANSLATE("ENTER") },
	{ WXK_RETURN, wxTRANSLATE("RETURN") },
	{ WXK_PAGEUP, wxTRANSLATE("PGUP") },
	{ WXK_PAGEDOWN, wxTRANSLATE("PGDN") },
	{ WXK_LEFT, wxTRANSLATE("LEFT") },
	{ WXK_RIGHT, wxTRANSLATE("RIGHT") },
	{ WXK_UP, wxTRANSLATE("UP") },
	{ WXK_DOWN, wxTRANSLATE("DOWN") },
	{ WXK_HOME, wxTRANSLATE("HOME") },
	{ WXK_END, wxTRANSLATE("END") },
	{ WXK_SPACE, wxTRANSLATE("SPACE") },
	{ WXK_TAB, wxTRANSLATE("TAB") },
	{ WXK_ESCAPE, wxTRANSLATE("ESC") },
	{ WXK_ESCAPE, wxTRANSLATE("ESCAPE") },
	{ WXK_CANCEL, wxTRANSLATE("CANCEL") },
	{ WXK_CLEAR, wxTRANSLATE("CLEAR") },
	{ WXK_MENU, wxTRANSLATE("MENU") },
	{ WXK_PAUSE, wxTRANSLATE("PAUSE") },
	{ WXK_CAPITAL, wxTRANSLATE("CAPITAL") },
	{ WXK_SELECT, wxTRANSLATE("SELECT") },
	{ WXK_PRINT, wxTRANSLATE("PRINT") },
	{ WXK_EXECUTE, wxTRANSLATE("EXECUTE") },
	{ WXK_SNAPSHOT, wxTRANSLATE("SNAPSHOT") },
	{ WXK_HELP, wxTRANSLATE("HELP") },
	{ WXK_ADD, wxTRANSLATE("ADD") },
	{ WXK_SEPARATOR, wxTRANSLATE("SEPARATOR") },
	{ WXK_SUBTRACT, wxTRANSLATE("SUBTRACT") },
	{ WXK_DECIMAL, wxTRANSLATE("DECIMAL") },
	{ WXK_DIVIDE, wxTRANSLATE("DIVIDE") },
	{ WXK_NUMLOCK, wxTRANSLATE("NUM_LOCK") },
	{ WXK_SCROLL, wxTRANSLATE("SCROLL_LOCK") },
	{ WXK_PAGEUP, wxTRANSLATE("PAGEUP") },
	{ WXK_PAGEDOWN, wxTRANSLATE("PAGEDOWN") },
	{ WXK_NUMPAD_SPACE, wxTRANSLATE("KP_SPACE") },
	{ WXK_NUMPAD_TAB, wxTRANSLATE("KP_TAB") },
	{ WXK_NUMPAD_ENTER, wxTRANSLATE("KP_ENTER") },
	{ WXK_NUMPAD_HOME, wxTRANSLATE("KP_HOME") },
	{ WXK_NUMPAD_LEFT, wxTRANSLATE("KP_LEFT") },
	{ WXK_NUMPAD_UP, wxTRANSLATE("KP_UP") },
	{ WXK_NUMPAD_RIGHT, wxTRANSLATE("KP_RIGHT") },
	{ WXK_NUMPAD_DOWN, wxTRANSLATE("KP_DOWN") },
	{ WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PRIOR") },
	{ WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PAGEUP") },
	{ WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_NEXT") },
	{ WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_PAGEDOWN") },
	{ WXK_NUMPAD_END, wxTRANSLATE("KP_END") },
	{ WXK_NUMPAD_BEGIN, wxTRANSLATE("KP_BEGIN") },
	{ WXK_NUMPAD_INSERT, wxTRANSLATE("KP_INSERT") },
	{ WXK_NUMPAD_DELETE, wxTRANSLATE("KP_DELETE") },
	{ WXK_NUMPAD_EQUAL, wxTRANSLATE("KP_EQUAL") },
	{ WXK_NUMPAD_MULTIPLY, wxTRANSLATE("KP_MULTIPLY") },
	{ WXK_NUMPAD_ADD, wxTRANSLATE("KP_ADD") },
	{ WXK_NUMPAD_SEPARATOR, wxTRANSLATE("KP_SEPARATOR") },
	{ WXK_NUMPAD_SUBTRACT, wxTRANSLATE("KP_SUBTRACT") },
	{ WXK_NUMPAD_DECIMAL, wxTRANSLATE("KP_DECIMAL") },
	{ WXK_NUMPAD_DIVIDE, wxTRANSLATE("KP_DIVIDE") },
	{ WXK_WINDOWS_LEFT, wxTRANSLATE("WINDOWS_LEFT") },
	{ WXK_WINDOWS_RIGHT, wxTRANSLATE("WINDOWS_RIGHT") },
	{ WXK_WINDOWS_MENU, wxTRANSLATE("WINDOWS_MENU") },
	{ WXK_COMMAND, wxTRANSLATE("COMMAND") },
};

#endif // __KEYBOARD_SHORTCUTS_H__
