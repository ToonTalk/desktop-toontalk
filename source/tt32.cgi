[directory stream]
DelayMaxSize=0

[open]
Fail=no

[OpenFile]
Fail=no

[_lread]
Access=no

[Options]
CodeGuard=no
Stats=no
ResourceLeakReport=no
Repeats=no
Append=yes

[memory block]
Leak=yes
DelayMaxSize=512

[object]
Leak=yes
DelayMaxSize=512

[object array]
Leak=yes
DelayMaxSize=512

[Windows global memory]
Leak=yes
DelayMaxSize=512

[Windows local memory]
Leak=yes
DelayMaxSize=512

[pipe stream]
DelayMaxSize=0

[file stream]
DelayMaxSize=0

[file handle]
DelayMaxSize=0

[Windows module]
DelayMaxSize=0

[LoadLibraryA]
Fail=no
