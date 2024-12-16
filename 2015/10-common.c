I look_and_say(C *in, Z in_len, I n) {
  if (in_len == 0)
    in_len = strlen(in);
  Z out_len = in_len * 10;
  C *out = alloc(0, out_len);

  I o = 0;
  for (Z i = 0; in[i];) {
    // count the run size
    I run = 1;
    while (in[i + run] == in[i])
      run++;

    // how many bytes needed?
    I len = digits(run) + 2; // including nul
    if(o + len >= out_len) {
      out_len *= 2;
      out = alloc(out, out_len);
    }

    // write result
    sprintf(&out[o], "%d%c", run, in[i]);

    i += run;
    o += len - 1;
  }

  in = alloc(in, 0);
  if (n > 1) {
    out = alloc(out, o + 1);
    return look_and_say(out, o, n - 1);
  } else {
    I result = strlen(out);
    out = alloc(out, 0);
    return result;
  }
}
