stock DebugLog(text[], type){
    switch(type){
        case 0:{
            return printf("[DEBUG BY SLAYDERDEV] %s | type: %d", text, type);
        }
    }
    return true;
}
cmd:debugtest(playerid, params[]){
	extract params -> new string:text[64], type; else return SendClientMessage(playerid, -1, "Используйте:/debugtest text type");
	DebugLog(text, type);
	return true;
}