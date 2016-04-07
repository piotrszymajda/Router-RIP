# Autor: Piotr Szymajda - 273 023
CXX = g++
FLAGS = -std=c++11 -Wall -Wextra -g -pedantic -pedantic-errors # -O2
NAME = router
OBJS = main.o nets.o

all: $(OBJS) 
	$(CXX) $^ -o $(NAME)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(FLAGS) $< -o $@

clean:
	rm -f *.o
	
distclean:
	rm -f $(NAME)
	
realclean: clean distclean  
	
.PHONY: all clean distclean realclean
