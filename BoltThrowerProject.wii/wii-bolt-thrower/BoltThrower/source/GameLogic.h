#ifndef GameLogic_H
#define GameLogic_H

#include <vector>
#include "ogc/gx.h"

#include <aesndlib.h>

#include "SoundManager.h"

struct StartAndEndFrameInfo
{
	int StartFrame;
	int EndFrame;
};

class ImageManager;
class Vessel;
class PlayerVessel;
class Item3D;
class Item3DChronometry;
class WiiManager;
class Timer;
class SoundManager;
class HashLabel;
class TurretItem3D;
class MoonItem3D;
class ScorePingVessel;

enum EVesselType { eGunShip, SemallShip};

using namespace std;

class GameLogic
{

public:

	GameLogic();

	void Init();

	void InitialiseGame();
	void InitialiseIntro();
	
	void DoGameLogic();
	void Intro();

	void AsteroidsLogic();
	void GunShipLogic();
	void SporesMovementLogic();

	void EnemySatelliteCollisionLogic();

	void MissileCollisionLogic();
	void FeoShieldLevelLogic();
	void PlayerCollisionLogic();
	void DisplayExplosions();
	void TurnShipLogic();
	void ExplosionLogic();
	void MissileLogic();

	void ProbeMineLogic( std::vector<Vessel>*  pVesselContainer = NULL, float ThrustPower = 0.05f, 	float ScanRange = 22.0f,float FrameSpeed = 0.05f) ;
	void RetrieveProbeMineLogic(float ThrustPower);
	void ProbeMineCollisionLogic();
	void BadShipsLogic();
	void BadShipsLogicForIntro();
	void ExhaustLogic();
	void ProjectileLogic();
	void CelestialBodyLogic();


	PlayerVessel* GetPlrVessel() { return m_MyVessel; }

	bool IsEndLevelTrigger() const { return m_bEndLevelTrigger; }
	void SetEndLevelTrigger(bool bState = true) { m_bEndLevelTrigger = bState; }

	
	void ClearBadContainer();

	guVector	m_CPUTarget;
	guVector	m_CPUTarget_Miss;

	u32 GetScore() { return m_Score; }
	void SetScore(u32 Value) { m_Score = Value; }
	void AddScore(Vessel* pVessel);

	void AddScorePing(Vessel* pVessel);
	void ScorePingLogic();

	void InitialiseMoonRocks(int Amount,float RadiusFactor = 0.0018f);
	void InitialiseSmallGunTurret(int Amount, float Dist ,float x1,float y1, float z1, float StartingAngleOffset = 0);

	// Mission section
	bool IsEnemyDestroyed();
	bool IsSalvagedShiledSatellites();
	bool IsJustOneShiledSatelliteLeftToSalvaged();
	bool IsSingleEnemyGunsipRemaining();	

	bool IsGamePaused()		{ return m_GamePaused|m_GamePausedByPlayer; }
	bool IsGamePausedByPopUp()		{ return m_GamePaused; }
	bool IsGamePausedByPlayer()		{ return m_GamePausedByPlayer; }
	void SetPaused(bool Status, bool GamePausedByPlayer = false) { m_GamePaused = Status; m_GamePausedByPlayer= GamePausedByPlayer;}


	// Shield Generator Section
	vector<Item3DChronometry>::iterator GetShieldGeneratorContainerBegin()	{ return m_ShieldGeneratorContainer->begin();}
	vector<Item3DChronometry>::iterator GetShieldGeneratorContainerEnd()	{ return m_ShieldGeneratorContainer->end();}
	vector<Item3DChronometry>::iterator EraseItemFromShieldGeneratorContainer(vector<Item3DChronometry>::iterator iter);
	int GetShieldGeneratorContainerSize(); // definition where it belongs, farward declaring most things for less includes (I care about compile time - crap PC)
	bool	IsShieldGeneratorContainerEmpty() { return m_ShieldGeneratorContainer->empty(); }

	// Gun Ship Section
	vector<Vessel>::iterator GetGunShipContainerBegin()	{ return m_GunShipContainer->begin();}
	vector<Vessel>::iterator GetGunShipContainerEnd()	{ return m_GunShipContainer->end();}
	bool	IsGunShipContainerEmpty() { return m_GunShipContainer->empty(); }
	int GetGunShipContainerSize();

	// PickUp Material
	vector<Item3D>::iterator GetMaterialPickUpContainerBegin()	{ return m_pMaterialPickUpContainer->begin();}
	vector<Item3D>::iterator GetMaterialPickUpContainerEnd()	{ return m_pMaterialPickUpContainer->end();}
	int GetMaterialPickUpContainerSize();

