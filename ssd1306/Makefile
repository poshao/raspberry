build:canvas.c ssd1306.c ssd1306.h
	gcc ./ssd1306.c ./canvas.c ./qrcode/qr_encode.c -o oled -lbcm2835 -lm

run:build
	sudo ./oled
	
clean:
	rm ./oled
