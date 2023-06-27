#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#define MAX_TEAMNAME		32
#define	MAX_QPATH		64
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token
#define	MAX_MODELS			256		// these are sent over the net as 8 bits
#define	MAX_SOUNDS			256		// so they cannot be blindly increased
#define	MAX_CLIENTS			64		// absolute limit
#define MAX_LOCATIONS		64
#define	TEAMCHAT_WIDTH		80
#define TEAMCHAT_HEIGHT		8
#define	NUM_CROSSHAIRS		10
#define	MAX_NAME_LENGTH		32		// max length of a client name

typedef enum {qfalse, qtrue}	qboolean;
typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef vec_t quat_t[4];

typedef int		qhandle_t;
typedef int		sfxHandle_t;
typedef int		fileHandle_t;
typedef int		clipHandle_t;

typedef unsigned char 		byte;

typedef enum {
    TEAM_FREE,
    TEAM_RED,
    TEAM_BLUE,
    TEAM_SPECTATOR,

    TEAM_NUM_TEAMS
} team_t;

typedef enum {
    GT_FFA,				// free for all
    GT_TOURNAMENT,		// one on one tournament
    GT_SINGLE_PLAYER,	// single player ffa

    //-- team games go after this --

    GT_TEAM,			// team deathmatch
    GT_CTF,				// capture the flag
    GT_1FCTF,
    GT_OBELISK,
    GT_HARVESTER,
    GT_MAX_GAME_TYPE
} gametype_t;

typedef enum {
    GLDRV_ICD,					// driver is integrated with window system
                                // WARNING: there are tests that check for
                                // > GLDRV_ICD for minidriverness, so this
                                // should always be the lowest value in this
                                // enum set
    GLDRV_STANDALONE,			// driver is a non-3Dfx standalone driver
    GLDRV_VOODOO				// driver is a 3Dfx standalone driver
} glDriverType_t;

typedef enum {
    GLHW_GENERIC,			// where everything works the way it should
    GLHW_3DFX_2D3D,			// Voodoo Banshee or Voodoo3, relevant since if this is
                            // the hardware type then there can NOT exist a secondary
                            // display adapter
    GLHW_RIVA128,			// where you can't interpolate alpha
    GLHW_RAGEPRO,			// where you can't modulate alpha on alpha textures
    GLHW_PERMEDIA2			// where you don't have src*dst
} glHardwareType_t;

typedef enum {
    TC_NONE,
    TC_S3TC,  // this is for the GL_S3_s3tc extension.
    TC_S3TC_ARB  // this is for the GL_EXT_texture_compression_s3tc extension.
} textureCompression_t;

typedef enum {
    BOTH_DEATH1,
    BOTH_DEAD1,
    BOTH_DEATH2,
    BOTH_DEAD2,
    BOTH_DEATH3,
    BOTH_DEAD3,

    TORSO_GESTURE,

    TORSO_ATTACK,
    TORSO_ATTACK2,

    TORSO_DROP,
    TORSO_RAISE,

    TORSO_STAND,
    TORSO_STAND2,

    LEGS_WALKCR,
    LEGS_WALK,
    LEGS_RUN,
    LEGS_BACK,
    LEGS_SWIM,

    LEGS_JUMP,
    LEGS_LAND,

    LEGS_JUMPB,
    LEGS_LANDB,

    LEGS_IDLE,
    LEGS_IDLECR,

    LEGS_TURN,

    TORSO_GETFLAG,
    TORSO_GUARDBASE,
    TORSO_PATROL,
    TORSO_FOLLOWME,
    TORSO_AFFIRMATIVE,
    TORSO_NEGATIVE,

    MAX_ANIMATIONS,

    LEGS_BACKCR,
    LEGS_BACKWALK,
    FLAG_RUN,
    FLAG_STAND,
    FLAG_STAND2RUN,

    MAX_TOTALANIMATIONS
} animNumber_t;


typedef enum {
    FOOTSTEP_NORMAL,
    FOOTSTEP_BOOT,
    FOOTSTEP_FLESH,
    FOOTSTEP_MECH,
    FOOTSTEP_ENERGY,
    FOOTSTEP_METAL,
    FOOTSTEP_SPLASH,

    FOOTSTEP_TOTAL
} footstep_t;

typedef enum {
    TR_STATIONARY,
    TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
    TR_LINEAR,
    TR_LINEAR_STOP,
    TR_SINE,					// value = base + sin( time / duration ) * delta
    TR_GRAVITY
} trType_t;

typedef struct {
    trType_t	trType;
    int		trTime;
    int		trDuration;			// if non 0, trTime + trDuration = stop time
    vec3_t	trBase;
    vec3_t	trDelta;			// velocity, etc
} trajectory_t;