	// PickUp Health
	vector<Vessel>::iterator GetHealthPickUpContainerBegin()	{ return m_pHealthPickUpContainer->begin();}
	vector<Vessel>::iterator GetHealthPickUpContainerEnd()		{ return m_pHealthPickUpContainer->end();}
	int GetHealthPickUpContainerSize();

	// Small Enemies
	vector<Vessel>::iterator GetSmallEnemiesContainerBegin()	{ return m_SmallEnemiesContainer->begin();}
	vector<Vessel>::iterator GetSmallEnemiesContainerEnd()	{ return m_SmallEnemiesContainer->end();}
	bool	IsSmallEnemiesContainerEmpty() { return m_SmallEnemiesContainer->empty(); }
	int GetSmallEnemiesContainerSize();

	//Small Gun Turret
	vector<TurretItem3D>::iterator GetSmallGunTurretContainerBegin()	{ return m_pGunTurretContainer->begin();}
	vector<TurretItem3D>::iterator GetSmallGunTurretContainerEnd()		{ return m_pGunTurretContainer->end();}
	bool	IsSmallGunTurretContainerEmpty() { return m_pGunTurretContainer->empty(); }
	int GetSmallGunTurretContainerSize();

	// Shot For Gun Turret
	vector<Item3D>::iterator GetShotForGunTurretContainerBegin()	{ return m_ShotForGunTurretContainer->begin();}
	vector<Item3D>::iterator GetShotForGunTurretContainerEnd()		{ return m_ShotForGunTurretContainer->end();}
	int GetShotForGunTurretContainerSize();

	// Moon Rocks
	vector<Item3D>::iterator GetMoonRocksContainerBegin()	{ return m_pMoonRocksContainer->begin();}
	vector<Item3D>::iterator GetMoonRocksContainerEnd()		{ return m_pMoonRocksContainer->end();}
	int GetMoonRocksContainerSize();

	// Spores 
	vector<Vessel>::iterator GetSporesContainerBegin()		{ return m_SporesContainer->begin();}
	vector<Vessel>::iterator GetSporesContainerEnd()		{ return m_SporesContainer->end();}
	int GetSporesContainerSize() const;

	// Enemy Satellite
	vector<Vessel>::iterator GetEnemySatelliteContainerBegin()		{ return m_EnemySatelliteContainer->begin();}
	vector<Vessel>::iterator GetEnemySatelliteContainerEnd()		{ return m_EnemySatelliteContainer->end();}
	int GetEnemySatelliteContainerSize() const;

	// Probe Mine
	vector<Vessel>::iterator GetProbeMineContainerBegin()	{ return m_ProbeMineContainer->begin();}
	vector<Vessel>::iterator GetProbeMineContainerEnd()		{ return m_ProbeMineContainer->end();}
	int GetProbeMineContainerSize();

	// Missile
	vector<Vessel>::iterator GetMissileContainerBegin()		{ return m_MissileContainer->begin();}
	vector<Vessel>::iterator GetMissileContainerEnd()		{ return m_MissileContainer->end();}
	int GetMissileContainerSize();

	// Asteroid
	vector<Item3D>::iterator GetAsteroidContainerBegin()	{ return m_AsteroidContainer->begin();}
	vector<Item3D>::iterator GetAsteroidContainerEnd()		{ return m_AsteroidContainer->end();}
	int GetAsteroidContainerSize();

	// Projectile
	vector<Vessel>::iterator GetProjectileContainerBegin()	{ return m_ProjectileContainer->begin();}
	vector<Vessel>::iterator GetProjectileContainerEnd()	{ return m_ProjectileContainer->end();}
	int GetProjectileContainerSize();

	// Exhaust
	vector<Vessel>::iterator GetExhaustContainerBegin()		{ return m_ExhaustContainer->begin();}
	vector<Vessel>::iterator GetExhaustContainerEnd()		{ return m_ExhaustContainer->end();}
	int GetExhaustContainerSize();

	// Explosions
	vector<Vessel>::iterator GetExplosionsContainerBegin()		{ return m_ExplosionsContainer->begin();}
	vector<Vessel>::iterator GetExplosionsContainerEnd()		{ return m_ExplosionsContainer->end();}
	int GetExplosionsContainerSize();

	// AimPointer
	vector<guVector>::iterator GetAimPointerContainerBegin()	{ return m_AimPointerContainer->begin();}
	vector<guVector>::iterator GetAimPointerContainerEnd()		{ return m_AimPointerContainer->end();}
	int GetAimPointerContainerSise();

	// Dying Enemies
	vector<Vessel>::iterator GetDyingEnemiesContainerBegin()	{ return m_DyingEnemiesContainer->begin();}
	vector<Vessel>::iterator GetDyingEnemiesContainerEnd()		{ return m_DyingEnemiesContainer->end();}
	int GetDyingEnemiesContainerSize();

