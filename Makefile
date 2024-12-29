build:
  wcl findfour.cpp game.cpp lib/drivers.lib -ml

clean:
  rm -f *.exe
  rm -f *.obj