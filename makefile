CPP = g++
CPP_FLAGS = -c -Wall -s
LINK = g++
VERSION = 0.0.1
PROJECT = MINESWEEPER++

a.out		:		Minesweeper_Project.o boardSolver.o gameBoard.o gameTile.o  boardOptions.o
					@echo "Linking $^..."
					@$(LINK) $^
				
Minesweeper_Project.o	:	Minesweeper_Project.cpp boardSolver.cpp gameBoard.cpp gameTile.cpp  boardOptions.cpp game_logic.h
							@echo "Compiling $<..."
							@$(CPP) $(CPP_FLAGS) $<
					
boardSolver.o :		boardSolver.cpp game_logic.h
					@echo "Compiling $<..."
					@$(CPP) $(CPP_FLAGS) $<
					
gameTile.o :		gameTile.cpp game_logic.h
					@echo "Compiling $<..."
					@$(CPP) $(CPP_FLAGS) $<
					
gameBoard.o :		gameBoard.cpp game_logic.h
					@echo "Compiling $<..."
					@$(CPP) $(CPP_FLAGS) $<

boardOptions.o :	boardOptions.cpp game_logic.h
					@echo "Compiling $<..."
					@$(CPP) $(CPP_FLAGS) $<

				
clean		:	
					rm -f a.out *.o
				
save		:	
					tar cvzf $(PROJECT)v$(VERSION).tar.gz
