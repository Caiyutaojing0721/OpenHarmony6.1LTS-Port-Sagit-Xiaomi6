#!/bin/bash
# 用法: ./apply-patches.sh <目标源码树>
# 把 local-patches/ 里的 26 个 git diff 应用到指定源码树
set -u
SRC_ROOT=${1:?用法: $0 <目标源码树>}
HERE=$(cd "$(dirname "$0")" && pwd)
fail=0
while IFS=$'\t' read -r patch repo; do
  [ -z "$patch" ] && continue
  p="$HERE/local-patches/$patch"
  if [ ! -f "$p" ]; then echo "缺少 patch: $patch"; fail=1; continue; fi
  if [ ! -e "$SRC_ROOT/$repo" ]; then echo "缺少仓库: $repo"; fail=1; continue; fi
  if git -C "$SRC_ROOT/$repo" apply --check "$p" 2>/dev/null; then
    git -C "$SRC_ROOT/$repo" apply "$p" && echo "OK   $repo"
  else
    echo "FAIL $repo  -> 人工处理: $p"
    fail=1
  fi
done < "$HERE/local-patches/MAPPING.txt"
exit $fail
