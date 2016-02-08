ee450project: patient1.o patient2.o doctor.o healthcenterserver.o
	gcc -o patient1 -g patient1.o -lnsl -lsocket -lresolv
	gcc -o patient2 -g patient2.o -lnsl -lsocket -lresolv
	gcc -o doctor -g doctor.o -lnsl -lsocket -lresolv
	gcc -o healthcenterserver -g healthcenterserver.o -lnsl -lsocket -lresolv

patient1.o: patient1.c patient1.h
	gcc -g -c -Wall patient1.c

patient2.o: patient2.c patient2.h
	gcc -g -c -Wall patient2.c

healthcenterserver.o: healthcenterserver.c healthcenterserver.h
	gcc -g -c -Wall healthcenterserver.c

doctor.o: doctor.c doctor.h
	gcc -g -c -Wall doctor.c

clean:
	rm -f *.o patient1 patient2 doctor healthcenterserver notavaindex.txt

