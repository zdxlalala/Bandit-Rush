#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/zhangdongxu/Desktop/CPSC427/team02/build
  make -f /Users/zhangdongxu/Desktop/CPSC427/team02/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/zhangdongxu/Desktop/CPSC427/team02/build
  make -f /Users/zhangdongxu/Desktop/CPSC427/team02/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/zhangdongxu/Desktop/CPSC427/team02/build
  make -f /Users/zhangdongxu/Desktop/CPSC427/team02/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/zhangdongxu/Desktop/CPSC427/team02/build
  make -f /Users/zhangdongxu/Desktop/CPSC427/team02/build/CMakeScripts/ReRunCMake.make
fi

