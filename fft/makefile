fileName = testFile.exe

$(fileName): FFT.c test.c
	gcc FFT.c test.c -o $(fileName)

test: $(fileName)
	.\$(fileName)