#if defined _rakcheat_included
	#endinput
#endif
#define _rakcheat_included

#include <a_samp>

#if !defined PAWNRAKNET_INC_
	#error You must have Pawn.RakNet include in order to use this one.
#endif
#if !defined RAK_ENABLED_SHOT
	#define RAK_ENABLED_SHOT
#endif
#if !defined RAK_DEBUG
	#define RAK_DEBUG
#endif

#if !defined RAK_MAX_QUAT_WARNINGS
	#define RAK_MAX_QUAT_WARNINGS		1
#endif

#if !defined RAK_MAX_SURF_WARNINGS
	#define RAK_MAX_SURF_WARNINGS		30
#endif

#if !defined RAK_MAX_SURF_DIST_WARNINGS
	#define RAK_MAX_SURF_DIST_WARNINGS	30
#endif

#if __Pawn >= 0x3A && defined __PawnBuild
	#if __Pawn > 0x3A || __PawnBuild >= 2
		#define static_enum static enum
	#endif
#endif
#if !defined static_enum
	#define static_enum enum
#endif

#if __Pawn >= 0x3A && defined __PawnBuild
	#if __Pawn > 0x3A || __PawnBuild >= 2
		#define static_const(%0) static enum { %0 }
	#endif
#endif
#if !defined static_const
	#define static_const(%0) static const %0
#endif

static_enum
{
	RAK_INVALID_POSITION = 1,
	RAK_INVALID_QUATERNION,
	RAK_INVALID_VELOCITY,
	RAK_INVALID_SURFING_OFFSETS,
	RAK_INVALID_SURFING,
	RAK_INVALID_ANIMATION_FLAGS,
	RAK_INVALID_HEALTH,
	RAK_INVALID_TRAIN_SPEED,
	RAK_INVALID_ROLL,
	RAK_INVALID_DIRECTION,
	RAK_INVALID_ANGULAR_VELOCITY,
	RAK_INVALID_ORIGIN,
	RAK_INVALID_HITPOS,
	RAK_INVALID_OFFSETS,
	RAK_INVALID_CAMFRONTVEC,
	RAK_INVALID_CAMPOS,
	RAK_INVALID_AIMZ,
	RAK_INVALID_SHOT
}

static_const(ID_PLAYER_SYNC = 207);
static_const(ID_VEHICLE_SYNC = 200);
static_const(ID_PASSENGER_SYNC = 211);
static_const(ID_UNOCCUPIED_SYNC = 209);
static_const(ID_TRAILER_SYNC = 210);
static_const(ID_SPECTATOR_SYNC = 212);
static_const(ID_BULLET_SYNC = 206);
static_const(ID_AIM_SYNC = 203);
#if defined OnRakcheatFilteredData
	forward OnRakcheatFilteredData(playerid, id, const desc[]);
#endif
static_enum rakInfo
{
	rakQuatCount,
	rakSurfingCount,
	rakSurfingDistCount
}
static RAKInfo[MAX_PLAYERS][rakInfo];

#if !defined isnull
	#define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

static CheckValidityData(Float:data)
{
	if(floatcmp(data, data) != 0 || floatcmp(data, Float:0x7F800000) == 0 || floatcmp(data, Float:0xFF800000) == 0) return 1;
	return 0;
}

static CheckOutputLimit(Float:data, Float:max, Float:min = 0.0, bool:check = false)
{
	if(floatcmp(floatabs(data), max) > 0 || check && floatcmp(data, min) < 0) return 1;
	return 0;
}

stock KickWithCode(playerid, id, const desc[])
{
	if(!IsPlayerConnected(playerid)) return 0;
	#if defined RAK_DEBUG
		printf("[Rakcheat] Suspicious player ID %d. Reason -> code: %i, string: %s", playerid, id, desc);
	#endif
	OnRakcheatFilteredData(playerid, id, desc);
	return 0;
}
enum E_AC_NAME
{
	acCode,
	acName[24]
};
new const ac_Name[][E_AC_NAME] = {
	{-1, 	"Неизвестный"},
	{0, 	"Флуд"},
	{207, 	"Пешком"},
	{200, 	"В машине"},
	{211, 	"Пассажир"},
	{209,	"С машиной"},
	{210, 	"Трейлер"},
	{212, 	"Слежка"},
	{206,	"Стрельба"},
	{203, 	"Наводка"},
	{1, 	"Airbrake onfoot"},
	{2,		"Airbrake incar"},
	{3,		"dgun"},
	{4,		"AFK Ghost"},
	{5, 	"Teleport onfoot"},
	{6, 	"SpeedHack incar"},
	{7, 	"SpeedHack onfoot"}
};

static const ac_tick_callback[][] =
{
	{90, 12}, //EnterVehicle - ID: 26 //0
	{90, 12}, //ExitVehicle - ID: 154 //1
	{1000, 4}, //VehicleDestroyed - ID: 136 //2
	{700, 7}, //SendSpawn - ID: 52 //3
	{700, 6}, //DeathNotification - ID: 53 //4
	{600, 10} //DialogResponse - ID: 62 //5
};
new const ac_Punish_Name[3][17] = 
{ 
	{"{BC7677}Отключён"}, 
	{"{ae433d}Warning"}, 
	{"{FF6666}Кикнуть"} 
};
enum E_PLAYERDATA_AC
{
	acTick,
	acWorld_ID,
	acInterior_ID,
	acGtc[2],
	Float:acSpeed,
	acSet[2],
	Float:acSetPos[3],
	Float:acSpawnPos[3],
	bool:acSpawnUsed
};
new player_ACInfo[MAX_PLAYERS][E_PLAYERDATA_AC];
new 
	player_floodCallableCount[MAX_PLAYERS][sizeof(ac_tick_callback)],
	player_floodCallableTick[MAX_PLAYERS][sizeof(ac_tick_callback)],
	Float:player_PositionData[MAX_PLAYERS][3],
	player_World[MAX_PLAYERS],
	player_Interior[MAX_PLAYERS],
	ac_Type[sizeof(ac_Name)],
	ac_Trigger[sizeof(ac_Name)],
	player_AC_FloodTime[MAX_PLAYERS][sizeof(ac_Name)],
	player_AC_FloodWarning[MAX_PLAYERS][sizeof(ac_Name)];
	/*
	type ->
		0 - ignore
		1 - notif
		2 - kick
	*/
/*enum E_CONNECT_DATA
{
	conIp[17],
	conTime
};
new ConnectData[MAX_PLAYERS][E_CONNECT_DATA];
public OnIncomingConnection(playerid, ip_address[], port)
{

    return 1;
}*/
//callback
stock _TogglePlayerSpectating(playerid, toggle)
{
	player_ACInfo[playerid][acGtc][0] = GetTickCount() + 3850;
	TogglePlayerSpectating(playerid, toggle);
	return 1;
}
#if defined _ALS_TogglePlayerSpectating
    #undef TogglePlayerSpectating
#else
    #define _ALS_TogglePlayerSpectating
#endif

