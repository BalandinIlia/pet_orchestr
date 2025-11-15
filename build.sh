cd logger
chmod ++x build.sh
./build.sh
cd ..

cd math_core
chmod ++x build.sh
./build.sh
cd ..

cd networking_utils
chmod ++x build.sh
./build.sh
cd ..

cd networking_library
chmod ++x build.sh
./build.sh
cd ..

cd parameters
chmod ++x build.sh
./build.sh
cd ..

echo "Created libraries:"
cd libraries
ls
cd ..

cd client
chmod ++x build.sh
./build.sh
cd ..

cd server
chmod ++x build.sh
./build.sh
cd ..

echo "Created Executables"
cd executables
ls
mv network_exec ../docker
cd ..
