#!/bin/bash -i

FIELD=( 0 0 0 0 0 0 0 0 0 )
INIT_FIELD=( 0 0 0 0 0 0 0 0 0 )
pipe=/tmp/tic_pipe

tput clear
tput civis

trap ctrl_z SIGTSTP
trap ctrl_z INT

function print_game_FIELD(){
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

function print_FIELD(){
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

function print_title(){
	tput cup 1 5
	tput setaf 3 
	echo "Tic-Tac-Toe"
	tput sgr0
	
	tput cup 2 5
	tput setaf 5 
	if [[ $SIDE -eq 1 ]]; then		
		echo "You play Os"	
	fi
	if [[ $SIDE -eq 2 ]]; then
		echo "You play Xs"	
	fi
	tput sgr0
}

function ctrl_z(){	
	clear	
	tput sgr0	
	tput cnorm
	rm -f $pipe 2>/dev/null	
	stty echo
	exit 0
}

function check_win(){
	check_param 1
	check_param 2
	check_busy
}

function check_param(){
	flag=0
	if [[ "${FIELD[0]}" -eq $1 && "${FIELD[1]}" -eq $1 && "${FIELD[2]}" -eq $1 ]]; then
	
	
	if [[ $flag -eq 1]] then
		tput cup 12 5
		tput setaf 5
		echo "Os win!!!"	
		read -p "Press any key to exit."
		ctrl_z
	fi	
}

function check_busy(){
	if [[ "${FIELD[0]}" -ne 0 && "${FIELD[1]}" -ne 0 && "${FIELD[2]}" -ne 0
	&&  "${FIELD[3]}" -ne 0 && "${FIELD[4]}" -ne 0 && "${FIELD[5]}" -ne 0
	&& "${FIELD[6]}" -ne 0 && "${FIELD[7]}" -ne 0 && "${FIELD[8]}" -ne 0 ]]; then
		tput cup 12 5
		tput setaf 5
		echo "Draw!"	
		read -p "Press any key to exit."
		ctrl_z	
	fi
}
	

if [[ -p $pipe ]]; then
	SIDE=2
	ENEMY_SIDE=1
	TURN=2
else
	mkfifo $pipe
	SIDE=1
	ENEMY_SIDE=2
	TURN=1	
fi

while true; do 	
	print_game_FIELD
	
	if [[ $TURN -eq 1 ]]; then
		stty echo
		tput cup 10 3
		tput setaf 2		
		read -p "Enter your turn:         " -n 1 arg
		tput cup 11 3
		echo "                         "
		
		if ! [[ "$arg" =~ [0-9] ]]
		then	
			tput clear
			tput cup 11 3
			tput setaf 1
			echo "Coords must be [1-9]      "
		else			
			arg=$(( $arg - 1))
			if [[ "${FIELD[${arg}]}" -ne 0 ]]; then
				tput clear
				tput cup 11 3
				tput setaf 1
				echo "Field is not empty       "
			else
				FIELD[$arg]=$SIDE;					
				print_game_FIELD;
				echo "$arg" > "$pipe";								
				check_win
				TURN=2	
			fi
		fi		
	else	
		stty -echo
		tput cup 10 3
		tput setaf 1		
		echo "Wait another player move    "
		read -r arg < "$pipe"
		if [[ "$arg" =~ [0-9] ]]
		then
			FIELD[$arg]=$ENEMY_SIDE	
			print_game_FIELD;
			check_win
			TURN=1
		else					
			continue
		fi	
	fi
done
