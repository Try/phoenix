// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "../DaedalusScript.hh"

#define var
#define string std::string
#define func int32_t

// clang-format off
#define FLAG(E)                                                                                                        \
	inline E operator|(E a, E b) {                                                                                     \
		return static_cast<E>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));                          \
	}                                                                                                                  \
	inline bool operator&(E a, E b) { return (static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)) != 0; }
// clang-format on

namespace phoenix {
	struct c_gil_values : public DaedalusInstance {
		static constexpr std::uint32_t count = 66;

		var int32_t water_depth_knee[count];
		var int32_t water_depth_chest[count];
		var int32_t jumpup_height[count];
		var int32_t swim_time[count];
		var int32_t dive_time[count];
		var int32_t step_height[count];
		var int32_t jumplow_height[count];
		var int32_t jumpmid_height[count];
		var int32_t slide_angle[count];
		var int32_t slide_angle2[count];
		var int32_t disable_autoroll[count];
		var int32_t surface_align[count];
		var int32_t climb_heading_angle[count];
		var int32_t climb_horiz_angle[count];
		var int32_t climb_ground_angle[count];
		var int32_t fight_range_base[count];
		var int32_t fight_range_fist[count];
		var int32_t fight_range_g[count];
		var int32_t fight_range_1hs[count];
		var int32_t fight_range_1ha[count];
		var int32_t fight_range_2hs[count];
		var int32_t fight_range_2ha[count];
		var int32_t falldown_height[count];
		var int32_t falldown_damage[count];
		var int32_t blood_disabled[count];
		var int32_t blood_max_distance[count];
		var int32_t blood_amount[count];
		var int32_t blood_flow[count];
		var string blood_emitter[count];
		var string blood_texture[count];
		var int32_t turn_speed[count];

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	namespace damage_type {
		static constexpr std::uint32_t barrier = 0U;
		static constexpr std::uint32_t blunt = 1U;
		static constexpr std::uint32_t edge = 2U;
		static constexpr std::uint32_t fire = 3U;
		static constexpr std::uint32_t fly = 4U;
		static constexpr std::uint32_t magic = 5U;
		static constexpr std::uint32_t point = 6U;
		static constexpr std::uint32_t fall = 7U;
		static constexpr std::uint32_t count = 8U;
	} // namespace damage_type

	namespace npc_attribute {
		static constexpr std::uint32_t hitpoints = 0U;
		static constexpr std::uint32_t hitpoints_max = 1U;
		static constexpr std::uint32_t mana = 2U;
		static constexpr std::uint32_t mana_max = 3U;
		static constexpr std::uint32_t strength = 4U;
		static constexpr std::uint32_t dexterity = 5U;
		static constexpr std::uint32_t regenerate_hp = 6U;
		static constexpr std::uint32_t regenerate_mana = 7U;
		static constexpr std::uint32_t count = 8U;
	} // namespace npc_attribute

	enum class npc_type : std::uint32_t {
		g1_ambient = 0U,
		g1_main = 1U,
		g1_guard = 2U,
		g1_friend = 3U,
		g1_mine_ambient = 4U,
		g1_mine_guard = 5U,
		g1_ow_ambient = 6U,
		g1_ow_guard = 7U,
		g1_rogue = 8U,

		g2_ambient = 0U,
		g2_main = 1U,
		g2_friend = 2U,
		g2_oc_ambient = 3U,
		g2_oc_main = 4U,
		g2_bl_ambient = 5U,
		g2_tal_ambient = 6U,
		g2_bl_main = 7U,

		ambient = g2_ambient,
		main = g2_main,

		friend_ PHOENIX_DEPRECATED("use npc_type::g2_friend") = g2_friend,
		oc_ambient PHOENIX_DEPRECATED("use npc_type::g2_oc_ambient") = g2_oc_ambient,
		oc_main PHOENIX_DEPRECATED("use npc_type::g2_oc_main") = g2_oc_main,
		bl_ambient PHOENIX_DEPRECATED("use npc_type::g2_bl_ambient") = g2_bl_ambient,
		tal_ambient PHOENIX_DEPRECATED("use npc_type::g2_tal_ambient") = g2_tal_ambient,
		bl_main PHOENIX_DEPRECATED("use npc_type::g2_bl_main") = g2_bl_main,
	};