#define TogglePlayerSpectating _TogglePlayerSpectating
//
stock SearchAC_UID(code)
{
	for(new i; i < sizeof(ac_Name); i++)
	{
		if(ac_Name[i][acCode] == code) return i;
	}
	return 0;
}
public OnPlayerConnect(playerid)
{
	for(new i; i < sizeof(ac_tick_callback); i++)
	{
		player_floodCallableTick[playerid][i] = 0;
		player_floodCallableCount[playerid][i] = 0;
	}
	for(new i; i < 3; i++)
	{
		player_PositionData[playerid][i] = 0.0;
	}
	for(new i; i < sizeof(ac_Name); i++)
	{
		player_AC_FloodTime[playerid][i] = 0;
		player_AC_FloodWarning[playerid][i] = 0;
	}
	RAKInfo[playerid][rakQuatCount] =
	RAKInfo[playerid][rakSurfingCount] =
	RAKInfo[playerid][rakSurfingDistCount] = 0;

	#if defined rak_OnPlayerConnect
		return rak_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect rak_OnPlayerConnect
#if defined rak_OnPlayerConnect
	forward rak_OnPlayerConnect(playerid);
#endif


//setplayerpos	
stock _SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	player_UpdatePosAC(playerid, x, y, z);
	return SetPlayerPos(playerid, x, y, z);
}
#if defined _ALS_SetPlayerPos
    #undef SetPlayerPos
#else
    #define _ALS_SetPlayerPos
#endif

#define SetPlayerPos _SetPlayerPos
//setspawninfo
stock _SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
	new result = SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
	//player_UpdatePosAC(playerid, x, y, z);
	player_ACInfo[playerid][acSpawnPos][0] = x;
	player_ACInfo[playerid][acSpawnPos][1] = y;
	player_ACInfo[playerid][acSpawnPos][2] = z;
	player_ACInfo[playerid][acSpawnUsed] = true;
	return result;
}
#if defined _ALS_SetSpawnInfo
    #undef SetSpawnInfo
#else
    #define _ALS_SetSpawnInfo
#endif

#define SetSpawnInfo _SetSpawnInfo
//
public OnPlayerSpawn(playerid) // Сам паблик
{
	// Код, для которого происходит перехват
	#if defined test_OnPlayerSpawn // Проверяем, если паблик перехвачен, то...
		test_OnPlayerSpawn(playerid); // вызываем перехватчик
	#endif // завершаем
	if(player_ACInfo[playerid][acSpawnUsed] != false)
	{
		player_ACInfo[playerid][acSpawnUsed] = false;
		player_UpdatePosAC(playerid, player_ACInfo[playerid][acSpawnPos][0], player_ACInfo[playerid][acSpawnPos][1], player_ACInfo[playerid][acSpawnPos][2]);
	}
	return 1;
}
// Дальше в основном аналог перехвата нативных функций.
#if defined _ALS_OnPlayerSpawn //Если паблик с префиксом _ALS_ уже перехвачена, то..
    #undef OnPlayerSpawn // удаляем/раздефайневаем старый паблик
#else // если паблик с префиксом _ALS_ еще не перехвачен, то
    #define _ALS_OnPlayerSpawn // перехватываем паблик
#endif //завершаем перехват
#define OnPlayerSpawn test_OnPlayerSpawn // Теперь перехват паблик будет вызываться, при использовании test_OnPlayerSpawn
#if defined test_OnPlayerSpawn // Проверим, создан ли перехват
forward test_OnPlayerSpawn(playerid); // Создаем форвард для перехватчика
#endif //Завершаем
//
stock player_UpdatePosAC(playerid, Float:x, Float:y, Float:z)
{
	player_PositionData[playerid][0] = x;
	player_PositionData[playerid][1] = y;
	player_PositionData[playerid][2] = z;
	player_ACInfo[playerid][acSetPos][0] = x;
	player_ACInfo[playerid][acSetPos][1] = y;
	player_ACInfo[playerid][acSetPos][2] = z;
	player_ACInfo[playerid][acSet][0] = 1;
	player_ACInfo[playerid][acGtc][0] = GetTickCount() + 3850;
}
//
const ID_VehicleDestroyed = 136;//ID RPC
IRPC:ID_VehicleDestroyed(playerid, BitStream:bs)
{
    new vehicleid;
    BS_IgnoreBits(bs, 8);//игнорируем биты
    BS_ReadUint16(bs, vehicleid);//Считываем информацию

    //Делаем проверку
    if(vehicleid == 0)
    {
        return false; //Делаем десинхронизацию
    }
    return 1;
}
//
IPacket:ID_AIM_SYNC(playerid, BitStream:bs)
{
	new AIMSYNC[PR_AimSync];

	BS_IgnoreBits(bs, 8); //Игнорируем биты
	BS_ReadAimSync(bs, AIMSYNC);//Считываем информацию
	
  	//Выполняем проверку
 	if(AIMSYNC[PR_camFrontVec][0] != AIMSYNC[PR_camFrontVec][0] || floatcmp(floatabs(AIMSYNC[PR_camFrontVec][0]), 1.0000) == 1
		|| AIMSYNC[PR_camFrontVec][1] != AIMSYNC[PR_camFrontVec][1] || floatcmp(floatabs(AIMSYNC[PR_camFrontVec][1]), 1.0000) == 1
		|| AIMSYNC[PR_camFrontVec][2] != AIMSYNC[PR_camFrontVec][2] || floatcmp(floatabs(AIMSYNC[PR_camFrontVec][2]), 1.0000) == 1
		|| AIMSYNC[PR_camPos][0] != AIMSYNC[PR_camPos][0] || floatcmp(floatabs(AIMSYNC[PR_camPos][0]), 3500.0000) == 1
		|| AIMSYNC[PR_camPos][1] != AIMSYNC[PR_camPos][1] || floatcmp(floatabs(AIMSYNC[PR_camPos][1]), 3500.0000) == 1
		|| AIMSYNC[PR_camPos][2] != AIMSYNC[PR_camPos][2] || floatcmp(floatabs(AIMSYNC[PR_camPos][2]), 3500.0000) == 1)
	{
        	return false; // Делаем десинхронизацию
	}
    return 1;
}
//
const UNOCCUPIED_SYNC = 209;

IPacket:UNOCCUPIED_SYNC(playerid, BitStream:bs)
{
    new unoccupiedData[PR_UnoccupiedSync];
 
    BS_IgnoreBits(bs, 8);
    BS_ReadUnoccupiedSync(bs, unoccupiedData);
 
    if(floatcmp(floatabs(unoccupiedData[PR_roll][0]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_roll][1]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_roll][2]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_direction][0]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_direction][1]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_direction][2]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_position][0]), 20000.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_position][1]), 20000.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_position][2]), 20000.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_angularVelocity][0]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_angularVelocity][1]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_angularVelocity][2]), 1.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_velocity][0]), 100.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_velocity][1]), 100.00000) == 1
        || floatcmp(floatabs(unoccupiedData[PR_velocity][2]), 100.00000) == 1
    ) {
        return false;
    }

    return true;
}
//

