// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef TT_MAIN_H

#define TT_MAIN_H

boolean tt_initialize();
void tt_initialize_objects();
void open_tt_error_file();
//boolean report_initialization_problems(); 
boolean one_tt_cycle();
void delete_deleted_houses();
millisecond behind_schedule();
void start_time_travelling();
void standard_exit(boolean emergency=FALSE);
void update_clock();
#endif
