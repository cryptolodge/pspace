#ifndef _SUBSPACEZONE_H_
#define _SUBSPACEZONE_H_

#include <list>
#include <map>
#include <vector>

using std::list;
using std::map;
using std::vector;

#include "DataTypes.h"
#include "GameDisplayManager.h"
#include "Rect.h"
#include "Thread.h"

#include "SubspaceBackground.h"
#include "SubspaceGlobal.h"
#include "SubspaceLVZManager.h"
#include "SubspaceMap.h"
#include "SubspacePrizeGenerator.h"

#include "ChatMessage.h"
#include "PlayerSettings.h"
#include "SubspaceCommand.h"
#include "SubspaceClientCommandListener.h"
#include "SubspaceServerCommandListener.h"

class SubspaceBall;
class SubspaceBomb;
class SubspaceBrick;
class SubspaceBullet;
class SubspaceFlag;
class SubspacePlayer;
class SubspacePortal;
class SubspaceRadar;
class SubspaceRepel;
class SubspaceSpecialEffect;
class SubspaceWeapon;

// TODO: the zone should just be a data model, it should have no concept of "my player"
class SubspaceZone : 
	public SubspaceClientCommandListener,
	public CommandGenerator<SubspaceServerCommandListener>
{
public:
	
	typedef list<SubspaceGameObject*> ObjectList;
	
	typedef list<SubspaceBomb*> BombList;
	typedef list<SubspaceBrick*> BrickList;
	typedef list<SubspaceBullet*> BulletList;
	typedef list<ChatMessage> ChatList;
	typedef list<SubspaceSpecialEffect*> EffectList;
	typedef list<SubspacePortal*> PortalList;
	typedef list<SubspaceRepel*> RepelList;
	typedef list<SubspaceWeapon*> WeaponList;

	typedef map<Uint, SubspaceBall*> BallMap;
	typedef map<Uint, SubspaceFlag*> FlagMap;
	typedef map<Uint, SubspacePlayer*> SubspacePlayerMap;		// TODO: make this a hash_map

public:
	SubspaceZone();
	~SubspaceZone();

	//Main functions
		void init();
		void resize();	

	//Arena management

        void onEnterZone();
		void onEnterArena();

		void initArena();			//TODO: synchronization of greens and doors here?
		bool loadSettings(const string& filename);	// TODO: not implemented yet
		void loadDefaultSettings();

		const ArenaSettings& getArenaSettings() const;
		void setLogin(const string& username, const string& password);
		void setStartArena(const string& arenaName);
		void setStartShip(ShipType ship);

	//Brick management
		void dropBrick(Uint team, Uint x1, Uint y1, Uint x2, Uint y2, Uint timestamp, bool alert = false);
		void removeBrick(SubspaceBrick* brick);
		void updateBricks();

	//Flag management
		void initFlag(int flagID);
		void initFlags();
		void setFlag(int flagID, int team, const Vector& position);
		//void updateFlagTeams();	//TODO: should not be necessary

	//Map management
		bool loadMap(const string& filename);		

		const SubspaceMap* getMap() const;
		void setZonePath(const string& path);
	
	//Network management
		// void forceSendPosition();		//should be a better way than using this
		// void setServerTimestamp(Uint32 time);
		// void updateNetworkState();		// TODO: this shouldn't be here
		bool saveBuffer(const char* filename, unsigned char* buffer, unsigned long size);
		bool uncompressBuffer(unsigned char* buffer, unsigned long size, unsigned char** outBuffer, unsigned long* outSize);
		
		// TODO: files should be saved by something else, data loaded into zone after
    
	// commands to server
		bool sendArenaLogin(const string& arenaName);		//0x01
		bool sendArenaLeave();								//0x02
		bool sendPosition();								//0x03		
		bool sendWeapon(const WeaponData& w);				//0x03
		bool sendDeath(Uint16 killerID, Uint16 bounty);		//0x05
		// bool sendChat(const string& name, const string& message, ChatType type);	//0x07
		// bool sendChat(Uint16 playerID, const string& message, ChatType type);		//0x07
		bool sendAttachRequest(Uint16 playerID);			//0x10
		bool sendPassword(const string& user, const string& password, bool newUser = false);		//0x0A
		bool sendMapRequest();								//0x0C
		bool sendTeamChange(Uint16 team);					//0x0F
		bool sendFlagRequest(Uint16 flagID);				//0x13
		bool sendFlagsDrop();								//0x15
		bool sendRegForm();									//0x17
		bool sendShipChange(ShipType ship);					//0x18
		bool sendChecksum(Uint32 checksumKey);				//0x1A
		bool sendKothTimerEnded();							//0x1E
		bool sendBrickDrop(Uint16 xTile, Uint16 yTile);		//0x21

	// commands from network
		void doSetMyPlayerID(Uint16 id);			//0x01
		void doInGame();							//0x02
		void doPlayerEntering(PlayerData& p);		//0x03
		void doPlayerLeaving(Uint16 id);			//0x04
		void doPositionWeapon(						//0x05													
			Uint16 playerID, Uint16 timer, Sint16 posX, Sint16 posY, Sint16 velX, Sint16 velY, 
			Uint8 dir, PlayerStates states, PlayerItems items, WeaponData weaponData, 
			Uint16 bounty, Uint16 energy, Uint16 timestamp, Uint8 checksum, Uint8 ping, Uint16 s2cLag);
		void doPlayerDeath(Uint16 killerID, Uint16 killedID, Uint16 bounty);			//0x06
		// void doChat(Uint16 playerID, string message, Uint8 chatType, Uint8 soundByte);	//0x07
		void doPlayerPrize(Uint16 playerID, Uint16 xTile, Uint16 yTile, Uint16 prizeType, Uint32 timestamp);		//0x08		
		void doScoreUpdate(Uint16 playerID, Uint32 killPoints, Uint32 flagPoints, Uint16 wins, Uint16 losses);
		void doPasswordResponse(BYTE response, const string& message, Uint32 version, BYTE regForm, Uint32 newsChecksum); //0x0A
		void doPlayerChangeTeam(Uint16 playerID, Uint16 team, Uint8 ship);					//0x0D
		void doTurretLink(Uint16 requesterID, Uint16 destID);			//0x0E
		void doSetArenaSettings(const ArenaSettings& settings);			//0x0F
		void doFlagPosition(Uint16 flagID, Uint16 xTile, Uint16 yTile, Uint16 teamID);								//0x12
		void doFlagClaim(Uint16 flagID, Uint16 playerID);				//0x13
		void doDestroyTurretLink(Uint16 playerID);						//0x15
		void doFlagDrop(Uint16 playerID);								//0x16
		void doServerSyncRequest(Uint32 prizeSeed, Uint32 doorSeed, Uint32 timestamp, Uint32 checksumKey);			//0x18
		void doPlayerChangeShip(Uint16 playerID, Uint16 team, Uint8 ship);											//0x1D
		void doPlayerBanner(Uint16 playerID, const string& bannerData);													//0x1F
		void doBrickPlace(vector<BrickData> bricks);					//0x21
		// TODO: move this to protocol layer?
		void doKeepAlive();												//0x27
		void doPosition(												//0x28
			Uint8 playerID, Uint8 bounty, Sint16 posX, Sint16 posY, Sint16 velX, Sint16 velY, Uint8 dir, 
			PlayerStates stateInfo, PlayerItems itemInfo, Uint16 energy, Uint16 timestamp, Uint8 ping, 
			Uint16 s2cLag, Uint16 timer);
		void doMapInfo(const string& mapFilename, Uint32 remoteFileChecksum, Uint32 downloadSize);							//0x29
		void doMapFile(const string& mapName, const string& data);					//0x2A	//always a compressed map file
		// TODO: move to player adapter
		void doKothTimerSet(Uint32 time);								//0x2B
		void doKothReset(Uint32 time, Uint16 playerID);					//0x2C
		void doBallPosition(											//0x2E
			Uint8 ballID, Uint16 xPixel, Uint16 yPixel, Sint16 xVelocity, Sint16 yVelocity, Uint16 ownerID,
			Uint16 timestamp);
		// void doArenaList(vector<string> names, vector<Uint> population);//0x2F
		void doPastLogin();												//0x31

	//Object Management
		//void addObject(SubspaceObject* object);
		void addBullet(SubspaceBullet* bullet);		//bullets, multifire bullets, bursts
		void addBomb(SubspaceBomb* bomb);			//bombs, thors
		void addBrick(SubspaceBrick* brick);
		void addPortal(SubspacePortal* portal);
		void addRepel(SubspaceRepel* repel);		//adds repel targets
		void addWeapon(SubspaceWeapon* wep);		//any other weapons, i.e. decoys
		void clearPortals(SubspacePlayer* owner);
		void destroyBullet(SubspaceBullet* bullet);
		void destroyBomb(SubspaceBomb* bomb);
		void fireShrapnel(const SubspaceBomb& bomb);//does not initialize velocity

	//Player management
		void initMyPlayer();						//sets up client player

		SubspacePlayer* getMyPlayer();
		Uint getMyPlayerID() const;
		Uint getNumPlayers() const;
		SubspacePlayer* getPlayer(int id);
		const SubspacePlayerMap* getPlayers() const;
		
		void addPlayer(SubspacePlayer* player, int id);
		void removePlayer(int id);
		void updatePlayer(SubspacePlayer* player, int id);
		//void requestShipChange(ShipType ship);	//TODO: implement this

		//TODO: should players automatically add stuff to the zone, or should they return something that the zone uses
		void playerChangeShip(int playerID, ShipType ship);
		void playerChangeTeam(int playerID, int team);
		void playerDeath(int playerID);
		void playerDropFlag(int playerID, bool alertDrop = false);	//alert everyone else about drop - prevents infinite network loop
		void playerFireBrick(int playerID);
		void playerFireBomb(int playerID);
		void playerFireBullet(int playerID);
		void playerFireBurst(int playerID);
		void playerFireDecoy(int playerID);
		void playerFireMine(int playerID);
		void playerFirePortal(int playerID);
		void playerFireRepel(int playerID);
		void playerFireThor(int playerID);
		void playerGotFlag(int playerID, int flagID);
		void playerGotPrize(int playerID, PrizeType prize, Uint xTile, Uint yTile);
		void playerKothEnded(int playerID);
		void playerRequestFlag(int playerID, int flagID);
		void playerRequestShipChange(int playerID, ShipType ship);
		void playerRequestTeamChange(int playerID, int team);


	//Powerball management
		void initBall(int ballID);
		void updateBalls();

		void setBall(int ballID, int ownerID, const Vector& position, const Vector& velocity, Uint timestamp);

	//Prize management
		void initPrizes();
		void updatePrizes(double time);
		void spawnPrize();
		void spawnPrize(Vector* position);

		Uint getNumPrizes() const;
		Uint getNumPrizesMax() const;
		SubspacePrizeGenerator& getPrizeGenerator();
		string getPrizeMessage(PrizeType prize) const;
	
	//TODO: implement team management/references system, team statistics and such

	//Special Effects Management
		void addEffect(SubspaceSpecialEffect* effect, bool isFar = false);
		void updateBackground();

		void setLVZ(SubspaceLVZManager* lvzManager);

	//Turreting
		void createTurret(int requesterID, int destinationID);
		void destroyTurret(int playerID);
		void requestTurret(int playerID);

	
	//COLLISION DETECTION
	//TODO: improve this with special case of .collideWith(object), all subspaceGameObjects
	//TODO: add support for arbitrarily sized (square) ships

	//Map collisions
		bool isLevelCollision(const Vector& pos);
		Vector getLevelCollision(const Vector& pos, const Vector& vel, double timestep);	//returns position after collision

		//TODO: use .collideWith()
		//Map-Object collisions - flags, prizes
		void handleMapObjectCollisions();
		void getTileObjectsInBox(const Rect& rect, ObjectList& objects);

		double handleMapCollision(double time, const Vector& oldPosition, const Vector& oldVelocity, Vector& newPosition, Vector& newVelocity, double friction = 1.0, bool stopAfterCollision = false);	
		double handleMapCollisionsHelper(double time, const Vector& oldPosition, const Vector& oldVelocity, Vector& newPosition, Vector& newVelocity, double friction = 1.0, bool stopAfterCollision = false);
		double handleMapBoxCollision(double time, const Rect& oldRect, const Vector& oldVelocity, Rect& newRect, Vector& newVelocity, double friction = 1.0, bool stopAfterCollision = false);
		double handleMapRectCollision(double time, const Rect& oldRect, const Vector& oldVelocity, Rect& newRect, Vector& newVelocity, double friction = 1.0, bool stopAfterCollision = false);
		
		static void handleMapCollisionX(double oldPosition, double oldVelocity, double* newPosition, double* newVelocity);
		static void handleMapCollisionY(double oldPosition, double oldVelocity, double* newPosition, double* newVelocity);

		static bool handleMapCollisionX(double friction, const Vector& nextPos, const Vector& vel, Vector& newPosition, Vector& newVelocity);
		static bool handleMapCollisionY(double friction, const Vector& nextPos, const Vector& vel, Vector& newPosition, Vector& newVelocity);
		
		static void calcDistanceStep(double time, const Vector& velocity, Vector& increments);
		static double calcDistanceToTile(double pos, double vel, double tileSize);	//distance in current direction
		static double calcTimeToTile(double pos, double vel, double tileSize);

		//TODO: improve this stuff: .collideWith(object), more general
		//Object-Player collisions - bullets, bombs, whatever
		void handlePlayerObjectCollisions();
		void handleBulletCollisions(SubspacePlayer* player);
		void handleBombCollisions(SubspacePlayer* player);

	////////////////////////////

	//Update
	void update(double timestep);

	template <class ListType> void updateList(ListType& list, double timeStep, ObjectLayerType effectLayer = UnknownLayer);
	template <class MapType> void updateMap(MapType& map, double timeStep);

	//Display
	//TODO: create a graphics manager class for display optimization, i.e. frustum culling
	void draw();
	void drawRadarItems(const SubspaceRadar& radar) const;

	//TODO: impelement modular drawing, i.e. map, player, objects
	template <class ListType> void drawList(const ListType& list) const;
	template <class MapType> void drawMap(const MapType& map) const;


private:

	//multithreaded routines
	//void notifyMapLoaded(bool isLoaded);
	//friend bool loadMapRoutine(void* arg);

	friend void positionThreadRoutine(void* SubspaceZone);

private:

	//Arena management
	ArenaSettings arenaSettings_;
	string startArena_;
	ShipType startShip_;
	string username_, password_;

	//Graphics		
	GameDisplayManager displayManager_[NUM_LAYERS];

	//Hud Management
	//SubspaceHud hud_;	//TODO: move the hud out of here

	Uint infoBoxMode_;

	//Map Management
	SubspaceMap map_;
	Uint32 prizeSeed_, doorSeed_;
	Thread loadMapThread_;
	bool validMap_;

	string zonePath_;

	//Mini-games
	BallMap balls_;
	FlagMap flags_;

	//My player
	int myPlayerID_;
	Thread positionThread_;

	//Network Management
	string mapFilename_;
	bool connected_;			// TODO: phase this out
	Uint32 serverTime_;

	//Object types
	BombList bombs_;
	BrickList bricks_;
	BulletList bullets_;
	PortalList portals_;
	RepelList repels_;
	WeaponList weapons_;			//additional weapons

	//Player Management
	SubspacePlayerMap players_;
	
	//Prizes - handled by map
	SubspacePrizeGenerator prizeGenerator_;
	Uint lastPrizeUpdate_;
	Uint numPrizes_;

	//Special effects
	SubspaceBackground background_;		//TODO: move outside of zone?
	EffectList effects_;		//bullet explosions, bomb explosions
	EffectList effectsFar_;		//effects that happen farther away

	SubspaceLVZManager* lvzManager_;
};

#endif