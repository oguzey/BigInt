#!/bin/bash

fail () {
	local msg=$1
	echo -e "\033[0;31m\n$msg\n\033[0m"
	exit -1
}

info(){
	local msg=$1
	echo -e "\033[0;32m\n$msg\n\033[0m"
}

make clean

build () {
	make "$1"
	if [ "$?" == 0 ]; then
		info "Build was successful"
		info "Run application"
		$2
		return $?
	else
		fail "Build end with fails"
	fi
}


if [ "$1" == "release" ]; then
	info "Start build release version of application"
	build "release" "./releaseApp"
elif [ "$1" == "debug" ] || [ -z ${1+x} ]; then
	info "Start build debug version of application"
	build "debug" "./debugApp"
elif [ "$1" == "test" ]; then
	info "Start build tests"
	build "test" "./testApp"
	if [ "$?" == 0 ]; then
		info "All tests passed"
	else
		fail "Tests failed"
	fi
else
	local str
	read -d '' str <<EOF
	Incorrect value of parameter was provided.
	Possible values: release, debug, test.
	If no one parameter was not provided, will use 'debug' as default.
EOF
	fail "$str"
fi

