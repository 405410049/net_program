all:h1_fork.c h1_select.c
	gcc h1_fork.c -o h1_fork
	gcc h1_select.c -o h1_select