	static_assert(npc_type::g1_ambient == npc_type::g2_ambient);
	static_assert(npc_type::g1_main == npc_type::g2_main);

	enum class npc_flag : std::uint32_t {
		none = 0U,
		friends = 1U << 0U,
		immortal = 1U << 1U,
		ghost = 1U << 2U,
		protected_ = 1U << 3U,
	};
	FLAG(npc_flag)

	struct c_npc : public DaedalusInstance {
		static constexpr std::uint32_t hitchance_count =
		    5; // -> TODO: one of "unknown", "one-handed", "two-handed", "bow", "crossbow"
		static constexpr std::uint32_t name_count = 5;
		static constexpr std::uint32_t mission_count = 5;
		static constexpr std::uint32_t aivar_count = 100;

		var int32_t id;
		var string name[name_count];
		var string slot;
		var string effect;
		var npc_type type;
		var npc_flag flags;
		var int32_t attribute[npc_attribute::count];
		var int32_t hitchance[hitchance_count];
		var int32_t protection[damage_type::count];
		var int32_t damage[damage_type::count];
		var int32_t damage_type;
		var int32_t guild;
		var int32_t level;
		var func mission[mission_count];
		var int32_t fight_tactic;
		var int32_t weapon;
		var int32_t voice;
		var int32_t voice_pitch;
		var int32_t body_mass;
		var func daily_routine;
		var func start_aistate;
		var string spawnpoint;
		var int32_t spawn_delay;
		var int32_t senses;
		var int32_t senses_range;
		var int32_t aivar[aivar_count];
		var string wp;
		var int32_t exp;
		var int32_t exp_next;
		var int32_t lp;
		var int32_t bodystate_interruptable_override;
		var int32_t no_focus;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_mission : public DaedalusInstance {
		var string name;
		var string description;
		var int32_t duration;
		var int32_t important;
		var func offer_conditions;
		var func offer;
		var func success_conditions;
		var func success;
		var func failure_conditions;
		var func failure;
		var func obsolete_conditions;
		var func obsolete;
		var func running;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	enum class item_flags : std::uint32_t {
		dagger = 1U << 13U,
		sword = 1U << 14U,
		axe = 1U << 15U,
		two_handed_sword = 1U << 16U,
		two_handed_axe = 1U << 17U,
		bow = 1U << 19U,
		crossbow = 1U << 20U,
		amulet = 1U << 22U,
		ring = 1U << 11U,
		belt = 1U << 24U,
		mission = 1U << 12U,
	};
	FLAG(item_flags)

	struct c_item : public DaedalusInstance {
		static constexpr std::uint32_t condition_count = 3;
		static constexpr std::uint32_t state_count = 4;

		var int32_t id;
		var string name;
		var string name_id;
		var int32_t hp;
		var int32_t hp_max;
		var int32_t main_flag;
		var item_flags flags;
		var int32_t weight;
		var int32_t value;
		var int32_t damage_type;
		var int32_t damage_total;
		var int32_t damage[damage_type::count];
		var int32_t wear;
		var int32_t protection[damage_type::count];
		var int32_t nutrition;
		var int32_t cond_atr[condition_count];
		var int32_t cond_value[condition_count];
		var int32_t change_atr[condition_count];
		var int32_t change_value[condition_count];
		var func magic;
		var func on_equip;
		var func on_unequip;
		var func on_state[state_count];
		var func owner;
		var int32_t owner_guild;
		var int32_t disguise_guild;
		var string visual;
		var string visual_change;
		var string effect;
		var int32_t visual_skin;
		var string scheme_name;
		var int32_t material;
		var int32_t munition;
		var int32_t spell;
		var int32_t range;
		var int32_t mag_circle;
		var string description;
		var string text[6];
		var int32_t count[6];
		var int32_t inv_zbias;
		var int32_t inv_rot_x;
		var int32_t inv_rot_y;
		var int32_t inv_rot_z;
		var int32_t inv_animate;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_focus : public DaedalusInstance {
		var float npc_longrange;
		var float npc_range1;
		var float npc_range2;
		var float npc_azi;
		var float npc_elevdo;
		var float npc_elevup;
		var int32_t npc_prio;
		var float item_range1;
		var float item_range2;
		var float item_azi;
		var float item_elevdo;
		var float item_elevup;
		var int32_t item_prio;
		var float mob_range1;
		var float mob_range2;
		var float mob_azi;
		var float mob_elevdo;
		var float mob_elevup;
		var int32_t mob_prio;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_info_choice {
		var string text;
		var int32_t function = 0;
	};

	struct c_info : public DaedalusInstance {
		var int32_t npc;
		var int32_t nr;
		var int32_t important;
		var func condition;
		var func information;
		var string description;
		var int32_t trade;
		var int32_t permanent;

		std::vector<c_info_choice> choices {};

		PHOENIX_API void add_choice(const c_info_choice& ch) {
			choices.insert(choices.begin(), ch);
		}

		PHOENIX_API void remove_choice(std::size_t index) {
			choices.erase(choices.begin() + static_cast<decltype(choices)::difference_type>(index));
		}

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_item_react : public DaedalusInstance {
		var int32_t npc;
		var int32_t trade_item;
		var int32_t trade_amount;
		var int32_t requested_cat;
		var int32_t requested_item;
		var int32_t requested_amount;
		var func reaction;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_spell : public DaedalusInstance {
		var float time_per_mana;
		var int32_t damage_per_level;
		var int32_t damage_type;
		var int32_t spell_type;
		var int32_t can_turn_during_invest;
		var int32_t can_change_target_during_invest;
		var int32_t is_multi_effect;
		var int32_t target_collect_algo;
		var int32_t target_collect_type;
		var int32_t target_collect_range;
		var int32_t target_collect_azi;
		var int32_t target_collect_elev;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_svm : public DaedalusInstance {
		var string MILGREETINGS;
		var string PALGREETINGS;
		var string WEATHER;
		var string IGETYOUSTILL;
		var string DIEENEMY;
		var string DIEMONSTER;
		var string ADDON_DIEMONSTER;
		var string ADDON_DIEMONSTER2;
		var string DIRTYTHIEF;
		var string HANDSOFF;
		var string SHEEPKILLER;
		var string SHEEPKILLERMONSTER;
		var string YOUMURDERER;
		var string DIESTUPIDBEAST;
		var string YOUDAREHITME;
		var string YOUASKEDFORIT;
		var string THENIBEATYOUOUTOFHERE;
		var string WHATDIDYOUDOINTHERE;
		var string WILLYOUSTOPFIGHTING;
		var string KILLENEMY;
		var string ENEMYKILLED;
		var string MONSTERKILLED;
		var string ADDON_MONSTERKILLED;
		var string ADDON_MONSTERKILLED2;
		var string THIEFDOWN;
		var string RUMFUMMLERDOWN;
		var string SHEEPATTACKERDOWN;
		var string KILLMURDERER;
		var string STUPIDBEASTKILLED;
		var string NEVERHITMEAGAIN;
		var string YOUBETTERSHOULDHAVELISTENED;
		var string GETUPANDBEGONE;
		var string NEVERENTERROOMAGAIN;
		var string THEREISNOFIGHTINGHERE;
		var string SPAREME;
		var string RUNAWAY;
		var string ALARM;
		var string GUARDS;
		var string HELP;
		var string GOODMONSTERKILL;
		var string GOODKILL;
		var string NOTNOW;
		var string RUNCOWARD;
		var string GETOUTOFHERE;
		var string WHYAREYOUINHERE;
		var string YESGOOUTOFHERE;
		var string WHATSTHISSUPPOSEDTOBE;
		var string YOUDISTURBEDMYSLUMBER;
		var string ITOOKYOURGOLD;
		var string SHITNOGOLD;
		var string ITAKEYOURWEAPON;
		var string WHATAREYOUDOING;
		var string LOOKINGFORTROUBLEAGAIN;
		var string STOPMAGIC;
		var string ISAIDSTOPMAGIC;
		var string WEAPONDOWN;
		var string ISAIDWEAPONDOWN;
		var string WISEMOVE;
		var string NEXTTIMEYOUREINFORIT;
		var string OHMYHEAD;
		var string THERESAFIGHT;
		var string OHMYGODITSAFIGHT;
		var string GOODVICTORY;
		var string NOTBAD;
		var string OHMYGODHESDOWN;
		var string CHEERFRIEND01;
		var string CHEERFRIEND02;
		var string CHEERFRIEND03;
		var string OOH01;
		var string OOH02;
		var string OOH03;
		var string WHATWASTHAT;
		var string GETOUTOFMYBED;
		var string AWAKE;
		var string ABS_COMMANDER;
		var string ABS_MONASTERY;
		var string ABS_FARM;
		var string ABS_GOOD;
		var string SHEEPKILLER_CRIME;
		var string ATTACK_CRIME;
		var string THEFT_CRIME;
		var string MURDER_CRIME;
		var string PAL_CITY_CRIME;
		var string MIL_CITY_CRIME;
		var string CITY_CRIME;
		var string MONA_CRIME;
		var string FARM_CRIME;
		var string OC_CRIME;
		var string TOUGHGUY_ATTACKLOST;
		var string TOUGHGUY_ATTACKWON;
		var string TOUGHGUY_PLAYERATTACK;
		var string GOLD_1000;
		var string GOLD_950;
		var string GOLD_900;
		var string GOLD_850;
		var string GOLD_800;
		var string GOLD_750;
		var string GOLD_700;
		var string GOLD_650;
		var string GOLD_600;
		var string GOLD_550;
		var string GOLD_500;
		var string GOLD_450;
		var string GOLD_400;
		var string GOLD_350;
		var string GOLD_300;
		var string GOLD_250;
		var string GOLD_200;
		var string GOLD_150;
		var string GOLD_100;
		var string GOLD_90;
		var string GOLD_80;
		var string GOLD_70;
		var string GOLD_60;
		var string GOLD_50;
		var string GOLD_40;
		var string GOLD_30;
		var string GOLD_20;
		var string GOLD_10;
		var string SMALLTALK01;
		var string SMALLTALK02;
		var string SMALLTALK03;
		var string SMALLTALK04;
		var string SMALLTALK05;
		var string SMALLTALK06;
		var string SMALLTALK07;
		var string SMALLTALK08;
		var string SMALLTALK09;
		var string SMALLTALK10;
		var string SMALLTALK11;
		var string SMALLTALK12;
		var string SMALLTALK13;
		var string SMALLTALK14;
		var string SMALLTALK15;
		var string SMALLTALK16;
		var string SMALLTALK17;
		var string SMALLTALK18;
		var string SMALLTALK19;
		var string SMALLTALK20;
		var string SMALLTALK21;
		var string SMALLTALK22;
		var string SMALLTALK23;
		var string SMALLTALK24;
		var string SMALLTALK25;
		var string SMALLTALK26;
		var string SMALLTALK27;
		var string SMALLTALK28;
		var string SMALLTALK29;
		var string SMALLTALK30;
		var string NOLEARNNOPOINTS;
		var string NOLEARNOVERPERSONALMAX;
		var string NOLEARNYOUREBETTER;
		var string YOULEARNEDSOMETHING;
		var string UNTERSTADT;
		var string OBERSTADT;
		var string TEMPEL;
		var string MARKT;
		var string GALGEN;
		var string KASERNE;
		var string HAFEN;
		var string WHERETO;
		var string OBERSTADT_2_UNTERSTADT;
		var string UNTERSTADT_2_OBERSTADT;
		var string UNTERSTADT_2_TEMPEL;
		var string UNTERSTADT_2_HAFEN;
		var string TEMPEL_2_UNTERSTADT;
		var string TEMPEL_2_MARKT;
		var string TEMPEL_2_GALGEN;
		var string MARKT_2_TEMPEL;
		var string MARKT_2_KASERNE;
		var string MARKT_2_GALGEN;
		var string GALGEN_2_TEMPEL;
		var string GALGEN_2_MARKT;
		var string GALGEN_2_KASERNE;
		var string KASERNE_2_MARKT;
		var string KASERNE_2_GALGEN;
		var string HAFEN_2_UNTERSTADT;
		var string DEAD;
		var string AARGH_1;
		var string AARGH_2;
		var string AARGH_3;
		var string ADDON_WRONGARMOR;
		var string ADDON_WRONGARMOR_SLD;
		var string ADDON_WRONGARMOR_MIL;
		var string ADDON_WRONGARMOR_KDF;
		var string ADDON_NOARMOR_BDT;
		var string ADDON_DIEBANDIT;
		var string ADDON_DIRTYPIRATE;
		var string SC_HEYTURNAROUND;
		var string SC_HEYTURNAROUND02;
		var string SC_HEYTURNAROUND03;
		var string SC_HEYTURNAROUND04;
		var string SC_HEYWAITASECOND;
		var string DOESNTWORK;
		var string PICKBROKE;
		var string NEEDKEY;
		var string NOMOREPICKS;
		var string NOPICKLOCKTALENT;
		var string NOSWEEPING;
		var string PICKLOCKORKEYMISSING;
		var string KEYMISSING;
		var string PICKLOCKMISSING;
		var string NEVEROPEN;
		var string MISSINGITEM;
		var string DONTKNOW;
		var string NOTHINGTOGET;
		var string NOTHINGTOGET02;
		var string NOTHINGTOGET03;
		var string HEALSHRINE;
		var string HEALLASTSHRINE;
		var string IRDORATHTHEREYOUARE;
		var string SCOPENSIRDORATHBOOK;
		var string SCOPENSLASTDOOR;
		var string TRADE_1;
		var string TRADE_2;
		var string TRADE_3;
		var string VERSTEHE;
		var string FOUNDTREASURE;
		var string CANTUNDERSTANDTHIS;
		var string CANTREADTHIS;
		var string STONEPLATE_1;
		var string STONEPLATE_2;
		var string STONEPLATE_3;
		var string COUGH;
		var string HUI;
		var string ADDON_THISLITTLEBASTARD;
		var string ADDON_OPENADANOSTEMPLE;
		var string ATTENTAT_ADDON_DESCRIPTION;
		var string ATTENTAT_ADDON_DESCRIPTION2;
		var string ATTENTAT_ADDON_PRO;
		var string ATTENTAT_ADDON_CONTRA;
		var string MINE_ADDON_DESCRIPTION;
		var string ADDON_SUMMONANCIENTGHOST;
		var string ADDON_ANCIENTGHOST_NOTNEAR;
		var string ADDON_GOLD_DESCRIPTION;

		var string WATCHYOURAIM;
		var string watchyouraimangry;
		var string letsforgetourlittlefight;
		var string strange;
		var string diemortalenemy;
		var string nowwait;
		var string nowwaitintruder;
		var string youstillnothaveenough;
		var string youattackedmycharge;
		var string iwillteachyourespectforforeignproperty;
		var string youkilledoneofus;
		var string berzerk;
		var string youllbesorryforthis;
		var string yesyes;
		var string shitwhatamonster;
		var string wewillmeetagain;
		var string nevertrythatagain;
		var string itookyourore;
		var string shitnoore;
		var string youviolatedforbiddenterritory;
		var string youwannafoolme;
		var string whatdidyouinthere;
		var string intruderalert;
		var string behindyou;
		var string heyheyhey;
		var string cheerfight;
		var string cheerfriend;
		var string ooh;
		var string yeahwelldone;
		var string hedefeatedhim;
		var string hedeservedit;
		var string hekilledhim;
		var string itwasagoodfight;
		var string friendlygreetings;
		var string algreetings;
		var string magegreetings;
		var string sectgreetings;
		var string thereheis;
		var string nolearnovermax;
		var string nolearnyoualreadyknow;
		var string heyyou;
		var string whatdoyouwant;
		var string isaidwhatdoyouwant;
		var string makeway;
		var string outofmyway;
		var string youdeaforwhat;
		var string lookaway;
		var string okaykeepit;
		var string whatsthat;
		var string thatsmyweapon;
		var string giveittome;
		var string youcankeepthecrap;
		var string theykilledmyfriend;
		var string suckergotsome;
		var string suckerdefeatedebr;
		var string suckerdefeatedgur;
		var string suckerdefeatedmage;
		var string suckerdefeatednov_guard;
		var string suckerdefeatedvlk_guard;
		var string youdefeatedmycomrade;
		var string youdefeatednov_guard;
		var string youdefeatedvlk_guard;
		var string youstolefromme;
		var string youstolefromus;
		var string youstolefromebr;
		var string youstolefromgur;
		var string stolefrommage;
		var string youkilledmyfriend;
		var string youkilledebr;
		var string youkilledgur;
		var string youkilledmage;
		var string youkilledocfolk;
		var string youkilledncfolk;
		var string youkilledpsifolk;
		var string getthingsright;
		var string youdefeatedmewell;
		var string om;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	enum class c_menu_flags : std::uint32_t {
		overtop = 1 << 0,
		exclusive = 1 << 1,
		no_animation = 1 << 2,
		dont_scale_dimension = 1 << 3,
		dont_scale_position = 1 << 4,
		align_center = 1 << 5,
		show_info = 1 << 6,
	};
	FLAG(c_menu_flags)

	struct c_menu : public DaedalusInstance {
		static constexpr std::uint8_t item_count = 150;

		var string back_pic;
		var string back_world;
		var int32_t pos_x;
		var int32_t pos_y;
		var int32_t dim_x;
		var int32_t dim_y;
		var int32_t alpha;
		var string music_theme;
		var int32_t event_timer_msec;
		var string items[item_count];
		var c_menu_flags flags;
		var int32_t default_outgame;
		var int32_t default_ingame;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	enum class c_menu_item_flags : std::uint32_t {
		chromakeyed = 1 << 0,
		transparent = 1 << 1,
		selectable = 1 << 2,
		movable = 1 << 3,
		centered = 1 << 4,
		disabled = 1 << 5,
		fade = 1 << 6,
		effects = 1 << 7,
		only_outgame = 1 << 8,
		only_ingame = 1 << 9,
		perf_option = 1 << 10,
		multiline = 1 << 11,
		needs_apply = 1 << 12,
		needs_restart = 1 << 13,
		extended_menu = 1 << 14,
	};
	FLAG(c_menu_item_flags)

	enum class c_menu_item_type : std::uint32_t {
		unknown = 0,
		text = 1,
		slider = 2,
		input = 3,
		cursor = 4,
		choicebox = 5,
		button = 6,
		listbox = 7,
	};

	enum class c_menu_item_select_event : uint8_t {
		execute = 1,
		changed = 2,
		leave = 3,
		timer = 4,
		close = 5,
		init = 6,
		select_previous = 7,
		select_next = 8,
	};

	enum class c_menu_item_select_action : uint8_t {
		unknown = 0,
		back = 1,
		start_menu = 2,
		start_item = 3,
		close = 4,
		con_commands = 5,
		play_sound = 6,
		execute_commands = 7,
	};

	struct c_menu_item : public DaedalusInstance {
		static constexpr std::uint32_t text_count = 10;
		static constexpr std::uint32_t select_action_count = 5;
		static constexpr std::uint32_t event_action_count = 10;
		static constexpr std::uint32_t user_item_count = 4;

		var string fontname;
		var string text[text_count];
		var string backpic;
		var string alphamode;
		var int32_t alpha;
		var c_menu_item_type type;
		var int32_t on_sel_action[select_action_count];
		var string on_sel_action_s[select_action_count];
		var string on_chg_set_option;
		var string on_chg_set_option_section;
		var func on_event_action[event_action_count];
		var int32_t pos_x;
		var int32_t pos_y;
		var int32_t dim_x;
		var int32_t dim_y;
		var float size_start_scale;
		var c_menu_item_flags flags;
		var float open_delay_time;
		var float open_duration;
		var float user_float[user_item_count];
		var string user_string[user_item_count];
		var int32_t frame_sizex;
		var int32_t frame_sizey;
		var string hide_if_option_section_set;
		var string hide_if_option_set;
		var int32_t hide_on_value;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_camera : public DaedalusInstance {
		var float best_range;
		var float min_range;
		var float max_range;
		var float best_elevation;
		var float min_elevation;
		var float max_elevation;
		var float best_azimuth;
		var float min_azimuth;
		var float max_azimuth;
		var float best_rot_z;
		var float min_rot_z;
		var float max_rot_z;
		var float rot_offset_x;
		var float rot_offset_y;
		var float rot_offset_z;
		var float target_offset_x;
		var float target_offset_y;
		var float target_offset_z;
		var float velo_trans;
		var float velo_rot;
		var int32_t translate;
		var int32_t rotate;
		var int32_t collision;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	enum class music_transition_type : std::uint32_t {
		unknown = 0,
		none = 1,
		groove = 2,
		fill = 3,
		break_ = 4,
		intro = 5,
		end = 6,
		end_and_into = 7
	};

	enum class music_transition_subtype : std::uint32_t { unknown = 0, immediate = 1, beat = 2, measure = 3 };

	struct c_music_system : public DaedalusInstance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t global_reverb_enabled;
		var int32_t sample_rate;
		var int32_t num_channels;
		var int32_t reverb_buffer_size;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_music_theme : public DaedalusInstance {
		var string file;
		var float vol;
		var int32_t loop;
		var float reverbmix;
		var float reverbtime;
		var music_transition_type transtype;
		var music_transition_subtype transsubtype;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_music_jingle : public DaedalusInstance {
		var string name;
		var int32_t loop;
		var float vol;
		var int32_t transsubtype;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_particle_fx : public DaedalusInstance {
		var float pps_value;
		var string pps_scale_keys_s;
		var int32_t pps_is_looping;
		var int32_t pps_is_smooth;
		var float pps_fps;
		var string pps_create_em_s;
		var float pps_create_em_delay;
		var string shp_type_s;
		var string shp_for_s;
		var string shp_offset_vec_s;
		var string shp_distrib_type_s;
		var float shp_distrib_walk_speed;
		var int32_t shp_is_volume;
		var string shp_dim_s;
		var string shp_mesh_s;
		var int32_t shp_mesh_render_b;
		var string shp_scale_keys_s;
		var int32_t shp_scale_is_looping;
		var int32_t shp_scale_is_smooth;
		var float shp_scale_fps;
		var string dir_mode_s;
		var string dir_for_s;
		var string dir_mode_target_for_s;
		var string dir_mode_target_pos_s;
		var float dir_angle_head;
		var float dir_angle_head_var;
		var float dir_angle_elev;
		var float dir_angle_elev_var;
		var float vel_avg;
		var float vel_var;
		var float lsp_part_avg;
		var float lsp_part_var;
		var string fly_gravity_s;
		var int32_t fly_colldet_b;
		var string vis_name_s;
		var string vis_orientation_s;
		var int32_t vis_tex_is_quadpoly;
		var float vis_tex_ani_fps;
		var int32_t vis_tex_ani_is_looping;
		var string vis_tex_color_start_s;
		var string vis_tex_color_end_s;
		var string vis_size_start_s;
		var float vis_size_end_scale;
		var string vis_alpha_func_s;
		var float vis_alpha_start;
		var float vis_alpha_end;
		var float trl_fade_speed;
		var string trl_texture_s;
		var float trl_width;
		var float mrk_fades_peed;
		var string mrkt_exture_s;
		var float mrk_size;
		var string flock_mode;
		var float flock_strength;
		var int32_t use_emitters_for;
		var string time_start_end_s;
		var int32_t m_bis_ambient_pfx;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_fx_base : public DaedalusInstance {
		static constexpr std::uint8_t user_string_count = 5;

		var string vis_name_s;
		var string vis_size_s;
		var float vis_alpha;
		var string vis_alpha_blend_func_s;
		var float vis_tex_ani_fps;
		var int32_t vis_tex_ani_is_looping;
		var string em_trj_mode_s;
		var string em_trj_origin_node;
		var string em_trj_target_node;
		var float em_trj_target_range;
		var float em_trj_target_azi;
		var float em_trj_target_elev;
		var int32_t em_trj_num_keys;
		var int32_t em_trj_num_keys_var;
		var float em_trj_angle_elev_var;
		var float em_trj_angle_head_var;
		var float em_trj_key_dist_var;
		var string em_trj_loop_mode_s;
		var string em_trj_ease_func_s;
		var float em_trj_ease_vel;
		var float em_trj_dyn_update_delay;
		var int32_t em_trj_dyn_update_target_only;
		var string em_fx_create_s;
		var string em_fx_invest_origin_s;
		var string em_fx_invest_target_s;
		var float em_fx_trigger_delay;
		var int32_t em_fx_create_down_trj;
		var string em_action_coll_dyn_s;
		var string em_action_coll_stat_s;
		var string em_fx_coll_stat_s;
		var string em_fx_coll_dyn_s;
		var string em_fx_coll_stat_align_s;
		var string em_fx_coll_dyn_align_s;
		var float em_fx_lifespan;
		var int32_t em_check_collision;
		var int32_t em_adjust_shp_to_origin;
		var float em_invest_next_key_duration;
		var float em_fly_gravity;
		var string em_self_rot_vel_s;
		var string user_string[user_string_count];
		var string light_preset_name;
		var string sfx_id;
		var int32_t sfx_is_ambient;
		var int32_t send_assess_magic;
		var float secs_per_damage;
		var string em_fx_coll_dyn_perc_s;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_particle_fx_emit_key : public DaedalusInstance {
		var string vis_name_s;
		var float vis_size_scale;
		var float scale_duration;
		var float pfx_pps_value;
		var int32_t pfx_pps_is_smooth_chg;
		var int32_t pfx_pps_is_looping_chg;
		var float pfx_sc_time;
		var string pfx_fly_gravity_s;
		var string pfx_shp_dim_s;
		var int32_t pfx_shp_is_volume_chg;
		var float pfx_shp_scale_fps;
		var float pfx_shp_distrib_walks_peed;
		var string pfx_shp_offset_vec_s;
		var string pfx_shp_distrib_type_s;
		var string pfx_dir_mode_s;
		var string pfx_dir_for_s;
		var string pfx_dir_mode_target_for_s;
		var string pfx_dir_mode_target_pos_s;
		var float pfx_vel_avg;
		var float pfx_lsp_part_avg;
		var float pfx_vis_alpha_start;
		var string light_preset_name;
		var float light_range;
		var string sfx_id;
		var int32_t sfx_is_ambient;
		var string em_create_fx_id;
		var float em_fly_gravity;
		var string em_self_rot_vel_s;
		var string em_trj_mode_s;
		var float em_trj_ease_vel;
		var int32_t em_check_collision;
		var float em_fx_lifespan;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	enum class c_fight_ai_move : std::uint32_t {
		nop = 0,
		run = 1,
		run_back = 2,
		jump_back = 3,
		turn = 4,
		strafe = 5,
		attack = 6,
		attack_side = 7,
		attack_front = 8,
		attack_triple = 9,
		attack_whirl = 10,
		attack_master = 11,
		turn_to_hit = 15,
		parry = 17,
		stand_up = 18,
		wait = 19,
		wait_longer = 23,
		wait_ext = 24,
	};

	struct c_fight_ai : public DaedalusInstance {
		static constexpr std::uint32_t move_count = 6;

		var c_fight_ai_move move[move_count];

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_sfx : public DaedalusInstance {
		var string file;
		var int32_t pitch_off;
		var int32_t pitch_var;
		var int32_t vol;
		var int32_t loop;
		var int32_t loop_start_offset;
		var int32_t loop_end_offset;
		var float reverb_level;
		var string pfx_name;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	struct c_sound_system : public DaedalusInstance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t sample_rate;
		var int32_t use_stereo;
		var int32_t num_sfx_channels;
		var string used_3d_provider_name;

		PHOENIX_API static void register_(DaedalusScript& s);
	};

	PHOENIX_API void register_all_script_classes(DaedalusScript& s);
} // namespace phoenix

#undef var
#undef string
#undef func
