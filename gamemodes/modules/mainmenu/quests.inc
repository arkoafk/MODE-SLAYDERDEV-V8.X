cmd:quest(playerid, params[]){
	questinfo(playerid);
}
stock questinfo(playerid){
	SPD(
		playerid, 20490, DIALOG_STYLE_LIST,
		"Квесты",
		"[1] Текущий квест\n\
		[2] Статистика",
		"Далее", "Назад"
	);
}