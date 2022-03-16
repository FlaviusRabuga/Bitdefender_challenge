build:
	gcc spam_detector.c -lm -o spam_detector
run:
	spam_detector
clean:
	rm spam_detector