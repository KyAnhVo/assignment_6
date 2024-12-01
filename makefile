F := objFile
OBJ := ${F}/server_tcp.o ${F}/server_udp.o ${F}/echo_s.o

echo_s : ${OBJ} | ${F}
	g++ -o echo_s ${OBJ} -lpthread

echo_c : echo_c.cpp
	g++ -o echo_c echo_c.cpp -lpthread

${F}/echo_s.o : echo_s.cpp | ${F}
	g++ -c echo_s.cpp -lpthread
	mv echo_s.o ${F}

${F}/%.o : %.cpp %.h | ${F}
	g++ -c $< -lpthread
	mv $(@F) ${F}

${F} :
	mkdir -p ${F}