# GIFCycle

Links:

Blog (Explains how to set up everything)
https://adaoh3.blogspot.com/2023/07/cycle-gifs-with-arduino-and-tft-display.html

Image to C Converter
https://github.com/bitbank2/image_to_c

Steps for Mac:
1. Download X Code and its command line tools
2. Copy filename location of the image_to_c folder 
3. Open Terminal and paste "cd (pathlocation):
4. Run make
5. Put your GIF file into the image_to_c folder
6. In the terminal run ./image_to_c filename.gif > file.h
7. Your file should appear in a C format (Drag the file into your vscode folder under include)

For Windows:
1. Follow these steps (Timestamp 3:18 - 4:28)
2. Include the file into your project folder


Libraries used
1. https://github.com/Bodmer/TFT_eSPI (TFT_eSPI)
2. https://github.com/bitbank2/AnimatedGIF (Animated GIF)
