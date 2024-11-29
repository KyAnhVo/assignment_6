F := objFile
OBJ := ${F}/server_tcp.o ${F}/server_udp.o ${F}/echo_s.o

echo_s : ${OBJ} | ${F}
	g++ -o echo_s ${OBJ}

${F}/echo_s.o : echo_s.cpp | ${F}
	g++ -c echo_s.cpp
	mv echo_s.o ${F}

${F}/%.o : %.cpp %.h | ${F}
	g++ -c $<
	mv $(@F) ${F}

${F} :
	mkdir -p ${F}