	// Celestial Body
	vector<MoonItem3D>::iterator GetCelestialBodyContainerBegin()	{ return m_CelestialBodyContainer->begin();}
	vector<MoonItem3D>::iterator GetCelestialBodyContainerEnd()		{ return m_CelestialBodyContainer->end();}
	int GetCelestialBodyContainerSize();

	// ScorePing
	vector<ScorePingVessel>::iterator GetScorePingContainerBegin()	{ return m_pScorePingContainer->begin();}
	vector<ScorePingVessel>::iterator GetScorePingContainerEnd()	{ return m_pScorePingContainer->end();}
	int GetScorePingContainerSize();

	// total enemies
	int GetTotalEnemiesContainerSize();

	// mission logic
	bool IsBaseShieldOnline() const { return m_IsBaseShieldOnline; }

	void InitMenu();


	// view clipping
	float GetClippingRadiusNeededForMoonRocks()	const { return m_ClippingRadiusNeededForMoonRocks; }

	void MoonRocksLogic();

	Vessel* GetGunTurretTarget(TurretItem3D* pTurret);

	void NukeArea(guVector& Pos, float fRadius );


	float FakLockOn_AngInc;

	float	m_TerraformingCounter;	

	bool IsBaseShieldOnline() { return m_IsBaseShieldOnline; }

private:



	void DyingShipsLogic();

	void AddAnim(HashLabel Frame, Item3D* pVessel, float FrameSpeed, float SpinAmount);
	void AddAnim(HashLabel Frame, Vessel* pVessel, float FrameSpeed, float SpinAmount);
	void AddScalingAnim(HashLabel Frame, Vessel* pVessel, float FrameSpeed, float SpinAmount,
							float TopScale, float ScaleStart, float ScaleFactor);

	void AddEnemy(float x, float y, HashLabel ShipType);
	void AddEnemy(float x, float y, float Velx, float Vely, HashLabel ShipType);
	void AddEnemy(int OriginX, int OriginY, int Amount, HashLabel ShipType, float Distance);
	void AddEnemySpawn(Vessel& item);

	void AddPickUps(Vessel* Position, int Amount);
	void PickUpsLogic();

	void AddHealthPickUps(Vessel* Position);
	void HealthPickUpsLogic();

	//void MoonRocksLogic();
	void GunTurretLogic();
	//void GunTurretShotsLogic();
	void GunTurretShotsLogic( std::vector<Vessel>* pEnemy );
	void InitialiseShieldGenerators(int Amount);
	void InitialiseEnermyAmardaArroundLastShieldGenerator(int Amount, float Distance);
	
	void StillAlive();

	void DoControls();

	ImageManager* m_pImageManager;

	std::vector<Item3DChronometry>* m_ShieldGeneratorContainer;
	std::vector<Item3D>* m_AsteroidContainer;
	std::vector<Vessel>* m_SporesContainer;

	std::vector<Vessel>* m_EnemySatelliteContainer;

	std::vector<Vessel>* m_MissileContainer;
	std::vector<Vessel>* m_ExplosionsContainer;
	std::vector<Vessel>* m_ProbeMineContainer;
	std::vector<Vessel>* m_GunShipContainer;
	std::vector<Vessel>* m_SmallEnemiesContainer;
	std::vector<Vessel>* m_ExhaustContainer;
	std::vector<Vessel>* m_ProjectileContainer;
	std::vector<guVector>* m_AimPointerContainer;
	std::vector<Item3D>* m_ShotForGunTurretContainer;
	std::vector<Vessel>* m_DyingEnemiesContainer;
	std::vector<MoonItem3D>* m_CelestialBodyContainer;
	std::vector<Item3D>* m_pMoonRocksContainer;
	std::vector<TurretItem3D>* m_pGunTurretContainer;
	std::vector<Item3D>* m_pMaterialPickUpContainer;
	std::vector<Vessel>* m_pHealthPickUpContainer;
	std::vector<ScorePingVessel>* m_pScorePingContainer;


	//bool			m_bAddMoreShipsFlag;
	bool			m_bEndLevelTrigger;
	u32				m_Score;
	float			m_ZoomAmountForSpaceBackground;
	float			m_ClippingRadiusNeededForMoonRocks;
	PlayerVessel*	m_MyVessel;
	bool			m_bDoEndGameEventOnce;
	Timer*			m_Timer;	
	bool			m_GamePaused;
	bool			m_GamePausedByPlayer;
	bool			m_IsBaseShieldOnline;

	WiiManager*		m_pWii;
	SoundManager*	m_pSoundManager;

	AESNDPB* m_LastChanUsedForSoundAfterBurn; 

	bool	m_bEnableRetrieveProbeMineMode;
};


#endif
