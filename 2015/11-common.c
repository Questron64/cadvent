V increment_password(Z len, C pass[len], Z n) {
  if (n >= len)
    die("invalid index");
  if (pass[n] == 'z') {
    if (n == 0)
      die("cannot increment");
    pass[n] = 'a';
    increment_password(len, pass, n - 1);
  } else {
    pass[n]++;
  }
}

B has_straight(Z len, C pass[len]) {
  if (len < 3)
    return false;
  for (Z i = 0; i < len - 2; i++)
    if (pass[i] == pass[i + 1] - 1 && pass[i] == pass[i + 2] - 2)
      return true;
  return false;
}

B has_forbidden(Z len, C pass[len]) {
  for (Z i = 0; i < len; i++)
    if (pass[i] == 'i' || pass[i] == 'o' || pass[i] == 'l')
      return true;
  return false;
}

B has_pairs(Z len, C pass[len]) {
  if (len < 4)
    return false;
  for (Z i = 0; i < len - 3; i++)
    if (pass[i] == pass[i + 1])
      for (Z j = i + 2; j < len; j++)
        if (pass[j] == pass[j + 1])
          return true;
  return false;
}

B is_password_valid(Z len, C pass[len]) {
  return has_straight(len, pass) &&   //
         !has_forbidden(len, pass) && //
         has_pairs(len, pass);
}
