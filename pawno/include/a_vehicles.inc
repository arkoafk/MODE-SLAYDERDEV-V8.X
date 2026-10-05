/*  SA-MP Vehicle Functions
 *
 *  (c) Copyright 2005-2012, SA-MP Team
 *
 */

#if defined _vehicles_included
	#endinput
#endif
#define _vehicles_included
#pragma library vehicles

#define CARMODTYPE_SPOILER		0
#define CARMODTYPE_HOOD			1
#define CARMODTYPE_ROOF			2
#define CARMODTYPE_SIDESKIRT	3
#define CARMODTYPE_LAMPS		4
#define CARMODTYPE_NITRO		5
#define CARMODTYPE_EXHAUST		6
#define CARMODTYPE_WHEELS		7
#define CARMODTYPE_STEREO		8
#define CARMODTYPE_HYDRAULICS	9
#define CARMODTYPE_FRONT_BUMPER	10
#define CARMODTYPE_REAR_BUMPER	11
#define CARMODTYPE_VENT_RIGHT	12
#define CARMODTYPE_VENT_LEFT	13

#define VEHICLE_PARAMS_UNSET	-1
#define VEHICLE_PARAMS_OFF		0
#define VEHICLE_PARAMS_ON		1

// Vehicle
native CreateVehicle(vehicletype, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0);
native DestroyVehicle(vehicleid);
native IsVehicleStreamedIn(vehicleid, forplayerid);
native GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleZAngle(vehicleid, &Float:z_angle);
native GetVehicleRotationQuat(vehicleid, &Float:w, &Float:x, &Float:y, &Float:z);
native Float:GetVehicleDistanceFromPoint(vehicleid, Float:X, Float:Y, Float:Z);
native SetVehicleZAngle(vehicleid, Float:z_angle);
native SetVehicleParamsForPlayer(vehicleid,playerid,objective,doorslocked);
native ManualVehicleEngineAndLights();
native SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
native GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
native GetVehicleParamsSirenState(vehicleid);
native SetVehicleParamsCarDoors(vehicleid, driver, passenger, backleft, backright);
native GetVehicleParamsCarDoors(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
native GetVehicleParamsCarWindows(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleToRespawn(vehicleid);
native LinkVehicleToInterior(vehicleid, interiorid);
native AddVehicleComponent(vehicleid, componentid);
native RemoveVehicleComponent(vehicleid, componentid);
native ChangeVehicleColor(vehicleid, color1, color2);
native ChangeVehiclePaintjob(vehicleid, paintjobid);
native SetVehicleHealth(vehicleid, Float:health);
native GetVehicleHealth(vehicleid, &Float:health);
native AttachTrailerToVehicle(trailerid, vehicleid);
native DetachTrailerFromVehicle(vehicleid);
native IsTrailerAttachedToVehicle(vehicleid);
native GetVehicleTrailer(vehicleid);
native SetVehicleNumberPlate(vehicleid, numberplate[]);
native GetVehicleModel(vehicleid);
native GetVehicleComponentInSlot(vehicleid, slot); // There is 1 slot for each CARMODTYPE_*
native GetVehicleComponentType(component); // Find CARMODTYPE_* for component id
native RepairVehicle(vehicleid); // Repairs the damage model and resets the health
native GetVehicleVelocity(vehicleid, &Float:X, &Float:Y, &Float:Z);
native SetVehicleVelocity(vehicleid, Float:X, Float:Y, Float:Z);
native SetVehicleAngularVelocity(vehicleid, Float:X, Float:Y, Float:Z);
native GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
native UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);

#define VEHICLE_MODEL_INFO_SIZE				1
#define VEHICLE_MODEL_INFO_FRONTSEAT		2
#define VEHICLE_MODEL_INFO_REARSEAT			3
#define VEHICLE_MODEL_INFO_PETROLCAP		4
#define VEHICLE_MODEL_INFO_WHEELSFRONT		5
#define VEHICLE_MODEL_INFO_WHEELSREAR		6
#define VEHICLE_MODEL_INFO_WHEELSMID		7
#define VEHICLE_MODEL_INFO_FRONT_BUMPER_Z	8
#define VEHICLE_MODEL_INFO_REAR_BUMPER_Z	9

native GetVehicleModelInfo(vehiclemodel, infotype, &Float:X, &Float:Y, &Float:Z);

// Virtual Worlds
native SetVehicleVirtualWorld(vehicleid, worldid);
native GetVehicleVirtualWorld(vehicleid);
/*
         OnPlayerAttachTrailer [v1.0]
      (c) Copyright 2013-2015 by Inferno
 
	  @author    : Inferno <inferno.piotr@gmail.com> (http://www.infus211.ct8.pl)
	  @website	 : www.PTSRP.pl
	  @date      : 14 may 2015
	  @update    : 14 may 2015

	  Instructions:
	  - Add this in oryginal publics:
	  		opat_OnPlayerUpdate(playerid); 
	  		opat_OnPlayerStateChange(playerid, newstate, oldstate);
	  		opat_OnPlayerConnect(playerid);
	  - Add anywhere in your gamemode new public OnPlayerAttachTrailer(playerid, vehicleid, trailerid, status)
	  - READY!
  
 */ 

#define ATTACHED_TRAILER 	0
#define DEATTACHED_TRAILER 	1

forward OnPlayerAttachTrailer(playerid, vehicleid, trailerid, status);

new opat_PlayerVehicleID[MAX_PLAYERS],
	opat_PlayerState[MAX_PLAYERS],
	opat_VehicleInfoTrailer[MAX_VEHICLES];

stock opat_OnPlayerUpdate(playerid) {
	if(opat_PlayerVehicleID[playerid] != -1 && opat_PlayerState[playerid] == PLAYER_STATE_DRIVER) {
		if(opat_VehicleInfoTrailer[ opat_PlayerVehicleID[playerid] ] == 0) {
			if(GetVehicleTrailer(opat_PlayerVehicleID[playerid]) != 0) {
				opat_VehicleInfoTrailer[ opat_PlayerVehicleID[playerid] ] = GetVehicleTrailer(opat_PlayerVehicleID[playerid]);
				OnPlayerAttachTrailer(playerid, opat_PlayerVehicleID[playerid], opat_VehicleInfoTrailer[ opat_PlayerVehicleID[playerid] ], ATTACHED_TRAILER);
			}
		} else {
			if(GetVehicleTrailer(opat_PlayerVehicleID[playerid]) == 0) {
				opat_VehicleInfoTrailer[ opat_PlayerVehicleID[playerid] ] = 0;
				OnPlayerAttachTrailer(playerid, playerid, opat_PlayerVehicleID[playerid], DEATTACHED_TRAILER);
			}
		}
	}
	return 1;
}

stock opat_OnPlayerStateChange(playerid, newstate, oldstate) {
	opat_PlayerState[playerid] = newstate;
	if(newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) {
		if(oldstate == PLAYER_STATE_ONFOOT) {
			opat_PlayerVehicleID[playerid] = GetPlayerVehicleID(playerid);
			opat_VehicleInfoTrailer[ opat_PlayerVehicleID[playerid] ] = GetVehicleTrailer(opat_PlayerVehicleID[playerid]);
		}
	}

	if(newstate == PLAYER_STATE_ONFOOT) {
		if(oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) {
			opat_PlayerVehicleID[playerid] = -1;
		}
	}
	return 1;
}

stock opat_OnPlayerConnect(playerid) {
	opat_PlayerVehicleID[playerid] = -1;
	return 1;
}