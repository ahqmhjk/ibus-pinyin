/* vim:set et sts=4: */
#ifndef __PY_EXT_EDITOR__
#define __PY_EXT_EDITOR__

#include <glib.h>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "lua-plugin.h"
}
#include <string>
#include "Editor.h"

namespace PY {

class ExtEditor : public Editor {
public:
    ExtEditor (PinyinProperties &props);

    virtual gboolean processKeyEvent (guint keyval, guint keycode, guint modifiers);
    virtual void pageUp (void);
    virtual void pageDown (void);
    virtual void cursorUp (void);
    virtual void cursorDown (void);
    virtual void update (void);
    virtual void reset (void);
    virtual void candidateClicked (guint index, guint button, guint state);

    int load_lua_script(std::string filename);
    void reset_lua_state();

private:
    Pointer<IBusEnginePlugin> m_lua_plugin;
    lua_command_t * m_current_command;
    std::string m_input;

    LookupTable m_lookup_table;
};

};
#endif
