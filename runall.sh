for day in ??.txt; do
  echo Day ${day%.*}
  for part in 01 02; do
    gcc -Wall ${day%.*}-$part.c && ./a.out <$day
  done
  echo
done
