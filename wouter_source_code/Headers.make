#/bin/csh -f

set hdrs = headers.H

# preparation

touch $hdrs
\rm $hdrs
touch $hdrs

# search for header lines

foreach a (`ls -1 *.cpp | sed 's/\.cpp$//'`)
  set out = $a.H
  echo $out
  sed "s/xxx/$a/" header.hdr >! $out
  awk -f header.awk $a.cpp | sed 's/)/);/' >> $out
  cat header.ftr >> $out
  cat $a.H >> $hdrs
  echo     >> $hdrs
end
