if [ ! -d "$1" ]; then
	mkdir -p $1 2> /dev/null
	if [ $? -ne 0 ]; then
		echo "cannot make dir"
		exit
	else
		cp ../template/main.c $1
		cp ../template/test.desc $1
	fi
fi
tmux split-window -h "vim -o $1/main.c $1/test.desc"
