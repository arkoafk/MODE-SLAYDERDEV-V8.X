#if defined _SYSTEM_PICKUP
	#endinput
#endif
#define _SYSTEM_PICKUP

// ------------------------------------
#define GetPickupInfo(%0,%1)		g_pickup_info[%0][%1]
#define SetPickupInfo(%0,%1,%2)		g_pickup_info[%0][%1] = %2
#define ClearPickupInfo(%0)			g_pickup_info[%0] = g_pickup_default_values
#define IsPickupExists(%0)			g_pickup_info[%0][P_CREATED]
#define IsValidPickupID(%0)			(0 <= %0 < MAX_PICKUPS)

// ------------------------------------
#define PICKUP_ACTION_TYPE_NONE -1
#define PICKUP_ACTION_ID_NONE 	-1

// ------------------------------------
forward OnPlayerPickUpPickupEx(playerid, pickupid, action_type, action_id);

// ------------------------------------
enum E_PICKUP_STRUCT
{
	P_MODEL,
	P_TYPE, 
	Float: P_POS_X,
	Float: P_POS_Y,
	Float: P_POS_Z,
	P_VIRTUAL_WORLD,
	// -------------
	P_ACTION_TYPE,
	P_ACTION_ID,
	// -------------
	bool: P_CREATED
};
new g_pickup_info[MAX_PICKUPS][E_PICKUP_STRUCT];
new 
	g_pickup_default_values[E_PICKUP_STRUCT] = 
{
	0,
	0,
	0.0,
	0.0,
	0.0,
	0,
	PICKUP_ACTION_TYPE_NONE,
	PICKUP_ACTION_ID_NONE,
	false
};
new g_pickup_flood[MAX_PLAYERS];
//interaction menu
#define MAX_INTERMENU 2096
enum E_INTERMENU_DATA
{
	inter_UID,
	inter_ActionID,
	inter_ActionType,
	inter_Name[64],
	inter_AreaID
};
new 
	InterMenuData[MAX_INTERMENU][E_INTERMENU_DATA],
	player_InterMenu[MAX_PLAYERS][5] = {{-1, ...}, ...};
new Iterator:InteractionsMenu<MAX_INTERMENU>;

stock ClearInterMenu(id)
{
	InterMenuData[id][inter_UID] = -1;
	InterMenuData[id][inter_ActionID] = -1;
	InterMenuData[id][inter_ActionType] = -1;
	InterMenuData[id][inter_Name] = "";
	if(InterMenuData[id][inter_AreaID] != -1)
	{
		DestroyDynamicArea(InterMenuData[id][inter_AreaID]);
	}
	InterMenuData[id][inter_AreaID] = -1;
	Iter_Remove(InteractionsMenu, id);
	return 1;
}
stock CreateInterMenu(const name[], Float:x, Float:y, Float:z, Float:size, action_id, action_type = 0, worldid = -1, interiorid = -1, playerid = -1)
{
	new id = Iter_Free(InteractionsMenu);
	if(id != -1)
	{
		ClearInterMenu(id);
		InterMenuData[id][inter_UID] = id;
		InterMenuData[id][inter_ActionID] = action_id;
		InterMenuData[id][inter_ActionType] = action_type;
		SetString(InterMenuData[id][inter_Name], name);
		InterMenuData[id][inter_AreaID] = CreateDynamicSphere(x, y, z, size, worldid, interiorid, playerid);

		Iter_Add(InteractionsMenu, id);
	}
	return id;
}
stock DestroyInterMenu(&id)
{
	if(InterMenuData[id][inter_AreaID] != -1)
	{
		if(IsValidDynamicArea(InterMenuData[id][inter_AreaID]))
		{
			ClearInterMenu(id);
			id = -1;
			return 1;
		}
	}
	return 0;
}
stock player_InterMenu_Open(playerid, add_id)
{
	new count;
	str_1[0] = EOS;
	for(new i; i < 5; i++)
	{
		new id = player_InterMenu[playerid][i];

		if(id != -1) 
		{
			count++;
		}
		else
		{
			player_InterMenu[playerid][i] = add_id;
			id = add_id;
		}

		if(id != -1)
		{
			f(str_1, sizeof(str_1), "%s%s,", str_1, InterMenuData[id][inter_Name]);
		}
	}
	strdel(str_1, strlen(str_1)-1, strlen(str_1));

	if(!count) //default open
	{
		//ExecutePacket(playerid, webOpen, "const string[]", const string2[]="")
	}
	else
	{

	}
	return 1;
}
//
new
	InteractionMenu_ID[2096],
	InteractionMenu_ActionID[2096],
	InteractionMenu_ActionType[2096],
	InteractionMenu_String[2096][64];

