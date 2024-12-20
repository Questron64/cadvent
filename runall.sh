for day in ??.txt; do
  echo Day ${day%.*}
  for part in 01 02; do
    gcc -Wall -I../common ${day%.*}-$part.c ../common/*.c -lm && ./a.out <$day
  done
  echo
done
