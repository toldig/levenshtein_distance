gcc ./src/main.c ./src/levenshtein.c ./include/levenshtein.h -Wall -Werror -Wpedantic -lpthread -g -o ./bin/lev
gcc ./src/test.c ./src/levenshtein.c ./include/levenshtein.h -Wall -Werror -Wpedantic -lpthread -g -lcunit -o ./bin/test_lev