new Iterator:InteractionMenu<4096>;
stock CreateInteractionMenu(const name[], Float:x, Float:y, Float:z, Float:size, action_id, action_id_2 = 0, worldid = -1, interiorid = -1, playerid = -1)
{
	new id = Iter_Free(InteractionMenu);
	InteractionMenu_ActionID[id] = action_id;
	InteractionMenu_ActionType[id] = action_id_2;
	SetString(InteractionMenu_String[id], name);
	InteractionMenu_ID[id] = CreateDynamicSphere(x, y, z, size, worldid, interiorid, playerid);
	Iter_Add(InteractionMenu, id);
	return id;
}
stock DestroyInteractionMenu(&id)
{
	if(InteractionMenu_ID[id] != -1)
	{
		if(IsValidDynamicArea(InteractionMenu_ID[id]))
		{
			DestroyDynamicArea(InteractionMenu_ID[id]);
			InteractionMenu_ID[id] = -1;
			Iter_Remove(InteractionMenu, id);
			id = -1;
		}
	}
}
/*enum E_INTERMENU_POS
{

};
new player_InterMenuEntered[MAX_PLAYERS][5];
static const InterMenu_Position[][E_INTERMENU_POS] = {
	{},
};*/
//
stock n_CreatePickup(model, type, Float:X, Float:Y, Float:Z, Virtualworld, action_type = PICKUP_ACTION_TYPE_NONE, action_id = PICKUP_ACTION_ID_NONE)
{
	if(X == 0.0 || Y == 0.0 || Z == 0.0) return -1;
	new n_pickupid = -1;
	n_pickupid = CreatePickup(model, type, X, Y, Z, Virtualworld);
	
	if(n_pickupid != -1)
	{
		SetPickupInfo(n_pickupid, P_MODEL, 	model);
		SetPickupInfo(n_pickupid, P_TYPE, 	type);
		
		SetPickupInfo(n_pickupid, P_POS_X, 	X);
		SetPickupInfo(n_pickupid, P_POS_Y, 	Y);
		SetPickupInfo(n_pickupid, P_POS_Z, 	Z);
		
		SetPickupInfo(n_pickupid, P_VIRTUAL_WORLD, Virtualworld);
		
		SetPickupInfo(n_pickupid, P_ACTION_TYPE, 	action_type);
		SetPickupInfo(n_pickupid, P_ACTION_ID, 		action_id);
		
		SetPickupInfo(n_pickupid, P_CREATED, true);
	}
	return n_pickupid; // The ID of the created pickup, -1 on failure (pickup max limit).
}
#if defined _ALS_CreatePickup
    #undef CreatePickup
#else
    #define _ALS_CreatePickup
#endif
#define CreatePickup n_CreatePickup
stock n_DestroyPickup(pickupid)
{
	if(IsPickupExists(pickupid))
	{
		ClearPickupInfo(pickupid);
	}
	return DestroyPickup(pickupid); // This function does not return any specific values;
}
#if defined _ALS_DestroyPickup
    #undef DestroyPickup
#else
    #define _ALS_DestroyPickup
#endif
#define DestroyPickup n_DestroyPickup

public OnGameModeInit()
{
    for(new idx; idx < MAX_PICKUPS; idx ++) ClearPickupInfo(idx);
	
#if defined n_OnGameModeInit
    n_OnGameModeInit();
#endif
    return 1;
}
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit n_OnGameModeInit
#if defined n_OnGameModeInit
forward n_OnGameModeInit();
#endif  