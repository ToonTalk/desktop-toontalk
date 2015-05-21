// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __CLEANUP_H

#define __CLEANUP_H

void copyright_notice();
void tt_exit(int code, string reason, string data);
void tt_exit_failure(Trouble trouble, const_string reason=NULL, const_string reason2=NULL);
void cleanup_for_exit();
void release_queue_and_robots();
void reset_toontalk();
#if TT_ALPHA2
void recreate_after_reset();
#endif
#endif
