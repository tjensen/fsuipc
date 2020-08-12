PYTHONVER=37
FSUIPCDIR=fsuipc

all: pyuipc.pyd.$(PYTHONVER)

pyuipc.pyd.%: pyuipc.cc
	i686-w64-mingw32-gcc -v -IPython$*/include -I$(FSUIPCDIR) -LPython$*/libs "$^" -Wall -W -O2 -shared -static-libgcc -o "$@" $(FSUIPCDIR)/FSUIPC_User.lib -lpython$(subst -32,,$*) -no-pthread -fno-rtti -fno-exceptions -fno-non-call-exceptions
	i686-w64-mingw32-strip "$@"
