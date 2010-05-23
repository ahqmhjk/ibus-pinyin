/* vim:set et ts=4 sts=4:
 *
 * ibus-pinyin - The Chinese PinYin engine for IBus
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <boost/bind.hpp>
#include "Types.h"
#include "Config.h"
#include "Util.h"

namespace PY {

PinyinConfig * PinyinConfig::m_instance = NULL;
BopomofoConfig * BopomofoConfig::m_instance = NULL;

#if 0
#endif

static const std::string engine_pinyin ("engine/Pinyin");
static const std::string correct_pinyin ("CorrectPinyin");
static const std::string fuzzy_pinyin ("FuzzyPinyin");

static const std::string orientation ("LookupTableOrientation");
static const std::string page_size ("LookupTablePageSize");
static const std::string shift_select_candidate ("ShiftSelectCandidate");
static const std::string minus_equal_page ("MinusEqualPage");
static const std::string comma_period_page ("CommaPeriodPage");
static const std::string auto_commit ("AutoCommit");

static const std::string double_pinyin ("DoublePinyin");
static const std::string double_pinyin_schema ("DoublePinyinSchema");
static const std::string double_pinyin_show_raw ("DoublePinyinShowRaw");

static const std::string init_chinese ("InitChinese");
static const std::string init_full ("InitFull");
static const std::string init_full_punct ("InitFullPunct");
static const std::string init_simp_chinese ("InitSimplifiedChinese");
static const std::string trad_candidate ("TradCandidate");
static const std::string special_phrases ("SpecialPhrases");



Config::Config (Bus & bus, const std::string & name)
    : Object (ibus_bus_get_config (bus)),
      m_section ("engine/" + name)
{
    m_option = PINYIN_INCOMPLETE_PINYIN | PINYIN_CORRECT_ALL;
    m_option_mask = PINYIN_INCOMPLETE_PINYIN | PINYIN_CORRECT_ALL;

    m_orientation = IBUS_ORIENTATION_HORIZONTAL;
    m_page_size = 5;
    m_shift_select_candidate = FALSE;
    m_minus_equal_page = TRUE;
    m_comma_period_page = TRUE;
    m_auto_commit = FALSE;

    m_double_pinyin = FALSE;
    m_double_pinyin_schema = 0;
    m_double_pinyin_show_raw = FALSE;

    m_init_chinese = TRUE;
    m_init_full = FALSE;
    m_init_full_punct = TRUE;
    m_init_simp_chinese = TRUE;
    m_trad_candidate = FALSE;
    m_special_phrases = TRUE;

    readDefaultValues ();
    g_signal_connect (get<IBusConfig> (),
                      "value-changed",
                      G_CALLBACK (valueChangedCallback),
                      this);
}

static const struct {
    std::string name;
    guint option;
    bool defval;
} options [] = {
    { std::string ("IncompletePinyin"), PINYIN_INCOMPLETE_PINYIN, TRUE },
    /* correct */
    { std::string ("CorrectPinyin_GN_NG"),    PINYIN_CORRECT_GN_TO_NG,    TRUE },
    { std::string ("CorrectPinyin_GN_NG"),    PINYIN_CORRECT_GN_TO_NG,    TRUE },
    { std::string ("CorrectPinyin_MG_NG"),    PINYIN_CORRECT_MG_TO_NG,    TRUE },
    { std::string ("CorrectPinyin_IOU_IU"),   PINYIN_CORRECT_IOU_TO_IU,   TRUE },
    { std::string ("CorrectPinyin_UEI_UI"),   PINYIN_CORRECT_UEI_TO_UI,   TRUE },
    { std::string ("CorrectPinyin_UEN_UN"),   PINYIN_CORRECT_UEN_TO_UN,   TRUE },
    { std::string ("CorrectPinyin_UE_VE"),    PINYIN_CORRECT_UE_TO_VE,    TRUE },
    { std::string ("CorrectPinyin_V_U"),      PINYIN_CORRECT_V_TO_U,      TRUE },
    { std::string ("CorrectPinyin_VE_UE"),    PINYIN_CORRECT_V_TO_U,      TRUE },
    /* fuzzy pinyin */
    { std::string ("FuzzyPinyin_C_CH"),      PINYIN_FUZZY_C_CH,  FALSE },
    { std::string ("FuzzyPinyin_CH_C"),      PINYIN_FUZZY_CH_C,  FALSE },
    { std::string ("FuzzyPinyin_Z_ZH"),      PINYIN_FUZZY_Z_ZH,  FALSE },
    { std::string ("FuzzyPinyin_ZH_Z"),      PINYIN_FUZZY_ZH_Z,  FALSE },
    { std::string ("FuzzyPinyin_S_SH"),      PINYIN_FUZZY_S_SH,  FALSE },
    { std::string ("FuzzyPinyin_SH_S"),      PINYIN_FUZZY_SH_S,  FALSE },
    { std::string ("FuzzyPinyin_L_N"),       PINYIN_FUZZY_L_N,   FALSE },
    { std::string ("FuzzyPinyin_N_L"),       PINYIN_FUZZY_N_L,   FALSE },
    { std::string ("FuzzyPinyin_F_H"),       PINYIN_FUZZY_F_H,   FALSE },
    { std::string ("FuzzyPinyin_H_F"),       PINYIN_FUZZY_H_F,   FALSE },
    { std::string ("FuzzyPinyin_L_R"),       PINYIN_FUZZY_L_R,   FALSE },
    { std::string ("FuzzyPinyin_R_L"),       PINYIN_FUZZY_R_L,   FALSE },
    { std::string ("FuzzyPinyin_K_G"),       PINYIN_FUZZY_K_G,   FALSE },
    { std::string ("FuzzyPinyin_G_K"),       PINYIN_FUZZY_G_K,   FALSE },
    { std::string ("FuzzyPinyin_AN_ANG"),    PINYIN_FUZZY_AN_ANG,    FALSE },
    { std::string ("FuzzyPinyin_ANG_AN"),    PINYIN_FUZZY_ANG_AN,    FALSE },
    { std::string ("FuzzyPinyin_EN_ENG"),    PINYIN_FUZZY_EN_ENG,    FALSE },
    { std::string ("FuzzyPinyin_ENG_EN"),    PINYIN_FUZZY_ENG_EN,    FALSE },
    { std::string ("FuzzyPinyin_IN_ING"),    PINYIN_FUZZY_IN_ING,    FALSE },
    { std::string ("FuzzyPinyin_ING_IN"),    PINYIN_FUZZY_ING_IN,    FALSE },
    { std::string ("FuzzyPinyin_IAN_IANG"),  PINYIN_FUZZY_IAN_IANG,  FALSE },
    { std::string ("FuzzyPinyin_IANG_IAN"),  PINYIN_FUZZY_IANG_IAN,  FALSE },
    { std::string ("FuzzyPinyin_UAN_UANG"),  PINYIN_FUZZY_UAN_UANG,  FALSE },
    { std::string ("FuzzyPinyin_UANG_UAN"),  PINYIN_FUZZY_UANG_UAN,  FALSE },
};

