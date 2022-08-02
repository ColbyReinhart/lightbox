lightbox: lightbox.h lightbox.c utilities.c
	cc -o lightbox lightbox.c utilities.c

test: lightbox.h lightbox.c utilities.c
	cc -DTEST -o lightbox lightbox.c utilities.c

clean:
	rm -rf lightbox