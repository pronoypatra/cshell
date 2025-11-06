all:
	gcc -g shell.c foreground.c prompt.c background.c discovery.c pinfo.c ls.c builtin.c history.c helper.c jobs.c autocomplete.c fgbg.c