#!/bin/bash

FIELD=( 0 0 0 0 0 0 0 0 0 )
INIT_FIELD=( 0 0 0 0 0 0 0 0 0 )
PIPE="tic_toe_pipe"

tput clear
tput civis

trap ctrl_c INT
trap ctrl_z INT

function print_game_FIELD()
{
	print_title

	tput cup 4 5; 
	print_FIELD ${FIELD[0]}
	echo -n " | "
	print_FIELD ${FIELD[1]}
	echo -n " | "
	print_FIELD ${FIELD[2]}
	tput cup 5 5
	echo "---------"
	
	tput cup 6 5
	print_FIELD ${FIELD[3]}
	echo -n " | "
	print_FIELD ${FIELD[4]}
	echo -n " | "
	print_FIELD ${FIELD[5]}
	tput cup 7 5
	echo "---------"
	
	tput cup 8 5
	print_FIELD ${FIELD[6]}
	echo -n " | "
	print_FIELD ${FIELD[7]}
	echo -n " | "
	print_FIELD ${FIELD[8]}	
	echo ""
}

function print_FIELD()
{
	if [ $1 == "0" ]
	then
		echo -n " "
	fi
	if [ $1 == "1" ]
	then
		echo -n "O"
	fi	
	if [ $1 == "2" ]
	then
		echo -n "X"
	fi	
}

function print_title()
{
	tput cup 2 5
	tput setaf 3 
	echo "Tic-Tac-Toe"
	tput sgr0
}

function ctrl_c ()
{
	tput sgr0	
	tput cnorm
	clear
	find . -type p -delete	
	exit 1
}

function ctrl_z ()
{
	tput sgr0	
	tput cnorm
	clear
	find . -type p -delete	
	exit 1
}

if [ -p $PIPE ]
then
	SIDE=1
	ENEMY_SIDE=2
	TURN=2
else
	mkfifo $PIPE
	SIDE=2
	ENEMY_SIDE=1
	TURN=1
fi

while true
do 	
	if [ $TURN -eq 2 ]
	then
		read -r line < $PIPE
		if [[ "$line" =~ [0-9] ]]
		then
			FIELD[$(($arg - 1))]=$ENEMY_SIDE	
			TURN=1
		else				
			continue
		fi
	fi	
	
	print_game_FIELD		
	tput cup 10 3
	tput setaf 2	
	
	read -p "Enter your turn: " -n 1 arg
	tput cup 11 3
	echo "                    "	
	
	if ! [[ "$arg" =~ [0-9] ]]
	then	
		tput clear
		tput cup 11 3
		tput setaf 1
		echo "Coords must be [1-9]"
	else		
		FIELD[$(($arg - 1))]=$SIDE		
		echo $arg >$PIPE;
		TURN=2
	fi
	tput sgr0	
done 
