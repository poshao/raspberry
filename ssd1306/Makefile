build:canvas.c ssd1306.c ssd1306.h
	gcc ./ssd1306.c ./canvas.c -o oled -lbcm2835

run:build
	sudo ./oled
	
clean:
	rm ./oled