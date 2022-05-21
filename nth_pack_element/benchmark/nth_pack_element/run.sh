rm -f *.cpp *.out *.zig *.rs *.d *.out out *.dat number
python3 gen.py

for ((i=2; i <= 400; ++i)); do
  echo $i
  echo $i >> number
  /usr/bin/time -f %e clang++ -std=c++20 tuple$i.cpp &>> clang.dat
  /usr/bin/time -f %e circle tuple$i.circle.cpp &>> circle.dat
  /usr/bin/time -f %e rustc tuple$i.rs &>> rust.dat
  /usr/bin/time -f %e dmd tuple$i.d &>> dmd.dat
  /usr/bin/time -f %e zig-linux-x86_64-0.10.0-dev.2132+ea3f5905f/zig build-exe tuple$i.zig &>> zig.dat
done

echo -n "num " >> out
echo *.dat | sed "s/.dat//g"  >> out
paste -d ' ' number *.dat >> out
