XUTC_SUBDIR := $(CURDIR)/xutc

CFLAGS += -DREG=register -pthread
# -DHZ=${HZ}
# -DPRATTLE

INC=-I./xutc/src

c_src_files := src/main.c src/mhultistone.c
libdeps := $(XUTC_SUBDIR)/xutThread.o $(XUTC_SUBDIR)/xutThreadPool.o
#c_src_files := src/main.c src/mhultistone.c xutc/src/xutThread.c xutc/src/xutThreadPool.c
	#dhry_1.c dhry_2.c


mhultistone: $(c_src_files) libxutc
	cc $(c_src_files) $(libdeps) $(INC) $(CFLAGS) -o $@ -O3

pybuildenv:
	python3 -m venv pybuildenv
	pybuildenv/bin/pip3 install jinja2

clean-pybuildenv:
	rm -rf pybuildenv

src/mhultistone.c: pybuildenv templates/mhultistone.h.jinja templates/mhultistone.c.jinja mhultistone.generator.py
	pybuildenv/bin/python3 mhultistone.generator.py


clean:
	$(MAKE) -C $(XUTC_SUBDIR) clean
	rm -f mhultistone
	rm -f src/mhultistone.h
	rm -f src/mhultistone.c

libxutc:
	$(MAKE) -C $(XUTC_SUBDIR)
