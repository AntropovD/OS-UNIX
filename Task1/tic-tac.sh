#!/bin/bash

field=( 0 0 0 0 0 0 0 0 0 )
init_field=( 0 0 0 0 0 0 0 0 0 )

tput clear
tput civis

trap ctrl_c INT

function print_game_field()
{
	print_title

	tput cup 4 5; 
	print_field ${field[0]}
	echo -n " | "
	print_field ${field[1]}
	echo -n " | "
	print_field ${field[2]}
	tput cup 5 5
	echo "---------"
	
	tput cup 6 5
	print_field ${field[3]}
	echo -n " | "
	print_field ${field[4]}
	echo -n " | "
	print_field ${field[5]}
	tput cup 7 5
	echo "---------"
	
	tput cup 8 5
	print_field ${field[6]}
	echo -n " | "
	print_field ${field[7]}
	echo -n " | "
	print_field ${field[8]}	
	echo ""
}

function print_field()
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

if [ -p "tic_tac_pipe" ]
then
	side=1
else
	mkfifo "tic_tac_pipe"
	side=2
fi

while true
do 		
	print_game_field		
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
		field[$(($arg - 1))]=$side
		
	fi
	tput sgr0	
done 
