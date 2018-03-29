g++ odroid_usb.cpp -std=c++11 -o output $(pkg-config --cflags --libs opencv)
./output

