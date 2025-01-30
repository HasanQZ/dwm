#include <X11/XF86keysym.h>
/* See LICENSE file for copyright and license details. */
/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */

static unsigned int enablegaps       = 1;
static unsigned int gappih     = 20;       /* horiz inner gap between windows */
static unsigned int gappiv     = 20;       /* vert inner gap between windows */
static unsigned int gappoh     = 20;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov     = 20;       /* vert outer gap between windows and screen edge */
static int smartgaps           = 0;        /* 1 means no outer gap when there is only one window */

static const int vertpad             = 0;       /* vertical padding for bar */
static const int sidepad             = 0;       /* side padding for bar */

static const unsigned int user_bh   = 17;       /* 0 means automatic bar height  */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char dmenufont[]       = "JetBrains Mono:size=12";
static const char *fonts[]          = { dmenufont,
                                           "Symbols Nerd Font:size=14:antialias=true:autohint=true" };

static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */


static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#000000";
static const char col_gray3[]       = "#ffffff";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#d3d3d3";

static const char s_base03[]        = "#002b36";
static const char s_base02[]        = "#073642";
static const char s_base01[]        = "#586e75";
static const char s_base00[]        = "#657b83";
static const char s_base0[]         = "#839496";
static const char s_base1[]         = "#93a1a1";
static const char s_base2[]         = "#eee8d5";
static const char s_base3[]         = "#fdf6e3";


static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { "#ffffff", "#000000", "#2e2e2e" },
	[SchemeSel]  = { "#ffffff", "#2e2e2e", "#ffffff" },
//	[SchemeNorm] = { s_base0, s_base03, "#000000" },
//	[SchemeSel]  = { s_base0, s_base02, s_base2 },
};

/* tagging */
static const char *tags[] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            0,           -1 },
        { "Shutter",  NULL,       NULL,       0,            1,           -1 }
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *termcmd[]  = { "alacritty" };
// static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-m", dmenumon, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
//static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-m", dmenumon, "-p", "λ:", "-nb", "#073642", "-nf", "#93a1a1", "-sb", "#002b36", "-sf", "#859900", NULL };
static const char *dmenucmd[] = { "rofi", "-monitor", "-4", "-show", "drun", NULL };

/* Control Media Players */
static const char player_name[] = "spotify";

static const char *upvol[]   = { "/usr/bin/amixer", "sset", "'Master'", "5%+",     NULL };
static const char *downvol[] = { "/usr/bin/amixer", "sset", "'Master'", "5%-",     NULL };
static const char *mutevol[] = { "/usr/bin/amixer", "sset", "'Master'", "toggle",  NULL };

static const char *player_upvol[]   = { "playerctl", "-p", player_name, "volume", "0.05+", NULL };
static const char *player_downvol[] = { "playerctl", "-p", player_name, "volume", "0.05-", NULL };
/* no player mute toggle (why would you want to leave music playing muted anyways?) */
static const char *player_playpause[] = { "playerctl", "-p", player_name, "play-pause", NULL };

static const char *medplaypausecmd[] = { "playerctl", "play-pause", NULL };
static const char *mednextcmd[] = { "playerctl", "-p", player_name, "next", NULL };
static const char *medprevcmd[] = { "playerctl", "-p", player_name, "previous", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_e,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_i,      focusstack,     {.i = -1 } },

	{ MODKEY|ShiftMask,             XK_m,      incnmaster,     {.i = +1 } },
        { MODKEY|ControlMask,           XK_m,      incnmaster,     {.i = -1 } },

        { MODKEY,                       XK_l,      pushdown,        {0} },
        { MODKEY,                       XK_u,      pushup,          {0} },

  	{ 0,                            XF86XK_AudioLowerVolume,   spawn,   {.v = downvol } },
	{ 0,                            XF86XK_AudioMute,          spawn,   {.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,   {.v = upvol   } },

  	{ ShiftMask,                    XF86XK_AudioLowerVolume,   spawn,   {.v = player_downvol } },
	{ ShiftMask,                    XF86XK_AudioMute,          spawn,   {.v = player_playpause } },
	{ ShiftMask,                    XF86XK_AudioRaiseVolume,   spawn,   {.v = player_upvol   } },
        { ShiftMask,                    XF86XK_AudioPlay,          spawn,   {.v = player_playpause} },

        { 0,                            XF86XK_AudioPlay,          spawn,   {.v = medplaypausecmd } },
        { 0,                            XF86XK_AudioNext,          spawn,   {.v = mednextcmd } },
        { 0,                            XF86XK_AudioPrev,          spawn,   {.v = medprevcmd } },

        /*
        { MODKEY,                       XK_y,      spawn,          SHCMD("cmus-remote -u") },
        { MODKEY,                       XK_minus,  spawn,          SHCMD("cmus-remote -v -10%") },
        { MODKEY,                       XK_equal,  spawn,          SHCMD("cmus-remote -v +10%") },
        { MODKEY|ShiftMask,             XK_minus,  spawn,          SHCMD("cmus-remote --prev") },
        { MODKEY|ShiftMask,             XK_equal,  spawn,          SHCMD("cmus-remote --next") },
        */

	{ MODKEY,                       XK_d,	   togglegaps,	   {0} },


	{ MODKEY,                       XK_w,      spawn,          SHCMD("firefox") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("firefox -private-window") },

        { MODKEY|ShiftMask,             XK_i,      spawn,          SHCMD("slock") },
        { MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("shutter") },
        { 0,                            XK_Print,  spawn,          SHCMD("shutter") },

	{ MODKEY,                       XK_semicolon,      spawn,          SHCMD("thunar") },
	{ MODKEY,                       XK_t,      togglefullscr,  {0} },

	{ MODKEY,                       XK_s,           spawn,      {.v = dmenucmd} },
	{ 0,                            XF86XK_Search,  spawn,      {.v = dmenucmd} },
	{ MODKEY,                       XK_n,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_o,      setmfact, {.f = +0.05} },
	{ MODKEY,                       XK_m,      zoom,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ControlMask,           XK_f,      setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_t,      togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_slash,  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_slash,  tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	//{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	//{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

