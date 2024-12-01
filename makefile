F := objFile
OBJ := ${F}/server_tcp.o ${F}/server_udp.o ${F}/echo_s.o

echo_s : ${OBJ} | ${F}
	g++ -o echo_s ${OBJ} -lpthread

${F}/echo_s.o : echo_s.cpp | ${F}
	g++ -c echo_s.cpp -lpthread
	mv echo_s.o ${F}

${F}/%.o : %.cpp %.h | ${F}
	g++ -c $< -lpthread
	mv $(@F) ${F}

${F} :
	mkdir -p ${F}