stock DistancePointToPoint2D(Float: x, Float: y, Float: fx, Float:fy) return floatround(floatsqroot(floatpower(fx - x, 2) + floatpower(fy - y, 2)));
stock anticheat_OnPlayerUpdateEx(playerid)
{
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	new 
		ac_gtc = GetTickCount(),
		ac_gpp = GetPlayerPing(playerid),
		Float:ac_dist = DistancePointToPoint2D(x, y, player_PositionData[playerid][0], player_PositionData[playerid][1]);
	
	if(ac_gtc - player_ACInfo[playerid][acTick])
	{
		new Float:count = (player_PositionData[playerid][0] + player_PositionData[playerid][1] + player_PositionData[playerid][2]);
		if(count > 0)
		{
			if(player_Interior[playerid] == GetPlayerInterior(playerid) && player_World[playerid] == GetPlayerVirtualWorld(playerid))
			{
				//dgun
				new weapons[13][2];
				for(new i; i < 13; i++)
				{
					GetPlayerWeaponData(playerid, i, weapons[i][0], weapons[i][1]);
					switch(weapons[i][0])
					{
						case 16..18,35..40,44..45:
						{
							KickWithCode(playerid, 3, (str_f("slot: %i, weapon: %i", i, weapons[i][0])));
							ResetPlayerWeapons(playerid);
							break;
						}
					}
				}
				//player_ACInfo[playerid][acGtc][0] = GetTickCount() + 3850;
				//SCMf(playerid, -1, "%i/%i", ac_gtc - player_ACInfo[playerid][acGtc][0], ac_gpp);
				if(ac_gtc - player_ACInfo[playerid][acGtc][0] > ac_gpp)
				{
					//SCMf(playerid, -1, "%i/%i", ac_gtc - player_ACInfo[playerid][acGtc][0], ac_gpp);
					//teleport
					new ac_gps = GetPlayerState(playerid);
					switch(ac_gps)
					{
						case 1..3:
						{
							//SCMf(playerid, -1, "%i %f", player_ACInfo[playerid][acSet][0], player_ACInfo[playerid][acSetPos][0]);
							if(player_ACInfo[playerid][acSet][0] == 1)
							{
								player_ACInfo[playerid][acSet][0] = 0;
								if(player_ACInfo[playerid][acSetPos][0] != 0.0)
								{
									new Float:ac_dist_set = GetPlayerDistanceFromPoint(playerid, player_ACInfo[playerid][acSetPos][0], player_ACInfo[playerid][acSetPos][1], player_ACInfo[playerid][acSetPos][2]);
									if(ac_dist_set > 50.0)
									{
										KickWithCode(playerid, 5, (str_f("dist: %i", floatround(ac_dist_set))));
									}
								}
							}
							//
							if(ac_gps == 1)
							{
								new Float:speed_next = (ac_dist - player_ACInfo[playerid][acSpeed]);
								if(speed_next > 25.0)
								{
									KickWithCode(playerid, 7, (str_f("dist: %i, speed: %i", floatround(ac_dist), floatround(speed_next))));
								}
								if(ac_dist > 25.0 && ac_dist < 500)
								{
									new vehicleid = GetPlayerSurfingVehicleID(playerid),
										bool:detect;
									if(vehicleid == INVALID_VEHICLE_ID)
									{
										detect = true;
									}
									else if(!IsVehicleStreamedIn(vehicleid, playerid))
									{
										detect = true;
									}
									else
									{
										if(GetVehicleSpeed(vehicleid) < 30)
										{
											detect = true;
										}
									}
									if(detect)
									{
										player_AC_FloodWarning[playerid][1]++;
										if(player_AC_FloodWarning[playerid][1] > 1)
										{
											player_AC_FloodWarning[playerid][1] = 0;
											KickWithCode(playerid, 1, (str_f("dist: %i", floatround(ac_dist))));
										}
									}
								}
							}
							else if(ac_gps == 2 || ac_gps == 3)
							{
								new vehicleid = GetPlayerVehicleID(playerid);
								if(vehicleid > 0)
								{
									new
										speed = GetVehicleSpeed(vehicleid),
										Float:max_speed = 400.0;
									new
										uid = vehicle_SearchHandling(vehicleid);
									if(uid >= 0) //triazov
									{
										max_speed = (Handling_StaticData[uid][E_MAXGEAR_VELOCITY] + Handling_VehicleData[vehicleid][E_MAXGEAR_VELOCITY]);
									}
									//SCMf(playerid, -1, "%f/%f/%f", ac_dist, player_ACInfo[playerid][acSpeed], ac_dist - player_ACInfo[playerid][acSpeed]);
									//speed hack
									new Float:speed_next = (ac_dist - player_ACInfo[playerid][acSpeed]);
									if(speed_next > 80.0)
									{
										//SCMf(playerid, -1, "%i/%i", max_speed+120, speed);
										//if(max_speed+140 < speed)
										//{
										KickWithCode(playerid, 6, (str_f("dist: %i, speed: %i", floatround(ac_dist), floatround(speed_next))));
										//}
									}
									else if(max_speed+140 < speed && speed_next > 15)
									{
										KickWithCode(playerid, 2, (str_f("dist: %i, speed: %i", floatround(ac_dist), floatround(speed_next))));
									}
								}
							}
						}
					}
				}
			}
		}
	}
	player_World[playerid] = GetPlayerVirtualWorld(playerid);
	player_Interior[playerid] = GetPlayerInterior(playerid);
	player_PositionData[playerid][0] = x;
	player_PositionData[playerid][1] = y;
	player_PositionData[playerid][2] = z;
	player_ACInfo[playerid][acTick] = ac_gtc;
	player_ACInfo[playerid][acSpeed] = ac_dist;
}
stock CheckFloodCallback(playerid, id)
{
	if(GetTickCount()-player_floodCallableTick[playerid][id] < ac_tick_callback[id][0])
	{
		player_floodCallableCount[playerid][id]++;
		if(player_floodCallableCount[playerid][id] > ac_tick_callback[id][1] && NetStats_MessagesRecvPerSecond(playerid) > 200) 
		{
			printf("xxl guard debug -> playerid %i, count packets only 1 sec %i, id rpc flood %i, code 10", \
					playerid, NetStats_MessagesRecvPerSecond(playerid), id);
			KickWithCode(playerid, 0, (str_f("flood public: %i", id)));
		}
	}
	player_floodCallableTick[playerid][id] = GetTickCount();
	return 1;
}
public OnIncomingRPC(playerid, rpcid, BitStream:bs)
{
	switch(rpcid)
	{
		case 26: CheckFloodCallback(playerid, 0);
		case 154: CheckFloodCallback(playerid, 1);
		case 136: CheckFloodCallback(playerid, 2);
		case 52: CheckFloodCallback(playerid, 3);
		case 53: CheckFloodCallback(playerid, 4);
		case 62: CheckFloodCallback(playerid, 5);
	}
	return 1;
}
IPacket:ID_PLAYER_SYNC(playerid, BitStream:bs)
{
	new onFootData[PR_OnFootSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadOnFootSync(bs, onFootData);

	if(ac_Type[2])
	{
		if(!onFootData[PR_surfingVehicleId]) 
		{
			//SCMf(playerid, -1, "%f, %f, %f, %i", onFootData[PR_velocity][0], onFootData[PR_velocity][1], onFootData[PR_velocity][2], onFootData[PR_surfingVehicleId]);
		
			if(onFootData[PR_velocity][0] >= 1.3 || onFootData[PR_velocity][0] <= -1.3)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 0 | %1.f", onFootData[PR_velocity][0])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
			else if(onFootData[PR_velocity][1] >= 1.3 || onFootData[PR_velocity][1] <= -1.3)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 1 | %1.f", onFootData[PR_velocity][1])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
			else if(onFootData[PR_velocity][2] >= 0.5)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 2 | %1.f", onFootData[PR_velocity][2])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
		}
	}
	/*if(player_Speed > 0)
	{

	}*/
	if(CheckValidityData(onFootData[PR_position][0]) || CheckValidityData(onFootData[PR_position][1]) || CheckValidityData(onFootData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Position x, y, z: %f, %f, %f",
			onFootData[PR_position][0], onFootData[PR_position][1], onFootData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 0)));
	}

	if(CheckOutputLimit(onFootData[PR_position][0], 20000.0) || CheckOutputLimit(onFootData[PR_position][1], 20000.0) ||
	CheckOutputLimit(onFootData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Position x, y, z: %f, %f, %f",
			onFootData[PR_position][0], onFootData[PR_position][1], onFootData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 1)));
	}

	if(CheckValidityData(onFootData[PR_quaternion][0]) || CheckValidityData(onFootData[PR_quaternion][1]) || CheckValidityData(onFootData[PR_quaternion][2]) ||
	CheckValidityData(onFootData[PR_quaternion][3]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Quaternion w, x, y, z: %f, %f, %f, %f",
			onFootData[PR_quaternion][0], onFootData[PR_quaternion][1], onFootData[PR_quaternion][2], onFootData[PR_quaternion][3]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_QUATERNION, 0)));
	}

	new Float:rak_ang = atan2(floatmul(2.0, floatadd(floatmul(onFootData[PR_quaternion][2], onFootData[PR_quaternion][3]),
	floatmul(onFootData[PR_quaternion][0], onFootData[PR_quaternion][1]))), floatadd(floatsub(floatsub(floatpower(onFootData[PR_quaternion][0], 2.0),
	floatpower(onFootData[PR_quaternion][1], 2.0)), floatpower(onFootData[PR_quaternion][2], 2.0)),
	floatpower(onFootData[PR_quaternion][3], 2.0)));
	if(floatabs(rak_ang) > 85.0 && onFootData[PR_specialAction] == SPECIAL_ACTION_NONE)
	{
		if(++RAKInfo[playerid][rakQuatCount] > RAK_MAX_QUAT_WARNINGS)
		{
			#if defined RAK_DEBUG
				printf("[Rakcheat debug] [ID_PLAYER_SYNC] Quaternion w, x, y, z: %f, %f, %f, %f, angle: %f",
				onFootData[PR_quaternion][0], onFootData[PR_quaternion][1], onFootData[PR_quaternion][2], onFootData[PR_quaternion][3], rak_ang);
			#endif
			return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_QUATERNION, 2)));
		}
	}
	else RAKInfo[playerid][rakQuatCount] = 0;

	if(CheckValidityData(onFootData[PR_velocity][0]) || CheckValidityData(onFootData[PR_velocity][1]) || CheckValidityData(onFootData[PR_velocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Velocity x, y, z: %f, %f, %f",
			onFootData[PR_velocity][0], onFootData[PR_velocity][1], onFootData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_VELOCITY, 0)));
	}

	if(CheckOutputLimit(onFootData[PR_velocity][0], 100.0) || CheckOutputLimit(onFootData[PR_velocity][1], 100.0) || CheckOutputLimit(onFootData[PR_velocity][2], 100.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Velocity x, y, z: %f, %f, %f",
			onFootData[PR_velocity][0], onFootData[PR_velocity][1], onFootData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_VELOCITY, 1)));
	}

	if(CheckValidityData(onFootData[PR_surfingOffsets][0]) || CheckValidityData(onFootData[PR_surfingOffsets][1]) || CheckValidityData(onFootData[PR_surfingOffsets][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Surfing offsets x, y, z: %f, %f, %f",
			onFootData[PR_surfingOffsets][0], onFootData[PR_surfingOffsets][1], onFootData[PR_surfingOffsets][2]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING_OFFSETS, 0)));
	}

	if(CheckOutputLimit(onFootData[PR_surfingOffsets][0], 50.0) || CheckOutputLimit(onFootData[PR_surfingOffsets][1], 50.0) ||
	CheckOutputLimit(onFootData[PR_surfingOffsets][2], 50.0))
	{
		if(++RAKInfo[playerid][rakSurfingCount] > RAK_MAX_SURF_WARNINGS)
		{
			#if defined RAK_DEBUG
				printf("[Rakcheat debug] [ID_PLAYER_SYNC] Surfing offsets x, y, z: %f, %f, %f",
				onFootData[PR_surfingOffsets][0], onFootData[PR_surfingOffsets][1], onFootData[PR_surfingOffsets][2]);
			#endif
			return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING_OFFSETS, 1)));
		}
	}
	else RAKInfo[playerid][rakSurfingCount] = 0;

	if(onFootData[PR_surfingVehicleId] != 0)
	{
		if(0 < onFootData[PR_surfingVehicleId] < MAX_VEHICLES)
		{
			new rak_model = GetVehicleModel(onFootData[PR_surfingVehicleId]);
			if(rak_model == 0)
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_PLAYER_SYNC] Veh model: %d, vehicleid: %d",
					rak_model, onFootData[PR_surfingVehicleId]);
				#endif
				return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING, 0)));
			}
			else
			{
				new Float:rak_vX, Float:rak_vY, Float:rak_vZ;
				GetVehiclePos(onFootData[PR_surfingVehicleId], rak_vX, rak_vY, rak_vZ);
				rak_ang = VectorSize(floatsub(onFootData[PR_position][0], rak_vX), floatsub(onFootData[PR_position][1], rak_vY),
				floatsub(onFootData[PR_position][2], rak_vZ));
				if(floatcmp(rak_ang, 50.0) >= 0)
				{
					if(++RAKInfo[playerid][rakSurfingDistCount] > RAK_MAX_SURF_DIST_WARNINGS)
					{
						#if defined RAK_DEBUG
							printf("[Rakcheat debug] [ID_PLAYER_SYNC] Veh x, y, z: %f, %f, %f, player x, y, z: %f, %f, %f, distance: %f",
							rak_vX, rak_vY, rak_vZ, onFootData[PR_position][0], onFootData[PR_position][1], onFootData[PR_position][2], rak_ang);
						#endif
						return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING, 1)));
					}
				}
				else RAKInfo[playerid][rakSurfingDistCount] = 0;
			}
		}
		else if(MAX_VEHICLES < onFootData[PR_surfingVehicleId] < MAX_VEHICLES + MAX_OBJECTS)
		{
			onFootData[PR_surfingVehicleId] -= MAX_VEHICLES;
			new rak_tmp = IsValidObject(onFootData[PR_surfingVehicleId]);
			if(!rak_tmp && !IsValidPlayerObject(playerid, onFootData[PR_surfingVehicleId]))
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_PLAYER_SYNC] Objectid: %d", onFootData[PR_surfingVehicleId]);
				#endif
				return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING, 2)));
			}
			else
			{
				new Float:rak_vX, Float:rak_vY, Float:rak_vZ;
				if(rak_tmp) GetObjectPos(onFootData[PR_surfingVehicleId], rak_vX, rak_vY, rak_vZ);
				else GetPlayerObjectPos(playerid, onFootData[PR_surfingVehicleId], rak_vX, rak_vY, rak_vZ);
				rak_ang = VectorSize(floatsub(onFootData[PR_position][0], rak_vX), floatsub(onFootData[PR_position][1], rak_vY),
				floatsub(onFootData[PR_position][2], rak_vZ));
				if(floatcmp(rak_ang, 50.0) >= 0)
				{
					if(++RAKInfo[playerid][rakSurfingDistCount] > RAK_MAX_SURF_DIST_WARNINGS)
					{
						#if defined RAK_DEBUG
							printf("[Rakcheat debug] [ID_PLAYER_SYNC] Object x, y, z: %f, %f, %f, player x, y, z: %f, %f, %f, distance: %f",
							rak_vX, rak_vY, rak_vZ, onFootData[PR_position][0], onFootData[PR_position][1], onFootData[PR_position][2], rak_ang);
						#endif
						return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING, 3)));
					}
				}
				else RAKInfo[playerid][rakSurfingDistCount] = 0;
			}
		}
		else if(onFootData[PR_surfingVehicleId] != INVALID_VEHICLE_ID)
		{
			#if defined RAK_DEBUG
				printf("[Rakcheat debug] [ID_PLAYER_SYNC] Surfing: %d", onFootData[PR_surfingVehicleId]);
			#endif
			return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SURFING, 4)));
		}
	}

	/*if(onFootData[PR_animationFlags] == 0)
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PLAYER_SYNC] Animation flags: %d", onFootData[PR_animationFlags]);
		#endif
		return KickWithCode(playerid, ID_PLAYER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_ANIMATION_FLAGS, 0)));
	}*/
	return 1;
}

