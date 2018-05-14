.PHONY: default clean

default:
	cd Client && ${MAKE} default && mv client .. && cd ..
	cd Server && ${MAKE} default && mv server .. && cd ..


clean:
	cd Client && ${MAKE} clean && cd ..
	cd Server && ${MAKE} clean && cd ..
	rm client && rm server
