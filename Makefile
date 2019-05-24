dijkstra:
	python generator.py toy_programs/dijkstra.schwa compiled/dijkstra.c
	gcc compiled/dijkstra.c -o dijkstra.out -lm
	./dijkstra.out
	rm dijkstra.out 
compare:
	python generator.py toy_programs/compare.schwa compiled/compare.c
	gcc compiled/compare.c -o compare.out -lm
	./compare.out
	rm compare.out 
switch_example:
	python generator.py toy_programs/switch_example.schwa compiled/switch_example.c
	gcc compiled/switch_example.c -o switch_example.out -lm
	./switch_example.out
	rm switch_example.out 
ref:
	python generator.py toy_programs/ref.schwa compiled/ref.c
	gcc compiled/ref.c -o ref.out -lm
	./ref.out
	rm ref.out 
