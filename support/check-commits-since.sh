#!/usr/bin/env bash

# Copyright (c) m8mble 2020.
# SPDX-License-Identifier: BSL-1.0

branch=`git rev-parse --abbrev-ref HEAD`
test_branch="compile-test"
old="$1"

if [[ "xx${old}xx" == "xxxx" ]]
then
   echo "Usage: <git-ident>"
   exit
fi

function build() {
    for compiler in g++ clang++;
    do
        DIR="cmake-compilability-${compiler}"
        cmake -E remove_directory ${DIR} || exit 1
        cmake -B${DIR} -DCMAKE_CXX_COMPILER="${compiler}" -G Ninja || exit 1
        cmake --build ${DIR} --parallel $(nproc) || exit 1
        cmake --build ${DIR} -- test || exit 1
    done
}

# git fetch cur

commit_count_cmd="git log --oneline ${old}..${branch}"
echo "Counting commits via ${commit_count_cmd}"
num_new_commits=$(${commit_count_cmd} | wc -l)

echo "Found ${num_new_commits} new commits.."

for commit in $(seq 0 ${num_new_commits})
do
   git branch -D ${test_branch}
   git checkout -b "${test_branch}" "${branch}~$(( ${num_new_commits} - ${commit} ))"
   ( build )
   ret_val=$?
   if [ ${ret_val} -ne 0 ]
   then
      echo "Commit ${commit} broke it!"
      echo "Info: `git log --oneline | head -1`"
      echo "Git command: git checkout ${test_branch}"
      exit
   fi
   git checkout ${branch}
done

git branch -d ${test_branch}
echo "All commits inbetween compile."
