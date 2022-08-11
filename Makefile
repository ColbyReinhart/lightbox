lightbox: *.h *.c
	cc -o lightbox lightbox.c utilities.c response.c

test: *.h *.c
	cc -o lightbox -DTEST lightbox.c utilities.c response.c

clean:
	rm -rf lightbox