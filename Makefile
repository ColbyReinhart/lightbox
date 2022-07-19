lightbox: lightbox.h lightbox.c utilities.c
	cc -o lightbox lightbox.c utilities.c

clean:
	rm -rf lightbox