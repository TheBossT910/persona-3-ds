/**
 * @file enums.h
 * @brief Holds enums used game-wide
 */

#pragma once
#include <aegis/system.hpp>

enum class MoonSprite
{
    MOON_0 = 0,
    MOON_1,
    MOON_2,
    MOON_3,
    MOON_4,
    MOON_5,
    MOON_6,
    MOON_7,
    MOON_8,
    MOON_9,
    MOON_10,
    MOON_11,
    MOON_12,
    MOON_13,
    MOON_14,
    MOON_15,
    MOON_16,
    MOON_17,
    MOON_18,
    MOON_19,
    MOON_20,
    MOON_21,
    MOON_22,
    MOON_23,
    MOON_24,
    MOON_25,
    MOON_26,
    MOON_27,
    MOON_28,
    MOON_29
};

enum class DayOfWeekSprite
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

enum class TimeSprite
{
    AFTER_SCHOOL_0_0 = 0,
    AFTER_SCHOOL_1_0,
    AFTER_SCHOOL_2_0,
    AFTERNOON_0_0,
    AFTERNOON_1_0,
    AFTERNOON_2_0,
    DAYTIME_0_0,
    DAYTIME_1_0,
    EARLY_MORNING_0_0,
    EARLY_MORNING_1_0,
    EARLY_MORNING_2_0,
    EARLY_MORNING_3_0,
    LATE_NIGHT_0_0,
    LATE_NIGHT_1_0,
    LATE_NIGHT_2_0,
    LUNCHTIME_0_0,
    LUNCHTIME_1_0,
    LUNCHTIME_2_0,
    MORNING_0_0,
    MORNING_1_0,
};

enum class SkillSprite
{
    SKILLS_LEVEL = 0
};

enum class DigitSprite
{
    DIGIT_0 = 0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,
    SLASH
};

enum class DialogueSprite
{
    NAME_TAG = 0,
    CALENDAR,
    TEXT_CORNER,
    TEXT_MIDDLE,
};

enum class MainMenuOptions
{
    LOAD_GAME = 0,
    SETTINGS,
    RETURN_TO_TITLE,
};

enum class LevelOptions
{
    START_GAME = 0,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    STATION,
    PAULOWNIA_MALL,
    SIGN_CONTRACT,
};

enum class SettingIntroOptions
{
    ORIGINAL = 0,
    FES,
    PORTABLE,
    RELOAD
};

enum class SettingOptions
{
    CHANGE_INTRO_VIDEO = 0,
};

enum class BattleMenuOptions
{
    NONE = 0,
    ACTION,
    SKILL,
    PERSONA,
    TARGET_ENEMY,
    TARGET_HEAL,
    ALL_OUT_ATTACK,
    ALERT
};

enum class PauseMenuOption
{
    SKILL = 0,
    ITEM = 1,
    PERSONA = 2,
    EQUIP = 3,
    STATUS = 4,
    S_LINK = 5,
    SYSTEM = 6
};

enum class Character
{
    MAKOTO = 0,
    YUKARI = 1,
    JUNPEI = 2,
    AKIHIKO = 3,
    MITSURU = 4,
    AIGIS = 5,
    KEN = 6,
    KOROMARU = 7,
    SHINJIRO = 8
};

enum class Item
{
    LIFE_STONE = 0,
    MEDICINE = 1,
    BEAD = 2
};

enum class Persona
{
    JACK_FROST = 0,
    BLACK_FROST = 1,
    KING_FROST = 2
};

enum class SLink
{
    FOOL = 0,
    MAGICIAN = 1,
    EMPEROR = 2
};

enum class SystemOption
{
    TUTORIAL = 0,
    CONFIG = 1,
    DICTIONARY = 2,
    LOAD_DATA = 3,
    SAVE_DATA = 4,
    RETURN_TO_TITLE = 5
};

enum class DebugOption
{
    DISCLAIMER_VIEW = 0,
    INTRO_VIEW,
    MAIN_MENU_VIEW,
    IWATODAI_DORM_VIEW,
    IWATODAI_STREETS_VIEW,
    STATION_VIEW,
    SIGN_CONTRACT_VIEW,
    PAULOWNIA_MALL_VIEW,
    INTRO_VIDEO,
    CUTSCENE_1,
    CUTSCENE_2,
    DEBUG_DIALOGUE,
    TOGGLE_BILLBOARDS,
    TOGGLE_DEBUG_PRINT,
    PLAY_CHARACTER_ANIM,
    CYCLE_CAMERA_MODE
};

enum class ViewPhase
{
    Battle,
    Pause,
    Dialogue,
    Environment
};