IPacket:ID_VEHICLE_SYNC(playerid, BitStream:bs)
{
	new inCarData[PR_InCarSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadInCarSync(bs, inCarData);

	if(ac_Type[3])
	{
		new vehicleid = inCarData[PR_vehicleId];
		
		//SCMf(playerid, -1, "%f, %f, %f", inCarData[PR_velocity][0], inCarData[PR_velocity][1], inCarData[PR_velocity][2]);
		//vehicle_MaxGearVelocity[vehicleid]
		//SCMf(playerid, -1, "%i/%f", floatround(floatsqroot(inCarData[PR_velocity][0]*inCarData[PR_velocity][0]+inCarData[PR_velocity][1]*inCarData[PR_velocity][1]+inCarData[PR_velocity][2]*inCarData[PR_velocity][2])*100), floatround(inCarData[PR_velocity][0]*inCarData[PR_velocity][1]*inCarData[PR_velocity][2]), vehicle_MaxGearVelocity[vehicleid]);
		if((inCarData[PR_velocity][0] >= 1.9 || inCarData[PR_velocity][0] <= -1.9) &&
			(inCarData[PR_velocity][1] >= 1.9 || inCarData[PR_velocity][1] <= -1.9))
		{
			KickWithCode(playerid, ID_VEHICLE_SYNC, (str_f("Rvanka: 0 | %1.f", inCarData[PR_velocity][2])));
			SetVehiclePos(vehicleid, inCarData[PR_position][0], inCarData[PR_position][1]+0.1, inCarData[PR_position][2]+1.0);
			return 0;
		}
	}
		/*else if(inCarData[PR_velocity][2] >= 1.1 || inCarData[PR_velocity][2] <= -1.9)
		{
			KickWithCode(playerid, ID_VEHICLE_SYNC, (str_f("Rvanka: 1 | %1.f", onFootData[PR_velocity][2])));
			SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
			return 0;
		}*/
		/*if(!onFootData[PR_surfingVehicleId]) 
		{
			//SCMf(playerid, -1, "%f, %f, %f, %i", onFootData[PR_velocity][0], onFootData[PR_velocity][1], onFootData[PR_velocity][2], onFootData[PR_surfingVehicleId]);
		
			if(onFootData[PR_velocity][0] >= 1.3 || onFootData[PR_velocity][0] <= -1.3)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 0 | %1.f", onFootData[PR_velocity][0])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
			else if(onFootData[PR_velocity][1] >= 1.3 || onFootData[PR_velocity][1] <= -1.3)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 1 | %1.f", onFootData[PR_velocity][1])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
			else if(onFootData[PR_velocity][2] >= 0.5)
			{
				KickWithCode(playerid, 207, (str_f("Rvanka: 2 | %1.f", onFootData[PR_velocity][2])));
				SetPlayerPos(playerid, onFootData[PR_position][0], onFootData[PR_position][1]+0.1, onFootData[PR_position][2]+1.0);
				return 0;
			}
		}*/
	/*if(CheckValidityData(inCarData[PR_quaternion][0]) || CheckValidityData(inCarData[PR_quaternion][1]) || CheckValidityData(inCarData[PR_quaternion][2]) ||
	CheckValidityData(inCarData[PR_quaternion][3]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Quaternion w, x, y, z: %f, %f, %f, %f",
			inCarData[PR_quaternion][0], inCarData[PR_quaternion][1], inCarData[PR_quaternion][2], inCarData[PR_quaternion][3]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_QUATERNION, 0);
	}

	if(CheckValidityData(inCarData[PR_position][0]) || CheckValidityData(inCarData[PR_position][1]) || CheckValidityData(inCarData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Position x, y, z: %f, %f, %f",
			inCarData[PR_position][0], inCarData[PR_position][1], inCarData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_POSITION, 0);
	}

	if(CheckOutputLimit(inCarData[PR_position][0], 20000.0) || CheckOutputLimit(inCarData[PR_position][1], 20000.0) ||
	CheckOutputLimit(inCarData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Position x, y, z: %f, %f, %f",
			inCarData[PR_position][0], inCarData[PR_position][1], inCarData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_POSITION, 1);
	}

	if(CheckValidityData(inCarData[PR_velocity][0]) || CheckValidityData(inCarData[PR_velocity][1]) || CheckValidityData(inCarData[PR_velocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Velocity x, y, z: %f, %f, %f",
			inCarData[PR_velocity][0], inCarData[PR_velocity][1], inCarData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_VELOCITY, 0);
	}

	if(CheckOutputLimit(inCarData[PR_velocity][0], 100.0) || CheckOutputLimit(inCarData[PR_velocity][1], 100.0) || CheckOutputLimit(inCarData[PR_velocity][2], 100.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Velocity x, y, z: %f, %f, %f",
			inCarData[PR_velocity][0], inCarData[PR_velocity][1], inCarData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_VELOCITY, 1);
	}

	if(CheckValidityData(inCarData[PR_vehicleHealth]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Veh health: %f", inCarData[PR_vehicleHealth]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_HEALTH, 0);
	}

	if(CheckValidityData(inCarData[PR_trainSpeed]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_VEHICLE_SYNC] Train speed: %f", inCarData[PR_trainSpeed]);
		#endif
		return KickWithCode(playerid, ID_VEHICLE_SYNC, RAK_INVALID_TRAIN_SPEED, 0);
	}*/
	return 1;
}

IPacket:ID_PASSENGER_SYNC(playerid, BitStream:bs)
{
	new passengerData[PR_PassengerSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadPassengerSync(bs, passengerData);
	if(ac_Type[4])
	{
//		new vehicleid = passengerData[PR_vehicleId];
		
		//SCMf(playerid, -1, "%f, %f, %f", inCarData[PR_velocity][0], inCarData[PR_velocity][1], inCarData[PR_velocity][2]);
		//vehicle_MaxGearVelocity[vehicleid]
		/*SCMf(playerid, -1, "%i/%f", floatround(floatsqroot(inCarData[PR_velocity][0]*inCarData[PR_velocity][0]+inCarData[PR_velocity][1]*inCarData[PR_velocity][1]+inCarData[PR_velocity][2]*inCarData[PR_velocity][2])*100), floatround(inCarData[PR_velocity][0]*inCarData[PR_velocity][1]*inCarData[PR_velocity][2]), vehicle_MaxGearVelocity[vehicleid]);
		if((inCarData[PR_velocity][0] >= 1.9 || inCarData[PR_velocity][0] <= -1.9) &&
			(inCarData[PR_velocity][1] >= 1.9 || inCarData[PR_velocity][1] <= -1.9))
		{
			KickWithCode(playerid, ID_VEHICLE_SYNC, (str_f("Rvanka: 0 | %1.f", inCarData[PR_velocity][2])));
			SetVehiclePos(vehicleid, inCarData[PR_position][0], inCarData[PR_position][1]+0.1, inCarData[PR_position][2]+1.0);
			return 0;
		}*/
	}
	/*if(CheckValidityData(passengerData[PR_position][0]) || CheckValidityData(passengerData[PR_position][1]) || CheckValidityData(passengerData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PASSENGER_SYNC] Position x, y, z: %f, %f, %f",
			passengerData[PR_position][0], passengerData[PR_position][1], passengerData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_PASSENGER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 0)));
	}

	if(CheckOutputLimit(passengerData[PR_position][0], 20000.0) || CheckOutputLimit(passengerData[PR_position][1], 20000.0) ||
	CheckOutputLimit(passengerData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_PASSENGER_SYNC] Position x, y, z: %f, %f, %f",
			passengerData[PR_position][0], passengerData[PR_position][1], passengerData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_PASSENGER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 1)));
	}*/
	return 1;
}

IPacket:ID_UNOCCUPIED_SYNC(playerid, BitStream:bs)
{
	/*new unoccupiedData[PR_UnoccupiedSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadUnoccupiedSync(bs, unoccupiedData);

	if(CheckValidityData(unoccupiedData[PR_roll][0]) || CheckValidityData(unoccupiedData[PR_roll][1]) || CheckValidityData(unoccupiedData[PR_roll][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Roll x, y, z: %f, %f, %f",
			unoccupiedData[PR_roll][0], unoccupiedData[PR_roll][1], unoccupiedData[PR_roll][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_ROLL, 0);
	}

	if(CheckOutputLimit(unoccupiedData[PR_roll][0], 1.0) || CheckOutputLimit(unoccupiedData[PR_roll][1], 1.0) || CheckOutputLimit(unoccupiedData[PR_roll][2], 1.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Roll x, y, z: %f, %f, %f",
			unoccupiedData[PR_roll][0], unoccupiedData[PR_roll][1], unoccupiedData[PR_roll][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_ROLL, 1);
	}

	if(CheckValidityData(unoccupiedData[PR_direction][0]) || CheckValidityData(unoccupiedData[PR_direction][1]) || CheckValidityData(unoccupiedData[PR_direction][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Direction x, y, z: %f, %f, %f",
			unoccupiedData[PR_direction][0], unoccupiedData[PR_direction][1], unoccupiedData[PR_direction][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_DIRECTION, 0);
	}

	if(CheckOutputLimit(unoccupiedData[PR_direction][0], 1.0) || CheckOutputLimit(unoccupiedData[PR_direction][1], 1.0) ||
	CheckOutputLimit(unoccupiedData[PR_direction][2], 1.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Direction x, y, z: %f, %f, %f",
			unoccupiedData[PR_direction][0], unoccupiedData[PR_direction][1], unoccupiedData[PR_direction][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_DIRECTION, 1);
	}

	if(CheckValidityData(unoccupiedData[PR_position][0]) || CheckValidityData(unoccupiedData[PR_position][1]) || CheckValidityData(unoccupiedData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Position x, y, z: %f, %f, %f",
			unoccupiedData[PR_position][0], unoccupiedData[PR_position][1], unoccupiedData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_POSITION, 0);
	}

	if(CheckOutputLimit(unoccupiedData[PR_position][0], 20000.0) || CheckOutputLimit(unoccupiedData[PR_position][1], 20000.0) ||
	CheckOutputLimit(unoccupiedData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Position x, y, z: %f, %f, %f",
			unoccupiedData[PR_position][0], unoccupiedData[PR_position][1], unoccupiedData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_POSITION, 1);
	}

	if(CheckValidityData(unoccupiedData[PR_velocity][0]) || CheckValidityData(unoccupiedData[PR_velocity][1]) || CheckValidityData(unoccupiedData[PR_velocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Velocity x, y, z: %f, %f, %f",
			unoccupiedData[PR_velocity][0], unoccupiedData[PR_velocity][1], unoccupiedData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_VELOCITY, 0);
	}

	if(CheckOutputLimit(unoccupiedData[PR_velocity][0], 100.0) || CheckOutputLimit(unoccupiedData[PR_velocity][1], 100.0) ||
	CheckOutputLimit(unoccupiedData[PR_velocity][2], 100.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Velocity x, y, z: %f, %f, %f",
			unoccupiedData[PR_velocity][0], unoccupiedData[PR_velocity][1], unoccupiedData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_VELOCITY, 1);
	}

	if(CheckValidityData(unoccupiedData[PR_angularVelocity][0]) || CheckValidityData(unoccupiedData[PR_angularVelocity][1]) ||
	CheckValidityData(unoccupiedData[PR_angularVelocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Angular velocity x, y, z: %f, %f, %f",
			unoccupiedData[PR_angularVelocity][0], unoccupiedData[PR_angularVelocity][1], unoccupiedData[PR_angularVelocity][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_ANGULAR_VELOCITY, 0);
	}

	if(CheckOutputLimit(unoccupiedData[PR_angularVelocity][0], 1.0) || CheckOutputLimit(unoccupiedData[PR_angularVelocity][1], 1.0) ||
	CheckOutputLimit(unoccupiedData[PR_angularVelocity][2], 1.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Angular velocity x, y, z: %f, %f, %f",
			unoccupiedData[PR_angularVelocity][0], unoccupiedData[PR_angularVelocity][1], unoccupiedData[PR_angularVelocity][2]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_ANGULAR_VELOCITY, 1);
	}

	if(CheckValidityData(unoccupiedData[PR_vehicleHealth]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_UNOCCUPIED_SYNC] Veh health: %f", unoccupiedData[PR_vehicleHealth]);
		#endif
		return KickWithCode(playerid, ID_UNOCCUPIED_SYNC, RAK_INVALID_HEALTH, 0);
	}*/
	return 1;
}

IPacket:ID_TRAILER_SYNC(playerid, BitStream:bs)
{
	new trailerData[PR_TrailerSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadTrailerSync(bs, trailerData);

	if(CheckValidityData(trailerData[PR_position][0]) || CheckValidityData(trailerData[PR_position][1]) || CheckValidityData(trailerData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Position x, y, z: %f, %f, %f",
			trailerData[PR_position][0], trailerData[PR_position][1], trailerData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 0)));
	}

	if(CheckOutputLimit(trailerData[PR_position][0], 20000.0) || CheckOutputLimit(trailerData[PR_position][1], 20000.0) ||
	CheckOutputLimit(trailerData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Position x, y, z: %f, %f, %f",
			trailerData[PR_position][0], trailerData[PR_position][1], trailerData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 1)));
	}

	if(CheckValidityData(trailerData[PR_quaternion][0]) || CheckValidityData(trailerData[PR_quaternion][1]) || CheckValidityData(trailerData[PR_quaternion][2]) ||
	CheckValidityData(trailerData[PR_quaternion][3]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Quaternion w, x, y, z: %f, %f, %f, %f",
			trailerData[PR_quaternion][0], trailerData[PR_quaternion][1], trailerData[PR_quaternion][2], trailerData[PR_quaternion][3]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_QUATERNION, 0)));
	}

	if(CheckValidityData(trailerData[PR_velocity][0]) || CheckValidityData(trailerData[PR_velocity][1]) || CheckValidityData(trailerData[PR_velocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Velocity x, y, z: %f, %f, %f",
			trailerData[PR_velocity][0], trailerData[PR_velocity][1], trailerData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_VELOCITY, 0)));
	}

	if(CheckOutputLimit(trailerData[PR_velocity][0], 100.0) || CheckOutputLimit(trailerData[PR_velocity][1], 100.0) ||
	CheckOutputLimit(trailerData[PR_velocity][2], 100.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Velocity x, y, z: %f, %f, %f",
			trailerData[PR_velocity][0], trailerData[PR_velocity][1], trailerData[PR_velocity][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_VELOCITY, 1)));
	}

	if(CheckValidityData(trailerData[PR_angularVelocity][0]) || CheckValidityData(trailerData[PR_angularVelocity][1]) ||
	CheckValidityData(trailerData[PR_angularVelocity][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Angular velocity x, y, z: %f, %f, %f",
			trailerData[PR_angularVelocity][0], trailerData[PR_angularVelocity][1], trailerData[PR_angularVelocity][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_ANGULAR_VELOCITY, 0)));
	}

	if(CheckOutputLimit(trailerData[PR_angularVelocity][0], 1.0) || CheckOutputLimit(trailerData[PR_angularVelocity][1], 1.0) ||
	CheckOutputLimit(trailerData[PR_angularVelocity][2], 1.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_TRAILER_SYNC] Angular velocity x, y, z: %f, %f, %f",
			trailerData[PR_angularVelocity][0], trailerData[PR_angularVelocity][1], trailerData[PR_angularVelocity][2]);
		#endif
		return KickWithCode(playerid, ID_TRAILER_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_ANGULAR_VELOCITY, 1)));
	}
	return 1;
}

