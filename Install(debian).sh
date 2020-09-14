echo Updating packages to latest && \
sudo apt-get update >>instalog.log && echo Latest packages downloaded && \
sleep 2 && echo Installing dependencies && sudo apt-get install qt5-default \
qtmultimedia5-dev libsfml-dev g++ >>instalog.log && \
echo Compiling! && qmake >>instalog.log && make >>instalog.log && \
./Boris >>instalog.log && cat instalog.log | less
