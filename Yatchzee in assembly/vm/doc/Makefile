#configuration optios for doxygen
DOXYGEN = doxygen
DOXYGEN_ROOT = .
DOXYGEN_OUTPUT_DIR = $(DOXYGEN_ROOT)/html
DOXYGEN_CONFIG_FILE = Doxyfile
DOXYGEN_CONFIG = $(DOXYGEN_ROOT)/$(DOXYGEN_CONFIG_FILE)

# dot is an executable, that is part of the graphviz package. Graphiz produces 
# graphs, and it is used by the doxygen script. It must exist to get the full docs.
DOT = dot

#make documents --> Runs doxygen on the config file located at DOXYGEN_CONFIG. A default version is
#include in the repository.	
documents:
	@if ! test -f ${DOXYGEN_CONFIG}; \
		then echo "Doxygen config file ${DOXYGEN_CONFIG} not found!"; \
		exit 2; \
	fi
	@if test -z `which ${DOXYGEN}`; \
		then echo "${DOXYGEN} not found, install doxygen and ensure it is in your path!"; \
		exit 2; \
	fi
	@if test -z `which ${DOT}`; \
		then echo "${DOT} not found, install dot (part of graphviz package) and ensure it is in your path!"; \
		exit 2; \
	fi
	@if ! test -d ${DOXYGEN_OUTPUT_DIR}; \
		then mkdir ${DOXYGEN_OUTPUT_DIR}; \
	fi
	${DOXYGEN} ${DOXYGEN_CONFIG}

#removedocs --> Must be called explicitly to remove the documentation, clean will not call this
#target	
remove_documents:
	@if test -d ${DOXYGEN_OUTPUT_DIR}; \
		then rm -rf ${DOXYGEN_OUTPUT_DIR}; \
	fi