IPacket:ID_SPECTATOR_SYNC(playerid, BitStream:bs)
{
	new spectatorData[PR_SpectatingSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadSpectatingSync(bs, spectatorData);

	if(CheckValidityData(spectatorData[PR_position][0]) || CheckValidityData(spectatorData[PR_position][1]) || CheckValidityData(spectatorData[PR_position][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_SPECTATOR_SYNC] Position x, y, z: %f, %f, %f",
			spectatorData[PR_position][0], spectatorData[PR_position][1], spectatorData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_SPECTATOR_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 0)));
	}

	if(CheckOutputLimit(spectatorData[PR_position][0], 20000.0) || CheckOutputLimit(spectatorData[PR_position][1], 20000.0) ||
	CheckOutputLimit(spectatorData[PR_position][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_SPECTATOR_SYNC] Position x, y, z: %f, %f, %f",
			spectatorData[PR_position][0], spectatorData[PR_position][1], spectatorData[PR_position][2]);
		#endif
		return KickWithCode(playerid, ID_SPECTATOR_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_POSITION, 1)));
	}
	return 1;
}

IPacket:ID_BULLET_SYNC(playerid, BitStream:bs)
{
	new bulletData[PR_BulletSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadBulletSync(bs, bulletData);

	if(CheckValidityData(bulletData[PR_origin][0]) || CheckValidityData(bulletData[PR_origin][1]) || CheckValidityData(bulletData[PR_origin][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_BULLET_SYNC] Origin x, y, z: %f, %f, %f",
			bulletData[PR_origin][0], bulletData[PR_origin][1], bulletData[PR_origin][2]);
		#endif
		return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_ORIGIN, 0)));
	}

	if(CheckOutputLimit(bulletData[PR_origin][0], 20000.0) || CheckOutputLimit(bulletData[PR_origin][1], 20000.0) || CheckOutputLimit(bulletData[PR_origin][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_BULLET_SYNC] Origin x, y, z: %f, %f, %f",
			bulletData[PR_origin][0], bulletData[PR_origin][1], bulletData[PR_origin][2]);
		#endif
		return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_ORIGIN, 1)));
	}

	if(CheckValidityData(bulletData[PR_hitPos][0]) || CheckValidityData(bulletData[PR_hitPos][1]) || CheckValidityData(bulletData[PR_hitPos][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_BULLET_SYNC] Hitpos x, y, z: %f, %f, %f",
			bulletData[PR_hitPos][0], bulletData[PR_hitPos][1], bulletData[PR_hitPos][2]);
		#endif
		return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_HITPOS, 0)));
	}

	if(CheckOutputLimit(bulletData[PR_hitPos][0], 20000.0) || CheckOutputLimit(bulletData[PR_hitPos][1], 20000.0) || CheckOutputLimit(bulletData[PR_hitPos][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_BULLET_SYNC] Hitpos x, y, z: %f, %f, %f",
			bulletData[PR_hitPos][0], bulletData[PR_hitPos][1], bulletData[PR_hitPos][2]);
		#endif
		return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_HITPOS, 1)));
	}

	if(CheckValidityData(bulletData[PR_offsets][0]) || CheckValidityData(bulletData[PR_offsets][1]) || CheckValidityData(bulletData[PR_offsets][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_BULLET_SYNC] Offsets x, y, z: %f, %f, %f",
			bulletData[PR_offsets][0], bulletData[PR_offsets][1], bulletData[PR_offsets][2]);
		#endif
		return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_OFFSETS, 0)));
	}

	#if defined RAK_ENABLED_SHOT
		if(bulletData[PR_hitType] != BULLET_HIT_TYPE_NONE)
		{
			new Float:rak_X, Float:rak_Y, Float:rak_Z;
			GetPlayerPos(playerid, rak_X, rak_Y, rak_Z);
			if(floatcmp(bulletData[PR_origin][0], rak_X) == 0 || floatcmp(bulletData[PR_origin][1], rak_Y) == 0 || floatcmp(bulletData[PR_origin][2], rak_Z) == 0)
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_BULLET_SYNC] Pos x, y, z: %f, %f, %f, sync x, y, z: %f, %f, %f",
					rak_X, rak_Y, rak_Z, bulletData[PR_origin][0], bulletData[PR_origin][1], bulletData[PR_origin][2]);
				#endif
				return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SHOT, 0)));
			}
		}
	#endif

	switch(bulletData[PR_hitType])
	{
		case BULLET_HIT_TYPE_PLAYER:
		{
			if(CheckOutputLimit(bulletData[PR_offsets][0], 2.0) || CheckOutputLimit(bulletData[PR_offsets][1], 2.0) || CheckOutputLimit(bulletData[PR_offsets][2], 2.0))
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_BULLET_SYNC] Offsets x, y, z: %f, %f, %f",
					bulletData[PR_offsets][0], bulletData[PR_offsets][1], bulletData[PR_offsets][2]);
				#endif
				return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_OFFSETS, 1)));
			}

			#if defined RAK_ENABLED_SHOT
				if(bulletData[PR_hitId] != INVALID_PLAYER_ID)
				{
					new Float:rak_vX, Float:rak_vY, Float:rak_vZ;
					GetPlayerPos(bulletData[PR_hitId], rak_vX, rak_vY, rak_vZ);
					if(floatcmp(bulletData[PR_hitPos][0], rak_vX) == 0 || floatcmp(bulletData[PR_hitPos][1], rak_vY) == 0 ||
					floatcmp(bulletData[PR_hitPos][2], rak_vZ) == 0)
					{
						#if defined RAK_DEBUG
							printf("[Rakcheat debug] [ID_BULLET_SYNC] Pos x, y, z: %f, %f, %f, sync x, y, z: %f, %f, %f",
							rak_vX, rak_vY, rak_vZ, bulletData[PR_hitPos][0], bulletData[PR_hitPos][1], bulletData[PR_hitPos][2]);
						#endif
						return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_OFFSETS, 1)));
					}
				}
			#endif
		}
		case BULLET_HIT_TYPE_VEHICLE:
		{
			if(CheckOutputLimit(bulletData[PR_offsets][0], 300.0) || CheckOutputLimit(bulletData[PR_offsets][1], 300.0) ||
			CheckOutputLimit(bulletData[PR_offsets][2], 300.0))
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_BULLET_SYNC] Offsets x, y, z: %f, %f, %f",
					bulletData[PR_offsets][0], bulletData[PR_offsets][1], bulletData[PR_offsets][2]);
				#endif
				return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_OFFSETS, 2)));
			}

			#if defined RAK_ENABLED_SHOT
				if(bulletData[PR_hitId] != INVALID_VEHICLE_ID)
				{
					new Float:rak_vX, Float:rak_vY, Float:rak_vZ;
					GetVehiclePos(bulletData[PR_hitId], rak_vX, rak_vY, rak_vZ);
					if(floatcmp(bulletData[PR_hitPos][0], rak_vX) == 0 || floatcmp(bulletData[PR_hitPos][1], rak_vY) == 0 ||
					floatcmp(bulletData[PR_hitPos][2], rak_vZ) == 0)
					{
						#if defined RAK_DEBUG
							printf("[Rakcheat debug] [ID_BULLET_SYNC] Pos x, y, z: %f, %f, %f, sync x, y, z: %f, %f, %f",
							rak_vX, rak_vY, rak_vZ, bulletData[PR_hitPos][0], bulletData[PR_hitPos][1], bulletData[PR_hitPos][2]);
						#endif
						return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SHOT, 1)));
					}
				}
			#endif
		}
		default:
		{
			if(CheckOutputLimit(bulletData[PR_offsets][0], 20000.0) || CheckOutputLimit(bulletData[PR_offsets][1], 20000.0) ||
			CheckOutputLimit(bulletData[PR_offsets][2], 20000.0))
			{
				#if defined RAK_DEBUG
					printf("[Rakcheat debug] [ID_BULLET_SYNC] Offsets x, y, z: %f, %f, %f",
					bulletData[PR_offsets][0], bulletData[PR_offsets][1], bulletData[PR_offsets][2]);
				#endif
				return KickWithCode(playerid, ID_BULLET_SYNC, (str_f("packet: %i, type: %i", RAK_INVALID_SHOT, 3)));
			}
		}
	}
	return 1;
}

