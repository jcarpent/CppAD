#! /bin/bash -e
# -----------------------------------------------------------------------------
# CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-17 Bradley M. Bell
#
# CppAD is distributed under multiple licenses. This distribution is under
# the terms of the
#                     Eclipse Public License Version 1.0.
#
# A copy of this license is included in the COPYING file of this distribution.
# Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
# -----------------------------------------------------------------------------
spell_list='
'
revert_list='
speed/adolc/makefile.am
speed/cppad/makefile.am
speed/double/makefile.am
speed/fadbad/makefile.am
speed/profile/makefile.am
speed/sacado/makefile.am
'
move_list='
	introduction/exp_apx/main.cpp
'
move_sed='s|main.cpp|exp_apx.cpp|'
#
cat << EOF > junk.sed
s|^\\tmain.cpp\$|\\texp_apx.cpp|
s|^\\tmain.cpp \\\\\$|\\texp_apx.cpp \\\\|
s|exp_apx_main.cpp|exp_apx.cpp|g
s|head main.cpp|head Source|
s|exp_apx/main.cpp|exp_apx/exp_apx.cpp|
EOF
# -----------------------------------------------------------------------------
if [ $0 != "bin/batch_edit.sh" ]
then
	echo "bin/batch_edit.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
cp bin/batch_edit.sh $HOME/trash/batch_edit.sh
git reset --hard
# ---------------------------------------------------------------------------
list_all=`bin/ls_files.sh | sed \
	-e '/^compile$/d' \
	-e '/^config\.guess$/d' \
	-e '/^config\.sub$/d' \
	-e '/^configure$/d' \
	-e '/^depcomp$/d' \
	-e '/^epl-v10.txt$/d' \
	-e '/^gpl-v10.txt$/d' \
	-e '/^makefile.in$/d' \
	-e '/\/makefile.in$/d' \
	-e '/^missing$/d'`
edit_list=''
for file in $list_all
do
	if [ "$file" != 'bin/batch_edit.sh' ]
	then
		sed -f junk.sed  $file > junk.$$
		if ! diff $file junk.$$ > /dev/null
		then
			echo_eval sed -f junk.sed  -i $file
			edit_list="$edit_list $file"
		fi
	fi
done
if [ -e junk.$$ ]
then
	rm junk.$$
fi
# ----------------------------------------------------------------------------
for word in $spell_list
do
#
cat << EOF > junk.sed
/\$spell\$/! b skip
:loop
/\$\\\$/b check
N
b loop
#
: check
/$word/b skip
s/\$spell/&\\n\\t$word/
#
: skip
EOF
	for file in $edit_list
	do
		echo_eval sed -f junk.sed -i $file
	done
done
# ----------------------------------------------------------------------------
for old in $move_list
do
	new=`echo $old | sed -e "$move_sed"`
	if [ -e "$new" ]
	then
		rm -r "$new"
	fi
	dir=`echo $new | sed -e 's|/[^/]*$||'`
	if [ "$dir" != "$new" ] && [ ! -e "$dir" ]
	then
		echo_eval mkdir $dir
	fi
	echo_eval git mv $old $new
done
# ----------------------------------------------------------------------------
# files that were hand edited and cached using 'git_new.sh to'
if [ -e new ]
then
	echo_eval git_new.sh from
fi
# ----------------------------------------------------------------------------
for file in $revert_list
do
	echo_eval git checkout $file
done
# ----------------------------------------------------------------------------
cp $HOME/trash/batch_edit.sh bin/batch_edit.sh
echo "$0: OK"
exit 0
