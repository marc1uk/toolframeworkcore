#!/bin/bash

#Application path location of applicaiton

Dependencies=`pwd`/Dependencies

export LD_LIBRARY_PATH=`pwd`/lib:${Dependencies}/ToolFramework/lib:$LD_LIBRARY_PATH

export SEGFAULT_SIGNALS="all"
