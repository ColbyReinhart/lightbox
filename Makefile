lightbox: lightbox.h response.h lightbox.c utilities.c response.c
	cc -o lightbox lightbox.c utilities.c response.c

test: lightbox.h response.h lightbox.c utilities.c response.c
	cc -o lightbox -DTEST lightbox.c utilities.c response.c

clean:
	rm -rf lightbox