/*IPacket:ID_AIM_SYNC(playerid, BitStream:bs)
{
	new aimData[PR_AimSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadAimSync(bs, aimData);

	if(CheckValidityData(aimData[PR_camFrontVec][0]) || CheckValidityData(aimData[PR_camFrontVec][1]) || CheckValidityData(aimData[PR_camFrontVec][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_AIM_SYNC] Camfrontvec x, y, z: %f, %f, %f",
			aimData[PR_camFrontVec][0], aimData[PR_camFrontVec][1], aimData[PR_camFrontVec][2]);
		#endif
		return KickWithCode(playerid, ID_AIM_SYNC, RAK_INVALID_CAMFRONTVEC, 0);
	}

	if(CheckOutputLimit(aimData[PR_camFrontVec][0], 1.0) || CheckOutputLimit(aimData[PR_camFrontVec][1], 1.0) || CheckOutputLimit(aimData[PR_camFrontVec][2], 1.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_AIM_SYNC] Camfrontvec x, y, z: %f, %f, %f",
			aimData[PR_camFrontVec][0], aimData[PR_camFrontVec][1], aimData[PR_camFrontVec][2]);
		#endif
		return KickWithCode(playerid, ID_AIM_SYNC, RAK_INVALID_CAMFRONTVEC, 1);
	}

	if(CheckValidityData(aimData[PR_camPos][0]) || CheckValidityData(aimData[PR_camPos][1]) || CheckValidityData(aimData[PR_camPos][2]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_AIM_SYNC] Campos x, y, z: %f, %f, %f",
			aimData[PR_camPos][0], aimData[PR_camPos][1], aimData[PR_camPos][2]);
		#endif
		return KickWithCode(playerid, ID_AIM_SYNC, RAK_INVALID_CAMPOS, 0);
	}

	if(CheckOutputLimit(aimData[PR_camPos][0], 20000.0) || CheckOutputLimit(aimData[PR_camPos][1], 20000.0) || CheckOutputLimit(aimData[PR_camPos][2], 20000.0))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_AIM_SYNC] Campos x, y, z: %f, %f, %f",
			aimData[PR_camPos][0], aimData[PR_camPos][1], aimData[PR_camPos][2]);
		#endif
		return KickWithCode(playerid, ID_AIM_SYNC, RAK_INVALID_CAMPOS, 1);
	}

	if(CheckValidityData(aimData[PR_aimZ]))
	{
		#if defined RAK_DEBUG
			printf("[Rakcheat debug] [ID_AIM_SYNC] Aimz: %f", aimData[PR_aimZ]);
		#endif
		return KickWithCode(playerid, ID_AIM_SYNC, RAK_INVALID_AIMZ, 0);
	}
	return 1;
}*/