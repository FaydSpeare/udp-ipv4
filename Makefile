CC = clang

# $@: target
# $^: all dependences
# $<: first dependence
# $?: all dependences newer than target

objects := byteorder.o ip.o udp.o
examples := send_udp.o listen_udp.o send_icmp_echo.o listen_icmp.o

all: ${objects} ${examples}
	${CC} -o send_udp send_udp.o ${objects}
	${CC} -o listen_udp listen_udp.o ${objects}
	${CC} -o send_icmp_echo send_icmp_echo.o ${objects}
	${CC} -o listen_icmp listen_icmp.o ${objects}

${objects}: %.o: %.c
	${CC} -c $< -o $@

${examples}: %.o: examples/%.c
	${CC} -c $< -o $@

clean:
	rm -f ${objects} ${examples}
	rm -f send_udp listen_udp send_icmp_echo listen_icmp