void
Config::readDefaultValues (void)
{
    /* double pinyin */
    m_double_pinyin = read (double_pinyin, false);
    m_double_pinyin_schema = read (double_pinyin_schema, 0);
    if (m_double_pinyin_schema >= 5) {
        m_double_pinyin_schema = 0;
        g_warn_if_reached ();
    }
    m_double_pinyin_show_raw = read (double_pinyin_show_raw, false);

    /* init states */
    m_init_chinese = read (init_chinese, true);
    m_init_full = read (init_full, false);
    m_init_full_punct = read (init_full_punct, true);
    m_init_simp_chinese = read (init_simp_chinese, true);

    m_trad_candidate = read (trad_candidate, false);
    m_special_phrases = read (special_phrases, true);

    /* others */
    m_orientation = read (PY::orientation, 0);
    if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
        m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
        m_orientation = IBUS_ORIENTATION_HORIZONTAL;
        g_warn_if_reached ();
    }
    m_page_size = read (page_size, 5);
    if (m_page_size > 10) {
        m_page_size = 5;
        g_warn_if_reached ();
    }
    m_shift_select_candidate = read (shift_select_candidate, false);
    m_minus_equal_page = read (minus_equal_page, true);
    m_comma_period_page = read (comma_period_page, true);
    m_auto_commit = read (auto_commit, false);

    /* correct pinyin */
    if (read (correct_pinyin, true))
        m_option_mask |= PINYIN_CORRECT_ALL;
    else
        m_option_mask &= ~PINYIN_CORRECT_ALL;

    /* fuzzy pinyin */
    if (read (fuzzy_pinyin, false))
        m_option_mask |= PINYIN_FUZZY_ALL;
    else
        m_option_mask &= ~PINYIN_FUZZY_ALL;

    /* read values */
    for (guint i = 0; i < G_N_ELEMENTS (options); i++) {
        if (read (options[i].name, options[i].defval))
            m_option |= options[i].option;
        else
            m_option &= ~options[i].option;
    }
}

