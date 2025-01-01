rm build-output.log
ls game/*.cpp game/*.h \
   physics/*.cpp physics/*.h \
   utils/*.cpp utils/*.h \
   data/* \
   containers/*.cpp containers/*.h \
   main.cpp timer.cpp \
   CMakeLists.txt \
| entr -c sh -c 'ninja --quiet -C build-release SoftBodyPhysicsShared 2>&1 | tee -a build-output.log'