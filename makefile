# Pick a platform below.  X11 for Linux/Mac/Unix, and WIN32 for windows.
# In either X11 or WIN32, Postscript is also simultaneously available.
# You can also pick NO_GRAPHICS, which will allow your code to compile without
# change on any platform, but no graphics will display (all calls are stubbed
# out).
# 
# Compiling to support X11 requires the X11 development libraries. On Ubuntu, use
# 'sudo apt-get install libx11-dev' to install.
# Please look online for information on how to install X11 on other Linux distributions.

# Please note that a Mac can run the X11 graphics if the X11 library is installed.

PLATFORM = X11
#PLATFORM = WIN32
#PLATFORM = NO_GRAPHICS

HDR = graphics.h easygl_constants.h branchbound.h net.h cell.h helper.h leaf.h
SRC = graphics.cpp a3.cpp branchbound.cpp net.cpp cell.cpp helper.cpp leaf.cpp
EXE = assignment3
BACKUP_FILENAME=`date "+backup-%Y%m%d-%H%M.zip"`
FLAGS =  -D$(PLATFORM) -Decf -std=c++11 
#FLAGS =  -D$(PLATFORM) -Decf -std=c++11 -Dearlyexit

# Need to tell the linker to link to the X11 libraries.
# WIN32 automatically links to the win32 API libraries (no need for flags)
ifeq ($(PLATFORM),X11)
   GRAPHICS_LIBS = -lX11
endif

$(EXE):graphics.o cell.o net.o leaf.o helper.o branchbound.o a3.o
	g++ $(FLAGS) graphics.o cell.o net.o leaf.o helper.o branchbound.o a3.o $(GRAPHICS_LIBS) -o $(EXE)
	
net.o: $(HDR)
	g++ -c $(FLAGS) net.cpp
	
leaf.o: $(HDR)
	g++ -c $(FLAGS) leaf.cpp
	
helper.o: $(HDR)
	g++ -c $(FLAGS) helper.cpp

branchbound.o: $(HDR)
	g++ -c $(FLAGS) branchbound.cpp

a3.o:  $(HDR)
	g++ -c $(FLAGS) a3.cpp

backup:
# Back up the source, makefile and Visual Studio project & solution. 
	echo Backing up your files into ${BACKUP_FILENAME}
	zip ${BACKUP_FILENAME} $(SRC) $(HDR) makefile easygl.sln easygl.vcxproj

clean:
	rm $(EXE) *.o