inline bool
Config::read (const std::string & name,
              bool                defval)
{
    GValue value = {0};
    if (ibus_config_get_value (get<IBusConfig> (), m_section.c_str (), name.c_str (), &value)) {
        if (G_VALUE_TYPE (&value) == G_TYPE_BOOLEAN)
            return g_value_get_boolean (&value);
    }
    return defval;
}

inline gint
Config::read (const std::string & name,
              gint                defval)
{
    GValue value = {0};
    if (ibus_config_get_value (get<IBusConfig> (), m_section.c_str (), name.c_str (), &value)) {
        if (G_VALUE_TYPE (&value) == G_TYPE_INT)
            return g_value_get_int (&value);
    }
    return defval;
}

static inline bool
normalizeGValue (const GValue *value, bool defval)
{
    if (value == NULL || G_VALUE_TYPE (value) != G_TYPE_BOOLEAN)
        return defval;
    return g_value_get_boolean (value);
}

static inline gint
normalizeGValue (const GValue *value, gint defval)
{
    if (value == NULL || G_VALUE_TYPE (value) != G_TYPE_INT)
        return defval;
    return g_value_get_int (value);
}


void
Config::valueChanged (const std::string & section,
                      const std::string & name,
                      const GValue  *value)
{
    if (engine_pinyin != section)
        return;

    /* double pinyin */
    if (double_pinyin == name)
        m_double_pinyin = normalizeGValue (value, false);
    else if (double_pinyin_schema == name) {
        m_double_pinyin_schema = normalizeGValue (value, 0);
        if (m_double_pinyin_schema >= 5) {
            m_double_pinyin_schema = 0;
            g_warn_if_reached ();
        }
    }
    else if (double_pinyin_show_raw == name)
        m_double_pinyin_show_raw = normalizeGValue (value, false);
    /* init states */
    else if (init_chinese == name)
        m_init_chinese = normalizeGValue (value, true);
    else if (init_full == name)
        m_init_full = normalizeGValue (value, true);
    else if (init_full_punct == name)
        m_init_full_punct = normalizeGValue (value, true);
    else if (init_simp_chinese == name)
        m_init_simp_chinese = normalizeGValue (value, true);
    else if (trad_candidate == name)
        m_trad_candidate = normalizeGValue (value, false);
    else if (special_phrases == name)
        m_special_phrases = normalizeGValue (value, true);
    /* lookup table page size */
    else if (PY::orientation == name) {
        m_orientation = normalizeGValue (value, 0);
        if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
            m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
            m_orientation = IBUS_ORIENTATION_HORIZONTAL;
            g_warn_if_reached ();
        }
    }
    else if (page_size == name) {
        m_page_size = normalizeGValue (value, 5);
        if (m_page_size > 10) {
            m_page_size = 5;
            g_warn_if_reached ();
        }
    }
    else if (shift_select_candidate == name)
        m_shift_select_candidate = normalizeGValue (value, false);
    else if (minus_equal_page == name)
        m_minus_equal_page = normalizeGValue (value, true);
    else if (comma_period_page == name)
        m_comma_period_page = normalizeGValue (value, true);
    else if (auto_commit == name)
        m_auto_commit = normalizeGValue (value, false);
    /* correct pinyin */
    else if (correct_pinyin == name) {
        if (normalizeGValue (value, TRUE))
            m_option_mask |= PINYIN_CORRECT_ALL;
        else
            m_option_mask &= ~PINYIN_CORRECT_ALL;
    }
    /* fuzzy pinyin */
    else if (fuzzy_pinyin == name) {
        if (normalizeGValue (value, TRUE))
            m_option_mask |= PINYIN_FUZZY_ALL;
        else
            m_option_mask &= ~PINYIN_FUZZY_ALL;
    }
    else {
        for (guint i = 0; i < G_N_ELEMENTS (options); i++) {
            if (G_LIKELY (options[i].name != name))
                continue;
            if (normalizeGValue (value, options[i].defval))
                m_option |= options[i].option;
            else
                m_option &= ~options[i].option;
            break;
        }
    }

}

void
Config::valueChangedCallback (IBusConfig    *config,
                              const gchar   *section,
                              const gchar   *name,
                              const GValue  *value,
                              Config        *self)
{
    self->valueChanged (section, name, value);
}

void
PinyinConfig::init (Bus & bus)
{
    if (PinyinConfig::m_instance == NULL) {
        PinyinConfig::m_instance = new PinyinConfig (bus);
    }
}

void
BopomofoConfig::init (Bus & bus)
{
    if (BopomofoConfig::m_instance == NULL) {
        BopomofoConfig::m_instance = new BopomofoConfig (bus);
    }
}

};
