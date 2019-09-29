perl -e '$|++;use Time::HiRes qw(usleep);while (1) {print("0,".(rand()-0.5)."\n"); usleep(25_000)}' | ../giraf