typedef struct animation_s {
    int		firstFrame;
    int		numFrames;
    int		loopFrames;			// 0 to numFrames
    int		frameLerp;			// msec between frames
    int		initialLerp;		// msec to get to first frame
    int		reversed;			// true if animation is reversed
    int		flipflop;			// true if animation should flipflop back to base
} animation_t;


typedef struct {
    int			oldFrame;
    int			oldFrameTime;		// time when ->oldFrame was exactly on

    int			frame;
    int			frameTime;			// time when ->frame will be exactly on

    float		backlerp;

    float		yawAngle;
    qboolean	yawing;
    float		pitchAngle;
    qboolean	pitching;

    int			animationNumber;	// may include ANIM_TOGGLEBIT
    animation_t	*animation;
    int			animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;

typedef struct {
    lerpFrame_t		legs, torso, flag;
    int				painTime;
    int				painDirection;	// flip from 0 to 1
    int				lightningFiring;

    int				railFireTime;

    // machinegun spinning
    float			barrelAngle;
    int				barrelTime;
    qboolean		barrelSpinning;
} playerEntity_t;

typedef struct entityState_s {
    int		number;			// entity index
    int		eType;			// entityType_t
    int		eFlags;

    trajectory_t	pos;	// for calculating position
    trajectory_t	apos;	// for calculating angles

    int		time;
    int		time2;

    vec3_t	origin;
    vec3_t	origin2;

    vec3_t	angles;
    vec3_t	angles2;

    int		otherEntityNum;	// shotgun sources, etc
    int		otherEntityNum2;

    int		groundEntityNum;	// ENTITYNUM_NONE = in air

    int		constantLight;	// r + (g<<8) + (b<<16) + (intensity<<24)
    int		loopSound;		// constantly loop this sound

    int		modelindex;
    int		modelindex2;
    int		clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
    int		frame;

    int		solid;			// for client side prediction, trap_linkentity sets this properly

    int		event;			// impulse events -- muzzle flashes, footsteps, etc
    int		eventParm;

    // for players
    int		powerups;		// bit flags
    int		weapon;			// determines weapon and flash model, etc
    int		legsAnim;		// mask off ANIM_TOGGLEBIT
    int		torsoAnim;		// mask off ANIM_TOGGLEBIT

    int		generic1;
} entityState_t;

typedef struct centity_s {
    entityState_t	currentState;	// from cg.frame
    entityState_t	nextState;		// from cg.nextFrame, if available
    qboolean		interpolate;	// true if next is valid to interpolate to
    qboolean		currentValid;	// true if cg.frame holds this entity

    int				muzzleFlashTime;	// move to playerEntity?
    int				previousEvent;
    int				teleportFlag;

    int				trailTime;		// so missile trails can handle dropped initial packets
    int				dustTrailTime;
    int				miscTime;

    int				snapShotTime;	// last time this entity was found in a snapshot

    playerEntity_t	pe;

    int				errorTime;		// decay the error from this time
    vec3_t			errorOrigin;
    vec3_t			errorAngles;

    qboolean		extrapolated;	// false if origin / angles is an interpolation
    vec3_t			rawOrigin;
    vec3_t			rawAngles;

    vec3_t			beamEnd;

    // exact interpolated position of entity on this frame
    vec3_t			lerpOrigin;
    vec3_t			lerpAngles;
} centity_t;

#define	MAX_CUSTOM_SOUNDS	32

typedef struct {
    qboolean		infoValid;

    char			name[MAX_QPATH];
    team_t			team;

    int				botSkill;		// 0 = not bot, 1-5 = bot

    vec3_t			color1;
    vec3_t			color2;

    byte c1RGBA[4];
    byte c2RGBA[4];

    int				score;			// updated by score servercmds
    int				location;		// location index for team mode
    int				health;			// you only get this info about your teammates
    int				armor;
    int				curWeapon;

    int				handicap;
    int				wins, losses;	// in tourney mode

    int				teamTask;		// task in teamplay (offence/defence)
    qboolean		teamLeader;		// true when this is a team leader

    int				powerups;		// so can display quad/flag status

    int				medkitUsageTime;
    int				invulnerabilityStartTime;
    int				invulnerabilityStopTime;

    int				breathPuffTime;

    // when clientinfo is changed, the loading of models/skins/sounds
    // can be deferred until you are dead, to prevent hitches in
    // gameplay
    char			modelName[MAX_QPATH];
    char			skinName[MAX_QPATH];
    char			headModelName[MAX_QPATH];
    char			headSkinName[MAX_QPATH];
    char			redTeam[MAX_TEAMNAME];
    char			blueTeam[MAX_TEAMNAME];
    qboolean		deferred;

    qboolean		newAnims;		// true if using the new mission pack animations
    qboolean		fixedlegs;		// true if legs yaw is always the same as torso yaw
    qboolean		fixedtorso;		// true if torso never changes yaw

    vec3_t			headOffset;		// move head in icon views
    footstep_t		footsteps;
    gender_t		gender;			// from model

    qhandle_t		legsModel;
    qhandle_t		legsSkin;

    qhandle_t		torsoModel;
    qhandle_t		torsoSkin;

    qhandle_t		headModel;
    qhandle_t		headSkin;

    qhandle_t		modelIcon;

    animation_t		animations[MAX_TOTALANIMATIONS];

    sfxHandle_t		sounds[MAX_CUSTOM_SOUNDS];
} clientInfo_t;


#define	MAX_GAMESTATE_CHARS	16000
#define	MAX_CONFIGSTRINGS	1024

typedef struct {
    int			stringOffsets[MAX_CONFIGSTRINGS];
    char		stringData[MAX_GAMESTATE_CHARS];
    int			dataCount;
} gameState_t;

#define	BIG_INFO_STRING		8192


typedef struct {
    char					renderer_string[MAX_STRING_CHARS];
    char					vendor_string[MAX_STRING_CHARS];
    char					version_string[MAX_STRING_CHARS];
    char					extensions_string[BIG_INFO_STRING];

    int						maxTextureSize;			// queried from GL
    int						numTextureUnits;		// multitexture ability

    int						colorBits, depthBits, stencilBits;

    glDriverType_t			driverType;
    glHardwareType_t		hardwareType;

    qboolean				deviceSupportsGamma;
    textureCompression_t	textureCompression;
    qboolean				textureEnvAddAvailable;

    int						vidWidth, vidHeight;
    // aspect is the screen's physical width / height, which may be different
    // than scrWidth / scrHeight if the pixels are non-square
    // normal screens should be 4/3, but wide aspect monitors may be 16/9
    float					windowAspect;

    int						displayFrequency;

    // synonymous with "does rendering consume the entire screen?", therefore
    // a Voodoo or Voodoo2 will have this set to TRUE, as will a Win32 ICD that
    // used CDS.
    qboolean				isFullscreen;
    qboolean				stereoEnabled;
    qboolean				smpActive;		// UNUSED, present for compatibility
} glconfig_t;

typedef struct {
    qhandle_t	charsetShader;
    qhandle_t	charsetProp;
    qhandle_t	charsetPropGlow;
    qhandle_t	charsetPropB;
    qhandle_t	whiteShader;

#ifdef MISSIONPACK
    qhandle_t	redCubeModel;
    qhandle_t	blueCubeModel;
    qhandle_t	redCubeIcon;
    qhandle_t	blueCubeIcon;
#endif
    qhandle_t	redFlagModel;
    qhandle_t	blueFlagModel;
    qhandle_t	neutralFlagModel;
    qhandle_t	redFlagShader[3];
    qhandle_t	blueFlagShader[3];
    qhandle_t	flagShader[4];

    qhandle_t	flagPoleModel;
    qhandle_t	flagFlapModel;

    qhandle_t	redFlagFlapSkin;
    qhandle_t	blueFlagFlapSkin;
    qhandle_t	neutralFlagFlapSkin;

    qhandle_t	redFlagBaseModel;
    qhandle_t	blueFlagBaseModel;
    qhandle_t	neutralFlagBaseModel;

#ifdef MISSIONPACK
    qhandle_t	overloadBaseModel;
    qhandle_t	overloadTargetModel;
    qhandle_t	overloadLightsModel;
    qhandle_t	overloadEnergyModel;

    qhandle_t	harvesterModel;
    qhandle_t	harvesterRedSkin;
    qhandle_t	harvesterBlueSkin;
    qhandle_t	harvesterNeutralModel;
#endif

    qhandle_t	armorModel;
    qhandle_t	armorIcon;

    qhandle_t	teamStatusBar;

    qhandle_t	deferShader;

    // gib explosions
    qhandle_t	gibAbdomen;
    qhandle_t	gibArm;
    qhandle_t	gibChest;
    qhandle_t	gibFist;
    qhandle_t	gibFoot;
    qhandle_t	gibForearm;
    qhandle_t	gibIntestine;
    qhandle_t	gibLeg;
    qhandle_t	gibSkull;
    qhandle_t	gibBrain;

    qhandle_t	smoke2;

    qhandle_t	machinegunBrassModel;
    qhandle_t	shotgunBrassModel;

    qhandle_t	railRingsShader;
    qhandle_t	railCoreShader;

    qhandle_t	lightningShader;

    qhandle_t	friendShader;

    qhandle_t	balloonShader;
    qhandle_t	connectionShader;

    qhandle_t	selectShader;
    qhandle_t	viewBloodShader;
    qhandle_t	tracerShader;
    qhandle_t	crosshairShader[NUM_CROSSHAIRS];
    qhandle_t	lagometerShader;
    qhandle_t	backTileShader;
    qhandle_t	noammoShader;

    qhandle_t	smokePuffShader;
    qhandle_t	smokePuffRageProShader;
    qhandle_t	shotgunSmokePuffShader;
    qhandle_t	plasmaBallShader;
    qhandle_t	waterBubbleShader;
    qhandle_t	bloodTrailShader;
#ifdef MISSIONPACK
    qhandle_t	nailPuffShader;
    qhandle_t	blueProxMine;
#endif

    qhandle_t	numberShaders[11];

    qhandle_t	shadowMarkShader;

    qhandle_t	botSkillShaders[5];

    // wall mark shaders
    qhandle_t	wakeMarkShader;
    qhandle_t	bloodMarkShader;
    qhandle_t	bulletMarkShader;
    qhandle_t	burnMarkShader;
    qhandle_t	holeMarkShader;
    qhandle_t	energyMarkShader;

    // powerup shaders
    qhandle_t	quadShader;
    qhandle_t	redQuadShader;
    qhandle_t	quadWeaponShader;
    qhandle_t	invisShader;
    qhandle_t	regenShader;
    qhandle_t	battleSuitShader;
    qhandle_t	battleWeaponShader;
    qhandle_t	hastePuffShader;
#ifdef MISSIONPACK
    qhandle_t	redKamikazeShader;
    qhandle_t	blueKamikazeShader;
#endif

    // weapon effect models
    qhandle_t	bulletFlashModel;
    qhandle_t	ringFlashModel;
    qhandle_t	dishFlashModel;
    qhandle_t	lightningExplosionModel;

    // weapon effect shaders
    qhandle_t	railExplosionShader;
    qhandle_t	plasmaExplosionShader;
    qhandle_t	bulletExplosionShader;
    qhandle_t	rocketExplosionShader;
    qhandle_t	grenadeExplosionShader;
    qhandle_t	bfgExplosionShader;
    qhandle_t	bloodExplosionShader;

    // special effects models
    qhandle_t	teleportEffectModel;
    qhandle_t	teleportEffectShader;
#ifdef MISSIONPACK
    qhandle_t	kamikazeEffectModel;
    qhandle_t	kamikazeShockWave;
    qhandle_t	kamikazeHeadModel;
    qhandle_t	kamikazeHeadTrail;
    qhandle_t	guardPowerupModel;
    qhandle_t	scoutPowerupModel;
    qhandle_t	doublerPowerupModel;
    qhandle_t	ammoRegenPowerupModel;
    qhandle_t	invulnerabilityImpactModel;
    qhandle_t	invulnerabilityJuicedModel;
    qhandle_t	medkitUsageModel;
    qhandle_t	dustPuffShader;
    qhandle_t	heartShader;
    qhandle_t	invulnerabilityPowerupModel;
#endif

    // scoreboard headers
    qhandle_t	scoreboardName;
    qhandle_t	scoreboardPing;
    qhandle_t	scoreboardScore;
    qhandle_t	scoreboardTime;

    // medals shown during gameplay
    qhandle_t	medalImpressive;
    qhandle_t	medalExcellent;
    qhandle_t	medalGauntlet;
    qhandle_t	medalDefend;
    qhandle_t	medalAssist;
    qhandle_t	medalCapture;

    // sounds
    sfxHandle_t	quadSound;
    sfxHandle_t	tracerSound;
    sfxHandle_t	selectSound;
    sfxHandle_t	useNothingSound;
    sfxHandle_t	wearOffSound;
    sfxHandle_t	footsteps[FOOTSTEP_TOTAL][4];
    sfxHandle_t	sfx_lghit1;
    sfxHandle_t	sfx_lghit2;
    sfxHandle_t	sfx_lghit3;
    sfxHandle_t	sfx_ric1;
    sfxHandle_t	sfx_ric2;
    sfxHandle_t	sfx_ric3;
    //sfxHandle_t	sfx_railg;
    sfxHandle_t	sfx_rockexp;
    sfxHandle_t	sfx_plasmaexp;
#ifdef MISSIONPACK
    sfxHandle_t	sfx_proxexp;
    sfxHandle_t	sfx_nghit;
    sfxHandle_t	sfx_nghitflesh;
    sfxHandle_t	sfx_nghitmetal;
    sfxHandle_t	sfx_chghit;
    sfxHandle_t	sfx_chghitflesh;
    sfxHandle_t	sfx_chghitmetal;
    sfxHandle_t kamikazeExplodeSound;
    sfxHandle_t kamikazeImplodeSound;
    sfxHandle_t kamikazeFarSound;
    sfxHandle_t useInvulnerabilitySound;
    sfxHandle_t invulnerabilityImpactSound1;
    sfxHandle_t invulnerabilityImpactSound2;
    sfxHandle_t invulnerabilityImpactSound3;
    sfxHandle_t invulnerabilityJuicedSound;
    sfxHandle_t obeliskHitSound1;
    sfxHandle_t obeliskHitSound2;
    sfxHandle_t obeliskHitSound3;
    sfxHandle_t	obeliskRespawnSound;
    sfxHandle_t	winnerSound;
    sfxHandle_t	loserSound;
#endif
    sfxHandle_t	gibSound;
    sfxHandle_t	gibBounce1Sound;
    sfxHandle_t	gibBounce2Sound;
    sfxHandle_t	gibBounce3Sound;
    sfxHandle_t	teleInSound;
    sfxHandle_t	teleOutSound;
    sfxHandle_t	noAmmoSound;
    sfxHandle_t	respawnSound;
    sfxHandle_t talkSound;
    sfxHandle_t landSound;
    sfxHandle_t fallSound;
    sfxHandle_t jumpPadSound;

    sfxHandle_t oneMinuteSound;
    sfxHandle_t fiveMinuteSound;
    sfxHandle_t suddenDeathSound;

    sfxHandle_t threeFragSound;
    sfxHandle_t twoFragSound;
    sfxHandle_t oneFragSound;

    sfxHandle_t hitSound;
    sfxHandle_t hitSoundHighArmor;
    sfxHandle_t hitSoundLowArmor;
    sfxHandle_t hitTeamSound;
    sfxHandle_t impressiveSound;
    sfxHandle_t excellentSound;
    sfxHandle_t deniedSound;
    sfxHandle_t humiliationSound;
    sfxHandle_t assistSound;
    sfxHandle_t defendSound;
    sfxHandle_t firstImpressiveSound;
    sfxHandle_t firstExcellentSound;
    sfxHandle_t firstHumiliationSound;

    sfxHandle_t takenLeadSound;
    sfxHandle_t tiedLeadSound;
    sfxHandle_t lostLeadSound;

    sfxHandle_t voteNow;
    sfxHandle_t votePassed;
    sfxHandle_t voteFailed;

    sfxHandle_t watrInSound;
    sfxHandle_t watrOutSound;
    sfxHandle_t watrUnSound;

    sfxHandle_t flightSound;
    sfxHandle_t medkitSound;

#ifdef MISSIONPACK
    sfxHandle_t weaponHoverSound;
#endif

    // teamplay sounds
    sfxHandle_t captureAwardSound;
    sfxHandle_t redScoredSound;
    sfxHandle_t blueScoredSound;
    sfxHandle_t redLeadsSound;
    sfxHandle_t blueLeadsSound;
    sfxHandle_t teamsTiedSound;

    sfxHandle_t	captureYourTeamSound;
    sfxHandle_t	captureOpponentSound;
    sfxHandle_t	returnYourTeamSound;
    sfxHandle_t	returnOpponentSound;
    sfxHandle_t	takenYourTeamSound;
    sfxHandle_t	takenOpponentSound;

    sfxHandle_t redFlagReturnedSound;
    sfxHandle_t blueFlagReturnedSound;
#ifdef MISSIONPACK
    sfxHandle_t neutralFlagReturnedSound;
#endif
    sfxHandle_t	enemyTookYourFlagSound;
    sfxHandle_t yourTeamTookEnemyFlagSound;
    sfxHandle_t	youHaveFlagSound;
#ifdef MISSIONPACK
    sfxHandle_t	enemyTookTheFlagSound;
    sfxHandle_t yourTeamTookTheFlagSound;
    sfxHandle_t yourBaseIsUnderAttackSound;
#endif
    sfxHandle_t holyShitSound;

    // tournament sounds
    sfxHandle_t	count3Sound;
    sfxHandle_t	count2Sound;
    sfxHandle_t	count1Sound;
    sfxHandle_t	countFightSound;
    sfxHandle_t	countPrepareSound;

#ifdef MISSIONPACK
    // new stuff
    qhandle_t patrolShader;
    qhandle_t assaultShader;
    qhandle_t campShader;
    qhandle_t followShader;
    qhandle_t defendShader;
    qhandle_t teamLeaderShader;
    qhandle_t retrieveShader;
    qhandle_t escortShader;
    qhandle_t flagShaders[3];
    sfxHandle_t	countPrepareTeamSound;

    sfxHandle_t ammoregenSound;
    sfxHandle_t doublerSound;
    sfxHandle_t guardSound;
    sfxHandle_t scoutSound;

    qhandle_t cursor;
    qhandle_t selectCursor;
    qhandle_t sizeCursor;
#endif

    sfxHandle_t	regenSound;
    sfxHandle_t	protectSound;
    sfxHandle_t	n_healthSound;
    sfxHandle_t	hgrenb1aSound;
    sfxHandle_t	hgrenb2aSound;
    sfxHandle_t	wstbimplSound;
    sfxHandle_t	wstbimpmSound;
    sfxHandle_t	wstbimpdSound;
    sfxHandle_t	wstbactvSound;

} cgMedia_t;

typedef struct {
    gameState_t		gameState;			// gamestate from server
    glconfig_t		glconfig;			// rendering configuration
    float			screenXScale;		// derived from glconfig
    float			screenYScale;
    float			screenXBias;

    int				serverCommandSequence;	// reliable command stream counter
    int				processedSnapshotNum;// the number of snapshots cgame has requested

    qboolean		localServer;		// detected on startup by checking sv_running

    // parsed from serverinfo
    gametype_t		gametype;
    int				dmflags;
    int				teamflags;
    int				fraglimit;
    int				capturelimit;
    int				timelimit;
    int				maxclients;
    char			mapname[MAX_QPATH];
    char			redTeam[MAX_QPATH];
    char			blueTeam[MAX_QPATH];

    int				voteTime;
    int				voteYes;
    int				voteNo;
    qboolean		voteModified;			// beep whenever changed
    char			voteString[MAX_STRING_TOKENS];

    int				teamVoteTime[2];
    int				teamVoteYes[2];
    int				teamVoteNo[2];
    qboolean		teamVoteModified[2];	// beep whenever changed
    char			teamVoteString[2][MAX_STRING_TOKENS];

    int				levelStartTime;

    int				scores1, scores2;		// from configstrings
    int				redflag, blueflag;		// flag status from configstrings
    int				flagStatus;

    qboolean  newHud;

    //
    // locally derived information from gamestate
    //
    qhandle_t		gameModels[MAX_MODELS];
    sfxHandle_t		gameSounds[MAX_SOUNDS];

    int				numInlineModels;
    qhandle_t		inlineDrawModel[MAX_MODELS];
    vec3_t			inlineModelMidpoints[MAX_MODELS];

    clientInfo_t	clientinfo[MAX_CLIENTS];

    // teamchat width is *3 because of embedded color codes
    char			teamChatMsgs[TEAMCHAT_HEIGHT][TEAMCHAT_WIDTH*3+1];
    int				teamChatMsgTimes[TEAMCHAT_HEIGHT];
    int				teamChatPos;
    int				teamLastChatPos;

    int cursorX;
    int cursorY;
    qboolean eventHandling;
    qboolean mouseCaptured;
    qboolean sizingHud;
    void *capturedItem;
    qhandle_t activeCursor;

    // orders
    int currentOrder;
    qboolean orderPending;
    int orderTime;
    int currentVoiceClient;
    int acceptOrderTime;
    int acceptTask;
    int acceptLeader;
    char acceptVoice[MAX_NAME_LENGTH];

    // media
    cgMedia_t		media;

} cgs_t;

#define MAX_REWARDSTACK		10
#define MAX_SOUNDBUFFER		20

//======================================================================

// all cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action
// occurs, and they will have visible effects for #define STEP_TIME or whatever msec after

#define MAX_PREDICTED_EVENTS	16

typedef enum {
    RT_MODEL,
    RT_POLY,
    RT_SPRITE,
    RT_BEAM,
    RT_RAIL_CORE,
    RT_RAIL_RINGS,
    RT_LIGHTNING,
    RT_PORTALSURFACE,		// doesn't draw anything, just info for portals

    RT_MAX_REF_ENTITY_TYPE
} refEntityType_t;

typedef struct {
    refEntityType_t	reType;
    int			renderfx;

    qhandle_t	hModel;				// opaque type outside refresh

    // most recent data
    vec3_t		lightingOrigin;		// so multi-part models can be lit identically (RF_LIGHTING_ORIGIN)
    float		shadowPlane;		// projection shadows go here, stencils go slightly lower

    vec3_t		axis[3];			// rotation vectors
    qboolean	nonNormalizedAxes;	// axis are not normalized, i.e. they have scale
    float		origin[3];			// also used as MODEL_BEAM's "from"
    int			frame;				// also used as MODEL_BEAM's diameter

    // previous data for frame interpolation
    float		oldorigin[3];		// also used as MODEL_BEAM's "to"
    int			oldframe;
    float		backlerp;			// 0.0 = current, 1.0 = old

    // texturing
    int			skinNum;			// inline skin index
    qhandle_t	customSkin;			// NULL for default skin
    qhandle_t	customShader;		// use one image for the entire thing

    // misc
    byte		shaderRGBA[4];		// colors used by rgbgen entity shaders
    float		shaderTexCoord[2];	// texture coordinates used by tcMod entity modifiers
    float		shaderTime;			// subtracted from refdef time to control effect start times

    // extra sprite information
    float		radius;
    float		rotation;
} refEntity_t;

#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16
#define	MAX_WEAPONS				16

#define	MAX_PS_EVENTS			2

#define	MAX_ENTITIES_IN_SNAPSHOT	256

typedef struct playerState_s {
    int			commandTime;	// cmd->serverTime of last executed command
    int			pm_type;
    int			bobCycle;		// for view bobbing and footstep generation
    int			pm_flags;		// ducked, jump_held, etc
    int			pm_time;

    vec3_t		origin;
    vec3_t		velocity;
    int			weaponTime;
    int			gravity;
    int			speed;
    int			delta_angles[3];	// add to command angles to get view direction
                                    // changed by spawns, rotating objects, and teleporters

    int			groundEntityNum;// ENTITYNUM_NONE = in air

    int			legsTimer;		// don't change low priority animations until this runs out
    int			legsAnim;		// mask off ANIM_TOGGLEBIT

    int			torsoTimer;		// don't change low priority animations until this runs out
    int			torsoAnim;		// mask off ANIM_TOGGLEBIT

    int			movementDir;	// a number 0 to 7 that represents the relative angle
                                // of movement to the view angle (axial and diagonals)
                                // when at rest, the value will remain unchanged
                                // used to twist the legs during strafing

    vec3_t		grapplePoint;	// location of grapple to pull towards if PMF_GRAPPLE_PULL

    int			eFlags;			// copied to entityState_t->eFlags

    int			eventSequence;	// pmove generated events
    int			events[MAX_PS_EVENTS];
    int			eventParms[MAX_PS_EVENTS];

    int			externalEvent;	// events set on player from another source
    int			externalEventParm;
    int			externalEventTime;

    int			clientNum;		// ranges from 0 to MAX_CLIENTS-1
    int			weapon;			// copied to entityState_t->weapon
    int			weaponstate;

    vec3_t		viewangles;		// for fixed views
    int			viewheight;

    // damage feedback
    int			damageEvent;	// when it changes, latch the other parms
    int			damageYaw;
    int			damagePitch;
    int			damageCount;

    int			stats[MAX_STATS];
    int			persistant[MAX_PERSISTANT];	// stats that aren't cleared on death
    int			powerups[MAX_POWERUPS];	// level.time that the powerup runs out
    int			ammo[MAX_WEAPONS];

    int			generic1;
    int			loopSound;
    int			jumppad_ent;	// jumppad entity hit this frame

    // not communicated over the net at all
    int			ping;			// server to game info for scoreboard
    int			pmove_framecount;
    int			jumppad_frame;
    int			entityEventSequence;
} playerState_t;

#define	MAX_MAP_AREA_BYTES		32
typedef struct {
    int				snapFlags;			// SNAPFLAG_RATE_DELAYED, etc
    int				ping;

    int				serverTime;		// server time the message is valid for (in msec)

    byte			areamask[MAX_MAP_AREA_BYTES];		// portalarea visibility bits

    playerState_t	ps;						// complete information about the current player at this time

    int				numEntities;			// all of the entities that need to be presented
    entityState_t	entities[MAX_ENTITIES_IN_SNAPSHOT];	// at the time of this snapshot

    int				numServerCommands;		// text based server commands to execute when this
    int				serverCommandSequence;	// snapshot becomes current
} snapshot_t;

#define	MAX_RENDER_STRINGS			8
#define	MAX_RENDER_STRING_LENGTH	32

typedef struct {
    int			x, y, width, height;
    float		fov_x, fov_y;
    vec3_t		vieworg;
    vec3_t		viewaxis[3];		// transformation matrix

    // time in milliseconds for shader effects and other time dependent rendering issues
    int			time;

    int			rdflags;			// RDF_NOWORLDMODEL, etc

    // 1 bits will prevent the associated area from rendering at all
    byte		areamask[MAX_MAP_AREA_BYTES];

    // text messages for deform text shaders
    char		text[MAX_RENDER_STRINGS][MAX_RENDER_STRING_LENGTH];
} refdef_t;

typedef struct {
    int				client;
    int				score;
    int				ping;
    int				time;
    int				scoreFlags;
    int				powerUps;
    int				accuracy;
    int				impressiveCount;
    int				excellentCount;
    int				guantletCount;
    int				defendCount;
    int				assistCount;
    int				captures;
    qboolean	perfect;
    int				team;
} score_t;

typedef struct {
    int			clientFrame;		// incremented each frame

    int			clientNum;

    qboolean	demoPlayback;
    qboolean	levelShot;			// taking a level menu screenshot
    int			deferredPlayerLoading;
    qboolean	loading;			// don't defer players at initial startup
    qboolean	intermissionStarted;	// don't play voice rewards, because game will end shortly

    // there are only one or two snapshot_t that are relevant at a time
    int			latestSnapshotNum;	// the number of snapshots the client system has received
    int			latestSnapshotTime;	// the time from latestSnapshotNum, so we don't need to read the snapshot yet

    snapshot_t	*snap;				// cg.snap->serverTime <= cg.time
    snapshot_t	*nextSnap;			// cg.nextSnap->serverTime > cg.time, or NULL
    snapshot_t	activeSnapshots[2];

    float		frameInterpolation;	// (float)( cg.time - cg.frame->serverTime ) / (cg.nextFrame->serverTime - cg.frame->serverTime)

    qboolean	thisFrameTeleport;
    qboolean	nextFrameTeleport;

    int			frametime;		// cg.time - cg.oldTime

    int			time;			// this is the time value that the client
                                // is rendering at.
    int			oldTime;		// time at last frame, used for missile trails and prediction checking

    int			physicsTime;	// either cg.snap->time or cg.nextSnap->time

    int			timelimitWarnings;	// 5 min, 1 min, overtime
    int			fraglimitWarnings;

    qboolean	mapRestart;			// set on a map restart to set back the weapon

    qboolean	renderingThirdPerson;		// during deaths, chasecams, etc

    // prediction state
    qboolean	hyperspace;				// true if prediction has hit a trigger_teleport
    playerState_t	predictedPlayerState;
    centity_t		predictedPlayerEntity;
    qboolean	validPPS;				// clear until the first call to CG_PredictPlayerState
    int			predictedErrorTime;
    vec3_t		predictedError;

    int			eventSequence;
    int			predictableEvents[MAX_PREDICTED_EVENTS];

    float		stepChange;				// for stair up smoothing
    int			stepTime;

    float		duckChange;				// for duck viewheight smoothing
    int			duckTime;

    float		landChange;				// for landing hard
    int			landTime;

    // input state sent to server
    int			weaponSelect;

    // auto rotating items
    vec3_t		autoAngles;
    vec3_t		autoAxis[3];
    vec3_t		autoAnglesFast;
    vec3_t		autoAxisFast[3];

    // view rendering
    refdef_t	refdef;
    vec3_t		refdefViewAngles;		// will be converted to refdef.viewaxis

    // zoom key
    qboolean	zoomed;
    int			zoomTime;
    float		zoomSensitivity;

    // information screen text during loading
    char		infoScreenText[MAX_STRING_CHARS];

    // scoreboard
    int			scoresRequestTime;
    int			numScores;
    int			selectedScore;
    int			teamScores[2];
    score_t		scores[MAX_CLIENTS];
    qboolean	showScores;
    qboolean	scoreBoardShowing;
    int			scoreFadeTime;
    char		killerName[MAX_NAME_LENGTH];
    char			spectatorList[MAX_STRING_CHARS];		// list of names
    int				spectatorLen;												// length of list
    float			spectatorWidth;											// width in device units
    int				spectatorTime;											// next time to offset
    int				spectatorPaintX;										// current paint x
    int				spectatorPaintX2;										// current paint x
    int				spectatorOffset;										// current offset from start
    int				spectatorPaintLen; 									// current offset from start

#ifdef MISSIONPACK
    // skull trails
    skulltrail_t	skulltrails[MAX_CLIENTS];
#endif

    // centerprinting
    int			centerPrintTime;
    int			centerPrintCharWidth;
    int			centerPrintY;
    char		centerPrint[1024];
    int			centerPrintLines;

    // low ammo warning state
    int			lowAmmoWarning;		// 1 = low, 2 = empty

    // crosshair client ID
    int			crosshairClientNum;
    int			crosshairClientTime;

    // powerup active flashing
    int			powerupActive;
    int			powerupTime;

    // attacking player
    int			attackerTime;
    int			voiceTime;

    // reward medals
    int			rewardStack;
    int			rewardTime;
    int			rewardCount[MAX_REWARDSTACK];
    qhandle_t	rewardShader[MAX_REWARDSTACK];
    qhandle_t	rewardSound[MAX_REWARDSTACK];

    // sound buffer mainly for announcer sounds
    int			soundBufferIn;
    int			soundBufferOut;
    int			soundTime;
    qhandle_t	soundBuffer[MAX_SOUNDBUFFER];

#ifdef MISSIONPACK
    // for voice chat buffer
    int			voiceChatTime;
    int			voiceChatBufferIn;
    int			voiceChatBufferOut;
#endif

    // warmup countdown
    int			warmup;
    int			warmupCount;

    //==========================

    int			itemPickup;
    int			itemPickupTime;
    int			itemPickupBlendTime;	// the pulse around the crosshair is timed separately

    int			weaponSelectTime;
    int			weaponAnimation;
    int			weaponAnimationTime;

    // blend blobs
    float		damageTime;
    float		damageX, damageY, damageValue;

    // status bar head
    float		headYaw;
    float		headEndPitch;
    float		headEndYaw;
    int			headEndTime;
    float		headStartPitch;
    float		headStartYaw;
    int			headStartTime;

    // view movement
    float		v_dmg_time;
    float		v_dmg_pitch;
    float		v_dmg_roll;

    // temp working variables for player view
    float		bobfracsin;
    int			bobcycle;
    float		xyspeed;
    int     nextOrbitTime;

    //qboolean cameraMode;		// if rendering from a loaded camera


    // development tool
    refEntity_t		testModelEntity;
    char			testModelName[MAX_QPATH];
    qboolean		testGun;

} cg_t;


#endif // GAME_UTILS_H
